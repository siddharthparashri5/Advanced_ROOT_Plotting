#include <TGMsgBox.h>

#include "AdvancedPlotGUI.h"
#include "DataReader.h"
#include "ColumnSelector.h"
#include "PlotTypes.h"
#include "FitUtils.h"
#include "ErrorHandling.h"
#include "RootDataInspector.h"
#include "CSVPreviewDialog.h"
#include "DropTextEntry.h"

#include <TGClient.h>
#include <TGButton.h>
#include <TGFileDialog.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGLayout.h>
#include <TGListBox.h>
#include <TGTextEdit.h>
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TGMsgBox.h>
#include <TGTextView.h>
#include <TGTextBuffer.h>
#include <TGScrollBar.h>
#include <TGFrame.h>
#include <TGTab.h>
#include <TPython.h>
#include <TObjString.h>

#include <TApplication.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <TFile.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooGaussian.h>
#include <RooPlot.h>
#include <RooFitResult.h>
#include <TH1.h>
#include <iostream>
#include <vector>
#include <string>
#include <TRint.h>
#include <TObject.h>

#include <fstream>
#include <cstdlib>
#include <sstream>


ClassImp(AdvancedPlotGUI);

/////// Constructor ////////

AdvancedPlotGUI::AdvancedPlotGUI(const TGWindow* p, UInt_t w, UInt_t h) 
    : TGMainFrame(p, w, h), currentRootFile(nullptr),
      coutBuf(nullptr), cerrBuf(nullptr), oldCoutBuf(nullptr), oldCerrBuf(nullptr) {

    fMainFrame = this;
    
   
    /////////////////////////////////
    TGGroupFrame* fileGroup = new TGGroupFrame(this, "Data File");
    TGHorizontalFrame* fileFrame = new TGHorizontalFrame(fileGroup);

    // Create text buffer and DropTextEntry (supports drag & drop)
    TGTextBuffer* buffer = new TGTextBuffer(512);
    fileEntry = new DropTextEntry(fileFrame, buffer, this);
    
    fileEntry->SetToolTipText("Drag & drop a file here or type a path");

    // Add entry to layout
    fileFrame->AddFrame(fileEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));

    // Browse button
    browseButton = new TGTextButton(fileFrame, "Browse...", kBrowseButton);
    browseButton->Associate(this);
    fileFrame->AddFrame(browseButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    // Add the file frame to the group and main frame
    fileGroup->AddFrame(fileFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
    AddFrame(fileGroup, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
    
    //loadButton = new TGTextButton(fileFrame, "Load", kLoadButton);
    //loadButton->Associate(this);
    //fileFrame->AddFrame(loadButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    // Plot configuration section
    TGGroupFrame* plotGroup = new TGGroupFrame(this, "Plot Configuration");
    
    addPlotButton = new TGTextButton(plotGroup, "Add Plot...", kAddPlotButton);
    addPlotButton->Associate(this);
    plotGroup->AddFrame(addPlotButton, new TGLayoutHints(kLHintsCenterX, 5,5,5,5));
    addPlotButton->SetEnabled(false);
    
    plotListBox = new TGListBox(plotGroup);
    plotListBox->Resize(100, 100);
    plotGroup->AddFrame(plotListBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));
    
    TGHorizontalFrame* plotButtonFrame = new TGHorizontalFrame(plotGroup);
    removePlotButton = new TGTextButton(plotButtonFrame, "Remove Selected", kRemovePlotButton);
    removePlotButton->Associate(this);
    clearAllButton = new TGTextButton(plotButtonFrame, "Clear All", kClearAllButton);
    clearAllButton->Associate(this);
    plotButtonFrame->AddFrame(removePlotButton, new TGLayoutHints(kLHintsCenterX, 5,5,2,2));
    plotButtonFrame->AddFrame(clearAllButton, new TGLayoutHints(kLHintsCenterX, 5,5,2,2));
    plotGroup->AddFrame(plotButtonFrame, new TGLayoutHints(kLHintsCenterX, 5,5,5,5));
    
    AddFrame(plotGroup, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));

    // Canvas title
    TGHorizontalFrame* titleFrame = new TGHorizontalFrame(this);
    TGLabel* titleLabel = new TGLabel(titleFrame, "Canvas Title:");
    titleFrame->AddFrame(titleLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    canvasTitleEntry = new TGTextEntry(titleFrame);
    canvasTitleEntry->SetText("hist");
    canvasTitleEntry->Resize(300, 20);
    titleFrame->AddFrame(canvasTitleEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));
    AddFrame(titleFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));

   // Canvas division + overlay on one horizontal line
    TGHorizontalFrame* divFrame = new TGHorizontalFrame(this);
    // Overlay checkbox
    sameCanvasCheck = new TGCheckButton(divFrame, "Same canvas (Overlay)");
    divFrame->AddFrame(sameCanvasCheck, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    // Divide canvas checkbox
    dividedCanvasCheck = new TGCheckButton(divFrame, "Divide Canvas:");
    divFrame->AddFrame(dividedCanvasCheck, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));

    // Rows
    divFrame->AddFrame(new TGLabel(divFrame, "Rows:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,2,2,2));
    nRowsEntry = new TGNumberEntry(divFrame, 2, 3, -1,
                               TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEAPositive,
                               TGNumberFormat::kNELLimitMinMax, 1, 10);
    nRowsEntry->Resize(50, 20);
    divFrame->AddFrame(nRowsEntry, new TGLayoutHints(kLHintsLeft, 2,5,2,2));
    // Cols
    divFrame->AddFrame(new TGLabel(divFrame, "Cols:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,2,2,2));
    nColsEntry = new TGNumberEntry(divFrame, 2, 3, -1,
                               TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEAPositive,
                               TGNumberFormat::kNELLimitMinMax, 1, 10);
    nColsEntry->Resize(50, 20);
    divFrame->AddFrame(nColsEntry, new TGLayoutHints(kLHintsLeft, 2,5,2,2));
    // Add this single horizontal frame to the main GUI
    AddFrame(divFrame, new TGLayoutHints(kLHintsLeft, 5,5,5,5));

    
    TGHorizontalFrame* fitFrame = new TGHorizontalFrame(this);
    // Fit label + combo
    fitFrame->AddFrame(new TGLabel(fitFrame, "Fit Function:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
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

    // Custom function label + entry
    fitFrame->AddFrame(new TGLabel(fitFrame, "Custom Fit:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    customFuncEntry = new TGTextEntry(fitFrame);
    customFuncEntry->SetText("[0]*x^2 + [1]*x + [2]");
    customFuncEntry->Resize(200, 20);
    fitFrame->AddFrame(customFuncEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));

    // Add single horizontal frame to main GUI
    AddFrame(fitFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));


    // Plot button
    plotButton = new TGTextButton(this, "Create Plots", kPlotButton);
    plotButton->Associate(this);
    AddFrame(plotButton, new TGLayoutHints(kLHintsCenterX, 5,5,10,10));
    plotButton->SetEnabled(false);

    /////////////////////////////////////////////////////////////////////////
    // ---------- Script Panel with Terminal Styling ----------
    /////////////////////////////////////////////////////////////////////////

    TGGroupFrame* scriptGroup = new TGGroupFrame(this, "Script Panel");
    AddFrame(scriptGroup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));

    TGHorizontalFrame* scriptControlFrame = new TGHorizontalFrame(scriptGroup);

    TGLabel* langLabel = new TGLabel(scriptControlFrame, "Language:");
    scriptControlFrame->AddFrame(langLabel,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));

    scriptLangCombo = new TGComboBox(scriptControlFrame);
    scriptLangCombo->AddEntry("ROOT/C++", 0);
    scriptLangCombo->AddEntry("Python", 1);
    scriptLangCombo->Select(0);
    scriptLangCombo->Resize(120, 20);
    scriptControlFrame->AddFrame(scriptLangCombo,new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    loadScriptButton = new TGTextButton(scriptControlFrame, "Load Script", kLoadScriptButton);
    loadScriptButton->Associate(this);
    scriptControlFrame->AddFrame(loadScriptButton,new TGLayoutHints(kLHintsLeft, 10,5,2,2));

    saveScriptButton = new TGTextButton(scriptControlFrame, "Save Script", kSaveScriptButton);
    saveScriptButton->Associate(this);
    scriptControlFrame->AddFrame(saveScriptButton,new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    scriptGroup->AddFrame(scriptControlFrame,new TGLayoutHints(kLHintsLeft, 5,5,5,5));

    TGVerticalFrame* editorFrame = new TGVerticalFrame(scriptGroup);

    TGLabel* editorLabel = new TGLabel(editorFrame, "Script Editor:");
    editorFrame->AddFrame(editorLabel,new TGLayoutHints(kLHintsLeft, 5,5,5,2));

    scriptEditor = new TGTextEdit(editorFrame, 400, 300);

    // Terminal styling
    scriptEditor->SetBackgroundColor(0x000000);  // Black
    scriptEditor->SetForegroundColor(0x00FF00);  // Green

    editorFrame->AddFrame(scriptEditor,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,2,5));
    
    TGHorizontalFrame* editorButtonFrame = new TGHorizontalFrame(editorFrame);
    runScriptButton = new TGTextButton(editorButtonFrame, "Run Script", kRunScriptButton);
    runScriptButton->Associate(this);
    editorButtonFrame->AddFrame(runScriptButton,new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    clearEditorButton = new TGTextButton(editorButtonFrame, "Clear Editor", kClearEditorButton);
    clearEditorButton->Associate(this);
    editorButtonFrame->AddFrame(clearEditorButton,new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    editorFrame->AddFrame(editorButtonFrame,new TGLayoutHints(kLHintsLeft, 5,5,2,5));

    scriptGroup->AddFrame(editorFrame,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));

    TGVerticalFrame* commandFrame = new TGVerticalFrame(scriptGroup);

    TGLabel* cmdLabel = new TGLabel(commandFrame,"Command Line (press Enter to execute):");
    commandFrame->AddFrame(cmdLabel,new TGLayoutHints(kLHintsLeft, 5,5,5,2));
    commandEntry = new TGTextEntry(commandFrame);
    commandEntry->Resize(400, 20);
    commandEntry->Associate(this);

    // Terminal styling
    commandEntry->SetBackgroundColor(0x000000);
    commandEntry->SetForegroundColor(0x00FF00);
    commandFrame->AddFrame(commandEntry,new TGLayoutHints(kLHintsExpandX, 5,5,2,2));
    runCommandButton = new TGTextButton(commandFrame,"Execute Command", kRunCommandButton);
    runCommandButton->Associate(this);
    commandFrame->AddFrame(runCommandButton,new TGLayoutHints(kLHintsLeft, 5,5,2,5));

    scriptGroup->AddFrame(commandFrame,new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
  
    /////////////////////////////////////////////////////////////////////////
    

    SetWindowName("Advanced Plot GUI");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
    Layout();

    //// Setup stream redirection
    //coutBuf  = new TextEditStreamBuf(outputView);
    //cerrBuf  = new TextEditStreamBuf(outputView);
    //oldCoutBuf = std::cout.rdbuf(coutBuf);
    //oldCerrBuf = std::cerr.rdbuf(cerrBuf);

    // IMPORTANT: Process events first to ensure widget is ready
    //gSystem->ProcessEvents();
    
    // Small delay to ensure GUI is fully initialized
    gSystem->Sleep(100);
    
    gSystem->ProcessEvents();
}

// ==========================
// Destructor
// ==========================
AdvancedPlotGUI::~AdvancedPlotGUI() {
    /*
    // Not needed if we're not redirecting streams
    if (oldCoutBuf) std::cout.rdbuf(oldCoutBuf);
    if (oldCerrBuf) std::cerr.rdbuf(oldCerrBuf);
    
    delete coutBuf;
    delete cerrBuf;
    */
    
    if (currentRootFile && currentRootFile->IsOpen()) {
        currentRootFile->Close();
        delete currentRootFile;
    }
}

void AdvancedPlotGUI::PrintWelcome() {
    std::cout << "=== Advanced ROOT Plotting GUI ===" << std::endl;
    std::cout << "Output window ready. Commands will appear here." << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << std::endl;
    std::cout.flush();
    gSystem->ProcessEvents();
}

void AdvancedPlotGUI::AppendToOutput(const std::string& text) {
    if (!outputView) return;
    
    TGText* txt = outputView->GetText();
    if (!txt) return;
    
    // Get end position
    TGLongPosition pos(txt->RowCount(), 0);
    
    // Insert text
    txt->InsText(pos, text.c_str());
    
    // Update widget
    outputView->Layout();
    outputView->Update();
    
    // Scroll to bottom
    outputView->SetVsbPosition(txt->RowCount());
    
    gSystem->ProcessEvents();
}

/////// Browse for file  ////////

void AdvancedPlotGUI::DoBrowse() {
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
        fileEntry->SetText(fi.fFilename);
    }
}



/////// Load data file ///////

void AdvancedPlotGUI::DoLoad() {
    const char* filename = fileEntry->GetText();
    if (!filename || strlen(filename) == 0) {
        ShowWarning(this, "No File", "Please select a file first.");
        return;
    }
    
    // Check if it's a ROOT file
    std::string filenameStr(filename);
    size_t dotPos = filenameStr.find_last_of('.');
    std::string ext = (dotPos != std::string::npos) ? filenameStr.substr(dotPos + 1) : "";
    
    // Convert extension to lowercase
    for (char& c : ext) c = std::tolower(c);
    
    if (ext == "root") {
        // For ROOT files, open in TBrowser
        std::cout << "Opening ROOT file in TBrowser: " << filename << std::endl;
        
        // Close previous ROOT file if open
        if (currentRootFile && currentRootFile->IsOpen()) {
            currentRootFile->Close();
            delete currentRootFile;
        }
        
        // Open the ROOT file
        currentRootFile = TFile::Open(filename, "READ");
        
        if (!currentRootFile || currentRootFile->IsZombie()) {
            ShowError(this, "Error", "Failed to open ROOT file.");
            if (currentRootFile) {
                delete currentRootFile;
                currentRootFile = nullptr;
            }
            return;
        }
        
        // Open TBrowser
        new TBrowser("browser", currentRootFile);
        
        ShowInfo(this, "ROOT File Loaded", 
                 "ROOT file opened in TBrowser.\n\n"
                 "You can now:\n"
                 "- Double-click on histograms to view them\n"
                 "- Draw objects to canvases\n"
                 "- Explore the file structure\n\n"
                 "The file will remain open until you close this application.");
        
        // Don't enable plot buttons for ROOT files since we're using TBrowser
        addPlotButton->SetEnabled(false);
        plotButton->SetEnabled(false);
        
    } else {
        // For text/CSV files, use the normal loading process
        if (DataReader::ReadFile(filename, currentData)) {
            addPlotButton->SetEnabled(true);
            plotButton->SetEnabled(true);
            ShowInfo(this, "Success",  Form("Loaded %d columns, %d rows",
                                            currentData.GetNumColumns(),
                                            currentData.GetNumRows()));
        } else {
            ShowError(this, "Error", "Failed to load file. Please check the file format.");
        }
    }
}

void AdvancedPlotGUI::DoLoadFromDrop(const char* path)
{
    if (!path || strlen(path) == 0) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", "No valid file path provided.",
            kMBIconExclamation, kMBOk);
        return;
    }
    
    TString filepath(path);
    
    printf("Loading file from drop: %s\n", path);
    
    // Determine file type and handle accordingly
    if (filepath.EndsWith(".root", TString::kIgnoreCase)) {
        // ─── ROOT FILE ────────────────────────────────────────────────
        if (LoadRootFile(path)) {
            // Success - file loaded
            new TGMsgBox(gClient->GetRoot(), this,
                "Success", Form("Loaded ROOT file: %s", path),
                kMBIconAsterisk,      
                kMBOk);
        } else {
            new TGMsgBox(gClient->GetRoot(), this,
                "Error", Form("Failed to load ROOT file: %s", path),
                kMBIconExclamation, kMBOk);
        }
    } 
    else if (filepath.EndsWith(".csv", TString::kIgnoreCase) ||
             filepath.EndsWith(".txt", TString::kIgnoreCase) ||
             filepath.EndsWith(".dat", TString::kIgnoreCase)) {
        // ─── CSV/TEXT FILE ────────────────────────────────────────────
        ShowCSVPreview(path);
    }
    else {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", 
            "Unsupported file format.\n\n"
            "Supported formats:\n"
            "  - ROOT files (.root)\n"
            "  - CSV files (.csv)\n"
            "  - Text files (.txt, .dat)",
            kMBIconExclamation, kMBOk);
    }
}

// ─── ROOT FILE LOADING ─────────────────────────────────────────────────────

Bool_t AdvancedPlotGUI::LoadRootFile(const char* filepath)
{
    // Close previous file if open
    if (currentRootFile) {
        currentRootFile->Close();
        delete currentRootFile;
        currentRootFile = nullptr;
    }
    
    // Open new ROOT file
    currentRootFile = TFile::Open(filepath, "READ");
    
    if (!currentRootFile || currentRootFile->IsZombie()) {
        printf("Error: Failed to open ROOT file: %s\n", filepath);
        currentRootFile = nullptr;
        return kFALSE;
    }
    
    // Load ROOT file inspection if you have RootDataInspector
    if (rootInspector) {
        if (rootInspector->LoadFile(filepath)) {
            printf("ROOT file loaded successfully: %s\n", filepath);
            return kTRUE;
        }
    }
    
    return kTRUE;
}

// ─── CSV PREVIEW DIALOG ────────────────────────────────────────────────────

void AdvancedPlotGUI::ShowCSVPreview(const char* filepath)
{
    // Create preview dialog
    CSVPreviewDialog* dialog = 
        new CSVPreviewDialog(fMainFrame, filepath);
    
    // Show modal dialog
    // User interacts with delimiter, skip rows, header settings
    // Dialog blocks until user clicks Load or Cancel
    dialog->DoModal();
    
    // Get user's import settings
    char delimiter = dialog->GetDelimiter();
    Int_t skipRows = dialog->GetSkipRows();
    Bool_t useHeader = dialog->UseHeaderRow();
    
    printf("CSV Preview Dialog Results:\n");
    printf("  Delimiter: '%c'\n", delimiter);
    printf("  Skip Rows: %d\n", skipRows);
    printf("  Use Header: %s\n", useHeader ? "Yes" : "No");
    
    // Clean up dialog
    delete dialog;
    
    // Load CSV with the settings
    LoadCSVWithSettings(filepath, delimiter, skipRows, useHeader);
}

// ─── CSV LOADING WITH SETTINGS ────────────────────────────────────────────

void AdvancedPlotGUI::LoadCSVWithSettings(
    const char* filepath,
    char delimiter,
    Int_t skipRows,
    Bool_t useHeader)
{
    printf("Loading CSV file with settings:\n");
    printf("  File: %s\n", filepath);
    printf("  Delimiter: '%c' (ASCII %d)\n", delimiter, (int)delimiter);
    printf("  Skip %d rows\n", skipRows);
    printf("  Header row: %s\n", useHeader ? "Yes" : "No");
    
    // ─── PARSE CSV FILE ───────────────────────────────────────────────
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Error", Form("Cannot open file: %s", filepath),
            kMBIconExclamation, kMBOk);
        return;
    }
    
    currentData.Clear(); 
    
    //currentData.xVals.clear();
    //currentData.yVals.clear();
    //currentData.labels.clear();
    
    std::string line;
    Int_t lineNum = 0;
    
    // Skip the first N rows
    while (lineNum < skipRows && std::getline(file, line)) {
        lineNum++;
    }
    
    // Parse header if requested
    std::vector<std::string> columnNames;
    if (useHeader && std::getline(file, line)) {
        lineNum++;
        
        // Split by delimiter
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            // Trim whitespace
            token.erase(0, token.find_first_not_of(" \t\r\n"));
            token.erase(token.find_last_not_of(" \t\r\n") + 1);
            columnNames.push_back(token);
        }
        
        printf("Columns: ");
        for (const auto& name : columnNames) {
            printf("[%s] ", name.c_str());
        }
        printf("\n");
    }
    
    // Parse data rows
    Int_t dataRows = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        // Split by delimiter
        std::stringstream ss(line);
        std::string token;
        int colIdx = 0;
        
        double x = 0, y = 0;
        
        while (std::getline(ss, token, delimiter) && colIdx < 2) {
            // Trim whitespace
            token.erase(0, token.find_first_not_of(" \t\r\n"));
            token.erase(token.find_last_not_of(" \t\r\n") + 1);
            
            try {
                double val = std::stod(token);
                if (colIdx == 0) x = val;
                else if (colIdx == 1) y = val;
            } catch (...) {
                // Could not parse as number, skip
                continue;
            }
            
            colIdx++;
        }
        
        if (colIdx >= 2) {
            currentData.xVals.push_back(x);
            currentData.yVals.push_back(y);
            dataRows++;
        }
    }
    
    file.close();
    
    printf("Loaded %d data rows from %s\n", dataRows, filepath);
    
    // Show success message
    new TGMsgBox(gClient->GetRoot(), this,
        "Success", Form("Loaded %d rows from %s", dataRows, filepath),
        kMBIconAsterisk, kMBOk);
}


