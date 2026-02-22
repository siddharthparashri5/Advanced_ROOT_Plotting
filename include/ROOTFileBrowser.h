#ifndef ROOTFILEBROWSER_H
#define ROOTFILEBROWSER_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGListTree.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGButtonGroup.h>
#include <TFile.h>
#include <TKey.h>
#include <TClass.h>
#include <vector>
#include <string>

class TH1;
class TGraph;
class TTree;

// Structure to hold ROOT object information
struct ROOTObjectInfo {
    std::string name;
    std::string title;
    std::string type;      // "TH1F", "TGraph", "TTree", etc.
    std::string category;  // "Histogram", "Graph", "Tree"
    TObject* object;       // Pointer to the actual object
    bool selected;
    
    ROOTObjectInfo() : object(nullptr), selected(false) {}
};

class ROOTFileBrowser : public TGTransientFrame {
private:
    // GUI components
    TGLabel*              fFilenameLabel;
    TGListTree*           fObjectTree;
    TGTextButton*         fLoadButton;
    TGTextButton*         fPlotButton;       // NEW: Direct plot button
    TGTextButton*         fBrowserButton;
    TGTextButton*         fCancelButton;
    TGCheckButton*        fShowBrowserCheck;
    
    // NEW: Canvas layout options
    TGGroupFrame*         fCanvasOptionsGroup;
    TGRadioButton*        fSeparateCanvasRadio;
    TGRadioButton*        fSingleCanvasRadio;
    TGRadioButton*        fDividedCanvasRadio;
    TGNumberEntry*        fNRowsEntry;
    TGNumberEntry*        fNColsEntry;
    
    // Data
    TFile*                       fFile;
    std::string                  fFilename;
    std::vector<ROOTObjectInfo>  fObjects;
    Int_t                        fModalResult;
    
    // Widget IDs
    enum EWidgetIDs {
        kObjectTree = 1000,
        kLoadButton,
        kPlotButton,
        kBrowserButton,
        kCancelButton,
        kShowBrowserCheck,
        kSeparateCanvasRadio,
        kSingleCanvasRadio,
        kDividedCanvasRadio
    };
    
    // Helper methods
    void BuildGUI();
    void ScanFile();
    void AddObjectToTree(const ROOTObjectInfo& obj, TGListTreeItem* parent);
    std::string GetObjectCategory(const char* className);
    
    // NEW: Plotting methods
    void PlotSelectedObjects();
    void PlotInSeparateCanvases(const std::vector<ROOTObjectInfo>& objects);
    void PlotInSingleCanvas(const std::vector<ROOTObjectInfo>& objects);
    void PlotInDividedCanvas(const std::vector<ROOTObjectInfo>& objects, Int_t nRows, Int_t nCols);
    
public:
    ROOTFileBrowser(const TGWindow* p, const char* filename);
    virtual ~ROOTFileBrowser();
    
    // Modal dialog interface
    Int_t DoModal();
    void CloseWindow() override;
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) override;
    
    // Accessors
    Bool_t ShowBrowser() const;
    std::vector<ROOTObjectInfo> GetSelectedObjects() const;
    
    // NEW: Get canvas layout preferences
    enum CanvasMode {
        kSeparate,
        kSingle,
        kDivided
    };
    CanvasMode GetCanvasMode() const;
    Int_t GetNRows() const { return (Int_t)fNRowsEntry->GetNumber(); }
    Int_t GetNCols() const { return (Int_t)fNColsEntry->GetNumber(); }

    ClassDefOverride(ROOTFileBrowser, 0)
};

#endif // ROOTFILEBROWSER_H