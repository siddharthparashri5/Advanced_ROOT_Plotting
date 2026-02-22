#include "PlotManager.h"
#include "AdvancedPlotGUI.h"
#include "ColumnSelectorDialog.h"
#include "PlotTypes.h"    // defines PlotConfig and PlotCreator
#include "ErrorHandling.h"

#include <TGClient.h>
#include <TGMsgBox.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TLegend.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>

#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooGaussian.h>
#include <RooPlot.h>
#include <RooFitResult.h>
#include <RooFit.h>

#include <iostream>


// ============================================================================
// Constructor
// ============================================================================
PlotManager::PlotManager(AdvancedPlotGUI* mainGUI)
    : fMainGUI(mainGUI)
{
}

// ============================================================================
// Destructor
// ============================================================================
PlotManager::~PlotManager()
{
}

// ============================================================================
// Add plot configuration
// ============================================================================
void PlotManager::AddPlot(const ColumnData& data)
{
    if (data.data.empty() || data.headers.empty()) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "No Data",
            "No data loaded. Please load a CSV or TXT file first.\n"
            "ROOT files are viewed in the Inspector, not plotted directly.",
            kMBIconExclamation, kMBOk);
        return;
    }

    // Stack-allocate config and result — no heap ownership confusion
    PlotConfig config;
    bool result = false;

    // ColumnSelectorDialog takes (parent, data*, config*, result*) and closes
    // itself via DeleteWindow(). gClient->WaitFor() blocks the event loop
    // until the dialog window is destroyed — this is the correct ROOT pattern.
    ColumnSelectorDialog* dialog = new ColumnSelectorDialog(
        fMainGUI,       // parent window
        &data,          // column data (read-only)
        &config,        // filled on OK
        &result         // set true on OK, false on Cancel
    );

    // Block here until dialog calls DeleteWindow() (in DoOK / DoCancel)
    gClient->WaitFor(dialog);

    // At this point `dialog` is deleted — do NOT touch it
    if (!result) return;

    // Store the config and update the list box
    fPlotConfigs.push_back(config);

    TString plotDesc = Form("Plot %zu: %s",
        fPlotConfigs.size(),
        fPlotConfigs.back().GetDescription().c_str());

    fMainGUI->AddPlotToListBox(plotDesc.Data(), (Int_t)fPlotConfigs.size() - 1);
}

// ============================================================================
// Remove plot configuration
// ============================================================================
void PlotManager::RemovePlot(Int_t index)
{
    if (index >= 0 && index < (Int_t)fPlotConfigs.size()) {
        fPlotConfigs.erase(fPlotConfigs.begin() + index);
        fMainGUI->RebuildPlotListBox(fPlotConfigs);
    }
}

// ============================================================================
// Clear all plot configurations
// ============================================================================
void PlotManager::ClearAll()
{
    fPlotConfigs.clear();
    fMainGUI->ClearPlotListBox();
}

// ============================================================================
// Main plot creation dispatcher
// ============================================================================
void PlotManager::CreatePlots(const std::string& canvasTitle, Bool_t overlayMode, 
                             Bool_t dividedMode, Int_t nRows, Int_t nCols,
                             FitUtils::FitType fitType, const std::string& customFunc,
                             const ColumnData& data)
{
    if (fPlotConfigs.empty()) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Warning", "No plots configured. Use 'Add Plot...' to create plots.",
            kMBIconExclamation, kMBOk);
        return;
    }

    if (dividedMode) {
        CreateDividedCanvas(canvasTitle, fitType, customFunc, data);
    } else if (overlayMode) {
        CreateOverlayCanvas(canvasTitle, fitType, customFunc, data);
    } else {
        CreateSeparateCanvases(canvasTitle, fitType, customFunc, data);
    }
    
    gSystem->ProcessEvents();
    ShowInfo(fMainGUI, "Plot Created", "Check the Plot Info in the terminal.\n\n");
}

