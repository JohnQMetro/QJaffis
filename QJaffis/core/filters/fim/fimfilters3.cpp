/******************************************************************************
Name    :   fimfilters3.cpp
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net, new in 2018
Created :   January 21, 2018
Updated :   August 23, 2023
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
const jfFilterTypeMeta CONTENT_TYPE_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::GENRE, "FIMContentTypeFilter",
                     "FIM Genre Filter",
          QString("Compares the Fimfiction.net genres of the fic against the") +
                  " include/exclude/alternate list specified by the filter.",
          IdForFIMFanfics(), createFilter<jfFIMContentTypeFilter> );
//=======================================================================
jfFIMContentTypeFilter::jfFIMContentTypeFilter(const QString& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
// ----------------------------------------
jfFIMContentTypeFilter::jfFIMContentTypeFilter(QString&& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
//---------------------------------------------
jfFIMContentTypeFilter::jfFIMContentTypeFilter(const jfFIMContentTypeFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
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
jfFilterBase* jfFIMContentTypeFilter::GenCopy() const {
  return new jfFIMContentTypeFilter(*this);
}
// ---------------------------------------------
const jfFilterTypeMeta& jfFIMContentTypeFilter::GetTypeMetaInfo() const {
    return CONTENT_TYPE_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMContentTypeFilter::CoreMatch(const jfSearchResultItem* testelem) const {
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
bool jfFIMContentTypeFilter::DoVerify() {
    if (fimcon::tags == NULL) return false;
    QStringList* contentlist = fimcon::tags->namesForType(FIMT_content);
    if (contentlist == NULL) return false;
    bool result = VerifyTags(contentlist,NULL);
    delete contentlist;
    return result;
}
// -------------------------------------------------------------
// check the tags against a list of pre-approved tags
bool jfFIMContentTypeFilter::DoVerifyCheck(jfTagListing* to_check) const {
    if (fimcon::tags == NULL) return false;
    QStringList* contentlist = fimcon::tags->namesForType(FIMT_content);
    if (contentlist == NULL) return false;
    bool result = VerifyCheck(to_check, contentlist,NULL);
    delete contentlist;
    return result;
}

//=======================================================================
const jfFilterTypeMeta FIM_WARNING_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::MISC, "FIMWarningFilter",
                     "FIM Warnings Filter",
          QString("Compares the Fimfiction.net warnings of the fic against the") +
                  " include/exclude/alternate list specified by the filter.",
          IdForFIMFanfics(), createFilter<jfFIMWarningsFilter> );
// ==================================================================
jfFIMWarningsFilter::jfFIMWarningsFilter(const QString& filter_name):jfTagFilterCore(filter_name) {
  cm = ",";
}
// -----------------------------------
jfFIMWarningsFilter::jfFIMWarningsFilter(QString&& filter_name):jfTagFilterCore(filter_name) {
  cm = ",";
}
//---------------------------------------------
jfFIMWarningsFilter::jfFIMWarningsFilter(const jfFIMWarningsFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
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
const jfFilterTypeMeta& jfFIMWarningsFilter::GetTypeMetaInfo() const {
    return FIM_WARNING_FILTER_INFO;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterBase* jfFIMWarningsFilter::GenCopy() const {
  return new jfFIMWarningsFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMWarningsFilter::CoreMatch(const jfSearchResultItem* testelem) const {
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
// ------------------------------------------
bool jfFIMWarningsFilter::DoVerifyCheck(jfTagListing* to_check) const {
    if (fimcon::tags == NULL) return false;
    QStringList* contentlist = fimcon::tags->namesForType(FIMT_warning);
    if (contentlist == NULL) return false;
    bool result = VerifyCheck(to_check, contentlist,NULL);
    delete contentlist;
    return result;
}
/*****************************************************************************/
