#include "ROOTBranchSelectorDialog.h"

#include <TGLayout.h>
#include <TG3DLine.h>
#include <TGMsgBox.h>
#include <TGNumberEntry.h>
#include <TSystem.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TLeaf.h>

#include <iostream>
#include <algorithm>

ClassImp(ROOTBranchSelectorDialog)

// ============================================================================
// Constructor
// ============================================================================
ROOTBranchSelectorDialog::ROOTBranchSelectorDialog(const TGWindow* parent,
                                                   const char* filepath)
    : TGTransientFrame(parent, nullptr, 700, 600),
      fFilepath(filepath)
{
    SetWindowName("Load ROOT File into GUI");
    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    SetCleanup(kDeepCleanup);

    fFile = TFile::Open(filepath, "READ");
    if (!fFile || fFile->IsZombie()) {
        printf("[ROOTBranchSelectorDialog] ERROR: Cannot open %s\n", filepath);
        fFile = nullptr;
        fModalResult = 0;
        return;
    }

    BuildGUI();
    ScanFile();
}

// ============================================================================
// Destructor
// ============================================================================
ROOTBranchSelectorDialog::~ROOTBranchSelectorDialog()
{
    if (fFile) {
        fFile->Close();
        delete fFile;
        fFile = nullptr;
    }
}

// ============================================================================
// Build GUI
// ============================================================================
void ROOTBranchSelectorDialog::BuildGUI()
{
    TGVerticalFrame* main = new TGVerticalFrame(this);
    AddFrame(main, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 8, 8, 8, 8));

    // ── file path label ─────────────────────────────────────────────────────
    fFileLabel = new TGLabel(main, fFilepath.Data());
    fFileLabel->SetTextJustify(kTextLeft);
    main->AddFrame(fFileLabel,
        new TGLayoutHints(kLHintsExpandX | kLHintsLeft, 2, 2, 0, 6));

    main->AddFrame(new TGHorizontal3DLine(main),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 6));

    // ── object list ─────────────────────────────────────────────────────────
    TGGroupFrame* objGrp = new TGGroupFrame(main,
        "Step 1 — Select an object (histogram or TTree)");
    main->AddFrame(objGrp,
        new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 4));

    fObjectListBox = new TGListBox(objGrp, kObjectListBox);
    fObjectListBox->Resize(640, 110);
    fObjectListBox->Associate(this);
    objGrp->AddFrame(fObjectListBox,
        new TGLayoutHints(kLHintsExpandX, 4, 4, 4, 4));

    fObjectInfoLabel = new TGLabel(objGrp, "Select an object above to see details");
    objGrp->AddFrame(fObjectInfoLabel,
        new TGLayoutHints(kLHintsLeft, 4, 4, 2, 4));

    // ── branch panel (TTree only) ────────────────────────────────────────────
    fBranchGroup = new TGGroupFrame(main,
        "Step 2 — Select branches to load  (Ctrl+click for multiple; leave blank = all)");
    main->AddFrame(fBranchGroup,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 4));

    fBranchListBox = new TGListBox(fBranchGroup, kBranchListBox);
    fBranchListBox->SetMultipleSelections(kTRUE);
    fBranchListBox->Resize(640, 130);
    fBranchGroup->AddFrame(fBranchListBox,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));

    fBranchHint = new TGLabel(fBranchGroup,
        "Only numeric (Double_t / Float_t / Int_t) branches are shown.");
    fBranchGroup->AddFrame(fBranchHint,
        new TGLayoutHints(kLHintsLeft, 4, 4, 0, 4));

    TGHorizontalFrame* branchBtnFrame = new TGHorizontalFrame(fBranchGroup);
    fSelectAllBranches = new TGTextButton(branchBtnFrame,
        "Select All", kSelectAllBtn);
    fSelectAllBranches->Associate(this);
    branchBtnFrame->AddFrame(fSelectAllBranches,
        new TGLayoutHints(kLHintsLeft, 4, 4, 2, 2));

    fClearBranches = new TGTextButton(branchBtnFrame, "Clear Selection", kClearBtn);
    fClearBranches->Associate(this);
    branchBtnFrame->AddFrame(fClearBranches,
        new TGLayoutHints(kLHintsLeft, 4, 4, 2, 2));

    fBranchGroup->AddFrame(branchBtnFrame,
        new TGLayoutHints(kLHintsLeft, 2, 2, 0, 4));

    // ── entry range (TTree only) ─────────────────────────────────────────────
    fRangeGroup = new TGGroupFrame(main,
        "Step 3 — Maximum entries to read  (0 = all)");
    main->AddFrame(fRangeGroup,
        new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 4));

    TGHorizontalFrame* rangeHF = new TGHorizontalFrame(fRangeGroup);
    rangeHF->AddFrame(new TGLabel(rangeHF, "Max entries:"),
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 8, 4, 4));

    fMaxEntriesEntry = new TGNumberEntry(rangeHF, 0, 10, -1,
        TGNumberFormat::kNESInteger,
        TGNumberFormat::kNEANonNegative,
        TGNumberFormat::kNELLimitMin, 0);
    fMaxEntriesEntry->Resize(120, 22);
    rangeHF->AddFrame(fMaxEntriesEntry,
        new TGLayoutHints(kLHintsLeft, 0, 8, 4, 4));

    fEntriesInfoLabel = new TGLabel(rangeHF, "");
    rangeHF->AddFrame(fEntriesInfoLabel,
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4, 4, 4, 4));

    fRangeGroup->AddFrame(rangeHF,
        new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 4));

    // ── OK / Cancel ──────────────────────────────────────────────────────────
    main->AddFrame(new TGHorizontal3DLine(main),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 6, 6));

    TGHorizontalFrame* btnFrame = new TGHorizontalFrame(main);
    main->AddFrame(btnFrame,
        new TGLayoutHints(kLHintsCenterX, 4, 4, 4, 4));

    fOkButton = new TGTextButton(btnFrame, "  Load into GUI  ", kOkButton);
    fOkButton->Associate(this);
    btnFrame->AddFrame(fOkButton,
        new TGLayoutHints(kLHintsLeft, 8, 8, 2, 2));

    fCancelButton = new TGTextButton(btnFrame, "  Cancel  ", kCancelButton);
    fCancelButton->Associate(this);
    btnFrame->AddFrame(fCancelButton,
        new TGLayoutHints(kLHintsLeft, 8, 8, 2, 2));
}

