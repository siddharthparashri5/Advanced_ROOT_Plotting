# Advanced ROOT Plotting GUI

A modern, user-friendly graphical interface for creating publication-quality plots with ROOT, supporting both CSV data files and ROOT files with drag-and-drop functionality.

![Version](https://img.shields.io/badge/version-2.2-blue)
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

### 📈 Plotting Capabilities
- **Multiple Plot Types**:
  - Scatter plots with error bars
  - Line plots
  - Histograms (1D, 2D, 3D)
  - Error bar plots
  - Color maps and surface plots
  
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

## Requirements

### System Requirements
- **Operating System**: Linux (Ubuntu 20.04+ recommended)
- **ROOT**: Version 6.26 or higher
- **C++ Compiler**: GCC 9.0+ with C++17 support
- **CMake**: Version 3.10 or higher

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
│   ├── CSVPreviewDialog.cpp      
│   ├── ColumnSelectorDialog.cpp       
│   ├── ColumnSelectorDict.cpp
│   ├── PlotManager.cpp
│   ├── FileHandler.cpp
│   ├── PloTypes.cpp
│   ├── ProjectDict.cpp
│   ├── RootDataInspector.cpp
│   ├── ROOTFileBrowser.cpp
│   ├── ScriptEngine.cpp
│   
├── include/
│   ├── AdvancedPlotGUI.h       # Main GUI implementation
│   ├── CSVPreviewDialog.h      
│   ├── ColumnSelectorDialog.h      
│   ├── ColumnSelectorLinkDef.h
│   ├── PlotManager.h
│   ├── DataReader.h
│   ├── FileHandler.h
│   ├── PloTypes.h
│   ├── Errorhandling.h
│   ├── FitUtils.h
│   ├── RootDataInspector.h
│   ├── ROOTFileBrowser.h
│   ├── ScriptEngine.h
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

1. **Click "Load CSV/Text File"**
2. **Select your data file** in the file browser
3. **Configure import settings** in the preview dialog:
   - Choose delimiter (comma, semicolon, tab, etc.)
   - Set whether first row contains headers
   - Skip metadata rows if needed
   - Preview shows formatted table
4. **Click "Load"** to import the data
5. **Success dialog** confirms the data is loaded

### Loading ROOT Files

1. **Click "Load ROOT File"**
2. **Select your .root file** in the file browser
3. **Content listing dialog** shows:
   - All histograms (TH1*, TH2*, TH3*)
   - All graphs (TGraph*)
   - All TTrees with branch information
4. **Select objects** to load into the plotting interface
5. Objects appear in the "Add Plot" column selector

### Creating Plots

#### From CSV Data:
1. Load CSV file (see above)
2. Click **"Add Plot"** button
3. **Column Selector Dialog** opens:
   - Select X-axis column
   - Select Y-axis column
   - Optionally select error columns (X error, Y error)
   - Optionally select Z column for 3D plots
4. Click **"OK"** to create the plot
5. Plot appears in a new canvas with:
   - Error bars (if error columns selected)
   - Legend
   - Grid (optional)
   - Proper axis labels

#### From ROOT Files:
1. Load ROOT file (see above)
2. **Option A**: Click object in content listing → loads to column selector
3. **Option B**: Open TBrowser and drag object to the GUI window
4. Plot is automatically created in a new canvas

### Using Drag & Drop

The GUI supports drag-and-drop from ROOT's TBrowser:

1. **Open TBrowser**: `TBrowser b;` in ROOT terminal
2. **Navigate** to your ROOT file
3. **Drag histogram or graph** from TBrowser
4. **Drop onto the GUI main window**
5. **Plot automatically created** with:
   - TH1/TH2/TH3 → histogram plot
   - TGraph → graph plot with markers
   - TTree → information displayed (select branches to plot)

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

1. Load `sample_data.csv`
2. Click "Add Plot"
3. Select: X=x, Y=y, X-Error=xerr, Y-Error=yerr
4. Result: Scatter plot with error bars

### Example 2: ROOT Histogram Comparison
```bash
# Create test ROOT file
root -l -q -e 'TFile f("test.root","RECREATE"); TH1F h1("h1","Signal",100,-5,5); h1.FillRandom("gaus",1000); h1.Write(); TH1F h2("h2","Background",100,-5,5); h2.FillRandom("expo",1000); h2.Write(); f.Close();'
```

1. Load `test.root` 
2. Select both h1 and h2 from content listing
3. Plots created in separate canvases
4. Use ROOT to overlay: `h1->Draw(); h2->SetLineColor(kRed); h2->Draw("same");`

### Example 3: TTree Analysis
```bash
# If your ROOT file contains TTrees:
1. Load ROOT file
2. Content listing shows TTree with branches
3. Select branches of interest
4. Create scatter plot or histogram from branches
```

## Features in Detail

### CSV Preview Dialog
- **Live Preview**: See first 100 lines formatted as table
- **Delimiter Auto-detection**: Smart detection of common delimiters
- **Custom Delimiters**: Support any single-character delimiter
- **Header Detection**: Automatically identifies header rows
- **Skip Rows**: Skip metadata or comments at file start
- **Encoding Support**: UTF-8, ASCII
- **Large File Handling**: Efficient loading of files with 100K+ rows

### Column Selector Dialog
- **Visual Selection**: Easy column picking with checkboxes
- **Multi-select**: Choose multiple columns at once
- **Column Preview**: See first few values
- **Data Type Detection**: Automatic numeric/string detection
- **Error Column Support**: Optional X/Y error bars
- **3D Plot Support**: Optional Z column for 3D plots

### ROOT Object Handling
- **Type Detection**: Automatic object type identification
- **Smart Plotting**: Appropriate plot style for each object type
- **Error Preservation**: Maintains error bars from TGraphErrors
- **Histogram Options**: Supports all TH1/TH2/TH3 draw options
- **Tree Browsing**: Lists all branches with types

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `Ctrl+O` | Open file browser |
| `Ctrl+S` | Save current canvas |
| `Ctrl+Q` | Quit application |
| `Ctrl+N` | New canvas |
| `Ctrl+W` | Close current canvas |

## Configuration Files

### CMakeLists.txt
Configure build options:
```cmake
# Enable/disable features
option(BUILD_TESTING "Build tests" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

# Set ROOT components
find_package(ROOT REQUIRED COMPONENTS Core Hist Graf Gpad Tree RIO)
```

### LinkDef.h
ROOT dictionary generation for custom classes:
```cpp
#ifdef __CLING__
#pragma link C++ class AdvancedPlotGUI+;
#pragma link C++ class CSVPreviewDialog+;
#pragma link C++ class ColumnSelector+;
#pragma link C++ class FileHandler+;
#endif
```

## Known Issues & Limitations

### Current Limitations
- CSV files must be properly formatted (no jagged rows)
- Maximum recommended file size: 10 million rows
- Drag-and-drop only works with ROOT 6.26+
- Some ROOT object types not yet supported (THStack, TMultiGraph)

### Known Issues
1. **Large TTree files**: Loading very large TTrees (>1GB) may be slow
2. **X11 Rendering**: Some display issues on Wayland sessions (use X11)
3. **Memory**: Large datasets may require significant RAM

### Workarounds
- For large files: Load specific branches only
- For Wayland: Set `GDK_BACKEND=x11` environment variable
- For memory issues: Process data in chunks

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

### "CSV preview dialog doesn't show"
- Check file permissions: `ls -l your_file.csv`
- Verify file is valid CSV/text format
- Try with provided sample_data.csv

### "Drag & drop not working"
- Ensure you're using ROOT 6.26 or higher: `root --version`
- Check X11 is running: `echo $DISPLAY`
- Restart application and TBrowser

### "Buttons remain disabled after loading CSV"
- Check console for error messages
- Verify CSV has numeric data
- Try with different delimiter settings

## Development

### Building with Debug Symbols
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
gdb ./AdvancedPlotGUIApp
```

### Adding Custom Features

**New Plot Type:**
1. Add plot type enum in `AdvancedPlotGUI.h`
2. Implement plotting logic in `AdvancedPlotGUI.cpp`
3. Add UI controls if needed

**New File Format:**
1. Add loader in `FileHandler.cpp`
2. Create preview dialog (follow `CSVPreviewDialog` pattern)
3. Update UI to include new file type

### Code Style
- Follow ROOT coding conventions
- Use `ClassDef`/`ClassImp` macros for ROOT classes
- Document public methods with Doxygen comments
- Keep functions under 50 lines when possible

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes with clear commit messages
4. Test thoroughly with sample data
5. Submit a pull request

### Testing
```bash
# Test with provided sample data
./AdvancedPlotGUIApp
# Load sample_data.csv
# Verify all features work

# Test with ROOT files
root -l -q test_data_generator.C  # Create test files
# Load test.root in application
```

## License

[Specify your license here - MIT, GPL, etc.]

## Citation

If you use this software in your research, please cite:

```bibtex
@software{advanced_root_plotting_gui,
  author = {Your Name},
  title = {Advanced ROOT Plotting GUI},
  year = {2024},
  url = {https://github.com/yourusername/Advance_ROOT_Plotting_dev}
}
```

## Contact & Support

- **Issues**: Open an issue on GitHub
- **Email**: your.email@institution.edu
- **ROOT Forum**: https://root-forum.cern.ch/

## Acknowledgments

- Built with [ROOT Data Analysis Framework](https://root.cern/)
- Inspired by ROOT's TBrowser and TCanvas
- Thanks to the ROOT development team

## Version History

### v1.0.0 (Current)
- Initial release
- CSV file support with preview dialog
- ROOT file support with content listing
- Drag & drop from TBrowser
- Multiple plot types
- Column selector dialog
- Multi-canvas support

### Planned Features (v1.1.0)
- ROOT file content browser dialog
- Direct drag-and-drop to column selector
- THStack and TMultiGraph support
- Plot templates and styles
- Batch processing mode
- Python binding

---

**Last Updated**: February 2026
**Maintained by**: [Your Name]
