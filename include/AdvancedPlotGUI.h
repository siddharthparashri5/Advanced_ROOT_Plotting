/**
 * @file AdvancedPlotGUI.h
 * @brief Advanced plotting GUI for ROOT data visualization with script panel
 * @author Siddharth Parashri
 * @date 2026-02-05
 * @version 2.0
 * 
 * This file contains the main GUI class for advanced ROOT plotting capabilities.
 * It provides an interactive interface for loading, visualizing, and analyzing
 * data using ROOT's powerful plotting features, with integrated script execution.
 */

#ifndef ADVANCEDPLOTGUI_H
#define ADVANCEDPLOTGUI_H

#include <streambuf>

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

// Forward declarations
//struct ColumnData;
struct PlotConfig;
class TGTextEdit;
class TextEditStreamBuf;

/**
 * @class AdvancedPlotGUI
 * @brief Main GUI class for advanced ROOT data visualization and scripting
 * 
 * This class provides a comprehensive graphical user interface for:
 * - Loading data from multiple formats (ROOT, CSV, TXT)
 * - Creating various plot types (TGraph, TH1D, TH2D, TH3D)
 * - Applying statistical fits (Gaussian, polynomial, custom)
 * - Executing ROOT/C++ and Python scripts
 * - Interactive command-line interface
 * 
 * @details
 * The GUI is organized into sections:
 * - File loading panel
 * - Plot configuration panel
 * - Canvas options (overlay, divided)
 * - Fit function selection
 * - Script panel (multi-line editor, command line, output viewer)
 * 
 * Example usage:
 * @code
 * TRint app("App", &argc, argv);
 * AdvancedPlotGUI gui(gClient->GetRoot(), 700, 900);
 * app.Run();
 * @endcode
 */


// Custom stream buffer that redirects to TGTextEdit
class TextEditStreamBuf : public std::streambuf {
private:
    TGTextEdit* textEdit;
    std::string buffer;
    
public:
    TextEditStreamBuf(TGTextEdit* edit) : textEdit(edit) {}
    // Force flush buffer to text edit
    void forceFlush() {if (!buffer.empty() || textEdit) {sync();}}
    
protected:
    virtual int overflow(int c) override {
        if (c != EOF) {
            if (c == '\n') {
                // Flush line to text edit
                if (textEdit) {
                    TGText* txt = textEdit->GetText();
                    if (!buffer.empty()) {
                        txt->InsText(TGLongPosition(txt->RowCount(), 0), buffer.c_str());
                        buffer.clear();}
                    txt->InsText(TGLongPosition(txt->RowCount(), 0), "\n");
                    textEdit->Layout(); textEdit->SetVsbPosition(txt->RowCount());
                    textEdit->Update(); gSystem->ProcessEvents();
                }
            } else { buffer += static_cast<char>(c);}
        }return c;
    }
    virtual int sync() override {
        if (!buffer.empty() && textEdit) {
            TGText* txt = textEdit->GetText();
            txt->InsText(TGLongPosition(txt->RowCount(), 0), buffer.c_str());
            txt->InsText(TGLongPosition(txt->RowCount(), 0), "\n");
            buffer.clear();
            textEdit->Layout();
            textEdit->SetVsbPosition(txt->RowCount());
            textEdit->Update();
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

    void PrintCanvasInfo(TCanvas* canvas);

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
    ClassDef(AdvancedPlotGUI, 2) // Advanced ROOT plotting GUI with scripting
};

#endif // ADVANCEDPLOTGUI_H