/////// Add plot configuration ///////

void AdvancedPlotGUI::DoAddPlot()
{
    if (currentData.GetNumColumns() == 0) {
        ShowWarning(this, "No Data", "Please load a data file first.");
        return;
    }

    PlotConfig config;
    bool result = false;

    ColumnSelectorDialog* dialog = new ColumnSelectorDialog(
            fMainFrame,
            &currentData,
            &config,
            &result
        );

    gClient->WaitFor(dialog);

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
        case PlotConfig::kTH1F:
            desc = Form("TH1F: %s",
                        currentData.headers[config.xColumn].c_str());
            break;
        case PlotConfig::kTH1I:
            desc = Form("TH1I: %s",
                        currentData.headers[config.xColumn].c_str());
            break;
        case PlotConfig::kTH2D:
            desc = Form("TH2D: %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str());
            break;
        case PlotConfig::kTH2F:
            desc = Form("TH2F: %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str());
            break;
        case PlotConfig::kTH2I:
            desc = Form("TH2I: %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str());
            break;
        case PlotConfig::kTH3D:
            desc = Form("TH3D: %s vs %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str(),
                        currentData.headers[config.zColumn].c_str());
            break;
        case PlotConfig::kTH3F:
            desc = Form("TH3F: %s vs %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str(),
                        currentData.headers[config.zColumn].c_str());
            break;
        case PlotConfig::kTH3I:
            desc = Form("TH3I: %s vs %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str(),
                        currentData.headers[config.zColumn].c_str());
            break;
    }

    plotListBox->AddEntry(desc.c_str(), plotConfigs.size() - 1);
    plotListBox->Layout();
}

/////// Remove selected plot ///////

void AdvancedPlotGUI::DoRemovePlot() {
    int selected = plotListBox->GetSelected();
    if (selected >= 0) {
        plotConfigs.erase(plotConfigs.begin() + selected);
        plotListBox->RemoveEntry(selected);
        plotListBox->Layout();
    }
}

/////// Clear all plots ////////

void AdvancedPlotGUI::DoClearAll() {
    if (plotConfigs.empty()) return;
    
    if (ShowQuestion(this, "Confirm", "Clear all plot configurations?")) {
        plotConfigs.clear();
        plotListBox->RemoveAll();
        plotListBox->Layout();
    }
}

//////// Script Panel Methods /////////

void AdvancedPlotGUI::DoRunScript() {
    if (!scriptEditor) return;

    int lang = scriptLangCombo->GetSelected(); // 0=C++, 1=Python
    TString fullScript = scriptEditor->GetText()->AsString();
    
    if (fullScript.IsNull() || fullScript.IsWhitespace()) {
        std::cout << "Script editor is empty." << std::endl;
        //std::cout.flush();
        //coutBuf->forceFlush();
        gSystem->ProcessEvents();
        return;
    }
    
    //printf("\n=== Executing Script ===\n");
    //std::cout.flush();
    //coutBuf->forceFlush();
    gSystem->ProcessEvents();
    
    // Split into lines
    std::vector<TString> lines;
    TObjArray* tokens = fullScript.Tokenize("\n");
    for (Int_t i = 0; i < tokens->GetEntries(); ++i) {
        TString line = ((TObjString*)tokens->At(i))->GetString();
        line = line.Strip(TString::kBoth);
        if (!line.IsNull() && !line.BeginsWith("//") && !line.BeginsWith("#")) {
            lines.push_back(line);
        }
    }
    delete tokens;
    
    // Execute each line
    for (const auto& line : lines) {
        //std::cout << "> " << line.Data() << std::endl;
        //std::cout.flush();
        //coutBuf->forceFlush();
        gSystem->ProcessEvents();
        
        if (lang == 0) {
            // ROOT/C++
            gROOT->ProcessLine(line);
        } else if (lang == 1) {
            // Python
            TPython::Exec(line);
        }
        
        //std::cout.flush();
        //std::cerr.flush();
        //coutBuf->forceFlush();
        //cerrBuf->forceFlush();
        
        // Update canvases
        TIter nextCanvas(gROOT->GetListOfCanvases());
        TCanvas* c;
        while ((c = (TCanvas*)nextCanvas())) {
            c->Modified();
            c->Update();
        }
        gSystem->ProcessEvents();
    }
    
    std::cout << "=== Script Execution Complete ===" << std::endl;
    std::cout.flush();
    //coutBuf->forceFlush();

    gSystem->ProcessEvents();
}

void AdvancedPlotGUI::DoRunCommand() {
    if (!commandEntry) return;
    
    TString command = commandEntry->GetText();
    command = command.Strip(TString::kBoth);
    
    if (command.IsNull()) return;
    
    int lang = scriptLangCombo->GetSelected();
    
    std::cout << "\n> " << command.Data() << std::endl;
    std::cout.flush();
    coutBuf->forceFlush();
    gSystem->ProcessEvents();
    
    if (lang == 0) {
        // ROOT/C++
        gROOT->ProcessLine(command);
    } else if (lang == 1) {
        // Python
        TPython::Exec(command);
    }
    
    std::cout.flush();
    std::cerr.flush();
    coutBuf->forceFlush();
    cerrBuf->forceFlush();
    
    // Update canvases
    TIter nextCanvas(gROOT->GetListOfCanvases());
    TCanvas* c;
    while ((c = (TCanvas*)nextCanvas())) {
        c->Modified();
        c->Update();
    }
    gSystem->ProcessEvents();
    
    std::cout << std::endl;
    std::cout.flush();
    coutBuf->forceFlush();
    gSystem->ProcessEvents();
    
    // Clear command entry for next command
    commandEntry->Clear();
}

void AdvancedPlotGUI::DoLoadScript() {
    TGFileInfo fi;
    const char* filetypes[] = {
        "C++ files", "*.C",
        "C++ files", "*.cpp",
        "Python files", "*.py",
        "All files", "*",
        0, 0
    };
    fi.fFileTypes = filetypes;

    new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);

    if (fi.fFilename && strlen(fi.fFilename) > 0) {
        std::ifstream file(fi.fFilename);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            
            scriptEditor->LoadFile(fi.fFilename);
            
            // Auto-detect language from extension
            std::string filename(fi.fFilename);
            if (filename.find(".py") != std::string::npos) {
                scriptLangCombo->Select(1); // Python
            } else {
                scriptLangCombo->Select(0); // C++
            }
            
            std::cout << "Loaded script: " << fi.fFilename << std::endl;
        } else {
            ShowError(this, "Error", "Failed to open script file.");
        }
    }
}

