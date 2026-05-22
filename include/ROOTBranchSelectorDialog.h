#ifndef ROOTBRANCHSELECTORDIALOG_H
#define ROOTBRANCHSELECTORDIALOG_H

// ============================================================================
// ROOTBranchSelectorDialog
//
// A modal dialog that lets the user pick objects from an open ROOT file and
// loads them into a ColumnData struct so the main GUI "Add Plot" workflow
// works normally with ROOT files (TTree branches AND histograms).
//
// Usage:
//   ROOTBranchSelectorDialog dlg(parent, "/path/to/file.root");
//   if (dlg.DoModal() == 1) {
//       ColumnData data = dlg.GetColumnData();
//       // data is now filled — pass to PlotManager::AddPlot()
//   }
// ============================================================================

#include <TGFrame.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGListBox.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TFile.h>
#include <TTree.h>
#include <TKey.h>
#include <TGNumberEntry.h>

#include "DataReader.h"   // ColumnData

#include <string>
#include <vector>

class ROOTBranchSelectorDialog : public TGTransientFrame {
public:
    ROOTBranchSelectorDialog(const TGWindow* parent, const char* filepath);
    virtual ~ROOTBranchSelectorDialog();

    // Run modal; returns 1 = OK, 0 = Cancel
    Int_t DoModal();

    // After DoModal() == 1, retrieve the populated ColumnData
    const ColumnData& GetColumnData() const { return fColumnData; }

    // TGMainFrame overrides
    void CloseWindow() override;
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) override;

    ClassDefOverride(ROOTBranchSelectorDialog, 0)

private:
    // ── data ──────────────────────────────────────────────────────────────
    TString                  fFilepath;
    TFile*                   fFile          {nullptr};
    TTree*                   fCurrentTree   {nullptr};
    ColumnData               fColumnData;
    Int_t                    fModalResult   {-1};

    // Names of top-level objects found in the file
    struct ObjEntry { std::string name; std::string cls; };
    std::vector<ObjEntry>    fObjects;       // all loadable objects

    // ── GUI widgets ────────────────────────────────────────────────────────
    // Object list (TH1*, TH2*, TH3*, TTree, …)
    TGLabel*     fFileLabel          {nullptr};
    TGListBox*   fObjectListBox      {nullptr};   // top-level objects
    TGLabel*     fObjectInfoLabel    {nullptr};

    // Branch panel (only visible for TTree)
    TGGroupFrame*  fBranchGroup      {nullptr};
    TGListBox*     fBranchListBox    {nullptr};   // multi-select branches
    TGLabel*       fBranchHint       {nullptr};
    TGTextButton*  fSelectAllBranches{nullptr};
    TGTextButton*  fClearBranches    {nullptr};

    // Entry-range for TTree
    TGGroupFrame*  fRangeGroup       {nullptr};
    TGNumberEntry* fMaxEntriesEntry  {nullptr};
    TGLabel*       fEntriesInfoLabel {nullptr};

    // Buttons
    TGTextButton*  fOkButton         {nullptr};
    TGTextButton*  fCancelButton     {nullptr};

    // ── widget IDs ─────────────────────────────────────────────────────────
    enum EWidgetIDs {
        kObjectListBox  = 3000,
        kBranchListBox,
        kSelectAllBtn,
        kClearBtn,
        kOkButton,
        kCancelButton
    };

    // ── helpers ────────────────────────────────────────────────────────────
    void BuildGUI();
    void ScanFile();
    void OnObjectSelected(Int_t id);
    void PopulateBranches();
    bool LoadSelectedData();         // fills fColumnData; returns false on failure
    bool LoadHistogram(const std::string& name, const std::string& cls);
    bool LoadTreeBranches(const std::string& treeName,
                          const std::vector<std::string>& branches,
                          Long64_t maxEntries);
    std::vector<std::string> GetSelectedBranches() const;
};

#endif // ROOTBRANCHSELECTORDIALOG_H