// ============================================================================
// ScanFile – populate the object list box
// ============================================================================
void ROOTBranchSelectorDialog::ScanFile()
{
    if (!fFile) return;

    fObjects.clear();
    fObjectListBox->RemoveAll();

    TIter next(fFile->GetListOfKeys());
    TKey* key;
    Int_t id = 0;

    while ((key = (TKey*)next())) {
        std::string cls  = key->GetClassName();
        std::string name = key->GetName();

        // Accept: histograms, TTrees, TNtuples
        bool isHist = (cls.substr(0,2) == "TH");
        bool isTree = (cls == "TTree" || cls == "TNtuple" || cls == "TChain");

        if (!isHist && !isTree) continue;

        ObjEntry e;
        e.name = name;
        e.cls  = cls;
        fObjects.push_back(e);

        std::string label = name + "   [" + cls + "]";
        fObjectListBox->AddEntry(label.c_str(), id++);
    }

    if (id > 0) {
        fObjectListBox->Select(0);
        OnObjectSelected(0);
    }

    fObjectListBox->Layout();
}

// ============================================================================
// OnObjectSelected – update branch list / info label when user picks an object
// ============================================================================
void ROOTBranchSelectorDialog::OnObjectSelected(Int_t id)
{
    fBranchListBox->RemoveAll();

    if (id < 0 || id >= (Int_t)fObjects.size()) return;

    const ObjEntry& obj = fObjects[id];
    bool isTree = (obj.cls == "TTree" || obj.cls == "TNtuple" || obj.cls == "TChain");

    if (isTree) {
        fCurrentTree = (TTree*)fFile->Get(obj.name.c_str());
        if (!fCurrentTree) return;

        char info[256];
        snprintf(info, sizeof(info), "TTree: %s  |  Entries: %lld  |  Branches: %d",
                 obj.name.c_str(),
                 fCurrentTree->GetEntries(),
                 fCurrentTree->GetNbranches());
        fObjectInfoLabel->SetText(info);

        char entriesHint[128];
        snprintf(entriesHint, sizeof(entriesHint),
                 "(tree has %lld entries)", fCurrentTree->GetEntries());
        fEntriesInfoLabel->SetText(entriesHint);

        // Default max = min(50000, nEntries) to keep it responsive
        Long64_t def = std::min((Long64_t)50000, fCurrentTree->GetEntries());
        fMaxEntriesEntry->SetNumber((Double_t)def);

        PopulateBranches();
    } else {
        // Histogram — no branches needed
        fCurrentTree = nullptr;
        fBranchListBox->RemoveAll();
        char info[256];
        snprintf(info, sizeof(info), "%s: %s  —  will be loaded as bin-centre vs counts",
                 obj.cls.c_str(), obj.name.c_str());
        fObjectInfoLabel->SetText(info);
        fEntriesInfoLabel->SetText("");
    }

    fBranchListBox->Layout();
    gClient->NeedRedraw(fObjectInfoLabel);
    gClient->NeedRedraw(fEntriesInfoLabel);
}

