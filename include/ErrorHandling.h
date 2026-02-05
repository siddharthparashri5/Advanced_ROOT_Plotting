#ifndef PHASE1_IMPROVEMENTS_H
#define PHASE1_IMPROVEMENTS_H

#include <TGMsgBox.h>

// Helper functions for error dialogs
inline void ShowError(const TGWindow* parent, const char* title, const char* message) {
    new TGMsgBox(gClient->GetRoot(), parent, title, message, 
                 kMBIconExclamation, kMBOk);
}

inline void ShowInfo(const TGWindow* parent, const char* title, const char* message) {
    new TGMsgBox(gClient->GetRoot(), parent, title, message,
                 kMBIconAsterisk, kMBOk);
}

inline void ShowWarning(const TGWindow* parent, const char* title, const char* message) {
    new TGMsgBox(gClient->GetRoot(), parent, title, message,
                 kMBIconExclamation, kMBOk);
}

inline Bool_t ShowQuestion(const TGWindow* parent, const char* title, const char* message) {
    Int_t retval;
    new TGMsgBox(gClient->GetRoot(), parent, title, message,
                 kMBIconQuestion, kMBYes | kMBNo, &retval);
    return (retval == kMBYes);
}

#endif // PHASE1_IMPROVEMENTS_H
