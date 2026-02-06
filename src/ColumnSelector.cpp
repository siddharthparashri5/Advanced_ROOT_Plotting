#include "ColumnSelector.h"

// ROOT dictionary macro
ClassImp(ColumnSelectorDialog)

// Constructor implementation
ColumnSelectorDialog::ColumnSelectorDialog(const TGWindow* parent,
                                           const ColumnData* columnData,
                                           PlotConfig* plotConfig,
                                           bool* result)
: TGTransientFrame(gClient->GetRoot(), parent, 600, 500),
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
    
    // Graph types
    TGGroupFrame* graphGroup = new TGGroupFrame(typeGroup, "Graphs");
    radioTGraph = new TGCheckButton(graphGroup, "TGraph (x vs y)", 1);
    radioTGraphErrors = new TGCheckButton(graphGroup, "TGraphErrors (x vs y with errors)", 2);
    graphGroup->AddFrame(radioTGraph, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    graphGroup->AddFrame(radioTGraphErrors, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(graphGroup, new TGLayoutHints(kLHintsExpandX,5,5,2,2));
    
    // 1D Histogram types
    TGGroupFrame* h1Group = new TGGroupFrame(typeGroup, "1D Histograms");
    radioTH1D = new TGCheckButton(h1Group, "TH1D (double precision)", 3);
    radioTH1F = new TGCheckButton(h1Group, "TH1F (float precision)", 4);
    radioTH1I = new TGCheckButton(h1Group, "TH1I (integer bins)", 5);
    h1Group->AddFrame(radioTH1D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h1Group->AddFrame(radioTH1F, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h1Group->AddFrame(radioTH1I, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(h1Group, new TGLayoutHints(kLHintsExpandX,5,5,2,2));
    
    // 2D Histogram types
    TGGroupFrame* h2Group = new TGGroupFrame(typeGroup, "2D Histograms");
    radioTH2D = new TGCheckButton(h2Group, "TH2D (double precision)", 6);
    radioTH2F = new TGCheckButton(h2Group, "TH2F (float precision)", 7);
    radioTH2I = new TGCheckButton(h2Group, "TH2I (integer bins)", 8);
    h2Group->AddFrame(radioTH2D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h2Group->AddFrame(radioTH2F, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h2Group->AddFrame(radioTH2I, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(h2Group, new TGLayoutHints(kLHintsExpandX,5,5,2,2));
    
    // 3D Histogram types
    TGGroupFrame* h3Group = new TGGroupFrame(typeGroup, "3D Histograms");
    radioTH3D = new TGCheckButton(h3Group, "TH3D (double precision)", 9);
    radioTH3F = new TGCheckButton(h3Group, "TH3F (float precision)", 10);
    radioTH3I = new TGCheckButton(h3Group, "TH3I (integer bins)", 11);
    h3Group->AddFrame(radioTH3D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h3Group->AddFrame(radioTH3F, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h3Group->AddFrame(radioTH3I, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(h3Group, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    // Default selection
    radioTGraph->SetOn();

    // Connect each radio button to UpdateColumnVisibility
    radioTGraph->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTGraphErrors->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH1D->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH1F->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH1I->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH2D->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH2F->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH2I->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH3D->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH3F->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    radioTH3I->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");

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
    zColumnCombo->Select(2 < data->GetNumColumns() ? 2 : 1);
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
    for(int i=0;i<data->GetNumColumns();++i) xErrCombo->AddEntry(data->headers[i].c_str(), i);
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
    for(int i=0;i<data->GetNumColumns();++i) yErrCombo->AddEntry(data->headers[i].c_str(), i);
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

    // Determine plot type
    if (radioTGraph->IsOn()) config->type = PlotConfig::kTGraph;
    else if (radioTGraphErrors->IsOn()) config->type = PlotConfig::kTGraphErrors;
    else if (radioTH1D->IsOn()) config->type = PlotConfig::kTH1D;
    else if (radioTH1F->IsOn()) config->type = PlotConfig::kTH1F;
    else if (radioTH1I->IsOn()) config->type = PlotConfig::kTH1I;
    else if (radioTH2D->IsOn()) config->type = PlotConfig::kTH2D;
    else if (radioTH2F->IsOn()) config->type = PlotConfig::kTH2F;
    else if (radioTH2I->IsOn()) config->type = PlotConfig::kTH2I;
    else if (radioTH3D->IsOn()) config->type = PlotConfig::kTH3D;
    else if (radioTH3F->IsOn()) config->type = PlotConfig::kTH3F;
    else if (radioTH3I->IsOn()) config->type = PlotConfig::kTH3I;

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

void ColumnSelectorDialog::UpdateColumnVisibility()
{
    // ---- Identify active plot type ----
    bool graph   = radioTGraph->IsOn();
    bool graphE  = radioTGraphErrors->IsOn();
    bool h1d     = radioTH1D->IsOn();
    bool h1f     = radioTH1F->IsOn();
    bool h1i     = radioTH1I->IsOn();
    bool h2d     = radioTH2D->IsOn();
    bool h2f     = radioTH2F->IsOn();
    bool h2i     = radioTH2I->IsOn();
    bool h3d     = radioTH3D->IsOn();
    bool h3f     = radioTH3F->IsOn();
    bool h3i     = radioTH3I->IsOn();

    bool h1 = (h1d || h1f || h1i);
    bool h2 = (h2d || h2f || h2i);
    bool h3 = (h3d || h3f || h3i);

    // ---- Enforce single selection ----
    if(graph) {
        radioTGraphErrors->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
    }
    else if(graphE) {
        radioTGraph->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
    }
    else if(h1) {
        radioTGraph->SetOn(false); radioTGraphErrors->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
        // Allow only one TH1 type
        if(h1d) { radioTH1F->SetOn(false); radioTH1I->SetOn(false); }
        else if(h1f) { radioTH1D->SetOn(false); radioTH1I->SetOn(false); }
        else if(h1i) { radioTH1D->SetOn(false); radioTH1F->SetOn(false); }
    }
    else if(h2) {
        radioTGraph->SetOn(false); radioTGraphErrors->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
        // Allow only one TH2 type
        if(h2d) { radioTH2F->SetOn(false); radioTH2I->SetOn(false); }
        else if(h2f) { radioTH2D->SetOn(false); radioTH2I->SetOn(false); }
        else if(h2i) { radioTH2D->SetOn(false); radioTH2F->SetOn(false); }
    }
    else if(h3) {
        radioTGraph->SetOn(false); radioTGraphErrors->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        // Allow only one TH3 type
        if(h3d) { radioTH3F->SetOn(false); radioTH3I->SetOn(false); }
        else if(h3f) { radioTH3D->SetOn(false); radioTH3I->SetOn(false); }
        else if(h3i) { radioTH3D->SetOn(false); radioTH3F->SetOn(false); }
    }

    // ---- Enable / Disable columns ----
    xColumnCombo->SetEnabled((graph || graphE || h1 || h2 || h3));
    yColumnCombo->SetEnabled((graph || graphE || h2 || h3));
    zColumnCombo->SetEnabled(h3);

    xErrCombo->SetEnabled(graphE);
    yErrCombo->SetEnabled(graphE);
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