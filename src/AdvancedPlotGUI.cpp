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

#include <fstream>
#include <cstdlib>
#include <sstream>

#include "DataReader.h"
#include "ColumnSelector.h"
#include "PlotTypes.h"
#include "FitUtils.h"
#include "ErrorHandling.h"

// Custom stream buffer that redirects to TGTextEdit
class TextEditStreamBuf : public std::streambuf {
private:
    TGTextEdit* textEdit;
    std::string buffer;
    
public:
    TextEditStreamBuf(TGTextEdit* edit) : textEdit(edit) {}
    
protected:
    virtual int overflow(int c) override {
        if (c != EOF) {
            if (c == '\n') {
                // Flush line to text edit
                TGText* txt = textEdit->GetText();
                if (!buffer.empty()) {
                    txt->InsText(TGLongPosition(txt->RowCount(), 0), buffer.c_str());
                    buffer.clear();
                }
                txt->InsText(TGLongPosition(txt->RowCount(), 0), "\n");
                textEdit->Layout();
                textEdit->SetVsbPosition(txt->RowCount());
                gSystem->ProcessEvents();
            } else {
                buffer += static_cast<char>(c);
            }
        }
        return c;
    }
    
    virtual int sync() override {
        if (!buffer.empty()) {
            TGText* txt = textEdit->GetText();
            txt->InsText(TGLongPosition(txt->RowCount(), 0), buffer.c_str());
            buffer.clear();
            textEdit->Layout();
            textEdit->SetVsbPosition(txt->RowCount());
            gSystem->ProcessEvents();
        }
        return 0;
    }
};

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
    
    // Script panel components
    TGTab* scriptTab;
    TGTextEdit* scriptEditor;        // Multi-line script editor
    TGTextEntry* commandEntry;       // Single-line command entry
    TGTextEdit* outputView;          // Output display
    TGComboBox* scriptLangCombo;
    TGTextButton* runScriptButton;
    TGTextButton* runCommandButton;
    TGTextButton* loadScriptButton;
    TGTextButton* saveScriptButton;
    TGTextButton* clearEditorButton;
    TGTextButton* clearOutputButton;
    
    // Data storage
    ColumnData currentData;
    std::vector<PlotConfig> plotConfigs;

    TGMainFrame* fMainFrame;
    
    // Store opened ROOT file
    TFile* currentRootFile;
    
    // Stream redirectors
    TextEditStreamBuf* coutBuf;
    TextEditStreamBuf* cerrBuf;
    std::streambuf* oldCoutBuf;
    std::streambuf* oldCerrBuf;
    
    enum {
        kBrowseButton = 100,
        kLoadButton,
        kAddPlotButton,
        kRemovePlotButton,
        kClearAllButton,
        kPlotButton,
        kRunScriptButton = 200,
        kRunCommandButton,
        kLoadScriptButton,
        kSaveScriptButton,
        kClearEditorButton,
        kClearOutputButton
    };

