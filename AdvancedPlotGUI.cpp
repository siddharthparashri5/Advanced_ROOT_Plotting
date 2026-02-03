#include <TGClient.h>
#include <TGButton.h>
#include <TGFileDialog.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGLayout.h>
#include <TGListBox.h>
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>
#include <vector>
#include <string>

#include "DataReader.h"
#include "ColumnSelector.h"
#include "PlotTypes.h"
#include "FitUtils.h"

// ==========================
// Main GUI Class
// ==========================
class AdvancedPlotGUI : public TGMainFrame {
private:
    // File selection
    TGTextEntry* fileEntry;
    TGTextButton* browseButton;
    TGTextButton* loadButton;
    
    // Plot configuration
    TGTextButton* addPlotButton;
    TGListBox* plotListBox;
    TGTextButton* removePlotButton;
    TGTextButton* clearAllButton;
    
    // Canvas options
    TGCheckButton* sameCanvasCheck;
    TGCheckButton* dividedCanvasCheck;
    TGNumberEntry* nRowsEntry;
    TGNumberEntry* nColsEntry;
    
    // Fit options
    TGComboBox* fitFunctionCombo;
    TGTextEntry* customFuncEntry;
    TGTextEntry* canvasTitleEntry;
    
    // Action buttons
    TGTextButton* plotButton;
    
    // Data storage
    ColumnData currentData;
    std::vector<PlotConfig> plotConfigs;

    TGMainFrame* fMainFrame;
    
    enum {
        kBrowseButton = 100,
        kLoadButton,
        kAddPlotButton,
        kRemovePlotButton,
        kClearAllButton,
        kPlotButton
    };

public:
    AdvancedPlotGUI(const TGWindow* p, UInt_t w, UInt_t h);
    virtual ~AdvancedPlotGUI() {}

    void DoBrowse();
    void DoLoad();
    void DoAddPlot();
    void DoRemovePlot();
    void DoClearAll();
    void DoPlot();
    
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};

