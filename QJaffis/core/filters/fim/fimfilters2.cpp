/******************************************************************************
Name    :   fimfilters2.cpp
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 12, 2015
Conversion to Qt Started Aug 4, 2015
Updated :   August 8, 2015
******************************************************************************/
#ifndef FIMFILTERS2_H_INCLUDED
  #include "fimfilters2.h"
#endif // FIMFILTERS2_H_INCLUDED
//---------------------------------------------------

#ifndef FIM_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/fim/fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED

#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "../../../fanfic/data/fim/fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
/*****************************************************************************/

// constructors
//----------------------------------------------------------
jfFIM_ShortDesc_ExprFilter::jfFIM_ShortDesc_ExprFilter():jfSimpleExpFilterCore() {}
//----------------------------------------------------------
jfFIM_ShortDesc_ExprFilter::jfFIM_ShortDesc_ExprFilter(const jfFIM_ShortDesc_ExprFilter& source) {
  CoreCopy(source);
}
//----------------------------------------------------------
jfFIM_ShortDesc_ExprFilter::jfFIM_ShortDesc_ExprFilter(jfSimpleExpr* in_source):jfSimpleExpFilterCore(in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIM_ShortDesc_ExprFilter::GetTypeDescription() const {
  return "The Short Description Expression Filter  matches elements against a boolean \
expression, the elements of which are themselves to be matched. These elements \
are either strings or substrings.";
}
//------------------------------------------------------------
jfFIM_ShortDesc_ExprFilter* jfFIM_ShortDesc_ExprFilter::Copy() const {
  return new jfFIM_ShortDesc_ExprFilter(*this);
}
//------------------------------------------------------------
jfBaseFilter* jfFIM_ShortDesc_ExprFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
QString jfFIM_ShortDesc_ExprFilter::GetTypeID() const {
  return "FIMShortDescFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIM_ShortDesc_ExprFilter::CoreMatch(const jfBasePD* testelem) const {
  QString datainfo;
  const jfFIM_Fanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  datainfo = typed_item->GetCompactSummary();
  return InternalMatch(datainfo);
}
//========================================================================================
// constructors
//----------------------------------------------------------
jfFIM_DualDesc_ExprFilter::jfFIM_DualDesc_ExprFilter():jfSimpleExpFilterCore() {}
//----------------------------------------------------------
jfFIM_DualDesc_ExprFilter::jfFIM_DualDesc_ExprFilter(const jfFIM_DualDesc_ExprFilter& source) {
  CoreCopy(source);
}
//----------------------------------------------------------
jfFIM_DualDesc_ExprFilter::jfFIM_DualDesc_ExprFilter(jfSimpleExpr* in_source):jfSimpleExpFilterCore(in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIM_DualDesc_ExprFilter::GetTypeDescription() const {
  return "The Dual Description Expression Filter  matches elements against a boolean \
expression, the elements of which are themselves to be matched. These elements \
are either strings or substrings.";
}
//------------------------------------------------------------
jfFIM_DualDesc_ExprFilter* jfFIM_DualDesc_ExprFilter::Copy() const {
  return new jfFIM_DualDesc_ExprFilter(*this);
}
//------------------------------------------------------------
jfBaseFilter* jfFIM_DualDesc_ExprFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
QString jfFIM_DualDesc_ExprFilter::GetTypeID() const {
  return "FIMDualDescFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIM_DualDesc_ExprFilter::CoreMatch(const jfBasePD* testelem) const {
  QString datainfo;
  const jfFIM_Fanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  datainfo = typed_item->GetCompactSummary();
  if (InternalMatch(datainfo)) return true;
  datainfo = typed_item->GetDescription();
  return InternalMatch(datainfo);
}

//========================================================================================
// constructors
//---------------------------------------------------
jfFIMGroupSCFilter::jfFIMGroupSCFilter():jfMinMaxUFilter() {}
//---------------------------------------------------
jfFIMGroupSCFilter::jfFIMGroupSCFilter(size_t inmin, size_t inmax):jfMinMaxUFilter(inmin,inmax) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIMGroupSCFilter::GetTypeID() const {
  return "FIMGroupSCFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------------
QString jfFIMGroupSCFilter::GetTypeDescription() const {
    return "For Fimfiction.Net Groups, this filter passes if \
the number of Stories is between min and max, inclusive.";
}
//---------------------------------------------------
jfBaseFilter* jfFIMGroupSCFilter::GenCopy() const {
  jfFIMGroupSCFilter* result;
  result = new jfFIMGroupSCFilter(min,max);
  CopyOver(result);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMGroupSCFilter::CoreMatch(const jfBasePD* testelem) const {
  // variables
  const jfFIMGroup* rvalue;
  size_t cvalue;
  // checks
  assert(testelem!=NULL);
  // determining the type
  rvalue = dynamic_cast<const jfFIMGroup*>(testelem);
  cvalue = rvalue->GetFicCount();
  // checking the wordcount
  return TestMatch(cvalue);
}
//========================================================================
// constructors
//---------------------------------------------------
jfFIMGroupMCFilter::jfFIMGroupMCFilter():jfMinMaxUFilter() {}
//---------------------------------------------------
jfFIMGroupMCFilter::jfFIMGroupMCFilter(size_t inmin, size_t inmax):jfMinMaxUFilter(inmin,inmax) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIMGroupMCFilter::GetTypeID() const {
  return "FIMGroupMCFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------------
QString jfFIMGroupMCFilter::GetTypeDescription() const {
    return "For Fanfiction.Net fics, this filter passes if \
the number of Favs is between min and max, inclusive.";
}
//---------------------------------------------------
jfBaseFilter* jfFIMGroupMCFilter::GenCopy() const {
  jfFIMGroupMCFilter* result;
  result = new jfFIMGroupMCFilter(min,max);
  CopyOver(result);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMGroupMCFilter::CoreMatch(const jfBasePD* testelem) const {
  // variables
  const jfFIMGroup* rvalue;
  size_t cvalue;
  // checks
  assert(testelem!=NULL);
  // determining the type
  rvalue = dynamic_cast<const jfFIMGroup*>(testelem);
  cvalue = rvalue->GetMemCount();
  // checking the wordcount
  return TestMatch(cvalue);
}
/*****************************************************************************/