// ============================================================================
// PopulateBranches – list numeric leaf branches of the selected TTree
// ============================================================================
void ROOTBranchSelectorDialog::PopulateBranches()
{
    fBranchListBox->RemoveAll();
    if (!fCurrentTree) return;

    TObjArray* branches = fCurrentTree->GetListOfBranches();
    if (!branches) return;

    Int_t id = 0;
    for (Int_t i = 0; i < branches->GetEntries(); ++i) {
        TBranch* br = (TBranch*)branches->At(i);
        if (!br) continue;

        // Check leaves for numeric type
        TObjArray* leaves = br->GetListOfLeaves();
        if (!leaves || leaves->GetEntries() == 0) continue;

        TLeaf* leaf = (TLeaf*)leaves->At(0);
        if (!leaf) continue;

        std::string typeName = leaf->GetTypeName();
        // Accept standard numeric types
        bool numeric = (typeName == "Double_t" || typeName == "Float_t"  ||
                        typeName == "Int_t"    || typeName == "Long64_t" ||
                        typeName == "Short_t"  || typeName == "UInt_t"   ||
                        typeName == "ULong64_t"|| typeName == "double"   ||
                        typeName == "float"    || typeName == "int");
        if (!numeric) continue;

        std::string label = std::string(br->GetName()) + "  [" + typeName + "]";
        fBranchListBox->AddEntry(label.c_str(), id++);
    }

    // Auto-select all by default so a quick click of OK just works
    for (Int_t i = 0; i < id; ++i) {
        fBranchListBox->Select(i);
    }

    fBranchListBox->Layout();
}