// ============================================================================
// Create divided canvas
// ============================================================================
void PlotManager::CreateDividedCanvas(const std::string& title, FitUtils::FitType fitType, 
                                     const std::string& customFunc, const ColumnData& data)
{
    Int_t nRows = fMainGUI->GetNRows();
    Int_t nCols = fMainGUI->GetNCols();
    
    TCanvas* canvas = new TCanvas("c_divided", title.c_str(), 800, 600);
    canvas->Divide(nCols, nRows);

    for (size_t i = 0; i < fPlotConfigs.size() && i < (size_t)(nRows * nCols); ++i) {
        canvas->cd(i + 1);
        PlotConfig& config = fPlotConfigs[i];
        config.color = (i % 9) + 1;
        if (config.color == 0) config.color = 1;

        if (config.type == PlotConfig::kTGraph) {
            TGraph* g = PlotCreator::CreateTGraph(data, config);
            if (g) {
                g->Draw("APL");
                gROOT->GetListOfGlobals()->Add(g);
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTGraphErrors) {
            TGraphErrors* g = PlotCreator::CreateTGraphErrors(data, config);
            if (g) {
                g->Draw("APE");
                gROOT->GetListOfGlobals()->Add(g);
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
            TH1* h = PlotCreator::CreateTH1(data, config);
            if (h) {
                h->Draw();
                gROOT->GetListOfGlobals()->Add(h);
                
                if (fitType == FitUtils::kGaus) {
                    ApplyRooFitGaussian(h, config.color);
                } else {
                    ApplyFit(h, fitType, config.color, customFunc);
                }
            }
        } else if (config.type == PlotConfig::kTH2D || 
                   config.type == PlotConfig::kTH2F || 
                   config.type == PlotConfig::kTH2I) {
            TH2* h = PlotCreator::CreateTH2(data, config);
            if (h) {
                h->Draw("COLZ");
                gROOT->GetListOfGlobals()->Add(h);
                ApplyFit(h, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(data, config);
            if (h) {
                h->Draw("ISO");
                gROOT->GetListOfGlobals()->Add(h);
            }
        }
    }

    canvas->Update();
    PrintCanvasInfo(canvas);
}

// ============================================================================
// Create overlay canvas
// ============================================================================
void PlotManager::CreateOverlayCanvas(const std::string& title, FitUtils::FitType fitType, 
                                     const std::string& customFunc, const ColumnData& data)
{
    TCanvas* canvas = new TCanvas("c_overlay", title.c_str(), 800, 600);
    
    // Create a legend
    TLegend* padLegend = new TLegend(0.70, 0.70, 0.92, 0.92);
    padLegend->SetBorderSize(1);
    padLegend->SetFillColor(0);
    padLegend->SetTextSize(0.03);
    
    Bool_t firstDraw = kTRUE;

    for (size_t i = 0; i < fPlotConfigs.size(); ++i) {
        PlotConfig& config = fPlotConfigs[i];
        config.color = (i % 9) + 1;
        if (config.color == 0) config.color = 1;

        if (config.type == PlotConfig::kTGraph) {
            TGraph* g = PlotCreator::CreateTGraph(data, config);
            if (g) {
                g->Draw(firstDraw ? "APL" : "PL SAME");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s vs %s", 
                    data.headers[config.xColumn].c_str(),
                    data.headers[config.yColumn].c_str());
                padLegend->AddEntry(g, legendLabel.c_str(), "lp");
                
                ApplyFit(g, fitType, config.color, customFunc);
                firstDraw = kFALSE;
            }
        } else if (config.type == PlotConfig::kTGraphErrors) {
            TGraphErrors* g = PlotCreator::CreateTGraphErrors(data, config);
            if (g) {
                g->Draw(firstDraw ? "APE" : "PE SAME");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s vs %s", 
                    data.headers[config.xColumn].c_str(),
                    data.headers[config.yColumn].c_str());
                padLegend->AddEntry(g, legendLabel.c_str(), "lpe");
                
                ApplyFit(g, fitType, config.color, customFunc);
                firstDraw = kFALSE;
            }
        } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
            TH1* h = PlotCreator::CreateTH1(data, config);
            if (h) {
                h->Draw(firstDraw ? "" : "SAME");
                gROOT->GetListOfGlobals()->Add(h);
                
                std::string legendLabel = Form("%s", data.headers[config.xColumn].c_str());
                padLegend->AddEntry(h, legendLabel.c_str(), "l");
                
                if (fitType == FitUtils::kGaus) {
                    ApplyRooFitGaussian(h, config.color);
                } else {
                    ApplyFit(h, fitType, config.color, customFunc);
                }
                firstDraw = kFALSE;
            }
        } else if (config.type == PlotConfig::kTH2D || 
                   config.type == PlotConfig::kTH2F || 
                   config.type == PlotConfig::kTH2I) {
            TH2* h = PlotCreator::CreateTH2(data, config);
            if (h) {
                h->Draw("COLZ");
                gROOT->GetListOfGlobals()->Add(h);
            }
        } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(data, config);
            if (h) {
                h->Draw("ISO");
                gROOT->GetListOfGlobals()->Add(h);
            }
        }
        
        // Draw legend if it has entries
        if (padLegend->GetNRows() > 0) {
            padLegend->Draw();
        }
    }
    
    canvas->Update();
    PrintCanvasInfo(canvas);
}

