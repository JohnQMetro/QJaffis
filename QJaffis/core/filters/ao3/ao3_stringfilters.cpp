//***************************************************************************
// Name:        ao3_stringfilters.cpp
// Author :     John Q Metro
// Purpose :    Simple expression filters specific to archiveofourown.org
// Created:     September 8, 2012
// Conversion to Qt Started Oct 3, 2013
// Updated:     April 15, 2023
//***************************************************************************
#ifndef AO3_STRINGFILTERS_H_INCLUDED
  #include "ao3_stringfilters.h"
#endif // AO3_STRINGFILTERS_H_INCLUDED
//----------------------------------------------
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#include <assert.h>
//****************************************************************************
const jfFilterTypeMeta EXTRATAG_EXPR_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::MISC, "ExtraTagFilter", "Extra Tags Filter",
          QString("The Extra Tag Filter matches the contents of the extra tags") +
                  " against a boolean expression, the elements of which are " +
                  "themselves to be matched. These elements are either strings" +
                  " or substrings.",
          IdForAO3Fanfic(), createFilter<jfExtraTagFilter> );
// =========================================================
// constructors
//----------------------------------------------------------
jfExtraTagFilter::jfExtraTagFilter(const QString& filter_name):jfSimpleExpFilterCore(filter_name) {

}
//---------------------------
jfExtraTagFilter::jfExtraTagFilter(QString&& filter_name):jfSimpleExpFilterCore(filter_name) {

}
//---------------------------
jfExtraTagFilter::jfExtraTagFilter(const jfExtraTagFilter& source):jfSimpleExpFilterCore(source.name) {
    CoreCopy(source);
}
// -------------------------------
jfExtraTagFilter::jfExtraTagFilter(const QString& filter_name, jfSimpleExpr* in_source):jfSimpleExpFilterCore(filter_name, in_source) {

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfExtraTagFilter* jfExtraTagFilter::Copy() const {
  return new jfExtraTagFilter(*this);
}
//------------------------------------------------------------
jfFilterBase* jfExtraTagFilter::GenCopy() const {
  return Copy();
}
const jfFilterTypeMeta& jfExtraTagFilter::GetTypeMetaInfo() const {
    return EXTRATAG_EXPR_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfExtraTagFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  QString datainfo;
  const jfAO3Fanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfAO3Fanfic*>(testelem);
  datainfo = typed_item->GetJoinedExtraTags();
  return InternalMatch(datainfo);
}
//****************************************************************************
