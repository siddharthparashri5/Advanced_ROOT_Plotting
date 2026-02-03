#ifndef FITUTILS_H
#define FITUTILS_H

#include <TObject.h>
#include <TMath.h>
#include <iostream>
#include <cstring>
#include <TF1.h>
#include <TGraph.h>
#include <TFitResult.h>
#include <TPaveText.h>
#include <string>
#include <map>
#include <TH1.h>


// RooFit (optional, only used if Gaussian + histogram)
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooGaussian.h>
#include <RooPlot.h>

//////////////////////////////
// Fit Function Definitions
//////////////////////////////
class FitUtils {
public:
    enum FitType {
        kNoFit = 0,
        kGaus,
        kLinear,
        kPol0,
        kPol1,
        kPol2,
        kPol3,
        kPol4,
        kExpo,
        kSine,
        kSineOffset,
        kDampedSine,
        kCustom
    };
    
    // Map of fit type to function string and name
    static std::map<FitType, std::pair<std::string, std::string>> GetFitFunctions() {
        std::map<FitType, std::pair<std::string, std::string>> fitMap;
        fitMap[kNoFit] = {"", "No Fit"};
        fitMap[kGaus] = {"gaus", "Gaussian"};
        fitMap[kLinear] = {"pol1", "Linear"};
        fitMap[kPol0] = {"pol0", "Pol0 (Constant)"};
        fitMap[kPol1] = {"pol1", "Pol1 (Linear)"};
        fitMap[kPol2] = {"pol2", "Pol2 (Quadratic)"};
        fitMap[kPol3] = {"pol3", "Pol3 (Cubic)"};
        fitMap[kPol4] = {"pol4", "Pol4 (Quartic)"};
        fitMap[kExpo] = {"expo", "Exponential"};
        fitMap[kSine] = {"[0]*sin([1]*x+[2])", "Sine Wave"};
        fitMap[kSineOffset] = {"[0]*sin([1]*x+[2])+[3]", "Sine + Offset"};
        fitMap[kDampedSine] = {"[0]*exp(-[1]*x)*sin([2]*x+[3])+[4]", "Damped Sine"};
        fitMap[kCustom] = {"", "Custom Function"};
        return fitMap;
    }
    
