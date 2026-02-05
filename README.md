# Advanced ROOT Plotting - User Manual

**Version:** 2.0  
**Date:** February 5, 2026  
**Author:** Siddharth Parashri

---

## Table of Contents

1. [Introduction](#introduction)
2. [What's New in Version 2.0](#whats-new-in-version-20)
3. [Quick Start Guide](#quick-start-guide)
4. [Installation](#installation)
5. [Data Preparation](#data-preparation)
6. [Workflow](#workflow)
7. [Script Panel Guide](#script-panel-guide)
8. [Extending The Code](#extending-the-code)
9. [Troubleshooting](#troubleshooting)
10. [License](#license)

---

## Introduction

Advanced ROOT Plotting is a comprehensive graphical user interface application built on the ROOT framework, designed to provide intuitive and powerful data visualization capabilities for scientific data analysis. Whether you're analyzing particle physics data, statistical distributions, or general scientific measurements, this tool offers comprehensive plotting, fitting, and **scripting** features in an easy-to-use interface.

### Key Features

- **FULL ROOT Support**: Built-in ROOT support to edit and plot data
- **Multi-format Support**: Load data from ROOT files, CSV, and text formats
- **Versatile Plotting**: Create TGraph, TGraphErrors, TH1D, TH2D, TH3D plots
- **Advanced Fitting**: Fit data with Gaussian, polynomial, exponential, or custom functions
- **RooFit Integration**: Advanced statistical modeling with RooFit
- **Interactive Canvas**: Zoom, pan, and interact with your plots
- **Script Panel**: Execute ROOT/C++ and Python scripts directly in the GUI
- **Command Line Interface**: Interactive command execution with output capture
- **Export Capabilities**: Save plots in multiple formats (PNG, PDF, EPS, ROOT)
- **Customization**: Full control over colors, markers, styles, and layouts

---

## What's New in Version 2.0

### 🎯 Major Features

#### 1. **Integrated Script Panel**
- **Multi-line Script Editor**: Write and execute complete ROOT/C++ or Python scripts
- **Command Line Interface**: Single-line command execution for quick operations
- **Output Viewer**: Separate tab for viewing script output with stream redirection
- **File Operations**: Load and save scripts with auto-language detection

#### 2. **Dual Language Support**
- **ROOT/C++**: Execute ROOT macros and C++ code
- **Python**: Run Python scripts with full ROOT bindings via TPython
- **Language Selection**: Easy toggle between C++ and Python modes

#### 3. **Enhanced ROOT File Support**
- **TBrowser Integration**: ROOT files now open directly in TBrowser
- **Full Functionality**: Access all ROOT file objects (histograms, trees, graphs)
- **Interactive Exploration**: Navigate complex ROOT file structures

#### 4. **Improved Architecture**
- **Organized Directory Structure**: Separate `include/`, `src/`, and `obj/` directories
- **Professional Makefile**: Multiple build targets (debug, release, install)
- **Better Code Organization**: Modular design for easier maintenance

#### 5. **Stream Redirection**
- **Captured Output**: All `std::cout` and `std::cerr` redirected to Output tab
- **Clean Interface**: Separate input and output areas
- **Real-time Display**: See results as scripts execute

### 🔧 Improvements
- Support for all TH1 variants (TH1D, TH1F, TH1I, etc.)
- Support for all TH2 and TH3 variants
- Better error handling and user feedback
- Enhanced canvas management
- Improved memory management

---

## Quick Start Guide

### Prerequisites

- **ROOT 6.x or later** ([https://root.cern.ch](https://root.cern.ch))
- **C++ compiler** (GCC 7+ or Clang 5+)
- **Python 3.x** with development libraries
- **Make** build system

### Project Structure 
```
Advance_ROOT_Plotting/
├── include/                    # Header files
│   ├── AdvancedPlotGUI.h
│   ├── ColumnSelector.h
│   ├── DataReader.h
│   ├── PlotTypes.h
│   ├── FitUtils.h
│   ├── ErrorHandling.h
│   └── ColumnSelectorLinkDef.h
├── src/                        # Source files
│   ├── AdvancedPlotGUI.cpp
│   └── ColumnSelector.cpp
├── obj/                        # Object files (auto-generated)
├── sample_data/                # Example data files
│   ├── sample_data.csv
│   ├── sample_data.txt
│   └── sample_data.root
├── Makefile                    # Build configuration
├── README.md                   # This file
└── LICENSE                     # GNU GPL v3
```

---

## Installation

### Option 1: Quick Setup (Recommended)

```bash
# Clone the repository
git clone https://github.com/siddharthparashri5/Advance_ROOT_Plotting.git
cd Advance_ROOT_Plotting

# Setup directory structure
make setup

# Move files to appropriate directories (if not already organized)
# mv *.h include/
# mv *.cpp src/

# Build
make

# Run
./AdvancedPlotGUI
```

### Option 2: Step-by-Step Build

```bash
# 1. Clone repository
git clone https://github.com/siddharthparashri5/Advance_ROOT_Plotting.git
cd Advance_ROOT_Plotting

# 2. Check dependencies
make check-deps

# 3. Create directory structure
make setup

# 4. Organize files
mv *.h include/ 2>/dev/null || true
mv *.cpp src/ 2>/dev/null || true

# 5. Build the application
make

# 6. Run
./AdvancedPlotGUI
```

### Build Options

```bash
make              # Standard build
make debug        # Build with debug symbols
make release      # Optimized build
make clean        # Clean build artifacts
make distclean    # Complete cleanup
make install      # Install to /usr/local/bin (requires sudo)
make help         # Show all available commands
```

---

## Data Preparation

### Supported File Formats

#### 1. Text/DAT Files (Space or Tab Separated)
```
# Optional comment lines starting with #
# Comments are ignored during parsing
x1    y1    x2    y2    x1_err    y1_err
0.0   1.5   2.3   4.1   0.01      0.2
1.0   2.3   3.4   5.2   0.003     0.1
2.0   3.1   4.5   6.3   0.02      0.15
...
```

#### 2. CSV Files (Comma Separated)
```
# Optional comment lines
x,y,xerr,yerr
0.0,1.5,0.1,0.2
1.0,2.3,0.1,0.3
2.0,3.1,0.15,0.25
...
```

#### 3. ROOT Files
- Opens automatically in TBrowser
- Supports:
  - TH1, TH2, TH3 histograms (all variants: D, F, I, S, C)
  - TGraph, TGraphErrors
  - TTrees
  - TCanvas with embedded plots

### Data Format Guidelines

- **Headers**: First non-comment line is treated as column headers
- **Column Names**: Used as axis titles and legend labels
- **Comments**: Lines starting with `#` are ignored
- **Missing Values**: Handled gracefully, skipped during parsing
- **Error Columns**: Optional, specify during plot configuration

---

## Workflow

### 1. Launch the Application

```bash
./AdvancedPlotGUI
```

The GUI window will appear with several panels:
- **Data File Panel**: Load data files
- **Plot Configuration Panel**: Configure plots
- **Canvas Options**: Set canvas layout
- **Fit Options**: Select fit functions
- **Script Panel**: Execute scripts and commands

### 2. Load Your Data

#### For Text/CSV Files:
1. Click **"Browse..."** button
2. Select your data file (.csv, .txt, .dat)
3. Click **"Load"**
4. A dialog shows the number of columns and rows loaded
5. Click **"Add Plot..."** to configure plots

#### For ROOT Files:
1. Click **"Browse..."** button
2. Select your ROOT file (.root)
3. Click **"Load"**
4. **TBrowser opens automatically**
5. Double-click objects in TBrowser to view them
6. Use the terminal or script panel for advanced operations

### 3. Configure Plots

1. Click **"Add Plot..."** button
2. **Select Plot Type**:
   - TGraph (x vs y scatter plot)
   - TGraphErrors (with error bars)
   - TH1D (1D histogram)
   - TH2D (2D histogram)
   - TH3D (3D histogram)

3. **Select Columns**:
   - X Column
   - Y Column (for graphs and 2D/3D histograms)
   - Z Column (for 3D histograms)
   - X/Y Error Columns (for TGraphErrors)

4. Click **"OK"**
5. Repeat to add multiple plots

### 4. Set Canvas Options

Choose one of three display modes:

#### Same Canvas (Overlay)
- Check **"Same canvas (Overlay)"**
- All plots drawn on one canvas
- Automatic legend generation
- Different colors for each plot

#### Divided Canvas
- Check **"Divide Canvas"**
- Set **Rows** and **Cols** (e.g., 2x2)
- Each plot in separate pad
- Maintains individual legends

#### Separate Canvases (Default)
- Leave both options unchecked
- Each plot in its own window
- Full control over each canvas

### 5. Apply Fit Functions (Optional)

1. Select from **Fit Function** dropdown:
   - **No Fit**: Just plot the data
   - **Gaussian**: Fit with Gaussian (uses RooFit for histograms)
   - **Linear**: Linear fit (pol1)
   - **Pol2/Pol3**: Polynomial fits
   - **Exponential**: Exponential decay/growth
   - **Sine + Offset**: Sinusoidal fit
   - **Custom Function**: Define your own

2. For **Custom Functions**:
   - Enter formula in **"Custom Fit"** box
   - Example: `[0]*x^2 + [1]*x + [2]`
   - Use `[0]`, `[1]`, etc. for parameters

### 6. Create Plots

1. Set **Canvas Title** (optional)
2. Click **"Create Plots"**
3. Canvases appear with your configured plots
4. Fit results (if selected) display on plots

### 7. Customize and Export

#### Using ROOT Canvas Features:
- Right-click on canvas elements to edit
- View → Editor: Enable graphical editor
- File → Save As: Export in various formats

#### Supported Export Formats:
- PNG, JPG, GIF
- PDF, EPS, SVG
- ROOT (.root)
- C++ macro (.C)

---

## Script Panel Guide

The Script Panel is one of the most powerful features in version 2.0, providing an integrated scripting environment.

### Script Panel Tabs

#### 1. Script Editor Tab
**Purpose**: Write and execute multi-line scripts

**Usage**:
```cpp
// Example ROOT/C++ script
TH1F *h = new TH1F("h", "Test Histogram", 100, -5, 5);
h->FillRandom("gaus", 10000);
TCanvas *c = new TCanvas();
h->Draw();
c->Update();
```

**Controls**:
- **Run Script**: Execute the entire script
- **Clear Editor**: Erase all text
- **Load Script**: Load script from file (.C, .cpp, .py)
- **Save Script**: Save current script to file

#### 2. Command Line Tab
**Purpose**: Execute single-line commands interactively

**Usage**:
```cpp
// Type command and press Enter
TH1F *h = new TH1F("h2", "Quick Hist", 50, 0, 100);
h->FillRandom("gaus", 5000);
h->Draw();
```

**Features**:
- Press **Enter** to execute
- Click **"Execute Command"** button
- Command history (use Up/Down arrows in future versions)
- Immediate execution and feedback

#### 3. Output Tab
**Purpose**: View all output from scripts and commands

**Features**:
- All `std::cout` output appears here
- All `std::cerr` errors appear here
- ROOT messages and warnings
- Read-only (prevents accidental edits)
- **Clear Output** button to reset

### Language Selection

**ROOT/C++** (Default):
- Execute ROOT macros
- Use all ROOT classes
- C++ syntax
- Compiled on-the-fly via Cling

**Python**:
- Full ROOT bindings via PyROOT
- Python syntax
- Use ROOT objects: `ROOT.TH1F()`, etc.
- Mix Python and ROOT seamlessly

### Example Scripts

#### Example 1: Create and Fit Histogram (C++)
```cpp
// Script Editor
TH1F *h = new TH1F("h1", "Gaussian Distribution;x;Counts", 100, -5, 5);
h->FillRandom("gaus", 10000);

TCanvas *c1 = new TCanvas("c1", "Fit Example", 800, 600);
h->Draw();
h->Fit("gaus");

c1->Update();
std::cout << "Mean: " << h->GetMean() << std::endl;
std::cout << "RMS: " << h->GetRMS() << std::endl;
```

#### Example 2: Create Graph with Errors (C++)
```cpp
// Command Line - execute line by line
double x[] = {1, 2, 3, 4, 5};
double y[] = {2.1, 3.9, 6.2, 7.8, 10.1};
double ex[] = {0.1, 0.1, 0.1, 0.1, 0.1};
double ey[] = {0.3, 0.4, 0.5, 0.4, 0.6};

TGraphErrors *gr = new TGraphErrors(5, x, y, ex, ey);
gr->SetTitle("My Data;X axis;Y axis");
gr->SetMarkerStyle(21);

TCanvas *c2 = new TCanvas();
gr->Draw("APE");
```

#### Example 3: Python Script
```python
# Select "Python" from language dropdown
import ROOT

# Create histogram
h = ROOT.TH1F("h", "Python Histogram", 100, -3, 3)
h.FillRandom("gaus", 5000)

# Create canvas and draw
c = ROOT.TCanvas("c", "Python Canvas", 800, 600)
h.Draw()
h.SetLineColor(ROOT.kBlue)
h.SetFillColor(ROOT.kCyan)

c.Update()
print(f"Entries: {h.GetEntries()}")
print(f"Mean: {h.GetMean():.3f}")
```

#### Example 4: Load and Analyze Data
```cpp
// Open a ROOT file and analyze it
TFile *f = TFile::Open("mydata.root");
TH1F *h = (TH1F*)f->Get("histogram_name");

if (h) {
    h->Draw();
    std::cout << "Histogram loaded successfully!" << std::endl;
    std::cout << "Entries: " << h->GetEntries() << std::endl;
} else {
    std::cerr << "Histogram not found!" << std::endl;
}
```

### Script Tips and Best Practices

1. **Memory Management**:
   - Objects created in scripts persist
   - Clean up if needed: `delete obj;`
   - Or let ROOT manage: `obj->SetBit(kCanDelete);`

2. **Canvas Updates**:
   - Always call `canvas->Update();` to see changes
   - The script panel does this automatically

3. **Error Handling**:
   - Check pointers before use
   - All errors appear in Output tab

4. **Accessing Loaded Data**:
   - Created plots are registered globally
   - Access via ROOT's global list

5. **Script Files**:
   - Save frequently used scripts
   - Share scripts with collaborators
   - Version control your analysis scripts

---

## Extending The Code

### Adding New File Formats

**File**: `include/DataReader.h`

```cpp
// Add new method
static bool ReadMyFormat(const std::string& filename, ColumnData& data) {
    // Your parsing code here
    return true;
}

// Update ReadFile() method
static bool ReadFile(const std::string& filename, ColumnData& data) {
    FileType type = GetFileType(filename);
    switch (type) {
        case kMyFormat:
            return ReadMyFormat(filename, data);
        // ... existing cases
    }
}
```

### Adding New Plot Types

**File**: `include/PlotTypes.h`

```cpp
// Add to PlotConfig enum
enum PlotType {
    kTGraph,
    kTGraphErrors,
    kTH1D,
    kTH2D,
    kTH3D,
    kMyNewPlotType  // Add here
};

// Add creation method in PlotCreator class
class PlotCreator {
public:
    static TMyObject* CreateMyNewPlot(const ColumnData& data, const PlotConfig& config) {
        // Your plot creation code
        return myObject;
    }
};
```

**Update**: `src/AdvancedPlotGUI.cpp` in `DoPlot()` method to handle new type.

### Adding New Fit Functions

**File**: `include/FitUtils.h`

```cpp
// Add to FitType enum
enum FitType {
    kNoFit,
    kGaus,
    kLinear,
    // ... existing
    kMyNewFit  // Add here
};

// Add to GetFitFunction()
static std::string GetFitFunction(FitType type) {
    switch(type) {
        case kMyNewFit: return "myfunction";
        // ... existing cases
    }
}
```

**Update**: `src/AdvancedPlotGUI.cpp` - add entry to fit combo box in constructor.

### Adding GUI Features

**File**: `include/AdvancedPlotGUI.h` and `src/AdvancedPlotGUI.cpp`

1. Add widget member variables in header
2. Create widgets in constructor
3. Add handler methods
4. Update `ProcessMessage()` to route events

### Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes with clear commit messages
4. Test thoroughly
5. Submit a pull request

---

## Troubleshooting

### Common Issues

#### 1. Dictionary Generation Fails
**Error**: `Missing FileEntry for ColumnSelector.h`

**Solution**:
```bash
# Clean and rebuild
make clean
make

# Or generate dictionary manually
cd include
rootcling -f ../src/ColumnSelectorDict.cpp ColumnSelector.h ColumnSelectorLinkDef.h
cd ..
make
```

See `TROUBLESHOOTING_DICTIONARY.md` for detailed solutions.

#### 2. Python Support Not Working
**Error**: `undefined symbol: Py_Initialize`

**Solution**:
```bash
# Install Python development files
sudo apt-get install python3-dev

# Update Makefile with correct Python version
# Edit: PYTHON_INC = -I/usr/include/python3.X
```

#### 3. ROOT Libraries Not Found
**Error**: `cannot find -lRooFit`

**Solution**:
```bash
# Check ROOT installation
root-config --libs

# Ensure RooFit is installed
# Ubuntu/Debian: sudo apt-get install root-system-bin
```

#### 4. Compilation Errors
**Error**: Various compilation errors

**Solution**:
```bash
# Check dependencies
make check-deps

# Update compiler
g++ --version  # Should be 7.0 or higher

# Try debug build for more info
make debug
```

### Getting Help

1. **Check Documentation**: Read this README thoroughly
2. **Troubleshooting Guides**: See `TROUBLESHOOTING_DICTIONARY.md`
3. **Issue Tracker**: [GitHub Issues](https://github.com/siddharthparashri5/Advance_ROOT_Plotting/issues)
4. **ROOT Forum**: [https://root-forum.cern.ch](https://root-forum.cern.ch)

### Reporting Bugs

When reporting bugs, please include:
- ROOT version: `root-config --version`
- OS and version
- Compiler version: `g++ --version`
- Python version: `python3 --version`
- Full error message
- Steps to reproduce

---

## License

**GNU General Public License v3.0**

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

---

## Acknowledgments

- **ROOT Team** at CERN for the excellent framework
- **RooFit** developers for statistical modeling tools
- **Contributors** who helped improve this project

---

## Citation

If you use this software in your research, please cite:

```
Parashri, S. (2026). Advanced ROOT Plotting v2.0: 
A Graphical Interface for Scientific Data Visualization.
GitHub: https://github.com/siddharthparashri5/Advance_ROOT_Plotting
```

---

## Version History

### Version 2.0 (February 5, 2026)
- ✨ Added integrated script panel with ROOT/C++ and Python support
- ✨ Added command-line interface for interactive execution
- ✨ Added output viewer with stream redirection
- ✨ Enhanced ROOT file support with TBrowser integration
- 🔧 Improved directory structure (include/, src/, obj/)
- 🔧 Support for all TH1/TH2/TH3 histogram variants
- 🔧 Enhanced Makefile with multiple build targets
- 📚 Comprehensive documentation and troubleshooting guides

### Version 1.0 (February 4, 2026)
- 🎉 Initial release
- Basic plotting functionality
- Multi-format file support
- Fit functions integration
- Canvas customization

---

**For questions or support, please open an issue on GitHub.**

**Happy Plotting! 📊**
