#ifndef DATAREADER_H
#define DATAREADER_H

#include <TFile.h>
#include <TTree.h>
#include <TKey.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
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
    
    // Read ROOT file - for now, just indicate it's supported
    // User will need to select TTree and branches
    static bool ReadROOTFile(const std::string& filename, ColumnData& data) {
        TFile* file = TFile::Open(filename.c_str(), "READ");
        if (!file || file->IsZombie()) {
            std::cerr << "Cannot open ROOT file: " << filename << std::endl;
            return false;
        }
        
        data.filename = filename;
        
        // Get first TTree in file
        TTree* tree = nullptr;
        TIter next(file->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)next())) {
            TObject* obj = key->ReadObj();
            if (obj->InheritsFrom("TTree")) {
                tree = (TTree*)obj;
                break;
            }
        }
        
        if (!tree) {
            std::cerr << "No TTree found in ROOT file" << std::endl;
            file->Close();
            return false;
        }
        
        // Get branches
        TObjArray* branches = tree->GetListOfBranches();
        int nBranches = branches->GetEntries();
        
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
        
        file->Close();
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
