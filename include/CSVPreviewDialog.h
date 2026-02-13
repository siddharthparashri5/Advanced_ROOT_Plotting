#ifndef CSV_PREVIEW_DIALOG_H
#define CSV_PREVIEW_DIALOG_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>
#include <TGTextEdit.h>
#include <TGLabel.h>
#include <TGMsgBox.h>


#include <string>
#include <vector>

/**
 * @class CSVPreviewDialog
 * @brief CSV/Text file import preview dialog (LibreOffice-style)
 * 
 * Provides an interactive preview of CSV/text file contents before loading,
 * allowing users to configure import settings such as:
 * - Delimiter character (comma, semicolon, tab, space, custom)
 * - Number of header rows to skip
 * - Data encoding
 * - Preview of first N rows with proper column separation
 * 
 * Example usage:
 * @code
 * CSVPreviewDialog dialog(gClient->GetRoot(), "data.csv");
 * if (dialog.DoModal() == 1) {
 *     char delimiter = dialog.GetDelimiter();
 *     Int_t skipRows = dialog.GetSkipRows();
 * }
 * @endcode
 */
class CSVPreviewDialog : public TGTransientFrame {
public:
    CSVPreviewDialog(const TGWindow* p, const char* filename);
    virtual ~CSVPreviewDialog();
    
    // Get configuration
    char GetDelimiter() const { return fSelectedDelimiter; }
    Int_t GetSkipRows() const { return fSkipRows; }
    Bool_t UseHeaderRow() const { return fUseHeaderRow; }
    const char* GetEncoding() const { return fEncoding.c_str(); }
    
    // Modal dialog
    Int_t DoModal();
    virtual void CloseWindow() override;

protected:
    virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

private:
    // File information
    TString fFilename;
    std::vector<std::string> fFileLines;
    
    // Configuration
    char fSelectedDelimiter;
    Int_t fSkipRows;
    Bool_t fUseHeaderRow;
    std::string fEncoding;
    Int_t fModalResult;  // ADDED: to track OK/Cancel
    
    // GUI Components
    TGLabel* fFilenameLabel;
    TGComboBox* fDelimiterCombo;
    TGTextEntry* fCustomDelimEntry;
    TGNumberEntry* fSkipRowsEntry;
    TGCheckButton* fHeaderCheckBox;
    TGComboBox* fEncodingCombo;
    
    // FIXED: Using TGTextEdit instead of TGTable/TGListView for ROOT 6.26 compatibility
    TGTextEdit* fPreviewText;
    TGListView* fListView;  // Keep declaration for compatibility but use TGTextEdit
    
    TGLabel* fRowCountLabel;
    TGLabel* fColCountLabel;
    
    TGTextButton* fOkButton;
    TGTextButton* fCancelButton;
    
    // Control IDs
    enum {
        kDelimiterCombo = 100,
        kCustomDelimEntry,
        kSkipRowsEntry,
        kHeaderCheckBox,
        kEncodingCombo,
        kOkButton,
        kCancelButton
    };
    
    // Helper methods
    void LoadFile();
    void ParseFile();
    void UpdatePreview();
    std::vector<std::string> SplitLine(const std::string& line, char delimiter);
    Bool_t IsValidDelimiter(char c) const;

    
};

#endif