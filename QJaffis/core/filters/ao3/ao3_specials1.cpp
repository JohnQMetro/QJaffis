/******************************************************************************
Name    :   ao3_specials1.cpp
Author  :   John Q Metro
Purpose :   AO3 rating and orientation filters
Created :   September 9, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   April 19, 2023
******************************************************************************/
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
//-------------------------------------------
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#ifndef AO3_CONSTS_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED
#ifndef AO3_LOOKVALS_H
    #include "../../../fanfic/data/ao3/ao3_lookvals.h"
#endif // AO3_LOOKVALS_H

#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
//----------------------------------------
#include <assert.h>
#include <QRegExp>
/*****************************************************************************/
// constructors
//---------------------------

jfSpecialsFilter::jfSpecialsFilter(const QString& filter_name):jfFilterBase(filter_name) {

}
// --------------------------
jfSpecialsFilter::jfSpecialsFilter(QString&& filter_name):jfFilterBase(filter_name) {

}
//+++++++++++++++++++++++++++++++++++++++++++
// public methods implemented here
//---------------------------
bool jfSpecialsFilter::IsEmpty() const {
  return (value.isEmpty() || IsFull());
}
//---------------------------
QString jfSpecialsFilter::ToString() const {
  return value;
}
//---------------------------
bool jfSpecialsFilter::Includes(QChar test) const {
  return value.contains(test);
}
//========================================================================
const jfFilterTypeMeta AO3_RATING_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::CATEGORY, "AO3RatingFilter",
                     "Rating Filter for AO3",
          QString("Checks if the rating for the tested fanfic is one of the") +
                     " selected ratings.",
          IdForAO3Fanfic(), createFilter<jfAO3RatingFilter> );
// =======================================================================

// constructors
//------------------------------------
jfAO3RatingFilter::jfAO3RatingFilter(const QString& filter_name):jfSpecialsFilter(filter_name) {

}
// -------------------------------------------
jfAO3RatingFilter::jfAO3RatingFilter(QString&& filter_name):jfSpecialsFilter(filter_name) {

}
// -------------------------------------------
jfAO3RatingFilter::jfAO3RatingFilter(const QString& filter_name, const QString& sourcedata):jfSpecialsFilter(filter_name) {
    QString error;
    bool okay = FromStringInner(sourcedata, error);
    jerror::AssertLog(okay, "jfAO3RatingFilter::jfAO3RatingFilter", error);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//------------------------------------
jfFilterBase* jfAO3RatingFilter::GenCopy() const {
  jfAO3RatingFilter* qval = new jfAO3RatingFilter(name);
  qval->description = description;
  qval->value = value;
  return qval;
}
//------------------------------------
const jfFilterTypeMeta& jfAO3RatingFilter::GetTypeMetaInfo() const {
    return AO3_RATING_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented custom methods
//------------------------------------
void jfAO3RatingFilter::SetToFull() {
  value = ao3con::rating_ac;
}
//------------------------------------
bool jfAO3RatingFilter::IsFull() const {
  return value==ao3con::rating_ac;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfAO3RatingFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    QRegExp xrpos = QRegExp("[^"+ao3con::rating_ac+"]");
    if (!sourcedata.contains(xrpos)) {
      value = sourcedata;
      return true;
    }
    else {
        error_out = "input contains invalid characters";
        return false;
    }
}
// ----------------------------------------------------
// the core matching method
bool jfAO3RatingFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfAO3Fanfic* testval = dynamic_cast<const jfAO3Fanfic*>(testelem);
  return value.contains(testval->GetRating());
}

//=======================================================================
const jfFilterTypeMeta AO3_ORIENTATION_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::CATEGORY, "AO3OrientationFilter",
                     "Orientation Filter for AO3",
          QString("Select whether or not the fic should be or allow Gen") +
                  ", M/M, F/M, etc.",
          IdForAO3Fanfic(), createFilter<jfAO3OrientationFilter> );

// =======================================================
jfAO3OrientationFilter::jfAO3OrientationFilter(const QString& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
//-----------------------------
jfAO3OrientationFilter::jfAO3OrientationFilter(QString&& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
//---------------------------------------------
jfAO3OrientationFilter::jfAO3OrientationFilter(const jfAO3OrientationFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
bool jfAO3OrientationFilter::SetToEmpty() {
    if (thedata == NULL) thedata = new jfTagListing();
    ao3values::orientMaker.SetTagsToEmpty(*thedata);
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfAO3OrientationFilter::GetTypeMetaInfo() const {
    return AO3_ORIENTATION_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterBase* jfAO3OrientationFilter::GenCopy() const {
  return new jfAO3OrientationFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAO3OrientationFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfAO3Fanfic* rvalue;
  // we start
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfAO3Fanfic*>(testelem);
  return MatchVsData(rvalue->GetOrientations());
}
//-----------------------------------------------
bool jfAO3OrientationFilter::ModifyList(QStringList* templist) const {
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check the tags against a list of pre-approved tags
bool jfAO3OrientationFilter::DoVerify() {
    QStringList* genrelist = ao3values::orientMaker.GetOrientations();
    if (genrelist == NULL) return false;
    bool result = VerifyTags(genrelist,NULL);
    delete genrelist;
    return result;
}
// ----------------------------------------------------------------
bool jfAO3OrientationFilter::DoVerifyCheck(jfTagListing* to_check) const {
    QStringList* genrelist = ao3values::orientMaker.GetOrientations();
    if (genrelist == NULL) return false;
    bool result = VerifyCheck(to_check, genrelist,NULL);
    delete genrelist;
    return result;
}
