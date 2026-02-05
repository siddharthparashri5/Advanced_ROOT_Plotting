#ifndef PLOTTYPES_H
#define PLOTTYPES_H

#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH2D.h>
#include <TH2F.h>
#include <TH3.h>
#include <TH3D.h>
#include <TH3F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <vector>
#include <string>
#include "DataReader.h"
#include "FitUtils.h"

//////////////////////////////
// Plot configuration
//////////////////////////////
struct PlotConfig {
    enum PlotType {
        kTGraph,
        kTGraphErrors,
        kTH1D,
        kTH2D,
        kTH3D
    };
    
    PlotType type;
    int xColumn;
    int yColumn;
    int zColumn;
    int xErrColumn;  // -1 if not used
    int yErrColumn;  // -1 if not used
    int zErrColumn;  // -1 if not used
    int weightColumn;
    std::string title;
    std::string xTitle;
    std::string yTitle;
    std::string zTitle;
    int bins;
    int color;
    int markerStyle;
    
    
    
    PlotConfig() : type(kTGraph), xColumn(-1), yColumn(-1), zColumn(-1), 
                   xErrColumn(-1), yErrColumn(-1), zErrColumn(-1), weightColumn(-1), bins(100), 
                   color(kBlue), markerStyle(20) {}
};

//////////////////////////////
// PlotCreator class
//////////////////////////////
class PlotCreator {
public:
    
    // Create TGraph from column data
    static TGraph* CreateTGraph(const ColumnData& data, const PlotConfig& config) {
        if (config.xColumn < 0 || config.yColumn < 0 ||
            config.xColumn >= data.GetNumColumns() ||
            config.yColumn >= data.GetNumColumns()) {
            std::cerr << "Invalid column indices for TGraph" << std::endl;
            return nullptr;
        }
        
        const auto& xData = data.data[config.xColumn];
        const auto& yData = data.data[config.yColumn];
        int nPoints = std::min(xData.size(), yData.size());
        
        if (nPoints == 0) return nullptr;
        
        TGraph* graph = new TGraph(nPoints, xData.data(), yData.data());
        
        // Use column headers for axis titles if available, otherwise use config titles
        std::string xTitle = data.headers[config.xColumn].empty() ? config.xTitle : data.headers[config.xColumn];
        std::string yTitle = data.headers[config.yColumn].empty() ? config.yTitle : data.headers[config.yColumn];
        
        graph->SetTitle(Form("%s;%s;%s", config.title.c_str(), xTitle.c_str(), yTitle.c_str()));
        graph->GetXaxis()->SetTitle(xTitle.c_str());
        graph->GetYaxis()->SetTitle(yTitle.c_str());
        graph->SetLineColor(config.color);
        graph->SetMarkerColor(config.color);
        graph->SetMarkerStyle(config.markerStyle);
        graph->SetLineWidth(2);
        
        return graph;
    }
    
