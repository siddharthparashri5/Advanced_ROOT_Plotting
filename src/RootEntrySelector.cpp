#include "RootEntrySelector.h"

#include <TGLayout.h>
#include <TGMsgBox.h>
#include <TG3DLine.h>
#include <TGFileDialog.h>
#include <TSystem.h>
#include <TKey.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TCut.h>
#include <TTreeFormula.h>
#include <TEventList.h>

#include <fstream>
#include <sstream>
#include <iostream>

ClassImp(RootEntrySelector)

// ============================================================================
// Constructor
// ============================================================================
RootEntrySelector::RootEntrySelector(const TGWindow* p, const char* filename)
    : TGTransientFrame(p, nullptr, 1000, 700),
      fFile(nullptr),
      fFilename(filename),
      fCurrentTree(nullptr),
      fCurrentHist(nullptr)
{
    SetWindowName("ROOT Entry Selector - Advanced Filtering");
    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    
    // Open file
    fFile = TFile::Open(filename);
    if (!fFile || fFile->IsZombie()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", Form("Cannot open ROOT file:\n%s", filename),
            kMBIconStop, kMBOk);
        return;
    }
    
    BuildGUI();
    ScanFile();
    
    MapSubwindows();
    Resize(GetDefaultSize());
    CenterOnParent();
    MapWindow();
}

// ============================================================================
// Destructor
// ============================================================================
RootEntrySelector::~RootEntrySelector()
{
    if (fFile) {
        fFile->Close();
        delete fFile;
    }
}