void AdvancedPlotGUI::DoSaveScript() {
    TGFileInfo fi;
    const char* filetypes[] = {
        "C++ files", "*.C",
        "Python files", "*.py",
        "All files", "*",
        0, 0
    };
    fi.fFileTypes = filetypes;

    new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fi);

    if (fi.fFilename && strlen(fi.fFilename) > 0) {
        scriptEditor->SaveFile(fi.fFilename);
        std::cout << "Saved script to: " << fi.fFilename << std::endl;
    }
}

void AdvancedPlotGUI::DoClearEditor() {
    if (scriptEditor) {
        scriptEditor->Clear();
        std::cout << "Script editor cleared." << std::endl;
    }
}

void AdvancedPlotGUI::DoClearOutput() {
    if (outputView) {
        outputView->Clear();
        gSystem->ProcessEvents();
    }
}

void AdvancedPlotGUI::DoTestOutput() {
    std::cout << "\n=== TEST OUTPUT ===" << std::endl;
    std::cout << "If you can see this, stream redirection is working!" << std::endl;
    //std::cout << "Current time: " << gSystem->Now() << std::endl;
    std::cout << "Widget pointer: " << (void*)outputView << std::endl;
    std::cout << "Text object: " << (void*)outputView->GetText() << std::endl;
    
    TGText* txt = outputView->GetText();
    if (txt) {
        std::cout << "Text rows: " << txt->RowCount() << std::endl;
    }
    
    std::cout << "===================" << std::endl;
    std::cout.flush();
    coutBuf->forceFlush();
    gSystem->ProcessEvents();
}