    // Create TGraphErrors from column data
    static TGraphErrors* CreateTGraphErrors(const ColumnData& data, const PlotConfig& config) {
        if (config.xColumn < 0 || config.yColumn < 0 ||
            config.xColumn >= data.GetNumColumns() ||
            config.yColumn >= data.GetNumColumns()) {
            std::cerr << "Invalid column indices for TGraphErrors" << std::endl;
            return nullptr;
        }
        
        const auto& xData = data.data[config.xColumn];
        const auto& yData = data.data[config.yColumn];
        int nPoints = std::min(xData.size(), yData.size());
        
        if (nPoints == 0) return nullptr;
        
        TGraphErrors* graph = nullptr;
        
        // Check which errors are provided
        bool hasXErr = (config.xErrColumn >= 0 && config.xErrColumn < data.GetNumColumns());
        bool hasYErr = (config.yErrColumn >= 0 && config.yErrColumn < data.GetNumColumns());
        
        if (hasXErr && hasYErr) {
            const auto& xErr = data.data[config.xErrColumn];
            const auto& yErr = data.data[config.yErrColumn];
            nPoints = std::min({nPoints, (int)xErr.size(), (int)yErr.size()});
            graph = new TGraphErrors(nPoints, xData.data(), yData.data(), 
                                    xErr.data(), yErr.data());
        } else if (hasXErr) {
            const auto& xErr = data.data[config.xErrColumn];
            nPoints = std::min(nPoints, (int)xErr.size());
            std::vector<double> zeroErr(nPoints, 0.0);
            graph = new TGraphErrors(nPoints, xData.data(), yData.data(),
                                    xErr.data(), zeroErr.data());
        } else if (hasYErr) {
            const auto& yErr = data.data[config.yErrColumn];
            nPoints = std::min(nPoints, (int)yErr.size());
            std::vector<double> zeroErr(nPoints, 0.0);
            graph = new TGraphErrors(nPoints, xData.data(), yData.data(),
                                    zeroErr.data(), yErr.data());
        } else {
            // No errors - create regular TGraphErrors with zero errors
            std::vector<double> zeroErr(nPoints, 0.0);
            graph = new TGraphErrors(nPoints, xData.data(), yData.data(),
                                    zeroErr.data(), zeroErr.data());
        }
        
        // Use column headers for axis titles if available
        std::string xTitle = data.headers[config.xColumn].empty() ? config.xTitle : data.headers[config.xColumn];
        std::string yTitle = data.headers[config.yColumn].empty() ? config.yTitle : data.headers[config.yColumn];
        
        graph->SetTitle(Form("%s;%s;%s", config.title.c_str(), xTitle.c_str(), yTitle.c_str()));
        graph->GetXaxis()->SetTitle(xTitle.c_str());
        graph->GetYaxis()->SetTitle(yTitle.c_str());
        graph->SetLineColor(config.color);
        graph->SetMarkerColor(config.color);
        graph->SetMarkerStyle(config.markerStyle);
        graph->SetLineWidth(2);
        
        return graph;
    }
    
    // Create TH1D from column data
    // NOTE: This returns TH1* to support TH1D, TH1F, etc.
    static TH1* CreateTH1D(const ColumnData& data, const PlotConfig& config,
                           int nBins = 100, double xMin = 0, double xMax = 0) {
        if (config.xColumn < 0 || config.xColumn >= data.GetNumColumns()) {
            std::cerr << "Invalid column index for TH1D" << std::endl;
            return nullptr;
        }
        
        const auto& xData = data.data[config.xColumn];
        if (xData.empty()) return nullptr;
        
        // Auto-calculate range if not provided
        if (xMin == 0 && xMax == 0) {
            xMin = *std::min_element(xData.begin(), xData.end());
            xMax = *std::max_element(xData.begin(), xData.end());
            double range = xMax - xMin;
            xMin -= 0.1 * range;
            xMax += 0.1 * range;
        }
        
        // Use column header for x-axis title
        std::string xTitle = data.headers[config.xColumn].empty() ? config.xTitle : data.headers[config.xColumn];
        
        TH1D* hist = new TH1D(Form("h1_%p", (void*)&config), 
                             Form("%s;%s;Entries", config.title.c_str(), xTitle.c_str()),
                             nBins, xMin, xMax);
        
        for (double val : xData) {
            hist->Fill(val);
        }
        
        hist->SetLineColor(config.color);
        hist->SetLineWidth(2);
        
        return hist;
    }
    
    // Create TH2D from column data
    // NOTE: This returns TH2* to support TH2D, TH2F, etc.
    static TH2* CreateTH2D(const ColumnData& data, const PlotConfig& config,
                           int nBinsX = 50, int nBinsY = 50,
                           double xMin = 0, double xMax = 0,
                           double yMin = 0, double yMax = 0) {
        if (config.xColumn < 0 || config.yColumn < 0 ||
            config.xColumn >= data.GetNumColumns() ||
            config.yColumn >= data.GetNumColumns()) {
            std::cerr << "Invalid column indices for TH2D" << std::endl;
            return nullptr;
        }
        
        const auto& xData = data.data[config.xColumn];
        const auto& yData = data.data[config.yColumn];
        int nPoints = std::min(xData.size(), yData.size());
        
        if (nPoints == 0) return nullptr;
        
        // Auto-calculate ranges if not provided
        if (xMin == 0 && xMax == 0) {
            xMin = *std::min_element(xData.begin(), xData.end());
            xMax = *std::max_element(xData.begin(), xData.end());
            double range = xMax - xMin;
            xMin -= 0.1 * range;
            xMax += 0.1 * range;
        }
        
        if (yMin == 0 && yMax == 0) {
            yMin = *std::min_element(yData.begin(), yData.end());
            yMax = *std::max_element(yData.begin(), yData.end());
            double range = yMax - yMin;
            yMin -= 0.1 * range;
            yMax += 0.1 * range;
        }
        
        // Use column headers for axis titles
        std::string xTitle = data.headers[config.xColumn].empty() ? config.xTitle : data.headers[config.xColumn];
        std::string yTitle = data.headers[config.yColumn].empty() ? config.yTitle : data.headers[config.yColumn];
        
        TH2D* hist = new TH2D(Form("h2_%p", (void*)&config), 
                             Form("%s;%s;%s", config.title.c_str(), xTitle.c_str(), yTitle.c_str()),
                             nBinsX, xMin, xMax, nBinsY, yMin, yMax);
        
        for (int i = 0; i < nPoints; ++i) {
            hist->Fill(xData[i], yData[i]);
        }
        
        return hist;
    }
    
