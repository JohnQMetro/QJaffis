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
    /**/JDEBUGLOGS(fname,2,theparser->parse_error)
    parse_error = theparser->parse_error;
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
    /**/JDEBUGLOGS(fname,7,oerr)
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
  loaded = false;
  parsedinfo = NULL;
  validdata = false;
}
//----------------------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(const QString& sourcedata) {
  parsedinfo = NULL;
  FromString(sourcedata);
}
//----------------------------------------------------------------------
jfSimpleExpFilterCore::jfSimpleExpFilterCore(jfSimpleExpr* in_source) {
  parsedinfo = NULL;
  FromExpr(in_source);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSimpleExpFilterCore::isEmpty() const {
	return (parsedinfo==NULL)||(parsedinfo->size()==0);
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
  // local data
  jfExpParserClass* theparser;
  jfElemArray* pfix;
  QString oerr;
  size_t osize;
  // setting the source
  /**/JDEBUGLOG(fname,1)
  srcstring = sourcedata.trimmed();
  // clearing out the old
  if (parsedinfo!=NULL) delete parsedinfo;
  parsedinfo = NULL;
  loaded = false;
  parse_error.clear();
  // producing the new
  theparser = new jfExpParserClass(srcstring,true,NULL);
  if (!theparser->ParseExpression(osize)) {
    /**/JDEBUGLOGS(fname,2,theparser->parse_error)
    parse_error = theparser->parse_error;
    delete theparser;
    return validdata = false;
  }
  // parsed okay
  parsedinfo = theparser->GetResult();
  assert(parsedinfo!=NULL);
  delete theparser;
  // next, we postfixize it
  pfix = MakeExprPostfix(parsedinfo,oerr);
  delete parsedinfo;
  if (pfix==NULL) {
    /**/JDEBUGLOGS(fname,3,oerr)
    parse_error = oerr;
    return validdata = false;
  }
  // postfixed okay
  parsedinfo = pfix;
  // done
  validdata = true;
  loaded = true;
  return true;
}
//----------------------------------------------------------------------
QString jfSimpleExpFilterCore::ToString() const {
  return srcstring;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// modifying and checking the contents
//----------------------------------------------------------------------
bool jfSimpleExpFilterCore::FromExpr(jfSimpleExpr* in_source) {
  if (in_source==NULL) return false;
  validdata = in_source->valid;
  if (validdata) {
    parsedinfo = in_source->parsedinfo;
    in_source->parsedinfo = NULL;
    in_source->valid = false;
  }
  srcstring = in_source->srcstring;
  parse_error = in_source->parse_error;
  if (validdata) loaded = true;
  return true;
}
//----------------------------------------------------------------------
// returning a string representation of the parsed contents
QString jfSimpleExpFilterCore::MakePList() const {
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
//-----------------------------------------------------------------------
// sort of like load string, except the string is empty
void jfSimpleExpFilterCore::EmptyFilter() {
  // setting the source
  srcstring = "";
  // clearing out the old
  if (parsedinfo!=NULL) delete parsedinfo;
  // creating the empty data
  parsedinfo = new jfElemArray();
  // done
  validdata = true;
  parse_error.clear();
  loaded = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfSimpleExpFilterCore::~jfSimpleExpFilterCore() {
  if (parsedinfo!=NULL) delete parsedinfo;
  parsedinfo = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSimpleExpFilterCore::InternalMatch(const QString& incheck) const {
  return StringExprMatch(incheck,parsedinfo);
}
//-----------------------------------------------------------------------
// check for validity
void jfSimpleExpFilterCore::SetValid() {
  validdata = loaded;
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
  buffer = EscapeNL(srcstring,IO_DELIMS);
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
  // there is only one line, and one filed, so this is pretty simple
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
  parse_error = source.parse_error;
  // copying expression filter specific stuff
  srcstring = source.srcstring;
  loaded = source.loaded;
  if (source.parsedinfo!=NULL) {
    parsedinfo = (source.parsedinfo)->Copy();
  }
  else parsedinfo = NULL;

}
//*******************************************************************************************
