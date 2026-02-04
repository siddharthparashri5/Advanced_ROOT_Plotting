# Advanced ROOT Plotting GUI

A modular, extensible GUI for creating various types of plots from data files in ROOT.

## Features

### Supported Input Formats
- **Text files** (.txt, .dat) - Space or tab separated
- **CSV files** (.csv) - Comma separated
- **ROOT files** (.root) - Automatically reads TTree branches

### Supported Plot Types
- **TGraph** - Simple x vs y scatter plots
- **TGraphErrors** - Scatter plots with error bars (x and/or y errors)
- **TH1D** - 1D histograms
- **TH2D** - 2D histograms (heatmaps)
- **TH3D** - 3D histograms (heatmaps)

### Features
- **Column Selection**: Choose which columns to use for x, y, and errors
- **Multiple Plots**: Add multiple plots to a single canvas
- **Canvas Division**: Arrange plots in a grid (e.g., 2x2, 3x3)
- **Fitting**: Supports predefined functions (Gaussian, Polynomial, Sine, etc.) and custom functions
- **Flexible Configuration**: Each plot can be configured independently
- **Error Handling**


## Project Structure

```
├── AdvancedPlotGUI.cpp   # Main GUI application
├── ColumnSelector.cpp    # for selecting columns
├── DataReader.h          # File I/O for different formats
├── ColumnSelector.h      # Dialog for selecting columns
├── PlotTypes.h           # Plot creation functions
├── Errorhandling.h           # Error Handling functions
├── FitUtils.h            # Fitting utilities
├── Makefile              # Build configuration
└── README.md             # This file
```

## Compilation

### Generate ROOT dictionary
```
rootcling -f ColumnSelectorDict.cpp -c ColumnSelector.h ColumnSelectorLinkDef.h
```

### Simple compilation:
```bash
make
```

### Manual compilation:
```bash
g++ -o AdvancedPlotGUI AdvancedPlotGUI.cpp `root-config --cflags --libs` -lGui
```

## Usage

### 1. Start the application
```bash
./AdvancedPlotGUI
```

### 2. Load data
- Click **Browse** to select a file (.txt, .dat, .csv, or .root)
- Click **Load** to read the file

### 3. Add plots
- Click **Add Plot** to open the column selector dialog
- Choose plot type (TGraph, TGraphErrors, TH1D, or TH2D)
- Select columns for x, y, and optionally errors
- Click **OK** to add the plot configuration

### 4. Configure display
- Check **Divide Canvas** to arrange multiple plots in a grid
- Set the number of rows and columns
- Select a fit function if desired
- For custom fits, enter a ROOT TF1 formula (e.g., `[0]*sin([1]*x+[2])`)

### 5. Create plots
- Click **Create Plots** to generate all configured plots

## Custom Fit Functions

Use ROOT's TF1 syntax for custom functions:
- Parameters: `[0]`, `[1]`, `[2]`, ...
- Variable: `x`
- Math functions: `sin(x)`, `exp(x)`, `sqrt(x)`, `log(x)`, `pow(x,n)`

### Examples:
```
Quadratic:         [0]*x^2 + [1]*x + [2]
Power law:         [0]*x^[1]
Double exp:        [0]*exp(-[1]*x) + [2]*exp(-[3]*x)
Lorentzian:        [0]/((x-[1])^2 + [2]^2)
Sine + linear:     [0]*sin([1]*x+[2]) + [3]*x + [4]
```

## Data File Format

### Text/DAT files:
```
# Optional comment line
x1  y1  x2  y2  ...
0.0 1.5 2.3 4.1
1.0 2.3 3.4 5.2
...
```

### CSV files:
```
x,y,xerr,yerr
0.0,1.5,0.1,0.2
1.0,2.3,0.1,0.3
...
```

### ROOT files:
- Must contain at least one TTree
- All branches will be available as columns

## Extending the Code

### Adding new file formats:
Edit `DataReader.h` and add a new `Read*File()` method.

### Adding new plot types:
Edit `PlotTypes.h` and add new creation methods to `PlotCreator`.

### Adding new fit functions:
Edit `FitUtils.h` and add to the `FitType` enum and `GetFitFunctions()` map.

## Requirements

- ROOT 6.28 or later
- C++17 or later compiler
- GUI libraries enabled in ROOT build

## License

Free to use and modify.