// ============================================================================
// Build GUI
// ============================================================================
void RootEntrySelector::BuildGUI()
{
    TGVerticalFrame* mainFrame = new TGVerticalFrame(this);
    AddFrame(mainFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10));
    
    // ═══════════════════════════════════════════════════
    // HEADER - File info
    // ═══════════════════════════════════════════════════
    TGGroupFrame* headerFrame = new TGGroupFrame(mainFrame, "File Information");
    std::string fileInfo = "File: " + std::string(fFilename.Data());
    TGLabel* fileLabel = new TGLabel(headerFrame, fileInfo.c_str());
    headerFrame->AddFrame(fileLabel, new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5));
    mainFrame->AddFrame(headerFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ═══════════════════════════════════════════════════
    // SECTION 1: Object Selection
    // ═══════════════════════════════════════════════════
    TGGroupFrame* objFrame = new TGGroupFrame(mainFrame, "Step 1: Select Object");
    
    // Object selector
    TGHorizontalFrame* objSelFrame = new TGHorizontalFrame(objFrame);
    objSelFrame->AddFrame(new TGLabel(objSelFrame, "Object:"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    fObjectCombo = new TGComboBox(objSelFrame, kObjectCombo);
    fObjectCombo->Resize(300, 20);
    fObjectCombo->Associate(this);
    objSelFrame->AddFrame(fObjectCombo, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    objFrame->AddFrame(objSelFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // Branch selector (for TTrees)
    TGHorizontalFrame* branchFrame = new TGHorizontalFrame(objFrame);
    branchFrame->AddFrame(new TGLabel(branchFrame, "Branch (for TTree):"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    fBranchCombo = new TGComboBox(branchFrame, kBranchCombo);
    fBranchCombo->Resize(200, 20);
    fBranchCombo->Associate(this);
    fBranchCombo->SetEnabled(kFALSE);
    branchFrame->AddFrame(fBranchCombo, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    objFrame->AddFrame(branchFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // Object info
    fObjectInfoLabel = new TGLabel(objFrame, "Select an object to see details");
    objFrame->AddFrame(fObjectInfoLabel, new TGLayoutHints(kLHintsLeft, 10, 5, 5, 5));
    
    fEntriesLabel = new TGLabel(objFrame, "Entries: -");
    objFrame->AddFrame(fEntriesLabel, new TGLayoutHints(kLHintsLeft, 10, 5, 2, 5));
    
    mainFrame->AddFrame(objFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ═══════════════════════════════════════════════════
    // SECTION 2: Entry Range & Cuts
    // ═══════════════════════════════════════════════════
    TGGroupFrame* rangeFrame = new TGGroupFrame(mainFrame, "Step 2: Configure Selection");
    
    // Entry range
    TGHorizontalFrame* entryRangeFrame = new TGHorizontalFrame(rangeFrame);
    entryRangeFrame->AddFrame(new TGLabel(entryRangeFrame, "Entry Range:"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 10, 5, 5));
    
    entryRangeFrame->AddFrame(new TGLabel(entryRangeFrame, "Start:"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    fStartEntry = new TGNumberEntry(entryRangeFrame, 0, 10, kStartEntry,
        TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber);
    fStartEntry->SetLimits(TGNumberFormat::kNELLimitMin, 0);
    entryRangeFrame->AddFrame(fStartEntry, new TGLayoutHints(kLHintsLeft, 5, 10, 5, 5));
    
    entryRangeFrame->AddFrame(new TGLabel(entryRangeFrame, "End:"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    fEndEntry = new TGNumberEntry(entryRangeFrame, -1, 10, kEndEntry,
        TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber);
    fEndEntry->SetLimits(TGNumberFormat::kNELLimitMin, -1);
    entryRangeFrame->AddFrame(fEndEntry, new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5));
    
    TGLabel* endHint = new TGLabel(entryRangeFrame, "(-1 = all)");
    endHint->SetTextColor(0x888888);
    entryRangeFrame->AddFrame(endHint, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    rangeFrame->AddFrame(entryRangeFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // Cut formula
    TGHorizontalFrame* cutFrame = new TGHorizontalFrame(rangeFrame);
    cutFrame->AddFrame(new TGLabel(cutFrame, "Selection Cut:"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    fCutEntry = new TGTextEntry(cutFrame, "", kCutEntry);
    fCutEntry->SetToolTipText("Example: Energy > 100 && abs(eta) < 2.5");
    cutFrame->AddFrame(fCutEntry, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    rangeFrame->AddFrame(cutFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // Draw options
    TGHorizontalFrame* drawFrame = new TGHorizontalFrame(rangeFrame);
    drawFrame->AddFrame(new TGLabel(drawFrame, "Draw Options:"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    fDrawOptEntry = new TGTextEntry(drawFrame, "", kDrawOptEntry);
    fDrawOptEntry->SetText("COLZ");
    fDrawOptEntry->SetToolTipText("ROOT draw options: COLZ, PE, HIST, etc.");
    drawFrame->AddFrame(fDrawOptEntry, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    rangeFrame->AddFrame(drawFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    mainFrame->AddFrame(rangeFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ═══════════════════════════════════════════════════
    // SECTION 3: Selection Chain Management
    // ═══════════════════════════════════════════════════
    TGGroupFrame* chainFrame = new TGGroupFrame(mainFrame, "Step 3: Build Selection Chain");
    
    // Action buttons
    TGHorizontalFrame* actionFrame = new TGHorizontalFrame(chainFrame);
    
    fAddStepButton = new TGTextButton(actionFrame, "Add to Chain →", kAddStepButton);
    fAddStepButton->Associate(this);
    fAddStepButton->SetToolTipText("Add current selection as a new step");
    actionFrame->AddFrame(fAddStepButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fRemoveStepButton = new TGTextButton(actionFrame, "Remove Selected", kRemoveStepButton);
    fRemoveStepButton->Associate(this);
    actionFrame->AddFrame(fRemoveStepButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fClearStepsButton = new TGTextButton(actionFrame, "Clear All", kClearStepsButton);
    fClearStepsButton->Associate(this);
    actionFrame->AddFrame(fClearStepsButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    chainFrame->AddFrame(actionFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // Selection chain list
    TGLabel* chainLabel = new TGLabel(chainFrame, "Selection Chain (sequential filtering):");
    chainFrame->AddFrame(chainLabel, new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    fStepListBox = new TGListBox(chainFrame);
    fStepListBox->Resize(100, 100);
    chainFrame->AddFrame(fStepListBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 2, 5));
    
    mainFrame->AddFrame(chainFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // ═══════════════════════════════════════════════════
    // SECTION 4: Plot Controls
    // ═══════════════════════════════════════════════════
    TGGroupFrame* plotFrame = new TGGroupFrame(mainFrame, "Step 4: Plot");
    
    TGHorizontalFrame* plotBtnFrame = new TGHorizontalFrame(plotFrame);
    
    fPlotCurrentButton = new TGTextButton(plotBtnFrame, "Plot Current Only", kPlotCurrentButton);
    fPlotCurrentButton->Associate(this);
    fPlotCurrentButton->SetToolTipText("Plot only the current selection (ignore chain)");
    plotBtnFrame->AddFrame(fPlotCurrentButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fPlotChainButton = new TGTextButton(plotBtnFrame, "Plot with Chain Cuts", kPlotChainButton);
    fPlotChainButton->Associate(this);
    fPlotChainButton->SetToolTipText("Apply all chain cuts cumulatively");
    plotBtnFrame->AddFrame(fPlotChainButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    plotFrame->AddFrame(plotBtnFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // Save/Load chain
    TGHorizontalFrame* ioFrame = new TGHorizontalFrame(plotFrame);
    
    fSaveChainButton = new TGTextButton(ioFrame, "Save Chain...", kSaveChainButton);
    fSaveChainButton->Associate(this);
    ioFrame->AddFrame(fSaveChainButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fLoadChainButton = new TGTextButton(ioFrame, "Load Chain...", kLoadChainButton);
    fLoadChainButton->Associate(this);
    ioFrame->AddFrame(fLoadChainButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    plotFrame->AddFrame(ioFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    mainFrame->AddFrame(plotFrame, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ═══════════════════════════════════════════════════
    // BOTTOM: Close button
    // ═══════════════════════════════════════════════════
    mainFrame->AddFrame(new TGHorizontal3DLine(mainFrame),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 5));
    
    fCloseButton = new TGTextButton(mainFrame, "Close", kCloseButton);
    fCloseButton->Associate(this);
    mainFrame->AddFrame(fCloseButton, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 10));
}

// ============================================================================
// Scan file for objects
// ============================================================================
void RootEntrySelector::ScanFile()
{
    if (!fFile) return;
    
    fObjectList.clear();
    fObjectCombo->RemoveAll();
    
    TIter next(fFile->GetListOfKeys());
    TKey* key;
    int id = 0;
    
    while ((key = (TKey*)next())) {
        TString className = key->GetClassName();
        TString objName = key->GetName();
        
        // Accept histograms and trees
        if (className.BeginsWith("TH") || className == "TTree") {
            char entry[512];
            snprintf(entry, sizeof(entry), "%s [%s]", objName.Data(), className.Data());
            fObjectList.push_back(objName.Data());
            fObjectCombo->AddEntry(entry, id++);
        }
    }
    
    if (id > 0) {
        fObjectCombo->Select(0);
        UpdateObjectInfo();
    }
}

// ============================================================================
// Populate branches for TTree
// ============================================================================
void RootEntrySelector::PopulateBranches()
{
    fBranchCombo->RemoveAll();
    fBranchCombo->SetEnabled(kFALSE);
    
    if (!fCurrentTree) return;
    
    TObjArray* branches = fCurrentTree->GetListOfBranches();
    if (!branches) return;
    
    int id = 0;
    for (int i = 0; i < branches->GetEntries(); ++i) {
        TBranch* br = (TBranch*)branches->At(i);
        if (br) {
            fBranchCombo->AddEntry(br->GetName(), id++);
        }
    }
    
    if (id > 0) {
        fBranchCombo->Select(0);
        fBranchCombo->SetEnabled(kTRUE);
    }
}

// ============================================================================
// Update object information display
// ============================================================================
void RootEntrySelector::UpdateObjectInfo()
{
    Int_t selected = fObjectCombo->GetSelected();
    if (selected < 0 || selected >= (Int_t)fObjectList.size()) return;
    
    std::string objName = fObjectList[selected];
    TObject* obj = fFile->Get(objName.c_str());
    
    if (!obj) {
        fObjectInfoLabel->SetText("ERROR: Could not retrieve object");
        return;
    }
    
    fCurrentTree = nullptr;
    fCurrentHist = nullptr;
    
    // Check object type
    if (obj->InheritsFrom(TTree::Class())) {
        fCurrentTree = (TTree*)obj;
        
        char info[512];
        snprintf(info, sizeof(info), "Type: TTree | Title: %s", obj->GetTitle());
        fObjectInfoLabel->SetText(info);
        
        char entries[128];
        snprintf(entries, sizeof(entries), "Entries: %lld", fCurrentTree->GetEntries());
        fEntriesLabel->SetText(entries);
        
        PopulateBranches();
        
        // Set default end entry
        fEndEntry->SetNumber(fCurrentTree->GetEntries() - 1);
        
    } else if (obj->InheritsFrom(TH1::Class())) {
        fCurrentHist = (TH1*)obj;
        
        char info[512];
        snprintf(info, sizeof(info), "Type: %s | Title: %s", 
                obj->ClassName(), obj->GetTitle());
        fObjectInfoLabel->SetText(info);
        
        char entries[128];
        snprintf(entries, sizeof(entries), "Entries: %.0f | Bins: %d", 
                fCurrentHist->GetEntries(), fCurrentHist->GetNbinsX());
        fEntriesLabel->SetText(entries);
        
        fBranchCombo->RemoveAll();
        fBranchCombo->SetEnabled(kFALSE);
        
        // Set default end entry to number of bins
        fEndEntry->SetNumber(fCurrentHist->GetNbinsX());
    }
    
    gClient->NeedRedraw(fObjectInfoLabel);
    gClient->NeedRedraw(fEntriesLabel);
}

// ============================================================================
// Add current selection to chain
// ============================================================================
void RootEntrySelector::AddSelectionStep()
{
    Int_t selected = fObjectCombo->GetSelected();
    if (selected < 0 || selected >= (Int_t)fObjectList.size()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "No Object", "Please select an object first",
            kMBIconExclamation, kMBOk);
        return;
    }
    
    SelectionStep step;
    step.objectName = fObjectList[selected];
    
    // Safely get object type without storing pointer
    TObject* obj = fFile->Get(step.objectName.c_str());
    if (!obj) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", "Cannot access object from file",
            kMBIconStop, kMBOk);
        return;
    }
    
    step.objectType = std::string(obj->ClassName());
    
    // Get numeric values
    step.entryStart = (Long64_t)fStartEntry->GetNumber();
    step.entryEnd = (Long64_t)fEndEntry->GetNumber();
    
    // Safely get text entries with null checks
    if (fCutEntry) {
        const char* cutText = fCutEntry->GetText();
        step.cutFormula = (cutText && strlen(cutText) > 0) ? std::string(cutText) : "";
    }
    
    if (fDrawOptEntry) {
        const char* drawText = fDrawOptEntry->GetText();
        step.drawOptions = (drawText && strlen(drawText) > 0) ? std::string(drawText) : "";
    }
    
    // For TTree, include branch if selected
    if (step.objectType.find("TTree") != std::string::npos) {
        if (fBranchCombo && fBranchCombo->IsEnabled()) {
            TGTextEntry* te = fBranchCombo->GetTextEntry();
            if (te) {
                const char* branchText = te->GetText();
                if (branchText && strlen(branchText) > 0) {
                    step.objectName += ":";
                    step.objectName += std::string(branchText);
                }
            }
        }
    }
    
    // Add to chain
    fSelectionChain.push_back(step);
    
    // Update list box - use safer string construction
    Int_t id = (Int_t)fSelectionChain.size() - 1;
    std::string listEntry = std::to_string(id + 1) + ". " + step.GetDescription();
    fStepListBox->AddEntry(listEntry.c_str(), id);
    fStepListBox->Layout();
    
    std::cout << "Added selection step " << (id + 1) << ": " 
              << step.GetDescription() << std::endl;
}

// ============================================================================
// Remove selected step from chain
// ============================================================================
void RootEntrySelector::RemoveSelectedStep()
{
    Int_t selected = fStepListBox->GetSelected();
    if (selected < 0) return;
    
    if (selected < (Int_t)fSelectionChain.size()) {
        fSelectionChain.erase(fSelectionChain.begin() + selected);
        
        // Rebuild list box
        fStepListBox->RemoveAll();
        for (size_t i = 0; i < fSelectionChain.size(); ++i) {
            std::string entry = std::to_string(i + 1) + ". " + 
                               fSelectionChain[i].GetDescription();
            fStepListBox->AddEntry(entry.c_str(), (Int_t)i);
        }
        fStepListBox->Layout();
    }
}

// ============================================================================
// Clear all steps
// ============================================================================
void RootEntrySelector::ClearAllSteps()
{
    fSelectionChain.clear();
    fStepListBox->RemoveAll();
    fStepListBox->Layout();
}

// ============================================================================
// Plot current selection only
// ============================================================================
void RootEntrySelector::PlotCurrentSelection()
{
    std::cout << "=== PlotCurrentSelection() START ===" << std::endl;
    
    // Safety check 1: Combo box
    if (!fObjectCombo) {
        std::cout << "ERROR: fObjectCombo is NULL!" << std::endl;
        return;
    }
    
    Int_t selected = fObjectCombo->GetSelected();
    std::cout << "Selected index: " << selected << std::endl;
    std::cout << "Object list size: " << fObjectList.size() << std::endl;
    
    if (selected < 0 || selected >= (Int_t)fObjectList.size()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "No Object", "Please select an object first",
            kMBIconExclamation, kMBOk);
        return;
    }
    
    // Safety check 2: File
    if (!fFile) {
        std::cout << "ERROR: fFile is NULL!" << std::endl;
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", "File not available",
            kMBIconStop, kMBOk);
        return;
    }
    
    // Get object name safely
    std::string objName = fObjectList[selected];
    std::cout << "Getting object: " << objName << std::endl;
    
    // Get fresh object from file
    TObject* obj = fFile->Get(objName.c_str());
    if (!obj) {
        std::cout << "ERROR: Cannot retrieve object from file!" << std::endl;
        char errMsg[256];
        snprintf(errMsg, sizeof(errMsg), "Cannot retrieve: %s", objName.c_str());
        new TGMsgBox(gClient->GetRoot(), this, "Error", errMsg,
            kMBIconStop, kMBOk);
        return;
    }
    
    std::cout << "Object retrieved: " << obj->GetName() 
              << " (class: " << obj->ClassName() << ")" << std::endl;
    
    // Build selection step
    SelectionStep step;
    step.objectName = objName;
    step.objectType = std::string(obj->ClassName());
    
    std::cout << "Getting entry values..." << std::endl;
    step.entryStart = (Long64_t)fStartEntry->GetNumber();
    step.entryEnd = (Long64_t)fEndEntry->GetNumber();
    std::cout << "Entry range: " << step.entryStart << " to " << step.entryEnd << std::endl;
    
    // Get text safely
    std::cout << "Getting cut text..." << std::endl;
    if (fCutEntry) {
        const char* cutText = fCutEntry->GetText();
        step.cutFormula = (cutText && strlen(cutText) > 0) ? std::string(cutText) : "";
    }
    std::cout << "Cut: " << step.cutFormula << std::endl;
    
    std::cout << "Getting draw options..." << std::endl;
    if (fDrawOptEntry) {
        const char* drawText = fDrawOptEntry->GetText();
        step.drawOptions = (drawText && strlen(drawText) > 0) ? std::string(drawText) : "";
    }
    std::cout << "Draw options: " << step.drawOptions << std::endl;
    
    // For TTree, include branch
    if (obj->InheritsFrom(TTree::Class())) {
        std::cout << "Object is TTree, checking for branch..." << std::endl;
        if (fBranchCombo && fBranchCombo->IsEnabled()) {
            TGTextEntry* te = fBranchCombo->GetTextEntry();
            if (te) {
                const char* branchText = te->GetText();
                if (branchText && strlen(branchText) > 0) {
                    step.objectName += ":";
                    step.objectName += std::string(branchText);
                    std::cout << "Branch added: " << branchText << std::endl;
                }
            }
        }
    }
    
    std::cout << "Final step objectName: " << step.objectName << std::endl;
    
    // Plot based on type
    std::cout << "Determining plot type..." << std::endl;
    if (obj->InheritsFrom(TH1::Class())) {
        std::cout << "Calling PlotHistogram..." << std::endl;
        PlotHistogram(step);
        std::cout << "PlotHistogram complete" << std::endl;
    } else if (obj->InheritsFrom(TTree::Class())) {
        std::cout << "Calling PlotTree..." << std::endl;
        PlotTree(step);
        std::cout << "PlotTree complete" << std::endl;
    } else {
        std::cout << "Unsupported object type: " << obj->ClassName() << std::endl;
        char errMsg[256];
        snprintf(errMsg, sizeof(errMsg), 
                 "Object type %s not yet supported for plotting", 
                 obj->ClassName());
        new TGMsgBox(gClient->GetRoot(), this, "Unsupported", errMsg,
            kMBIconExclamation, kMBOk);
    }
    
    std::cout << "=== PlotCurrentSelection() END ===" << std::endl;
}

// ============================================================================
// Plot entire chain with cumulative cuts
// ============================================================================
void RootEntrySelector::PlotEntireChain()
{
    if (fSelectionChain.empty()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Empty Chain", "No selection steps in chain",
            kMBIconExclamation, kMBOk);
        return;
    }
    
    PlotWithChain(fSelectionChain);
}

// ============================================================================
// Build cumulative cut string from all steps
// ============================================================================
std::string RootEntrySelector::BuildCumulativeCut() const
{
    std::string cumCut;
    
    for (const auto& step : fSelectionChain) {
        if (!step.cutFormula.empty()) {
            if (!cumCut.empty()) {
                cumCut += " && ";
            }
            cumCut += "(" + step.cutFormula + ")";
        }
    }
    
    return cumCut;
}

// ============================================================================
// Plot histogram with selection
// ============================================================================
TCanvas* RootEntrySelector::PlotHistogram(const SelectionStep& step)
{
    std::cout << "=== PlotHistogram() START ===" << std::endl;
    std::cout << "Step objectName: " << step.objectName << std::endl;
    
    TH1* hist = (TH1*)fFile->Get(step.objectName.c_str());
    if (!hist) {
        std::cout << "ERROR: Cannot retrieve histogram!" << std::endl;
        char errMsg[256];
        snprintf(errMsg, sizeof(errMsg), "Cannot retrieve: %s", step.objectName.c_str());
        new TGMsgBox(gClient->GetRoot(), this, "Error", errMsg,
            kMBIconStop, kMBOk);
        return nullptr;
    }
    
    std::cout << "Histogram retrieved: " << hist->GetName() 
              << " (" << hist->GetEntries() << " entries, " 
              << hist->GetNbinsX() << " bins)" << std::endl;
    
    static int canvasCount = 0;
    canvasCount++;
    
    char canvasName[128];
    snprintf(canvasName, sizeof(canvasName), "c_hist_%d", canvasCount);
    
    std::string canvasTitle = "Histogram: " + step.objectName;
    
    TCanvas* c = new TCanvas(canvasName, canvasTitle.c_str(), 800, 600);
    
    // Clone histogram to avoid modifying original
    char cloneName[256];
    snprintf(cloneName, sizeof(cloneName), "%s_clone_%d", hist->GetName(), canvasCount);
    TH1* hClone = (TH1*)hist->Clone(cloneName);
    
    // Apply entry range as bin range if specified
    if (step.entryStart > 0 || (step.entryEnd > 0 && step.entryEnd < hClone->GetNbinsX())) {
        Long64_t startBin = step.entryStart > 0 ? step.entryStart : 1;
        Long64_t endBin = step.entryEnd > 0 ? step.entryEnd : hClone->GetNbinsX();
        
        std::cout << "Applying bin range: " << startBin << " to " << endBin << std::endl;
        std::cout << "Total bins: " << hClone->GetNbinsX() << std::endl;
        
        // Zero out bins outside range
        for (int i = 1; i < startBin && i <= hClone->GetNbinsX(); ++i) {
            hClone->SetBinContent(i, 0);
            hClone->SetBinError(i, 0);
        }
        for (int i = endBin + 1; i <= hClone->GetNbinsX(); ++i) {
            hClone->SetBinContent(i, 0);
            hClone->SetBinError(i, 0);
        }
        
        std::cout << "Zeroed bins outside range [" << startBin << ", " << endBin << "]" << std::endl;
    }
    
    // Draw with specified options (default is empty for histograms)
    std::string drawOpt = step.drawOptions.empty() ? "" : step.drawOptions;
    hClone->Draw(drawOpt.c_str());
    
    c->Update();
    
    std::cout << "Plotted histogram: " << step.objectName << std::endl;
    std::cout << "  Entries: " << hClone->GetEntries() << std::endl;
    std::cout << "  Integral: " << hClone->Integral() << std::endl;
    
    return c;
}

// ============================================================================
// Plot TTree with selection
// ============================================================================
TCanvas* RootEntrySelector::PlotTree(const SelectionStep& step)
{
    std::cout << "=== PlotTree() START ===" << std::endl;
    std::cout << "Step objectName: " << step.objectName << std::endl;
    
    // Parse tree name and branch name
    std::string treeName = step.objectName;
    std::string branchName;
    
    size_t colonPos = step.objectName.find(':');
    if (colonPos != std::string::npos) {
        treeName = step.objectName.substr(0, colonPos);
        branchName = step.objectName.substr(colonPos + 1);
        std::cout << "Parsed - Tree: " << treeName << ", Branch: " << branchName << std::endl;
    }
    
    std::cout << "Getting tree from file..." << std::endl;
    TTree* tree = (TTree*)fFile->Get(treeName.c_str());
    if (!tree) {
        std::cout << "ERROR: Cannot retrieve tree!" << std::endl;
        char errMsg[256];
        snprintf(errMsg, sizeof(errMsg), "Cannot retrieve tree: %s", treeName.c_str());
        new TGMsgBox(gClient->GetRoot(), this, "Error", errMsg,
            kMBIconStop, kMBOk);
        return nullptr;
    }
    
    std::cout << "Tree retrieved: " << tree->GetName() 
              << " (" << tree->GetEntries() << " entries)" << std::endl;
    
    static int canvasCount = 0;
    canvasCount++;
    
    char canvasName[128];
    snprintf(canvasName, sizeof(canvasName), "c_tree_%d", canvasCount);
    
    std::string canvasTitle = "Tree: " + step.objectName;
    
    TCanvas* c = new TCanvas(canvasName, canvasTitle.c_str(), 800, 600);
    
    // Build draw command - use branch name if specified, otherwise full object name
    std::string drawCmd;
    if (!branchName.empty()) {
        drawCmd = branchName;
    } else {
        // No branch specified - try to draw first numeric branch
        TObjArray* branches = tree->GetListOfBranches();
        if (branches && branches->GetEntries() > 0) {
            drawCmd = branches->At(0)->GetName();
        } else {
            new TGMsgBox(gClient->GetRoot(), this,
                "Error", "No branches found in tree",
                kMBIconStop, kMBOk);
            delete c;
            return nullptr;
        }
    }
    
    // Build cut string with entry range
    std::string cutStr = step.cutFormula;
    
    // Add entry range to cut if specified
    if (step.entryEnd > 0) {
        char rangeFormula[256];
        snprintf(rangeFormula, sizeof(rangeFormula), 
                 "(Entry$ >= %lld && Entry$ <= %lld)", 
                 step.entryStart, step.entryEnd);
        if (!cutStr.empty()) {
            cutStr = "(" + cutStr + ") && " + std::string(rangeFormula);
        } else {
            cutStr = std::string(rangeFormula);
        }
    } else if (step.entryStart > 0) {
        char rangeFormula[256];
        snprintf(rangeFormula, sizeof(rangeFormula), 
                 "Entry$ >= %lld", step.entryStart);
        if (!cutStr.empty()) {
            cutStr = "(" + cutStr + ") && " + std::string(rangeFormula);
        } else {
            cutStr = std::string(rangeFormula);
        }
    }
    
    std::cout << "Drawing: " << drawCmd << std::endl;
    std::cout << "Cut: " << (cutStr.empty() ? "(none)" : cutStr) << std::endl;
    std::cout << "Options: " << step.drawOptions << std::endl;
    
    // Draw with proper options
    Long64_t nDrawn = tree->Draw(drawCmd.c_str(), 
                                 cutStr.c_str(), 
                                 step.drawOptions.c_str());
    
    std::cout << "Drew " << nDrawn << " entries" << std::endl;
    
    if (nDrawn == 0) {
        std::cout << "WARNING: No entries passed the cut!" << std::endl;
        std::cout << "  Total entries in tree: " << tree->GetEntries() << std::endl;
        std::cout << "  Check your cut formula and entry range" << std::endl;
    }
    
    c->Update();
    
    return c;
}

// ============================================================================
// Plot with entire chain
// ============================================================================
TCanvas* RootEntrySelector::PlotWithChain(const std::vector<SelectionStep>& chain)
{
    if (chain.empty()) return nullptr;
    
    static int canvasCount = 0;
    canvasCount++;
    
    char canvasName[128];
    snprintf(canvasName, sizeof(canvasName), "c_chain_%d", canvasCount);
    
    TCanvas* c = new TCanvas(canvasName, "Chained Selection", 1000, 700);
    
    // Apply all steps sequentially
    std::string cumulativeCut = BuildCumulativeCut();
    
    std::cout << "\n=== Plotting Selection Chain ===" << std::endl;
    std::cout << "Cumulative cut: " << cumulativeCut << std::endl;
    
    for (size_t i = 0; i < chain.size(); ++i) {
        const SelectionStep& step = chain[i];
        
        std::cout << "Step " << (i+1) << ": " << step.GetDescription() << std::endl;
        
        // For last step, actually plot
        if (i == chain.size() - 1) {
            // Parse tree/branch
            std::string objName = step.objectName;
            std::string branchName;
            
            size_t colonPos = objName.find(':');
            if (colonPos != std::string::npos) {
                objName = step.objectName.substr(0, colonPos);
                branchName = step.objectName.substr(colonPos + 1);
            }
            
            TObject* obj = fFile->Get(objName.c_str());
            if (!obj) continue;
            
            if (obj->InheritsFrom(TTree::Class())) {
                TTree* tree = (TTree*)obj;
                std::string drawCmd = branchName.empty() ? objName : branchName;
                
                tree->Draw(drawCmd.c_str(), cumulativeCut.c_str(), 
                          step.drawOptions.c_str());
            } else if (obj->InheritsFrom(TH1::Class())) {
                TH1* hist = (TH1*)obj;
                hist->Draw(step.drawOptions.c_str());
            }
        }
    }
    
    c->Update();
    std::cout << "=== Chain Plot Complete ===" << std::endl;
    
    return c;
}

// ============================================================================
// Save selection chain to file
// ============================================================================
void RootEntrySelector::SaveChainToFile()
{
    const char* filetypes[] = {
        "Selection chain", "*.selchain",
        "Text files", "*.txt",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    
    new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fileInfo);
    
    if (!fileInfo.fFilename) return;
    
    std::ofstream file(fileInfo.fFilename);
    if (!file.is_open()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", Form("Cannot write to: %s", fileInfo.fFilename),
            kMBIconStop, kMBOk);
        return;
    }
    
    file << "# ROOT Selection Chain\n";
    file << "# File: " << fFilename.Data() << "\n\n";
    
    for (size_t i = 0; i < fSelectionChain.size(); ++i) {
        const SelectionStep& step = fSelectionChain[i];
        file << "[Step " << (i+1) << "]\n";
        file << "Object=" << step.objectName << "\n";
        file << "Type=" << step.objectType << "\n";
        file << "EntryStart=" << step.entryStart << "\n";
        file << "EntryEnd=" << step.entryEnd << "\n";
        file << "Cut=" << step.cutFormula << "\n";
        file << "DrawOptions=" << step.drawOptions << "\n\n";
    }
    
    file.close();
    
    new TGMsgBox(gClient->GetRoot(), this,
        "Success", Form("Chain saved to:\n%s", fileInfo.fFilename),
        kMBIconAsterisk, kMBOk);
}

// ============================================================================
// Load selection chain from file
// ============================================================================
void RootEntrySelector::LoadChainFromFile()
{
    const char* filetypes[] = {
        "Selection chain", "*.selchain",
        "Text files", "*.txt",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    
    new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fileInfo);
    
    if (!fileInfo.fFilename) return;
    
    std::ifstream file(fileInfo.fFilename);
    if (!file.is_open()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", Form("Cannot open: %s", fileInfo.fFilename),
            kMBIconStop, kMBOk);
        return;
    }
    
    ClearAllSteps();
    
    std::string line;
    SelectionStep currentStep;
    bool inStep = false;
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("[Step") == 0) {
            if (inStep) {
                fSelectionChain.push_back(currentStep);
            }
            currentStep = SelectionStep();
            inStep = true;
        } else if (inStep) {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);
                
                if (key == "Object") currentStep.objectName = value;
                else if (key == "Type") currentStep.objectType = value;
                else if (key == "EntryStart") currentStep.entryStart = std::stoll(value);
                else if (key == "EntryEnd") currentStep.entryEnd = std::stoll(value);
                else if (key == "Cut") currentStep.cutFormula = value;
                else if (key == "DrawOptions") currentStep.drawOptions = value;
            }
        }
    }
    
    if (inStep) {
        fSelectionChain.push_back(currentStep);
    }
    
    file.close();
    
    // Rebuild list box
    for (size_t i = 0; i < fSelectionChain.size(); ++i) {
        std::string entry = std::to_string(i + 1) + ". " + 
                           fSelectionChain[i].GetDescription();
        fStepListBox->AddEntry(entry.c_str(), (Int_t)i);
    }
    fStepListBox->Layout();
    
    char successMsg[256];
    snprintf(successMsg, sizeof(successMsg), 
             "Loaded %zu steps from chain file", fSelectionChain.size());
    new TGMsgBox(gClient->GetRoot(), this,
        "Success", successMsg,
        kMBIconAsterisk, kMBOk);
}

// ============================================================================
// Process messages
// ============================================================================
Bool_t RootEntrySelector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    switch(GET_MSG(msg)) {
        case kC_COMMAND:
            switch(GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == kAddStepButton) {
                        AddSelectionStep();
                    } else if (parm1 == kRemoveStepButton) {
                        RemoveSelectedStep();
                    } else if (parm1 == kClearStepsButton) {
                        ClearAllSteps();
                    } else if (parm1 == kPlotCurrentButton) {
                        PlotCurrentSelection();
                    } else if (parm1 == kPlotChainButton) {
                        PlotEntireChain();
                    } else if (parm1 == kSaveChainButton) {
                        SaveChainToFile();
                    } else if (parm1 == kLoadChainButton) {
                        LoadChainFromFile();
                    } else if (parm1 == kCloseButton) {
                        CloseWindow();
                    }
                    break;
                    
                case kCM_COMBOBOX:
                    if (parm1 == kObjectCombo) {
                        UpdateObjectInfo();
                    }
                    break;
            }
            break;
    }
    
    return TGTransientFrame::ProcessMessage(msg, parm1, parm2);
}

// ============================================================================
// Close window
// ============================================================================
void RootEntrySelector::CloseWindow()
{
    DeleteWindow();
}