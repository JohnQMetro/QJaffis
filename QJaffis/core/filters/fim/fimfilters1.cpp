/******************************************************************************
Name    :   fimfilters1.cpp
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 4, 2012
Conversion to QT Started September 30, 2013
Updated :   May 28, 2023
******************************************************************************/
#ifndef FIMFILTERS1_H_INCLUDED
  #include "fimfilters1.h"
#endif // FIMFILTERS1_H_INCLUDED
#ifndef FIM_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/fim/fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED
#ifndef FIM_CONSTANTS_H
  #include "../../../fanfic/data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-------------------------------------------------
#include <assert.h>
#include <QRegExp>
/*****************************************************************************/
const jfFilterTypeMeta FIM_THUMBS_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::FAVS, "FimThumbsFilter", "FIM Thumbs Filter",
          QString("Fimfiction.net fics have thumbs up and thumbs down rating.") +
                  " This filter passes fics for which the thumbsup-thumbsdown " +
                  "lies between the min and max.",
          IdForFIMFanfics(), createFilter<jfFimThumbsFilter> );

// =====================================================================
jfFimThumbsFilter::jfFimThumbsFilter(const QString& filter_name):jfFilterBase(filter_name) {
    min = INT_MIN;
    max =  INT_MAX;
    include_disabled = true;
}
// ------------------------------------------
jfFimThumbsFilter::jfFimThumbsFilter(QString&& filter_name):jfFilterBase(filter_name) {
    min = INT_MIN;
    max =  INT_MAX;
    include_disabled = true;
}

