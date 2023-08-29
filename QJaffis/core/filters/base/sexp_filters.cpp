//***************************************************************************
// Name:        sexp_filters.cpp
// Author :     John Q Metro
// Purpose :    Simple expression filters that work on specific string data
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     April 2, 2023 (Rebasing)
//***************************************************************************
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
//-------------------------------------------------------------------------
#ifndef JFEXPRPARSE
  #include "../../express/expr_parse.h"
#endif
/*
#ifndef JFILTERGLOBALS
  #include "../filter_globals.h"
#endif
*/
#ifndef JFEXPRMATCH
	#include "../../express/expr_match.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED

//----------------------------------
#include <assert.h>
//***************************************************************************
jfSimpleExpr::jfSimpleExpr() {
  parsedinfo = NULL;
  valid = false;
  isempty = false;
}
//-----------------------------------------------------
jfSimpleExpr::jfSimpleExpr(const QString& sourcedata) {
  parsedinfo = NULL;
  isempty = false;
  valid = false;
  FromString(sourcedata);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data setting methods
bool jfSimpleExpr::FromString(const QString& sourcedata) {
  // constsants
  const QString fname = "jfSimpleExpr::FromString";
  // local data
  jfExpParserClass* theparser;
  jfElemArray* pfix;
  size_t osize;
  QString oerr;
  // setting the source
  srcstring = sourcedata.trimmed();
  // clearing out the old
  if (parsedinfo!=NULL) delete parsedinfo;
  parsedinfo = NULL;
  parse_error.clear();
  // special case: empty string
  if (srcstring.isEmpty()) {
    valid = isempty = true;
    return true;
  }
  // producing the new
  /**/JDEBUGLOGS(fname,1,srcstring)
  theparser = new jfExpParserClass(srcstring,true,NULL);
  if (!theparser->ParseExpression(osize)) {
      parse_error = theparser->parse_error;
      jerror::ParseLog(fname,parse_error);
    delete theparser;
    return valid = false;
  }
  /**/JDEBUGLOG(fname,3)
  // parsed okay
  parsedinfo = theparser->GetResult();
  assert(parsedinfo!=NULL);
  delete theparser;
  /**/JDEBUGLOG(fname,4)
  // next, we postfixize it
  pfix = MakeExprPostfix(parsedinfo,oerr);
  /**/JDEBUGLOG(fname,5)
  delete parsedinfo;
  /**/JDEBUGLOG(fname,6)
  if (pfix==NULL) {
      jerror::ParseLog(fname, oerr);
        parse_error = oerr;
        return valid = false;
  }
  // okay
  parsedinfo = pfix;
  // done
  valid = true;
  /**/JDEBUGLOG(fname,8)
  return true;
}
//--------------------------------------------------------------------------
bool jfSimpleExpr::InternalMatch(const QString& incheck) const {
  if (isempty) return true;
  return StringExprMatch(incheck,parsedinfo);
}
// -------------------------------------------------------------------------
jfSimpleExpr* jfSimpleExpr::Copy() const {
    jfSimpleExpr* result = new jfSimpleExpr();
    result->parse_error = parse_error;
    result->srcstring = srcstring;
    result->valid = valid;
    result->isempty = isempty;
    if (parsedinfo != NULL) {
        result->parsedinfo = parsedinfo->Copy();
    }
    return result;
}
// -------------------------------------------------------------------------
QString jfSimpleExpr::MakePList() const {
  // local variables
  size_t index,maxc;
  QString result;
  if (parsedinfo == NULL) return "";
  // starting
  maxc = (parsedinfo->size());
  // the loop
  for (index = 0;index<maxc;index++) {
    result+= ((*parsedinfo)[index])->String();
  }
  // done
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info
//-----------------------------------------------------
bool jfSimpleExpr::IsValid() const { return valid; }
//-----------------------------------------------------
bool jfSimpleExpr::isEmpty() const { return isempty; }
//-----------------------------------------------------
QString jfSimpleExpr::GetSrc() const {
  return srcstring;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructors
jfSimpleExpr::~jfSimpleExpr() {
  if (parsedinfo!=NULL) delete parsedinfo;
}
//==========================================================================
// the constructors
//----------------------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(const QString& filter_name):jfFilterBase(filter_name) {
    parsed_expression = NULL;
    old_unescape = true;
}
// --------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(QString&& filter_name):jfFilterBase(filter_name) {
    parsed_expression = NULL;
    old_unescape = true;
}
// --------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(const QString& filter_name, const QString& sourcedata):jfFilterBase(filter_name) {
    QString perror;
    bool set_okay = FromStringInner(sourcedata, perror);
    jerror::AssertLog(set_okay, "jfSimpleExpFilterCore::jfSimpleExpFilterCore","Constructor String Expression Error: " + perror );
    old_unescape = true;
}
// --------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(const QString& filter_name, jfSimpleExpr* in_source):jfFilterBase(filter_name) {
    if ((in_source == NULL) || (in_source->isEmpty())) {
        parsed_expression = NULL;
    }
    else {
        bool okay = in_source->IsValid();
        jerror::AssertLog(okay, "jfSimpleExpFilterCore::jfSimpleExpFilterCore","Constructor input expression is not valid!" );
        parsed_expression = in_source;
    }
    old_unescape = true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSimpleExpFilterCore::IsEmpty() const {
    return (parsed_expression == NULL) || (parsed_expression->isEmpty());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfSimpleExpFilterCore::ToString() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->GetSrc();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// modifying and checking the contents
//----------------------------------------------------------------------
bool jfSimpleExpFilterCore::FromExpr(jfSimpleExpr* in_source) {
    if ((in_source == NULL) || (in_source->IsValid())) {
        if (parsed_expression != NULL) {
            delete parsed_expression;
        }
        if ((in_source == NULL) || (in_source->isEmpty())) {
            parsed_expression = NULL;
        }
        else parsed_expression = in_source;
        return true;
    }
    else return false;
}
//----------------------------------------------------------------------
// returning a string representation of the parsed contents
/*
QString jfSimpleExpFilterCore::MakePList() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->MakePList();
}
*/
//-----------------------------------------------------------------------
// sort of like load string, except the string is empty
void jfSimpleExpFilterCore::EmptyFilter() {
    if (parsed_expression != NULL) {
        delete parsed_expression;
        parsed_expression = NULL;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfSimpleExpFilterCore::~jfSimpleExpFilterCore() {
    EmptyFilter();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------
bool jfSimpleExpFilterCore::FromStringInner(const QString& sourcedata, QString& error_out) {
    jfSimpleExpr* temp_expr = new jfSimpleExpr(sourcedata);

    if (temp_expr->IsValid()) {
        if (parsed_expression != NULL) {
            delete parsed_expression;
        }
        if (temp_expr->isEmpty()) parsed_expression = NULL;
        else parsed_expression = temp_expr;

        return true;
    }
    else {
        error_out = temp_expr->parse_error;
        delete temp_expr;
        return false;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSimpleExpFilterCore::InternalMatch(const QString& incheck) const {
    if (parsed_expression == NULL) return true;
    else return parsed_expression->InternalMatch(incheck);
}
//-----------------------------------------------------------------------
void jfSimpleExpFilterCore::CoreCopy(const jfSimpleExpFilterCore& source) {
  // copying the most basic items
  name = source.name;
  description = source.description;

  // we just re-parse
  if (parsed_expression != NULL) {
      delete parsed_expression;
      parsed_expression = NULL;
  }
  if (source.parsed_expression != NULL) {
      parsed_expression = source.parsed_expression->Copy();
  }
}
//*******************************************************************************************
