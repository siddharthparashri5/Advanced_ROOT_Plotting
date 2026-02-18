#ifndef ADVANCEDPLOTGUI_H
#define ADVANCEDPLOTGUI_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGListBox.h>
#include <TGListView.h>
#include <TGTextEdit.h>
#include <TGTextView.h>
#include <TGComboBox.h>
#include <TGDNDManager.h>

#include "FileHandler.h"
#include "PlotManager.h"
#include "ScriptEngine.h"
#include "PlotTypes.h"
#include "FitUtils.h"

#include <vector>
#include <string>

class FileHandler;

class AdvancedPlotGUI : public TGMainFrame {
private:
    enum EWidgetIds {
        kBrowseButton = 1,
        kLoadButton,
        kAddPlotButton,
        kRemovePlotButton,
        kClearAllButton,
        kPlotButton,
        kRunScriptButton,
        kRunCommandButton,
        kLoadScriptButton,
        kSaveScriptButton,
        kClearEditorButton,
        kEntrySelector,
        kClearOutputButton
    };

    // GUI Components
    TGMainFrame* fMainFrame;
    TGTextEntry* fFileEntry;  // CHANGED: Regular TGTextEntry instead of DropTextEntry
    TGTextButton* fBrowseButton;
    TGTextButton* fAddPlotButton;
    TGTextButton* fRemovePlotButton;
    TGTextButton* fClearAllButton;
    TGTextButton* fPlotButton;
    TGListBox* fPlotListBox;
    TGTextEntry* fCanvasTitleEntry;
    TGCheckButton* fSameCanvasCheck;
    TGCheckButton* fDividedCanvasCheck;
    TGNumberEntry* fNRowsEntry;
    TGNumberEntry* fNColsEntry;
    TGComboBox* fFitFunctionCombo;
    TGTextEntry* fCustomFuncEntry;
    TGTextButton* fEntrySelectorButton;
    
    // Script panel
    TGComboBox* fScriptLangCombo;
    TGTextEdit* fScriptEditor;
    TGTextView* fScriptOutput;
    TGTextEntry* fCommandEntry;
    
    // Manager objects
    FileHandler* fFileHandler;
    PlotManager* fPlotManager;
    ScriptEngine* fScriptEngine;
    
    // GUI construction helpers
    void BuildFileSection();
    void BuildPlotConfigSection();
    void BuildCanvasOptionsSection();
    void BuildFitSection();
    void BuildScriptPanel();
    
public:
    AdvancedPlotGUI(const TGWindow* p, UInt_t w, UInt_t h);
    virtual ~AdvancedPlotGUI();

    // ADDED: Handle drag-and-drop of ROOT objects from TBrowser
    virtual Bool_t HandleDNDDrop(TDNDData* data);
    
    // Message processing
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
    
    // Getters for managers to access GUI state
    const char* GetFilePath() const { return fFileEntry->GetText(); }
    void SetFilePath(const char* path) { fFileEntry->SetText(path); }

    void OnCommandEnter();
    
    Int_t GetNRows() const { return (Int_t)fNRowsEntry->GetNumber(); }
    Int_t GetNCols() const { return (Int_t)fNColsEntry->GetNumber(); }
    
    TGTextEdit* GetScriptEditor() { return fScriptEditor; }
    TGTextView* GetScriptOutput() { return fScriptOutput; }
    TGTextEntry* GetCommandEntry() { return fCommandEntry; }
    
    // GUI state manipulation helpers for managers
    void EnablePlotControls(Bool_t enable);
    void AddPlotToListBox(const char* desc, Int_t id);
    void RebuildPlotListBox(const std::vector<PlotConfig>& configs);
    void ClearPlotListBox();
    
    ClassDef(AdvancedPlotGUI, 1)
};

#endif // ADVANCEDPLOTGUI_H