//---------------------------------------------------
jfFimThumbsFilter::jfFimThumbsFilter(int inmin, int inmax):jfFilterBase("(Default Name)") {
    bool okay = SetValues(inmin, inmax, false);
    jerror::AssertLog(okay, "jfFimThumbsFilter", "Invalid minium and maximum values");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting values
bool jfFimThumbsFilter::SetValues(int inmin, int inmax, bool inc_disabled) {
  if (inmax>=inmin) {
    min = inmin;
    max = inmax;
    include_disabled = inc_disabled;
    return true;
  }
  else return false;
}
//-----------------------------------------------------------------------
// getting values
//-------------------------------------------
int jfFimThumbsFilter::GetMin() const { return min; }
//-------------------------------------------
int  jfFimThumbsFilter::GetMax() const { return max; }
//-------------------------------------------
bool jfFimThumbsFilter::GetIncludeDisabled() const { return include_disabled; }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual methods
bool jfFimThumbsFilter::IsEmpty() const {
    if (!include_disabled) return false;
    return ((min == INT_MIN) && (max == INT_MAX));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFimThumbsFilter::ToString() const {
  jfOutString result;
  result << min << max << include_disabled;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFimThumbsFilter::GetTypeMetaInfo() const {
    return FIM_THUMBS_FILTER_INFO;
}
//---------------------------------------------------
jfFilterBase* jfFimThumbsFilter::GenCopy() const {
  jfFimThumbsFilter* result;
  result = new jfFimThumbsFilter(min,max);
  result->include_disabled = include_disabled;
  result->name = name;
  result->description = description;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFimThumbsFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    jfLineParse split_line = jfLineParse(sourcedata);
    if (split_line.NNotX(3)) {
        error_out = "The source does not have 3 fields!";
        return false;
    }
    int tmin; int tmax;
    bool tinclude_disabled;
    bool okay = split_line.IIntVal(0, tmin);
    okay = okay && split_line.IIntVal(1, tmax);
    okay = okay && split_line.BoolVal(2, tinclude_disabled);
    if (!okay) {
        error_out = "One of the fields was invalid!";
        return false;
    }
    min = tmin;
    max = tmax;
    include_disabled = tinclude_disabled;
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFimThumbsFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  const jfFIM_Fanfic* rvalue;
  int tnet;
  // checks
  assert(testelem!=NULL);
  // determining the type
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  if (rvalue->AreRatingsDisabled()) {
      /**/JDEBUGLOGB("jfFimThumbsFilter::CoreMatch",1,include_disabled)
      return include_disabled;
  }
  // getting net thumbs up
  tnet = (rvalue->GetThumbs(false)) - (rvalue->GetThumbs(true));
  // checking the FimThumbs
  assert(tnet<=999999999);
  if (tnet<min) return false;
  else return (tnet<=max);
}

//=======================================================================
const jfFilterTypeMeta FIM_GENRE_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::GENRE, "FIMGenreFilter", "FIM Genres Filter",
          QString("Compares the Fimfiction.net genres of the fic against the ") +
                  "include/exclude/alternate list specified by the filter.",
          IdForFIMFanfics(), createFilter<jfFIMGenreFilter> );
// ++++++++++++++++++++++++++++++++++++++++++++++++
jfFIMGenreFilter::jfFIMGenreFilter(const QString& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
// -------------------------------
jfFIMGenreFilter::jfFIMGenreFilter(QString&& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
//---------------------------------------------
jfFIMGenreFilter::jfFIMGenreFilter(const jfFIMGenreFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
  bool okay = DoVerifyCheck(thedata);
  jerror::AssertLog(okay, "jfFIMGenreFilter", "Verification failed in the constructor");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
bool jfFIMGenreFilter::SetToEmpty() {
    if (fimcon::tags == NULL) return false;
    if (thedata == NULL) thedata = new jfTagListing();
    fimcon::tags->SetTagsToEmpty(*thedata,FIMT_genre);
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFIMGenreFilter::GetTypeMetaInfo() const {
    return FIM_GENRE_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterBase* jfFIMGenreFilter::GenCopy() const {
  return new jfFIMGenreFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMGenreFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfFIM_Fanfic* rvalue;
  // we start
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  return MatchVsData(rvalue->GetGenres());
}
//-----------------------------------------------
bool jfFIMGenreFilter::ModifyList(QStringList* templist) const {
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check the tags against a list of pre-approved tags
bool jfFIMGenreFilter::DoVerify() {
    if (fimcon::tags == NULL) return false;
    QStringList* genrelist = fimcon::tags->namesForType(FIMT_genre);
    if (genrelist == NULL) return false;
    bool result = VerifyTags(genrelist,NULL);
    delete genrelist;
    return result;
}
// -----------------------------------------------------
bool jfFIMGenreFilter::DoVerifyCheck(jfTagListing* to_check) const {
    if (fimcon::tags == NULL) return false;
    QStringList* genrelist = fimcon::tags->namesForType(FIMT_genre);
    if (genrelist == NULL) return false;
    bool result = VerifyCheck(to_check, genrelist, NULL);
    delete genrelist;
    return result;
}
//=======================================================================
QStringList* MakeFIMCharRep() {
  QStringList* newrep = new QStringList();
  newrep->append("DJ P0N-3");
  newrep->append("Vinyl Scratch");
  return newrep;
}
//=====================================================================
const jfFilterTypeMeta FIM_CHARACTER_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::CHARACTERS, "FIMCharacterFilter",
                     "FIM Character Filter",
          QString("Compares the Fimfiction.net Characters of the fic against") +
                  " the include/exclude/alternate list specified by the filter.",
          IdForFIMFanfics(), createFilter<jfFIMCharacterFilter> );
// ++++++++++++++++++++++++++++++++++++++++++++++++
jfFIMCharacterFilter::jfFIMCharacterFilter(const QString& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
//---------------------
jfFIMCharacterFilter::jfFIMCharacterFilter(QString&& filter_name):jfTagFilterCore(filter_name) {
    cm = ",";
}
//---------------------------------------------
jfFIMCharacterFilter::jfFIMCharacterFilter(const jfFIMCharacterFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
  bool okay = DoVerify();
  jerror::AssertLog(okay, "jfFIMCharacterFilter", "Verification failed in the constructor");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
bool jfFIMCharacterFilter::SetToEmpty() {
    if (fimcon::tags == NULL) return false;
    if (thedata == NULL) thedata = new jfTagListing();
    fimcon::tags->SetTagsToEmpty(*thedata,FIMT_character);
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFIMCharacterFilter::GetTypeMetaInfo() const {
    return FIM_CHARACTER_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterBase* jfFIMCharacterFilter::GenCopy() const {
  return new jfFIMCharacterFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMCharacterFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfFIM_Fanfic* rvalue;
  // we start
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  return MatchVsData(rvalue->GetCharacterList().join(","));
}
//------------------------------------------------------
bool jfFIMCharacterFilter::ModifyList(QStringList* templist) const {
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check the tags against a list of pre-approved tags
bool jfFIMCharacterFilter::DoVerify() {
    if (fimcon::tags == NULL) return false;
    QStringList* charlist = fimcon::tags->namesForType(FIMT_character);
    if (charlist == NULL) return false;
    QStringList* newrep = MakeFIMCharRep();  // this might need checking
    bool rval = VerifyTags(charlist,newrep);
    delete charlist;
    delete newrep;
    return rval;
}
//--------------------------------------------------
bool jfFIMCharacterFilter::DoVerifyCheck(jfTagListing* to_check) const {
    if (fimcon::tags == NULL) return false;
    QStringList* charlist = fimcon::tags->namesForType(FIMT_character);
    if (charlist == NULL) return false;
    QStringList* newrep = MakeFIMCharRep();  // this might need checking
    bool rval = VerifyCheck(to_check, charlist, newrep);
    delete charlist;
    delete newrep;
    return rval;
}
//******************************************************************************
const jfFilterTypeMeta FIM_RATING_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::CATEGORY, "FimRatingFilter",
                     "FIM Rating Filter",
          QString("Filters items depending on their rating. The default is to") +
                  "include all ratings.",
          IdForFIMFanfics(), createFilter<jfFimRatingFilter> );
// ---------------------------------
const QString FIM_RATINGS = "ETM";
const QRegularExpression NOT_FIM_RATINGS = QRegularExpression("[^"+FIM_RATINGS+"]");
//============================================================================
// constructors
//--------------------------------------------------
jfFimRatingFilter::jfFimRatingFilter(const QString& filter_name):jfFilterBase(filter_name) {
    value = FIM_RATINGS;
}
// -------------------
jfFimRatingFilter::jfFimRatingFilter(QString&& filter_name):jfFilterBase(filter_name) {
    value = FIM_RATINGS;
}
//--------------------------------------------------
jfFimRatingFilter::jfFimRatingFilter(const QString& filter_name, QString value_in):jfFilterBase(filter_name) {
  FromString(value_in);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter, note this currently dependant on fimfiction.net
bool jfFimRatingFilter::IsEmpty() const {
  return (FIM_RATINGS==value);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFimRatingFilter::ToString() const {
  return value;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfFimRatingFilter::GetTypeMetaInfo() const {
    return FIM_RATING_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfFilterBase* jfFimRatingFilter::GenCopy() const {
  jfFimRatingFilter* result;
  result = new jfFimRatingFilter(name);
  result->description = description;
  result->FromString(value);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFimRatingFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    if (sourcedata.contains(NOT_FIM_RATINGS)) {
        error_out = "The input contains invalid characters";
        return false;
    }
    value = sourcedata;
    return true;
}
//------------------------------------------------------------
// the core matching method
bool jfFimRatingFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfFIM_Fanfic* rvalue;
  QString cvalue;
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  cvalue = rvalue->GetRating();
  return (value.contains(cvalue[0]));
}
//--------------------------------------------------------------
// custom methods
bool jfFimRatingFilter::Includes(QString test) const {
  return (value.contains(test[0]));
}

//******************************************************************************
const jfFilterTypeMeta FIM_PERCENT_RATING_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::CATEGORY, "FimThumbPercentFilter",
                     "FIM Percentage of Thumbs Up Filter",
          QString("Filters items depending on what percentage of the sum ") +
                  "of thumbs up and thumbs down is thumbs up. The value is " +
                  "the minimum percentage accepeted.",
          IdForFIMFanfics(), createFilter<jfFimThumbPercentFilter> );

// constructors
//--------------------------------------------------
jfFimThumbPercentFilter::jfFimThumbPercentFilter(const QString& filter_name):jfFilterBase(filter_name) {
    value = 0;
    include_disabled = false;
}
// -------------------------------------------
jfFimThumbPercentFilter::jfFimThumbPercentFilter(QString&& filter_name):jfFilterBase(filter_name) {
    value = 0;
    include_disabled = false;
}

//--------------------------------------------------
jfFimThumbPercentFilter::jfFimThumbPercentFilter(const QString& filter_name, QString value_in):jfFilterBase(filter_name) {
    QString error;
    bool okay = FromStringInner(value_in, error);
    jerror::AssertLog(okay, "jfFimThumbPercentFilter", "The input is not a valid FIM thumbs percent filter!" );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter, note this currently dependant on fimfiction.net
bool jfFimThumbPercentFilter::IsEmpty() const {
    return ((0==value) && include_disabled);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFimThumbPercentFilter::ToString() const {
    jfOutString result;
    return (result << value << include_disabled);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFimThumbPercentFilter::SetPercent(size_t invalue) {
  if (invalue>100) return false;
  value = invalue;
  return true;
}
//---------------------------------------------------
void jfFimThumbPercentFilter::SetIncludeDisabled(bool yes) {
    include_disabled = yes;
}
//---------------------------------------------------
size_t jfFimThumbPercentFilter::GetPercent() const {
  return value;
}
//---------------------------------------------------
bool jfFimThumbPercentFilter::GetIncludeDisabled() const {
    return include_disabled;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets a description
const jfFilterTypeMeta& jfFimThumbPercentFilter::GetTypeMetaInfo() const {
    return FIM_PERCENT_RATING_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfFilterBase* jfFimThumbPercentFilter::GenCopy() const {
  jfFimThumbPercentFilter* result = new jfFimThumbPercentFilter(name);
  result->description = description;
  result->SetPercent(value);
  result->SetIncludeDisabled(include_disabled);
  return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFimThumbPercentFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    jfLineParse* lparser;
    QString oerr = "";
    lparser = new jfLineParse(sourcedata);
    //validdata = false;
    //include_disabled = false;
    size_t lnum = lparser->Num();
    if ((lnum < 1) || (lnum > 2)) {
        error_out = "Too Many Fields!";
        delete lparser;
        return false;
    }
    else {
        size_t temp_value;
        bool okay = false;
        if (lparser->SBoundVal(0,100,temp_value,oerr)) {
            if (lnum == 1) {
                value = temp_value;
                include_disabled = false;
                okay = true;
            }
            else if (lparser->BoolVal(1, include_disabled)){
                value = temp_value;
                okay = true;
            }
            else {
                error_out = "The include disabled field could not be parsed";
            }
        }
        else {
            error_out = "The percent value is not in range!";
        }
        delete lparser;
        return okay;
    }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFimThumbPercentFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // constants and checks
  const jfFIM_Fanfic* rvalue;
  size_t cvalue1,cvalue2;
  float compvalue;
  assert(testelem!=NULL);
  // getting values
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  // testing for Ratings Disabled
  if (rvalue->AreRatingsDisabled()) {
      /**/JDEBUGLOGB("jfFimThumbPercentFilter::CoreMatch",1,include_disabled)
      return include_disabled;
  }
  // more general test
  cvalue1 = rvalue->GetThumbs(false);
  cvalue2 = rvalue->GetThumbs(true);
  compvalue = ((float)cvalue1 + (float)cvalue2);
  compvalue = (cvalue1/compvalue)*100;
  // doing the result
  return (compvalue>=value);
}

/*****************************************************************************/
