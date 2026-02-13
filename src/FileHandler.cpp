#include "FileHandler.h"
#include "AdvancedPlotGUI.h"
#include "CSVPreviewDialog.h"
#include "ROOTFileBrowser.h"
#include "RootDataInspector.h"
#include "DataReader.h"

#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TGClient.h>
#include <TSystem.h>
#include <TBrowser.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <fstream>
#include <sstream>

// ============================================================================
// Constructor
// ============================================================================
FileHandler::FileHandler(AdvancedPlotGUI* mainGUI)
    : fMainGUI(mainGUI),
      fCurrentRootFile(nullptr)
{
}

// ============================================================================
// Destructor
// ============================================================================
FileHandler::~FileHandler()
{
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
        fCurrentRootFile = nullptr;
    }
}

// ============================================================================
// Browse for file
// ============================================================================
std::string FileHandler::Browse()
{
    const char* filetypes[] = {
        "All files", "*",
        "ROOT files", "*.root",
        "CSV files", "*.csv",
        "Text files", "*.txt",
        "Data files", "*.dat",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    
    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDOpen, &fileInfo);
    
    if (fileInfo.fFilename && strlen(fileInfo.fFilename) > 0) {
        fMainGUI->SetFilePath(fileInfo.fFilename);
        return std::string(fileInfo.fFilename);
    }
    
    return "";
}

// ============================================================================
// Load file based on type
// ============================================================================
void FileHandler::Load(const std::string& filepath)
{
    if (filepath.empty()) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", "Please enter a file path.",
            kMBIconExclamation, kMBOk);
        return;
    }

    // Check if ROOT file
    TString filename(filepath.c_str());
    if (filename.EndsWith(".root")) {
        LoadRootFile(filepath.c_str());
        return;
    }

    // Check if CSV file - show preview dialog
    if (filename.EndsWith(".csv")) {
        LoadCSVFile(filepath.c_str());
        return;
    }

    // Load other text data using DataReader
    if (!DataReader::ReadFile(filepath, fCurrentData)) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", "Failed to load data file. Check console for details.",
            kMBIconStop, kMBOk);
        return;
    }

    fMainGUI->EnablePlotControls(true);

    new TGMsgBox(gClient->GetRoot(), fMainGUI,
        "Success", Form("Data loaded successfully!\nRows: %d\nColumns: %d",
            fCurrentData.GetNumRows(), fCurrentData.GetNumColumns()),
        kMBIconAsterisk, kMBOk);
}

// ============================================================================
// Load ROOT file with content browser
// ============================================================================
void FileHandler::LoadRootFile(const char* filepath)
{
    // Close previous file
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
        fCurrentRootFile = nullptr;
    }

    // Open the browser dialog
    ROOTFileBrowser* browser = new ROOTFileBrowser(gClient->GetRoot(), filepath);
    
    Int_t ret = browser->DoModal();
    
    if (ret == 0) {
        // User cancelled or error
        gSystem->ProcessEvents();
        gSystem->Sleep(100);
        delete browser;
        return;
    }
    
    Bool_t showBrowser = browser->ShowBrowser();
    std::vector<ROOTObjectInfo> selectedObjects = browser->GetSelectedObjects();
    
    // Clean up dialog
    gSystem->ProcessEvents();
    gSystem->Sleep(100);
    delete browser;
    
    // Open the file for our use
    fCurrentRootFile = TFile::Open(filepath, "READ");
    if (!fCurrentRootFile || fCurrentRootFile->IsZombie()) {
        new TGMsgBox(gClient->GetRoot(), nullptr,
            "Error", Form("Cannot open ROOT file:\n%s", filepath),
            kMBIconStop, kMBOk);
        fCurrentRootFile = nullptr;
        return;
    }
    
    // If user wants TBrowser, open it
    if (showBrowser || ret == 2) {
        new TBrowser("browser", fCurrentRootFile);
    }
    
    // Handle selected objects
    if (ret == 1 && !selectedObjects.empty()) {
        std::cout << "\n=== Loading " << selectedObjects.size() << " objects ===" << std::endl;
        
        for (const auto& objInfo : selectedObjects) {
            std::cout << "Loading: " << objInfo.name << " (" << objInfo.type << ")" << std::endl;
            
            // Get the object from file
            TObject* obj = fCurrentRootFile->Get(objInfo.name.c_str());
            if (!obj) {
                std::cout << "  WARNING: Could not retrieve object!" << std::endl;
                continue;
            }
            
            // Handle different object types
            if (objInfo.category == "Histogram") {
                PlotHistogram(obj, objInfo.name.c_str());
            } else if (objInfo.category == "Graph") {
                PlotGraph(obj, objInfo.name.c_str());
            } else if (objInfo.category == "Tree") {
                ShowTreeInfo(obj, objInfo.name.c_str());
            }
        }
        
        std::cout << "=== Loading complete ===" << std::endl;
    }
    
    // Show summary message
    if (ret == 1) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "ROOT File Loaded",
            Form("Loaded %d objects from ROOT file.\n\n"
                 "Objects have been plotted in separate canvases.",
                 (int)selectedObjects.size()),
            kMBIconAsterisk, kMBOk);
    }
}

