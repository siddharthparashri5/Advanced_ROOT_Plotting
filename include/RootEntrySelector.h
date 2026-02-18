#ifndef ROOKENTRYSELECTOR_H
#define ROOKENTRYSELECTOR_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGComboBox.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGListBox.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>

#include <vector>
#include <string>
#include <cstdio>

// ============================================================================
// Structure to hold a selection step
// ============================================================================
struct SelectionStep {
    std::string objectName;      // Name of histogram/branch
    std::string objectType;      // "TH1D", "TTree", "TGraph", etc.
    Long64_t    entryStart;      // First entry to plot
    Long64_t    entryEnd;        // Last entry to plot
    std::string cutFormula;      // ROOT TCut-style formula
    std::string drawOptions;     // Draw options like "COLZ", "PE", etc.
    
    SelectionStep() 
        : entryStart(0), entryEnd(-1) {}
    
    std::string GetDescription() const {
        std::string desc = objectName;
        if (entryEnd > 0) {
            char buf[128];
            snprintf(buf, sizeof(buf), " [entries %lld-%lld]", entryStart, entryEnd);
            desc += std::string(buf);
        }
        if (!cutFormula.empty()) {
            desc += " | Cut: " + cutFormula;
        }
        return desc;
    }
};

// ============================================================================
// RootEntrySelector - Main dialog for ROOT file entry selection
// ============================================================================
class RootEntrySelector : public TGTransientFrame {
private:
    // Widget IDs
    enum EWidgetIDs {
        kObjectCombo = 2000,
        kBranchCombo,
        kStartEntry,
        kEndEntry,
        kCutEntry,
        kDrawOptEntry,
        kAddStepButton,
        kRemoveStepButton,
        kClearStepsButton,
        kPlotCurrentButton,
        kPlotChainButton,
        kSaveChainButton,
        kLoadChainButton,
        kCloseButton
    };
    
    // File and data
    TFile*                    fFile;
    TString                   fFilename;
    TTree*                    fCurrentTree;
    TH1*                      fCurrentHist;
    std::vector<std::string>  fObjectList;
    std::vector<SelectionStep> fSelectionChain;
    
    // GUI Components
    TGComboBox*      fObjectCombo;
    TGComboBox*      fBranchCombo;
    TGNumberEntry*   fStartEntry;
    TGNumberEntry*   fEndEntry;
    TGTextEntry*     fCutEntry;
    TGTextEntry*     fDrawOptEntry;
    TGLabel*         fObjectInfoLabel;
    TGLabel*         fEntriesLabel;
    TGListBox*       fStepListBox;
    
    TGTextButton*    fAddStepButton;
    TGTextButton*    fRemoveStepButton;
    TGTextButton*    fClearStepsButton;
    TGTextButton*    fPlotCurrentButton;
    TGTextButton*    fPlotChainButton;
    TGTextButton*    fSaveChainButton;
    TGTextButton*    fLoadChainButton;
    TGTextButton*    fCloseButton;
    
    // Helper methods
    void BuildGUI();
    void ScanFile();
    void PopulateBranches();
    void UpdateObjectInfo();
    void AddSelectionStep();
    void RemoveSelectedStep();
    void ClearAllSteps();
    void PlotCurrentSelection();
    void PlotEntireChain();
    void SaveChainToFile();
    void LoadChainFromFile();
    
    // Plotting helpers
    TCanvas* PlotHistogram(const SelectionStep& step);
    TCanvas* PlotTree(const SelectionStep& step);
    TCanvas* PlotWithChain(const std::vector<SelectionStep>& chain);
    std::string BuildCumulativeCut() const;
    
public:
    RootEntrySelector(const TGWindow* p, const char* filename);
    virtual ~RootEntrySelector();
    
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) override;
    void CloseWindow() override;
    
    //ClassDef(RootEntrySelector, 0)
};

#endif // ROOKENTRYSELECTOR_H