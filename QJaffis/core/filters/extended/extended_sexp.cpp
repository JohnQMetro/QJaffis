//***************************************************************************
// Name:        extended_sexp.cpp
// Author :     John Q Metro
// Purpose :    Specializations of string expressions for particular strings
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     May 3, 2023
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
const jfFilterTypeMeta AUTHOR_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::MISC, "AuthorFilter", "Author Filter",
          QString("The Author Expression Filter  matches elements against a boolean") +
                  " expression, the elements of which are themselves to be matched. " +
                  "These elements are either strings or substrings.",
          IdForGenericFanfic(), createFilter<jfAuthExprFilter> );

// constructors
//----------------------------------------------------------
jfAuthExprFilter::jfAuthExprFilter(const QString& filter_name):jfSimpleExpFilterCore(filter_name) {

}
// ------------------------------
jfAuthExprFilter::jfAuthExprFilter(QString&& filter_name):jfSimpleExpFilterCore(filter_name){

}
//----------------------------------------------------------
jfAuthExprFilter::jfAuthExprFilter(const jfAuthExprFilter& source):jfSimpleExpFilterCore(source.name, source.parsed_expression) {
    description = source.description;
}
//----------------------------------------------------------
jfAuthExprFilter::jfAuthExprFilter(const QString& filter_name, jfSimpleExpr* in_source):jfSimpleExpFilterCore(filter_name, in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const jfFilterTypeMeta& jfAuthExprFilter::GetTypeMetaInfo() const {
    return AUTHOR_FILTER_INFO;
}
// --------------------------------------------------------
jfAuthExprFilter* jfAuthExprFilter::Copy() const {
  return new jfAuthExprFilter(*this);
}
//------------------------------------------------------------
jfFilterBase* jfAuthExprFilter::GenCopy() const {
  return Copy();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAuthExprFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  QString datainfo;
  const jfGenericFanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfGenericFanfic*>(testelem);
  datainfo = typed_item->GetAuthor();
  return InternalMatch(datainfo);
}

//===========================================================================================
const jfFilterTypeMeta LANGUAGE_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::MISC, "LanguageFilter", "Language Filter",
          QString("The Language Expression Filter matches the language name ") +
                  "against a boolean expression, the elements of which are " +
                  "themselves to be matched. These elements are either strings" +
                  " or substrings.",
          IdForFFNItemCore(), createFilter<jfLanguageExprFilter> );


// constructors
//----------------------------------------------------------
jfLanguageExprFilter::jfLanguageExprFilter(const QString& filter_name):jfSimpleExpFilterCore(filter_name) {}
// ----------------------------------------------------------
jfLanguageExprFilter::jfLanguageExprFilter(QString&& filter_name):jfSimpleExpFilterCore(filter_name) {}
//----------------------------------------------------------
jfLanguageExprFilter::jfLanguageExprFilter(const jfLanguageExprFilter& source):jfSimpleExpFilterCore(source.name, source.parsed_expression) {
  description = source.description;
}
//----------------------------------------------------------
jfLanguageExprFilter::jfLanguageExprFilter(const QString& filter_name, jfSimpleExpr* in_source):jfSimpleExpFilterCore(filter_name, in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfLanguageExprFilter::GetTypeMetaInfo() const {
    return LANGUAGE_FILTER_INFO;
}
//------------------------------------------------------------
jfLanguageExprFilter* jfLanguageExprFilter::Copy() const {
  return new jfLanguageExprFilter(*this);
}
//------------------------------------------------------------
jfFilterBase* jfLanguageExprFilter::GenCopy() const {
  return Copy();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfLanguageExprFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  QString datainfo;
  const jfFFNItem* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfFFNItem*>(testelem);
  datainfo = typed_item->GetLanguage();
  return InternalMatch(datainfo);
}

/***************************************************************************/
