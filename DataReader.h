#ifndef DATAREADER_H
#define DATAREADER_H

#include <TFile.h>
#include <TTree.h>
#include <TKey.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TList.h>
#include <TClass.h>
#include <TPad.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

//////////////////////////////
// Data structure to hold column data
//////////////////////////////
struct ColumnData {
    std::vector<std::string> headers;      // Column names/headers
    std::vector<std::vector<double>> data; // Column data
    std::string filename;
    
    int GetNumColumns() const { return headers.size(); }
    int GetNumRows() const { 
        return data.empty() ? 0 : data[0].size(); 
    }
};

//////////////////////////////
// DataReader class
//////////////////////////////
class DataReader {
public:
    enum FileType {
        kUnknown,
        kText,      // .txt, .dat
        kCSV,       // .csv
        kROOT       // .root
    };
    
    // Detect file type from extension
    static FileType GetFileType(const std::string& filename) {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) return kText; // No extension = text
        
        std::string ext = filename.substr(dotPos + 1);
        // Convert to lowercase
        for (char& c : ext) c = std::tolower(c);
        
        if (ext == "root") return kROOT;
        if (ext == "csv") return kCSV;
        if (ext == "txt" || ext == "dat") return kText;
        return kText; // Default to text
    }
    
    // Read text/dat file (space or tab separated)
    static bool ReadTextFile(const std::string& filename, ColumnData& data) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return false;
        }
        
        data.filename = filename;
        std::string line;
        bool firstLine = true;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue; // Skip empty and comment lines
            
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            
            while (ss >> token) {
                tokens.push_back(token);
            }
            
            if (tokens.empty()) continue;
            
            // First line could be headers (check if non-numeric)
            if (firstLine) {
                bool isHeader = false;
                try {
                    std::stod(tokens[0]);
                } catch (...) {
                    isHeader = true;
                }
                
                if (isHeader) {
                    data.headers = tokens;
                    data.data.resize(tokens.size());
                    firstLine = false;
                    continue;
                }
            }
            
            // If no header found, create default headers
            if (data.data.empty()) {
                data.data.resize(tokens.size());
                for (size_t i = 0; i < tokens.size(); ++i) {
                    data.headers.push_back(Form("Col%zu", i));
                }
            }
            
            // Parse data
            for (size_t i = 0; i < tokens.size() && i < data.data.size(); ++i) {
                try {
                    double val = std::stod(tokens[i]);
                    data.data[i].push_back(val);
                } catch (...) {
                    // Skip non-numeric values
                }
            }
            
            firstLine = false;
        }
        
        return !data.data.empty();
    }
    
    // Read CSV file (comma separated)
    static bool ReadCSVFile(const std::string& filename, ColumnData& data) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return false;
        }
        
        data.filename = filename;
        std::string line;
        bool firstLine = true;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                // Trim whitespace
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                tokens.push_back(token);
            }
            
            if (tokens.empty()) continue;
            
            // First line is typically header in CSV
            if (firstLine) {
                bool isHeader = false;
                try {
                    std::stod(tokens[0]);
                } catch (...) {
                    isHeader = true;
                }
                
                if (isHeader) {
                    data.headers = tokens;
                    data.data.resize(tokens.size());
                    firstLine = false;
                    continue;
                }
            }
            
            // Create default headers if needed
            if (data.data.empty()) {
                data.data.resize(tokens.size());
                for (size_t i = 0; i < tokens.size(); ++i) {
                    data.headers.push_back(Form("Col%zu", i));
                }
            }
            
            // Parse data
            for (size_t i = 0; i < tokens.size() && i < data.data.size(); ++i) {
                try {
                    double val = std::stod(tokens[i]);
                    data.data[i].push_back(val);
                } catch (...) {
                    // Skip non-numeric values
                }
            }
            
            firstLine = false;
        }
        
        return !data.data.empty();
    }
    
    // Helper function to extract data from TH1
    static bool ExtractFromTH1(TH1* hist, ColumnData& data) {
        if (!hist) return false;
        
        int nBins = hist->GetNbinsX();
        
        // Create two columns: bin center and bin content
        data.headers.push_back(Form("%s_BinCenter", hist->GetName()));
        data.headers.push_back(Form("%s_Content", hist->GetName()));
        data.data.resize(2);
        
        for (int i = 1; i <= nBins; ++i) {
            data.data[0].push_back(hist->GetBinCenter(i));
            data.data[1].push_back(hist->GetBinContent(i));
        }
        
        std::cout << "Extracted TH1: " << hist->GetName() << " (" << nBins << " bins)" << std::endl;
        return true;
    }
    
    // Helper function to extract data from TH2
    static bool ExtractFromTH2(TH2* hist, ColumnData& data) {
        if (!hist) return false;
        
        int nBinsX = hist->GetNbinsX();
        int nBinsY = hist->GetNbinsY();
        
        // Create three columns: X bin center, Y bin center, and bin content
        data.headers.push_back(Form("%s_X", hist->GetName()));
        data.headers.push_back(Form("%s_Y", hist->GetName()));
        data.headers.push_back(Form("%s_Content", hist->GetName()));
        data.data.resize(3);
        
        for (int i = 1; i <= nBinsX; ++i) {
            for (int j = 1; j <= nBinsY; ++j) {
                data.data[0].push_back(hist->GetXaxis()->GetBinCenter(i));
                data.data[1].push_back(hist->GetYaxis()->GetBinCenter(j));
                data.data[2].push_back(hist->GetBinContent(i, j));
            }
        }
        
        std::cout << "Extracted TH2: " << hist->GetName() 
                  << " (" << nBinsX << "x" << nBinsY << " bins)" << std::endl;
        return true;
    }
    
    // Helper function to extract data from TH3
    static bool ExtractFromTH3(TH3* hist, ColumnData& data) {
        if (!hist) return false;
        
        int nBinsX = hist->GetNbinsX();
        int nBinsY = hist->GetNbinsY();
        int nBinsZ = hist->GetNbinsZ();
        
        // Create four columns: X, Y, Z bin centers, and bin content
        data.headers.push_back(Form("%s_X", hist->GetName()));
        data.headers.push_back(Form("%s_Y", hist->GetName()));
        data.headers.push_back(Form("%s_Z", hist->GetName()));
        data.headers.push_back(Form("%s_Content", hist->GetName()));
        data.data.resize(4);
        
        for (int i = 1; i <= nBinsX; ++i) {
            for (int j = 1; j <= nBinsY; ++j) {
                for (int k = 1; k <= nBinsZ; ++k) {
                    data.data[0].push_back(hist->GetXaxis()->GetBinCenter(i));
                    data.data[1].push_back(hist->GetYaxis()->GetBinCenter(j));
                    data.data[2].push_back(hist->GetZaxis()->GetBinCenter(k));
                    data.data[3].push_back(hist->GetBinContent(i, j, k));
                }
            }
        }
        
        std::cout << "Extracted TH3: " << hist->GetName() 
                  << " (" << nBinsX << "x" << nBinsY << "x" << nBinsZ << " bins)" << std::endl;
        return true;
    }
    
    // Recursive helper to search through lists and pads
    static TObject* FindHistogramInList(TList* list, int depth = 0) {
        if (!list || depth > 10) return nullptr; // Prevent infinite recursion
        
        TIter next(list);
        TObject* obj;
        
        while ((obj = next())) {
            if (!obj) continue;
            
            std::cout << std::string(depth * 2, ' ') << "  -> " << obj->GetName() 
                      << " (class: " << obj->ClassName() << ")" << std::endl;
            
            // Check for histograms (most specific first)
            if (obj->InheritsFrom(TH3::Class())) {
                std::cout << "Found TH3 histogram!" << std::endl;
                return obj;
            } else if (obj->InheritsFrom(TH2::Class())) {
                std::cout << "Found TH2 histogram!" << std::endl;
                return obj;
            } else if (obj->InheritsFrom(TH1::Class())) {
                std::cout << "Found TH1 histogram!" << std::endl;
                return obj;
            }
            // Check if this is a pad (sub-canvas)
            else if (obj->InheritsFrom(TPad::Class())) {
                TPad* pad = (TPad*)obj;
                TList* padPrimitives = pad->GetListOfPrimitives();
                if (padPrimitives) {
                    std::cout << std::string(depth * 2, ' ') << "Searching in pad: " 
                              << pad->GetName() << std::endl;
                    TObject* found = FindHistogramInList(padPrimitives, depth + 1);
                    if (found) return found;
                }
            }
            // Check if this is another TList
            else if (obj->InheritsFrom(TList::Class())) {
                TList* sublist = (TList*)obj;
                std::cout << std::string(depth * 2, ' ') << "Searching in TList (size: " 
                          << sublist->GetSize() << ")" << std::endl;
                TObject* found = FindHistogramInList(sublist, depth + 1);
                if (found) return found;
            }
        }
        
        return nullptr;
    }
    
    // Helper function to extract histograms from a canvas
    static bool ExtractFromCanvas(TCanvas* canvas, ColumnData& data) {
        if (!canvas) return false;
        
        std::cout << "Analyzing canvas: " << canvas->GetName() << std::endl;
        
        TList* primitives = canvas->GetListOfPrimitives();
        if (!primitives) {
            std::cerr << "Canvas " << canvas->GetName() << " has no primitives list" << std::endl;
            return false;
        }
        
        std::cout << "Canvas primitives list size: " << primitives->GetSize() << std::endl;
        
        // Recursively search for histograms
        TObject* foundObj = FindHistogramInList(primitives);
        
        if (!foundObj) {
            std::cerr << "No histograms found in canvas " << canvas->GetName() << std::endl;
            return false;
        }
        
        // Extract data based on histogram type
        if (foundObj->InheritsFrom(TH3::Class())) {
            return ExtractFromTH3((TH3*)foundObj, data);
        } else if (foundObj->InheritsFrom(TH2::Class())) {
            return ExtractFromTH2((TH2*)foundObj, data);
        } else if (foundObj->InheritsFrom(TH1::Class())) {
            return ExtractFromTH1((TH1*)foundObj, data);
        }
        
        return false;
    }
    
    // Read ROOT file - enhanced to handle histograms and canvases
    static bool ReadROOTFile(const std::string& filename, ColumnData& data) {
        TFile* file = TFile::Open(filename.c_str(), "READ");
        if (!file || file->IsZombie()) {
            std::cerr << "Cannot open ROOT file: " << filename << std::endl;
            return false;
        }
        
        data.filename = filename;
        bool success = false;
        
        // Iterate through all keys in the file
        TIter next(file->GetListOfKeys());
        TKey* key;
        
        std::cout << "=== Scanning ROOT file for objects ===" << std::endl;
        
        while ((key = (TKey*)next())) {
            TObject* obj = key->ReadObj();
            if (!obj) continue;
            
            std::cout << "Found: " << obj->GetName() 
                      << " (class: " << obj->ClassName() << ")" << std::endl;
            
            // Check for TH3 first (most specific)
            if (obj->InheritsFrom(TH3::Class())) {
                success = ExtractFromTH3((TH3*)obj, data);
                if (success) break;
            }
            // Then TH2
            else if (obj->InheritsFrom(TH2::Class())) {
                success = ExtractFromTH2((TH2*)obj, data);
                if (success) break;
            }
            // Then TH1
            else if (obj->InheritsFrom(TH1::Class())) {
                success = ExtractFromTH1((TH1*)obj, data);
                if (success) break;
            }
            // Check for Canvas
            else if (obj->InheritsFrom(TCanvas::Class())) {
                success = ExtractFromCanvas((TCanvas*)obj, data);
                if (success) break;
            }
            // Finally check for TTree
            else if (obj->InheritsFrom(TTree::Class())) {
                success = ReadROOTTree((TTree*)obj, data);
                if (success) break;
            }
        }
        
        if (!success) {
            std::cerr << "No compatible objects (TH1/TH2/TH3/TCanvas/TTree) found in ROOT file" << std::endl;
        }
        
        file->Close();
        return success;
    }
    
    // Helper function to read TTree (extracted from original ReadROOTFile)
    static bool ReadROOTTree(TTree* tree, ColumnData& data) {
        if (!tree) return false;
        
        // Get branches
        TObjArray* branches = tree->GetListOfBranches();
        int nBranches = branches->GetEntries();
        
        if (nBranches == 0) {
            std::cerr << "TTree has no branches" << std::endl;
            return false;
        }
        
        data.headers.resize(nBranches);
        data.data.resize(nBranches);
        
        std::vector<Double_t> branchValues(nBranches);
        
        for (int i = 0; i < nBranches; ++i) {
            TBranch* branch = (TBranch*)branches->At(i);
            data.headers[i] = branch->GetName();
            tree->SetBranchAddress(branch->GetName(), &branchValues[i]);
        }
        
        // Read all entries
        Long64_t nEntries = tree->GetEntries();
        for (Long64_t entry = 0; entry < nEntries; ++entry) {
            tree->GetEntry(entry);
            for (int i = 0; i < nBranches; ++i) {
                data.data[i].push_back(branchValues[i]);
            }
        }
        
        std::cout << "Extracted TTree: " << tree->GetName() 
                  << " (" << nBranches << " branches, " << nEntries << " entries)" << std::endl;
        return true;
    }
    
    // Main read function - auto-detects file type
    static bool ReadFile(const std::string& filename, ColumnData& data) {
        FileType type = GetFileType(filename);
        
        switch (type) {
            case kCSV:
                return ReadCSVFile(filename, data);
            case kROOT:
                return ReadROOTFile(filename, data);
            case kText:
            default:
                return ReadTextFile(filename, data);
        }
    }
};

#endif // DATAREADER_H