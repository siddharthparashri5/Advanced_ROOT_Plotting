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

#include "AdvancedPlotGUI.h"
#include "DataReader.h"
#include "ColumnSelector.h"
#include "PlotTypes.h"
#include "FitUtils.h"
#include "ErrorHandling.h"

ClassImp(AdvancedPlotGUI);

/////// Constructor ////////

AdvancedPlotGUI::AdvancedPlotGUI(const TGWindow* p, UInt_t w, UInt_t h) 
    : TGMainFrame(p, w, h), currentRootFile(nullptr),
      coutBuf(nullptr), cerrBuf(nullptr), oldCoutBuf(nullptr), oldCerrBuf(nullptr) {

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
    plotListBox->Resize(500, 100);
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

    // Language selection and file operations
    TGHorizontalFrame* scriptControlFrame = new TGHorizontalFrame(scriptGroup);

    TGLabel* langLabel = new TGLabel(scriptControlFrame, "Language:");
    scriptControlFrame->AddFrame(langLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));

    scriptLangCombo = new TGComboBox(scriptControlFrame);
    scriptLangCombo->AddEntry("ROOT/C++", 0);
    scriptLangCombo->AddEntry("Python", 1);
    scriptLangCombo->Select(0);
    scriptLangCombo->Resize(120, 20);
    scriptControlFrame->AddFrame(scriptLangCombo, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    loadScriptButton = new TGTextButton(scriptControlFrame, "Load Script", kLoadScriptButton);
    loadScriptButton->Associate(this);
    scriptControlFrame->AddFrame(loadScriptButton, new TGLayoutHints(kLHintsLeft, 10,5,2,2));

    saveScriptButton = new TGTextButton(scriptControlFrame, "Save Script", kSaveScriptButton);
    saveScriptButton->Associate(this);
    scriptControlFrame->AddFrame(saveScriptButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    scriptGroup->AddFrame(scriptControlFrame, new TGLayoutHints(kLHintsLeft, 5,5,5,5));

    // Create SIDE-BY-SIDE horizontal frame for editor and output
    TGHorizontalFrame* editorOutputFrame = new TGHorizontalFrame(scriptGroup);

    // --- LEFT SIDE: Script Editor ---
    TGVerticalFrame* editorFrame = new TGVerticalFrame(editorOutputFrame);

    TGLabel* editorLabel = new TGLabel(editorFrame, "Script Editor:");
    editorFrame->AddFrame(editorLabel, new TGLayoutHints(kLHintsLeft, 5,5,5,2));

    scriptEditor = new TGTextEdit(editorFrame, 400, 250);

    // Terminal styling - black background, green text ((Select One))
    scriptEditor->SetBackgroundColor(0x000000);  // Black background
    scriptEditor->SetForegroundColor(0x00FF00);  // Green text
    // Modern Dark Theme
    //scriptEditor->SetBackgroundColor(0x1E1E1E);   // Dark gray
    //scriptEditor->SetForegroundColor(0xD4D4D4);   // Light gray
    // Ubbuntu Termianl
    //scriptEditor->SetBackgroundColor(0x2C001E);   // Purple-black
    //scriptEditor->SetForegroundColor(0xFFFFFF);   // White
    
    editorFrame->AddFrame(scriptEditor, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,2,5));

    TGHorizontalFrame* editorButtonFrame = new TGHorizontalFrame(editorFrame);
    runScriptButton = new TGTextButton(editorButtonFrame, "Run Script", kRunScriptButton);
    runScriptButton->Associate(this);
    editorButtonFrame->AddFrame(runScriptButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    clearEditorButton = new TGTextButton(editorButtonFrame, "Clear Editor", kClearEditorButton);
    clearEditorButton->Associate(this);
    editorButtonFrame->AddFrame(clearEditorButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));
    editorFrame->AddFrame(editorButtonFrame, new TGLayoutHints(kLHintsLeft, 5,5,2,5));

    editorOutputFrame->AddFrame(editorFrame, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5,5,5,5));

    // --- RIGHT SIDE: Output View ---
    TGVerticalFrame* outputFrame = new TGVerticalFrame(editorOutputFrame);

    TGLabel* outputLabel = new TGLabel(outputFrame, "Output:");
    outputFrame->AddFrame(outputLabel, new TGLayoutHints(kLHintsLeft, 5,5,5,2));

    outputView = new TGTextEdit(outputFrame, 400, 250);
    outputView->SetReadOnly(kTRUE);

    // Terminal styling - black background, light green/white text (Select One)
    outputView->SetBackgroundColor(0x000000);  // Black background
    outputView->SetForegroundColor(0xCCFFCC);  // Light green text
    // Modern Dark Theme
    //outputView->SetBackgroundColor(0x1E1E1E);
    //outputView->SetForegroundColor(0xCCCCCC);
    // Ubuntu Termianl
    //outputView->SetBackgroundColor(0x2C001E);
    //outputView->SetForegroundColor(0xFFFFFF);

    outputFrame->AddFrame(outputView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,2,5));

    clearOutputButton = new TGTextButton(outputFrame, "Clear Output", kClearOutputButton);
    clearOutputButton->Associate(this);
    outputFrame->AddFrame(clearOutputButton, new TGLayoutHints(kLHintsLeft, 5,5,2,5));

    editorOutputFrame->AddFrame(outputFrame, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5,5,5,5));

    scriptGroup->AddFrame(editorOutputFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));

    // --- BOTTOM: Command Line ---
    TGVerticalFrame* commandFrame = new TGVerticalFrame(scriptGroup);

    TGLabel* cmdLabel = new TGLabel(commandFrame, "Command Line (press Enter to execute):");
    commandFrame->AddFrame(cmdLabel, new TGLayoutHints(kLHintsLeft, 5,5,5,2));

    commandEntry = new TGTextEntry(commandFrame);
    commandEntry->Resize(780, 20);
    commandEntry->Associate(this);
    // Terminal styling for command entry
    commandEntry->SetBackgroundColor(0x000000);  // Black
    commandEntry->SetForegroundColor(0x00FF00);  // Green
    commandFrame->AddFrame(commandEntry, new TGLayoutHints(kLHintsExpandX, 5,5,2,2));
    
    runCommandButton = new TGTextButton(commandFrame, "Execute Command", kRunCommandButton);
    runCommandButton->Associate(this);
    commandFrame->AddFrame(runCommandButton, new TGLayoutHints(kLHintsLeft, 5,5,2,5));
    
    scriptGroup->AddFrame(commandFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));

    // Redirect cout and cerr to output view
    coutBuf = new TextEditStreamBuf(outputView);
    cerrBuf = new TextEditStreamBuf(outputView);
    oldCoutBuf = std::cout.rdbuf(coutBuf);
    oldCerrBuf = std::cerr.rdbuf(cerrBuf);

    // Print welcome message
    std::cout << "=== Advanced ROOT Plotting GUI ===" << std::endl;
    std::cout << "Output window ready. Commands will appear here." << std::endl;
    std::cout << "===================================" << std::endl << std::endl;
    std::cout.flush();
    gSystem->ProcessEvents();

    /////////////////////////////////////////////////////////////////////////
    
    SetWindowName("Advanced Plot GUI");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

