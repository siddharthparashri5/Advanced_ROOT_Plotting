#include "ROOTFileBrowser.h"

#include <TGLayout.h>
#include <TGSplitter.h>
#include <TG3DLine.h>
#include <TGMsgBox.h>
#include <TSystem.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <iostream>

//ClassImp(ROOTFileBrowser)

// ============================================================================
// Constructor
// ============================================================================
ROOTFileBrowser::ROOTFileBrowser(const TGWindow* p, const char* filename)
    : TGTransientFrame(p, nullptr, 800, 600),
      fFile(nullptr),
      fFilename(filename),
      fModalResult(-1)
{
    SetWindowName("ROOT File Browser");
    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    
    // Try to open the file
    fFile = TFile::Open(filename);
    if (!fFile || fFile->IsZombie()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", Form("Cannot open ROOT file:\n%s", filename),
            kMBIconStop, kMBOk);
        fModalResult = 0;
        return;
    }
    
    BuildGUI();
    ScanFile();
}

// ============================================================================
// Destructor
// ============================================================================
ROOTFileBrowser::~ROOTFileBrowser()
{
    if (fFile) {
        fFile->Close();
        delete fFile;
    }
}

// ============================================================================
// Build the GUI
// ============================================================================
void ROOTFileBrowser::BuildGUI()
{
    // Main vertical frame
    TGVerticalFrame* mainFrame = new TGVerticalFrame(this);
    AddFrame(mainFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // ─── FILE INFO ────────────────────────────────────
    TGHorizontalFrame* infoFrame = new TGHorizontalFrame(mainFrame);
    infoFrame->AddFrame(new TGLabel(infoFrame, "File:"), 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    fFilenameLabel = new TGLabel(infoFrame, fFilename.c_str());
    infoFrame->AddFrame(fFilenameLabel, 
        new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 5, 5, 5, 5));
    
    mainFrame->AddFrame(infoFrame, 
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ─── SEPARATOR ────────────────────────────────────
    mainFrame->AddFrame(new TGHorizontal3DLine(mainFrame),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 5));
    
    // ─── INSTRUCTIONS ─────────────────────────────────
    TGLabel* instrLabel = new TGLabel(mainFrame, 
        "Select objects to load. Double-click to plot immediately.");
    instrLabel->SetTextJustify(kTextLeft);
    mainFrame->AddFrame(instrLabel,
        new TGLayoutHints(kLHintsLeft, 10, 5, 5, 10));
    
    // ─── OBJECT TREE ──────────────────────────────────
    TGGroupFrame* treeFrame = new TGGroupFrame(mainFrame, "ROOT File Contents");
    
    // Create canvas for tree with scrollbar
    TGCanvas* canvas = new TGCanvas(treeFrame, 750, 400);
    fObjectTree = new TGListTree(canvas, kObjectTree);
    fObjectTree->Associate(this);
    
    treeFrame->AddFrame(canvas, 
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    mainFrame->AddFrame(treeFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // ─── BROWSER CHECKBOX ─────────────────────────────
    fShowBrowserCheck = new TGCheckButton(mainFrame, 
        "Open TBrowser after loading", kShowBrowserCheck);
    fShowBrowserCheck->SetState(kButtonUp);  // Default: don't show
    fShowBrowserCheck->Associate(this);
    mainFrame->AddFrame(fShowBrowserCheck,
        new TGLayoutHints(kLHintsLeft, 10, 5, 5, 10));
    
    // ─── BUTTON FRAME ─────────────────────────────────
    TGHorizontalFrame* buttonFrame = new TGHorizontalFrame(mainFrame);
    
    fLoadButton = new TGTextButton(buttonFrame, "&Load Selected", kLoadButton);
    fLoadButton->Associate(this);
    fLoadButton->SetToolTipText("Load selected objects for plotting");
    buttonFrame->AddFrame(fLoadButton,
        new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fBrowserButton = new TGTextButton(buttonFrame, "&TBrowser Only", kBrowserButton);
    fBrowserButton->Associate(this);
    fBrowserButton->SetToolTipText("Just open TBrowser, don't load objects");
    buttonFrame->AddFrame(fBrowserButton,
        new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fCancelButton = new TGTextButton(buttonFrame, "&Cancel", kCancelButton);
    fCancelButton->Associate(this);
    buttonFrame->AddFrame(fCancelButton,
        new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    mainFrame->AddFrame(buttonFrame,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 10, 5));
}

// ============================================================================
// Scan the ROOT file and populate the tree
// ============================================================================
void ROOTFileBrowser::ScanFile()
{
    if (!fFile) return;
    
    // Create root categories
    TGListTreeItem* histItem = fObjectTree->AddItem(nullptr, "Histograms");
    TGListTreeItem* graphItem = fObjectTree->AddItem(nullptr, "Graphs");
    TGListTreeItem* treeItem = fObjectTree->AddItem(nullptr, "TTrees");
    TGListTreeItem* otherItem = fObjectTree->AddItem(nullptr, "Other Objects");
    
    histItem->SetCheckBox(kTRUE);
    graphItem->SetCheckBox(kTRUE);
    treeItem->SetCheckBox(kTRUE);
    otherItem->SetCheckBox(kTRUE);
    
    // Iterate through all keys in the file
    TIter next(fFile->GetListOfKeys());
    TKey* key;
    
    while ((key = (TKey*)next())) {
        const char* className = key->GetClassName();
        const char* objName = key->GetName();
        const char* objTitle = key->GetTitle();
        
        // Get the object to check its type
        TObject* obj = key->ReadObj();
        if (!obj) continue;
        
        ROOTObjectInfo info;
        info.name = objName;
        info.title = objTitle;
        info.type = className;
        info.category = GetObjectCategory(className);
        info.object = obj;
        info.selected = false;
        
        // Add to appropriate category
        TGListTreeItem* parent = nullptr;
        if (info.category == "Histogram") {
            parent = histItem;
        } else if (info.category == "Graph") {
            parent = graphItem;
        } else if (info.category == "Tree") {
            parent = treeItem;
            
            // For TTrees, add branch information
            TTree* tree = dynamic_cast<TTree*>(obj);
            if (tree) {
                info.title = Form("%s (%lld entries)", objTitle, tree->GetEntries());
            }
        } else {
            parent = otherItem;
        }
        
        if (parent) {
            AddObjectToTree(info, parent);
            fObjects.push_back(info);
        }
    }
    
    // Expand all categories
    fObjectTree->OpenItem(histItem);
    fObjectTree->OpenItem(graphItem);
    fObjectTree->OpenItem(treeItem);
    
    // Show summary
    int nHist = 0, nGraph = 0, nTree = 0, nOther = 0;
    for (const auto& obj : fObjects) {
        if (obj.category == "Histogram") nHist++;
        else if (obj.category == "Graph") nGraph++;
        else if (obj.category == "Tree") nTree++;
        else nOther++;
    }
    
    std::cout << "\n=== ROOT File Contents ===" << std::endl;
    std::cout << "File: " << fFilename << std::endl;
    std::cout << "Histograms: " << nHist << std::endl;
    std::cout << "Graphs: " << nGraph << std::endl;
    std::cout << "TTrees: " << nTree << std::endl;
    std::cout << "Other: " << nOther << std::endl;
    std::cout << "Total: " << fObjects.size() << " objects" << std::endl;
    std::cout << "=========================\n" << std::endl;
}

// ============================================================================
// Add object to tree
// ============================================================================
void ROOTFileBrowser::AddObjectToTree(const ROOTObjectInfo& obj, TGListTreeItem* parent)
{
    std::string label = Form("%s : %s", obj.name.c_str(), obj.type.c_str());
    if (!obj.title.empty() && obj.title != obj.name) {
        label += Form(" - %s", obj.title.c_str());
    }
    
    TGListTreeItem* item = fObjectTree->AddItem(parent, label.c_str());
    item->SetCheckBox(kTRUE);
    item->SetUserData((void*)&obj);  // Store pointer to object info
    
    // For TTrees, add branch information as children
    if (obj.category == "Tree") {
        TTree* tree = dynamic_cast<TTree*>(obj.object);
        if (tree) {
            TObjArray* branches = tree->GetListOfBranches();
            if (branches) {
                for (int i = 0; i < branches->GetEntries() && i < 20; i++) {  // Limit to 20 branches
                    TBranch* branch = (TBranch*)branches->At(i);
                    if (branch) {
                        std::string branchInfo = Form("  └─ %s [%s]", 
                            branch->GetName(), 
                            branch->GetClassName()[0] ? branch->GetClassName() : "numeric");
                        TGListTreeItem* branchItem = fObjectTree->AddItem(item, branchInfo.c_str());
                        branchItem->SetCheckBox(kFALSE);  // Branches not selectable yet
                    }
                }
                if (branches->GetEntries() > 20) {
                    fObjectTree->AddItem(item, Form("  ... and %d more branches", 
                        branches->GetEntries() - 20));
                }
            }
        }
    }
}

// ============================================================================
// Determine object category from class name
// ============================================================================
std::string ROOTFileBrowser::GetObjectCategory(const char* className)
{
    std::string cn = className;
    
    // Histograms
    if (cn.find("TH1") == 0 || cn.find("TH2") == 0 || cn.find("TH3") == 0 ||
        cn.find("TProfile") == 0) {
        return "Histogram";
    }
    
    // Graphs
    if (cn.find("TGraph") == 0) {
        return "Graph";
    }
    
    // Trees
    if (cn == "TTree" || cn == "TNtuple" || cn == "TChain") {
        return "Tree";
    }
    
    return "Other";
}

// ============================================================================
// Modal dialog
// ============================================================================
Int_t ROOTFileBrowser::DoModal()
{
    if (fModalResult == 0) {
        // File open failed, already showed error
        return 0;
    }
    
    fModalResult = -1;  // Reset
    
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
    
    gSystem->ProcessEvents();
    gSystem->Sleep(50);
    
    if (!IsMapped()) {
        std::cout << "ERROR: ROOT file browser window failed to map!" << std::endl;
        return 0;
    }
    
    // Process events until window is unmapped
    while (IsMapped()) {
        gSystem->ProcessEvents();
        gSystem->Sleep(10);
    }
    
    if (fModalResult == -1) {
        fModalResult = 0;  // User closed window
    }
    
    return fModalResult;
}

// ============================================================================
// Close window
// ============================================================================
void ROOTFileBrowser::CloseWindow()
{
    fModalResult = 0;
    UnmapWindow();
}

// ============================================================================
// Process messages
// ============================================================================
Bool_t ROOTFileBrowser::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    switch(GET_MSG(msg)) {
        case kC_COMMAND:
            switch(GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == kLoadButton) {
                        // Mark selected objects
                        for (auto& obj : fObjects) {
                            // Find corresponding tree item and check if selected
                            // For now, just mark all as selected (we'll implement selection later)
                            obj.selected = true;
                        }
                        fModalResult = 1;  // Load objects
                        UnmapWindow();
                        return kTRUE;
                        
                    } else if (parm1 == kBrowserButton) {
                        fModalResult = 2;  // Just open TBrowser
                        UnmapWindow();
                        return kTRUE;
                        
                    } else if (parm1 == kCancelButton) {
                        fModalResult = 0;  // Cancel
                        UnmapWindow();
                        return kTRUE;
                    }
                    break;
            }
            break;
            
        case kC_LISTTREE:
            switch(GET_SUBMSG(msg)) {
                case kCT_ITEMDBLCLICK:
                    // Double-click on object - plot immediately
                    {
                        TGListTreeItem* item = fObjectTree->GetSelected();
                        if (item && item->GetUserData()) {
                            ROOTObjectInfo* obj = (ROOTObjectInfo*)item->GetUserData();
                            obj->selected = true;
                            fModalResult = 1;  // Load this object
                            UnmapWindow();
                            return kTRUE;
                        }
                    }
                    break;
                    
                case kCT_ITEMCLICK:
                    // Single click - update selection state
                    {
                        TGListTreeItem* item = fObjectTree->GetSelected();
                        if (item && item->GetUserData()) {
                            ROOTObjectInfo* obj = (ROOTObjectInfo*)item->GetUserData();
                            obj->selected = item->IsChecked();
                        }
                    }
                    break;
            }
            break;
    }
    
    return TGTransientFrame::ProcessMessage(msg, parm1, parm2);
}

// ============================================================================
// Accessors
// ============================================================================
Bool_t ROOTFileBrowser::ShowBrowser() const
{
    return fShowBrowserCheck->IsOn();
}

std::vector<ROOTObjectInfo> ROOTFileBrowser::GetSelectedObjects() const
{
    std::vector<ROOTObjectInfo> selected;
    for (const auto& obj : fObjects) {
        if (obj.selected) {
            selected.push_back(obj);
        }
    }
    return selected;
}