// ============================================================================
// GetSelectedBranches
// ============================================================================
std::vector<std::string> ROOTBranchSelectorDialog::GetSelectedBranches() const
{
    std::vector<std::string> result;
    TList selected;
    fBranchListBox->GetSelectedEntries(&selected);
    // Check whether anything is selected
    if (selected.IsEmpty())
        return result;
    TIter next(&selected);
    TObject* obj;
    while ((obj = next())) {
        TGTextLBEntry* entry = dynamic_cast<TGTextLBEntry*>(obj);
        if (!entry) continue;
        // Label format: "BranchName  [TypeName]"
        std::string label = entry->GetText()->GetString();
        size_t spacePos = label.find("  [");
        if (spacePos != std::string::npos)
            label = label.substr(0, spacePos);
        result.push_back(label);
    }
    return result;
}
// ============================================================================
// LoadSelectedData – core method that fills fColumnData
// ============================================================================
bool ROOTBranchSelectorDialog::LoadSelectedData()
{
    Int_t selectedId = fObjectListBox->GetSelected();
    if (selectedId < 0 || selectedId >= (Int_t)fObjects.size()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "No Selection", "Please select an object from the list.",
            kMBIconExclamation, kMBOk);
        return false;
    }

    const ObjEntry& obj = fObjects[selectedId];
    bool isTree = (obj.cls == "TTree" || obj.cls == "TNtuple" || obj.cls == "TChain");

    // Reset
    fColumnData = ColumnData();
    fColumnData.filename = fFilepath.Data();
    fColumnData.name     = obj.name;

    if (isTree) {
        std::vector<std::string> branches = GetSelectedBranches();
        Long64_t maxEntries = (Long64_t)fMaxEntriesEntry->GetNumber();
        return LoadTreeBranches(obj.name, branches, maxEntries);
    } else {
        return LoadHistogram(obj.name, obj.cls);
    }
}

// ============================================================================
// LoadHistogram – use DataReader helpers to fill ColumnData from a histogram
// ============================================================================
bool ROOTBranchSelectorDialog::LoadHistogram(const std::string& name,
                                             const std::string& cls)
{
    TObject* obj = fFile->Get(name.c_str());
    if (!obj) {
        printf("[ROOTBranchSelector] Cannot retrieve %s\n", name.c_str());
        return false;
    }

    bool ok = false;
    if (obj->InheritsFrom(TH3::Class())) {
        ok = DataReader::ExtractFromTH3((TH3*)obj, fColumnData);
    } else if (obj->InheritsFrom(TH2::Class())) {
        ok = DataReader::ExtractFromTH2((TH2*)obj, fColumnData);
    } else if (obj->InheritsFrom(TH1::Class())) {
        ok = DataReader::ExtractFromTH1((TH1*)obj, fColumnData);
    }

    if (ok) {
        printf("[ROOTBranchSelector] Loaded histogram '%s': %d columns, %d rows\n",
               name.c_str(),
               fColumnData.GetNumColumns(),
               fColumnData.GetNumRows());
    }
    return ok;
}

