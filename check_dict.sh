#!/bin/bash
# Dictionary Generation Debug Script

echo "=== Checking Directory Structure ==="
echo "Current directory: $(pwd)"
echo ""

echo "Include directory contents:"
ls -la include/ 2>/dev/null || echo "  include/ directory not found!"
echo ""

echo "Source directory contents:"
ls -la src/ 2>/dev/null || echo "  src/ directory not found!"
echo ""

echo "=== Checking Header Files ==="
if [ -f "include/ColumnSelector.h" ]; then
    echo "✓ include/ColumnSelector.h found"
else
    echo "✗ include/ColumnSelector.h NOT found"
fi

if [ -f "include/ColumnSelectorLinkDef.h" ]; then
    echo "✓ include/ColumnSelectorLinkDef.h found"
else
    echo "✗ include/ColumnSelectorLinkDef.h NOT found"
fi

echo ""
echo "=== Checking ColumnSelectorLinkDef.h content ==="
if [ -f "include/ColumnSelectorLinkDef.h" ]; then
    cat include/ColumnSelectorLinkDef.h
else
    echo "File not found!"
fi

echo ""
echo "=== Testing rootcling command ==="
if [ -f "include/ColumnSelector.h" ] && [ -f "include/ColumnSelectorLinkDef.h" ]; then
    echo "Running: rootcling -f src/ColumnSelectorDict.cpp -I./include include/ColumnSelector.h include/ColumnSelectorLinkDef.h"
    rootcling -f src/ColumnSelectorDict.cpp -I./include include/ColumnSelector.h include/ColumnSelectorLinkDef.h
    if [ $? -eq 0 ]; then
        echo "✓ Dictionary generation successful!"
    else
        echo "✗ Dictionary generation failed!"
    fi
else
    echo "Missing required files!"
fi
