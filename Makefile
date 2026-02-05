# ============================================================================
# Makefile for Advanced ROOT Plotting GUI
# Author: Siddharth Parashri
# Date: 2026-02-05
# Version: 2.0
# ============================================================================

# ============================================================================
# Compiler and Tools
# ============================================================================
CXX = g++
RM = rm -rf
MKDIR = mkdir -p

# ============================================================================
# Directory Structure
# ============================================================================
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# ============================================================================
# ROOT Configuration
# ============================================================================
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS  = $(shell root-config --glibs)

# ============================================================================
# Python Configuration (adjust version as needed)
# ============================================================================
PYTHON_INC = -I/usr/include/python3.10
PYTHON_LIB = -lpython3.10

# ============================================================================
# Compiler Flags
# ============================================================================
CXXFLAGS = -Wall -std=c++17 $(ROOTCFLAGS) -fno-PIC -I$(INC_DIR) $(PYTHON_INC)

# ============================================================================
# Linker Flags
# ============================================================================
LDFLAGS = $(ROOTLIBS) $(ROOTGLIBS) -lGui -lRooFit -lRooFitCore -lROOTTPython $(PYTHON_LIB) -no-pie

# ============================================================================
# Target Executable
# ============================================================================
TARGET = AdvancedPlotGUI

# ============================================================================
# Source Files
# ============================================================================
SOURCES = $(SRC_DIR)/AdvancedPlotGUI.cpp \
          $(SRC_DIR)/ColumnSelector.cpp

# ============================================================================
# Header Files
# ============================================================================
HEADERS = $(INC_DIR)/AdvancedPlotGUI.h \
          $(INC_DIR)/DataReader.h \
          $(INC_DIR)/ColumnSelector.h \
          $(INC_DIR)/PlotTypes.h \
          $(INC_DIR)/FitUtils.h \
          $(INC_DIR)/ErrorHandling.h \
          $(INC_DIR)/ColumnSelectorLinkDef.h

# ============================================================================
# Object Files
# ============================================================================
OBJECTS = $(OBJ_DIR)/AdvancedPlotGUI.o \
          $(OBJ_DIR)/ColumnSelector.o

# ============================================================================
# ROOT Dictionary
# ============================================================================
DICT_SRC = $(SRC_DIR)/ColumnSelectorDict.cpp
DICT_OBJ = $(OBJ_DIR)/ColumnSelectorDict.o
DICT_PCM = ColumnSelectorDict_rdict.pcm
DICT_ROOTMAP = $(OBJ_DIR)/ColumnSelectorDict.rootmap

# ============================================================================
# Default Target
# ============================================================================
all: directories $(TARGET)
	@echo ""
	@echo "================================================"
	@echo "  Build complete! Executable: ./$(TARGET)"
	@echo "================================================"

# ============================================================================
# Create Directory Structure
# ============================================================================
directories:
	@echo "Creating directory structure..."
	@$(MKDIR) $(OBJ_DIR)
	@$(MKDIR) $(SRC_DIR)
	@$(MKDIR) $(INC_DIR)

# ============================================================================
# Compile Source Files
# ============================================================================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================================
# Generate ROOT Dictionary
# ============================================================================
$(DICT_SRC): $(INC_DIR)/ColumnSelector.h $(INC_DIR)/ColumnSelectorLinkDef.h
	@echo "Generating ROOT dictionary..."
	@cd $(INC_DIR) && rootcling -f ../$(DICT_SRC) -rmf ../$(DICT_ROOTMAP) -rml libColumnSelector.so ColumnSelector.h ColumnSelectorLinkDef.h
	@if [ -f $(DICT_PCM) ]; then echo "Note: $(DICT_PCM) generated in project root"; fi

# ============================================================================
# Compile ROOT Dictionary
# ============================================================================
$(DICT_OBJ): $(DICT_SRC)
	@echo "Compiling ROOT dictionary..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================================
# Link the Executable
# ============================================================================
$(TARGET): $(OBJECTS) $(DICT_OBJ)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(OBJECTS) $(DICT_OBJ) -o $(TARGET) $(LDFLAGS)

