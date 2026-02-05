#ifndef COLUMNSELECTOR_H
#define COLUMNSELECTOR_H

#include <TGClient.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGTextEntry.h>
#include <TSystem.h>
#include "DataReader.h"
#include "PlotTypes.h"


//////////////////////////////
// Column Selector Dialog
//////////////////////////////
class ColumnSelectorDialog : public TGTransientFrame {
private:
    const ColumnData* data;
    PlotConfig* config;

    TGComboBox* xColumnCombo;
    TGComboBox* yColumnCombo;
    TGComboBox* zColumnCombo;
    TGComboBox* xErrCombo;
    TGComboBox* yErrCombo;

    TGCheckButton* radioNone;
    TGCheckButton* radioTGraph;
    TGCheckButton* radioTGraphErrors;
    TGCheckButton* radioTH1D;
    TGCheckButton* radioTH2D;
    TGCheckButton* radioTH3D;

    TGTextButton* okButton;
    TGTextButton* cancelButton;

    bool* dialogResult;

public:
    // Constructor
    ColumnSelectorDialog(const TGWindow* parent,
                         const ColumnData* columnData,
                         PlotConfig* plotConfig,
                         bool* result);

    // Slot to update combo box enable/disable depending on plot type
    void UpdateColumnVisibility();

    // Other helper functions
    void DoOK();
    void DoCancel();
    void PopulateComboBox(TGComboBox* combo, int startIdx = 0);

    // Process messages from buttons
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) override;

    ClassDef(ColumnSelectorDialog, 1) 
};

#endif // COLUMNSELECTOR_H

