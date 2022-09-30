//***************************************************************************
// Name:        ao3_stringfilters.cpp
// Author :     John Q Metro
// Purpose :    Simple expression filters specific to archiveofourown.org
// Created:     September 8, 2012
// Conversion to Qt Started Oct 3, 2013
// Updated:     May 15, 2014
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
// constructors
//----------------------------------------------------------
jfExtraTagFilter::jfExtraTagFilter():jfSimpleExpFilterCore() {}
//----------------------------------------------------------
jfExtraTagFilter::jfExtraTagFilter(const jfExtraTagFilter& source):jfSimpleExpFilterCore() {
  CoreCopy(source);
}
//----------------------------------------------------------
jfExtraTagFilter::jfExtraTagFilter(jfSimpleExpr* in_source):jfSimpleExpFilterCore(in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfExtraTagFilter::GetTypeDescription() const {
  return "The Extra Tag Filter matches the contents of the extra tags against\
 a boolean expression, the elements of which are themselves to be matched. These \
elements are either strings or substrings.";
}
//------------------------------------------------------------
jfExtraTagFilter* jfExtraTagFilter::Copy() const {
  return new jfExtraTagFilter(*this);
}
//------------------------------------------------------------
jfBaseFilter* jfExtraTagFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
QString jfExtraTagFilter::GetTypeID() const {
  return "ExtraTagFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfExtraTagFilter::CoreMatch(const jfBasePD* testelem) const {
  QString datainfo;
  const jfAO3Fanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfAO3Fanfic*>(testelem);
  datainfo = typed_item->GetJoinedExtraTags();
  return InternalMatch(datainfo);
}
//****************************************************************************