# ============================================================================
# Clean Build Artifacts
# ============================================================================
clean:
	@echo "Cleaning build artifacts..."
	@$(RM) $(OBJ_DIR)/*.o
	@$(RM) $(DICT_SRC)
	@$(RM) $(DICT_PCM)
	@$(RM) $(DICT_ROOTMAP)
	@$(RM) $(TARGET)
	@echo "Clean complete!"

# ============================================================================
# Deep Clean (removes directories)
# ============================================================================
distclean: clean
	@echo "Performing deep clean..."
	@$(RM) $(OBJ_DIR)
	@echo "Deep clean complete!"

# ============================================================================
# Run the Application
# ============================================================================
run: $(TARGET)
	@echo "Running $(TARGET)..."
	@./$(TARGET)

# ============================================================================
# Setup Project Structure (one-time setup)
# ============================================================================
setup:
	@echo "Setting up project structure..."
	@$(MKDIR) $(SRC_DIR)
	@$(MKDIR) $(INC_DIR)
	@$(MKDIR) $(OBJ_DIR)
	@echo "Project structure created:"
	@echo "  $(SRC_DIR)/  - Source files (.cpp)"
	@echo "  $(INC_DIR)/  - Header files (.h)"
	@echo "  $(OBJ_DIR)/  - Object files (.o)"
	@echo ""
	@echo "Please move your files to the appropriate directories:"
	@echo "  - Move *.cpp files to $(SRC_DIR)/"
	@echo "  - Move *.h files to $(INC_DIR)/"

# ============================================================================
# Install (copy executable to system path - requires sudo)
# ============================================================================
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin..."
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete!"

# ============================================================================
# Uninstall
# ============================================================================
uninstall:
	@echo "Removing $(TARGET) from /usr/local/bin..."
	@sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstallation complete!"

# ============================================================================
# Debug Build (with debug symbols)
# ============================================================================
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean all
	@echo "Debug build complete!"

# ============================================================================
# Release Build (optimized)
# ============================================================================
release: CXXFLAGS += -O3 -DNDEBUG
release: clean all
	@echo "Release build complete!"

# ============================================================================
# Check Dependencies
# ============================================================================
check-deps:
	@echo "Checking dependencies..."
	@which root-config > /dev/null || (echo "ERROR: ROOT not found!" && exit 1)
	@which g++ > /dev/null || (echo "ERROR: g++ not found!" && exit 1)
	@which python3 > /dev/null || (echo "ERROR: Python3 not found!" && exit 1)
	@echo "All dependencies found!"

# ============================================================================
# Show Project Statistics
# ============================================================================
stats:
	@echo "Project Statistics:"
	@echo "  Source files:  $(words $(SOURCES))"
	@echo "  Header files:  $(words $(HEADERS))"
	@echo "  Total lines:"
	@wc -l $(SOURCES) $(HEADERS) 2>/dev/null | tail -1 || echo "  Unable to count"

# ============================================================================
# Print Help
# ============================================================================
help:
	@echo "================================================"
	@echo "  Advanced Plot GUI - Makefile Help"
	@echo "================================================"
	@echo ""
	@echo "Build Targets:"
	@echo "  all         - Build the application (default)"
	@echo "  debug       - Build with debug symbols"
	@echo "  release     - Build optimized release version"
	@echo "  clean       - Remove build artifacts"
	@echo "  distclean   - Remove all generated files and directories"
	@echo ""
	@echo "Setup Targets:"
	@echo "  setup       - Create project directory structure"
	@echo "  check-deps  - Verify all dependencies are installed"
	@echo ""
	@echo "Run Targets:"
	@echo "  run         - Build and run the application"
	@echo ""
	@echo "Install Targets:"
	@echo "  install     - Install executable to /usr/local/bin"
	@echo "  uninstall   - Remove executable from /usr/local/bin"
	@echo ""
	@echo "Info Targets:"
	@echo "  stats       - Show project statistics"
	@echo "  help        - Display this help message"
	@echo ""
	@echo "Directory Structure:"
	@echo "  $(SRC_DIR)/     - C++ source files"
	@echo "  $(INC_DIR)/     - C++ header files"
	@echo "  $(OBJ_DIR)/     - Compiled object files"
	@echo ""

# ============================================================================
# Phony Targets
# ============================================================================
.PHONY: all clean distclean run setup install uninstall debug release check-deps stats help directories