// ==========================
// Constructor
// ==========================
AdvancedPlotGUI::AdvancedPlotGUI(const TGWindow* p, UInt_t w, UInt_t h) 
    : TGMainFrame(p, w, h) {

    fMainFrame = this;

    // File selection section
    TGGroupFrame* fileGroup = new TGGroupFrame(this, "Data File");
    TGHorizontalFrame* fileFrame = new TGHorizontalFrame(fileGroup);
    
    fileEntry = new TGTextEntry(fileFrame);
    fileEntry->Resize(300, 20);
    fileFrame->AddFrame(fileEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));
    
    browseButton = new TGTextButton(fileFrame, "Browse...", kBrowseButton);
    browseButton->Associate(this);
    fileFrame->AddFrame(browseButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));
    
    loadButton = new TGTextButton(fileFrame, "Load", kLoadButton);
    loadButton->Associate(this);
    fileFrame->AddFrame(loadButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));
    
    fileGroup->AddFrame(fileFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
    AddFrame(fileGroup, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
    
    // Plot configuration section
    TGGroupFrame* plotGroup = new TGGroupFrame(this, "Plot Configuration");
    
    addPlotButton = new TGTextButton(plotGroup, "Add Plot...", kAddPlotButton);
    addPlotButton->Associate(this);
    plotGroup->AddFrame(addPlotButton, new TGLayoutHints(kLHintsCenterX, 5,5,5,5));
    addPlotButton->SetEnabled(false);
    
    plotListBox = new TGListBox(plotGroup);
    plotListBox->Resize(500, 150);
    plotGroup->AddFrame(plotListBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));
    
    TGHorizontalFrame* plotButtonFrame = new TGHorizontalFrame(plotGroup);
    removePlotButton = new TGTextButton(plotButtonFrame, "Remove Selected", kRemovePlotButton);
    removePlotButton->Associate(this);
    clearAllButton = new TGTextButton(plotButtonFrame, "Clear All", kClearAllButton);
    clearAllButton->Associate(this);
    plotButtonFrame->AddFrame(removePlotButton, new TGLayoutHints(kLHintsCenterX, 5,5,2,2));
    plotButtonFrame->AddFrame(clearAllButton, new TGLayoutHints(kLHintsCenterX, 5,5,2,2));
    plotGroup->AddFrame(plotButtonFrame, new TGLayoutHints(kLHintsCenterX, 5,5,5,5));
    
    AddFrame(plotGroup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));

     // Canvas title
    TGHorizontalFrame* titleFrame = new TGHorizontalFrame(this);
    TGLabel* titleLabel = new TGLabel(titleFrame, "Canvas Title:");
    titleFrame->AddFrame(titleLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    
    canvasTitleEntry = new TGTextEntry(titleFrame);
    canvasTitleEntry->SetText("Plot");
    canvasTitleEntry->Resize(300, 20);
    titleFrame->AddFrame(canvasTitleEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));
    AddFrame(titleFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
    
    // Same canvas option
    //sameCanvasCheck = new TGCheckButton(this, "Same canvas (Overlay)");
    //AddFrame(sameCanvasCheck, new TGLayoutHints(kLHintsLeft, 5,5,5,5));
    
    // Canvas division options
    TGHorizontalFrame* divFrame1 = new TGHorizontalFrame(this);
    sameCanvasCheck = new TGCheckButton(divFrame1, "Same canvas (Overlay)");
    divFrame1->AddFrame(sameCanvasCheck, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    AddFrame(divFrame1, new TGLayoutHints(kLHintsLeft, 5,5,5,5));

    TGHorizontalFrame* divFrame = new TGHorizontalFrame(this);

    dividedCanvasCheck = new TGCheckButton(divFrame, "Divide Canvas:");
    divFrame->AddFrame(dividedCanvasCheck, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    
    TGLabel* rowLabel = new TGLabel(divFrame, "Rows:");
    divFrame->AddFrame(rowLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,2,2,2));
    
    nRowsEntry = new TGNumberEntry(divFrame, 2, 3, -1, 
                                    TGNumberFormat::kNESInteger,
                                    TGNumberFormat::kNEAPositive,
                                    TGNumberFormat::kNELLimitMinMax, 1, 10);
    nRowsEntry->Resize(50, 20);
    divFrame->AddFrame(nRowsEntry, new TGLayoutHints(kLHintsLeft, 2,5,2,2));
    
    TGLabel* colLabel = new TGLabel(divFrame, "Cols:");
    divFrame->AddFrame(colLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,2,2,2));
    
    nColsEntry = new TGNumberEntry(divFrame, 2, 3, -1,
                                    TGNumberFormat::kNESInteger,
                                    TGNumberFormat::kNEAPositive,
                                    TGNumberFormat::kNELLimitMinMax, 1, 10);
    nColsEntry->Resize(50, 20);
    divFrame->AddFrame(nColsEntry, new TGLayoutHints(kLHintsLeft, 2,5,2,2));
    
    AddFrame(divFrame, new TGLayoutHints(kLHintsLeft, 5,5,5,5));
    
    // Fit function section
    TGHorizontalFrame* fitFrame = new TGHorizontalFrame(this);
    TGLabel* fitLabel = new TGLabel(fitFrame, "Fit Function:");
    fitFrame->AddFrame(fitLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    
    fitFunctionCombo = new TGComboBox(fitFrame);
    fitFunctionCombo->AddEntry("No Fit", FitUtils::kNoFit);
    fitFunctionCombo->AddEntry("Gaussian", FitUtils::kGaus);
    fitFunctionCombo->AddEntry("Linear", FitUtils::kLinear);
    fitFunctionCombo->AddEntry("Pol2", FitUtils::kPol2);
    fitFunctionCombo->AddEntry("Pol3", FitUtils::kPol3);
    fitFunctionCombo->AddEntry("Exponential", FitUtils::kExpo);
    fitFunctionCombo->AddEntry("Sine + Offset", FitUtils::kSineOffset);
    fitFunctionCombo->AddEntry("Custom Function", FitUtils::kCustom);
    fitFunctionCombo->Select(FitUtils::kNoFit);
    fitFunctionCombo->Resize(150, 20);
    fitFrame->AddFrame(fitFunctionCombo, new TGLayoutHints(kLHintsLeft, 5,5,2,2));
    
    AddFrame(fitFrame, new TGLayoutHints(kLHintsLeft, 5,5,5,5));
    
    // Custom function entry
    TGHorizontalFrame* customFrame = new TGHorizontalFrame(this);
    TGLabel* customLabel = new TGLabel(customFrame, "Custom Fit:");
    customFrame->AddFrame(customLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    
    customFuncEntry = new TGTextEntry(customFrame);
    customFuncEntry->SetText("[0]*x^2 + [1]*x + [2]");
    customFuncEntry->Resize(300, 20);
    customFrame->AddFrame(customFuncEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));
    
    AddFrame(customFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
    
    // Plot button
    plotButton = new TGTextButton(this, "Create Plots", kPlotButton);
    plotButton->Associate(this);
    AddFrame(plotButton, new TGLayoutHints(kLHintsCenterX, 5,5,10,10));
    plotButton->SetEnabled(false);

    SetWindowName("Advanced Plot GUI");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

// ==========================
// Browse for file
// ==========================
void AdvancedPlotGUI::DoBrowse() {
    //std::cout << "DoBrowse called" << std::endl;
    
    TGFileInfo fi;
    const char* filetypes[] = {
        "Text files", "*.txt",
        "Data files", "*.dat",
        "CSV files", "*.csv",
        "ROOT files", "*.root",
        "All files", "*",
        0, 0
    };
    fi.fFileTypes = filetypes;

    new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);

    if (fi.fFilename && strlen(fi.fFilename) > 0) {
        //std::cout << "Selected file: " << fi.fFilename << std::endl;
        fileEntry->SetText(fi.fFilename);
    } else {
        std::cout << "No file selected" << std::endl;
    }
}

// ==========================
// Load data file
// ==========================
void AdvancedPlotGUI::DoLoad() {
    const char* filename = fileEntry->GetText();
    if (!filename || strlen(filename) == 0) {
        std::cout << "No file selected!" << std::endl;
        return;
    }
    
    if (DataReader::ReadFile(filename, currentData)) {
        //std::cout << "Loaded " << currentData.GetNumColumns() << " columns, "<< currentData.GetNumRows() << " rows" << std::endl;
        addPlotButton->SetEnabled(true);
        plotButton->SetEnabled(true);
    } else {
        std::cout << "Failed to load file!" << std::endl;
    }
}

// ==========================
// Add plot configuration
// ==========================
void AdvancedPlotGUI::DoAddPlot()
{
    //std::cout << "DoAddPlot called" << std::endl;

    if (currentData.GetNumColumns() == 0) {
        std::cout << "No data loaded!" << std::endl;
        return;
    }

    PlotConfig config;
    bool result = false;

    ColumnSelectorDialog* dialog = new ColumnSelectorDialog(
            fMainFrame,      // parent
            &currentData,
            &config,
            &result
        );

  
    gClient->WaitFor(dialog);

    //std::cout << "Dialog closed, result=" << result << std::endl;

    //delete dialog;

    if (!result) return;

    plotConfigs.push_back(config);

    std::string desc;
    switch (config.type) {
        case PlotConfig::kTGraph:
            desc = Form("TGraph: %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str());
            break;
        case PlotConfig::kTGraphErrors:
            desc = Form("TGraphErrors: %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str());
            break;
        case PlotConfig::kTH1D:
            desc = Form("TH1D: %s",
                        currentData.headers[config.xColumn].c_str());
            break;
        case PlotConfig::kTH2D:
            desc = Form("TH2D: %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str());
            break;
    }

    plotListBox->AddEntry(desc.c_str(), plotConfigs.size() - 1);
    plotListBox->Layout();
}


// ==========================
// Remove selected plot
// ==========================
void AdvancedPlotGUI::DoRemovePlot() {
    int selected = plotListBox->GetSelected();
    if (selected >= 0) {
        plotConfigs.erase(plotConfigs.begin() + selected);
        plotListBox->RemoveEntry(selected);
        plotListBox->Layout();
    }
}

// ==========================
// Clear all plots
// ==========================
void AdvancedPlotGUI::DoClearAll() {
    plotConfigs.clear();
    plotListBox->RemoveAll();
    plotListBox->Layout();
}

// ==========================
// Create plots
// ==========================
void AdvancedPlotGUI::DoPlot() {
    if (plotConfigs.empty()) {
        //std::cout << "No plots configured!" << std::endl;
        return;
    }
    
    bool sameCanvas = sameCanvasCheck->IsOn();
    bool dividedCanvas = dividedCanvasCheck->IsOn();
    int nRows = (int)nRowsEntry->GetNumber();
    int nCols = (int)nColsEntry->GetNumber();
    std::string canvasTitle = canvasTitleEntry->GetText();
    
    FitUtils::FitType fitType = static_cast<FitUtils::FitType>(
        fitFunctionCombo->GetSelected());
    std::string customFunc = customFuncEntry->GetText();
    
    TCanvas* canvas = nullptr;
    TLegend* legend = nullptr;
    
    // Same canvas mode - overlay all plots
    if (sameCanvas) {
        canvas = new TCanvas("c_same", canvasTitle.c_str(), 900, 700);
        legend = new TLegend(0.65, 0.65, 0.89, 0.89);
        legend->SetBorderSize(1);
        legend->SetFillColor(0);
        
        bool firstPlot = true;
        for (size_t i = 0; i < plotConfigs.size(); ++i) {
            PlotConfig& config = plotConfigs[i];
            config.color = (i % 9) + 1;
            
            if (config.type == PlotConfig::kTGraph) {
                TGraph* g = PlotCreator::CreateTGraph(currentData, config);
                if (g) {
                    g->SetTitle(canvasTitle.c_str());
                    g->Draw(firstPlot ? "APL" : "PL SAME");
                    legend->AddEntry(g, currentData.headers[config.yColumn].c_str(), "lp");
                    firstPlot = false;
                    
                    if (fitType != FitUtils::kNoFit) {
                        TF1* fit = FitUtils::FitGraph(g, fitType, config.color, customFunc);
                        if (fit) fit->Draw("SAME");
                    }
                }
            } else if (config.type == PlotConfig::kTGraphErrors) {
                TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
                if (g) {
                    g->SetTitle(canvasTitle.c_str());
                    g->Draw(firstPlot ? "APE" : "PE SAME");
                    legend->AddEntry(g, currentData.headers[config.yColumn].c_str(), "lpe");
                    firstPlot = false;
                    
                    if (fitType != FitUtils::kNoFit) {
                        TF1* fit = FitUtils::FitGraph(g, fitType, config.color, customFunc);
                        if (fit) fit->Draw("SAME");
                    }
                }
            } else if (config.type == PlotConfig::kTH1D) {
                TH1D* h = PlotCreator::CreateTH1D(currentData, config);
                if (h) {
                    h->SetTitle(canvasTitle.c_str());
                    h->SetLineColor(config.color);
                    h->Draw(firstPlot ? "" : "SAME");
                    legend->AddEntry(h, currentData.headers[config.xColumn].c_str(), "l");
                    firstPlot = false;
                }
            }
        }
        
        if (legend && legend->GetNRows() > 0) legend->Draw();
        canvas->Update();
    }
    // Divided canvas mode
    else if (dividedCanvas) {
        canvas = new TCanvas("c_divided", canvasTitle.c_str(), 1200, 900);
        canvas->Divide(nCols, nRows);
        
        int padNum = 1;
        for (size_t i = 0; i < plotConfigs.size(); ++i) {
            PlotConfig& config = plotConfigs[i];
            config.color = (i % 9) + 1;
            
            if (padNum > nRows * nCols) break;
            canvas->cd(padNum++);
            
            if (config.type == PlotConfig::kTGraph) {
                TGraph* g = PlotCreator::CreateTGraph(currentData, config);
                if (g) {
                    g->Draw("APL");
                    if (fitType != FitUtils::kNoFit) {
                        TF1* fit = FitUtils::FitGraph(g, fitType, config.color, customFunc);
                        if (fit) fit->Draw("SAME");
                    }
                }
            } else if (config.type == PlotConfig::kTGraphErrors) {
                TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
                if (g) {
                    g->Draw("APE");
                    if (fitType != FitUtils::kNoFit) {
                        TF1* fit = FitUtils::FitGraph(g, fitType, config.color, customFunc);
                        if (fit) fit->Draw("SAME");
                    }
                }
            } else if (config.type == PlotConfig::kTH1D) {
                TH1D* h = PlotCreator::CreateTH1D(currentData, config);
                if (h) h->Draw();
            } else if (config.type == PlotConfig::kTH2D) {
                TH2D* h = PlotCreator::CreateTH2D(currentData, config);
                if (h) h->Draw("COLZ");
            }
        }
        canvas->Update();
    }
    // Separate canvases mode
    else {
        for (size_t i = 0; i < plotConfigs.size(); ++i) {
            PlotConfig& config = plotConfigs[i];
            config.color = (i % 9) + 1;
            
            TCanvas* c = new TCanvas(Form("c%zu", i), 
                                    Form("%s - %zu", canvasTitle.c_str(), i), 
                                    800, 600);
            
            if (config.type == PlotConfig::kTGraph) {
                TGraph* g = PlotCreator::CreateTGraph(currentData, config);
                if (g) {
                    g->Draw("APL");
                    if (fitType != FitUtils::kNoFit) {
                        TF1* fit = FitUtils::FitGraph(g, fitType, config.color, customFunc);
                        if (fit) fit->Draw("SAME");
                    }
                }
            } else if (config.type == PlotConfig::kTGraphErrors) {
                TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
                if (g) {
                    g->Draw("APE");
                    if (fitType != FitUtils::kNoFit) {
                        TF1* fit = FitUtils::FitGraph(g, fitType, config.color, customFunc);
                        if (fit) fit->Draw("SAME");
                    }
                }
            } else if (config.type == PlotConfig::kTH1D) {
                TH1D* h = PlotCreator::CreateTH1D(currentData, config);
                if (h) h->Draw();
            } else if (config.type == PlotConfig::kTH2D) {
                TH2D* h = PlotCreator::CreateTH2D(currentData, config);
                if (h) h->Draw("COLZ");
            }
            
            c->Update();
        }
    }
    
    gSystem->ProcessEvents();
}

// ==========================
// Process messages
// ==========================
Bool_t AdvancedPlotGUI::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) {
    //std::cout << "ProcessMessage: msg=" << msg << " parm1=" << parm1 << " parm2=" << parm2 << std::endl;
    
    switch (GET_MSG(msg)) {
        case kC_COMMAND:
            switch (GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    //std::cout << "Button clicked: " << parm1 << std::endl;
                    if (parm1 == kBrowseButton) {
                        DoBrowse();
                    } else if (parm1 == kLoadButton) {
                        DoLoad();
                    } else if (parm1 == kAddPlotButton) {
                        DoAddPlot();
                    } else if (parm1 == kRemovePlotButton) {
                        DoRemovePlot();
                    } else if (parm1 == kClearAllButton) {
                        DoClearAll();
                    } else if (parm1 == kPlotButton) {
                        DoPlot();
                    }
                    break;
            }
            break;
    }
    return kTRUE;
}



// ==========================
// Main
// ==========================
int main(int argc, char** argv) {
    TApplication app("AdvancedPlotApp", &argc, argv);
    AdvancedPlotGUI* gui = new AdvancedPlotGUI(gClient->GetRoot(), 600, 700);
    app.Run();
    return 0;
}
