#ifndef COLUMNSELECTORDIALOG_H
#define COLUMNSELECTORDIALOG_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <RQ_OBJECT.h>

#include "DataReader.h"
#include "PlotTypes.h"

// ============================================================================
// ColumnSelectorDialog
// All private member names exactly match ColumnSelector.cpp
// FIXED: Changed TGRadioButton to TGCheckButton to match implementation
// ============================================================================
class ColumnSelectorDialog : public TGTransientFrame {
    //RQ_OBJECT("ColumnSelectorDialog")
    ClassDefOverride(ColumnSelectorDialog, 0)

public:
    // Signature matches ColumnSelector.cpp line 7
    ColumnSelectorDialog(const TGWindow* parent,
                         const ColumnData* columnData,
                         PlotConfig*       plotConfig,
                         bool*             result);
    virtual ~ColumnSelectorDialog() {}

    Int_t DoModal() {
        MapWindow();
        gClient->WaitFor(this);
        return (*dialogResult) ? 1 : 0;
    }

    void CloseWindow() override { UnmapWindow(); }
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) override;

    ///// Problem part
    //virtual void CloseWindow() { UnmapWindow(); }
    //Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
     
    void UpdateColumnVisibility();

    // ClassDef 0 avoids -Winconsistent-missing-override with ROOT 6.26
    

private:
    // Out-parameters — owned by the caller (ColumnSelector wrapper)
    const ColumnData* data;
    PlotConfig*       config;
    bool*             dialogResult;

    // FIXED: Plot-type checkboxes (not radio buttons - we handle mutual exclusion manually)
    TGCheckButton* radioTGraph;
    TGCheckButton* radioTGraphErrors;
    TGCheckButton* radioTH1D;
    TGCheckButton* radioTH1F;
    TGCheckButton* radioTH1I;
    TGCheckButton* radioTH2D;
    TGCheckButton* radioTH2F;
    TGCheckButton* radioTH2I;
    TGCheckButton* radioTH3D;
    TGCheckButton* radioTH3F;
    TGCheckButton* radioTH3I;

    // Column selector combos
    TGComboBox* xColumnCombo;
    TGComboBox* yColumnCombo;
    TGComboBox* zColumnCombo;
    TGComboBox* xErrCombo;
    TGComboBox* yErrCombo;

    // Buttons
    TGTextButton* okButton;
    TGTextButton* cancelButton;

    // Private helpers — names match cpp exactly
    void PopulateComboBox(TGComboBox* combo, int startIdx);
    void DoOK();
    void DoCancel();
};

// ============================================================================
// ColumnSelector — thin RAII wrapper used by PlotManager::AddPlot()
// ============================================================================
class ColumnSelector {
private:
    PlotConfig fConfig;
    bool       fAccepted = false;
    const TGWindow* fParent;
    const ColumnData& fData;

public:
    ColumnSelector(const TGWindow* p, const ColumnData& data) 
        : fParent(p), fData(data) {}

    // ADD THIS METHOD
    Int_t DoModal() {
        // Create the dialog and run it modally
        ColumnSelectorDialog* diag = new ColumnSelectorDialog(fParent, &fData, &fConfig, &fAccepted);
        Int_t ret = diag->DoModal();
        //delete diag; // Clean up the dialog window memory
        return ret;
    }

    PlotConfig* GetPlotConfig() {
        if (!fAccepted) return nullptr;
        return new PlotConfig(fConfig);
    }
};
#endif // COLUMNSELECTOR_H