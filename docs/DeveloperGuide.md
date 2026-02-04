# Advanced ROOT Plotting - Developer Guide

**Version:** 1.0  
**Date:** February 4, 2026  
**Audience:** Developers extending the codebase

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Code Structure](#code-structure)
3. [Building and Development Setup](#building-and-development-setup)
4. [How to Add New Plot Types](#how-to-add-new-plot-types)
5. [How to Add New File Formats](#how-to-add-new-file-formats)
6. [How to Add New Fit Functions](#how-to-add-new-fit-functions)
7. [Style Guide and Best Practices](#style-guide-and-best-practices)
8. [Testing](#testing)
9. [Contributing](#contributing)

---

## Architecture Overview

### Design Philosophy

The Advanced ROOT Plotting application follows these core principles:

1. **Separation of Concerns**: GUI logic, data handling, and visualization are separated
2. **Extensibility**: Easy to add new plot types, file formats, and fit functions
3. **ROOT Integration**: Leverages ROOT's powerful capabilities while providing a simpler interface
4. **User-Centric**: Focus on ease of use without sacrificing advanced features

### System Architecture

```
┌─────────────────────────────────────────────────────┐
│                  AdvancedPlotGUI                    │
│              (Main GUI Controller)                  │
└───────────┬─────────────────────────┬───────────────┘
            │                         │
            ▼                         ▼
┌───────────────────────┐   ┌────────────────────────┐
│   Data Manager        │   │   Plot Manager         │
│  - File Parsers       │   │  - Plot Factory        │
│  - Data Structures    │   │  - Style Manager       │
│  - Validation         │   │  - Canvas Manager      │
└───────────┬───────────┘   └───────────┬────────────┘
            │                           │
            ▼                           ▼
┌───────────────────────┐   ┌────────────────────────┐
│   File Format         │   │   ROOT Objects         │
│   Handlers            │   │  - TH1, TH2            │
│  - ROOTParser         │   │  - TGraph, TGraph2D    │
│  - CSVParser          │   │  - TF1, TCanvas        │
│  - TextParser         │   └────────────────────────┘
└───────────────────────┘
            │
            ▼
┌─────────────────────────────────────────────────────┐
│              Fit Engine                             │
│  - Function Library                                 │
│  - Minimization Algorithms                          │
│  - Parameter Estimation                             │
└─────────────────────────────────────────────────────┘
```

### Key Components

#### 1. AdvancedPlotGUI (Main Controller)
- Manages the GUI lifecycle
- Coordinates between data, plotting, and user interactions
- Handles menu and button events
- Located in: `AdvancedPlotGUI.h` and `AdvancedPlotGUI.cxx`

#### 2. Data Manager
- Parses different file formats
- Validates data integrity
- Converts data to ROOT structures
- Located in: `DataManager.h` and `DataManager.cxx`

#### 3. Plot Manager
- Creates and manages plot objects
- Applies styling and customization
- Handles canvas operations
- Located in: `PlotManager.h` and `PlotManager.cxx`

#### 4. Fit Engine
- Manages fit function library
- Executes fitting algorithms
- Displays fit results
- Located in: `FitEngine.h` and `FitEngine.cxx`

---

## Code Structure

### Directory Layout

```
Advance_ROOT_Plotting/
├── include/                    # Header files
│   ├── AdvancedPlotGUI.h      # Main GUI class
│   ├── DataManager.h          # Data handling
│   ├── PlotManager.h          # Plot creation
│   ├── FitEngine.h            # Fitting capabilities
│   └── Utilities.h            # Helper functions
│
├── src/                       # Implementation files
│   ├── AdvancedPlotGUI.cxx
│   ├── DataManager.cxx
│   ├── PlotManager.cxx
│   ├── FitEngine.cxx
│   └── Utilities.cxx
│
├── parsers/                   # File format parsers
│   ├── ROOTParser.h
│   ├── ROOTParser.cxx
│   ├── CSVParser.h
│   ├── CSVParser.cxx
│   ├── TextParser.h
│   └── TextParser.cxx
│
├── plots/                     # Plot type implementations
│   ├── HistogramPlot.h
│   ├── HistogramPlot.cxx
│   ├── ScatterPlot.h
│   ├── ScatterPlot.cxx
│   └── ...
│
├── fits/                      # Fit function definitions
│   ├── GaussianFit.h
│   ├── GaussianFit.cxx
│   ├── PolynomialFit.h
│   └── ...
│
├── tests/                     # Unit tests
│   ├── test_data_loading.cxx
│   ├── test_plotting.cxx
│   └── test_fitting.cxx
│
├── examples/                  # Example data and scripts
│   ├── sample_data.root
│   ├── sample_data.csv
│   └── example_usage.C
│
├── docs/                      # Documentation
│   ├── UserManual.md
│   ├── DeveloperGuide.md
│   └── Doxygen/
│
├── CMakeLists.txt             # Build configuration
├── Doxyfile                   # Doxygen configuration
├── README.md                  # Project overview
└── LICENSE                    # License file
```

### Class Hierarchy

```
TGMainFrame (ROOT)
    │
    └── AdvancedPlotGUI
            │
            ├── uses: DataManager
            │           │
            │           ├── uses: ROOTParser
            │           ├── uses: CSVParser
            │           └── uses: TextParser
            │
            ├── uses: PlotManager
            │           │
            │           ├── creates: HistogramPlot
            │           ├── creates: ScatterPlot
            │           └── creates: LinePlot
            │
            └── uses: FitEngine
                        │
                        ├── uses: GaussianFit
                        ├── uses: PolynomialFit
                        └── uses: CustomFit
```

---

## Building and Development Setup

### Prerequisites

```bash
# Install ROOT
# On Ubuntu/Debian:
sudo apt-get install root-system

# On macOS (using Homebrew):
brew install root

# Or build from source:
# See https://root.cern.ch/building-root
```

### Development Build

```bash
# Clone the repository
git clone https://github.com/siddharthparashri5/Advance_ROOT_Plotting.git
cd Advance_ROOT_Plotting

# Create build directory
mkdir build && cd build

# Configure with CMake (Debug mode)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make -j$(nproc)

# Run tests
make test

# Generate documentation
make docs
```

### IDE Setup

#### Visual Studio Code

Recommended extensions:
- C/C++ (Microsoft)
- CMake Tools
- Doxygen Documentation Generator

`.vscode/settings.json`:
```json
{
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "files.associations": {
        "*.cxx": "cpp",
        "*.h": "cpp"
    }
}
```

#### CLion

1. Open project directory
2. CLion will automatically detect CMake configuration
3. Set Run/Debug configurations to point to built executable

### Code Formatting

Use `clang-format` with the provided configuration:

```bash
# Format all source files
find src include -name "*.cxx" -o -name "*.h" | xargs clang-format -i
```

`.clang-format`:
```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: None
```

---

## How to Add New Plot Types

### Step-by-Step Guide

#### Step 1: Create the Plot Class

Create new files `plots/MyNewPlot.h` and `plots/MyNewPlot.cxx`:

**MyNewPlot.h:**
```cpp
/**
 * @file MyNewPlot.h
 * @brief Implementation of custom plot type
 */

#ifndef MYNEWPLOT_H
#define MYNEWPLOT_H

#include "BasePlot.h"
#include <TCanvas.h>
#include <TH1.h>

/**
 * @class MyNewPlot
 * @brief Creates a custom visualization type
 * 
 * @details
 * Detailed description of what this plot type does,
 * when to use it, and what kind of data it requires.
 */
class MyNewPlot : public BasePlot {

private:
    // Add private members specific to this plot type
    TH1D* fHistogram;
    Int_t fCustomParameter;

public:
    /**
     * @brief Constructor
     * @param data Pointer to data container
     * @param canvas Pointer to canvas for drawing
     */
    MyNewPlot(DataContainer* data, TCanvas* canvas);
    
    /**
     * @brief Destructor
     */
    virtual ~MyNewPlot();
    
    /**
     * @brief Create and draw the plot
     * @return true if successful
     */
    virtual bool Draw() override;
    
    /**
     * @brief Update plot with new data
     */
    virtual void Update() override;
    
    /**
     * @brief Apply custom styling
     */
    virtual void ApplyStyle() override;
    
    /**
     * @brief Set custom parameter
     * @param param Parameter value
     */
    void SetCustomParameter(Int_t param) { fCustomParameter = param; }
    
    ClassDef(MyNewPlot, 1) // Custom plot type
};

#endif // MYNEWPLOT_H
```

**MyNewPlot.cxx:**
```cpp
#include "MyNewPlot.h"
#include <iostream>

ClassImp(MyNewPlot)

//____________________________________________________________________________
MyNewPlot::MyNewPlot(DataContainer* data, TCanvas* canvas)
    : BasePlot(data, canvas),
      fHistogram(nullptr),
      fCustomParameter(0)
{
    // Constructor implementation
}

//____________________________________________________________________________
MyNewPlot::~MyNewPlot()
{
    // Cleanup
    if (fHistogram) {
        delete fHistogram;
        fHistogram = nullptr;
    }
}

//____________________________________________________________________________
bool MyNewPlot::Draw()
{
    if (!fData || !fCanvas) {
        std::cerr << "Error: Invalid data or canvas" << std::endl;
        return false;
    }
    
    // Create the plot object
    Int_t nBins = fData->GetNPoints();
    Double_t xMin = fData->GetXMin();
    Double_t xMax = fData->GetXMax();
    
    fHistogram = new TH1D("myplot", "My Custom Plot", nBins, xMin, xMax);
    
    // Fill with data
    for (Int_t i = 0; i < nBins; i++) {
        fHistogram->Fill(fData->GetX(i), fData->GetY(i));
    }
    
    // Apply styling
    ApplyStyle();
    
    // Draw on canvas
    fCanvas->cd();
    fHistogram->Draw("YOUR_DRAW_OPTIONS");
    fCanvas->Update();
    
    return true;
}

//____________________________________________________________________________
void MyNewPlot::Update()
{
    if (fHistogram) {
        fCanvas->cd();
        fHistogram->Draw("YOUR_DRAW_OPTIONS");
        fCanvas->Update();
    }
}

//____________________________________________________________________________
void MyNewPlot::ApplyStyle()
{
    if (!fHistogram) return;
    
    fHistogram->SetLineColor(fLineColor);
    fHistogram->SetLineWidth(fLineWidth);
    fHistogram->SetFillColor(fFillColor);
    // Add more styling as needed
}
```

#### Step 2: Register in PlotManager

Edit `PlotManager.h` to add enum value:

```cpp
enum PlotType {
    kHistogram = 0,
    kScatter = 1,
    kLine = 2,
    kMyNewPlot = 3,  // Add your new type
    kNPlotTypes
};
```

Edit `PlotManager.cxx` to create plot instance:

```cpp
#include "MyNewPlot.h"  // Add include

BasePlot* PlotManager::CreatePlot(PlotType type) {
    switch(type) {
        case kHistogram:
            return new HistogramPlot(fData, fCanvas);
        case kScatter:
            return new ScatterPlot(fData, fCanvas);
        case kLine:
            return new LinePlot(fData, fCanvas);
        case kMyNewPlot:  // Add your case
            return new MyNewPlot(fData, fCanvas);
        default:
            return nullptr;
    }
}
```

#### Step 3: Add to GUI Menu

Edit `AdvancedPlotGUI.cxx` to add menu item:

```cpp
void AdvancedPlotGUI::CreateMenuBar() {
    // ... existing code ...
    
    fMenuPlot->AddEntry("My New Plot", MENU_MYNEWPLOT);
    fMenuPlot->AddSeparator();
    
    // ... rest of menu ...
}

void AdvancedPlotGUI::HandleMenu(Int_t id) {
    switch(id) {
        // ... existing cases ...
        
        case MENU_MYNEWPLOT:
            SetPlotType(PlotManager::kMyNewPlot);
            DoPlot();
            break;
    }
}
```

#### Step 4: Update CMakeLists.txt

Add your new files to the build:

```cmake
set(SOURCES
    src/AdvancedPlotGUI.cxx
    # ... existing sources ...
    plots/MyNewPlot.cxx
)

set(HEADERS
    include/AdvancedPlotGUI.h
    # ... existing headers ...
    plots/MyNewPlot.h
)
```

#### Step 5: Add Tests

Create `tests/test_mynewplot.cxx`:

```cpp
#include "MyNewPlot.h"
#include "DataContainer.h"
#include <TCanvas.h>
#include <cassert>
#include <iostream>

void test_mynewplot() {
    // Create test data
    DataContainer* data = new DataContainer();
    for (Int_t i = 0; i < 100; i++) {
        data->AddPoint(i, i*i);
    }
    
    // Create canvas
    TCanvas* canvas = new TCanvas("c1", "Test Canvas", 800, 600);
    
    // Create plot
    MyNewPlot* plot = new MyNewPlot(data, canvas);
    
    // Test drawing
    bool success = plot->Draw();
    assert(success && "Plot drawing failed");
    
    // Test update
    plot->Update();
    
    // Cleanup
    delete plot;
    delete canvas;
    delete data;
    
    std::cout << "MyNewPlot tests passed!" << std::endl;
}

int main() {
    test_mynewplot();
    return 0;
}
```

#### Step 6: Document

Add Doxygen comments (as shown above) and update UserManual.md:

```markdown
### My New Plot

**Description**: Brief description of the plot type

**When to Use**: 
- Use case 1
- Use case 2

**Example**:
- Load data
- Select "My New Plot" from menu
- Customize parameters
```

---

## How to Add New File Formats

### Step-by-Step Guide

#### Step 1: Create Parser Class

Create `parsers/MyFormatParser.h` and `parsers/MyFormatParser.cxx`:

**MyFormatParser.h:**
```cpp
/**
 * @file MyFormatParser.h
 * @brief Parser for .myformat files
 */

#ifndef MYFORMATPARSER_H
#define MYFORMATPARSER_H

#include "BaseParser.h"
#include <string>

/**
 * @class MyFormatParser
 * @brief Parses data from .myformat files
 * 
 * @details
 * File format specification:
 * - Line 1: Header with metadata
 * - Following lines: Data in specific format
 * - Comments start with '#'
 */
class MyFormatParser : public BaseParser {

private:
    std::string fDelimiter;  ///< Field delimiter
    bool fHasHeader;         ///< Whether file has header row
    
    /**
     * @brief Parse header line
     * @param line Header string
     * @return true if valid header
     */
    bool ParseHeader(const std::string& line);
    
    /**
     * @brief Parse data line
     * @param line Data string
     * @return true if valid data
     */
    bool ParseDataLine(const std::string& line);

public:
    /**
     * @brief Constructor
     */
    MyFormatParser();
    
    /**
     * @brief Destructor
     */
    virtual ~MyFormatParser();
    
    /**
     * @brief Parse file and extract data
     * @param filename Path to .myformat file
     * @param data Output data container
     * @return true if parsing successful
     */
    virtual bool Parse(const char* filename, DataContainer* data) override;
    
    /**
     * @brief Validate file format
     * @param filename Path to file
     * @return true if file is valid .myformat
     */
    virtual bool Validate(const char* filename) override;
    
    /**
     * @brief Set field delimiter
     * @param delimiter Delimiter character(s)
     */
    void SetDelimiter(const std::string& delimiter) { fDelimiter = delimiter; }
    
    ClassDef(MyFormatParser, 1) // Parser for .myformat files
};

#endif // MYFORMATPARSER_H
```

**MyFormatParser.cxx:**
```cpp
#include "MyFormatParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

ClassImp(MyFormatParser)

//____________________________________________________________________________
MyFormatParser::MyFormatParser()
    : BaseParser(),
      fDelimiter(","),
      fHasHeader(true)
{
}

//____________________________________________________________________________
MyFormatParser::~MyFormatParser()
{
}

//____________________________________________________________________________
bool MyFormatParser::Validate(const char* filename)
{
    // Check file extension
    std::string fname(filename);
    if (fname.substr(fname.find_last_of(".") + 1) != "myformat") {
        return false;
    }
    
    // Check if file exists and is readable
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }
    
    // Additional validation logic
    // e.g., check magic number, format version, etc.
    
    file.close();
    return true;
}

//____________________________________________________________________________
bool MyFormatParser::Parse(const char* filename, DataContainer* data)
{
    if (!Validate(filename)) {
        return false;
    }
    
    std::ifstream file(filename);
    std::string line;
    Int_t lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse header
        if (lineNum == 1 && fHasHeader) {
            if (!ParseHeader(line)) {
                std::cerr << "Invalid header at line " << lineNum << std::endl;
                return false;
            }
            continue;
        }
        
        // Parse data lines
        if (!ParseDataLine(line)) {
            std::cerr << "Invalid data at line " << lineNum << std::endl;
            return false;
        }
    }
    
    file.close();
    return true;
}

//____________________________________________________________________________
bool MyFormatParser::ParseHeader(const std::string& line)
{
    // Implement header parsing logic
    // Extract column names, metadata, etc.
    
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, fDelimiter[0])) {
        // Process header token
        fColumnNames.push_back(token);
    }
    
    return true;
}

//____________________________________________________________________________
bool MyFormatParser::ParseDataLine(const std::string& line)
{
    // Implement data parsing logic
    // Extract values and add to data container
    
    std::stringstream ss(line);
    std::string token;
    std::vector<Double_t> values;
    
    while (std::getline(ss, token, fDelimiter[0])) {
        try {
            Double_t value = std::stod(token);
            values.push_back(value);
        } catch (...) {
            return false;
        }
    }
    
    // Add to data container
    if (values.size() >= 2) {
        fData->AddPoint(values[0], values[1]);
        if (values.size() >= 4) {
            fData->SetError(values[2], values[3]);
        }
    }
    
    return true;
}
```

#### Step 2: Register Parser in DataManager

Edit `DataManager.h`:

```cpp
enum FileFormat {
    kROOT = 0,
    kCSV = 1,
    kTXT = 2,
    kMyFormat = 3,  // Add new format
    kUnknown
};
```

Edit `DataManager.cxx`:

```cpp
#include "MyFormatParser.h"  // Add include

FileFormat DataManager::DetectFormat(const char* filename) {
    std::string fname(filename);
    std::string ext = fname.substr(fname.find_last_of(".") + 1);
    
    if (ext == "root") return kROOT;
    if (ext == "csv") return kCSV;
    if (ext == "txt" || ext == "dat") return kTXT;
    if (ext == "myformat") return kMyFormat;  // Add detection
    
    return kUnknown;
}

bool DataManager::LoadFile(const char* filename) {
    FileFormat format = DetectFormat(filename);
    BaseParser* parser = nullptr;
    
    switch(format) {
        case kROOT:
            parser = new ROOTParser();
            break;
        case kCSV:
            parser = new CSVParser();
            break;
        case kTXT:
            parser = new TextParser();
            break;
        case kMyFormat:  // Add case
            parser = new MyFormatParser();
            break;
        default:
            return false;
    }
    
    bool success = parser->Parse(filename, fData);
    delete parser;
    return success;
}
```

#### Step 3: Update File Dialog Filter

Edit `AdvancedPlotGUI.cxx`:

```cpp
void AdvancedPlotGUI::DoLoad() {
    TGFileInfo fileInfo;
    const char* fileTypes[] = {
        "All supported files", "*.root;*.csv;*.txt;*.dat;*.myformat",
        "ROOT files", "*.root",
        "CSV files", "*.csv",
        "Text files", "*.txt;*.dat",
        "MyFormat files", "*.myformat",  // Add new type
        "All files", "*",
        nullptr, nullptr
    };
    
    fileInfo.fFileTypes = fileTypes;
    new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fileInfo);
    
    // ... rest of implementation ...
}
```

#### Step 4: Add Tests

Create `tests/test_myformat_parser.cxx`:

```cpp
#include "MyFormatParser.h"
#include "DataContainer.h"
#include <cassert>
#include <iostream>
#include <fstream>

void createTestFile() {
    std::ofstream file("test_data.myformat");
    file << "# Test file" << std::endl;
    file << "x,y,ex,ey" << std::endl;
    file << "1.0,2.0,0.1,0.2" << std::endl;
    file << "2.0,4.5,0.1,0.3" << std::endl;
    file << "3.0,6.2,0.1,0.2" << std::endl;
    file.close();
}

void test_myformat_parser() {
    createTestFile();
    
    DataContainer* data = new DataContainer();
    MyFormatParser* parser = new MyFormatParser();
    
    // Test validation
    assert(parser->Validate("test_data.myformat"));
    
    // Test parsing
    bool success = parser->Parse("test_data.myformat", data);
    assert(success);
    assert(data->GetNPoints() == 3);
    
    // Cleanup
    delete parser;
    delete data;
    
    std::cout << "MyFormatParser tests passed!" << std::endl;
}

int main() {
    test_myformat_parser();
    return 0;
}
```

#### Step 5: Document

Update UserManual.md with format specification and examples.

---

## How to Add New Fit Functions

### Step-by-Step Guide

#### Step 1: Create Fit Function Class

Create `fits/MyCustomFit.h` and `fits/MyCustomFit.cxx`:

**MyCustomFit.h:**
```cpp
/**
 * @file MyCustomFit.h
 * @brief Custom fit function implementation
 */

#ifndef MYCUSTOMFIT_H
#define MYCUSTOMFIT_H

#include "BaseFit.h"
#include <TF1.h>

/**
 * @class MyCustomFit
 * @brief Implements custom mathematical fit function
 * 
 * @details
 * Function form: f(x) = p0 * your_function(x, p1, p2, ...)
 * 
 * Parameters:
 * - p[0]: Amplitude
 * - p[1]: Custom parameter 1
 * - p[2]: Custom parameter 2
 * 
 * Use cases:
 * - Specific application 1
 * - Specific application 2
 */
class MyCustomFit : public BaseFit {

private:
    Int_t fNParameters;  ///< Number of fit parameters
    
    /**
     * @brief Estimate initial parameters from data
     * @param data Data to fit
     */
    void EstimateInitialParameters(DataContainer* data);

public:
    /**
     * @brief Constructor
     */
    MyCustomFit();
    
    /**
     * @brief Destructor
     */
    virtual ~MyCustomFit();
    
    /**
     * @brief Create TF1 function object
     * @param xmin Lower fit range
     * @param xmax Upper fit range
     * @return Pointer to TF1 object
     */
    virtual TF1* CreateFunction(Double_t xmin, Double_t xmax) override;
    
    /**
     * @brief Perform the fit
     * @param hist Histogram to fit
     * @param options Fit options string
     * @return Fit status (0 = success)
     */
    virtual Int_t Fit(TH1* hist, Option_t* options = "") override;
    
    /**
     * @brief Get fit function description
     * @return Description string
     */
    virtual const char* GetDescription() const override {
        return "Custom Function: f(x) = p0 * your_function(x, p1, p2)";
    }
    
    ClassDef(MyCustomFit, 1) // Custom fit function
};

// Define the actual mathematical function
Double_t MyCustomFunction(Double_t *x, Double_t *par);

#endif // MYCUSTOMFIT_H
```

**MyCustomFit.cxx:**
```cpp
#include "MyCustomFit.h"
#include <TMath.h>
#include <iostream>

ClassImp(MyCustomFit)

//____________________________________________________________________________
// Define the mathematical function
Double_t MyCustomFunction(Double_t *x, Double_t *par) {
    // par[0] = amplitude
    // par[1] = parameter 1
    // par[2] = parameter 2
    
    Double_t xx = x[0];
    
    // Example: Damped oscillation
    // f(x) = A * exp(-x/tau) * sin(omega*x + phi)
    Double_t amplitude = par[0];
    Double_t tau = par[1];
    Double_t omega = par[2];
    Double_t phi = par[3];
    
    Double_t value = amplitude * TMath::Exp(-xx/tau) * TMath::Sin(omega*xx + phi);
    
    return value;
}

//____________________________________________________________________________
MyCustomFit::MyCustomFit()
    : BaseFit(),
      fNParameters(4)
{
}

//____________________________________________________________________________
MyCustomFit::~MyCustomFit()
{
}

//____________________________________________________________________________
TF1* MyCustomFit::CreateFunction(Double_t xmin, Double_t xmax)
{
    TF1* func = new TF1("mycustomfit", MyCustomFunction, xmin, xmax, fNParameters);
    
    // Set parameter names
    func->SetParName(0, "Amplitude");
    func->SetParName(1, "Tau");
    func->SetParName(2, "Omega");
    func->SetParName(3, "Phi");
    
    // Set reasonable limits
    func->SetParLimits(0, 0, 1e6);     // Amplitude > 0
    func->SetParLimits(1, 0.1, 100);   // Tau > 0
    func->SetParLimits(2, 0, 10);      // Omega
    func->SetParLimits(3, -TMath::Pi(), TMath::Pi());  // Phi
    
    return func;
}

//____________________________________________________________________________
void MyCustomFit::EstimateInitialParameters(DataContainer* data)
{
    // Estimate initial parameter values from data
    // This improves fit convergence
    
    Double_t maxY = data->GetYMax();
    Double_t meanX = data->GetXMean();
    
    fInitialParams[0] = maxY;           // Amplitude ~ max value
    fInitialParams[1] = meanX;          // Tau ~ mean x
    fInitialParams[2] = 1.0;            // Omega ~ 1
    fInitialParams[3] = 0.0;            // Phi ~ 0
}

//____________________________________________________________________________
Int_t MyCustomFit::Fit(TH1* hist, Option_t* options)
{
    if (!hist) {
        std::cerr << "Error: Invalid histogram" << std::endl;
        return -1;
    }
    
    Double_t xmin = hist->GetXaxis()->GetXmin();
    Double_t xmax = hist->GetXaxis()->GetXmax();
    
    TF1* func = CreateFunction(xmin, xmax);
    
    // Set initial parameters
    EstimateInitialParameters(nullptr);  // Could pass data container
    for (Int_t i = 0; i < fNParameters; i++) {
        func->SetParameter(i, fInitialParams[i]);
    }
    
    // Perform fit
    Int_t status = hist->Fit(func, options);
    
    // Store results
    fFitFunction = func;
    fFitStatus = status;
    
    return status;
}
```

#### Step 2: Register in FitEngine

Edit `FitEngine.h`:

```cpp
enum FitType {
    kGaussian = 0,
    kPolynomial = 1,
    kExponential = 2,
    kLandau = 3,
    kMyCustom = 4,  // Add new fit type
    kNFitTypes
};
```

Edit `FitEngine.cxx`:

```cpp
#include "MyCustomFit.h"  // Add include

BaseFit* FitEngine::CreateFit(FitType type) {
    switch(type) {
        case kGaussian:
            return new GaussianFit();
        case kPolynomial:
            return new PolynomialFit();
        case kExponential:
            return new ExponentialFit();
        case kLandau:
            return new LandauFit();
        case kMyCustom:  // Add case
            return new MyCustomFit();
        default:
            return nullptr;
    }
}

const char* FitEngine::GetFitName(FitType type) {
    switch(type) {
        case kGaussian: return "Gaussian";
        case kPolynomial: return "Polynomial";
        case kExponential: return "Exponential";
        case kLandau: return "Landau";
        case kMyCustom: return "My Custom Fit";  // Add name
        default: return "Unknown";
    }
}
```

#### Step 3: Add to GUI Menu

Edit `AdvancedPlotGUI.cxx`:

```cpp
void AdvancedPlotGUI::CreateMenuBar() {
    // ... existing code ...
    
    fMenuFit->AddEntry("My Custom Fit", MENU_FIT_CUSTOM);
    
    // Also add to combo box
    fFitFuncCombo->AddEntry("My Custom Fit", FitEngine::kMyCustom);
}

void AdvancedPlotGUI::HandleMenu(Int_t id) {
    switch(id) {
        // ... existing cases ...
        
        case MENU_FIT_CUSTOM:
            fFitEngine->SetFitType(FitEngine::kMyCustom);
            DoFit();
            break;
    }
}
```

#### Step 4: Add Tests

Create `tests/test_mycustomfit.cxx`:

```cpp
#include "MyCustomFit.h"
#include <TH1D.h>
#include <TRandom3.h>
#include <cassert>
#include <iostream>

void test_mycustomfit() {
    // Create test histogram with known parameters
    TH1D* hist = new TH1D("test", "Test", 100, 0, 10);
    
    TRandom3 rnd(42);
    for (Int_t i = 0; i < 10000; i++) {
        Double_t x = rnd.Uniform(0, 10);
        Double_t y = 5.0 * TMath::Exp(-x/2.0) * TMath::Sin(1.5*x);
        hist->Fill(x, y);
    }
    
    // Perform fit
    MyCustomFit* fit = new MyCustomFit();
    Int_t status = fit->Fit(hist, "Q");
    
    // Check fit succeeded
    assert(status == 0);
    
    // Check parameters are reasonable
    TF1* func = fit->GetFunction();
    Double_t amp = func->GetParameter(0);
    Double_t tau = func->GetParameter(1);
    
    assert(amp > 0);
    assert(tau > 0);
    
    // Cleanup
    delete fit;
    delete hist;
    
    std::cout << "MyCustomFit tests passed!" << std::endl;
}

int main() {
    test_mycustomfit();
    return 0;
}
```

#### Step 5: Document

Update UserManual.md with:
- Function formula and parameters
- When to use this fit
- Example usage
- Interpretation of parameters

---

## Style Guide and Best Practices

### Code Style

#### Naming Conventions

```cpp
// Classes: PascalCase
class AdvancedPlotGUI { };

// Member variables: fVariableName (f prefix)
Double_t fAmplitude;
TH1D* fHistogram;

// Member functions: PascalCase
void DoLoad();
bool CreatePlot();

// Local variables: camelCase
Int_t numPoints = 100;
Double_t meanValue = 0.0;

// Constants: kConstantName (k prefix)
const Int_t kMaxIterations = 1000;
const Double_t kTolerance = 1e-6;

// Enums: kEnumValue
enum PlotType {
    kHistogram,
    kScatter
};
```

#### File Organization

```cpp
// 1. Copyright/license header
/**
 * Copyright (C) 2026
 * License information
 */

// 2. File documentation
/**
 * @file ClassName.h
 * @brief Brief description
 * @details Detailed description
 */

// 3. Include guards
#ifndef CLASSNAME_H
#define CLASSNAME_H

// 4. System includes
#include <iostream>
#include <string>

// 5. ROOT includes
#include <TH1.h>
#include <TCanvas.h>

// 6. Project includes
#include "BaseClass.h"

// 7. Class declaration
// 8. Implementation (in .cxx file)

#endif // CLASSNAME_H
```

#### Commenting

```cpp
/**
 * @brief Short description (one line)
 * 
 * @details Longer description with implementation details,
 * algorithm explanation, usage notes, etc.
 * 
 * @param param1 Description of parameter 1
 * @param param2 Description of parameter 2
 * @return Description of return value
 * 
 * @note Important notes
 * @warning Warnings about usage
 * @see RelatedFunction()
 * 
 * Example usage:
 * @code
 * MyClass obj;
 * obj.MyFunction(10, 20);
 * @endcode
 */
void MyFunction(Int_t param1, Double_t param2);
```

### ROOT-Specific Best Practices

#### Memory Management

```cpp
// BAD: Memory leak
void BadExample() {
    TH1D* hist = new TH1D("h1", "Hist", 100, 0, 10);
    // ... use hist ...
    // Never deleted!
}

// GOOD: Proper cleanup
void GoodExample() {
    TH1D* hist = new TH1D("h1", "Hist", 100, 0, 10);
    // ... use hist ...
    delete hist;
}

// BETTER: Use ROOT's ownership
void BetterExample() {
    TFile* file = new TFile("output.root", "RECREATE");
    TH1D* hist = new TH1D("h1", "Hist", 100, 0, 10);
    // ... fill hist ...
    hist->Write();  // ROOT takes ownership
    file->Close();
    delete file;  // Automatically deletes hist
}
```

#### Canvas Management

```cpp
// Update canvas after modifications
void UpdateCanvas() {
    fCanvas->cd();
    fHistogram->Draw();
    fCanvas->Modified();
    fCanvas->Update();
}

// Save canvas state
void SaveCanvas() {
    fCanvas->SaveAs("plot.png");
    fCanvas->SaveAs("plot.pdf");
    fCanvas->SaveAs("plot.C");  // Saves as macro
}
```

### Error Handling

```cpp
// Always validate inputs
bool LoadData(const char* filename) {
    if (!filename || strlen(filename) == 0) {
        std::cerr << "Error: Empty filename" << std::endl;
        return false;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open " << filename << std::endl;
        return false;
    }
    
    // ... process file ...
    
    return true;
}

// Use return codes
enum Status {
    kSuccess = 0,
    kErrorFile = 1,
    kErrorData = 2,
    kErrorFit = 3
};
```

### Performance Tips

```cpp
// BAD: Repeated calculations in loop
for (Int_t i = 0; i < nPoints; i++) {
    Double_t value = func->Eval(data->GetX(i));  // Expensive
    hist->Fill(value);
}

// GOOD: Cache results when possible
std::vector<Double_t> values(nPoints);
for (Int_t i = 0; i < nPoints; i++) {
    values[i] = func->Eval(data->GetX(i));
}
for (Int_t i = 0; i < nPoints; i++) {
    hist->Fill(values[i]);
}

// BETTER: Use ROOT's built-in batch operations
// Many ROOT functions have optimized batch modes
```

---

## Testing

### Unit Testing Framework

We use ROOT's built-in testing capabilities with custom test macros.

#### Running Tests

```bash
# Run all tests
cd build
make test

# Run specific test
./tests/test_data_loading

# Run with ROOT
root -l -b -q tests/test_plotting.C
```

#### Writing Unit Tests

```cpp
/**
 * @file test_example.cxx
 * @brief Unit tests for Example class
 */

#include "Example.h"
#include <cassert>
#include <iostream>

// Test basic functionality
void test_basic() {
    Example obj;
    assert(obj.IsValid());
    std::cout << "✓ Basic test passed" << std::endl;
}

// Test edge cases
void test_edge_cases() {
    Example obj;
    
    // Test with nullptr
    assert(!obj.ProcessData(nullptr));
    
    // Test with empty data
    DataContainer empty;
    assert(!obj.ProcessData(&empty));
    
    std::cout << "✓ Edge case tests passed" << std::endl;
}

// Test error handling
void test_error_handling() {
    Example obj;
    
    // Should handle invalid input gracefully
    bool threw = false;
    try {
        obj.RiskyOperation();
    } catch (...) {
        threw = true;
    }
    assert(!threw);  // Should not throw
    
    std::cout << "✓ Error handling tests passed" << std::endl;
}

int main() {
    std::cout << "Running Example tests..." << std::endl;
    
    test_basic();
    test_edge_cases();
    test_error_handling();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
```

### Integration Testing

Test complete workflows:

```cpp
void test_complete_workflow() {
    // Create GUI
    AdvancedPlotGUI* gui = new AdvancedPlotGUI(
        gClient->GetRoot(), 800, 600);
    
    // Load data
    assert(gui->DoLoad("test_data.csv"));
    
    // Create plot
    gui->SetPlotType(PlotManager::kHistogram);
    assert(gui->DoPlot());
    
    // Fit data
    assert(gui->DoFit());
    
    // Export
    assert(gui->DoExport());
    
    delete gui;
}
```

### Test Data

Create test datasets in `examples/test_data/`:

```bash
examples/test_data/
├── simple_gaussian.csv     # Basic Gaussian distribution
├── multi_peak.root         # Multiple peaks for fitting
├── scatter_data.txt        # Scatter plot data
└── time_series.dat         # Time-dependent data
```

---

## Contributing

### Getting Started

1. **Fork the Repository**
   ```bash
   # On GitHub, click "Fork"
   git clone https://github.com/YOUR_USERNAME/Advance_ROOT_Plotting.git
   ```

2. **Create a Branch**
   ```bash
   git checkout -b feature/my-new-feature
   ```

3. **Make Changes**
   - Follow the style guide
   - Add tests
   - Update documentation

4. **Commit Changes**
   ```bash
   git add .
   git commit -m "Add feature: description"
   ```

5. **Push and Create Pull Request**
   ```bash
   git push origin feature/my-new-feature
   # Create PR on GitHub
   ```

### Commit Message Format

```
type(scope): brief description

Detailed description of changes.

- Bullet point 1
- Bullet point 2

Fixes #issue_number
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code formatting
- `refactor`: Code restructuring
- `test`: Adding tests
- `chore`: Build/maintenance tasks

### Code Review Process

1. **Automated Checks**: CI runs tests and linting
2. **Peer Review**: At least one approving review required
3. **Discussion**: Address comments and suggestions
4. **Merge**: Squash and merge when approved

### Documentation Requirements

All contributions must include:
- Doxygen comments for public APIs
- User Manual updates (if user-facing)
- Developer Guide updates (if architectural)
- Test cases
- Example usage (if applicable)

---

## Appendix

### Useful ROOT Resources

- **Official Documentation**: https://root.cern.ch/doc/master/
- **Tutorials**: https://root.cern.ch/doc/master/group__Tutorials.html
- **Forum**: https://root-forum.cern.ch/
- **GitHub**: https://github.com/root-project/root

### Related Projects

- **RooFit**: Statistical modeling framework
- **TMVA**: Machine learning toolkit  
- **RDataFrame**: High-level data analysis interface

### Contact

- **Maintainer**: Siddharth Parashari
- **Repository**: https://github.com/siddharthparashri5/Advance_ROOT_Plotting
- **Issues**: Use GitHub issue tracker
- **Email**: [your-email@example.com]

---

**Document Version**: 1.0  
**Last Updated**: February 4, 2026  
**Next Review**: March 4, 2026
