/******************************************************************************
Name    :   fimfilters1.cpp
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 4, 2012
Conversion to QT Started September 30, 2013
Updated :   October 11, 2019
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
jfFimThumbsFilter::jfFimThumbsFilter():jfBaseFilter() {
  validdata = false;
  min = -99999;
  max =  99999;
  include_disabled = false;
}
//---------------------------------------------------
jfFimThumbsFilter::jfFimThumbsFilter(int inmin, int inmax):jfBaseFilter() {
  validdata = SetValues(inmin,inmax);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting values
bool jfFimThumbsFilter::SetValues(int inmin, int inmax, bool inc_disabled) {
  if (inmax>=inmin) {
    min = inmin;
    max = inmax;
    include_disabled = inc_disabled;
    validdata = true;
  }
  else validdata = false;
  return validdata;
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
bool jfFimThumbsFilter::isEmpty() const {
    if (!include_disabled) return false;
    return ((min == INT_MIN) && (max == INT_MAX));
}
//----------------------------------------------------------------------
QString jfFimThumbsFilter::GetTypeID() const {
  return "FimThumbsFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//---------------------------------------------------
bool jfFimThumbsFilter::FromString(const QString& sourcedata) {
  jfLineParse* lparser;
  lparser = new jfLineParse(sourcedata);
  validdata = false;
  include_disabled = false;
  size_t lnum = lparser->Num();
  if ((lnum < 2) || (lnum > 3)) delete lparser;
  else {
    if ((lparser->IIntVal(0,min)) && (lparser->IIntVal(1,max))) {
        if (lnum == 2) validdata = true;
        else validdata = lparser->BoolVal(2,include_disabled);
    }
    delete lparser;
  }
  return validdata;
}
//---------------------------------------------------
QString jfFimThumbsFilter::ToString() const {
  jfOutString result;
  result << min << max << include_disabled;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------------
QString jfFimThumbsFilter::GetTypeDescription() const {
    return "Fimfiction.net fics have thumbs up and thumbs down rating.\
This filter passes fics for which the thumbsup-thumbsdown lies between the min and max.";
}
//---------------------------------------------------
jfBaseFilter* jfFimThumbsFilter::GenCopy() const {
  jfFimThumbsFilter* result;
  result = new jfFimThumbsFilter(min,max);
  result->include_disabled = include_disabled;
  CopyOver(result);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFimThumbsFilter::CoreMatch(const jfBasePD* testelem) const {
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//-----------------------------------
bool jfFimThumbsFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  buffer = ToString();
  (*outfile) << buffer << "\n";
  return true;
}
//-----------------------------------
bool jfFimThumbsFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFimThumbsFilter::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadLine(cline,funcname)) return false;
  // there is only one line, so this is pretty simple
  resx = FromString(cline);
  if (!resx) return infile->BuildError("The min-max string is invalid!");
  else return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* since different types are stored together, the text file reprentation
may have objects of varying length */
size_t jfFimThumbsFilter::ExtraLines() const { return 1;}
//=======================================================================
jfFIMGenreFilter::jfFIMGenreFilter():jfTagFilterCore() {
  cm = ",";
}
//---------------------------------------------
jfFIMGenreFilter::jfFIMGenreFilter(const jfFIMGenreFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
  validdata = DoVerify();
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
// redefined virtual methods
QString jfFIMGenreFilter::GetTypeID() const {
  return "FIMGenreFilter";
}
//-----------------------------------------------------------------------------
QString jfFIMGenreFilter::GetTypeDescription() const {
    return "Compares the Fimfiction.net genres of the fic against the \
include/exclude/alternate list specified by the filter.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfBaseFilter* jfFIMGenreFilter::GenCopy() const {
  return new jfFIMGenreFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMGenreFilter::CoreMatch(const jfBasePD* testelem) const {
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
//=======================================================================
QStringList* MakeFIMCharRep() {
  QStringList* newrep = new QStringList();
  newrep->append("DJ P0N-3");
  newrep->append("Vinyl Scratch");
  return newrep;
}
//=====================================================================
jfFIMCharacterFilter::jfFIMCharacterFilter():jfTagFilterCore() {
  cm = ",";
}
//---------------------------------------------
jfFIMCharacterFilter::jfFIMCharacterFilter(const jfFIMCharacterFilter& insrc):jfTagFilterCore(insrc) {
  cm = ",";
  validdata = DoVerify();
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
// redefined virtual methods
QString jfFIMCharacterFilter::GetTypeID() const {
  return "FIMCharacterFilter";
}
//-----------------------------------------------------------------------------
QString jfFIMCharacterFilter::GetTypeDescription() const {
    return "Compares the Fimfiction.net Characters of the fic against the \
include/exclude/alternate list specified by the filter.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfBaseFilter* jfFIMCharacterFilter::GenCopy() const {
  return new jfFIMCharacterFilter(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFIMCharacterFilter::CoreMatch(const jfBasePD* testelem) const {
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
//******************************************************************************
const QString FIM_RATINGS = "ETM";
//============================================================================
// constructors
//--------------------------------------------------
jfFimRatingFilter::jfFimRatingFilter() {
  value = FIM_RATINGS;
}
//--------------------------------------------------
jfFimRatingFilter::jfFimRatingFilter(QString value_in) {
  FromString(value_in);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter, note this currently dependant on fimfiction.net
bool jfFimRatingFilter::isEmpty() const {
  return (FIM_RATINGS==value);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//--------------------------------------------------
bool jfFimRatingFilter::FromString(const QString& sourcedata) {
  QRegExp tval = QRegExp("[^"+FIM_RATINGS+"]");
  value = sourcedata;
  return (!sourcedata.contains(tval));
}
//--------------------------------------------------
QString jfFimRatingFilter::ToString() const {
  return value;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets a description
QString jfFimRatingFilter::GetTypeDescription() const {
    return "Filters items depending on their rating. The default is to \
include all ratings.";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfBaseFilter* jfFimRatingFilter::GenCopy() const {
  jfFimRatingFilter* result;
  result = new jfFimRatingFilter();
  CopyOver(result);
  result->FromString(value);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfFimRatingFilter::GetTypeID() const {
  return "FimRatingFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFimRatingFilter::CoreMatch(const jfBasePD* testelem) const {
  const jfFIM_Fanfic* rvalue;
  QString cvalue;
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfFIM_Fanfic*>(testelem);
  cvalue = rvalue->GetRating();
  return (value.contains(cvalue[0]));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//--------------------------------------------------
bool jfFimRatingFilter::AddRestToFile(QTextStream* outfile) const {
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  (*outfile) << value << "\n";
  return true;
}
//============================================================================


//--------------------------------------------------------------
// custom methods
bool jfFimRatingFilter::Includes(QString test) const {
  return (value.contains(test[0]));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//--------------------------------------------------

//--------------------------------------------------
bool jfFimRatingFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFimRatingFilter::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadLine(cline,funcname)) return false;
  // there is only one line, and one filed, so this is pretty simple
  resx = FromString(cline);
  if (!resx) return infile->BuildError("The completed string is invalid!");
  else return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* since different types are stored together, the text file reprentation
may have objects of varying length */
size_t jfFimRatingFilter::ExtraLines() const {return 1;}
//******************************************************************************
// constructors
//--------------------------------------------------
jfFimThumbPercentFilter::jfFimThumbPercentFilter() {
  value = 0;
  include_disabled = false;
}
//--------------------------------------------------
jfFimThumbPercentFilter::jfFimThumbPercentFilter(QString value_in) {
  FromString(value_in);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter, note this currently dependant on fimfiction.net
bool jfFimThumbPercentFilter::isEmpty() const {
    return ((0==value) && include_disabled);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//--------------------------------------------------
bool jfFimThumbPercentFilter::FromString(const QString& sourcedata) {
    jfLineParse* lparser;
    QString oerr = "";
    lparser = new jfLineParse(sourcedata);
    validdata = false;
    include_disabled = false;
    size_t lnum = lparser->Num();
    if ((lnum < 1) || (lnum > 2)) delete lparser;
    else {
      if (lparser->SBoundVal(0,100,value,oerr)) {
          if (lnum == 1) validdata = true;
          else validdata = lparser->BoolVal(1,include_disabled);
      }
      delete lparser;
    }
    return validdata;
}
//--------------------------------------------------
QString jfFimThumbPercentFilter::ToString() const {
    jfOutString result;
    return (result << value << include_disabled);
}
//---------------------------------------------------
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
QString jfFimThumbPercentFilter::GetTypeDescription() const {
    return "Filters items depending on what percentage of the sum\
of thumbs up and thumbs down is thumbs up. The value is the minimum\
percentage accepeted.";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfBaseFilter* jfFimThumbPercentFilter::GenCopy() const {
  jfFimThumbPercentFilter* result;
  result = new jfFimThumbPercentFilter();
  CopyOver(result);
  result->SetPercent(value);
  result->SetIncludeDisabled(include_disabled);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfFimThumbPercentFilter::GetTypeID() const {
  return "FimThumbPercentFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFimThumbPercentFilter::CoreMatch(const jfBasePD* testelem) const {
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//--------------------------------------------------
bool jfFimThumbPercentFilter::AddRestToFile(QTextStream* outfile) const {
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  (*outfile) << ToString() << "\n";
  return true;
}
//--------------------------------------------------
bool jfFimThumbPercentFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFimThumbPercentFilter::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadLine(cline,funcname)) return false;
  // there is only one line, and one filed, so this is pretty simple
  resx = FromString(cline);
  if (!resx) return infile->BuildError("The percentage string is invalid!");
  else return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* since different types are stored together, the text file reprentation
may have objects of varying length */
size_t jfFimThumbPercentFilter::ExtraLines() const {return 1;}
/*****************************************************************************/
