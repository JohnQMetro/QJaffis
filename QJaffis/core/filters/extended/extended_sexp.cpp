//***************************************************************************
// Name:        extended_sexp.cpp
// Author :     John Q Metro
// Purpose :    Specializations of string expressions for particular strings
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     March 3, 2023 (removed character filter)
//***************************************************************************
#ifndef EXTENDED_SEXP_FILTER_H
  #include "extended_sexp.h"
#endif // EXTENDED_SEXP_FILTER_H
//------------------------------------------
#ifndef FICBASE_H
  #include "../../../fanfic/data/ficbase.h"
#endif // FICBASE_H
#ifndef JFFNFICOBJ
  #include "../../../fanficnet/data/ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
//--------------------------------
#include <assert.h>
/***************************************************************************/

// constructors
//----------------------------------------------------------
jfAuthExprFilter::jfAuthExprFilter():jfSimpleExpFilterCore() {}
//----------------------------------------------------------
jfAuthExprFilter::jfAuthExprFilter(const jfAuthExprFilter& source):jfSimpleExpFilterCore() {
  CoreCopy(source);
}
//----------------------------------------------------------
jfAuthExprFilter::jfAuthExprFilter(jfSimpleExpr* in_source):jfSimpleExpFilterCore(in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfAuthExprFilter::GetTypeDescription() const {
  return "The Author Expression Filter  matches elements against a boolean \
expression, the elements of which are themselves to be matched. These elements \
are either strings or substrings.";
}
//------------------------------------------------------------
jfAuthExprFilter* jfAuthExprFilter::Copy() const {
  return new jfAuthExprFilter(*this);
}
//------------------------------------------------------------
jfBaseFilter* jfAuthExprFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
QString jfAuthExprFilter::GetTypeID() const {
  return "AuthorFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAuthExprFilter::CoreMatch(const jfBasePD* testelem) const {
  QString datainfo;
  const jfGenericFanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfGenericFanfic*>(testelem);
  datainfo = typed_item->GetAuthor();
  return InternalMatch(datainfo);
}

//===========================================================================================
// constructors
//----------------------------------------------------------
jfLanguageExprFilter::jfLanguageExprFilter():jfSimpleExpFilterCore() {}
//----------------------------------------------------------
jfLanguageExprFilter::jfLanguageExprFilter(const jfLanguageExprFilter& source) {
  CoreCopy(source);
}
//----------------------------------------------------------
jfLanguageExprFilter::jfLanguageExprFilter(jfSimpleExpr* in_source):jfSimpleExpFilterCore(in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfLanguageExprFilter::GetTypeDescription() const {
  return "The Language Expression Filter matches the language name against a boolean \
expression, the elements of which are themselves to be matched. These elements \
are either strings or substrings.";
}
//------------------------------------------------------------
jfLanguageExprFilter* jfLanguageExprFilter::Copy() const {
  return new jfLanguageExprFilter(*this);
}
//------------------------------------------------------------
jfBaseFilter* jfLanguageExprFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
QString jfLanguageExprFilter::GetTypeID() const {
  return "LanguageFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfLanguageExprFilter::CoreMatch(const jfBasePD* testelem) const {
  QString datainfo;
  const jfFFNItem* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfFFNItem*>(testelem);
  datainfo = typed_item->GetLanguage();
  return InternalMatch(datainfo);
}

/***************************************************************************/
