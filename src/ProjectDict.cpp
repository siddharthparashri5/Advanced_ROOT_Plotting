// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dOdOdIsrcdIProjectDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "../include/AdvancedPlotGUI.h"
#include "../include/ColumnSelector.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void delete_AdvancedPlotGUI(void *p);
   static void deleteArray_AdvancedPlotGUI(void *p);
   static void destruct_AdvancedPlotGUI(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::AdvancedPlotGUI*)
   {
      ::AdvancedPlotGUI *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::AdvancedPlotGUI >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("AdvancedPlotGUI", ::AdvancedPlotGUI::Class_Version(), "../include/AdvancedPlotGUI.h", 143,
                  typeid(::AdvancedPlotGUI), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::AdvancedPlotGUI::Dictionary, isa_proxy, 4,
                  sizeof(::AdvancedPlotGUI) );
      instance.SetDelete(&delete_AdvancedPlotGUI);
      instance.SetDeleteArray(&deleteArray_AdvancedPlotGUI);
      instance.SetDestructor(&destruct_AdvancedPlotGUI);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::AdvancedPlotGUI*)
   {
      return GenerateInitInstanceLocal((::AdvancedPlotGUI*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_ColumnSelectorDialog(void *p);
   static void deleteArray_ColumnSelectorDialog(void *p);
   static void destruct_ColumnSelectorDialog(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ColumnSelectorDialog*)
   {
      ::ColumnSelectorDialog *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ColumnSelectorDialog >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("ColumnSelectorDialog", ::ColumnSelectorDialog::Class_Version(), "../include/ColumnSelector.h", 19,
                  typeid(::ColumnSelectorDialog), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ColumnSelectorDialog::Dictionary, isa_proxy, 4,
                  sizeof(::ColumnSelectorDialog) );
      instance.SetDelete(&delete_ColumnSelectorDialog);
      instance.SetDeleteArray(&deleteArray_ColumnSelectorDialog);
      instance.SetDestructor(&destruct_ColumnSelectorDialog);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ColumnSelectorDialog*)
   {
      return GenerateInitInstanceLocal((::ColumnSelectorDialog*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr AdvancedPlotGUI::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *AdvancedPlotGUI::Class_Name()
{
   return "AdvancedPlotGUI";
}

//______________________________________________________________________________
const char *AdvancedPlotGUI::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int AdvancedPlotGUI::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *AdvancedPlotGUI::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *AdvancedPlotGUI::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::AdvancedPlotGUI*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr ColumnSelectorDialog::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *ColumnSelectorDialog::Class_Name()
{
   return "ColumnSelectorDialog";
}

//______________________________________________________________________________
const char *ColumnSelectorDialog::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int ColumnSelectorDialog::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ColumnSelectorDialog::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ColumnSelectorDialog::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ColumnSelectorDialog*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void AdvancedPlotGUI::Streamer(TBuffer &R__b)
{
   // Stream an object of class AdvancedPlotGUI.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(AdvancedPlotGUI::Class(),this);
   } else {
      R__b.WriteClassBuffer(AdvancedPlotGUI::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_AdvancedPlotGUI(void *p) {
      delete ((::AdvancedPlotGUI*)p);
   }
   static void deleteArray_AdvancedPlotGUI(void *p) {
      delete [] ((::AdvancedPlotGUI*)p);
   }
   static void destruct_AdvancedPlotGUI(void *p) {
      typedef ::AdvancedPlotGUI current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::AdvancedPlotGUI

//______________________________________________________________________________
void ColumnSelectorDialog::Streamer(TBuffer &R__b)
{
   // Stream an object of class ColumnSelectorDialog.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(ColumnSelectorDialog::Class(),this);
   } else {
      R__b.WriteClassBuffer(ColumnSelectorDialog::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_ColumnSelectorDialog(void *p) {
      delete ((::ColumnSelectorDialog*)p);
   }
   static void deleteArray_ColumnSelectorDialog(void *p) {
      delete [] ((::ColumnSelectorDialog*)p);
   }
   static void destruct_ColumnSelectorDialog(void *p) {
      typedef ::ColumnSelectorDialog current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::ColumnSelectorDialog

namespace ROOT {
   static TClass *vectorlEPlotConfiggR_Dictionary();
   static void vectorlEPlotConfiggR_TClassManip(TClass*);
   static void *new_vectorlEPlotConfiggR(void *p = nullptr);
   static void *newArray_vectorlEPlotConfiggR(Long_t size, void *p);
   static void delete_vectorlEPlotConfiggR(void *p);
   static void deleteArray_vectorlEPlotConfiggR(void *p);
   static void destruct_vectorlEPlotConfiggR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PlotConfig>*)
   {
      vector<PlotConfig> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PlotConfig>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PlotConfig>", -2, "vector", 389,
                  typeid(vector<PlotConfig>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPlotConfiggR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PlotConfig>) );
      instance.SetNew(&new_vectorlEPlotConfiggR);
      instance.SetNewArray(&newArray_vectorlEPlotConfiggR);
      instance.SetDelete(&delete_vectorlEPlotConfiggR);
      instance.SetDeleteArray(&deleteArray_vectorlEPlotConfiggR);
      instance.SetDestructor(&destruct_vectorlEPlotConfiggR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PlotConfig> >()));

      ::ROOT::AddClassAlternate("vector<PlotConfig>","std::vector<PlotConfig, std::allocator<PlotConfig> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<PlotConfig>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPlotConfiggR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PlotConfig>*)nullptr)->GetClass();
      vectorlEPlotConfiggR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPlotConfiggR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPlotConfiggR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PlotConfig> : new vector<PlotConfig>;
   }
   static void *newArray_vectorlEPlotConfiggR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PlotConfig>[nElements] : new vector<PlotConfig>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPlotConfiggR(void *p) {
      delete ((vector<PlotConfig>*)p);
   }
   static void deleteArray_vectorlEPlotConfiggR(void *p) {
      delete [] ((vector<PlotConfig>*)p);
   }
   static void destruct_vectorlEPlotConfiggR(void *p) {
      typedef vector<PlotConfig> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PlotConfig>

namespace {
  void TriggerDictionaryInitialization_ProjectDict_Impl() {
    static const char* headers[] = {
"../include/AdvancedPlotGUI.h",
"../include/ColumnSelector.h",
nullptr
    };
    static const char* includePaths[] = {
"/opt/root/root-6.26.06-install/include/",
"/home/siddharth/IFIC_Work/My_Codes/Advance_ROOT_Plotting/obj/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "ProjectDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate(R"ATTRDUMP(Advanced ROOT plotting GUI with scripting)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$../include/AdvancedPlotGUI.h")))  AdvancedPlotGUI;
class __attribute__((annotate("$clingAutoload$../include/ColumnSelector.h")))  ColumnSelectorDialog;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "ProjectDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "../include/AdvancedPlotGUI.h"
#include "../include/ColumnSelector.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"AdvancedPlotGUI", payloadCode, "@",
"ColumnSelectorDialog", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("ProjectDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_ProjectDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_ProjectDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_ProjectDict() {
  TriggerDictionaryInitialization_ProjectDict_Impl();
}
