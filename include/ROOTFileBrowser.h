#ifndef ROOTFILEBROWSER_H
#define ROOTFILEBROWSER_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGListTree.h>
#include <TGLabel.h>
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
    TGTextButton*         fBrowserButton;
    TGTextButton*         fCancelButton;
    TGCheckButton*        fShowBrowserCheck;
    
    // Data
    TFile*                       fFile;
    std::string                  fFilename;
    std::vector<ROOTObjectInfo>  fObjects;
    Int_t                        fModalResult;
    
    // Widget IDs
    enum EWidgetIDs {
        kObjectTree = 1000,
        kLoadButton,
        kBrowserButton,
        kCancelButton,
        kShowBrowserCheck
    };
    
    // Helper methods
    void BuildGUI();
    void ScanFile();
    void AddObjectToTree(const ROOTObjectInfo& obj, TGListTreeItem* parent);
    std::string GetObjectCategory(const char* className);
    
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
    
    //ClassDefOverride(ROOTFileBrowser, 0)
};

#endif // ROOTFILEBROWSER_H