    // Create TH3D from column data, with optional weights
    // NOTE: This returns TH3* to support TH3D, TH3F, etc.
    static TH3* CreateTH3D(const ColumnData& data, const PlotConfig& config,
                        int nBinsX = 50, int nBinsY = 50, int nBinsZ = 50,
                        double xMin = 0, double xMax = 0,
                        double yMin = 0, double yMax = 0,
                        double zMin = 0, double zMax = 0) {
        // Check column indices
        if (config.xColumn < 0 || config.yColumn < 0 || config.zColumn < 0 ||
            config.xColumn >= data.GetNumColumns() ||
            config.yColumn >= data.GetNumColumns() ||
            config.zColumn >= data.GetNumColumns() ||
            (config.weightColumn >= data.GetNumColumns() && config.weightColumn != -1)) 
        {
            std::cerr << "Invalid column indices for TH3D" << std::endl;
            return nullptr;
        }

        const auto& xData = data.data[config.xColumn];
        const auto& yData = data.data[config.yColumn];
        const auto& zData = data.data[config.zColumn];
        const auto* wData = (config.weightColumn >= 0) ? &data.data[config.weightColumn] : nullptr;
    
        int nPoints = std::min({xData.size(), yData.size(), zData.size()});
        if (wData) nPoints = std::min(nPoints, (int)wData->size());
    
        if (nPoints == 0) return nullptr;
    
        // Auto-calculate ranges if not provided
        if (xMin == 0 && xMax == 0) {
            xMin = *std::min_element(xData.begin(), xData.end());
            xMax = *std::max_element(xData.begin(), xData.end());
            double range = xMax - xMin;
            xMin -= 0.1 * range;
            xMax += 0.1 * range;
        }
    
        if (yMin == 0 && yMax == 0) {
            yMin = *std::min_element(yData.begin(), yData.end());
            yMax = *std::max_element(yData.begin(), yData.end());
            double range = yMax - yMin;
            yMin -= 0.1 * range;
            yMax += 0.1 * range;
        }
    
        if (zMin == 0 && zMax == 0) {
            zMin = *std::min_element(zData.begin(), zData.end());
            zMax = *std::max_element(zData.begin(), zData.end());
            double range = zMax - zMin;
            zMin -= 0.1 * range;
            zMax += 0.1 * range;
        }
    
        // Use column headers for axis titles
        std::string xTitle = data.headers[config.xColumn].empty() ? config.xTitle : data.headers[config.xColumn];
        std::string yTitle = data.headers[config.yColumn].empty() ? config.yTitle : data.headers[config.yColumn];
        std::string zTitle = data.headers[config.zColumn].empty() ? config.zTitle : data.headers[config.zColumn];
    
        TH3D* hist = new TH3D(Form("h3_%p", (void*)&config),
                              Form("%s;%s;%s;%s", config.title.c_str(), xTitle.c_str(), yTitle.c_str(), zTitle.c_str()),
                              nBinsX, xMin, xMax, nBinsY, yMin, yMax, nBinsZ, zMin, zMax);
    
        for (int i = 0; i < nPoints; ++i) {
            if (wData) {
                hist->Fill(xData[i], yData[i], zData[i], (*wData)[i]);
            } else {
                hist->Fill(xData[i], yData[i], zData[i]);
            }
        }

        return hist;
    }

};

#endif // PLOTTYPES_H