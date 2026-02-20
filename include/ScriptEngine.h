#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <TGTextEdit.h>
#include <TGTextView.h>
#include <TGTextEntry.h>
#include <sstream>

class AdvancedPlotGUI;

class ScriptEngine {
private:
    AdvancedPlotGUI* fMainGUI;

    // Stream redirection for capturing output
    std::ostringstream* fCoutStream;
    std::ostringstream* fCerrStream;
    std::streambuf*     fOldCout;
    std::streambuf*     fOldCerr;

    void RedirectStreams();
    void RestoreStreams();
    void FlushStreams();                    // legacy – forwards to widget version
    void FlushStreams(TGTextView* output);  // preferred

public:
    ScriptEngine(AdvancedPlotGUI* mainGUI);
    virtual ~ScriptEngine();

    // Script operations
    void RunScript(Int_t language);
    void RunCommand(Int_t language);
    void LoadScript();
    void SaveScript();
    void ClearEditor();
    void ClearOutput();
};

#endif // SCRIPTENGINE_H