// ==========================
// Destructor
// ==========================
AdvancedPlotGUI::~AdvancedPlotGUI() {
    // Restore original stream buffers
    if (oldCoutBuf) std::cout.rdbuf(oldCoutBuf);
    if (oldCerrBuf) std::cerr.rdbuf(oldCerrBuf);
    
    // Delete custom stream buffers
    if (coutBuf) delete coutBuf;
    if (cerrBuf) delete cerrBuf;
    
    // Close ROOT file if open
    if (currentRootFile && currentRootFile->IsOpen()) {
        currentRootFile->Close();
        delete currentRootFile;
    }
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
        return;
    }
    
    std::cout << "=== Executing Script ===" << std::endl;
    
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
        if (lang == 0) {
            // ROOT/C++
            gROOT->ProcessLine(line);
        } else if (lang == 1) {
            // Python
            TPython::Exec(line);
        }
        
        std::cout.flush();
        std::cerr.flush();
        
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
}

void AdvancedPlotGUI::DoRunCommand() {
    if (!commandEntry) return;
    
    TString command = commandEntry->GetText();
    command = command.Strip(TString::kBoth);
    
    if (command.IsNull()) return;
    
    int lang = scriptLangCombo->GetSelected();
    
    std::cout << "> " << command.Data() << std::endl;
    
    if (lang == 0) {
        // ROOT/C++
        gROOT->ProcessLine(command);
    } else if (lang == 1) {
        // Python
        TPython::Exec(command);
    }
    
    std::cout.flush();
    std::cerr.flush();
    
    // Update canvases
    TIter nextCanvas(gROOT->GetListOfCanvases());
    TCanvas* c;
    while ((c = (TCanvas*)nextCanvas())) {
        c->Modified();
        c->Update();
    }
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

////////////////////////////////////
/////// Print Canvas/Pad Information ///////

void AdvancedPlotGUI::PrintCanvasInfo(TCanvas* canvas) {
    if (!canvas) return;
    
    // Force flush any pending output first
    std::cout.flush();
    std::cerr.flush();
    
    std::cout << "\n=== Canvas/Pad Information ===" << std::endl;
    std::cout << "Canvas: " << canvas->GetName() << std::endl;
    std::cout << "  - Title: " << canvas->GetTitle() << std::endl;
    std::cout << "  - Size: " << canvas->GetWw() << " x " << canvas->GetWh() << " pixels" << std::endl;
    std::cout << "  - Position: (" << canvas->GetWindowTopX() << ", " << canvas->GetWindowTopY() << ")" << std::endl;
    std::cout << "  - Number of primitives: " << canvas->GetListOfPrimitives()->GetSize() << std::endl;
    
    TPad* pad = (TPad*)gPad;
    if (pad) {
        std::cout << "  - Current pad: " << pad->GetName() << std::endl;
        std::cout << "\nCurrent Pad Properties:" << std::endl;
        std::cout << "  - Log X: " << (pad->GetLogx() ? "Yes" : "No") << std::endl;
        std::cout << "  - Log Y: " << (pad->GetLogy() ? "Yes" : "No") << std::endl;
        std::cout << "  - Log Z: " << (pad->GetLogz() ? "Yes" : "No") << std::endl;
        std::cout << "  - Grid X: " << (pad->GetGridx() ? "Yes" : "No") << std::endl;
        std::cout << "  - Grid Y: " << (pad->GetGridy() ? "Yes" : "No") << std::endl;
        std::cout << "  - Tick X: " << pad->GetTickx() << std::endl;
        std::cout << "  - Tick Y: " << pad->GetTicky() << std::endl;
        std::cout << "  - Margins: L=" << pad->GetLeftMargin() << " R=" << pad->GetRightMargin()
                  << " T=" << pad->GetTopMargin() << " B=" << pad->GetBottomMargin() << std::endl;
        std::cout << "  - Fill Color: " << pad->GetFillColor() << std::endl;
        std::cout << "  - Fill Style: " << pad->GetFillStyle() << std::endl;
        
        // List primitives
        TList* prims = pad->GetListOfPrimitives();
        if (prims && prims->GetSize() > 0) {
            std::cout << "\nObjects in current pad:" << std::endl;
            TIter next(prims);
            TObject* obj;
            int count = 0;
            while ((obj = next()) && count < 10) {
                std::cout << "  [" << count++ << "] " << obj->ClassName() 
                          << " - " << obj->GetName() << std::endl;
            }
            if (prims->GetSize() > 10) {
                std::cout << "  ... and " << (prims->GetSize() - 10) << " more objects" << std::endl;
            }
        }
    }
    
    std::cout << "\nUseful Commands:" << std::endl;
    std::cout << "  gPad->SetLogy(1);          // Log Y scale" << std::endl;
    std::cout << "  gPad->SetGridx(1);         // X grid" << std::endl;
    std::cout << "  gPad->SetGridy(1);         // Y grid" << std::endl;
    std::cout << "  gPad->SetFillColor(10);    // Background" << std::endl;
    std::cout << "  " << canvas->GetName() << "->cd();" << std::endl;
    std::cout << "  " << canvas->GetName() << "->SaveAs(\"plot.pdf\");" << std::endl;
    std::cout << "===========================" << std::endl << std::endl;
    
    // CRITICAL: Force immediate flush and GUI update
    std::cout.flush();
    std::cerr.flush();
    gSystem->ProcessEvents();
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
                    gROOT->GetListOfGlobals()->Add(g);
                    legend->AddEntry(g, currentData.headers[config.yColumn].c_str(), "lp");
                    ApplyFit(g, fitType, config.color, customFunc);
                    firstPlot = false;
                }
            } else if (config.type == PlotConfig::kTGraphErrors) {
                TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
                if (g) {
                    g->SetTitle(canvasTitle.c_str());
                    g->Draw(firstPlot ? "APE" : "PE SAME");
                    gROOT->GetListOfGlobals()->Add(g);
                    legend->AddEntry(g, currentData.headers[config.yColumn].c_str(), "lpe");
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
                    h->Draw(firstPlot ? "HIST" : "HIST SAME");
                    gROOT->GetListOfGlobals()->Add(h);
                    legend->AddEntry(h, currentData.headers[config.xColumn].c_str(), "l");
                    firstPlot = false;

                    if (fitType == FitUtils::kGaus) {applyRooFitGaussian(h, config.color);
                } else {ApplyFit(h, fitType, config.color, customFunc);}
            }
        }
    }
    legend->Draw();
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
            if (padNum > nRows * nCols) break;
            canvas->cd(padNum++);

            if (config.type == PlotConfig::kTGraph) {
                TGraph* g = PlotCreator::CreateTGraph(currentData, config);
                if (g) {
                g->Draw("APL");
                gROOT->GetListOfGlobals()->Add(g);
                ApplyFit(g, fitType, config.color, customFunc);}
            } else if (config.type == PlotConfig::kTGraphErrors) {
                TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
                if (g) {
                g->Draw("APE");
                gROOT->GetListOfGlobals()->Add(g);
                ApplyFit(g, fitType, config.color, customFunc);}
            } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
                TH1* h = PlotCreator::CreateTH1(currentData, config);
                if (h) {
                h->Draw();
                gROOT->GetListOfGlobals()->Add(h);
                if (fitType == FitUtils::kGaus) applyRooFitGaussian(h, config.color);
                else ApplyFit(h, fitType, config.color, customFunc);}
            } else if (config.type == PlotConfig::kTH2D || 
                config.type == PlotConfig::kTH2F || 
                config.type == PlotConfig::kTH2I) {
                TH2* h = PlotCreator::CreateTH2(currentData, config);
                if (h) {
                h->Draw("COLZ");
                gROOT->GetListOfGlobals()->Add(h);
                ApplyFit(h, fitType, config.color, customFunc);}
        } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(currentData, config);
            if (h) {h->Draw("COLZ"); gROOT->GetListOfGlobals()->Add(h);}
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

        TCanvas* c = new TCanvas(Form("c%zu", i), Form("%s - %zu", canvasTitle.c_str(), i), 800, 600);

            if (config.type == PlotConfig::kTGraph) {
            TGraph* g = PlotCreator::CreateTGraph(currentData, config);
            if (g) {g->Draw("AP");
                gROOT->GetListOfGlobals()->Add(g);
                ApplyFit(g, fitType, config.color, customFunc);}
            } else if (config.type == PlotConfig::kTGraphErrors) {
            TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
            if (g) {g->Draw("APE");
                gROOT->GetListOfGlobals()->Add(g);
                ApplyFit(g, fitType, config.color, customFunc);}
            } else if (config.type == PlotConfig::kTH1D || 
                   config.type == PlotConfig::kTH1F || 
                   config.type == PlotConfig::kTH1I) {
            TH1* h = PlotCreator::CreateTH1(currentData, config);
            if (h) { h->Draw();
                gROOT->GetListOfGlobals()->Add(h);
                if (fitType == FitUtils::kGaus) applyRooFitGaussian(h, config.color);
                else ApplyFit(h, fitType, config.color, customFunc);}
            } else if (config.type == PlotConfig::kTH2D || 
                   config.type == PlotConfig::kTH2F || 
                   config.type == PlotConfig::kTH2I) {
            TH2* h = PlotCreator::CreateTH2(currentData, config);
            if (h) { h->Draw("COLZ");
                gROOT->GetListOfGlobals()->Add(h);
                ApplyFit(h, fitType, config.color, customFunc);}
            } else if (config.type == PlotConfig::kTH3D || 
                   config.type == PlotConfig::kTH3F || 
                   config.type == PlotConfig::kTH3I) {
            TH3* h = PlotCreator::CreateTH3(currentData, config);
            if (h) {h->Draw("COLZ"); gROOT->GetListOfGlobals()->Add(h);}
            }
        c->Update();
        // AUTO-PRINT CANVAS INFO for each canvas
        PrintCanvasInfo(c);
        }
    }
    gSystem->ProcessEvents();
}

/////// Process messages ///////

Bool_t AdvancedPlotGUI::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) {
    switch (GET_MSG(msg)) {
        case kC_COMMAND:
            switch (GET_SUBMSG(msg)) {
                case kCM_BUTTON:
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
    return kTRUE;
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

    new AdvancedPlotGUI(gClient->GetRoot(), 700, 900);

    app.Run();   // ROOT prompt + GUI
    return 0;
}