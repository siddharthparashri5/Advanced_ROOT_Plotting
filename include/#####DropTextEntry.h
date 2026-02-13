#ifndef DROP_TEXT_ENTRY_H
#define DROP_TEXT_ENTRY_H

#include <TGTextEntry.h>
#include "FileHandler.h"

class AdvancedPlotGUI;

/**
 * @class DropTextEntry
 * @brief Enhanced TGTextEntry with drag-and-drop functionality
 * 
 * Provides reliable drag-and-drop support for:
 * - File path dropping (file:// URIs)
 * - ROOT object names
 * - Plain text paths
 * 
 * Features:
 * - Validates file before loading
 * - Properly handles URI encoding (%20 for space, etc.)
 * - Supports multiple file types (ROOT, CSV, TXT)
 * - Error feedback on invalid files
 */
class DropTextEntry : public TGTextEntry {
public:
    DropTextEntry(const TGWindow* p,
                  TGTextBuffer* buffer,
                  AdvancedPlotGUI* gui);
    
    virtual ~DropTextEntry();

    // DND event handler - FIXED: added override keyword
    virtual Bool_t HandleDNDDrop(TDNDData* data) override;
    
    // ADDED: Override DoRedraw to lazily register DND after window is mapped
    virtual void DoRedraw() override;
    
    const char* GetDroppedFilePath() const { return fDroppedPath.Data(); }

    ClassDef(DropTextEntry, 0)

private:
    AdvancedPlotGUI* fGUI;
    TString fDroppedPath;
    
    // ADDED: DND registration flag and cached atoms
    Bool_t fDNDRegistered;
    Atom_t fAtomUri;
    Atom_t fAtomText;
    Atom_t fAtomRoot;
    
    // Helper methods
    TString ExtractFilePath(const char* uriText);
    Bool_t IsValidFile(const char* filepath);
};

#endif