    // Perform fit on a graph
    static TF1* FitGraph(TGraph* graph, FitType fitType, int color, 
                        const std::string& customFunc = "") {
        if (fitType == kNoFit || !graph) return nullptr;
        
        auto fitMap = GetFitFunctions();
        std::string fitFunc = fitMap[fitType].first;
        
        // Use custom function if provided
        if (fitType == kCustom) {
            if (customFunc.empty()) {
                std::cerr << "Custom fit selected but no function provided!" << std::endl;
                return nullptr;
            }
            fitFunc = customFunc;
        }
        
        if (fitFunc.empty()) return nullptr;
        
        // Get x-axis range and estimate parameters for periodic functions
        Double_t xmin, xmax, ymin, ymax;
        graph->GetPoint(0, xmin, ymin);
        xmax = xmin;
        ymin = ymin;
        ymax = ymin;
        
        for (int i = 0; i < graph->GetN(); i++) {
            Double_t x, y;
            graph->GetPoint(i, x, y);
            if (x < xmin) xmin = x;
            if (x > xmax) xmax = x;
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
        }
        
        // Create and configure fit function
        TF1* fit = new TF1(Form("fit_%s_%p", fitFunc.c_str(), (void*)graph), 
                          fitFunc.c_str(), xmin, xmax);
        fit->SetLineColor(color);
        fit->SetLineWidth(2);
        fit->SetLineStyle(2);  // dashed line
        
        // Set initial parameters for sinusoidal fits
        if (fitType == kSine || fitType == kSineOffset || fitType == kDampedSine) {
            Double_t amplitude = (ymax - ymin) / 2.0;
            Double_t offset = (ymax + ymin) / 2.0;
            Double_t period = (xmax - xmin) / 3.0;  // Estimate ~3 periods in range
            Double_t frequency = 2.0 * TMath::Pi() / period;
            
            if (fitType == kSine) {
                // [0]*sin([1]*x+[2])
                fit->SetParameter(0, amplitude);     // Amplitude
                fit->SetParameter(1, frequency);     // Angular frequency
                fit->SetParameter(2, 0);             // Phase
                fit->SetParName(0, "Amplitude");
                fit->SetParName(1, "Frequency");
                fit->SetParName(2, "Phase");
            } else if (fitType == kSineOffset) {
                // [0]*sin([1]*x+[2])+[3]
                fit->SetParameter(0, amplitude);     // Amplitude
                fit->SetParameter(1, frequency);     // Angular frequency
                fit->SetParameter(2, 0);             // Phase
                fit->SetParameter(3, offset);        // Offset
                fit->SetParName(0, "Amplitude");
                fit->SetParName(1, "Frequency");
                fit->SetParName(2, "Phase");
                fit->SetParName(3, "Offset");
            } else if (fitType == kDampedSine) {
                // [0]*exp(-[1]*x)*sin([2]*x+[3])+[4]
                fit->SetParameter(0, amplitude);     // Initial amplitude
                fit->SetParameter(1, 0.01);          // Damping factor
                fit->SetParameter(2, frequency);     // Angular frequency
                fit->SetParameter(3, 0);             // Phase
                fit->SetParameter(4, offset);        // Offset
                fit->SetParName(0, "Amplitude");
                fit->SetParName(1, "Damping");
                fit->SetParName(2, "Frequency");
                fit->SetParName(3, "Phase");
                fit->SetParName(4, "Offset");
            }
        }
        
        // Perform the fit
        graph->Fit(fit, "Q");  // Q for quiet mode
        
        return fit;
    }
    
