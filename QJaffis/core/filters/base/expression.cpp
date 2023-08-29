/////////////////////////////////////////////////////////////////////////////
// Name:        expression.cpp
// Author :     John Q Metro
// Purpose :    Defines  the summary and title general expression filter
// Created:     31.12.06
// Conversion to QT Started April 11, 2013
// Updated:     April 5, 2023 (Rebasing)
/////////////////////////////////////////////////////////////////////////////
///

#ifndef EXPRESSION_H
  #include "expression.h"
#endif // EXPRESSION_H

//--------------------------------------
#ifndef JFEXPRPARSE
  #include "../../express/expr_parse.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED

#ifndef JFEXPRMATCH
    #include "../../express/expr_match.h"
#endif
//-------------------------------
#include <assert.h>
//*************************************************************************
size_t efcounter;

const jfFilterTypeMeta EXPRESSION_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::EXPRESSION, "ExprFilter", "General Expression Filter",
          QString("The Expression Filter is rather complicated; it matches ") +
          "elements against a boolean expression, the elements of which are themselves " +
          "to be matched. These elements are either string to match against the title or " +
          "summary, or embedded filters, whcih can be references to global filters.",
          IdForBaseSearch(), createFilter<jfExpressionFilter> );

//===============================================================
// --- [ methods for jfExpressionFilter ] -------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the constructors
//----------------------------------------------------------------------
jfExpressionFilter::jfExpressionFilter(const QString& filter_name):jfFilterBase(filter_name) {
    old_unescape = true;
    loaded = false;
    parsedinfo = NULL;
    fmap_link = NULL;
    verify_state = jfExpVerifiedState::UNCHECKED;
}
// --------------------------------------------
jfExpressionFilter::jfExpressionFilter(QString&& filter_name):jfFilterBase(filter_name) {
    old_unescape = true;
    loaded = false;
    parsedinfo = NULL;
    fmap_link = NULL;
    verify_state = jfExpVerifiedState::UNCHECKED;
}
//----------------------------------------------------------------------
/* a copy constructor. I've decided, after a while, to avoid validation
during object construction: we verify afterwards using VerifyNames.
also, no CopyLoad done here, we wait until later (after verifynames is called) */
jfExpressionFilter::jfExpressionFilter(const jfExpressionFilter& source, const jfFilterMap* infmap_link):jfFilterBase(source.name) {
    old_unescape = true;
    description = source.description;

    // copying expression filter specific stuff
    srcstring = source.srcstring;
    loaded = source.loaded;
    if (source.parsedinfo!=NULL) {
        parsedinfo = (source.parsedinfo)->Copy();
    }
    else parsedinfo = NULL;
    fmap_link = infmap_link;
    if (infmap_link == source.fmap_link) {
        verify_state = source.verify_state;
    }
    else verify_state = jfExpVerifiedState::UNCHECKED;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterBase* jfExpressionFilter::GenCopy() const {
    return Copy();
}
//-----------------------------------------------
bool jfExpressionFilter::IsEmpty() const {
  return (parsedinfo==NULL) || (parsedinfo->size()==0);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfExpressionFilter::SetFiltermapLink(const jfFilterMap* infmap_link) {
  fmap_link = infmap_link;
  return (fmap_link!=NULL);
}

//----------------------------------------------------------------------
QString jfExpressionFilter::ToString() const {
  return srcstring;
}
//----------------------------------------------------------------------
// returning a copy
jfExpressionFilter* jfExpressionFilter::Copy() const {
  return new jfExpressionFilter(*this, fmap_link);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic constants
//----------------------------------------------------------------------
const jfFilterTypeMeta& jfExpressionFilter::GetTypeMetaInfo() const {
    return EXPRESSION_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// modifying and checking the contents
//----------------------------------------------------------------------
bool jfExpressionFilter::VerifyNames(jfNameVerif* nameobj, bool notinmap) {
  assert(nameobj!=NULL);
  bool tres;
  // we add the filter name onto the nameobj stack
  if (!notinmap) nameobj->PushLocal(name);
  // verifying using the parsed info method
  tres = parsedinfo->VerifyNames(nameobj);
  // handling the result
  verify_state = (tres) ? jfExpVerifiedState::VERIFIED : jfExpVerifiedState::FAILED;
  if (tres) nameobj->PopName();
  return tres;
}
//----------------------------------------------------------------------
bool jfExpressionFilter::VerifyNames(jfNameVerif* nameobj, const QString& inmapname) {
  assert(nameobj!=NULL);
  bool tres;
  // we add the filter name onto the nameobj stack
  nameobj->PushGlobal(inmapname,name);
  // verifying using the parsed info method
  tres = parsedinfo->VerifyNames(nameobj);
  // handling the result
  verify_state = (tres) ? jfExpVerifiedState::VERIFIED : jfExpVerifiedState::FAILED;
  if (tres) nameobj->PopName();
  return tres;
}
//----------------------------------------------------------------------
const jfExpVerifiedState& jfExpressionFilter::VerifiedState() const {
    return verify_state;
}
//----------------------------------------------------------------------
// doing the copy load
bool jfExpressionFilter::CopyLoad() {
  loaded = parsedinfo->CopyLoad();
  return loaded;
}
//----------------------------------------------------------------------
// returning a string representation of the parsed contents
QString jfExpressionFilter::MakePList() const {
  // local variables
  size_t index,maxc;
  QString result;
  // starting
  maxc = (parsedinfo->size());
  // the loop
  for (index = 0;index<maxc;index++) {
    result+= ((*parsedinfo)[index])->String();
  }
  // done
  return result;
}
// ------------------------------------------------------------------
bool jfExpressionFilter::FromString(const QString& sourcedata, QString& error_out) {
    return FromStringInner(sourcedata, error_out);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// sort of like load string, except the string is empty
void jfExpressionFilter::EmptyFilter() {
  // setting the source
  srcstring = "";
  // clearing out the old
  if (parsedinfo!=NULL) delete parsedinfo;
  // creating the empty data
  parsedinfo = new jfElemArray();
  // done
  verify_state = jfExpVerifiedState::VERIFIED;
  loaded = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfExpressionFilter::~jfExpressionFilter() {
  if (parsedinfo!=NULL) delete parsedinfo;
  parsedinfo = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfExpressionFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    const QString fname = "jfExpressionFilter::FromStringInner";

    QString srcstring_new = sourcedata.trimmed();
    jfExpParserClass* theparser = new jfExpParserClass(srcstring_new, false, fmap_link);
    size_t pcount;

    // trying to parse
    if (!theparser->ParseExpression(pcount)) {
        error_out = theparser->parse_error;
        jerror::ParseLog(fname,error_out);
        delete theparser;
        return false;
    }
    // parsing succeeded
    jfElemArray* parsedinfo_new = theparser->GetResult();
    assert(parsedinfo_new != NULL);
    delete theparser;

    // next, we postfixize it
    QString oerr;
    jfElemArray* pfix = MakeExprPostfix(parsedinfo_new,oerr);
    delete parsedinfo_new;
    parsedinfo_new = NULL;
    // if postfixing failed...
    if (pfix == NULL) {
        jerror::ParseLog(fname,oerr);
        error_out = oerr;
        return false;
    }
    // things are okay
    srcstring = sourcedata;
    if (parsedinfo != NULL) delete parsedinfo;
    parsedinfo = pfix;
    loaded = true;
    verify_state = jfExpVerifiedState::UNCHECKED;

    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic matching
bool jfExpressionFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  return FullItemExprMatch(testelem,parsedinfo);
}


///////////////////////////////////////////////////////////////////////////////
