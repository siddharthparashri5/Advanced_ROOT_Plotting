/**
 * @file AdvancedPlotGUI.h
 * @brief Advanced plotting GUI for ROOT data visualization
 * @author Siddharth Parashri
 * @date 2026-02-04
 * @version 1.0
 * 
 * This file contains the main GUI class for advanced ROOT plotting capabilities.
 * It provides an interactive interface for loading, visualizing, and analyzing
 * data using ROOT's powerful plotting features.
 */

#ifndef ADVANCEDPLOTGUI_H
#define ADVANCEDPLOTGUI_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGMenu.h>
#include <TRootEmbeddedCanvas.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TH1.h>
#include <TGraph.h>
#include <TF1.h>

/**
 * @class AdvancedPlotGUI
 * @brief Main GUI class for advanced ROOT data visualization
 * 
 * This class provides a comprehensive graphical user interface for loading,
 * visualizing, and analyzing data using ROOT. It supports multiple file formats,
 * various plot types, fitting capabilities, and customization options.
 * 
 * @details
 * The GUI includes:
 * - File loading capabilities (ROOT, CSV, TXT formats)
 * - Multiple plot types (histogram, scatter, line plots)
 * - Interactive fitting with various functions
 * - Plot customization (colors, markers, styles)
 * - Export functionality (PNG, PDF, ROOT formats)
 * 
 * Example usage:
 * @code
 * TApplication theApp("App", &argc, argv);
 * AdvancedPlotGUI gui(gClient->GetRoot(), 1200, 800);
 * theApp.Run();
 * @endcode
 */
class AdvancedPlotGUI : public TGMainFrame {

private:
    // GUI Components
    TRootEmbeddedCanvas *fCanvas;           ///< Embedded ROOT canvas for plotting
    TGMenuBar           *fMenuBar;          ///< Main menu bar
    TGPopupMenu         *fMenuFile;         ///< File menu
    TGPopupMenu         *fMenuPlot;         ///< Plot menu
    TGPopupMenu         *fMenuFit;          ///< Fit menu
    TGPopupMenu         *fMenuHelp;         ///< Help menu
    
    // Control widgets
    TGTextButton        *fLoadButton;       ///< Button to load data files
    TGTextButton        *fPlotButton;       ///< Button to create plots
    TGTextButton        *fFitButton;        ///< Button to fit data
    TGTextButton        *fExportButton;     ///< Button to export plots
    TGTextEntry         *fFileEntry;        ///< Text entry for file path
    TGComboBox          *fPlotTypeCombo;    ///< Combo box for plot type selection
    TGComboBox          *fFitFuncCombo;     ///< Combo box for fit function selection
    
    // Data members
    TH1D                *fHistogram;        ///< Current histogram object
    TGraph              *fGraph;            ///< Current graph object
    TF1                 *fFitFunction;      ///< Current fit function
    TString             fCurrentFile;       ///< Path to currently loaded file
    Int_t               fDataPoints;        ///< Number of data points loaded
    
    // Style settings
    Int_t               fLineColor;         ///< Line color for plots
    Int_t               fMarkerStyle;       ///< Marker style for graphs
    Int_t               fFillColor;         ///< Fill color for histograms
    
    /**
     * @brief Initialize GUI layout and components
     * @details Sets up the menu bar, canvas, control buttons, and layout managers
     */
    void InitGUI();
    
    /**
     * @brief Create and configure the menu bar
     * @details Creates File, Plot, Fit, and Help menus with appropriate items
     */
    void CreateMenuBar();
    
    /**
     * @brief Parse ROOT file and extract data
     * @param filename Path to ROOT file
     * @return true if parsing successful, false otherwise
     * @details Attempts to read histograms, trees, or graphs from ROOT file
     */
    bool ParseROOTFile(const char* filename);
    
    /**
     * @brief Parse CSV file and load data
     * @param filename Path to CSV file
     * @return true if parsing successful, false otherwise
     * @details Reads comma-separated values and creates appropriate data structures
     */
    bool ParseCSVFile(const char* filename);
    
    /**
     * @brief Parse text file with columnar data
     * @param filename Path to text file
     * @return true if parsing successful, false otherwise
     * @details Reads space or tab-separated columnar data
     */
    bool ParseTextFile(const char* filename);
    
    /**
     * @brief Apply current style settings to plot
     * @details Updates line color, marker style, and fill color based on user preferences
     */
    void ApplyPlotStyle();
    