// ============================================================================
// Load CSV file with preview dialog
// ============================================================================
void FileHandler::LoadCSVFile(const char* filepath)
{
    CSVPreviewDialog* preview = new CSVPreviewDialog(gClient->GetRoot(), filepath);
    
    Int_t ret = preview->DoModal();
    
    if (ret != 1) {
        // User cancelled
        gSystem->ProcessEvents();
        gSystem->Sleep(100);
        delete preview;
        return;
    }
    
    char    delimiter = preview->GetDelimiter();
    Int_t   skipRows  = preview->GetSkipRows();
    Bool_t  useHeader = preview->UseHeaderRow();
    
    // Clean up dialog before proceeding
    gSystem->ProcessEvents();
    gSystem->Sleep(100);
    delete preview;
    
    LoadCSVWithSettings(filepath, delimiter, skipRows, useHeader);
}

// ============================================================================
// Load CSV with user-specified settings
// ============================================================================
void FileHandler::LoadCSVWithSettings(const char* filepath, char delim, 
                                      Int_t skipRows, Bool_t useHeader)
{
    // CRITICAL: Clear old data first
    fCurrentData = ColumnData();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        printf("ERROR: Cannot open file!\n");
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", Form("Cannot open: %s", filepath),
            kMBIconStop, kMBOk);
        return;
    }
    
    std::string line;
    int lineNum = 0;

    // Skip rows
    while (lineNum < skipRows && std::getline(file, line)) {
        lineNum++;
    }

    // Header row
    if (useHeader && std::getline(file, line)) {
        lineNum++;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, delim)) {
            // Trim whitespace
            token.erase(0, token.find_first_not_of(" \t\r\n"));
            token.erase(token.find_last_not_of(" \t\r\n") + 1);
            if (!token.empty() && token.back() == '\r') token.pop_back();
            fCurrentData.headers.push_back(token);
        }
        
        if (!fCurrentData.headers.empty()) {
            fCurrentData.data.resize(fCurrentData.headers.size());
        }
    }

    // Data rows
    int dataRowCount = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, delim)) {
            token.erase(0, token.find_first_not_of(" \t\r\n"));
            token.erase(token.find_last_not_of(" \t\r\n") + 1);
            if (!token.empty() && token.back() == '\r') token.pop_back();
            tokens.push_back(token);
        }
        
        // First data row — create default headers if not using header row
        if (fCurrentData.data.empty() && !tokens.empty()) {
            fCurrentData.data.resize(tokens.size());
            if (fCurrentData.headers.empty()) {
                for (size_t i = 0; i < tokens.size(); ++i) {
                    fCurrentData.headers.push_back(Form("Col%zu", i));
                }
            }
        }
        
        // Parse numeric values
        size_t numCols = std::min(tokens.size(), fCurrentData.data.size());
        for (size_t i = 0; i < numCols; ++i) {
            try {
                if (!tokens[i].empty()) {
                    double val = std::stod(tokens[i]);
                    fCurrentData.data[i].push_back(val);
                }
            } catch (...) { 
                // Non-numeric value, skip
            }
        }
        dataRowCount++;
    }
    file.close();

    // CRITICAL: Check data validity and enable controls
    bool hasData = !fCurrentData.data.empty() && fCurrentData.GetNumRows() > 0;
    
    if (hasData) {
        fMainGUI->EnablePlotControls(kTRUE);
        
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Success", Form("CSV loaded successfully!\n\nColumns: %d\nRows: %d",
                fCurrentData.GetNumColumns(),
                fCurrentData.GetNumRows()),
            kMBIconAsterisk, kMBOk);
    } else {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Warning", "No numeric data found in file.\n"
                       "Check delimiter and format.",
            kMBIconExclamation, kMBOk);
    }
}

