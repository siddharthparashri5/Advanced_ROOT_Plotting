#include "FileHandler.h"
#include "AdvancedPlotGUI.h"
#include "CSVPreviewDialog.h"
#include "RootDataInspector.h"
#include "DataReader.h"

#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TGClient.h>
#include <TSystem.h>
#include <TBrowser.h>
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
// Load ROOT file with TBrowser
// ============================================================================
void FileHandler::LoadRootFile(const char* filepath)
{
    // Close previous file
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
        fCurrentRootFile = nullptr;
    }

    fCurrentRootFile = TFile::Open(filepath, "READ");
    if (!fCurrentRootFile || fCurrentRootFile->IsZombie()) {
        new TGMsgBox(gClient->GetRoot(), nullptr,
            "Error", Form("Cannot open ROOT file:\n%s", filepath),
            kMBIconStop, kMBOk);
        fCurrentRootFile = nullptr;
        return;
    }

    // Open in TBrowser for exploration
    new TBrowser("browser", fCurrentRootFile);

    new TGMsgBox(gClient->GetRoot(), fMainGUI,
        "ROOT File Loaded",
        "ROOT file opened in TBrowser.\n\n"
        "You can:\n"
        "• Drag histograms/graphs from TBrowser onto the main window to plot them\n"
        "• Double-click objects in TBrowser to view them\n"
        "• Explore the file structure\n\n"
        "Tip: Drag objects from TBrowser window to the main GUI window!",
        kMBIconAsterisk, kMBOk);
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