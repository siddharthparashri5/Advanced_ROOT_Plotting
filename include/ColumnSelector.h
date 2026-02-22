#ifndef COLUMNSELECTOR_H
#define COLUMNSELECTOR_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>

#include "DataReader.h"
#include "PlotTypes.h"

// ============================================================================
// ColumnSelectorDialog
// All private member names exactly match ColumnSelector.cpp
// FIXED: Changed TGRadioButton to TGCheckButton to match implementation
// ============================================================================
class ColumnSelectorDialog : public TGTransientFrame {


public:
    // Signature matches ColumnSelector.cpp line 7
    ColumnSelectorDialog(const TGWindow* parent,
                         const ColumnData* columnData,
                         PlotConfig*       plotConfig,
                         bool*             result);
    virtual ~ColumnSelectorDialog() {}

    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

    // ClassDef 0 avoids -Winconsistent-missing-override with ROOT 6.26
    ClassDefOverride(ColumnSelectorDialog, 0)

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
    void UpdateColumnVisibility();
    

};

// ============================================================================
// ColumnSelector — thin RAII wrapper used by PlotManager::AddPlot()
// ============================================================================
class ColumnSelector {
private:
    PlotConfig fConfig;
    bool       fAccepted = false;

public:
    ColumnSelector(const TGWindow* p, const ColumnData& data)
    {
        new ColumnSelectorDialog(p, &data, &fConfig, &fAccepted);
        // Dialog runs modally; fConfig and fAccepted are filled by DoOK()
    }

    // Returns heap-allocated copy when accepted, nullptr when cancelled.
    // Caller takes ownership.
    PlotConfig* GetPlotConfig()
    {
        if (!fAccepted) return nullptr;
        return new PlotConfig(fConfig);
    }
};

#endif // COLUMNSELECTOR_H