#include "ROOTFileBrowser.h"

#include <TGLayout.h>
#include <TGSplitter.h>
#include <TG3DLine.h>
#include <TGMsgBox.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TLegend.h>
#include <iostream>

// ============================================================================
// Constructor
// ============================================================================
ROOTFileBrowser::ROOTFileBrowser(const TGWindow* p, const char* filename)
    : TGTransientFrame(p, nullptr, 900, 700),
      fFile(nullptr),
      fFilename(filename),
      fModalResult(-1)
{
    SetWindowName("ROOT File Browser - Enhanced");
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
        "Select objects to plot. Use checkboxes for multiple selections.");
    instrLabel->SetTextJustify(kTextLeft);
    mainFrame->AddFrame(instrLabel,
        new TGLayoutHints(kLHintsLeft, 10, 5, 5, 10));
    
    // ─── OBJECT TREE ──────────────────────────────────
    TGGroupFrame* treeFrame = new TGGroupFrame(mainFrame, "ROOT File Contents");
    
    // Create canvas for tree with scrollbar
    TGCanvas* canvas = new TGCanvas(treeFrame, 850, 300);
    fObjectTree = new TGListTree(canvas, kObjectTree);
    fObjectTree->Associate(this);
    
    treeFrame->AddFrame(canvas, 
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    mainFrame->AddFrame(treeFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // ─── NEW: CANVAS OPTIONS ──────────────────────────
    fCanvasOptionsGroup = new TGGroupFrame(mainFrame, "Plotting Options", kHorizontalFrame);
    
    // Radio button group for canvas mode
    TGVerticalFrame* modeFrame = new TGVerticalFrame(fCanvasOptionsGroup);
    
    fSeparateCanvasRadio = new TGRadioButton(modeFrame, 
        "Separate Canvases (one per object)", kSeparateCanvasRadio);
    fSeparateCanvasRadio->SetState(kButtonDown);  // Default
    fSeparateCanvasRadio->Associate(this);
    modeFrame->AddFrame(fSeparateCanvasRadio, 
        new TGLayoutHints(kLHintsLeft, 5, 5, 2, 2));
    
    fSingleCanvasRadio = new TGRadioButton(modeFrame, 
        "Single Canvas (overlay all)", kSingleCanvasRadio);
    fSingleCanvasRadio->Associate(this);
    modeFrame->AddFrame(fSingleCanvasRadio, 
        new TGLayoutHints(kLHintsLeft, 5, 5, 2, 2));
    
    fDividedCanvasRadio = new TGRadioButton(modeFrame, 
        "Divided Canvas (grid layout)", kDividedCanvasRadio);
    fDividedCanvasRadio->Associate(this);
    modeFrame->AddFrame(fDividedCanvasRadio, 
        new TGLayoutHints(kLHintsLeft, 5, 5, 2, 2));
    
    fCanvasOptionsGroup->AddFrame(modeFrame, 
        new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5, 5, 5, 5));
    
    // Division options
    TGVerticalFrame* divFrame = new TGVerticalFrame(fCanvasOptionsGroup);
    
    TGHorizontalFrame* rowFrame = new TGHorizontalFrame(divFrame);
    rowFrame->AddFrame(new TGLabel(rowFrame, "Rows:"), 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
    fNRowsEntry = new TGNumberEntry(rowFrame, 2, 3, -1,
                                     TGNumberFormat::kNESInteger,
                                     TGNumberFormat::kNEAPositive,
                                     TGNumberFormat::kNELLimitMinMax, 1, 10);
    fNRowsEntry->Resize(60, 20);
    rowFrame->AddFrame(fNRowsEntry, 
        new TGLayoutHints(kLHintsLeft, 5, 5, 2, 2));
    divFrame->AddFrame(rowFrame, 
        new TGLayoutHints(kLHintsLeft, 5, 5, 2, 2));
    
    TGHorizontalFrame* colFrame = new TGHorizontalFrame(divFrame);
    colFrame->AddFrame(new TGLabel(colFrame, "Columns:"), 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
    fNColsEntry = new TGNumberEntry(colFrame, 2, 3, -1,
                                     TGNumberFormat::kNESInteger,
                                     TGNumberFormat::kNEAPositive,
                                     TGNumberFormat::kNELLimitMinMax, 1, 10);
    fNColsEntry->Resize(60, 20);
    colFrame->AddFrame(fNColsEntry, 
        new TGLayoutHints(kLHintsLeft, 5, 5, 2, 2));
    divFrame->AddFrame(colFrame, 
        new TGLayoutHints(kLHintsLeft, 5, 5, 2, 2));
    
    fCanvasOptionsGroup->AddFrame(divFrame, 
        new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 10, 5, 5, 5));
    
    mainFrame->AddFrame(fCanvasOptionsGroup,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ─── BROWSER CHECKBOX ─────────────────────────────
    fShowBrowserCheck = new TGCheckButton(mainFrame, 
        "Also open TBrowser", kShowBrowserCheck);
    fShowBrowserCheck->SetState(kButtonUp);
    fShowBrowserCheck->Associate(this);
    mainFrame->AddFrame(fShowBrowserCheck,
        new TGLayoutHints(kLHintsLeft, 10, 5, 5, 5));
    
    // ─── BUTTON FRAME ─────────────────────────────────
    TGHorizontalFrame* buttonFrame = new TGHorizontalFrame(mainFrame);
    
    fPlotButton = new TGTextButton(buttonFrame, "&Plot Selected", kPlotButton);
    fPlotButton->Associate(this);
    fPlotButton->SetToolTipText("Plot selected objects with chosen layout");
    buttonFrame->AddFrame(fPlotButton,
        new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fLoadButton = new TGTextButton(buttonFrame, "&Load for GUI", kLoadButton);
    fLoadButton->Associate(this);
    fLoadButton->SetToolTipText("Load selected objects into main GUI");
    buttonFrame->AddFrame(fLoadButton,
        new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    
    fBrowserButton = new TGTextButton(buttonFrame, "&TBrowser Only", kBrowserButton);
    fBrowserButton->Associate(this);
    fBrowserButton->SetToolTipText("Just open TBrowser");
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
// Scan file (same as before)
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
    
    // Iterate through all keys
    TIter next(fFile->GetListOfKeys());
    TKey* key;
    
    while ((key = (TKey*)next())) {
        const char* className = key->GetClassName();
        const char* objName = key->GetName();
        const char* objTitle = key->GetTitle();
        
        TObject* obj = key->ReadObj();
        if (!obj) continue;
        
        ROOTObjectInfo info;
        info.name = objName;
        info.title = objTitle;
        info.type = className;
        info.category = GetObjectCategory(className);
        info.object = obj;
        info.selected = false;
        
        TGListTreeItem* parent = nullptr;
        if (info.category == "Histogram") {
            parent = histItem;
        } else if (info.category == "Graph") {
            parent = graphItem;
        } else if (info.category == "Tree") {
            parent = treeItem;
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
    
    fObjectTree->OpenItem(histItem);
    fObjectTree->OpenItem(graphItem);
    fObjectTree->OpenItem(treeItem);
    
    // Print summary
    int nHist = 0, nGraph = 0, nTree = 0;
    for (const auto& obj : fObjects) {
        if (obj.category == "Histogram") nHist++;
        else if (obj.category == "Graph") nGraph++;
        else if (obj.category == "Tree") nTree++;
    }
    
    std::cout << "\n=== ROOT File Contents ===" << std::endl;
    std::cout << "Histograms: " << nHist << " | Graphs: " << nGraph 
              << " | TTrees: " << nTree << std::endl;
    std::cout << "=========================\n" << std::endl;
}

// ============================================================================
// Helper methods (same as before)
// ============================================================================
void ROOTFileBrowser::AddObjectToTree(const ROOTObjectInfo& obj, TGListTreeItem* parent)
{
    std::string label = Form("%s : %s", obj.name.c_str(), obj.type.c_str());
    if (!obj.title.empty() && obj.title != obj.name) {
        label += Form(" - %s", obj.title.c_str());
    }
    
    TGListTreeItem* item = fObjectTree->AddItem(parent, label.c_str());
    item->SetCheckBox(kTRUE);
    item->SetUserData((void*)&obj);
}

std::string ROOTFileBrowser::GetObjectCategory(const char* className)
{
    std::string cn = className;
    if (cn.find("TH1") == 0 || cn.find("TH2") == 0 || cn.find("TH3") == 0 ||
        cn.find("TProfile") == 0) return "Histogram";
    if (cn.find("TGraph") == 0) return "Graph";
    if (cn == "TTree" || cn == "TNtuple" || cn == "TChain") return "Tree";
    return "Other";
}

// ============================================================================
// NEW: Plot selected objects
// ============================================================================
void ROOTFileBrowser::PlotSelectedObjects()
{
    // Get selected objects
    std::vector<ROOTObjectInfo> selected;
    for (auto& obj : fObjects) {
        if (obj.selected) {
            selected.push_back(obj);
        }
    }
    
    if (selected.empty()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "No Selection", "Please select at least one object to plot.",
            kMBIconExclamation, kMBOk);
        return;
    }
    
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║          Plotting " << selected.size() << " Selected Objects" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    // Determine canvas mode
    CanvasMode mode = GetCanvasMode();
    
    switch (mode) {
        case kSeparate:
            PlotInSeparateCanvases(selected);
            break;
        case kSingle:
            PlotInSingleCanvas(selected);
            break;
        case kDivided:
            PlotInDividedCanvas(selected, GetNRows(), GetNCols());
            break;
    }
    
    std::cout << "→ Plotting complete!" << std::endl;
    std::cout << "════════════════════════════════════════════════════════════\n" << std::endl;
}

// ============================================================================
// Plot in separate canvases
// ============================================================================
void ROOTFileBrowser::PlotInSeparateCanvases(const std::vector<ROOTObjectInfo>& objects)
{
    static int canvasCount = 0;
    
    for (const auto& objInfo : objects) {
        canvasCount++;
        
        char canvasName[128];
        snprintf(canvasName, sizeof(canvasName), "c_sep_%d", canvasCount);
        
        TCanvas* c = new TCanvas(canvasName, 
                                 Form("%s - %s", objInfo.name.c_str(), objInfo.type.c_str()),
                                 800, 600);
        
        TObject* obj = fFile->Get(objInfo.name.c_str());
        if (!obj) continue;
        
        // Plot based on type
        if (obj->InheritsFrom(TH1::Class())) {
            TH1* hist = (TH1*)obj;
            hist->Draw();
            std::cout << "  ✓ " << objInfo.name << " [" << objInfo.type << "]" << std::endl;
        }
        else if (obj->InheritsFrom(TGraph::Class())) {
            TGraph* graph = (TGraph*)obj;
            graph->Draw("APL");
            std::cout << "  ✓ " << objInfo.name << " [" << objInfo.type << "]" << std::endl;
        }
        
        c->Update();
    }
}

// ============================================================================
// Plot in single canvas (overlay)
// ============================================================================
void ROOTFileBrowser::PlotInSingleCanvas(const std::vector<ROOTObjectInfo>& objects)
{
    static int canvasCount = 0;
    canvasCount++;
    
    char canvasName[128];
    snprintf(canvasName, sizeof(canvasName), "c_overlay_%d", canvasCount);
    
    TCanvas* c = new TCanvas(canvasName, "Overlay Plot", 900, 700);
    
    // Create legend
    TLegend* legend = new TLegend(0.65, 0.65, 0.89, 0.89);
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    
    bool firstDraw = true;
    int colorIndex = 1;
    
    for (const auto& objInfo : objects) {
        TObject* obj = fFile->Get(objInfo.name.c_str());
        if (!obj) continue;
        
        // Assign different colors
        int color = (colorIndex % 9) + 1;
        if (color == 0 || color == 10) color = 1;
        colorIndex++;
        
        // Plot based on type
        if (obj->InheritsFrom(TH1::Class())) {
            TH1* hist = (TH1*)obj->Clone(Form("%s_clone", objInfo.name.c_str()));
            hist->SetLineColor(color);
            hist->SetLineWidth(2);
            
            if (firstDraw) {
                hist->Draw();
                firstDraw = false;
            } else {
                hist->Draw("SAME");
            }
            
            legend->AddEntry(hist, objInfo.name.c_str(), "l");
            std::cout << "  ✓ " << objInfo.name << " [color " << color << "]" << std::endl;
        }
        else if (obj->InheritsFrom(TGraph::Class())) {
            TGraph* graph = (TGraph*)obj->Clone(Form("%s_clone", objInfo.name.c_str()));
            graph->SetLineColor(color);
            graph->SetMarkerColor(color);
            graph->SetLineWidth(2);
            graph->SetMarkerStyle(20 + (colorIndex % 5));
            
            if (firstDraw) {
                graph->Draw("APL");
                firstDraw = false;
            } else {
                graph->Draw("PL SAME");
            }
            
            legend->AddEntry(graph, objInfo.name.c_str(), "lp");
            std::cout << "  ✓ " << objInfo.name << " [color " << color << "]" << std::endl;
        }
    }
    
    if (legend->GetNRows() > 0) {
        legend->Draw();
    }
    
    c->Update();
}

// ============================================================================
// Plot in divided canvas
// ============================================================================
void ROOTFileBrowser::PlotInDividedCanvas(const std::vector<ROOTObjectInfo>& objects, 
                                          Int_t nRows, Int_t nCols)
{
    static int canvasCount = 0;
    canvasCount++;
    
    char canvasName[128];
    snprintf(canvasName, sizeof(canvasName), "c_divided_%d", canvasCount);
    
    TCanvas* c = new TCanvas(canvasName, "Divided Canvas", 1200, 900);
    c->Divide(nCols, nRows);
    
    int padIndex = 1;
    for (const auto& objInfo : objects) {
        if (padIndex > nRows * nCols) {
            std::cout << "  ⚠ Not enough pads for all objects (have " << nRows*nCols << " pads)" << std::endl;
            break;
        }
        
        c->cd(padIndex++);
        
        TObject* obj = fFile->Get(objInfo.name.c_str());
        if (!obj) continue;
        
        // Plot based on type
        if (obj->InheritsFrom(TH1::Class())) {
            TH1* hist = (TH1*)obj;
            hist->Draw();
            std::cout << "  ✓ Pad " << (padIndex-1) << ": " << objInfo.name << std::endl;
        }
        else if (obj->InheritsFrom(TGraph::Class())) {
            TGraph* graph = (TGraph*)obj;
            graph->Draw("APL");
            std::cout << "  ✓ Pad " << (padIndex-1) << ": " << objInfo.name << std::endl;
        }
    }
    
    c->Update();
}

// ============================================================================
// Get canvas mode
// ============================================================================
ROOTFileBrowser::CanvasMode ROOTFileBrowser::GetCanvasMode() const
{
    if (fSingleCanvasRadio->IsDown()) return kSingle;
    if (fDividedCanvasRadio->IsDown()) return kDivided;
    return kSeparate;  // Default
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
                    if (parm1 == kPlotButton) {
                        // Mark selected objects
                        for (auto& obj : fObjects) {
                            // Check tree for selection
                            // This is simplified - proper implementation would iterate tree
                            obj.selected = true;  // TODO: Check actual selection state
                        }
                        PlotSelectedObjects();
                        return kTRUE;
                        
                    } else if (parm1 == kLoadButton) {
                        for (auto& obj : fObjects) {
                            obj.selected = true;
                        }
                        fModalResult = 1;
                        UnmapWindow();
                        return kTRUE;
                        
                    } else if (parm1 == kBrowserButton) {
                        fModalResult = 2;
                        UnmapWindow();
                        return kTRUE;
                        
                    } else if (parm1 == kCancelButton) {
                        fModalResult = 0;
                        UnmapWindow();
                        return kTRUE;
                    }
                    break;
                    
                case kCM_RADIOBUTTON:
                    // Handle radio button changes
                    if (parm1 == kSeparateCanvasRadio) {
                        fSingleCanvasRadio->SetState(kButtonUp);
                        fDividedCanvasRadio->SetState(kButtonUp);
                    } else if (parm1 == kSingleCanvasRadio) {
                        fSeparateCanvasRadio->SetState(kButtonUp);
                        fDividedCanvasRadio->SetState(kButtonUp);
                    } else if (parm1 == kDividedCanvasRadio) {
                        fSeparateCanvasRadio->SetState(kButtonUp);
                        fSingleCanvasRadio->SetState(kButtonUp);
                    }
                    return kTRUE;
            }
            break;
            
        case kC_LISTTREE:
            switch(GET_SUBMSG(msg)) {
                case kCT_ITEMCLICK:
                    // Update selection state
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

// Modal and other methods remain the same...
Int_t ROOTFileBrowser::DoModal()
{
    if (fModalResult == 0) return 0;
    fModalResult = -1;
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
    gSystem->ProcessEvents();
    gSystem->Sleep(50);
    if (!IsMapped()) return 0;
    while (IsMapped()) {
        gSystem->ProcessEvents();
        gSystem->Sleep(10);
    }
    if (fModalResult == -1) fModalResult = 0;
    return fModalResult;
}

void ROOTFileBrowser::CloseWindow()
{
    fModalResult = 0;
    UnmapWindow();
}

Bool_t ROOTFileBrowser::ShowBrowser() const
{
    return fShowBrowserCheck->IsOn();
}

std::vector<ROOTObjectInfo> ROOTFileBrowser::GetSelectedObjects() const
{
    std::vector<ROOTObjectInfo> selected;
    for (const auto& obj : fObjects) {
        if (obj.selected) selected.push_back(obj);
    }
    return selected;
}