// ============================================================================
// Helper: Plot histogram
// ============================================================================
void FileHandler::PlotHistogram(TObject* obj, const char* name)
{
    if (!obj) return;
    
    TH1* hist = dynamic_cast<TH1*>(obj);
    if (!hist) {
        std::cout << "  WARNING: Object is not a histogram!" << std::endl;
        return;
    }
    
    // Create new canvas
    static int canvasCounter = 0;
    TCanvas* c = new TCanvas(Form("c_hist_%d", canvasCounter++),
                             Form("Histogram: %s", name),
                             800, 600);
    
    hist->Draw();
    c->Update();
    
    std::cout << "  ✓ Histogram plotted in canvas: " << c->GetName() << std::endl;
}

// ============================================================================
// Helper: Plot graph
// ============================================================================
void FileHandler::PlotGraph(TObject* obj, const char* name)
{
    if (!obj) return;
    
    TGraph* graph = dynamic_cast<TGraph*>(obj);
    if (!graph) {
        std::cout << "  WARNING: Object is not a graph!" << std::endl;
        return;
    }
    
    // Create new canvas
    static int canvasCounter = 0;
    TCanvas* c = new TCanvas(Form("c_graph_%d", canvasCounter++),
                             Form("Graph: %s", name),
                             800, 600);
    
    graph->Draw("AP");
    c->Update();
    
    std::cout << "  ✓ Graph plotted in canvas: " << c->GetName() << std::endl;
}

// ============================================================================
// Helper: Show TTree info
// ============================================================================
void FileHandler::ShowTreeInfo(TObject* obj, const char* name)
{
    if (!obj) return;
    
    TTree* tree = dynamic_cast<TTree*>(obj);
    if (!tree) {
        std::cout << "  WARNING: Object is not a TTree!" << std::endl;
        return;
    }
    
    std::cout << "\n=== TTree Information ===" << std::endl;
    std::cout << "Name: " << tree->GetName() << std::endl;
    std::cout << "Title: " << tree->GetTitle() << std::endl;
    std::cout << "Entries: " << tree->GetEntries() << std::endl;
    std::cout << "Branches: " << tree->GetNbranches() << std::endl;
    
    // Show branch info
    TObjArray* branches = tree->GetListOfBranches();
    if (branches) {
        std::cout << "\nBranches:" << std::endl;
        for (int i = 0; i < branches->GetEntries() && i < 10; i++) {
            TBranch* branch = (TBranch*)branches->At(i);
            if (branch) {
                std::cout << "  • " << branch->GetName();
                if (branch->GetClassName()[0]) {
                    std::cout << " [" << branch->GetClassName() << "]";
                }
                std::cout << std::endl;
            }
        }
        if (branches->GetEntries() > 10) {
            std::cout << "  ... and " << (branches->GetEntries() - 10) << " more branches" << std::endl;
        }
    }
    std::cout << "=========================\n" << std::endl;
    
    // Show message box
    new TGMsgBox(gClient->GetRoot(), fMainGUI,
        "TTree Information",
        Form("TTree: %s\n\nEntries: %lld\nBranches: %d\n\n"
             "See console for branch details.\n"
             "Note: TTree plotting from browser not yet implemented.",
             tree->GetName(), tree->GetEntries(), tree->GetNbranches()),
        kMBIconAsterisk, kMBOk);
}