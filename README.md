# Advanced ROOT Plotting GUI

A modern, user-friendly graphical interface for creating publication-quality plots with ROOT, supporting CSV data files, ROOT files with drag-and-drop functionality, advanced TTree entry selection with chained filtering, and integrated script execution.

![Version](https://img.shields.io/badge/version-2.3-blue)
![ROOT](https://img.shields.io/badge/ROOT-6.26+-green)
![C++](https://img.shields.io/badge/C++-17-orange)

## Features

### 📊 Data Input
- **CSV/Text File Import**: Interactive preview dialog with customizable settings
  - Multiple delimiter options (comma, semicolon, tab, space, custom)
  - Header row detection and configuration
  - Skip rows for metadata
  - Live preview with formatted table view
  
- **ROOT File Support**: Native ROOT object handling
  - Load histograms (TH1, TH2, TH3)
  - Load graphs (TGraph, TGraphErrors, TGraphAsymmErrors)
  - Load TTree objects
  - Drag-and-drop support from TBrowser
  - Interactive ROOT File Browser with object preview

### 🎯 NEW: Advanced ROOT Analysis 
- **Entry Range Selection**: Select specific event ranges from TTrees
  - Choose start and end entry numbers
  - Apply to any TTree branch
  - Bin range selection for histograms
  
- **Cut Formulas**: Apply physics cuts using ROOT's TCut syntax
  - Complex boolean expressions: `Energy > 100 && Pt > 30`
  - Support for mathematical functions
  - Entry$ variable for entry-based cuts
  
- **Selection Chains**: Build cumulative filter pipelines
  - Sequential application of multiple cuts
  - Each step builds on previous selections
  - Visual chain representation in GUI
  - Real-time entry count feedback
  
- **Save/Load Chains**: Persistent selection workflows
  - Save chains as `.selchain` files (plain text, version-control friendly)
  - Load and reuse selection chains
  - Share analysis workflows with collaborators
  
- **Plot Options**: 
  - Plot current selection only (single step)
  - Plot with entire chain (cumulative cuts)
  - Automatic histogram creation from filtered TTree data
  - Full control over draw options

### 📈 Plotting Capabilities
- **Multiple Plot Types**:
  - Scatter plots with error bars
  - Line plots
  - Histograms (1D, 2D, 3D)
  - Error bar plots
  - Color maps and surface plots
  - TTree-based histograms with cuts
  
- **Customization Options**:
  - Axis labels and titles
  - Custom color schemes
  - Marker styles and sizes
  - Line styles and widths
  - Legend positioning
  - Grid options

### 🎨 Advanced Features
- **Multi-canvas Support**: Create and manage multiple plot canvases
- **Column Selector**: Interactive dialog for choosing data columns
- **Live Preview**: See changes in real-time
- **Export Options**: Save plots in multiple formats (PDF, PNG, EPS, SVG)
- **Drag & Drop**: Drop ROOT objects directly onto the GUI
- **Integrated Script Engine**: Execute ROOT/C++ and Python scripts
  - Multi-line script editor with syntax highlighting
  - Command-line interface for single commands
  - Load/save external script files
  - Access loaded ROOT files from scripts
- **Advanced Fitting**: Multiple fit functions with RooFit support
  - Gaussian, polynomial, exponential fits
  - Sine, damped sine for periodic data
  - Custom function support
  - Fit parameter display with uncertainties

## Requirements

### System Requirements
- **Operating System**: Linux (Ubuntu 20.04+ recommended)
- **ROOT**: Version 6.26 or higher (6.28+ recommended for all features)
- **C++ Compiler**: GCC 9.0+ with C++17 support
- **CMake**: Version 3.12 or higher
- **Python** (optional): For Python script execution

### ROOT Installation
```bash
# Install ROOT from official repository or build from source
# For Ubuntu/Debian:
sudo apt-get install root-system

# Or download from https://root.cern/install/
```

## Installation

### 1. Clone or Download the Project
```bash
git clone <repository-url>
cd Advance_ROOT_Plotting_dev
```

### 2. Project Structure
```
Advance_ROOT_Plotting_dev/
├── src/
│   ├── AdvancedPlotGUI.cpp       # Main GUI implementation
│   ├── CSVPreviewDialog.cpp      # CSV preview and import
│   ├── ColumnSelectorDialog.cpp  # Column selection dialog     
│   ├── ColumnSelectorDict.cpp    # ROOT dictionary
│   ├── RootEntrySelector.cpp     # Entry selector GUI
│   ├── PlotManager.cpp           # Plot creation and management
│   ├── FileHandler.cpp           # File loading and processing
│   ├── PlotTypes.cpp             # Plot type implementations
│   ├── ProjectDict.cpp           # ROOT dictionary
│   ├── RootDataInspector.cpp     # ROOT data inspection
│   ├── ROOTFileBrowser.cpp       # ROOT file browser dialog
│   ├── ScriptEngine.cpp          # Script execution engine
│   
├── include/
│   ├── AdvancedPlotGUI.h         # Main GUI header
│   ├── CSVPreviewDialog.h        # CSV preview dialog
│   ├── ColumnSelectorDialog.h    # Column selector
│   ├── ColumnSelectorLinkDef.h   # ROOT linkdef
│   ├── RootEntrySelector.h       # Entry selector GUI
│   ├── PlotManager.h             # Plot management
│   ├── DataReader.h              # Data reading utilities
│   ├── FileHandler.h             # File handling
│   ├── PlotTypes.h               # Plot type definitions
│   ├── ErrorHandling.h           # Error utilities
│   ├── FitUtils.h                # Fitting utilities
│   ├── RootDataInspector.h       # Data inspection
│   ├── ROOTFileBrowser.h         # File browser
│   ├── ScriptEngine.h            # Script engine
│
├── main.cpp                       # Application entry point
├── CMakeLists.txt                 # Build configuration
└── sample_data.csv                # Example data file
```

### 3. Build the Application
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

This will create the executable: `AdvancedPlotGUIApp`

### 4. Troubleshooting Build Issues

**Missing ROOT**: If CMake can't find ROOT, source the ROOT environment:
```bash
source /path/to/root/bin/thisroot.sh
```

**Compiler Errors**: Ensure you have C++17 support:
```bash
g++ --version  # Should be 9.0 or higher
```

## Usage

### Starting the Application
```bash
# From the build directory:
./AdvancedPlotGUIApp

# Or with ROOT:
root -l
.x ../main.cpp
```

### Loading CSV Files

1. **Click "Browse"** button
2. **Select your data file** in the file browser
3. **Configure import settings** in the preview dialog:
   - Choose delimiter (comma, semicolon, tab, etc.)
   - Set whether the first row contains headers
   - Skip metadata rows if needed
   - Preview shows a formatted table
4. **Click "Load"** to import the data
5. **Success dialog** confirms the data is loaded

### Loading ROOT Files

1. **Click "Browse"** button
2. **Select your .root file** in the file browser
3. **ROOT File Browser dialog** shows:
   - All histograms (TH1*, TH2*, TH3*)
   - All graphs (TGraph*)
   - All TTrees with branch information
4. **Select objects** to load
5. **Choose**: Plot Objects in a single, overlay, or divided canvas OR just open TBrowser
6. **Choose**: Load objects OR just open TBrowser

### Using the ROOT Analysis (NEW)

The ROOT Analysis provides advanced TTree event selection with chained filtering.


#### Opening the ROOT Analysis
#### Basic Workflow
1. **Load ROOT File**: File automatically opens on startup
2. **Select Object**: Choose TTree or histogram from dropdown
3. **Select Branch** (for TTree): Choose which branch to plot
4. **Set Entry Range**: 
   - Start entry: First event to include (default: 0)
   - End entry: Last event to include (default: all)
5. **Add Cut Formula** (optional):
   ```
   Energy > 100
   Energy > 50 && Pt > 30
   abs(Eta) < 2.5
   ```
6. **Add to Chain**: Click to save this selection step
7. **Repeat**: Add more steps to build a chain
8. **Plot**: 
   - "Plot Current Only": Just the current settings
   - "Plot with Chain Cuts": All cuts applied cumulatively

#### Selection Chain Example
```
Step 1: Events:Energy | Cut: Energy > 70      → ~7000 events
Step 2: Events:Pt     | Cut: Pt > 30          → ~3500 events  
Step 3: Events:Eta    | Cut: abs(Eta) < 2.0   → ~2800 events

Final plot shows Energy distribution with ALL cuts applied!
```

#### Saving and Loading Chains
- **Save Chain**: Click "Save Chain" → Choose filename → `.selchain` file created
- **Load Chain**: Click "Load Chain" → Select `.selchain` file → All steps restored
- Chain files are plain text and version-control friendly

#### Important Notes
- **Histograms**: Only entry/bin range works, cuts are ignored (histograms are pre-binned)
- **TTrees**: Both entry ranges AND cuts work
- **Entry$ Variable**: Special ROOT variable for entry number in cuts
  ```
  Entry$ < 1000              # First 1000 events
  Entry$ >= 5000 && Entry$ < 10000  # Events 5000-10000
  ```

### Creating Plots

#### From CSV Data:
1. Load CSV file (see above)
2. Click **"Add Plot"** button
3. **Column Selector Dialog** opens:
   - Select plot type (TGraph, TGraphErrors, TH1, TH2, TH3)
   - Select X-axis column
   - Select Y-axis column (for scatter/2D plots)
   - Select Z-axis column (for 3D plots)
   - Optionally select error columns (X error, Y error)
4. Click **"OK"** to add to plot list
5. Configure canvas options:
   - Same canvas (overlay) or separate canvases
   - Divided canvas with rows/columns
6. Click **"Create Plots"**

#### From ROOT Files:
1. Load ROOT file (see above)
2. **Option A**: Select objects in browser → Click "Load Selected"
3. **Option B**: Open TBrowser and drag object to GUI window
4. **Option C**: Use Entry Selector for advanced TTree filtering
5. Plots automatically created in canvases

### Using Drag & Drop

The GUI supports drag-and-drop from ROOT's TBrowser:

1. **Open TBrowser**: `TBrowser b;` in ROOT terminal or click "TBrowser Only" in file browser
2. **Navigate** to your ROOT file
3. **Drag histogram or graph** from TBrowser
4. **Drop onto the GUI main window**
5. **Plot automatically created** with:
   - TH1/TH2/TH3 → histogram plot
   - TGraph → graph plot with markers
   - TTree → information displayed (use Entry Selector for plotting)

### Using the Script Engine (NEW)

The integrated script engine supports both ROOT/C++ and Python.

#### Command Box (Single Line Execution)
Type commands in the bottom entry box and press **Enter**:

```cpp
// ROOT/C++ commands:
cout << "Hello ROOT" << endl
TH1F *h = new TH1F("h","Test",100,-5,5)
h->FillRandom("gaus",10000)
h->Draw()
gDirectory->ls()
```

#### Script Editor (Multi-line Scripts)
Write multi-line scripts in the editor and click **"Run Script"**:

```cpp
void myAnalysis() {
    TCanvas *c = new TCanvas("c","Analysis",1200,800);
    c->Divide(2,2);
    
    // Plot 1: Gaussian
    c->cd(1);
    TH1F *h1 = new TH1F("h1","Gaussian",100,-5,5);
    h1->FillRandom("gaus",10000);
    h1->Draw();
    
    // Plot 2: Exponential
    c->cd(2);
    TH1F *h2 = new TH1F("h2","Exponential",100,0,10);
    h2->FillRandom("expo",10000);
    h2->Draw();
    
    // Plot 3: Scatter
    c->cd(3);
    TGraph *g = new TGraph(50);
    for(int i=0; i<50; i++) {
        g->SetPoint(i, i, sin(i*0.2)*exp(-i/20.0));
    }
    g->Draw("APL");
    
    // Plot 4: 2D histogram
    c->cd(4);
    TH2F *h3 = new TH2F("h3","2D Dist",50,-3,3,50,-3,3);
    h3->FillRandom("gaus",50000);
    h3->Draw("COLZ");
    
    c->Update();
}
```

Then in command box: `myAnalysis()`

#### Python Scripts
Select "Python" from language dropdown:

```python
import ROOT

# Create canvas
c = ROOT.TCanvas("c", "Python Plot", 800, 600)

# Create and fill histogram
h = ROOT.TH1F("h", "Energy Distribution;Energy [GeV];Events", 100, 0, 200)
for i in range(10000):
    h.Fill(ROOT.gRandom.Gaus(100, 15))

h.SetLineColor(ROOT.kBlue)
h.SetFillColor(ROOT.kCyan)
h.Draw()
c.Update()
```

#### Loading External Scripts
- Click **"Load..."** to load `.C`, `.cpp`, or `.py` files
- Click **"Save..."** to save current script
- Scripts persist between sessions

#### Important Script Notes
- **Multi-line C++**: Must wrap in function (see examples above)
- **Single-line C++**: Can execute directly
- **Python**: No wrapping needed
- **Access loaded files**: Use `gDirectory->GetFile()` in scripts

### Advanced Fitting (NEW)

The GUI includes comprehensive fitting capabilities:

#### Available Fit Functions
- **Gaussian**: Standard normal distribution
- **Polynomial**: Pol0 (constant), Pol1 (linear), Pol2-Pol4 (higher order)
- **Exponential**: Decay functions
- **Sine Functions**: Sine, Sine + Offset, Damped Sine (for oscillatory data)
- **Custom**: Enter any ROOT formula

#### Using Fits
1. Select fit function from dropdown
2. For custom fits, enter formula: `[0]*x^2 + [1]*x + [2]`
3. Create plots normally
4. Fit is applied automatically
5. Fit parameters displayed with uncertainties

#### RooFit Integration
For Gaussian fits on histograms, RooFit is automatically used:
- More robust parameter estimation
- Proper error propagation
- Chi-square and NDF displayed

### Customizing Plots

**Axis Labels:**
- Automatically uses column headers from CSV
- Or uses ROOT object titles
- Can be customized in ROOT canvas

**Plot Style:**
- Right-click on canvas for ROOT context menu
- Access SetLogy(), SetLogx(), SetLogz()
- Change marker styles, colors
- Modify histogram fill colors

**Export:**
- Right-click canvas → "Save As"
- Supported formats: PDF, PNG, EPS, SVG, ROOT

## Example Workflows

### Example 1: Simple CSV Scatter Plot
```csv
# sample_data.csv
x,y,xerr,yerr,z
1.0,2.5,0.1,0.2,5.0
2.0,3.8,0.1,0.3,7.5
3.0,5.2,0.15,0.25,9.0
4.0,7.1,0.2,0.4,11.5
```

1. Click "Browse" → Load `sample_data.csv`
2. Click "Add Plot"
3. Select: TGraphErrors, X=x, Y=y, X-Error=xerr, Y-Error=yerr
4. Click "OK"
5. Click "Create Plots"
6. Result: Scatter plot with error bars

### Example 2: Advanced TTree Event Selection (NEW)

Create test data:
```bash
cd RootEntrySelector
./test_entry_selector  # Creates test_data.root with physics events
```

Analysis workflow:
```
1. Open Entry Selector with test_data.root
2. Select "Events" tree
3. Add selection chain:
   
   Step 1: 
   - Branch: Energy
   - Cut: Energy > 70
   - Click "Add to Chain"
   
   Step 2:
   - Branch: Pt  
   - Cut: Pt > 30
   - Click "Add to Chain"
   
   Step 3:
   - Branch: Eta
   - Cut: abs(Eta) < 2.0
   - Click "Add to Chain"

4. Click "Plot with Chain Cuts"
5. Result: Histogram showing Energy distribution for events passing ALL cuts

6. Save chain: Click "Save Chain" → "physics_cuts.selchain"
7. Later: Click "Load Chain" → Select "physics_cuts.selchain" → All steps restored!
```

### Example 3: Script-Based Analysis (NEW)

```cpp
// Load in script editor:
void quickAnalysis() {
    // Get the loaded ROOT file
    TFile *f = (TFile*)gDirectory->GetFile();
    if (!f) {
        cout << "No file loaded!" << endl;
        return;
    }
    
    // Get tree
    TTree *tree = (TTree*)f->Get("Events");
    if (!tree) {
        cout << "Tree not found!" << endl;
        return;
    }
    
    // Apply cuts and plot
    TCanvas *c = new TCanvas("c", "Analysis", 1200, 400);
    c->Divide(3,1);
    
    c->cd(1);
    tree->Draw("Energy", "Energy > 50");
    
    c->cd(2);
    tree->Draw("Pt", "Pt > 20 && Energy > 50");
    
    c->cd(3);
    tree->Draw("Eta:Phi", "Energy > 100", "COLZ");
    
    c->Update();
    
    cout << "Total events: " << tree->GetEntries() << endl;
    cout << "Passed Energy>50: " << tree->GetEntries("Energy > 50") << endl;
}
```

Execute: Type `quickAnalysis()` in command box and press Enter

### Example 4: Fitting Analysis (NEW)

```
1. Load ROOT file with histogram or create from CSV
2. Select fit function: "Gaussian"
3. Add plot, create plots
4. Fit automatically applied
5. Fit parameters shown on plot:
   - Mean ± error
   - Sigma ± error  
   - Chi²/NDF
6. Right-click → Save As → Export with fit
```

## Features in Detail

### CSV Preview Dialog
- **Live Preview**: See first 100 lines formatted as table
- **Delimiter Auto-detection**: Smart detection of common delimiters
- **Custom Delimiters**: Support any single-character delimiter
- **Header Detection**: Automatically identifies header rows
- **Skip Rows**: Skip metadata or comments at file start
- **Encoding Support**: UTF-8, ASCII, ISO-8859-1
- **Large File Handling**: Efficient loading of files with 100K+ rows

### Column Selector Dialog  
- **Plot Type Selection**: TGraph, TGraphErrors, TH1, TH2, TH3
- **Visual Column Selection**: Easy picking with dropdowns
- **Data Type Detection**: Automatic numeric/string detection
- **Error Column Support**: Optional X/Y error bars
- **3D Plot Support**: Optional Z column for 3D plots
- **Smart Defaults**: Auto-selects appropriate columns

### ROOT File Browser (NEW)
- **Object Tree View**: Hierarchical display of file contents
- **Type Filtering**: Separate categories for histograms, graphs, trees
- **Multi-select**: Load multiple objects at once
- **TTree Details**: Shows entry count and branch information
- **TBrowser Integration**: Option to open traditional TBrowser
- **Double-click Plot**: Instantly plot by double-clicking object

### ROOT Entry Selector (NEW)
- **Entry Range Selection**: Choose specific event ranges
- **Cut Formulas**: Apply physics cuts with ROOT syntax
- **Selection Chains**: Build multi-step filter pipelines
- **Chain Persistence**: Save/load selection workflows
- **Real-time Feedback**: See filtered entry counts
- **Branch Selection**: Choose which TTree branch to plot
- **Histogram Support**: Bin range selection for histograms
- **Documentation**: Comprehensive user guides and examples

### Script Engine (NEW)
- **Dual Interface**: Command box for single lines, editor for scripts
- **Multi-language**: ROOT/C++ and Python support
- **File I/O**: Load/save script files
- **Output Capture**: Script output shown in dedicated panel
- **Auto-complete**: Available for ROOT commands (if enabled)
- **History**: Command history with arrow keys

### Advanced Fitting (NEW)
- **Multiple Functions**: Gaussian, polynomial, exponential, sine
- **Custom Functions**: Any ROOT formula supported
- **RooFit Integration**: Automatic for Gaussian fits on histograms
- **Parameter Display**: Fit results with uncertainties
- **Chi-square**: Quality metrics shown
- **Visual Overlay**: Fit curve drawn on plots

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `Ctrl+O` | Open file browser |
| `Ctrl+S` | Save current canvas |
| `Ctrl+Q` | Quit application |
| `Ctrl+N` | New canvas |
| `Ctrl+W` | Close current canvas |
| `Enter` | Execute command (in command box) |
| `Ctrl+Enter` | Run script (in script editor) |

## Configuration Files

### CMakeLists.txt
Configure build options:
```cmake
# Enable/disable features
option(BUILD_TESTING "Build tests" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

# Set ROOT components
find_package(ROOT REQUIRED COMPONENTS 
    Core Hist Graf Gpad Tree RIO 
    RooFit RooFitCore  # For advanced fitting
    Gui Rint           # For GUI
    ROOTTPython        # For Python support
)
```

### Entry Selector Integration
To integrate Entry Selector into main GUI:
1. See `RootEntrySelector/docs/INTEGRATION_GUIDE.md`
2. Add includes and create RootEntrySelector instance
3. Add menu item or button to launch
4. Full integration examples provided

## Known Issues & Limitations

### Current Limitations
- CSV files must be properly formatted (no jagged rows)
- Maximum recommended file size: 10 million rows
- Drag-and-drop only works with ROOT 6.26+
- Some ROOT object types not yet supported (THStack, TMultiGraph)
- Script editor doesn't have syntax highlighting
- Entry Selector standalone tool (integration guide provided)

### Known Issues
1. **Large TTree files**: Loading very large TTrees (>1GB) may be slow
2. **X11 Rendering**: Some display issues on Wayland sessions (use X11)
3. **Memory**: Large datasets may require significant RAM
4. **Script Errors**: Multi-line C++ scripts must be wrapped in functions

### Workarounds
- For large files: Load specific branches only or use Entry Selector
- For Wayland: Set `GDK_BACKEND=x11` environment variable
- For memory issues: Process data in chunks
- For scripts: See Script Troubleshooting Guide

## Troubleshooting

### "Cannot open libAdvancedPlotGUI.so"
```bash
# Add build directory to library path
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)
```

### "Dictionary generation failed"
```bash
# Clean and rebuild
rm -rf build/
mkdir build && cd build
cmake .. && make clean && make
```

### "Entry Selector crashes with segfault"
This is usually due to `Form()` function issues. The latest version has fixed this:
```bash
cd RootEntrySelector
make clean
make
# Ensure you're using the fixed versions from outputs folder
```

### "Branch selection not working in Entry Selector"
Use the fixed `AddSelectionStep()` from `AddSelectionStep_PROPER_FIX.cpp` that:
- Properly captures selected branch from combo box
- Verifies branch exists in tree
- Falls back to first branch with warning if none selected

### "Plot with Chain Cuts returns -1 entries"
Ensure branch name is included in chain steps:
- The chain should store "TreeName:BranchName" format
- Use fixed `PlotWithChain()` that auto-selects first branch if missing
- Check console output for detailed error messages

### "Script won't execute"
Common issues:
- **Multi-line scripts**: Must wrap in function (void myFunc() { ... })
- **Single commands**: Can execute directly
- **Syntax errors**: Check ROOT syntax
- **Missing semicolons**: C++ requires them
- See `SCRIPT_TROUBLESHOOTING_GUIDE.txt` for complete guide

### "CSV preview dialog doesn't show"
- Check file permissions: `ls -l your_file.csv`
- Verify file is valid CSV/text format
- Try with provided sample_data.csv
- Check for non-ASCII characters

### "Drag & drop not working"
- Ensure you're using ROOT 6.26 or higher: `root --version`
- Check X11 is running: `echo $DISPLAY`
- Restart application and TBrowser
- Make sure you drag from TBrowser, not file manager

### "Buttons remain disabled after loading CSV"
- Check console for error messages
- Verify CSV has numeric data
- Try with different delimiter settings
- Ensure at least 2 columns of numbers

## Development

### Building with Debug Symbols
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
gdb ./AdvancedPlotGUIApp
```

### Adding Custom Features

**New Plot Type:**
1. Add plot type enum in `PlotTypes.h`
2. Implement creator in `PlotTypes.cpp`
3. Add UI controls in `ColumnSelectorDialog`
4. Update `PlotManager` to handle new type

**New File Format:**
1. Add loader in `FileHandler.cpp`
2. Create preview dialog (follow `CSVPreviewDialog` pattern)
3. Update UI to include new file type
4. Add to file type filters

**Integrating Entry Selector:**
1. Follow `RootEntrySelector/docs/INTEGRATION_GUIDE.md`
2. Add launcher button/menu in main GUI
3. Pass file path to RootEntrySelector
4. Handle returned selections

### Code Style
- Follow ROOT coding conventions
- Use `ClassDef`/`ClassImp` macros for ROOT classes
- Document public methods with Doxygen comments
- Keep functions under 50 lines when possible
- Use `snprintf()` instead of `Form()` for string construction

### Testing

**Basic Functionality:**
```bash
./AdvancedPlotGUIApp
# Test CSV loading with sample_data.csv
# Test ROOT file loading
# Test drag & drop from TBrowser
# Test script execution
```

**Entry Selector:**
```bash
cd RootEntrySelector
./test_entry_selector
# Test entry range selection
# Test cut formulas
# Test chain building
# Test save/load chains
# Test plotting with cuts
```

**Script Engine:**
```bash
# Test single commands
# Test multi-line scripts  
# Test Python execution
# Test load/save scripts
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes with clear commit messages
4. Test thoroughly with sample data
5. Document new features in README
6. Submit a pull request

### Priority Areas
- Entry Selector GUI integration into main application
- Script editor syntax highlighting
- Additional fit functions
- THStack and TMultiGraph support
- Improved error handling
- Unit tests

## License

[Specify your license here - MIT, GPL, etc.]

## Citation

If you use this software in your research, please cite:

```bibtex
@software{advanced_root_plotting_gui,
  author = {Your Name},
  title = {Advanced ROOT Plotting GUI},
  version = {2.3},
  year = {2024},
  url = {https://github.com/yourusername/Advance_ROOT_Plotting_dev}
}
```

## Contact & Support

- **Issues**: Open an issue on GitHub
- **Email**: your.email@institution.edu
- **ROOT Forum**: https://root-forum.cern.ch/
- **Documentation**: See `RootEntrySelector/docs/` for detailed guides

## Acknowledgments

- Built with [ROOT Data Analysis Framework](https://root.cern/)
- Inspired by ROOT's TBrowser and TCanvas
- Thanks to the ROOT development team
- RooFit integration for advanced fitting

## Version History

### v2.3 (Current - February 2026)
- **NEW**: Advanced ROOT Entry Selector with chained filtering
  - Entry range selection for TTrees and histograms
  - Complex cut formulas with ROOT syntax
  - Sequential filter chains
  - Save/load selection workflows
  - Standalone tool with integration guide
- **NEW**: Integrated script engine
  - ROOT/C++ and Python script execution
  - Multi-line script editor
  - Command-line interface
  - Load/save external scripts
  - Output capture panel
- **NEW**: Advanced fitting capabilities
  - Multiple predefined fit functions
  - Custom function support
  - RooFit integration for Gaussian fits
  - Fit parameter display with uncertainties
- **NEW**: ROOT File Browser dialog
  - Interactive object tree view
  - Multi-select support
  - TTree branch information
  - Double-click to plot
- **IMPROVED**: Error handling throughout
- **IMPROVED**: Debug output for troubleshooting
- **FIXED**: Form() usage replaced with snprintf() for stability
- **FIXED**: Branch selection in TTree plotting
- **FIXED**: Memory leaks and segfaults
- **DOCS**: Comprehensive documentation added

### v2.2
- CSV preview dialog with live formatting
- Column selector improvements
- Drag-and-drop from TBrowser
- Multiple canvas management
- Export in multiple formats

### v2.1
- ROOT file support
- TTree basic handling
- Histogram and graph loading
- Initial GUI framework

### v2.0
- Complete rewrite with modern C++17
- ROOT 6.26+ support
- Improved stability

### v1.0 (Initial)
- Basic CSV file support
- Simple plotting capabilities
- Column selection
- Canvas management

### Planned Features (v2.4)
- Full Entry Selector integration into main GUI
- Script editor syntax highlighting
- Auto-complete for ROOT commands
- Plot templates and style manager
- Batch processing mode
- Additional file format support (HDF5, Parquet)
- Unit test framework
- Performance profiling tools

---

**Last Updated**: February 18, 2026  
**Version**: 2.3  
**Maintained by**: [Your Name]

---

## Quick Start Guide

**For CSV Analysis:**
1. Launch: `./AdvancedPlotGUIApp`
2. Click "Browse" → Select CSV
3. Configure delimiter → Click "Load"
4. Click "Add Plot" → Select columns → OK
5. Click "Create Plots"

**For ROOT File Analysis:**
1. Launch: `./AdvancedPlotGUIApp`
2. Click "Browse" → Select .root file
3. Select objects in browser → "Load Selected"
4. Plots automatically created

**For Advanced TTree Filtering:**
1. Launch Entry Selector: `cd RootEntrySelector && ./test_entry_selector`
2. Select tree → Select branch
3. Enter cut formula → "Add to Chain"
4. Repeat for multiple cuts
5. "Plot with Chain Cuts" → See result
6. "Save Chain" to preserve workflow

**For Script-Based Analysis:**
1. Launch main GUI
2. Select "ROOT/C++" or "Python" language
3. Type commands in command box (bottom) + Enter
4. Or write multi-line script in editor + "Run Script"
5. Access loaded files with `gDirectory->GetFile()`

**Need Help?**
- See `RootEntrySelector/docs/` for detailed tutorials
- Check console output for error messages
- Consult `SCRIPT_TROUBLESHOOTING_GUIDE.txt` for script issues
- Open an issue on GitHub with:
  - What you're trying to do
  - What error you see
  - Console output
  - ROOT version: `root --version`
