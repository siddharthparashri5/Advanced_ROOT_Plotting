# Makefile for Advanced ROOT Plotting GUI (non-PIE, RooFit enabled)

# Compiler
CXX = g++

# ROOT configuration
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS  = $(shell root-config --glibs)

# Compiler flags
CXXFLAGS = -Wall -std=c++17 $(ROOTCFLAGS) -fno-PIC

# Linker flags
# Add RooFit explicitly
LDFLAGS = $(ROOTLIBS) $(ROOTGLIBS) -lGui -lRooFit -lRooFitCore -no-pie

# Target executable
TARGET = AdvancedPlotGUI

# Source files
SOURCES = AdvancedPlotGUI.cpp ColumnSelector.cpp
HEADERS = DataReader.h ColumnSelector.h PlotTypes.h FitUtils.h ColumnSelectorLinkDef.h AdvancedPlotGUI.h

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# ROOT dictionary
DICT_SRC = ColumnSelectorDict.cpp
DICT_OBJ = ColumnSelectorDict.o

# Default target
all: $(TARGET)

# Compile source files
%.o: %.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate ROOT dictionary
$(DICT_SRC): ColumnSelector.h ColumnSelectorLinkDef.h
	@echo "Generating ROOT dictionary..."
	rootcling -f $@ -c ColumnSelector.h ColumnSelectorLinkDef.h

# Compile ROOT dictionary
$(DICT_OBJ): $(DICT_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the executable
$(TARGET): $(OBJECTS) $(DICT_OBJ)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) $(DICT_OBJ) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful!"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(DICT_OBJ) $(DICT_SRC) $(TARGET)
	@echo "Clean complete!"

# Run the application
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Print help
help:
	@echo "Available targets:"
	@echo "  all     - Build the application (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Build and run the application"
	@echo "  help    - Display this help message"

.PHONY: all clean run help

