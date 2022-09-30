//***************************************************************************
// Name:        sexp_filters.cpp
// Author :     John Q Metro
// Purpose :    Simple expression filters that work on specific string data
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     August 16, 2014
//***************************************************************************
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
//-------------------------------------------------------------------------
#ifndef JFEXPRPARSE
  #include "../../express/expr_parse.h"
#endif
#ifndef JFILTERGLOBALS
  #include "../filter_globals.h"
#endif
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
// the basic constructor
jfSimpleExpFilterCore::jfSimpleExpFilterCore():jfBaseFilter() {
    parsed_expression = NULL;
    validdata = false;
}
//----------------------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(const QString& sourcedata) {
    parsed_expression = NULL;
    validdata = false;
    FromString(sourcedata);
}
//----------------------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(jfSimpleExpr* in_source) {
    parsed_expression = NULL;
    validdata = false;
    FromExpr(in_source);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSimpleExpFilterCore::isEmpty() const {
    return (parsed_expression == NULL) || (parsed_expression->isEmpty());
}
//-----------------------------------------------------------------------------
// returns a general filter type. The default is 0, unless overriden
size_t jfSimpleExpFilterCore::GetFilType() const { return 1;}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual functions
// loading stuff
bool jfSimpleExpFilterCore::FromString(const QString& sourcedata) {
  // constants
  const QString fname = "jfSimpleExpFilterCore::FromString";

  if (parsed_expression != NULL) {
      delete parsed_expression;
      parsed_expression = NULL;
  }
  parsed_expression = new jfSimpleExpr(sourcedata);
  validdata = parsed_expression->IsValid();
  return validdata;
}
//----------------------------------------------------------------------
QString jfSimpleExpFilterCore::ToString() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->GetSrc();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// modifying and checking the contents
//----------------------------------------------------------------------
bool jfSimpleExpFilterCore::FromExpr(jfSimpleExpr* in_source) {
  if (in_source==NULL) return false;
  else {
      parsed_expression = in_source->Copy();
      validdata = parsed_expression->IsValid();
  }
  return true;
}
//----------------------------------------------------------------------
// returning a string representation of the parsed contents
QString jfSimpleExpFilterCore::MakePList() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->MakePList();
}
//-----------------------------------------------------------------------
// sort of like load string, except the string is empty
void jfSimpleExpFilterCore::EmptyFilter() {
    if (parsed_expression != NULL) {
        delete parsed_expression;
        parsed_expression = NULL;
    }
    validdata = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfSimpleExpFilterCore::~jfSimpleExpFilterCore() {
    EmptyFilter();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSimpleExpFilterCore::InternalMatch(const QString& incheck) const {
    if (parsed_expression == NULL) return true;
    else return parsed_expression->InternalMatch(incheck);
}
//-----------------------------------------------------------------------
size_t jfSimpleExpFilterCore::ExtraLines() const {
  return 1;
}
//-----------------------------------------------------------------------
bool jfSimpleExpFilterCore::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  QString src = (parsed_expression == NULL) ? "" : parsed_expression->GetSrc();
  buffer = EscapeNL(src,IO_DELIMS);
  (*outfile) << buffer << "\n";
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSimpleExpFilterCore::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfSimpleExpFilterCore::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  /**/JDEBUGLOG(funcname,1)
  assert(infile!=NULL);
  /**/JDEBUGLOG(funcname,2)
  if (!infile->ReadUnEsc(cline,funcname)) return false;
  // there is only one line, and one field, so this is pretty simple
  /**/JDEBUGLOGS(funcname,3,cline)
  resx = FromString(cline);
  /**/JDEBUGLOGB(funcname,4,resx)
  if (!resx) return infile->BuildError("The expression string is invalid!");
  else return true;
}
//-----------------------------------------------------------------------
void jfSimpleExpFilterCore::CoreCopy(const jfSimpleExpFilterCore& source) {
  // copying the most basic items
  name = source.name;
  validdata = source.validdata;
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
