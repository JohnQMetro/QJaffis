/******************************************************************************
Name    :   ffn_special1.cpp
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016
Updated :   July 29, 2016
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

#include <assert.h>
/*****************************************************************************/
/* since different types are stored together, the text file reprentation
may have objects of varying length */
size_t jfTagFilterCore::ExtraLines() const { return 1;}
//==========================================================================

jfFFNGenresFilter::jfFFNGenresFilter():jfTagFilterCore() {
  cm = "/";
}
//---------------------------------------------
jfFFNGenresFilter::jfFFNGenresFilter(const jfFFNGenresFilter& insrc):jfTagFilterCore(insrc) {
  cm = "/";
  validdata = DoVerify();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
bool jfFFNGenresFilter::SetToEmpty() {
  SetEmpty(ffn_consts::taggen_list);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual methods
QString jfFFNGenresFilter::GetTypeID() const {
  return "FFNTagFilter";
}
//-----------------------------------------------------------------------------
QString jfFFNGenresFilter::GetTypeDescription() const {
    return "Compares the Fanfiction.Net genres of the fic against the \
include/exclude/alternate list specified by the filter.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfBaseFilter* jfFFNGenresFilter::GenCopy() const {
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
//======================================================================
const QString FFN_RATINGS = "K+TM";
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//--------------------------------------------------
jfFFNRatingFilter::jfFFNRatingFilter() {
  value = FFN_RATINGS;
}
//--------------------------------------------------
jfFFNRatingFilter::jfFFNRatingFilter(QString value_in) {
  FromString(value_in);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter, note this currently dependant on fanfiction.net
bool jfFFNRatingFilter::isEmpty() const {
  return (FFN_RATINGS==value);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//--------------------------------------------------
bool jfFFNRatingFilter::FromString(const QString& sourcedata) {
  QRegExp xrg = QRegExp("[^"+FFN_RATINGS+"]");
  value = sourcedata;
  return (!sourcedata.contains(xrg));
}
//--------------------------------------------------
QString jfFFNRatingFilter::ToString() const {
  return value;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets a description
QString jfFFNRatingFilter::GetTypeDescription() const {
    return "Filters items depending on their rating. The default is to \
include all ratings.";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfBaseFilter* jfFFNRatingFilter::GenCopy() const {
  jfFFNRatingFilter* result;
  result = new jfFFNRatingFilter();
  CopyOver(result);
  result->FromString(value);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfFFNRatingFilter::GetTypeID() const {
  return "FFNRatingFilter";
}
//--------------------------------------------------------------
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
// file i/o
//--------------------------------------------------
bool jfFFNRatingFilter::AddRestToFile(QTextStream* outfile) const {
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  (*outfile) << value << "\n";
  return true;
}
//--------------------------------------------------
bool jfFFNRatingFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFNRatingFilter::ReadRestFromFile";
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
size_t jfFFNRatingFilter::ExtraLines() const {return 1;}
//================================================================
// FFN Favs Filter
// constructors
//---------------------------------------------------
jfFFNFavsFilter::jfFFNFavsFilter():jfMinMaxUFilter() {}
//---------------------------------------------------
jfFFNFavsFilter::jfFFNFavsFilter(size_t inmin, size_t inmax):jfMinMaxUFilter(inmin,inmax) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNFavsFilter::GetTypeID() const {
  return "FFNFavsFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------------
QString jfFFNFavsFilter::GetTypeDescription() const {
    return "For Fanfiction.Net fics, this filter passes if \
the number of Favs is between min and max, inclusive.";
}
//---------------------------------------------------
jfBaseFilter* jfFFNFavsFilter::GenCopy() const {
  jfFFNFavsFilter* result;
  result = new jfFFNFavsFilter(min,max);
  CopyOver(result);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFFNFavsFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  const jfFFNItemCore* rvalue;
  size_t cvalue;
  // checks
  assert(testelem!=NULL);
  // determining the type
  rvalue = dynamic_cast<const jfFFNItemCore*>(testelem);
  cvalue = rvalue->GetFavs();
  // checking the wordcount
  return TestMatch(cvalue);
}
//===========================================================================================
// constructors
//----------------------------------------------------------
jfFFN_CategoryExprFilter::jfFFN_CategoryExprFilter():jfSimpleExpFilterCore() {}
//----------------------------------------------------------
jfFFN_CategoryExprFilter::jfFFN_CategoryExprFilter(const jfFFN_CategoryExprFilter& source) {
  CoreCopy(source);
}
//----------------------------------------------------------
jfFFN_CategoryExprFilter::jfFFN_CategoryExprFilter(jfSimpleExpr* in_source):jfSimpleExpFilterCore(in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFN_CategoryExprFilter::GetTypeDescription() const {
  return "The Category Expression Filter matches the category name against a boolean \
expression, the elements of which are themselves to be matched. These elements \
are either strings or substrings.";
}
//------------------------------------------------------------
jfFFN_CategoryExprFilter* jfFFN_CategoryExprFilter::Copy() const {
  return new jfFFN_CategoryExprFilter(*this);
}
//------------------------------------------------------------
jfBaseFilter* jfFFN_CategoryExprFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
QString jfFFN_CategoryExprFilter::GetTypeID() const {
  return "CategoryFilter";
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