public:
    AdvancedPlotGUI(const TGWindow* p, UInt_t w, UInt_t h);
    virtual ~AdvancedPlotGUI();

    void DoBrowse();
    void DoLoad();
    void DoAddPlot();
    void DoRemovePlot();
    void DoClearAll();
    void DoPlot();
    
    // Script panel methods
    void DoRunScript();
    void DoRunCommand();
    void DoLoadScript();
    void DoSaveScript();
    void DoClearEditor();
    void DoClearOutput();
    
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};

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

    /////////////////////////////////////////////////////////////////////////
    // ---------- Script Panel with Tabs ----------
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
    
    // Create tab widget for script editor and output
    scriptTab = new TGTab(scriptGroup, 600, 300);
    
    // --- Script Editor Tab ---
    TGCompositeFrame* editorTab = scriptTab->AddTab("Script Editor");
    
    scriptEditor = new TGTextEdit(editorTab, 600, 200);
    editorTab->AddFrame(scriptEditor, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));
    
    TGHorizontalFrame* editorButtonFrame = new TGHorizontalFrame(editorTab);
    
    runScriptButton = new TGTextButton(editorButtonFrame, "Run Script", kRunScriptButton);
    runScriptButton->Associate(this);
    editorButtonFrame->AddFrame(runScriptButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));
    
    clearEditorButton = new TGTextButton(editorButtonFrame, "Clear Editor", kClearEditorButton);
    clearEditorButton->Associate(this);
    editorButtonFrame->AddFrame(clearEditorButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));
    
    editorTab->AddFrame(editorButtonFrame, new TGLayoutHints(kLHintsLeft, 5,5,5,5));
    
    // --- Command Line Tab ---
    TGCompositeFrame* commandTab = scriptTab->AddTab("Command Line");
    
    TGLabel* cmdLabel = new TGLabel(commandTab, "Enter command (press Enter to execute):");
    commandTab->AddFrame(cmdLabel, new TGLayoutHints(kLHintsLeft, 5,5,5,2));
    
    commandEntry = new TGTextEntry(commandTab);
    commandEntry->Resize(580, 20);
    commandEntry->Associate(this);
    commandTab->AddFrame(commandEntry, new TGLayoutHints(kLHintsExpandX, 5,5,2,5));
    
    runCommandButton = new TGTextButton(commandTab, "Execute Command", kRunCommandButton);
    runCommandButton->Associate(this);
    commandTab->AddFrame(runCommandButton, new TGLayoutHints(kLHintsLeft, 5,5,5,5));
    
    // --- Output Tab ---
    TGCompositeFrame* outputTab = scriptTab->AddTab("Output");
    
    outputView = new TGTextEdit(outputTab, 600, 200);
    outputView->SetReadOnly(kTRUE);
    outputTab->AddFrame(outputView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));
    
    clearOutputButton = new TGTextButton(outputTab, "Clear Output", kClearOutputButton);
    clearOutputButton->Associate(this);
    outputTab->AddFrame(clearOutputButton, new TGLayoutHints(kLHintsLeft, 5,5,5,5));
    
    scriptGroup->AddFrame(scriptTab, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));
    
    // Redirect cout and cerr to output view
    coutBuf = new TextEditStreamBuf(outputView);
    cerrBuf = new TextEditStreamBuf(outputView);
    oldCoutBuf = std::cout.rdbuf(coutBuf);
    oldCerrBuf = std::cerr.rdbuf(cerrBuf);
    
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
        case PlotConfig::kTH2D:
            desc = Form("TH2D: %s vs %s",
                        currentData.headers[config.xColumn].c_str(),
                        currentData.headers[config.yColumn].c_str());
            break;
        case PlotConfig::kTH3D:
            desc = Form("TH3D: %s vs %s vs %s",
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
            } else if (config.type == PlotConfig::kTH1D) {
                TH1* h = PlotCreator::CreateTH1D(currentData, config);
                if (h) {
                    h->SetTitle(canvasTitle.c_str());
                    h->SetLineColor(config.color);
                    h->Draw(firstPlot ? "HIST" : "HIST SAME");
                    gROOT->GetListOfGlobals()->Add(h);
                    legend->AddEntry(h, currentData.headers[config.xColumn].c_str(), "l");
                    firstPlot = false;

                    if (fitType == FitUtils::kGaus) {
                        applyRooFitGaussian(h, config.color);
                    } else {
                        ApplyFit(h, fitType, config.color, customFunc);
                    }
                }
            }
        }
        legend->Draw();
        canvas->Update();
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
                    ApplyFit(g, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTGraphErrors) {
                TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
                if (g) {
                    g->Draw("APE");
                    gROOT->GetListOfGlobals()->Add(g);
                    ApplyFit(g, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTH1D) {
                TH1* h = PlotCreator::CreateTH1D(currentData, config);
                if (h) {
                    h->Draw();
                    gROOT->GetListOfGlobals()->Add(h);
                    if (fitType == FitUtils::kGaus) applyRooFitGaussian(h, config.color);
                    else ApplyFit(h, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTH2D) {
                TH2* h = PlotCreator::CreateTH2D(currentData, config);
                if (h) {
                    h->Draw("COLZ");
                    gROOT->GetListOfGlobals()->Add(h);
                    ApplyFit(h, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTH3D) {
                TH3* h = PlotCreator::CreateTH3D(currentData, config);
                if (h) {
                    h->Draw("ISO");
                    gROOT->GetListOfGlobals()->Add(h);
                }
            }
        }
        canvas->Update();
    }
    
    ////// SEPARATE CANVAS MODE ////////
    else {
        for (size_t i = 0; i < plotConfigs.size(); ++i) {
            PlotConfig& config = plotConfigs[i];
            config.color = (i % 9) + 1;

            TCanvas* c = new TCanvas(Form("c%zu", i), Form("%s - %zu", canvasTitle.c_str(), i), 800, 600);

            if (config.type == PlotConfig::kTGraph) {
                TGraph* g = PlotCreator::CreateTGraph(currentData, config);
                if (g) {
                    g->Draw("AP");
                    gROOT->GetListOfGlobals()->Add(g);
                    ApplyFit(g, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTGraphErrors) {
                TGraphErrors* g = PlotCreator::CreateTGraphErrors(currentData, config);
                if (g) {
                    g->Draw("APE");
                    gROOT->GetListOfGlobals()->Add(g);
                    ApplyFit(g, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTH1D) {
                TH1* h = PlotCreator::CreateTH1D(currentData, config);
                if (h) {
                    h->Draw();
                    gROOT->GetListOfGlobals()->Add(h);
                    if (fitType == FitUtils::kGaus) applyRooFitGaussian(h, config.color);
                    else ApplyFit(h, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTH2D) {
                TH2* h = PlotCreator::CreateTH2D(currentData, config);
                if (h) {
                    h->Draw("COLZ");
                    gROOT->GetListOfGlobals()->Add(h);
                    ApplyFit(h, fitType, config.color, customFunc);
                }
            } else if (config.type == PlotConfig::kTH3D) {
                TH3* h = PlotCreator::CreateTH3D(currentData, config);
                if (h) {
                    h->Draw("ISO");
                    gROOT->GetListOfGlobals()->Add(h);
                }
            }
            c->Update();
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