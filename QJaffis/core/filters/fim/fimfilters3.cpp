/******************************************************************************
Name    :   fimfilters3.cpp
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net, new in 2018
Created :   January 21, 2018
Updated :   January 21, 2018
******************************************************************************/
#ifndef FIMFILTERS3_H
    #include "fimfilters3.h"
#endif // FIMFILTERS3_H
#ifndef FIM_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/fim/fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED
#ifndef FIM_CONSTANTS_H
  #include "../../../fanfic/data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H
//-------------------------------------------------
#include <assert.h>
/*****************************************************************************/
//=======================================================================
jfFIMContentTypeFilter::jfFIMContentTypeFilter():jfTagFilterCore() {
  cm = ",";
}
//---------------------------------------------
jfFIMContentTypeFilter::jfFIMContentTypeFilter(const jfFIMContentTypeFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
  validdata = DoVerify();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
bool jfFIMContentTypeFilter::SetToEmpty() {
    if (fimcon::tags == NULL) return false;
    if (thedata == NULL) thedata = new jfTagListing();
    fimcon::tags->SetTagsToEmpty(*thedata,FIMT_content);
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual methods
QString jfFIMContentTypeFilter::GetTypeID() const {
  return "FIMContentTypeFilter";
}
//-----------------------------------------------------------------------------
QString jfFIMContentTypeFilter::GetTypeDescription() const {
    return "Compares the Fimfiction.net genres of the fic against the \
include/exclude/alternate list specified by the filter.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfBaseFilter* jfFIMContentTypeFilter::GenCopy() const {
  return new jfFIMContentTypeFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMContentTypeFilter::CoreMatch(const jfBasePD* testelem) const {
  const jfFIM_Fanfic* rvalue;
  // we start
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  return MatchVsData(rvalue->GetContentTypes());
}
//-----------------------------------------------
bool jfFIMContentTypeFilter::ModifyList(QStringList* templist) const {
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check the tags against a list of pre-approved tags
bool jfFIMContentTypeFilter::DoVerify() {
    if (fimcon::tags == NULL) return false;
    QStringList* contentlist = fimcon::tags->namesForType(FIMT_content);
    if (contentlist == NULL) return false;
    bool result = VerifyTags(contentlist,NULL);
    delete contentlist;
    return result;
}
//=======================================================================
jfFIMWarningsFilter::jfFIMWarningsFilter():jfTagFilterCore() {
  cm = ",";
}
//---------------------------------------------
jfFIMWarningsFilter::jfFIMWarningsFilter(const jfFIMWarningsFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
  validdata = DoVerify();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
bool jfFIMWarningsFilter::SetToEmpty() {
    if (fimcon::tags == NULL) return false;
    if (thedata == NULL) thedata = new jfTagListing();
    fimcon::tags->SetTagsToEmpty(*thedata,FIMT_warning);
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual methods
QString jfFIMWarningsFilter::GetTypeID() const {
  return "FIMWarningFilter";
}
//-----------------------------------------------------------------------------
QString jfFIMWarningsFilter::GetTypeDescription() const {
    return "Compares the Fimfiction.net genres of the fic against the \
include/exclude/alternate list specified by the filter.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfBaseFilter* jfFIMWarningsFilter::GenCopy() const {
  return new jfFIMWarningsFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMWarningsFilter::CoreMatch(const jfBasePD* testelem) const {
  const jfFIM_Fanfic* rvalue;
  // we start
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  return MatchVsData(rvalue->GetWarnings());
}
//-----------------------------------------------
bool jfFIMWarningsFilter::ModifyList(QStringList* templist) const {
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check the tags against a list of pre-approved tags
bool jfFIMWarningsFilter::DoVerify() {
    if (fimcon::tags == NULL) return false;
    QStringList* contentlist = fimcon::tags->namesForType(FIMT_warning);
    if (contentlist == NULL) return false;
    bool result = VerifyTags(contentlist,NULL);
    delete contentlist;
    return result;
}
//=======================================================================
/*****************************************************************************/
