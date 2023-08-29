/******************************************************************************
Name    :   ffn_special1.cpp
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016
Updated :   May 5, 2023
******************************************************************************/
#ifndef FFN_SPECIAL1_H
  #include "ffn_special1.h"
#endif // FFN_SPECIAL1_H
//--------------------------------
#ifndef JFFNFICOBJ
  #include "../../../fanficnet/data/ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef FFN_LINKDATA_H_INCLUDED
  #include "../../../fanficnet/data/ffn_linkdata.h"
#endif // FFN_LINKDATA_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
#include <QRegularExpression>
/*****************************************************************************/
const jfFilterTypeMeta FFN_GENRES_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::GENRE, "FFNTagFilter", "FFN Genres Filter",
          QString("Compares the Fanfiction.Net genres of the fic against the ") +
                  "include/exclude/alternate list specified by the filter.",
          IdForFFNItemCore(), createFilter<jfFFNGenresFilter> );
//==========================================================================
jfFFNGenresFilter::jfFFNGenresFilter(const QString& filter_name):jfTagFilterCore(filter_name) {
    cm = "/";
}
// -------------------------------
jfFFNGenresFilter::jfFFNGenresFilter(QString&& filter_name):jfTagFilterCore(filter_name) {
    cm = "/";
}
//---------------------------------------------
jfFFNGenresFilter::jfFFNGenresFilter(const jfFFNGenresFilter& insrc):jfTagFilterCore(insrc) {
  cm = "/";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
bool jfFFNGenresFilter::SetToEmpty() {
  SetEmpty(ffn_consts::taggen_list);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFFNGenresFilter::GetTypeMetaInfo() const {
    return FFN_GENRES_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterBase* jfFFNGenresFilter::GenCopy() const {
  return new jfFFNGenresFilter(*this);
}
//----------------------------------------------
bool jfFFNGenresFilter::ModifyList(QStringList* templist) const {
  // no problem false cases
  if (templist == NULL) return false;
  int hurtpos = templist->indexOf("Hurt");
  if (hurtpos<0) return false;
  // these probably should not happen, but false anyway
  if ((templist->size())==(hurtpos+1)) return false;
  if ((templist->at(hurtpos+1)) != "Comfort" ) return false;
  // the one true case
  templist->replace(hurtpos,"Hurt/Comfort");
  templist->removeAt(hurtpos+1);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFFNGenresFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfFFNItem* rvalue;
  // we start
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFFNItem*>(testelem);
  return MatchVsData(rvalue->GetGenres());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check the tags against a list of pre-approved tags
bool jfFFNGenresFilter::DoVerify() {
  return VerifyTags(ffn_consts::taggen_list,NULL);
}
//------------------------------------------------
bool jfFFNGenresFilter::DoVerifyCheck(jfTagListing* to_check) const {
    return VerifyCheck(to_check, ffn_consts::taggen_list,NULL);
}


//======================================================================
const jfFilterTypeMeta FFN_RATINGS_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::CATEGORY, "FFNRatingFilter", "FFN Rating Filter",
          QString("Filters items depending on their rating. The default is to") +
                  "include all ratings.",
          IdForFFNItemCore(), createFilter<jfFFNRatingFilter> );
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const QString FFN_RATINGS = "K+TM";
const QRegularExpression NOT_FFN_RATINGS = QRegularExpression("[^"+FFN_RATINGS+"]");
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//--------------------------------------------------
jfFFNRatingFilter::jfFFNRatingFilter(const QString& filter_name):jfFilterBase(filter_name) {
    value = FFN_RATINGS;
}
//--------------------------------------
jfFFNRatingFilter::jfFFNRatingFilter(QString&& filter_name):jfFilterBase(filter_name) {
    value = FFN_RATINGS;
}
//--------------------------------------------------
jfFFNRatingFilter::jfFFNRatingFilter(const QString& filter_name, QString value_in):jfFilterBase(filter_name) {
    QString error;
    bool okay = FromStringInner(value_in, error);
    jerror::AssertLog(okay, "jfFFNRatingFilter" ,error);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter, note this currently dependant on fanfiction.net
bool jfFFNRatingFilter::IsEmpty() const {
  return (FFN_RATINGS==value);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNRatingFilter::ToString() const {
  return value;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFFNRatingFilter::GetTypeMetaInfo() const {
    return FFN_RATINGS_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfFilterBase* jfFFNRatingFilter::GenCopy() const {
  jfFFNRatingFilter* result;
  result = new jfFFNRatingFilter(name);
  result->description = description;
  result->value = value;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods
bool jfFFNRatingFilter::Includes(QChar test) const {
  return value.contains(test);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFFNRatingFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfFFNItem* rvalue;
  QChar cvalue;
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFFNItem*>(testelem);
  cvalue = rvalue->GetRating();
  return value.contains(cvalue);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNRatingFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    if (sourcedata.contains(NOT_FFN_RATINGS)) {
        error_out = "String contains characters not used for ratings!";
        return false;
    }
    value = sourcedata;
    return true;
}

//==========================================================================================
const jfFilterTypeMeta FFN_FANDOM_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::FANDOM, "CategoryFilter", "FFN Fandom Filter",
          QString("The Category Expression Filter matches the category name") +
                  "against a boolean expression, the elements of which are themselves" +
                  " to be matched. These elements are either strings or substrings.",
          IdForFFNItemCore(), createFilter<jfFFN_CategoryExprFilter> );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// constructors
//----------------------------------------------------------
jfFFN_CategoryExprFilter::jfFFN_CategoryExprFilter(const QString& filter_name):jfSimpleExpFilterCore(filter_name) {

}
//------------------------
jfFFN_CategoryExprFilter::jfFFN_CategoryExprFilter(QString&& filter_name):jfSimpleExpFilterCore(filter_name) {

}
//----------------------------------------------------------
jfFFN_CategoryExprFilter::jfFFN_CategoryExprFilter(const jfFFN_CategoryExprFilter& source):jfSimpleExpFilterCore(source) {

}
//----------------------------------------------------------
jfFFN_CategoryExprFilter::jfFFN_CategoryExprFilter(const QString& filter_name, jfSimpleExpr* in_source):jfSimpleExpFilterCore(filter_name, in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFFN_CategoryExprFilter::GetTypeMetaInfo() const {
    return FFN_FANDOM_FILTER_INFO;
}

//------------------------------------------------------------
jfFFN_CategoryExprFilter* jfFFN_CategoryExprFilter::Copy() const {
  return new jfFFN_CategoryExprFilter(*this);
}
//------------------------------------------------------------
jfFilterBase* jfFFN_CategoryExprFilter::GenCopy() const {
  return Copy();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFFN_CategoryExprFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  QString datainfo;
  const jfFFNItem* typed_item;
  // checks and starts
  assert(testelem!=NULL);
  typed_item = dynamic_cast<const jfFFNItem*>(testelem);
  datainfo = typed_item->GetCatString();
  return InternalMatch(datainfo);
}
/*****************************************************************************/