//////// Unified Fit Handler /////////

void ApplyFit(
    TObject* obj,
    FitUtils::FitType fitType,
    int color,
    const std::string& customFunc
) {
    if (fitType == FitUtils::kNoFit || !obj) return;

    // ---------- RooFit Gaussian for Histograms ----------
    if (fitType == FitUtils::kGaus && obj->InheritsFrom(TH1::Class())) {
        TH1* h = static_cast<TH1*>(obj);

        RooRealVar x("x", "x", h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());

        RooDataHist data("data", "data", x, h);

        RooRealVar mean("mean", "Mean", h->GetMean());
        RooRealVar sigma("sigma", "Sigma", h->GetRMS());

        RooGaussian gauss("gauss", "Gaussian", x, mean, sigma);
        gauss.fitTo(data, RooFit::PrintLevel(-1));

        RooPlot* frame = x.frame();
        data.plotOn(frame);
        gauss.plotOn(frame);
        frame->Draw("SAME");

        std::cout << "[RooFit] Mean=" << mean.getVal()
                  << " Sigma=" << sigma.getVal() << std::endl;
        return;
    }

    // ---------- TF1 Fits (Graphs + Hists) ----------
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


/////// Print Canvas/Pad Information ///////

void AdvancedPlotGUI::PrintCanvasInfo(TCanvas* canvas) {
    if (!canvas) return;
    
    // Use printf - always goes to terminal, ignores stream redirection

    printf("\n╔═════════════════════════════════════════════╗\n");
    printf("║          Canvas/Plot Information            ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("Canvas: %s\n", canvas->GetName());
    printf("  - Title: %s\n", canvas->GetTitle());
    printf("  - Size: %d x %d pixels\n", canvas->GetWw(), canvas->GetWh());
    printf("  - Number of primitives: %d\n", canvas->GetListOfPrimitives()->GetSize());
    
    TPad* pad = (TPad*)gPad;
    if (pad) {
        printf("  - Current pad: %s\n", pad->GetName());
        printf("\nCurrent Pad Properties:\n");
        printf("  - Log X: %s\n", pad->GetLogx() ? "Yes" : "No");
        printf("  - Log Y: %s\n", pad->GetLogy() ? "Yes" : "No");
        printf("  - Log Z: %s\n", pad->GetLogz() ? "Yes" : "No");
        printf("  - Grid X: %s\n", pad->GetGridx() ? "Yes" : "No");
        printf("  - Grid Y: %s\n", pad->GetGridy() ? "Yes" : "No");
        printf("  - Tick X: %d\n", pad->GetTickx());
        printf("  - Tick Y: %d\n", pad->GetTicky());
        printf("  - Margins: L=%.2f R=%.2f T=%.2f B=%.2f\n", 
               pad->GetLeftMargin(), pad->GetRightMargin(),
               pad->GetTopMargin(), pad->GetBottomMargin());
        printf("  - Fill Color: %d\n", pad->GetFillColor());
        printf("  - Fill Style: %d\n", pad->GetFillStyle());
        
        // List primitives
        TList* prims = pad->GetListOfPrimitives();
        if (prims && prims->GetSize() > 0) {
            printf("\nObjects in current pad:\n");
            TIter next(prims);
            TObject* obj;
            int count = 0;
            while ((obj = next()) && count < 10) {
                printf("  [%d] %s - %s\n", count++, obj->ClassName(), obj->GetName());
            }
            if (prims->GetSize() > 10) {
                printf("  ... and %d more objects\n", prims->GetSize() - 10);
            }
        }
    }
    
    printf("\nUseful Commands for ROOT Prompt:\n");
    printf("  gPad->SetLogy(1);          // Log Y scale\n");
    printf("  gPad->SetGridx(1);         // X grid\n");
    printf("  gPad->SetGridy(1);         // Y grid\n");
    printf("  gPad->SetFillColor(10);    // Background\n");
    printf("  %s->cd();\n", canvas->GetName());
    printf("  %s->SaveAs(\"plot.pdf\");\n", canvas->GetName());
    printf("╚═════════════════════════════════════════════╝\n");
    fflush(stdout);  // Force immediate output to terminal
}


/////////////////////////////////////////


/////// Create plots ///////

void AdvancedPlotGUI::DoPlot() {
    if (plotConfigs.empty()) {
        ShowWarning(this, "No Plots", "Please add at least one plot configuration first.");
        return;
    }
    
    gROOT->cd();
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
    legend = new TLegend(0.65, 0.65, 0.89, 0.89);
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    
    // Helper lambda for RooFit Gaussian
    auto applyRooFitGaussian = [&](TH1* h, int color) {
        if (!h) return;
        h->Sumw2();
        for (int i = 1; i <= h->GetNbinsX(); ++i) {
            if (h->GetBinError(i) <= 0) h->SetBinError(i, 1.0);
        }

        RooRealVar x("x", "x", h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
        RooDataHist data("data", "data", x, h);

        double meanInit = h->GetMean();
        double rmsInit  = h->GetRMS();
        RooRealVar mean("mean", "Mean", meanInit, h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());
        RooRealVar sigma("sigma", "Sigma", rmsInit > 0 ? rmsInit : 1.0, 1e-3, h->GetXaxis()->GetXmax()-h->GetXaxis()->GetXmin());

        RooGaussian gauss("gauss", "Gaussian", x, mean, sigma);
        gauss.fitTo(data, RooFit::PrintLevel(-1), RooFit::Save(true));

        RooPlot* frame = x.frame();
        data.plotOn(frame);
        gauss.plotOn(frame);
        frame->Draw("SAME");

        // Stats box
        TPaveText* pt = new TPaveText(0.65,0.6,0.9,0.8,"NDC");
        pt->SetFillColor(0);
        pt->SetBorderSize(1);
        pt->SetTextAlign(12);
        pt->SetTextSize(0.03);
        pt->AddText("Fit: Gaussian (RooFit)");
        pt->AddText(Form("Mean = %.3f", mean.getVal()));
        pt->AddText(Form("Sigma = %.3f", sigma.getVal()));
        pt->Draw("SAME");
    };
    
    //////  SAME CANVAS MODE /////////
    
    if (sameCanvas) {
    canvas = new TCanvas("c_same", canvasTitle.c_str(), 900, 700);
    
    // Create legend with better positioning
    legend = new TLegend(0.70, 0.70, 0.95, 0.92);
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    legend->SetFillStyle(1001);
    legend->SetTextSize(0.03);
    legend->SetHeader(canvasTitle.c_str(), "C");
    
    bool firstPlot = true;

    for (size_t i = 0; i < plotConfigs.size(); ++i) {
        PlotConfig& config = plotConfigs[i];
        config.color = (i % 9) + 1;
        if (config.color == 0) config.color = 1;  // Avoid white

        if (config.type == PlotConfig::kTGraph) {
            TGraph* g = PlotCreator::CreateTGraph(currentData, config);
            if (g) {
                g->SetTitle(canvasTitle.c_str());
                g->Draw(firstPlot ? "APL" : "PL SAME");
                gROOT->GetListOfGlobals()->Add(g);
                
                // Better legend entry
                std::string legendLabel = Form("%s", currentData.headers[config.yColumn].c_str());
                legend->AddEntry(g, legendLabel.c_str(), "lp");
                
                ApplyFit(g, fitType, config.color, customFunc);
                firstPlot = false;
            }
        } else if (config.type == PlotConfig::kTGraphErrors) {
            TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
            if (g) {
                g->SetTitle(canvasTitle.c_str());
                g->Draw(firstPlot ? "APE" : "PE SAME");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s", currentData.headers[config.yColumn].c_str());
                legend->AddEntry(g, legendLabel.c_str(), "lpe");
                
                ApplyFit(g, fitType, config.color, customFunc);
                firstPlot = false;
            }
        } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
            TH1* h = PlotCreator::CreateTH1(currentData, config);
            if (h) {
                h->SetTitle(canvasTitle.c_str());
                h->SetLineColor(config.color);
                h->SetLineWidth(2);
                h->Draw(firstPlot ? "HIST" : "HIST SAME");
                gROOT->GetListOfGlobals()->Add(h);
                
                std::string legendLabel = Form("%s", currentData.headers[config.xColumn].c_str());
                legend->AddEntry(h, legendLabel.c_str(), "l");
                
                firstPlot = false;

                if (fitType == FitUtils::kGaus) {
                    applyRooFitGaussian(h, config.color);
                } else {
                    ApplyFit(h, fitType, config.color, customFunc);
                }
            }
        } else if (config.type == PlotConfig::kTH2D || 
                   config.type == PlotConfig::kTH2F || 
                   config.type == PlotConfig::kTH2I) {
            TH2* h = PlotCreator::CreateTH2(currentData, config);
            if (h) {
                h->SetTitle(canvasTitle.c_str());
                h->Draw(firstPlot ? "COLZ" : "COLZ SAME");
                gROOT->GetListOfGlobals()->Add(h);
                
                std::string legendLabel = Form("%s vs %s", 
                    currentData.headers[config.xColumn].c_str(),
                    currentData.headers[config.yColumn].c_str());
                legend->AddEntry(h, legendLabel.c_str(), "f");
                
                firstPlot = false;
                ApplyFit(h, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(currentData, config);
            if (h) {
                h->SetTitle(canvasTitle.c_str());
                h->Draw(firstPlot ? "ISO" : "ISO SAME");
                gROOT->GetListOfGlobals()->Add(h);
                
                std::string legendLabel = Form("%s vs %s vs %s", 
                    currentData.headers[config.xColumn].c_str(),
                    currentData.headers[config.yColumn].c_str(),
                    currentData.headers[config.zColumn].c_str());
                legend->AddEntry(h, legendLabel.c_str(), "f");
                
                firstPlot = false;
            }
        }
    }
    
    // Draw legend if it has entries
    if (legend->GetNRows() > 1) {  // More than just header
        legend->Draw();
    }
    
    canvas->Update();

    // AUTO-PRINT CANVAS INFO
    PrintCanvasInfo(canvas);
    }
    
    /////// DIVIDED CANVAS MODE  ////////
    else if (dividedCanvas) {
    canvas = new TCanvas("c_divided", canvasTitle.c_str(), 1200, 900);
    canvas->Divide(nCols, nRows);

    int padNum = 1;
    for (size_t i = 0; i < plotConfigs.size(); ++i) {
        PlotConfig& config = plotConfigs[i];
        config.color = (i % 9) + 1;
        if (config.color == 0) config.color = 1;
        
        if (padNum > nRows * nCols) break;
        canvas->cd(padNum++);
        
        // Create legend for this pad
        TLegend* padLegend = new TLegend(0.65, 0.75, 0.92, 0.92);
        padLegend->SetBorderSize(1);
        padLegend->SetFillColor(0);
        padLegend->SetTextSize(0.04);

        if (config.type == PlotConfig::kTGraph) {
            TGraph* g = PlotCreator::CreateTGraph(currentData, config);
            if (g) {
                g->Draw("APL");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s", currentData.headers[config.yColumn].c_str());
                padLegend->AddEntry(g, legendLabel.c_str(), "lp");
                
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTGraphErrors) {
            TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
            if (g) {
                g->Draw("APE");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s", currentData.headers[config.yColumn].c_str());
                padLegend->AddEntry(g, legendLabel.c_str(), "lpe");
                
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
            TH1* h = PlotCreator::CreateTH1(currentData, config);
            if (h) {
                h->Draw();
                gROOT->GetListOfGlobals()->Add(h);
                
                std::string legendLabel = Form("%s", currentData.headers[config.xColumn].c_str());
                padLegend->AddEntry(h, legendLabel.c_str(), "l");
                
                if (fitType == FitUtils::kGaus) {
                    applyRooFitGaussian(h, config.color);
                } else {
                    ApplyFit(h, fitType, config.color, customFunc);
                }
            }
        } else if (config.type == PlotConfig::kTH2D || 
                   config.type == PlotConfig::kTH2F || 
                   config.type == PlotConfig::kTH2I) {
            TH2* h = PlotCreator::CreateTH2(currentData, config);
            if (h) {
                h->Draw("COLZ");
                gROOT->GetListOfGlobals()->Add(h);
                
                // TH2 doesn't usually need legend in divided mode
                // but we can add title
                std::string legendLabel = Form("%s vs %s", 
                    currentData.headers[config.xColumn].c_str(),
                    currentData.headers[config.yColumn].c_str());
                padLegend->AddEntry(h, legendLabel.c_str(), "f");
                
                ApplyFit(h, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(currentData, config);
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

    // AUTO-PRINT CANVAS INFO
    PrintCanvasInfo(canvas);
    }

    ////// SEPARATE CANVAS MODE ////////
    else {
    for (size_t i = 0; i < plotConfigs.size(); ++i) {
        PlotConfig& config = plotConfigs[i];
        config.color = (i % 9) + 1;
        if (config.color == 0) config.color = 1;

        TCanvas* c = new TCanvas(Form("c%zu", i), Form("%s - %zu", canvasTitle.c_str(), i), 800, 600);
        
        // Create legend for this canvas
        TLegend* canvasLegend = new TLegend(0.70, 0.75, 0.92, 0.92);
        canvasLegend->SetBorderSize(1);
        canvasLegend->SetFillColor(0);
        canvasLegend->SetTextSize(0.03);

        if (config.type == PlotConfig::kTGraph) {
            TGraph* g = PlotCreator::CreateTGraph(currentData, config);
            if (g) {
                g->Draw("APL");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s vs %s", 
                    currentData.headers[config.xColumn].c_str(),
                    currentData.headers[config.yColumn].c_str());
                canvasLegend->AddEntry(g, legendLabel.c_str(), "lp");
                
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTGraphErrors) {
            TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
            if (g) {
                g->Draw("APE");
                gROOT->GetListOfGlobals()->Add(g);
                
                std::string legendLabel = Form("%s vs %s", 
                    currentData.headers[config.xColumn].c_str(),
                    currentData.headers[config.yColumn].c_str());
                canvasLegend->AddEntry(g, legendLabel.c_str(), "lpe");
                
                ApplyFit(g, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
            TH1* h = PlotCreator::CreateTH1(currentData, config);
            if (h) {
                h->Draw();
                gROOT->GetListOfGlobals()->Add(h);
                
                std::string legendLabel = Form("%s", currentData.headers[config.xColumn].c_str());
                canvasLegend->AddEntry(h, legendLabel.c_str(), "l");
                
                if (fitType == FitUtils::kGaus) {
                    applyRooFitGaussian(h, config.color);
                } else {
                    ApplyFit(h, fitType, config.color, customFunc);
                }
            }
        } else if (config.type == PlotConfig::kTH2D || 
                   config.type == PlotConfig::kTH2F || 
                   config.type == PlotConfig::kTH2I) {
            TH2* h = PlotCreator::CreateTH2(currentData, config);
            if (h) {
                h->Draw("COLZ");
                gROOT->GetListOfGlobals()->Add(h);
                ApplyFit(h, fitType, config.color, customFunc);
            }
        } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(currentData, config);
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
        
        // AUTO-PRINT CANVAS INFO for each canvas
        PrintCanvasInfo(c);
    }
    }
    gSystem->ProcessEvents();
    ShowInfo(this, "Plot Created", 
                 "Check the Plot Info in the terminal.\n\n");
}
/////// Process messages ///////

Bool_t AdvancedPlotGUI::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) {
    switch (GET_MSG(msg)) {
        case kC_COMMAND:
            switch (GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == kBrowseButton) {
                        DoBrowse();
                        DoLoad();
                        const char* filepath = fileEntry->GetText();
                        if (filepath && strlen(filepath) > 0) {
                            DoLoadFromDrop(filepath);  // Reuse drop handler
                        } else {
                            new TGMsgBox(gClient->GetRoot(), this,
                                "Error", "Please enter a file path or drag a file.",
                                kMBIconExclamation, kMBOk);
                        }
                    } else if (parm1 == kLoadButton) {
                        //DoLoad();
                    } else if (parm1 == kAddPlotButton) {
                        DoAddPlot();
                    } else if (parm1 == kRemovePlotButton) {
                        DoRemovePlot();
                    } else if (parm1 == kClearAllButton) {
                        DoClearAll();
                    } else if (parm1 == kPlotButton) {
                        DoPlot();
                    } else if (parm1 == kRunScriptButton) {
                        DoRunScript();
                    } else if (parm1 == kRunCommandButton) {
                        DoRunCommand();
                    } else if (parm1 == kLoadScriptButton) {
                        DoLoadScript();
                    } else if (parm1 == kSaveScriptButton) {
                        DoSaveScript();
                    } else if (parm1 == kClearEditorButton) {
                        DoClearEditor();
                    } else if (parm1 == kClearOutputButton) {
                        DoClearOutput();
                    }
                    break;
            }
            break;
        case kC_TEXTENTRY:
            switch (GET_SUBMSG(msg)) {
                case kTE_ENTER:
                    // Handle Enter key press in command entry
                    if (commandEntry && parm1 == (Long_t)commandEntry->WidgetId()) {
                        DoRunCommand();
                    }
                    break;
            }
            break;
    }
     return TGMainFrame::ProcessMessage(msg, parm1, parm2);
}

//////// Main /////////

int main(int argc, char** argv) {

    // -------- Batch Mode --------
    if (argc >= 3 && std::string(argv[1]) == "--batch") {
        ColumnData data;
        if (!DataReader::ReadFile(argv[2], data)) {
            std::cerr << "Failed to read file\n";
            return 1;
        }

        PlotConfig cfg;
        cfg.type = PlotConfig::kTH1D;
        cfg.xColumn = 0;
        cfg.bins = 100;

        TCanvas c("batch", "Batch Plot", 800, 600);
        TH1* h = PlotCreator::CreateTH1D(data, cfg);
        if (h) {
            h->Draw();
            c.SaveAs("batch_output.png");
            c.SaveAs("batch_output.pdf");
        }
        return 0;
    }

    // -------- GUI + ROOT Prompt Mode --------
    TRint app("AdvancedPlotApp", &argc, argv);

    new AdvancedPlotGUI(gClient->GetRoot(), 400, 200);

    app.Run();   // ROOT prompt + GUI
    return 0;
}