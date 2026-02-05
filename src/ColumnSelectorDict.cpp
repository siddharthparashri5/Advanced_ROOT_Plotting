// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdIColumnSelectorDict
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
#include "../include/ColumnSelector.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

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
         instance("ColumnSelectorDialog", ::ColumnSelectorDialog::Class_Version(), "ColumnSelector.h", 19,
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

namespace {
  void TriggerDictionaryInitialization_ColumnSelectorDict_Impl() {
    static const char* headers[] = {
"include/ColumnSelector.h",
nullptr
    };
    static const char* includePaths[] = {
"./include",
"/opt/root/root-6.26.06-install/include/",
"/home/siddharth/Advance_ROOT_Plotting/Advance_ROOT_Plotting/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "ColumnSelectorDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$include/ColumnSelector.h")))  ColumnSelectorDialog;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "ColumnSelectorDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "include/ColumnSelector.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"ColumnSelectorDialog", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("ColumnSelectorDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_ColumnSelectorDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_ColumnSelectorDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_ColumnSelectorDict() {
  TriggerDictionaryInitialization_ColumnSelectorDict_Impl();
}