// ============================================================================
// Create separate canvases
// ============================================================================
void PlotManager::CreateSeparateCanvases(const std::string& title, FitUtils::FitType fitType, 
                                        const std::string& customFunc, const ColumnData& data)
{
    for (size_t i = 0; i < fPlotConfigs.size(); ++i) {
        PlotConfig& config = fPlotConfigs[i];
        config.color = (i % 9) + 1;
        if (config.color == 0) config.color = 1;

        TCanvas* c = new TCanvas(Form("c%zu", i), Form("%s - %zu", title.c_str(), i), 800, 600);
        
        // Create legend for this canvas
        TLegend* canvasLegend = new TLegend(0.70, 0.75, 0.92, 0.92);
        canvasLegend->SetBorderSize(1);
        canvasLegend->SetFillColor(0);
        canvasLegend->SetTextSize(0.03);

        if (config.type == PlotConfig::kTGraph) {
            TGraph* g = PlotCreator::CreateTGraph(data, config);
            if (g) {
                g->Draw("APL");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s vs %s", 
                    data.headers[config.xColumn].c_str(),
                    data.headers[config.yColumn].c_str());
                canvasLegend->AddEntry(g, legendLabel.c_str(), "lp");
                
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTGraphErrors) {
            TGraphErrors* g = PlotCreator::CreateTGraphErrors(data, config);
            if (g) {
                g->Draw("APE");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s vs %s", 
                    data.headers[config.xColumn].c_str(),
                    data.headers[config.yColumn].c_str());
                canvasLegend->AddEntry(g, legendLabel.c_str(), "lpe");
                
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
            TH1* h = PlotCreator::CreateTH1(data, config);
            if (h) {
                h->Draw();
                gROOT->GetListOfGlobals()->Add(h);
                
                std::string legendLabel = Form("%s", data.headers[config.xColumn].c_str());
                canvasLegend->AddEntry(h, legendLabel.c_str(), "l");
                
                if (fitType == FitUtils::kGaus) {
                    ApplyRooFitGaussian(h, config.color);
                } else {
                    ApplyFit(h, fitType, config.color, customFunc);
                }
            }
        } else if (config.type == PlotConfig::kTH2D || 
                   config.type == PlotConfig::kTH2F || 
                   config.type == PlotConfig::kTH2I) {
            TH2* h = PlotCreator::CreateTH2(data, config);
            if (h) {
                h->Draw("COLZ");
                gROOT->GetListOfGlobals()->Add(h);
                ApplyFit(h, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(data, config);
            if (h) {
                h->Draw("ISO");
                gROOT->GetListOfGlobals()->Add(h);
            }
        }
        
        // Draw legend if it has entries
        if (canvasLegend->GetNRows() > 0) {
            canvasLegend->Draw();
        }
        
        c->Update();
        PrintCanvasInfo(c);
    }
}

// ============================================================================
// Apply fit to object
// ============================================================================
void PlotManager::ApplyFit(TObject* obj, FitUtils::FitType type, Int_t color, 
                          const std::string& customFunc)
{
    FitUtils::ApplyFit(obj, type, color, customFunc);
}

// ============================================================================
// Apply RooFit Gaussian
// ============================================================================
void PlotManager::ApplyRooFitGaussian(TH1* hist, Int_t color)
{
    if (!hist) return;

    Double_t mean = hist->GetMean();
    Double_t sigma = hist->GetRMS();
    Double_t min = hist->GetXaxis()->GetXmin();
    Double_t max = hist->GetXaxis()->GetXmax();

    RooRealVar x("x", "x", min, max);
    RooDataHist data("data", "dataset", x, hist);

    RooRealVar mu("mu", "mean", mean, mean - 3*sigma, mean + 3*sigma);
    RooRealVar sig("sig", "sigma", sigma, 0.1*sigma, 3*sigma);
    RooGaussian gauss("gauss", "gaussian", x, mu, sig);

    RooFitResult* result = gauss.fitTo(data, RooFit::Save(), RooFit::PrintLevel(-1));

    RooPlot* frame = x.frame();
    data.plotOn(frame);
    gauss.plotOn(frame, RooFit::LineColor(color));
    frame->Draw("same");

    std::cout << "\n=== RooFit Gaussian Fit Results ===" << std::endl;
    result->Print();
    std::cout << "Mean: " << mu.getVal() << " ± " << mu.getError() << std::endl;
    std::cout << "Sigma: " << sig.getVal() << " ± " << sig.getError() << std::endl;
    std::cout << "===================================\n" << std::endl;
}

// ============================================================================
// Print canvas information
// ============================================================================
void PlotManager::PrintCanvasInfo(TCanvas* canvas)
{
    if (!canvas) return;

    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              Canvas Information                            ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Canvas Name:   " << canvas->GetName() << std::endl;
    std::cout << "║ Title:         " << canvas->GetTitle() << std::endl;
    std::cout << "║ Dimensions:    " << canvas->GetWw() << " x " << canvas->GetWh() << " pixels" << std::endl;
    
    Int_t numPads = 1;
    if (canvas->GetListOfPrimitives()) {
        TIter next(canvas->GetListOfPrimitives());
        TObject* obj;
        while ((obj = next())) {
            if (obj->InheritsFrom(TPad::Class())) numPads++;
        }
    }
    std::cout << "║ Number of Pads: " << numPads << std::endl;

    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                  Objects in Canvas                         ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;

    if (canvas->GetListOfPrimitives()) {
        TIter next(canvas->GetListOfPrimitives());
        TObject* obj;
        Int_t count = 1;
        while ((obj = next())) {
            std::cout << "║ [" << count++ << "] " 
                     << obj->ClassName() << ": " 
                     << obj->GetName();
            
            if (obj->InheritsFrom(TH1::Class())) {
                TH1* hist = (TH1*)obj;
                std::cout << " (Entries: " << hist->GetEntries() << ")";
            }
            std::cout << std::endl;
        }
    }

    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}
