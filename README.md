# Advanced ROOT Plotting - User Manual

**Version:** 1.0  
**Date:** February 4, 2026  
**Author:** Siddharth Parashri

---

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start Guide](#quick-start-guide)
3. [Installation](#installation)
4. [Data Preparation](#data-preparation)
5. [Workflow](#workflow)
6. [Extending The Code](#extending-the-code)
7. [License](#license)

---

## Introduction

Advanced ROOT Plotting is a graphical user interface application built on the ROOT framework, designed to provide intuitive and powerful data visualization capabilities for scientific data analysis. Whether you're analyzing particle physics data, statistical distributions, or general scientific measurements, this tool offers comprehensive plotting and fitting features in an easy-to-use interface.

### Key Features

- **FULL ROOT Support**: Built-in ROOT support to edit and plot data.
- **Multi-format Support**: Load data from ROOT files, CSV, and text formats
- **Versatile Plotting**: Create histograms, scatter plots, line graphs, and more
- **Advanced Fitting**: Fit data with built-in or custom functions
- **Interactive Canvas**: Zoom, pan, and interact with your plots
- **Export Capabilities**: Save plots in multiple formats (PNG, PDF, EPS, ROOT)
- **Customization**: Full control over colors, markers, styles, and layouts

---

## Quick Start Guide

### Getting Started in 5 Minutes

### Prerequisites

- ROOT 6.x or later ([https://root.cern.ch](https://root.cern.ch))
- C++ compiler (GCC 7+ or Clang 5+)
- CMake 3.10 or later (for building)

### Project Structure 
```
Advance_ROOT_Plotting/
   ├── AdvancedPlotGUI.cpp
   ├── ColumnSelector.cpp
   ├── AdvancedPlotGUI.h
   ├── DataReader.h
   ├── ErrorHandling.h
   ├── ColumnSelector.h
   ├── PlotTypes.h
   ├── FitUtils.h
   ├── sample_data.csv
   ├── sample_data.txt
   ├── Makefile
   ├── README.md
   ├── LICENSE
```

## Installation

### Building from Source

```bash
# Clone the repository
git clone https://github.com/siddharthparashri5/Advance_ROOT_Plotting.git
cd Advance_ROOT_Plotting

# Generate ROOT dictionary
rootcling -f ColumnSelectorDict.cpp -c ColumnSelector.h ColumnSelectorLinkDef.h

# Configure and make
make

# Run the application
./AdvancedPlotGUI
```

## Data Preparation

- When preparing data files (.txt, .dat, .csv) for plotting, put data in columns
- Each column header is treated as an axis title
- Legends are made from the chosen Y-axis header name
-  Text/DAT files: 
     ```
     # Optional comment line
     # Optional comment line
     x1 y1 x2 y2 x1_err y1_err...  //Header line
     0.0 1.5 2.3 4.1 0.01 0.2
     1.0 2.3 3.4 5.2 0.003 0.1
     ...
     ```
-  CSV files: 
     ```
     # Optional comment line
     # Optional comment line
     x,y,xerr,yerr...  //Header line
     0.0,1.5,0.1,0.2
     1.0,2.3,0.1,0.3
     ...
     ```
 - Root files:
       - Opens in a TBrowser for easier functionality
       - The terminal can be used as usual

## Workflow

1. **Launch the Application (after #installation)**
   ```bash
   ./AdvancedPlotGUI.C
   ```

2. **Load Your Data**
   - Click the **"Browse"** button
   - Select your data file (supported: .root, .csv, .txt, .dat)
   - The file path will appear in the text entry box
   - Click the **"Load"** button, and a dialog box will appear to show the loaded data
   - The ROOT files will be opened in a TBrowser 

3. **Create a Plot**
   - Select your desired plot type from the dropdown menu
   - Click the **"Add Plot"** button
   - Select the **"Plot Type"** followed by **"Column Selection"**, Press **"OK"**
   - You can add multiple plots at a time
   - Select the title for the TCanvas 
   - Plot multiple plots in the same canvas using the built-in Draw("SAME") function
   - Or Divide TCanvas using Divide(mxn) and plot multiple plots in a grid format.
   - Your data will appear on the canvas

4. **Fit Your Data** (Optional)
   - Select a fit function from the fit dropdown menu
   - The user can also provide a custom fit function
   - Click the **"Create Plots"** button
   - TCanvas will appear with the applied Fit function
   - Fit results will be displayed on the plot

5. **Make Plot Look Pretty**
   - Use the built-in ROOT functionality to Edit Canvases.

6. **Export Your Plot**
   - Click the **"Save as"** button from the TCanvas 
   - Choose your desired format and location
   - Click "Save"

## Extending The Code
   - Adding new file formats:
     - Edit DataReader.h and add a new Read*File() method.
     - Edit DataReader.h and add a new Read*File() method.
   - Adding new plot types:
     - Edit PlotTypes.h and add new creation methods to PlotCreator.
     - Edit PlotTypes.h and add new creation methods to PlotCreator.
   - Adding new fit functions:
     - Edit FitUtils.h, add to the FitType enum and GetFitFunctions() map.
    
      
## License 
Free to use and modify. GNU General Public License v3.