    // Create a text box with fit parameters
    static TPaveText* CreateFitStatsBox(TF1* fit, FitType fitType, 
                                       double x1, double y1, double x2, double y2) {
        if (!fit) return nullptr;
        
        TPaveText* pt = new TPaveText(x1, y1, x2, y2, "NDC");
        pt->SetFillColor(0);
        pt->SetBorderSize(1);
        pt->SetTextAlign(12);
        pt->SetTextSize(0.03);
        
        auto fitMap = GetFitFunctions();
        pt->AddText(Form("Fit: %s", fitMap[fitType].second.c_str()));
        
        // Add chi-square and NDF
        pt->AddText(Form("#chi^{2}/NDF = %.2f / %d", 
                        fit->GetChisquare(), fit->GetNDF()));
        
        // Add parameters based on fit type
        switch(fitType) {
            case kGaus:
                pt->AddText(Form("Constant = %.3e #pm %.3e", 
                               fit->GetParameter(0), fit->GetParError(0)));
                pt->AddText(Form("Mean = %.3e #pm %.3e", 
                               fit->GetParameter(1), fit->GetParError(1)));
                pt->AddText(Form("Sigma = %.3e #pm %.3e", 
                               fit->GetParameter(2), fit->GetParError(2)));
                break;
            case kExpo:
                pt->AddText(Form("p0 = %.3e #pm %.3e", 
                               fit->GetParameter(0), fit->GetParError(0)));
                pt->AddText(Form("p1 = %.3e #pm %.3e", 
                               fit->GetParameter(1), fit->GetParError(1)));
                break;
            case kSine:
                pt->AddText(Form("Amplitude = %.3e #pm %.3e", 
                               fit->GetParameter(0), fit->GetParError(0)));
                pt->AddText(Form("Frequency = %.3e #pm %.3e", 
                               fit->GetParameter(1), fit->GetParError(1)));
                pt->AddText(Form("Phase = %.3e #pm %.3e", 
                               fit->GetParameter(2), fit->GetParError(2)));
                break;
            case kSineOffset:
                pt->AddText(Form("Amplitude = %.3e #pm %.3e", 
                               fit->GetParameter(0), fit->GetParError(0)));
                pt->AddText(Form("Frequency = %.3e #pm %.3e", 
                               fit->GetParameter(1), fit->GetParError(1)));
                pt->AddText(Form("Phase = %.3e #pm %.3e", 
                               fit->GetParameter(2), fit->GetParError(2)));
                pt->AddText(Form("Offset = %.3e #pm %.3e", 
                               fit->GetParameter(3), fit->GetParError(3)));
                break;
            case kDampedSine:
                pt->AddText(Form("Amplitude = %.3e #pm %.3e", 
                               fit->GetParameter(0), fit->GetParError(0)));
                pt->AddText(Form("Damping = %.3e #pm %.3e", 
                               fit->GetParameter(1), fit->GetParError(1)));
                pt->AddText(Form("Frequency = %.3e #pm %.3e", 
                               fit->GetParameter(2), fit->GetParError(2)));
                pt->AddText(Form("Phase = %.3e #pm %.3e", 
                               fit->GetParameter(3), fit->GetParError(3)));
                pt->AddText(Form("Offset = %.3e #pm %.3e", 
                               fit->GetParameter(4), fit->GetParError(4)));
                break;
            case kCustom:
                // For custom fits, just show all parameters
                for (int i = 0; i < fit->GetNpar(); i++) {
                    const char* parName = fit->GetParName(i);
                    if (parName && strlen(parName) > 0) {
                        pt->AddText(Form("%s = %.3e #pm %.3e", parName,
                                       fit->GetParameter(i), fit->GetParError(i)));
                    } else {
                        pt->AddText(Form("p%d = %.3e #pm %.3e", i,
                                       fit->GetParameter(i), fit->GetParError(i)));
                    }
                }
                break;
            case kLinear:
            case kPol0:
            case kPol1:
            case kPol2:
            case kPol3:
            case kPol4:
                for (int i = 0; i < fit->GetNpar(); i++) {
                    pt->AddText(Form("p%d = %.3e #pm %.3e", i,
                                   fit->GetParameter(i), fit->GetParError(i)));
                }
                break;
            default:
                break;
        }
        
        return pt;
    }
    
    // Perform fit on a histogram using TF1
	static TF1* FitHist(TH1* hist, FitType fitType, int color,
                    const std::string& customFunc = "") {
    	if (!hist || fitType == kNoFit) return nullptr;

    	auto fitMap = GetFitFunctions();
    	std::string fitFunc = fitMap[fitType].first;

	    if (fitType == kCustom) {
    	    if (customFunc.empty()) {
    	        std::cerr << "Custom fit selected but no function provided!" << std::endl;
    	        return nullptr;
    	    }
    	    fitFunc = customFunc;
    	}

    	if (fitFunc.empty()) return nullptr;

    	TF1* fit = new TF1(
    	    Form("fit_%s_%p", fitFunc.c_str(), (void*)hist),
    	    fitFunc.c_str(),
    	    hist->GetXaxis()->GetXmin(),
    	    hist->GetXaxis()->GetXmax()
    	);

    	fit->SetLineColor(color);
    	fit->SetLineWidth(2);
    	fit->SetLineStyle(2);

		for (int i = 1; i <= hist->GetNbinsX(); i++) {
		    hist->SetBinError(i, 0);
		}

    	hist->Fit(fit, "RQ");  // R = range, Q = quiet
    	return fit;
	}

