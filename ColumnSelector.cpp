#include "ColumnSelector.h"

// ROOT dictionary macro
ClassImp(ColumnSelectorDialog)

// Constructor implementation
ColumnSelectorDialog::ColumnSelectorDialog(const TGWindow* parent,
                                           const ColumnData* columnData,
                                           PlotConfig* plotConfig,
                                           bool* result)
: TGTransientFrame(gClient->GetRoot(), parent, 600, 400),
  data(columnData),
  config(plotConfig),
  dialogResult(result)
{
    *dialogResult = false;
	SetCleanup(kDeepCleanup);
	
	TGVerticalFrame* mainFrame = new TGVerticalFrame(this);
	
	// Title
	TGLabel* titleLabel = new TGLabel(mainFrame,
    Form("Configure Plot - %d columns available", data->GetNumColumns()));
    mainFrame->AddFrame(titleLabel, new TGLayoutHints(kLHintsCenterX,5,5,10,10));

    // ===========================
    // Plot type selection
    // ===========================
    TGGroupFrame* typeGroup = new TGGroupFrame(mainFrame, "Plot Type");
        	// Create radio buttons
    radioNone = new TGCheckButton(typeGroup, "None", 0);
    radioTGraph = new TGCheckButton(typeGroup, "TGraph (x vs y)", 1);
	radioTGraphErrors = new TGCheckButton(typeGroup, "TGraphErrors (x vs y with errors)", 2);
	radioTH1D = new TGCheckButton(typeGroup, "TH1D (histogram of x)", 3);
	radioTH2D = new TGCheckButton(typeGroup, "TH2D (2D histogram x vs y)", 4);
    radioTH3D = new TGCheckButton(typeGroup, "TH3D (3D histogram x vs y vs z)", 5);

    // Add to type group layout
    typeGroup->AddFrame(radioNone, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(radioTGraph, new TGLayoutHints(kLHintsLeft,5,5,2,2));
	typeGroup->AddFrame(radioTGraphErrors, new TGLayoutHints(kLHintsLeft,5,5,2,2));
	typeGroup->AddFrame(radioTH1D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
	typeGroup->AddFrame(radioTH2D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(radioTH3D, new TGLayoutHints(kLHintsLeft,5,5,2,2));

	// Default selection
	//radioNone->SetOn();

	// Connect each radio button to UpdateColumnVisibility
	radioTGraph->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
	radioTGraphErrors->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
	radioTH1D->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
	radioTH2D->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH3D->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
	radioNone->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");


	mainFrame->AddFrame(typeGroup, new TGLayoutHints(kLHintsExpandX,5,5,5,5));

	//////////////////////////////
	// Column selection
	//////////////////////////////
	TGGroupFrame* colGroup = new TGGroupFrame(mainFrame, "Column Selection");
	TGVerticalFrame* colFrame = new TGVerticalFrame(colGroup);
	
	// X column
	TGHorizontalFrame* xFrame = new TGHorizontalFrame(colFrame);
	TGLabel* xLabel = new TGLabel(xFrame, "X Column:");
	xLabel->SetWidth(100);
	xFrame->AddFrame(xLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,2,2));
    xColumnCombo = new TGComboBox(xFrame);
    for(int i=0; i<data->GetNumColumns(); ++i) xColumnCombo->AddEntry(data->headers[i].c_str(), i);
    xColumnCombo->Select(0);
    xColumnCombo->Resize(200,20);
    xFrame->AddFrame(xColumnCombo, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    colFrame->AddFrame(xFrame, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    // Y column
    TGHorizontalFrame* yFrame = new TGHorizontalFrame(colFrame);
    TGLabel* yLabel = new TGLabel(yFrame, "Y Column:");
    yLabel->SetWidth(100);
    yFrame->AddFrame(yLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,2,2));
    yColumnCombo = new TGComboBox(yFrame);
    for(int i=0; i<data->GetNumColumns(); ++i) yColumnCombo->AddEntry(data->headers[i].c_str(), i);
    yColumnCombo->Select(1);
    yColumnCombo->Resize(200,20);
    yFrame->AddFrame(yColumnCombo, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    colFrame->AddFrame(yFrame, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    // Z column
    TGHorizontalFrame* zFrame = new TGHorizontalFrame(colFrame);
    TGLabel* zLabel = new TGLabel(zFrame, "Z Column:");
    zLabel->SetWidth(100);
    zFrame->AddFrame(zLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,2,2));
    zColumnCombo = new TGComboBox(zFrame);
    for(int i=0; i<data->GetNumColumns(); ++i) zColumnCombo->AddEntry(data->headers[i].c_str(), i);
    zColumnCombo->Select(1);
    zColumnCombo->Resize(200,20);
    zFrame->AddFrame(zColumnCombo, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    colFrame->AddFrame(zFrame, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    // X error column
    TGHorizontalFrame* xErrFrame = new TGHorizontalFrame(colFrame);
    TGLabel* xErrLabel = new TGLabel(xErrFrame, "X Error:");
    xErrLabel->SetWidth(100);
    xErrFrame->AddFrame(xErrLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,2,2));
    xErrCombo = new TGComboBox(xErrFrame);
    xErrCombo->AddEntry("None", -1);
    for(int i=1;i<data->GetNumColumns();++i) xErrCombo->AddEntry(data->headers[i].c_str(), i);
    xErrCombo->Select(-1);
    xErrCombo->Resize(200,20);
    xErrFrame->AddFrame(xErrCombo, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    colFrame->AddFrame(xErrFrame, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    // Y error column
    TGHorizontalFrame* yErrFrame = new TGHorizontalFrame(colFrame);
    TGLabel* yErrLabel = new TGLabel(yErrFrame, "Y Error:");
    yErrLabel->SetWidth(100);
    yErrFrame->AddFrame(yErrLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,2,2));
    yErrCombo = new TGComboBox(yErrFrame);
    yErrCombo->AddEntry("None", -1);
    for(int i=1;i<data->GetNumColumns();++i) yErrCombo->AddEntry(data->headers[i].c_str(), i);
    yErrCombo->Select(-1);
    yErrCombo->Resize(200,20);
    yErrFrame->AddFrame(yErrCombo, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    colFrame->AddFrame(yErrFrame, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    colGroup->AddFrame(colFrame, new TGLayoutHints(kLHintsExpandX,5,5,5,5));
    mainFrame->AddFrame(colGroup, new TGLayoutHints(kLHintsExpandX,5,5,5,5));

    //////////////////////////////
    // Buttons
    //////////////////////////////
    TGHorizontalFrame* buttonFrame = new TGHorizontalFrame(mainFrame);
    okButton = new TGTextButton(buttonFrame, "OK", 1);
    okButton->Associate(this);
    cancelButton = new TGTextButton(buttonFrame, "Cancel", 2);
    cancelButton->Associate(this);
    buttonFrame->AddFrame(okButton, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
    buttonFrame->AddFrame(cancelButton, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
    mainFrame->AddFrame(buttonFrame, new TGLayoutHints(kLHintsCenterX,5,5,10,5));

    AddFrame(mainFrame, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));

    SetWindowName("Column Selector");
    MapSubwindows();
    Resize(GetDefaultSize());
    CenterOnParent();

    // Make sure dropdowns are updated correctly at start
    UpdateColumnVisibility();

    MapWindow();
}

    
    void ColumnSelectorDialog::PopulateComboBox(TGComboBox* combo, int startIdx) {
    for (int i = 0; i < data->GetNumColumns(); ++i) {
        combo->AddEntry(data->headers[i].c_str(), i);
    }
}

void ColumnSelectorDialog::DoOK() {
    config->xColumn = xColumnCombo->GetSelected();
    config->yColumn = yColumnCombo->GetSelected();
    config->zColumn = zColumnCombo->GetSelected();
    config->xErrColumn = xErrCombo->GetSelected();
    config->yErrColumn = yErrCombo->GetSelected();

    if (radioTGraph->IsOn()) config->type = PlotConfig::kTGraph;
    else if (radioTGraphErrors->IsOn()) config->type = PlotConfig::kTGraphErrors;
    else if (radioTH1D->IsOn()) config->type = PlotConfig::kTH1D;
    else if (radioTH2D->IsOn()) config->type = PlotConfig::kTH2D;
    else if (radioTH3D->IsOn()) config->type = PlotConfig::kTH3D;

    if (config->xColumn >= 0 && config->xColumn < (int)data->headers.size())
        config->xTitle = data->headers[config->xColumn];
    if (config->yColumn >= 0 && config->yColumn < (int)data->headers.size())
        config->yTitle = data->headers[config->yColumn];
    if (config->zColumn >= 0 && config->zColumn < (int)data->headers.size())
        config->zTitle = data->headers[config->zColumn];

    *dialogResult = true;
    DeleteWindow();
}

void ColumnSelectorDialog::DoCancel() {
    *dialogResult = false;
    DeleteWindow();
}

void ColumnSelectorDialog::UpdateColumnVisibility() {
    if (radioTGraphErrors->IsOn()) {
        xErrCombo->SetEnabled(true);
        yErrCombo->SetEnabled(true);
    } else {
        xErrCombo->SetEnabled(false);
        yErrCombo->SetEnabled(false);
    }

    xColumnCombo->SetEnabled(radioTGraph->IsOn() || radioTGraphErrors->IsOn() ||
                             radioTH1D->IsOn() || radioTH2D->IsOn() || radioTH3D->IsOn());

    yColumnCombo->SetEnabled(radioTGraph->IsOn() || radioTGraphErrors->IsOn() || radioTH2D->IsOn() || radioTH3D->IsOn());

    zColumnCombo->SetEnabled(radioTH3D->IsOn());

    if (radioNone->IsOn()) {
        xColumnCombo->SetEnabled(false);
        yColumnCombo->SetEnabled(false);
        xErrCombo->SetEnabled(false);
        yErrCombo->SetEnabled(false);

        radioTGraph->SetOn(false);
        radioTGraphErrors->SetOn(false);
        radioTH1D->SetOn(false);
        radioTH2D->SetOn(false);
        radioTH3D->SetOn(false);
    } else {
        if (radioTGraph->IsOn() || radioTGraphErrors->IsOn() || radioTH1D->IsOn() || radioTH2D->IsOn() || radioTH3D->IsOn())
            radioNone->SetOn(false);
    }
}

Bool_t ColumnSelectorDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) {
    switch (GET_MSG(msg)) {
        case kC_COMMAND:
            switch (GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == 1) DoOK();
                    else if (parm1 == 2) DoCancel();
                    break;
            }
            break;
    }
    return kTRUE;
}