    /**
     * @brief Perform fit operation on current data
     * @param funcName Name of the fit function (e.g., "gaus", "pol1", "expo")
     * @return Pointer to the fit function, nullptr if fit failed
     * @details Fits the selected function to current histogram or graph
     */
    TF1* PerformFit(const char* funcName);

public:
    /**
     * @brief Constructor for AdvancedPlotGUI
     * @param p Pointer to parent window (usually gClient->GetRoot())
     * @param w Width of main window in pixels
     * @param h Height of main window in pixels
     * 
     * @details
     * Initializes the main GUI window with specified dimensions.
     * Sets up all GUI components and prepares for user interaction.
     */
    AdvancedPlotGUI(const TGWindow *p, UInt_t w, UInt_t h);
    
    /**
     * @brief Destructor for AdvancedPlotGUI
     * @details Cleans up allocated memory and closes any open files
     */
    virtual ~AdvancedPlotGUI();
    
    /**
     * @brief Load data from file
     * @return true if successful, false otherwise
     * 
     * @details
     * Opens a file browser dialog and loads the selected file.
     * Supports ROOT (.root), CSV (.csv), and text (.txt, .dat) formats.
     * Automatically detects file format based on extension.
     * 
     * @note The loaded data is stored in internal data structures
     * and can be accessed for plotting and fitting operations.
     */
    bool DoLoad();
    
    /**
     * @brief Create plot from loaded data
     * @return true if plot created successfully, false otherwise
     * 
     * @details
     * Creates a plot based on the currently loaded data and selected plot type.
     * Applies user-specified styling options and updates the canvas.
     * 
     * Supported plot types:
     * - Histogram (1D, 2D)
     * - Scatter plot
     * - Line plot
     * - Error bars plot
     */
    bool DoPlot();
    
    /**
     * @brief Fit current data with selected function
     * @return true if fit successful, false otherwise
     * 
     * @details
     * Performs a least-squares fit of the selected function to the current data.
     * Displays fit parameters and statistics on the plot.
     * 
     * Available fit functions:
     * - Gaussian (gaus)
     * - Polynomial (pol0-pol9)
     * - Exponential (expo)
     * - Landau distribution
     * - Custom user-defined functions
     * 
     * @see PerformFit()
     */
    bool DoFit();
    
    /**
     * @brief Export current plot to file
     * @return true if export successful, false otherwise
     * 
     * @details
     * Opens a save dialog and exports the current canvas to the selected format.
     * Supported formats: PNG, PDF, EPS, SVG, ROOT, C++
     * 
     * @note The exported file includes all visible elements on the canvas
     * including the plot, fit results, and any annotations.
     */
    bool DoExport();
    
    /**
     * @brief Set plot type for visualization
     * @param type Plot type identifier (0=histogram, 1=scatter, 2=line, etc.)
     * 
     * @details
     * Changes the visualization method for the current data.
     * Automatically redraws the plot with the new type if data is loaded.
     */
    void SetPlotType(Int_t type);
    
    /**
     * @brief Set line color for plots
     * @param color ROOT color index (see TColor documentation)
     * @see https://root.cern.ch/doc/master/classTColor.html
     */
    void SetLineColor(Int_t color) { fLineColor = color; }
    
    /**
     * @brief Set marker style for scatter plots
     * @param style ROOT marker style index (1-34)
     * @see https://root.cern.ch/doc/master/classTAttMarker.html
     */
    void SetMarkerStyle(Int_t style) { fMarkerStyle = style; }
    
    /**
     * @brief Set fill color for histograms
     * @param color ROOT color index
     */
    void SetFillColor(Int_t color) { fFillColor = color; }
    
    /**
     * @brief Handle menu item selections
     * @param id Menu item identifier
     * 
     * @details
     * This slot is connected to menu bar signals and processes
     * user selections from File, Plot, Fit, and Help menus.
     */
    void HandleMenu(Int_t id);
    
    /**
     * @brief Handle button clicks
     * 
     * @details
     * This slot processes button click events from the main control panel.
     * Routes the action to the appropriate handler function.
     */
    void HandleButtons();
    
    /**
     * @brief Clear all loaded data and plots
     * 
     * @details
     * Resets the GUI to initial state, clearing the canvas and
     * freeing memory associated with current data structures.
     */
    void ClearAll();
    
    /**
     * @brief Display application information
     * 
     * @details
     * Shows a dialog with version information, credits, and usage tips.
     */
    void ShowAbout();

    ClassDef(AdvancedPlotGUI, 1) // Advanced ROOT plotting GUI
};

#endif // ADVANCEDPLOTGUI_H
