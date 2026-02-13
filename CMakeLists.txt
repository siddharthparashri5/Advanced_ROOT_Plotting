cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# ============================================================================
# Project
# ============================================================================
project(AdvancedPlotGUI
    VERSION 2.3
    DESCRIPTION "Advanced ROOT plotting GUI"
    LANGUAGES CXX
)

# ============================================================================
# C++ Standard
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Build Type
# ============================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ============================================================================
# Find ROOT
# ============================================================================
find_package(ROOT REQUIRED COMPONENTS
    Core RIO Tree Hist Graf Gpad Gui Rint RooFit RooFitCore
)

include(${ROOT_USE_FILE})
message(STATUS "Using ROOT ${ROOT_VERSION}")

# ============================================================================
# Include directories
# ============================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)

# ============================================================================
# Sources
# ============================================================================
set(LIB_SOURCES
    src/AdvancedPlotGUI.cpp
    src/ColumnSelectorDialog.cpp
    src/CSVPreviewDialog.cpp
    src/RootDataInspector.cpp
    src/ROOTFileBrowser.cpp
    src/FileHandler.cpp
    src/PlotManager.cpp
    src/ScriptEngine.cpp
    src/PlotTypes.cpp
)

set(CMAKE_ROOT_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)

# Headers that require dictionary
set(DICT_HEADERS
    AdvancedPlotGUI.h
    ColumnSelectorDialog.h
    CSVPreviewDialog.h
    RootDataInspector.h
    ROOTFileBrowser.h
)

# ============================================================================
# Generate ROOT dictionary
# ============================================================================
ROOT_GENERATE_DICTIONARY(
    AdvancedPlotGUIDict
    ${DICT_HEADERS}
    #LINKDEF include/LinkDef.h
)

# ============================================================================
# Shared Library (GUI + ROOT classes)
# ============================================================================
add_library(AdvancedPlotGUI SHARED
    ${LIB_SOURCES}
     ${CMAKE_CURRENT_BINARY_DIR}/AdvancedPlotGUIDict.cxx
)

target_link_libraries(AdvancedPlotGUI
    Core
    RIO
    Tree
    Hist
    Graf
    Gpad
    Net
    Gui
    Rint
    RooFit
    RooFitCore
    ROOTTPython
)

target_include_directories(AdvancedPlotGUI
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(AdvancedPlotGUI PROPERTIES
    OUTPUT_NAME AdvancedPlotGUI
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Optional compile flags to reduce ROOT macro warnings
target_compile_options(AdvancedPlotGUI PRIVATE -Wall -Wno-unused-variable)

# ============================================================================
# Executable (tiny launcher)
# ============================================================================
add_executable(AdvancedPlotGUIApp
    main.cpp
)

target_link_libraries(AdvancedPlotGUIApp
    AdvancedPlotGUI
)

set_target_properties(AdvancedPlotGUIApp PROPERTIES
    OUTPUT_NAME AdvancedPlotGUIApp
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    INSTALL_RPATH "${CMAKE_BINARY_DIR}:${ROOT_LIBRARY_DIR}"
)

# ============================================================================
# Installation
# ============================================================================
install(TARGETS AdvancedPlotGUI AdvancedPlotGUIApp
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "================================================")
message(STATUS " AdvancedPlotGUI (Canonical ROOT Layout)")
message(STATUS "================================================")
message(STATUS " ROOT version : ${ROOT_VERSION}")
message(STATUS " Build type   : ${CMAKE_BUILD_TYPE}")
message(STATUS " Library      : libAdvancedPlotGUI.so")
message(STATUS " Executable   : AdvancedPlotGUIApp")
message(STATUS "================================================")
message(STATUS "")

