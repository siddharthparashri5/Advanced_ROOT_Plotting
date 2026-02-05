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

#include <TGFrame.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGListBox.h>
#include <TGTextEdit.h>
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TGTab.h>
#include <TFile.h>
#include <vector>
#include <string>
#include <streambuf>

// Forward declarations
struct ColumnData;
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
class AdvancedPlotGUI : public TGMainFrame {

private:
    // ==================== File Selection ====================
    TGTextEntry* fileEntry;              ///< Text entry for file path
    TGTextButton* browseButton;          ///< Browse button for file selection
    TGTextButton* loadButton;            ///< Load button to read file
    
    // ==================== Plot Configuration ====================
    TGTextButton* addPlotButton;         ///< Button to add plot configuration
    TGListBox* plotListBox;              ///< List of configured plots
    TGTextButton* removePlotButton;      ///< Remove selected plot config
    TGTextButton* clearAllButton;        ///< Clear all plot configurations
    
    // ==================== Canvas Options ====================
    TGCheckButton* sameCanvasCheck;      ///< Overlay plots on same canvas
    TGCheckButton* dividedCanvasCheck;   ///< Divide canvas into pads
    TGNumberEntry* nRowsEntry;           ///< Number of rows for division
    TGNumberEntry* nColsEntry;           ///< Number of columns for division
    
    // ==================== Fit Options ====================
    TGComboBox* fitFunctionCombo;        ///< Combo box for fit function selection
    TGTextEntry* customFuncEntry;        ///< Custom fit function entry
    TGTextEntry* canvasTitleEntry;       ///< Canvas title entry
    
    // ==================== Action Buttons ====================
    TGTextButton* plotButton;            ///< Create plots button
    
    // ==================== Script Panel Components ====================
    TGTab* scriptTab;                    ///< Tab widget for script panel
    TGTextEdit* scriptEditor;            ///< Multi-line script editor
    TGTextEntry* commandEntry;           ///< Single-line command entry
    TGTextEdit* outputView;              ///< Read-only output display
    TGComboBox* scriptLangCombo;         ///< Language selection (C++/Python)
    TGTextButton* runScriptButton;       ///< Run script button
    TGTextButton* runCommandButton;      ///< Execute command button
    TGTextButton* loadScriptButton;      ///< Load script from file
    TGTextButton* saveScriptButton;      ///< Save script to file
    TGTextButton* clearEditorButton;     ///< Clear script editor
    TGTextButton* clearOutputButton;     ///< Clear output view
    
    // ==================== Data Storage ====================
    ColumnData currentData;              ///< Currently loaded data
    std::vector<PlotConfig> plotConfigs; ///< List of plot configurations
    
    TGMainFrame* fMainFrame;             ///< Pointer to main frame
    
    // ==================== ROOT File Management ====================
    TFile* currentRootFile;              ///< Currently open ROOT file
    
    // ==================== Stream Redirectors ====================
    TextEditStreamBuf* coutBuf;          ///< Custom cout stream buffer
    TextEditStreamBuf* cerrBuf;          ///< Custom cerr stream buffer
    std::streambuf* oldCoutBuf;          ///< Original cout buffer
    std::streambuf* oldCerrBuf;          ///< Original cerr buffer
    
    // ==================== Widget IDs ====================
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
    /**
     * @brief Constructor for AdvancedPlotGUI
     * @param p Pointer to parent window (usually gClient->GetRoot())
     * @param w Width of main window in pixels
     * @param h Height of main window in pixels
     * 
     * @details
     * Initializes the main GUI window with specified dimensions.
     * Sets up all GUI components including:
     * - File loading interface
     * - Plot configuration panel
     * - Canvas options
     * - Fit function selector
     * - Script panel with tabs
     * - Stream redirection for output capture
     */
    AdvancedPlotGUI(const TGWindow *p, UInt_t w, UInt_t h);
    
    /**
     * @brief Destructor for AdvancedPlotGUI
     * @details 
     * Cleans up allocated memory:
     * - Restores original stream buffers
     * - Deletes custom stream redirectors
     * - Closes open ROOT files
     */
    virtual ~AdvancedPlotGUI();
    
    // ==================== File Operations ====================
    
    /**
     * @brief Open file browser dialog
     * @details Opens a file selection dialog for choosing data files
     */
    void DoBrowse();
    
    /**
     * @brief Load data from selected file
     * @details 
     * Loads data based on file extension:
     * - .root files: Opens in TBrowser
     * - .txt, .dat, .csv: Loads as column data
     */
    void DoLoad();
    
    // ==================== Plot Configuration ====================
    
    /**
     * @brief Add a new plot configuration
     * @details Opens column selector dialog to configure a new plot
     */
    void DoAddPlot();
    
    /**
     * @brief Remove selected plot configuration
     * @details Removes the currently selected plot from the list
     */
    void DoRemovePlot();
    
    /**
     * @brief Clear all plot configurations
     * @details Removes all plots from the configuration list
     */
    void DoClearAll();
    
    /**
     * @brief Create plots from configurations
     * @details 
     * Creates plots based on current configurations.
     * Supports three modes:
     * - Same canvas (overlay)
     * - Divided canvas (pads)
     * - Separate canvases
     * 
     * Applies selected fit functions if specified.
     */
    void DoPlot();
    
    // ==================== Script Operations ====================
    
    /**
     * @brief Execute script from editor
     * @details 
     * Runs the complete script from the script editor tab.
     * Splits into lines and executes each non-empty, non-comment line.
     * Updates all canvases after execution.
     */
    void DoRunScript();
    
    /**
     * @brief Execute single command
     * @details 
     * Executes the command entered in the command line tab.
     * Clears the command entry after execution.
     * Updates all canvases.
     */
    void DoRunCommand();
    
    /**
     * @brief Load script from file
     * @details 
     * Opens file dialog to load a script file.
     * Auto-detects language from file extension (.C, .cpp, .py)
     */
    void DoLoadScript();
    
    /**
     * @brief Save script to file
     * @details Opens save dialog to save current script editor content
     */
    void DoSaveScript();
    
    /**
     * @brief Clear script editor
     * @details Removes all text from the script editor
     */
    void DoClearEditor();
    
    /**
     * @brief Clear output view
     * @details Clears all output text from the output tab
     */
    void DoClearOutput();
    
    // ==================== Message Handling ====================
    
    /**
     * @brief Process GUI messages
     * @param msg Message type
     * @param parm1 Parameter 1 (usually widget ID)
     * @param parm2 Parameter 2 (usually sub-message)
     * @return kTRUE if message handled
     * 
     * @details 
     * Handles all GUI events including:
     * - Button clicks
     * - Text entry (Return key)
     * - Menu selections
     */
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    ClassDef(AdvancedPlotGUI, 2) // Advanced ROOT plotting GUI with scripting
};

#endif // ADVANCEDPLOTGUI_H