// ============================================================================
// LoadTreeBranches – read selected branches of a TTree into ColumnData
// ============================================================================
bool ROOTBranchSelectorDialog::LoadTreeBranches(const std::string& treeName,
                                                const std::vector<std::string>& branches,
                                                Long64_t maxEntries)
{
    TTree* tree = (TTree*)fFile->Get(treeName.c_str());
    if (!tree) {
        printf("[ROOTBranchSelector] Cannot retrieve tree %s\n", treeName.c_str());
        return false;
    }

    Long64_t nEntries = tree->GetEntries();
    if (maxEntries > 0 && maxEntries < nEntries) nEntries = maxEntries;

    // Determine which branches to load
    std::vector<std::string> toLoad = branches;
    if (toLoad.empty()) {
        // Load all numeric branches
        TObjArray* blist = tree->GetListOfBranches();
        for (Int_t i = 0; i < blist->GetEntries(); ++i) {
            TBranch* br = (TBranch*)blist->At(i);
            if (!br) continue;
            TObjArray* leaves = br->GetListOfLeaves();
            if (!leaves || leaves->GetEntries() == 0) continue;
            TLeaf* leaf = (TLeaf*)leaves->At(0);
            if (!leaf) continue;
            std::string typeName = leaf->GetTypeName();
            bool numeric = (typeName == "Double_t" || typeName == "Float_t"  ||
                            typeName == "Int_t"    || typeName == "Long64_t" ||
                            typeName == "Short_t"  || typeName == "UInt_t"   ||
                            typeName == "double"   || typeName == "float"    ||
                            typeName == "int");
            if (numeric) toLoad.push_back(br->GetName());
        }
    }

    if (toLoad.empty()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "No Branches",
            "No numeric branches found or selected.\n"
            "Only Double_t / Float_t / Int_t branches are supported.",
            kMBIconExclamation, kMBOk);
        return false;
    }

    Int_t nBranches = (Int_t)toLoad.size();
    fColumnData.headers = toLoad;
    fColumnData.data.resize(nBranches);

    // Disconnect all existing branch addresses and reset to avoid stale pointers
    tree->ResetBranchAddresses();

    // Allocate per-branch value buffers
    std::vector<Double_t> vals(nBranches, 0.0);
    for (Int_t i = 0; i < nBranches; ++i) {
        Int_t status = tree->SetBranchAddress(toLoad[i].c_str(), &vals[i]);
        if (status < 0) {
            printf("[ROOTBranchSelector] WARNING: Cannot set address for branch '%s' (status=%d)\n",
                   toLoad[i].c_str(), status);
        }
    }

    // Reserve memory up front to avoid repeated reallocation
    for (Int_t i = 0; i < nBranches; ++i)
        fColumnData.data[i].reserve((size_t)nEntries);

    printf("[ROOTBranchSelector] Reading %lld entries, %d branches...\n",
           nEntries, nBranches);

    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);
        for (Int_t i = 0; i < nBranches; ++i)
            fColumnData.data[i].push_back(vals[i]);
    }

    tree->ResetBranchAddresses();

    printf("[ROOTBranchSelector] Done: %d columns x %lld rows\n",
           nBranches, nEntries);
    return true;
}

// ============================================================================
// DoModal
// ============================================================================
Int_t ROOTBranchSelectorDialog::DoModal()
{
    if (fModalResult == 0) return 0;   // file failed to open
    fModalResult = -1;

    MapSubwindows();
    Resize(GetDefaultSize());
    CenterOnParent();
    MapWindow();

    gSystem->ProcessEvents();
    gSystem->Sleep(50);

    while (IsMapped()) {
        gSystem->ProcessEvents();
        gSystem->Sleep(10);
    }

    if (fModalResult == -1) fModalResult = 0;
    return fModalResult;
}

// ============================================================================
// CloseWindow
// ============================================================================
void ROOTBranchSelectorDialog::CloseWindow()
{
    fModalResult = 0;
    UnmapWindow();
}

// ============================================================================
// ProcessMessage
// ============================================================================
Bool_t ROOTBranchSelectorDialog::ProcessMessage(Long_t msg,
                                                Long_t parm1,
                                                Long_t parm2)
{
    switch (GET_MSG(msg)) {
        case kC_COMMAND:
            switch (GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == kOkButton) {
                        if (LoadSelectedData()) {
                            fModalResult = 1;
                            UnmapWindow();
                        }
                        return kTRUE;
                    }
                    if (parm1 == kCancelButton) {
                        fModalResult = 0;
                        UnmapWindow();
                        return kTRUE;
                    }
                    if (parm1 == kSelectAllBtn) {
                        Int_t n = fBranchListBox->GetNumberOfEntries();
                        for (Int_t i = 0; i < n; ++i)
                            fBranchListBox->Select(i);
                        return kTRUE;
                    }
                    if (parm1 == kClearBtn) {
                        Int_t n = fBranchListBox->GetNumberOfEntries();
                        for (Int_t i = 0; i < n; ++i)
                            fBranchListBox->Select(i, kFALSE);
                        return kTRUE;
                    }
                    break;

                case kCM_LISTBOX:
                    if (parm1 == kObjectListBox) {
                        OnObjectSelected(parm2);
                        return kTRUE;
                    }
                    break;
            }
            break;
    }

    return TGTransientFrame::ProcessMessage(msg, parm1, parm2);
}
