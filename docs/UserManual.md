# Advanced ROOT Plotting - User Manual

**Version:** 1.0  
**Date:** February 4, 2026  
**Author:** Siddharth Parashari

---

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start Guide](#quick-start-guide)
3. [Installation](#installation)
4. [Feature Descriptions](#feature-descriptions)
5. [Example Workflows](#example-workflows)
6. [Keyboard Shortcuts](#keyboard-shortcuts)
7. [Troubleshooting](#troubleshooting)
8. [FAQ](#faq)

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

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the application
./AdvancedPlotGUI
```

### Using ROOT Macros

Alternatively, you can run directly with ROOT:

```bash
root -l AdvancedPlotGUI.C
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

1. Click **"Load"** button or `File → Open` (Ctrl+O)
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

#### Box Plot
- Statistical summary of data distribution
- Shows median, quartiles, and outliers
- Best for: Comparing multiple datasets

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

- **Fit Range**: Specify X-axis range for fitting
- **Fit Options**: 
  - `Q`: Quiet mode (minimal output)
  - `R`: Use the range specified in the function
  - `N`: No plotting
  - `E`: Better error estimation
  - `M`: Improve fit results

### 4. Plot Customization

#### Color Settings

Access via `Plot → Customize → Colors`

- **Line Color**: 1-50 (ROOT color palette)
- **Fill Color**: For histograms and areas
- **Marker Color**: For scatter plots
- **Popular colors**: 
  - Black (1), Red (2), Green (3), Blue (4)
  - Yellow (5), Magenta (6), Cyan (7)

#### Marker Styles

Access via `Plot → Customize → Markers`

- Styles 1-34 available
- Common styles:
  - 20: Filled circle (default)
  - 21: Filled square
  - 22: Filled triangle up
  - 23: Filled triangle down
  - 24: Open circle
  - 25: Open square

#### Line Styles

- Solid (1), Dashed (2), Dotted (3), Dash-dot (4)
- Line width: 1-10 pixels

#### Axis Configuration

- **Title**: Set X and Y axis labels
- **Range**: Manual or automatic scaling
- **Log Scale**: Toggle logarithmic axes
- **Grid**: Show/hide grid lines

### 5. Canvas Interaction

#### Mouse Controls

- **Left Click + Drag**: Pan the plot
- **Right Click**: Context menu
- **Mouse Wheel**: Zoom in/out
- **Double Click**: Zoom to default view

#### Zoom and Pan

- Click and drag to create a zoom box
- Right-click → `UnZoom` to reset
- Use toolbar zoom tools for precise control

### 6. Export Options

#### Supported Formats

**Raster Formats**
- PNG (Portable Network Graphics) - Best for web
- JPG (JPEG) - Smaller file size, some quality loss
- GIF (Graphics Interchange Format)

**Vector Formats**
- PDF (Portable Document Format) - Best for papers
- EPS (Encapsulated PostScript) - For LaTeX
- SVG (Scalable Vector Graphics) - For presentations

**ROOT Format**
- .root file with canvas object
- Preserves all interactive features
- Can be reopened and edited later

**Source Code**
- .C (ROOT macro) - Executable C++ code
- Recreates the plot when run

#### Export Process

1. Click **"Export"** or `File → Export`
2. Choose format from dropdown
3. Enter filename
4. Click "Save"
5. Confirmation message appears

---

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

### Workflow 3: Time Series Visualization

**Scenario**: Plotting time-dependent measurements.

**Steps**:

1. **Load Time Series Data**
   ```
   - Prepare CSV with timestamp, value columns
   - Load via "Load" button
   ```

2. **Create Line Plot**
   ```
   - Select "Line Plot" from dropdown
   - Click "Plot"
   ```

3. **Customize Time Axis**
   ```
   - Right-click X-axis → "SetTimeDisplay"
   - Choose time format (e.g., "%H:%M:%S")
   ```

4. **Add Trend Line**
   ```
   - Fit with "pol1" for linear trend
   - Or "pol2" for curved trend
   ```

5. **Annotate**
   ```
   - Right-click on canvas
   - Insert → Text to add labels
   - Mark important events or thresholds
   ```

### Workflow 4: Multiple Dataset Comparison

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

## Keyboard Shortcuts

### File Operations
| Shortcut | Action |
|----------|--------|
| `Ctrl+O` | Open file |
| `Ctrl+S` | Save canvas to ROOT file |
| `Ctrl+E` | Export plot |
| `Ctrl+Q` | Quit application |

### Plot Operations
| Shortcut | Action |
|----------|--------|
| `Ctrl+P` | Create/refresh plot |
| `Ctrl+F` | Perform fit |
| `Ctrl+Z` | Undo last action |
| `Ctrl+R` | Reset view |

### Canvas Navigation
| Shortcut | Action |
|----------|--------|
| `Arrow Keys` | Pan canvas |
| `+` / `-` | Zoom in/out |
| `Home` | Reset to default view |
| `Space` | Toggle full screen |

### Customization
| Shortcut | Action |
|----------|--------|
| `Ctrl+L` | Toggle log scale Y-axis |
| `Ctrl+Shift+L` | Toggle log scale X-axis |
| `Ctrl+G` | Toggle grid |
| `Ctrl+T` | Toggle statistics box |

### Help
| Shortcut | Action |
|----------|--------|
| `F1` | Open help documentation |
| `Ctrl+A` | About dialog |

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

## FAQ

### General Questions

**Q: What is ROOT?**  
A: ROOT is a modular scientific software framework developed at CERN. It provides tools for data analysis, visualization, and storage, widely used in particle physics and other scientific fields.

**Q: Do I need programming knowledge to use this tool?**  
A: No, the GUI is designed for users without programming experience. However, knowledge of ROOT and C++ can help with advanced customization.

**Q: Can I use this for non-scientific data?**  
A: Absolutely! While designed for scientific analysis, the tool works well for any numerical data visualization needs.

### Data-Related Questions

**Q: What's the maximum file size I can load?**  
A: This depends on your system's available memory. Files up to several GB can typically be handled, but performance may degrade with very large datasets.

**Q: Can I load multiple files at once?**  
A: Currently, files are loaded one at a time. However, you can overlay plots from different files using the "Add Dataset" feature.

**Q: Does it support 3D plotting?**  
A: The current version focuses on 1D and 2D plots. 3D plotting support is planned for future releases.

### Technical Questions

**Q: Which ROOT version is required?**  
A: ROOT 6.0 or later. Recommended: ROOT 6.24 or newer for best compatibility and features.

**Q: Can I run this on Windows?**  
A: Yes, if you have ROOT installed on Windows. Alternatively, use WSL (Windows Subsystem for Linux) with ROOT.

**Q: How do I report bugs?**  
A: Please report bugs on the GitHub repository issues page with:
- Operating system and ROOT version
- Steps to reproduce the issue
- Error messages or screenshots
- Sample data file (if applicable)

**Q: Is the source code open?**  
A: Yes! The project is open-source and available on GitHub. Contributions are welcome.

---

## Additional Resources

### Links
- **Project Repository**: [https://github.com/siddharthparashri5/Advance_ROOT_Plotting](https://github.com/siddharthparashri5/Advance_ROOT_Plotting)
- **ROOT Official Site**: [https://root.cern.ch](https://root.cern.ch)
- **ROOT Tutorials**: [https://root.cern.ch/doc/master/group__Tutorials.html](https://root.cern.ch/doc/master/group__Tutorials.html)

### Related Documentation
- Developer Guide (for extending functionality)
- API Reference (Doxygen-generated)
- Example datasets and scripts

---

**Document Version**: 1.0  
**Last Updated**: February 4, 2026  
**Feedback**: Please submit suggestions and corrections via GitHub issues
