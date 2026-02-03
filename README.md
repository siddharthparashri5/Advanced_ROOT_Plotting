*****************************************************************
# Advanced ROOT Plotting GUI

A modular and extensible GUI for creating various types of plots from data files using ROOT.

Initially developed and maintained by Dr. Siddharth Parashari.
*****************************************************************

## Features 

### Supported Input Formats 

- Text files (.txt, .dat) — space or tab separated
- CSV files (.csv) — comma separated
- ROOT files (.root) — automatically reads TTree branches

### Supported Plot Types 

- TGraph — simple x vs y scatter plots
- TGraphErrors — scatter plots with error bars (x and/or y errors)
- TH1D — 1D histograms
- TH2D — 2D histograms (heatmaps)

### Plot Features 
- Column Selection: Choose which columns to use for x, y, and optional errors
- Multiple Plots: Overlay multiple plots on the same canvas
- Canvas Division: Arrange plots in a grid (e.g., 2x2, 3x3)
- Fitting: Supports predefined functions (Gaussian, Polynomial, Sine, etc.) and custom TF1 functions
- Flexible Configuration: Each plot can be configured independently

### Project Structure

├── AdvancedPlotGUI.cpp     # Main GUI application
├── ColumnSelector.cpp      # GUI for selecting columns
├── ColumnSelector.h        # Header file for column selector
├── ColumnSelectorLinkDef.h # ROOT dictionary link definitions
├── DataReader.h            # File I/O for different formats
├── PlotTypes.h             # Plot creation functions
├── FitUtils.h              # Fitting utilities
├── Makefile                # Build configuration
└── README.md               # This file

## Requirements
- ROOT 6.x or later (with GUI libraries enabled)
- C++11 or later compiler

## Compilation
- Using Makefile
  make

## Manual Compilation 
 g++ -o AdvancedPlotGUI AdvancedPlotGUI.cpp `root-config --cflags --libs` -lGui

## ROOT Dictionary Generation (if needed)
rootcling -f ColumnSelectorDict.cpp -c ColumnSelector.h ColumnSelectorLinkDef.h

## Usage

### 1. Start the application
./AdvancedPlotGUI

### 2. Load data
- Click Browse to select a file (.txt, .dat, .csv, or .root)
- Click Load to read the file
- Add plots
- Click Add Plot to open the column selector dialog
- Choose the plot type (TGraph, TGraphErrors, TH1D, or TH2D)
- Select columns for x, y, and optionally errors
- Click OK to add the plot configuration

### 3. Configure display
- Check Divide Canvas to arrange multiple plots in a grid
- Set the number of rows and columns
- Select a fit function if desired
- For custom fits, enter a ROOT TF1 formula (e.g., [0]*sin([1]*x+[2]))

### 4. Create plots
-Click Create Plots to generate all configured plots


## Custom Fit Functions

- Use ROOT's TF1 syntax for custom functions:
- Parameters: [0], [1], [2], ...
- Variable: x
- Math functions: sin(x), exp(x), sqrt(x), log(x), pow(x,n)

### Examples:

Quadratic:         [0]*x^2 + [1]*x + [2]
Power law:         [0]*x^[1]
Double exponential:[0]*exp(-[1]*x) + [2]*exp(-[3]*x)
Lorentzian:        [0]/((x-[1])^2 + [2]^2)
Sine + linear:     [0]*sin([1]*x+[2]) + [3]*x + [4]

## Data File Format 

### 1. Text/DAT files:
- # Optional comment line 
x1  y1  x2  y2 ...
0.0 1.5 2.3 4.1
1.0 2.3 3.4 5.2
...

### 2. CSV files:
x,y,xerr,yerr
0.0,1.5,0.1,0.2
1.0,2.3,0.1,0.3
...

### 3. ROOT files:
- Must contain at least one TTree
- All branches will be available as columns

## Extending the Code 
- Adding new file formats:
- Edit DataReader.h and add a new Read*File() method.
- Adding new plot types:
- Edit PlotTypes.h and add new creation methods to PlotCreator.
- Adding new fit functions:
- Edit FitUtils.h, add to the FitType enum and GetFitFunctions() map.

# License
Free to use and modify.
