/******************************************************************************
Name    :   morefilters1.cpp
Author  :   John Q Metro
Purpose :   More Filters
Created :   December 6, 2010
Conversion to QT Started September 30, 2013
Updated :   April 14, 2023 (rebasing)
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
jfTagFilterCore::jfTagFilterCore(const QString& filter_name):jfFilterBase(filter_name) {
    thedata = NULL;
    nullresult = true;
    empty_state = jfTagFilterEmpty::UNKNOWN;
}
// -----------------------------------------
jfTagFilterCore::jfTagFilterCore(QString&& filter_name):jfFilterBase(filter_name) {
    thedata = NULL;
    nullresult = true;
    empty_state = jfTagFilterEmpty::UNKNOWN;
}
// -----------------------------------------
jfTagFilterCore::jfTagFilterCore(const jfTagFilterCore& insrc):jfFilterBase(insrc.name) {
    description = insrc.description;
    if (insrc.thedata == NULL) thedata = NULL;
    else thedata = new jfTagListing(*insrc.thedata);
    nullresult = insrc.nullresult;
    empty_state = insrc.empty_state;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting and setting values
//-----------------------------------
bool jfTagFilterCore::SetTags(const jfTagListing* insource) {
  // constants
  const QString fname = "jfTagFilterCore::SetTags";
  // special cases
  /**/JDEBUGLOG(fname,1)
  if (insource == NULL) return false;
  /**/JDEBUGLOG(fname,2)

  jfTagListing* tag_copy = new jfTagListing(*insource);
  if (!DoVerifyCheck(tag_copy)) {
      delete tag_copy;
      return false;
  }

  if (thedata != NULL) delete thedata;
  thedata = tag_copy;
  DetermineEmptyState();
  MakeNullResult();

  return true;
}
//-----------------------------------
jfTagListing* jfTagFilterCore::GetTagsCopy() const {
  if (thedata == NULL) return NULL;
  return new jfTagListing(*thedata);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual methods
bool jfTagFilterCore::IsEmpty() const {
    return (empty_state == jfTagFilterEmpty::IS_EMPTY);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfTagFilterCore::ToString() const {
  if (thedata==NULL) return "";
  else return thedata->ToString();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfTagFilterCore::DetermineEmptyState() {
    // variables
    bool result = true;
    jfTagListing::const_iterator iter;
    // special case
    if (thedata == NULL) {
        // each possible tag must have a value, no if there is none, the state is unset
        empty_state = jfTagFilterEmpty::UNKNOWN;
    }
    else {
        // main checking
        iter = thedata->begin();
        while (result) {
            result = ((iter->second) == jfts_NONE);
            iter++;
            if (iter == (thedata->end()))
                break;
        }
        empty_state = (result) ? jfTagFilterEmpty::IS_EMPTY : jfTagFilterEmpty::NOT_EMPTY;
    }
}
// -------------------------------------------------
bool jfTagFilterCore::FromStringInner(const QString& sourcedata, QString& error_out) {
    // local variables
    // we create and convert
    jfTagListing* newvalue = new jfTagListing();
    bool conversult = newvalue->FromString(sourcedata);

    if ((conversult) && DoVerifyCheck(newvalue)) {
        if (thedata != NULL) delete thedata;
        thedata = newvalue;
        DetermineEmptyState();
        MakeNullResult();
        return true;
    }
    else {
        if (!conversult) error_out = "Parsing the tag listing failed!";
        else error_out = "The input cannot be matched with the defined tags!";
        delete newvalue;
        return false;
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

    assert(empty_state != jfTagFilterEmpty::UNKNOWN);
    if (empty_state == jfTagFilterEmpty::IS_EMPTY) return true;

    // parsing the inout to check against
    delimlist = ListToStrArr(incheck,cm[0]);
    if (delimlist == NULL) return nullresult;
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
    if (empty_state == jfTagFilterEmpty::IS_EMPTY) {
        nullresult = true;
    }
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
  jerror::AssertLog(intags!=NULL,fname,"input is NULL");
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
    size_t tcount, tfoundc, tnfound;
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
    for (int tloop=0 ; tloop<(sourcelist->size()) ; tloop++) {
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
// --------------------------------------
// this verison of verify tags uses a parameter instead of thedata field.
bool jfTagFilterCore::VerifyCheck(jfTagListing* to_check, const QStringList* sourcelist, const QStringList* replist) const {
    // variables
    size_t tcount, tfoundc, tnfound;
    QString ctag;
    bool tfound;
    // checks
    assert(sourcelist != NULL);
    if (to_check==NULL) return false;
    // we start
    tcount = to_check->size();
    tfoundc = 0;
    tnfound = 0;
    // the replacement list
    if (replist!=NULL) {
        to_check->ReplaceTags(replist);
    }
    // the loop
    for (int tloop=0 ; tloop<(sourcelist->size()) ; tloop++) {
        ctag = sourcelist->at(tloop);
        tfound = ((to_check->end())!=(to_check->find(ctag)));
        if (tfound) tfoundc++;
        else {
            tnfound++;
            (*to_check)[ctag] = jfts_NONE;
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
    empty_state = jfTagFilterEmpty::IS_EMPTY;
    nullresult = true;
}
//------------------------------------------------------------
void jfTagFilterCore::SetEmpty(const QStringList* sourcelist) {
    // variables
    QString tag;
    // starting checks
    assert(sourcelist!=NULL);
    if (thedata == NULL) thedata = new jfTagListing();
    else thedata->clear();
    // loop to add items
    for ( int loopc = 0 ; loopc < sourcelist->size() ; loopc++ ) {
        tag = QString(sourcelist->at(loopc));
        (*thedata)[tag] = jfts_NONE;
    }
    empty_state = jfTagFilterEmpty::IS_EMPTY;
    nullresult = true;
}


//========================================================================
const jfFilterTypeMeta COMPLETED_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::FLAGS, "CompletedFilter", "Completed Filter",
          QString("For items that have a completed flag, this filter passes if ") +
                  "the value matches what the filter specifies: complete, incomplete," +
                  " or either (the default empty value).",
          IdForGenericFanfic2(), createFilter<jfCompletedFilter> );
//========================================================================
// constructors
//-----------------------------------------------------
jfCompletedFilter::jfCompletedFilter(const QString& filter_name):jfFilterBase(filter_name) {
    state_wanted = jfCompletedState::EITHER;
}
//--------------------------------------
jfCompletedFilter::jfCompletedFilter(QString&& filter_name):jfFilterBase(filter_name) {
    state_wanted = jfCompletedState::EITHER;
}
//----------------------------------------------------
jfCompletedFilter::jfCompletedFilter(const QString& filter_name, jfCompletedState value):jfFilterBase(filter_name) {
    state_wanted = value;
}
//---------------------------------------------------
jfCompletedFilter::jfCompletedFilter(QString&& filter_name, jfCompletedState value):jfFilterBase(filter_name) {
    state_wanted = value;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter
bool jfCompletedFilter::IsEmpty() const {
  return (state_wanted == jfCompletedState::EITHER);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfCompletedFilter::ToString() const {
    if (state_wanted == jfCompletedState::EITHER) return "meh";
    else if (state_wanted == jfCompletedState::COMPLETE) return "yes";
    else return "no";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data setting
//-----------------------------------------------------
void jfCompletedFilter::SetValue(jfCompletedState value_in) {
    state_wanted = value_in;
}
//----------------------------------------------
void jfCompletedFilter::SetCompleted(bool value_in) {
    state_wanted = (value_in) ? jfCompletedState::COMPLETE : jfCompletedState::INCOMPLETE;
}
//-----------------------------------------------------
jfCompletedState jfCompletedFilter::GetValue() const {
    return state_wanted;
}
//----------------------------------------------------------------
void jfCompletedFilter::SetEmpty() {
    state_wanted = jfCompletedState::EITHER;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfCompletedFilter::GetTypeMetaInfo() const {
    return COMPLETED_FILTER_INFO;
}
//-------------------------------------------------
// copy
jfFilterBase* jfCompletedFilter::GenCopy() const {
  jfCompletedFilter* result = new jfCompletedFilter(name, state_wanted);
  result->description = description;
  return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfCompletedFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    jfLineParse* lparser;
    bool success = false;
    lparser = new jfLineParse(sourcedata);
    // new 'yes'/'no'/'meh' format
    if (lparser->Num() == 1) {
        success = true;
        if (sourcedata == "yes") state_wanted = jfCompletedState::COMPLETE;
        else if (sourcedata == "no") state_wanted = jfCompletedState::INCOMPLETE;
        else if (sourcedata == "meh") state_wanted = jfCompletedState::EITHER;
        else {
            success = false;
            error_out = "Singled field data invalid!";
        }
    }
    // compatability
    else if (lparser->Num() == 2) {
        bool valset; bool is_complete;
        if ((lparser->BoolVal(0,valset)) && (lparser->BoolVal(1,is_complete))) {
            success = true;
            if (valset) {
                state_wanted = (is_complete) ? jfCompletedState::COMPLETE : jfCompletedState::INCOMPLETE;
            }
            else state_wanted = jfCompletedState::EITHER;
        }
        else {
            error_out = "Boolean flags could not be parsed!";
        }
    }
    // number of fields is bad
    else {
        error_out = "Wrong number of fields";
    }
    delete lparser;
    return success;
}
// ---------------------------------
// the core matching method
bool jfCompletedFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const jfGenericFanfic2* rvalue;
  bool cvalue;
  assert(testelem!=NULL);
  if (state_wanted == jfCompletedState::EITHER) return true;
  rvalue = dynamic_cast<const jfGenericFanfic2*>(testelem);
  cvalue = rvalue->IsComplete();
  if (cvalue) return (state_wanted == jfCompletedState::COMPLETE);
  else return (state_wanted == jfCompletedState::INCOMPLETE);
}
/******************************************************************************/
