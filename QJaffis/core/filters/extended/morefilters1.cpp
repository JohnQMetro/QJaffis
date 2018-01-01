/******************************************************************************
Name    :   morefilters1.cpp
Author  :   John Q Metro
Purpose :   More Filters
Created :   December 6, 2010
Conversion to QT Started September 30, 2013
Updated :   July 8, 2016 (removed minmax and wordcount, added completion)
******************************************************************************/
#ifndef MOREFILTERS1_H_INCLUDED
  #include "morefilters1.h"
#endif
//------------------------------------------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FICBASE_H
  #include "../../../fanfic/data/ficbase.h"
#endif // FICBASE_H

//----------------------------------------------
#include <assert.h>
/******************************************************************************/

// constructors
jfTagFilterCore::jfTagFilterCore():jfBaseFilter() {
  thedata = NULL;
  validdata = false;
  nullresult = true;
  emptyc = 0;
}
//-----------------------------------------------------------------------
jfTagFilterCore::jfTagFilterCore(const jfTagFilterCore& insrc) {
  insrc.CopyOver(this);
  if (validdata==false) thedata = NULL;
  else {
    thedata = new jfTagListing(*insrc.thedata);
  }
  nullresult = insrc.nullresult;
  emptyc = insrc.emptyc;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
//-----------------------------------
bool jfTagFilterCore::SetTags(const jfTagListing* insource) {
  // constants
  const QString fname = "jfTagFilterCore::SetTags";
  // special cases
  /**/JDEBUGLOG(fname,1)
  if (insource==NULL) return false;
  // clearing the old one
  if (thedata != NULL) delete thedata;
  // getting the new one
  thedata = new jfTagListing(*insource);
  /**/JDEBUGLOG(fname,2)
  validdata = DoVerify();
  /**/JDEBUGLOGB(fname,3,validdata)
  // we set some flags
  isEmpty();
  MakeNullResult();
  // done
  return validdata;

}
//-----------------------------------
jfTagListing* jfTagFilterCore::GetTagsCopy() const {
  if (thedata == NULL) return NULL;
  return new jfTagListing(*thedata);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual methods
bool jfTagFilterCore::isEmpty() const {
  // variables
  bool result = true;
  jfTagListing::const_iterator iter;
  // special case
  if (thedata == NULL ) return false;
  // main checking
  iter = thedata->begin();
  while (result) {
    result = ((iter->second)==jfts_NONE);
    iter++;
    if (iter==(thedata->end())) break;
  }
  emptyc = (result)?(1):(2);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//-----------------------------------
bool jfTagFilterCore::FromString(const QString& sourcedata) {
  // local variables
  jfTagListing* newvalue;
  bool conversult;
  // we create and convert
  newvalue = new jfTagListing();
  conversult = newvalue->FromString(sourcedata);
  // the results
  if (conversult) {
    // success!
    if (thedata != NULL) delete thedata;
    thedata = newvalue;
    validdata = DoVerify();
    // we set some flags
    isEmpty();
    MakeNullResult();
    // done
    return validdata;
  }
  else {
    delete newvalue;
    return false;
  }
}
//-----------------------------------
QString jfTagFilterCore::ToString() const {
  if (thedata==NULL) return "";
  else return thedata->ToString();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfTagFilterCore::MatchVsData(const QString& incheck) const {
  // constants
  const QString fname = "jfTagFilterCore::MatchVsData";
  // local variables
  QString     tag;
  jfTAG_STATUS todo;
  jfTagListing::const_iterator iter,enditer;
  QStringList* delimlist;
  size_t altercount,altermatch;
  bool isfound;
  // special case 1: the filter is actually empty
  assert(emptyc!=0);
  if (emptyc==1) return true;
  // parsing the inout to check against
  delimlist = ListToStrArr(incheck,cm[0]);
  if ((delimlist==NULL)) return nullresult;
  else if (delimlist->isEmpty()) {
    delete delimlist;
    return nullresult;
  }
  ModifyList(delimlist);
  // initializing some values
  iter = thedata->begin();
  enditer = thedata->end();
  altercount = 0;
  altermatch = 0;
  // the main loop
  while (iter!=enditer) {
    todo = iter->second;
    if (todo!=jfts_NONE) {
      // we get some info
      tag = iter->first;
      // search
      isfound = delimlist->contains(tag);
      // the results
      if (todo==jfts_INCLUDE) {
        if (!isfound) {
          delete delimlist;
          return false;
        }
      }
      else if (todo==jfts_EXCLUDE) {
        if (isfound) {
          delete delimlist;
          return false;
        }
      }
      else {
        altercount++;
        if (isfound) altermatch++;
      }
    }
    iter++;
  }
  // if we get past the loop, things are correct unless there is
  // no match for the alternates
  delete delimlist;
  if (altercount>0) return (altermatch>0);
  else return true;
}
//---------------------------------------------------------------------
// calculates the result if what we check against is empty
void jfTagFilterCore::MakeNullResult() {
  // variables
  jfTAG_STATUS todo;
  jfTagListing::const_iterator iter,enditer;
  // special case
  if (emptyc==1) nullresult = true;
  // more general case
  else {
    nullresult = true;
    iter = thedata->begin();
    enditer = thedata->end();
    while (iter!=enditer) {
      todo = iter->second;
      if ((todo==jfts_INCLUDE) || (todo==jfts_ALTERNATE)) {
        nullresult = false;
        break;
      }
      iter++;
    }
  }
  // done
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check the tags against a list of pre-approved tags
/* all tags in the filter must be in intags, but tags in intags not
in the list will be added, no error */
bool jfTagFilterCore::VerifyTags(size_t NUMTAGIN, const QString intags[], QStringList* replist) {
  // constants
  const QString fname = "jfTagFilterCore::VerifyTags";
  // variables
  size_t tcount, tfoundc, tnfound, tloop;
  QString ctag;
  bool tfound;
  // checks
  /**/JDEBUGLOG(fname,1)
  jfAssertLog(intags!=NULL,fname,"input is NULL");
  if (thedata==NULL) return false;
  /**/JDEBUGLOG(fname,2)
  // we start
  tcount = thedata->size();
  tfoundc = 0;
  tnfound = 0;
  /**/JDEBUGLOGST(fname,3,NUMTAGIN)
  // the replacement list
  if (replist!=NULL) {
      thedata->ReplaceTags(replist);
  }
  // the loop
  for (tloop=0; tloop<NUMTAGIN; tloop++) {
    ctag = QString(intags[tloop]);
    tfound = ((thedata->end())!=(thedata->find(ctag)));
    if (tfound) tfoundc++;
    else {
      tnfound++;
      (*thedata)[ctag] = jfts_NONE;
    }
  }
  // the end of the list, we check now whether things worked out okay
  /**/JDEBUGLOGST(fname,4,tcount)
  /**/JDEBUGLOGST(fname,5,tfoundc)
  return (tcount == tfoundc);
}
//----------------------------------
bool jfTagFilterCore::VerifyTags(const QStringList* sourcelist, QStringList* replist) {
  // variables
  size_t tcount, tfoundc, tnfound, tloop;
  QString ctag;
  bool tfound;
  // checks
  assert(sourcelist != NULL);
  if (thedata==NULL) return false;
  // we start
  tcount = thedata->size();
  tfoundc = 0;
  tnfound = 0;
  // the replacement list
  if (replist!=NULL) {
      thedata->ReplaceTags(replist);
  }
  // the loop
  for (tloop=0 ; tloop<(sourcelist->size()) ; tloop++) {
    ctag = sourcelist->at(tloop);
    tfound = ((thedata->end())!=(thedata->find(ctag)));
    if (tfound) tfoundc++;
    else {
      tnfound++;
      (*thedata)[ctag] = jfts_NONE;
    }
  }
  return (tcount == tfoundc);
}

//-----------------------------------
void jfTagFilterCore::SetEmpty(size_t NUMTAGIN, const QString intags[]) {
  // variables
  size_t loopc;
  QString tag;
  // starting checks
  assert(intags!=NULL);
  if (thedata == NULL) thedata = new jfTagListing();
  else thedata->clear();
  // loop to add items
  for (loopc=0;loopc<NUMTAGIN;loopc++) {
    tag = QString(intags[loopc]);
    (*thedata)[tag] = jfts_NONE;
  }
  validdata = true;
  emptyc = 1;
  nullresult = true;
}
//------------------------------------------------------------
void jfTagFilterCore::SetEmpty(const QStringList* sourcelist) {
  // variables
  size_t loopc;
  QString tag;
  // starting checks
  assert(sourcelist!=NULL);
  if (thedata == NULL) thedata = new jfTagListing();
  else thedata->clear();
  // loop to add items
  for ( loopc = 0 ; loopc < sourcelist->size() ; loopc++ ) {
    tag = QString(sourcelist->at(loopc));
    (*thedata)[tag] = jfts_NONE;
  }
  validdata = true;
  emptyc = 1;
  nullresult = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//-----------------------------------
bool jfTagFilterCore::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  buffer = ToString();
  (*outfile) << buffer << "\n";
  return true;
}
//-----------------------------------
bool jfTagFilterCore::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfExpressionFilter::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadLine(cline,fname)) return false;
  // there is only one line, and one filed, so this is pretty simple
  resx = FromString(cline);
  /**/JDEBUGLOGB(fname,3,resx)
  if (!resx) return infile->BuildError("The tag string is invalid!");
  resx = DoVerify();
  /**/JDEBUGLOGB(fname,4,resx)
  if (!resx) return infile->BuildError("The tags do not match what is defined for this type!");
  isEmpty();
  MakeNullResult();
  /**/JDEBUGLOG(fname,4)
  return true;
}
//========================================================================
// constructors
//-----------------------------------------------------
jfCompletedFilter::jfCompletedFilter():jfBaseFilter() {
  valset = false;
  validdata = true;
}
//-----------------------------------------------------
jfCompletedFilter::jfCompletedFilter(bool incexl_in):jfBaseFilter() {
  SetValue(incexl_in);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter
bool jfCompletedFilter::isEmpty() const {
  return !valset;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//-----------------------------------------------------
bool jfCompletedFilter::FromString(const QString& sourcedata) {
  jfLineParse* lparser;
  lparser = new jfLineParse(sourcedata);
  validdata = false;
  if (lparser->NNotX(2)) delete lparser;
  else {
    if ((lparser->BoolVal(0,valset)) && (lparser->BoolVal(1,incexl))) validdata = true;
    delete lparser;
  }
  return validdata;
}
//-----------------------------------------------------
QString jfCompletedFilter::ToString() const {
  QString result = Bool2Str(valset) + ";";
  result += Bool2Str(incexl);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data setting
//-----------------------------------------------------
void jfCompletedFilter::SetValue(bool incexl_in) {
  valset = true;
  incexl = incexl_in;
}
//-----------------------------------------------------
bool jfCompletedFilter::GetValue() const {
  return incexl;
}
//----------------------------------------------------------------
void jfCompletedFilter::SetEmpty() {
  valset = false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets a description
QString jfCompletedFilter::GetTypeDescription() const {
    return "For items that have a completed flag, this filter passes if \
the filter is empty, or if the flag is either set or not (depending on the \
value).";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfBaseFilter* jfCompletedFilter::GenCopy() const {
  jfCompletedFilter* result;
  result = new jfCompletedFilter();
  CopyOver(result);
  if (valset) result->SetValue(incexl);
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfCompletedFilter::GetTypeID() const {
  return "CompletedFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfCompletedFilter::CoreMatch(const jfBasePD* testelem) const {
  const jfGenericFanfic2* rvalue;
  bool cvalue;
  assert(testelem!=NULL);
  rvalue = dynamic_cast<const jfGenericFanfic2*>(testelem);
  cvalue = rvalue->IsComplete();
  return (incexl==cvalue);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//-----------------------------------------------------
bool jfCompletedFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  buffer = ToString();
  (*outfile) << buffer << "\n";
  return true;
}
//-----------------------------------------------------
bool jfCompletedFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfCompletedFilter::ReadRestFromFile";
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* since different types are stored together, the text file reprentation may
have objects of varying length */
size_t jfCompletedFilter::ExtraLines() const { return 1;}

/******************************************************************************/
