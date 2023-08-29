/******************************************************************************
Name    :   fimfilters2.cpp
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 12, 2015
Conversion to Qt Started Aug 4, 2015
Updated :   July 29, 2023
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
const jfFilterTypeMeta FIM_SHORTDESC_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::MISC, "FIMShortDescFilter",
                     "FIM Short Description Filter",
          QString("The Short Description Expression Filter matches elements") +
                  " against a boolean expression, the elements of which are " +
                  "themselves to be matched. These elements are either strings" +
                  " or substrings. The default source is the short summary found" +
                  " in the 'Card View'.",
          IdForFIMFanfics(), createFilter<jfFIM_ShortDesc_ExprFilter> );

// constructors
//----------------------------------------------------------
jfFIM_ShortDesc_ExprFilter::jfFIM_ShortDesc_ExprFilter(const QString& filter_name):jfSimpleExpFilterCore(filter_name) {

}
//----------------------------------------------------------
jfFIM_ShortDesc_ExprFilter::jfFIM_ShortDesc_ExprFilter(QString&& filter_name):jfSimpleExpFilterCore(filter_name) {

}
//----------------------------------------------------------
jfFIM_ShortDesc_ExprFilter::jfFIM_ShortDesc_ExprFilter(const jfFIM_ShortDesc_ExprFilter& source)
                                                        :jfSimpleExpFilterCore(source.name) {
  CoreCopy(source);
}
//----------------------------------------------------------
jfFIM_ShortDesc_ExprFilter::jfFIM_ShortDesc_ExprFilter(const QString& filter_name,
                            jfSimpleExpr* in_source):jfSimpleExpFilterCore(filter_name, in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFIM_ShortDesc_ExprFilter::GetTypeMetaInfo() const {
    return FIM_SHORTDESC_FILTER_INFO;
}

//------------------------------------------------------------
jfFIM_ShortDesc_ExprFilter* jfFIM_ShortDesc_ExprFilter::Copy() const {
  return new jfFIM_ShortDesc_ExprFilter(*this);
}
//------------------------------------------------------------
jfFilterBase* jfFIM_ShortDesc_ExprFilter::GenCopy() const {
  return Copy();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIM_ShortDesc_ExprFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  QString datainfo;
  const jfFIM_Fanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  datainfo = typed_item->GetCompactSummary();
  return InternalMatch(datainfo);
}
//========================================================================================
const jfFilterTypeMeta FIM_DUAL_DESC_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::MISC, "FIMDualDescFilter",
                     "FIM DUAL Description Filter",
          QString("The Dual Description Expression Filter matches elements against a ") +
                  "boolean expression, the elements of which are themselves to be " +
                  "matched. These elements are either strings or substrings. We use either" +
                  " the card summary of the full summary.",
          IdForFIMFanfics(), createFilter<jfFIM_DualDesc_ExprFilter> );

// constructors
//----------------------------------------------------------
jfFIM_DualDesc_ExprFilter::jfFIM_DualDesc_ExprFilter(const QString& filter_name):jfSimpleExpFilterCore(filter_name) {
}
// --------------------------------------
jfFIM_DualDesc_ExprFilter::jfFIM_DualDesc_ExprFilter(QString&& filter_name):jfSimpleExpFilterCore(filter_name) {

}
//----------------------------------------------------------
jfFIM_DualDesc_ExprFilter::jfFIM_DualDesc_ExprFilter(const jfFIM_DualDesc_ExprFilter& source)
                                                    :jfSimpleExpFilterCore(source.name) {
  CoreCopy(source);
}
//----------------------------------------------------------
jfFIM_DualDesc_ExprFilter::jfFIM_DualDesc_ExprFilter(const QString& filter_name,
                          jfSimpleExpr* in_source):jfSimpleExpFilterCore(filter_name, in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFIM_DualDesc_ExprFilter::GetTypeMetaInfo() const {
    return FIM_DUAL_DESC_FILTER_INFO;
}
//------------------------------------------------------------
jfFIM_DualDesc_ExprFilter* jfFIM_DualDesc_ExprFilter::Copy() const {
  return new jfFIM_DualDesc_ExprFilter(*this);
}
//------------------------------------------------------------
jfFilterBase* jfFIM_DualDesc_ExprFilter::GenCopy() const {
  return Copy();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIM_DualDesc_ExprFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  QString datainfo;
  const jfFIM_Fanfic* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  datainfo = typed_item->GetCompactSummary();
  if (InternalMatch(datainfo)) return true;
  datainfo = typed_item->GetSummary();
  return InternalMatch(datainfo);
}

//========================================================================================
const jfFilterTypeMeta FIM_GROUP_STORY_COUNT_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::COUNTS, "FIMGroupSCFilter",
                     "FIM Group Story Count",
          QString("For Fimfiction.Net Groups, this filter passes if ") +
                  "the number of Stories is between min and max, inclusive.",
          IdForFIMGroups(), createFilter<jfFIMGroupSCFilter> );

// constructors
//---------------------------------------------------
jfFIMGroupSCFilter::jfFIMGroupSCFilter(const QString& name):jfMinMaxUFilter(name) {}
//---------------------------------------------------
jfFIMGroupSCFilter::jfFIMGroupSCFilter(const QString& name, size_t inmin, size_t inmax):jfMinMaxUFilter(name, inmin,inmax) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFIMGroupSCFilter::GetTypeMetaInfo() const {
    return FIM_GROUP_STORY_COUNT_FILTER_INFO;
}
//---------------------------------------------------
jfFilterBase* jfFIMGroupSCFilter::GenCopy() const {
  jfFIMGroupSCFilter* result;
  result = new jfFIMGroupSCFilter(name, min,max);
  result->description = description;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMGroupSCFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const QString fname = "jfFIMGroupSCFilter::CoreMatch";
  // variables
  const jfFIMGroup* rvalue;
  size_t cvalue;
  // checks
  assert(testelem!=NULL);
  // determining the type
  rvalue = dynamic_cast<const jfFIMGroup*>(testelem);
  /**/JDEBUGLOGS(fname,1,rvalue->GetTitle())
  cvalue = rvalue->GetFicCount();
  /**/JDEBUGLOGST(fname,2,cvalue)
  // checking the wordcount
  bool tmatch = TestMatch(cvalue);
  /**/JDEBUGLOGB(fname,3,cvalue)
  return tmatch;
}
//========================================================================
const jfFilterTypeMeta FIM_GROUP_MEMBER_COUNT_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::COUNTS, "FIMGroupMCFilter",
                     "FIM Group Member Count",
          QString("For Fimfiction.Net Groups, this filter passes if ") +
                  "the number of Members is between min and max, inclusive.",
          IdForFIMGroups(), createFilter<jfFIMGroupMCFilter> );


// constructors
//---------------------------------------------------
jfFIMGroupMCFilter::jfFIMGroupMCFilter(const QString& name):jfMinMaxUFilter(name) {}
//---------------------------------------------------
jfFIMGroupMCFilter::jfFIMGroupMCFilter(const QString& name, size_t inmin, size_t inmax):jfMinMaxUFilter(name, inmin,inmax) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFIMGroupMCFilter::GetTypeMetaInfo() const {
    return FIM_GROUP_MEMBER_COUNT_FILTER_INFO;
}

//---------------------------------------------------
jfFilterBase* jfFIMGroupMCFilter::GenCopy() const {
  jfFIMGroupMCFilter* result;
  result = new jfFIMGroupMCFilter(name, min,max);
  result->description = description;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMGroupMCFilter::CoreMatch(const jfSearchResultItem* testelem) const {
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