	// RooFit Gaussian for histograms with stats box
	static void FitHistRooGaussian(TH1* hist, int color = kRed) {
	    if (!hist) return;

    	// Define RooRealVar over histogram range
	    RooRealVar x("x", "x",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());

	    // Import histogram into RooDataHist	
    	RooDataHist data("data", "data", x, hist);

    	// Initial mean and sigma
    	RooRealVar mean("mean", "Mean", hist->GetMean());
    	RooRealVar sigma("sigma", "Sigma", hist->GetRMS());

    	// Create Gaussian PDF
    	RooGaussian gauss("gauss", "Gaussian", x, mean, sigma);
	
    	// Fit Gaussian to data quietly
    	gauss.fitTo(data, RooFit::PrintLevel(-1), RooFit::Extended(false), RooFit::Save(true));
	
    	// Create RooPlot frame and plot
    	RooPlot* frame = x.frame();
    	data.plotOn(frame);
    	gauss.plotOn(frame, RooFit::LineColor(color), RooFit::LineWidth(2));

    	// Draw the frame on the current canvas
    	frame->Draw("SAME");

    	// Add stats box
    	TPaveText* pt = new TPaveText(0.65, 0.65, 0.9, 0.85, "NDC");
    	pt->SetFillColor(0);
    	pt->SetBorderSize(1);
    	pt->SetTextAlign(12);
    	pt->SetTextSize(0.03);
    	pt->AddText("RooFit Gaussian");
    	pt->AddText(Form("Mean  = %.3f", mean.getVal()));
    	pt->AddText(Form("Sigma = %.3f", sigma.getVal()));
    	pt->Draw();

    	// Force canvas update
    	gPad->Update();
	
    	// Optional: print to console
    	std::cout << "[RooFit] Mean=" << mean.getVal() << " Sigma=" << sigma.getVal() << std::endl;
	}

	
	// Unified fit interface for Graphs and Histograms
	static void ApplyFit( TObject* obj, FitUtils::FitType fitType,int color,const std::string& customFunc = "") {
	    if (!obj || fitType == FitUtils::kNoFit) return;

    	// For histograms, use RooFit Gaussian if requested
    	if (fitType == FitUtils::kGaus && obj->InheritsFrom(TH1::Class())) {
    	    TH1* h = static_cast<TH1*>(obj);
	
    	    // Ensure no zero errors
    	    h->Sumw2();  // enable proper errors
    	    for (int i = 1; i <= h->GetNbinsX(); ++i) {
    	        if (h->GetBinError(i) <= 0)
    	            h->SetBinError(i, 1.0);  // small default error
    	    }
	
    	    // Create RooFit variables with safe ranges
    	    RooRealVar x("x", "x", h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
    	    RooDataHist data("data", "data", x, h);
		
    	    double meanInit = h->GetMean();
    	    double rmsInit = h->GetRMS();
    	    RooRealVar mean("mean", "Mean", meanInit, h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
    	    RooRealVar sigma("sigma", "Sigma", rmsInit > 0 ? rmsInit : 1.0, 1e-3, h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin());
	
    	    RooGaussian gauss("gauss", "Gaussian", x, mean, sigma);
    	    gauss.fitTo(data, RooFit::PrintLevel(-1), RooFit::Extended(false), RooFit::Save(true));
	
    	    RooPlot* frame = x.frame();
    	    data.plotOn(frame);
    	    gauss.plotOn(frame);

    	    frame->Draw("SAME");
	
    	    std::cout << "[RooFit] Mean=" << mean.getVal() << " Sigma=" << sigma.getVal() << std::endl;
	
	        return;
    	}

    	// TF1-based fits
    	if (obj->InheritsFrom(TGraph::Class())) {
    	    TGraph* g = static_cast<TGraph*>(obj);
    	    TF1* f = FitUtils::FitGraph(g, fitType, color, customFunc);
    	    if (f) f->Draw("SAME");
    	}
    	else if (obj->InheritsFrom(TH1::Class())) {
    	    TH1* h = static_cast<TH1*>(obj);
    	    TF1* f = FitUtils::FitHist(h, fitType, color, customFunc);
    	    if (f) f->Draw("SAME");
    	}
	}	


};


#endif // FITUTILS_H
