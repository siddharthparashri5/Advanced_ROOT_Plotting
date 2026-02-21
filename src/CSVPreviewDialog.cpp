#include "CSVPreviewDialog.h"

#include <TGLayout.h>
#include <TSystem.h>
#include <TGSplitter.h>
#include "TG3DLine.h"
#include "TGListView.h"
#include "TString.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

ClassImp(CSVPreviewDialog)

// ============================================================================
// Constructor
// ============================================================================
CSVPreviewDialog::CSVPreviewDialog(const TGWindow* p, const char* filename)
    : TGTransientFrame(p, nullptr, 900, 600),
      fFilename(filename),
      fSelectedDelimiter(','),
      fSkipRows(0),
      fUseHeaderRow(kTRUE),
      fEncoding("UTF-8"),
      fModalResult(-1),  // -1 = not yet determined
      fListView(nullptr)
{
    SetWindowName("CSV/Text File Import Preview");
    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    
    // Main layout
    TGVerticalFrame* mainFrame = new TGVerticalFrame(this);
    AddFrame(mainFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // ─── FILE INFO ────────────────────────────────────
    TGHorizontalFrame* infoFrame = new TGHorizontalFrame(mainFrame);
    infoFrame->AddFrame(new TGLabel(infoFrame, "File:"), 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    fFilenameLabel = new TGLabel(infoFrame, filename);
    infoFrame->AddFrame(fFilenameLabel, 
        new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 5, 5, 5, 5));
    
    mainFrame->AddFrame(infoFrame, 
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ─── SEPARATOR ────────────────────────────────────
    mainFrame->AddFrame(new TGHorizontal3DLine(mainFrame),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 5));
    
    // ─── CONFIGURATION PANEL ──────────────────────────
    TGGroupFrame* configFrame = new TGGroupFrame(mainFrame, "Import Settings", kHorizontalFrame);
    
    // Delimiter selection
    TGVerticalFrame* delimFrame = new TGVerticalFrame(configFrame);
    delimFrame->AddFrame(new TGLabel(delimFrame, "Delimiter:"),
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    TGTextEntry* fDummyTxtEntry = new TGTextEntry();
    UInt_t myBoxHeight = fDummyTxtEntry->GetHeight();
    
    fDelimiterCombo = new TGComboBox(delimFrame, kDelimiterCombo);
    fDelimiterCombo->SetHeight(myBoxHeight);
    fDelimiterCombo->AddEntry("Comma (,)", 0);
    fDelimiterCombo->AddEntry("Semicolon (;)", 1);
    fDelimiterCombo->AddEntry("Tab (\\t)", 2);
    fDelimiterCombo->AddEntry("Space", 3);
    fDelimiterCombo->AddEntry("Custom", 4);
    fDelimiterCombo->Select(0);
    fDelimiterCombo->Associate(this);
    delimFrame->AddFrame(fDelimiterCombo, 
        new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));
    
    fCustomDelimEntry = new TGTextEntry(delimFrame, "", kCustomDelimEntry);
    fCustomDelimEntry->SetMaxLength(1);
    fCustomDelimEntry->SetText(",");
    fCustomDelimEntry->SetState(kFALSE); 
    fCustomDelimEntry->Associate(this);
    delimFrame->AddFrame(fCustomDelimEntry,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));
    
    configFrame->AddFrame(delimFrame, 
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Skip rows
    TGVerticalFrame* skipFrame = new TGVerticalFrame(configFrame);
    skipFrame->AddFrame(new TGLabel(skipFrame, "Skip First Rows:"),
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    fSkipRowsEntry = new TGNumberEntry(skipFrame, 0, 5, kSkipRowsEntry);
    fSkipRowsEntry->SetLimits(TGNumberEntry::kNELLimitMinMax, 0, 100);
    fSkipRowsEntry->Associate(this);
    skipFrame->AddFrame(fSkipRowsEntry,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));
    
    configFrame->AddFrame(skipFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Header row
    TGVerticalFrame* headerFrame = new TGVerticalFrame(configFrame);
    fHeaderCheckBox = new TGCheckButton(headerFrame, "First row as header", kHeaderCheckBox);
    fHeaderCheckBox->SetState(kButtonDown);
    fHeaderCheckBox->Associate(this);
    headerFrame->AddFrame(fHeaderCheckBox,
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5));
    
    configFrame->AddFrame(headerFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Encoding
    TGVerticalFrame* encodingFrame = new TGVerticalFrame(configFrame);
    encodingFrame->AddFrame(new TGLabel(encodingFrame, "Encoding:"),
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    fEncodingCombo = new TGComboBox(encodingFrame, kEncodingCombo);
    fEncodingCombo->SetHeight(myBoxHeight);
    fEncodingCombo->AddEntry("UTF-8", 0);
    fEncodingCombo->AddEntry("ISO-8859-1", 1);
    fEncodingCombo->AddEntry("ASCII", 2);
    fEncodingCombo->Select(0);
    fEncodingCombo->Associate(this);
    encodingFrame->AddFrame(fEncodingCombo,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    configFrame->AddFrame(encodingFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    mainFrame->AddFrame(configFrame,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ─── SEPARATOR ────────────────────────────────────
    mainFrame->AddFrame(new TGHorizontal3DLine(mainFrame),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 5));
    
    // ─── PREVIEW PANEL ────────────────────────────────
    TGGroupFrame* previewFrame = new TGGroupFrame(mainFrame, "Data Preview", kVerticalFrame);

    // Create TGTextEdit with proper initialization
    fPreviewText = new TGTextEdit(previewFrame, 600, 300);
    fPreviewText->SetReadOnly(kTRUE);
    
    // CRITICAL: Set font BEFORE setting text (use GetFont for ROOT 6.26)
    const TGFont* courierFont = gClient->GetFont("-*-courier-medium-r-*-*-12-*-*-*-*-*-*-*");
    if (courierFont) {
        FontStruct_t fs = courierFont->GetFontStruct();
        fPreviewText->SetFont(fs);
    }

    previewFrame->AddFrame(fPreviewText, 
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Statistics
    TGHorizontalFrame* statsFrame = new TGHorizontalFrame(previewFrame);
    
    fRowCountLabel = new TGLabel(statsFrame, "Rows: 0");
    statsFrame->AddFrame(fRowCountLabel,
        new TGLayoutHints(kLHintsLeft, 5, 20, 5, 5));
    
    fColCountLabel = new TGLabel(statsFrame, "Columns: 0");
    statsFrame->AddFrame(fColCountLabel,
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5));

    previewFrame->AddFrame(statsFrame, 
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

    mainFrame->AddFrame(previewFrame, 
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // ─── BUTTONS ──────────────────────────────────────
    TGHorizontalFrame* buttonFrame = new TGHorizontalFrame(mainFrame);
    
    fOkButton = new TGTextButton(buttonFrame, "Load", kOkButton);
    fOkButton->Associate(this);
    buttonFrame->AddFrame(fOkButton,
        new TGLayoutHints(kLHintsRight, 5, 5, 5, 5));
    
    fCancelButton = new TGTextButton(buttonFrame, "Cancel", kCancelButton);
    fCancelButton->Associate(this);
    buttonFrame->AddFrame(fCancelButton,
        new TGLayoutHints(kLHintsRight, 5, 5, 5, 5));
    
    mainFrame->AddFrame(buttonFrame,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    MapSubwindows();
    Resize(GetDefaultSize());
    CenterOnParent();
    
    // CRITICAL: Load file AFTER all widgets are mapped
    LoadFile();
}

// ============================================================================
// Destructor
// ============================================================================
CSVPreviewDialog::~CSVPreviewDialog()
{
}

// ============================================================================
// Load file contents into memory
// ============================================================================
void CSVPreviewDialog::LoadFile()
{
    fFileLines.clear();
    
    std::ifstream file(fFilename.Data());
    
    if (!file.is_open()) {
        fprintf(stderr, "ERROR: Could not open file: %s\n", fFilename.Data());
        return;
    }
    
    std::string line;
    int count = 0;
    while (std::getline(file, line) && count < 100) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        fFileLines.push_back(line);
        count++;
    }
    file.close();
    
    printf("Loaded %d lines for preview.\n", (int)fFileLines.size());
    
    // Update preview immediately
    UpdatePreview();
}

// ============================================================================
// Split line by delimiter
// ============================================================================
std::vector<std::string> CSVPreviewDialog::SplitLine(const std::string& line, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    
    return tokens;
}

// ============================================================================
// Parse file with current settings
// ============================================================================
void CSVPreviewDialog::ParseFile()
{
    UpdatePreview();
}

// ============================================================================
// Update preview
// ============================================================================
void CSVPreviewDialog::UpdatePreview()
{
    // Determine delimiter
    char delimiter = ',';
    Int_t delimChoice = fDelimiterCombo->GetSelected();
    
    switch(delimChoice) {
        case 0: delimiter = ','; break;
        case 1: delimiter = ';'; break;
        case 2: delimiter = '\t'; break;
        case 3: delimiter = ' '; break;
        case 4: 
            const char* customStr = fCustomDelimEntry->GetText();
            if (customStr && strlen(customStr) > 0) {
                delimiter = customStr[0];
            }
            break;
    }
    
    fSelectedDelimiter = delimiter;
    fSkipRows = (Int_t)fSkipRowsEntry->GetNumber();
    fUseHeaderRow = fHeaderCheckBox->IsDown();

    if (!fPreviewText) return;

    // Clear preview
    fPreviewText->Clear();

    Int_t rowIdx = 0;
    Int_t maxCols = 0;
    std::vector<std::vector<std::string>> allRows;
    
    // Parse and collect rows
    for (size_t i = 0; i < fFileLines.size(); ++i) {
        if ((Int_t)i < fSkipRows) continue;
        
        const std::string& line = fFileLines[i];
        std::vector<std::string> columns = SplitLine(line, delimiter);
        
        if ((Int_t)columns.size() > maxCols) {
            maxCols = (Int_t)columns.size();
        }
        
        allRows.push_back(columns);
        rowIdx++;
        if (rowIdx >= 50) break;
    }
    
    // Build formatted table text
    std::ostringstream tableText;
    const int colWidth = 20;
    
    // Add rows to text
    for (size_t r = 0; r < allRows.size(); ++r) {
        const auto& row = allRows[r];
        
        for (size_t c = 0; c < row.size(); ++c) {
            std::string cell = row[c];
            
            // Truncate if too long
            if (cell.length() > colWidth - 2) {
                cell = cell.substr(0, colWidth - 5) + "...";
            }
            
            // Pad to column width
            while (cell.length() < colWidth) {
                cell += " ";
            }
            
            tableText << cell;
            
            if (c < row.size() - 1) {
                tableText << "| ";
            }
        }
        tableText << "\n";
        
        // Add separator after header row
        if (r == 0 && fUseHeaderRow) {
            for (int c = 0; c < maxCols; ++c) {
                tableText << std::string(colWidth, '-');
                if (c < maxCols - 1) tableText << "+-";
            }
            tableText << "\n";
        }
    }
    
    std::string finalTable = tableText.str();
    if (finalTable.empty()) {
        finalTable = "No data to preview with current settings.";
    }
    
    // CRITICAL: Use LoadBuffer instead of SetText with new TGText
    fPreviewText->LoadBuffer(finalTable.c_str());
    
    // Update labels
    fRowCountLabel->SetText(Form("Rows: %d", (int)allRows.size()));
    fColCountLabel->SetText(Form("Columns: %d", maxCols));
    
    // Force layout update
    fPreviewText->Layout();
    Layout();
    gClient->NeedRedraw(fPreviewText);
}

// ============================================================================
// Process messages
// ============================================================================
Bool_t CSVPreviewDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    switch(GET_MSG(msg)) {
        case kC_COMMAND:
            switch(GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == kOkButton) {
                        printf("OK button clicked\n");
                        UpdatePreview();  // Update one last time
                        fModalResult = 1;
                        UnmapWindow();
                        return kTRUE;
                    } else if (parm1 == kCancelButton) {
                        printf("Cancel button clicked\n");
                        fModalResult = 0;
                        UnmapWindow();
                        return kTRUE;
                    }
                    break;
                    
                case kCM_COMBOBOX:
                    if (parm1 == kDelimiterCombo) {
                        if (fDelimiterCombo->GetSelected() == 4) {
                            fCustomDelimEntry->SetState(kTRUE);
                        } else {
                            fCustomDelimEntry->SetState(kFALSE);
                        }
                        UpdatePreview();
                        return kTRUE;
                    } else if (parm1 == kEncodingCombo) {
                        UpdatePreview();
                        return kTRUE;
                    }
                    break;
                    
                case kCM_CHECKBUTTON:
                    if (parm1 == kHeaderCheckBox) {
                        UpdatePreview();
                        return kTRUE;
                    }
                    break;
            }
            break;
            
        case kC_TEXTENTRY:
            switch(GET_SUBMSG(msg)) {
                case kTE_TEXTCHANGED:
                    if (parm1 == kSkipRowsEntry || parm1 == kCustomDelimEntry) {
                        UpdatePreview();
                        return kTRUE;
                    }
                    break;
            }
            break;
    }
    
    return TGTransientFrame::ProcessMessage(msg, parm1, parm2);
}

// ============================================================================
// Modal dialog - Manual event loop
// ============================================================================
Int_t CSVPreviewDialog::DoModal()
{
    fModalResult = -1;  // -1 = still running
    
    // Map the dialog window
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
    
    // Wait for window system to finish mapping
    gSystem->ProcessEvents();
    gSystem->Sleep(50);
    
    if (!IsMapped()) {
        printf("ERROR: CSV preview window failed to map!\n");
        return 0;
    }
    
    // Process events until window is explicitly unmapped
    while (IsMapped()) {
        gSystem->ProcessEvents();
        gSystem->Sleep(10);
    }
    
    // If we exited loop but fModalResult is still -1, user closed window
    if (fModalResult == -1) {
        fModalResult = 0;
    }
    
    return fModalResult;
}

// ============================================================================
// Close window properly
// ============================================================================
void CSVPreviewDialog::CloseWindow()
{
    fModalResult = 0;
    UnmapWindow();
}

// ============================================================================
// Validation
// ============================================================================
Bool_t CSVPreviewDialog::IsValidDelimiter(char c) const
{
    return std::isprint(c) && c != '"' && c != '\n' && c != '\r';
}
