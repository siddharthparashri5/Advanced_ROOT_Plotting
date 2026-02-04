# Advanced ROOT Plotting - User Manual

**Version:** 1.0  
**Date:** February 4, 2026  
**Author:** Siddharth Parashri

---

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start Guide](#quick-start-guide)
3. [Installation](#installation)
4. [Feature Descriptions](#feature-descriptions)
5. [Example Workflows](#example-workflows)
6. [Troubleshooting](#troubleshooting)
7. [FAQ](#faq)

---

## Introduction

Advanced ROOT Plotting is a graphical user interface application built on the ROOT framework, designed to provide intuitive and powerful data visualization capabilities for scientific data analysis. Whether you're analyzing particle physics data, statistical distributions, or general scientific measurements, this tool offers comprehensive plotting and fitting features in an easy-to-use interface.

### Key Features

- **Multi-format Support**: Load data from ROOT files, CSV, and text formats
- **Versatile Plotting**: Create histograms, scatter plots, line graphs, and more
- **Advanced Fitting**: Fit data with built-in or custom functions
- **Interactive Canvas**: Zoom, pan, and interact with your plots
- **Export Capabilities**: Save plots in multiple formats (PNG, PDF, EPS, ROOT)
- **Customization**: Full control over colors, markers, styles, and layouts

---

## Quick Start Guide

### Getting Started in 5 Minutes

1. **Launch the Application**
   ```bash
   root -l AdvancedPlotGUI.C
   ```

2. **Load Your Data**
   - Click the **"Load"** button or use `File → Open`
   - Select your data file (supported: .root, .csv, .txt, .dat)
   - The file path will appear in the text entry box

3. **Create a Plot**
   - Select your desired plot type from the dropdown menu
   - Click the **"Plot"** button
   - Your data will appear on the canvas

4. **Fit Your Data** (Optional)
   - Select a fit function from the fit dropdown menu
   - Click the **"Fit"** button
   - Fit results will be displayed on the plot

5. **Export Your Plot**
   - Click the **"Export"** button
   - Choose your desired format and location
   - Click "Save"

---

## Installation

### Prerequisites

- ROOT 6.x or later ([https://root.cern.ch](https://root.cern.ch))
- C++ compiler (GCC 7+ or Clang 5+)
- CMake 3.10 or later (for building)

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

### Using ROOT while running the App

You can run directly with ROOT:

```bash
For example ...

root [0] gPad->ls();
root [1] auto g = (TGraph*) gPad->GetPrimitive("Graph");
root [2] g->Draw("LP");
```

---

## Feature Descriptions

### 1. Data Loading

#### Supported File Formats

**ROOT Files (.root)**
- Automatically detects and loads TH1, TH2, TGraph objects
- Supports TTrees with branch selection
- Preserves histogram binning and statistics

**CSV Files (.csv)**
- Comma-separated or semicolon-separated values
- First row can be headers (automatically detected)
- Supports 1D and 2D data
- Example format:
  ```
  x,y,error
  1.0,2.3,0.1
  2.0,4.1,0.2
  3.0,5.9,0.15
  ```

**Text Files (.txt, .dat)**
- Space or tab-separated columns
- Comment lines starting with '#' are ignored
- Flexible column assignment
- Example format:
  ```
  # Energy  Count  Error
  10.5      145    12
  20.3      267    16
  30.1      321    18
  ```

#### Loading Process

1. Click **"Load"** button 
2. Navigate to your data file
3. Select the file and click "Open"
4. Data is automatically parsed and loaded
5. Status message confirms successful loading

### 2. Plot Types

#### Histogram (1D)
- Displays frequency distribution of a single variable
- Customizable bin width and range
- Supports various drawing options (bars, lines, points)
- Best for: Statistical distributions, measurement counts

#### Scatter Plot
- Shows correlation between two variables
- Customizable marker styles and sizes
- Optional error bars in X and Y
- Best for: Correlation analysis, data point visualization

#### Line Plot
- Connects data points with lines
- Smooth or straight line options
- Customizable line width and style
- Best for: Time series, continuous data trends

#### 2D Histogram
- Displays density of (x, y) pairs
- Color-coded intensity
- Multiple color palettes available
- Best for: Heat maps, 2D distributions

#### 3D Histogram
- Displays density of (x, y) pairs
- Color-coded intensity
- Multiple color palettes available
- Best for: Heat maps, 3D distributions, Profile 2D, 3D

### 3. Fitting Capabilities

#### Built-in Fit Functions

**Gaussian (gaus)**
- Formula: `A * exp(-(x-μ)²/(2σ²))`
- Parameters: Amplitude, Mean, Sigma
- Use for: Peak fitting, normal distributions

**Polynomial (pol0-pol9)**
- Formula: `p₀ + p₁x + p₂x² + ... + pₙxⁿ`
- Degrees: 0 (constant) to 9
- Use for: Trend lines, baseline fitting

**Exponential (expo)**
- Formula: `exp(p₀ + p₁x)`
- Use for: Decay curves, growth models

**Landau**
- Formula: Landau distribution function
- Use for: Energy loss distributions

**Custom Functions**
- Define your own functions using ROOT syntax
- Access via `Fit → Custom Function`
- Example: `[0]*sin([1]*x+[2])+[3]`

#### Fit Options
- Can be accessed via ROOT FitPanel options

### 4. Plot Customization
- Full control using ROOT FitPanel or via terminal window


## Example Workflows

### Workflow 1: Analyzing Histogram Data

**Scenario**: You have energy measurement data in a CSV file and want to create a histogram, fit it with a Gaussian, and export the result.

**Steps**:

1. **Prepare Your Data**
   - Ensure your CSV file has a column with energy values
   - Example: `energies.csv`

2. **Load and Plot**
   ```
   - Click "Load" → Select "energies.csv"
   - Select "Histogram" from plot type dropdown
   - Click "Plot"
   ```

3. **Customize Appearance**
   ```
   - Right-click on histogram
   - Select "SetLineColor" → Choose color (e.g., blue = 4)
   - Select "SetFillColor" → Choose color (e.g., light blue = 38)
   ```

4. **Fit with Gaussian**
   ```
   - Select "gaus" from fit function dropdown
   - Click "Fit"
   - Fit parameters will be displayed in a stats box
   ```

5. **Fine-tune the Fit**
   ```
   - If needed, adjust fit range via "Fit → Set Range"
   - Refit using "Fit → Refit"
   ```

6. **Export**
   ```
   - Click "Export"
   - Choose "PDF" format
   - Save as "energy_distribution.pdf"
   ```

### Workflow 2: Correlation Analysis

**Scenario**: You want to examine the correlation between two variables from a text file.

**Steps**:

1. **Load Data**
   ```
   - Click "Load" → Select your .txt file with two columns
   - Data format: "x_value y_value" per line
   ```

2. **Create Scatter Plot**
   ```
   - Select "Scatter Plot" from dropdown
   - Click "Plot"
   ```

3. **Add Error Bars** (if available)
   ```
   - If your file has error columns, enable via:
   - Plot → Options → Show Error Bars
   ```

4. **Fit with Polynomial**
   ```
   - Select "pol1" (linear) from fit dropdown
   - Click "Fit"
   - Check fit quality (χ²/ndf value)
   ```

5. **Analyze Results**
   ```
   - Fit parameters show: y = p0 + p1*x
   - p1 is the slope (correlation strength)
   - Examine residuals plot if needed
   ```

### Workflow 3: Multiple Dataset Comparison

**Scenario**: Comparing experimental data with simulation.

**Steps**:

1. **Load First Dataset**
   ```
   - Load experimental data
   - Plot as histogram with style 1
   ```

2. **Overlay Second Dataset**
   ```
   - File → Add Dataset
   - Load simulation data
   - Plot with "SAME" option
   - Use different color/style
   ```

3. **Create Legend**
   ```
   - Plot → Insert Legend
   - Add entries: "Experimental", "Simulation"
   - Position in empty plot area
   ```

4. **Statistical Comparison**
   ```
   - Tools → Chi-Square Test
   - Compares distributions
   - Shows agreement level
   ```
---

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Cannot open file" error

**Symptoms**: Error message when trying to load a file

**Solutions**:
- Check that the file path is correct and contains no special characters
- Ensure you have read permissions for the file
- Verify the file format is supported (.root, .csv, .txt, .dat)
- For CSV files, check that the delimiter is comma or semicolon
- Check that the file is not corrupted (try opening in a text editor)

#### Issue 2: Plot appears empty or blank

**Symptoms**: Canvas shows but no data is visible

**Solutions**:
- Verify data was loaded successfully (check status bar message)
- Check if data values are within a reasonable range
- Try using "Reset View" or "Auto Scale" options
- Ensure plot type matches your data (e.g., don't use 2D plot for 1D data)
- Check for infinite or NaN values in your data

#### Issue 3: Fit does not converge

**Symptoms**: "Fit failed" or "Minimization failed" error

**Solutions**:
- Provide better initial parameter estimates
- Reduce fit range to region of interest
- Use a simpler fit function first
- Check data quality (remove outliers if needed)
- Try different fit options (add "M" for MIGRAD, "E" for better errors)
- Increase maximum number of iterations

#### Issue 4: Exported plot looks different from screen

**Symptoms**: Exported PDF/PNG has different fonts, sizes, or layout

**Solutions**:
- Set canvas size explicitly before exporting
- Use vector formats (PDF, EPS) for best quality
- Check ROOT's batch mode settings
- Adjust font sizes in style settings
- Use higher DPI for raster exports (PNG, JPG)

#### Issue 5: Application crashes on startup

**Symptoms**: Program exits immediately or shows segmentation fault

**Solutions**:
- Check ROOT installation: `root-config --version`
- Verify all required libraries are installed
- Check for conflicting ROOT versions
- Run with debugging: `root -l -b AdvancedPlotGUI.C`
- Review error logs for specific library issues

#### Issue 6: Slow performance with large datasets

**Symptoms**: Application becomes unresponsive or very slow

**Solutions**:
- Reduce number of data points (bin data or use sampling)
- Use compiled version instead of interpreted macros
- Disable auto-refresh while adjusting settings
- Close unused canvases and plots
- Increase system memory allocation for ROOT

#### Issue 7: Cannot see fit results

**Symptoms**: Fit completes but parameters not displayed

**Solutions**:
- Enable statistics box: `Plot → Show Stats Box`
- Right-click on plot → `SetOptFit(1111)`
- Ensure stats box is not hidden behind plot elements
- Resize stats box if it's too small
- Check that fit was actually successful (look for error messages)

### Getting More Help

If you encounter issues not listed here:

1. **Check the Log**: Look at terminal output for error messages
2. **ROOT Documentation**: [https://root.cern.ch/doc/master/](https://root.cern.ch/doc/master/)
3. **GitHub Issues**: Report bugs at the repository issue tracker
4. **ROOT Forum**: Ask questions at [https://root-forum.cern.ch/](https://root-forum.cern.ch/)
5. **Contact**: Email the developer with detailed error description

---

**Document Version**: 1.0  
**Last Updated**: February 4, 2026  
**Feedback**: Please submit suggestions and corrections via GitHub issues
