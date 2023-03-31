/////////////////////////////////////////////////////////////////////////////
// Name:        expression.cpp
// Author :     John Q Metro
// Purpose :    Defines  the summary and title general expression filter
// Created:     31.12.06
// Conversion to QT Started April 11, 2013
// Updated:     February 12, 2018 (fixed crash in destructor caused by dangling pointer)
/////////////////////////////////////////////////////////////////////////////
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
#ifndef JFILTERGLOBALS
  #include "../filter_globals.h"
#endif
#ifndef JFEXPRMATCH
#include "../../express/expr_match.h"
#endif
//-------------------------------
#include <assert.h>
//*************************************************************************
size_t efcounter;
const QString EXP_FIL_TAG = "ExprFilter";
// --- [ methods for jfExpressionFilter ] -------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the constructors
//----------------------------------------------------------------------
// the basic constructor
jfExpressionFilter::jfExpressionFilter():jfBaseFilter() {
  loaded = false;
  parsedinfo = NULL;
  validdata = false;
  fmap_link = NULL;
}
//----------------------------------------------------------------------
/* a copy constructor. I've decided, after a while, to avoid validation
during object construction: we verify afterwards using VerifyNames.
also, no CopyLoad done here, we wait until later (after verifynames is called) */
jfExpressionFilter::jfExpressionFilter(const jfExpressionFilter& source,jfFilterMap* infmap_link) {
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
  fmap_link = infmap_link;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfExpressionFilter::isEmpty() const {
  return (parsedinfo==NULL)||(parsedinfo->size()==0);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfExpressionFilter::SetFiltermapLink(jfFilterMap* infmap_link) {
  fmap_link = infmap_link;
  return (fmap_link!=NULL);
}
//-------------------------------------------------------------------------
// redefined virtual functions
// loading stuff
bool jfExpressionFilter::FromString(const QString& sourcedata) {
  // constants
  const QString fname = "jfExpressionFilter::FromString";
  // local data
  jfExpParserClass* theparser;
  jfElemArray* pfix;
  size_t pcount;
  QString oerr;
  // setting the source
  srcstring = sourcedata.trimmed();
  // clearing out the old
  if (parsedinfo!=NULL) delete parsedinfo;
  parsedinfo = NULL;
  parse_error.clear();
  loaded = false;
  // producing the new
  theparser = new jfExpParserClass(srcstring,false,fmap_link);
  if (!theparser->ParseExpression(pcount)) {
      parse_error = theparser->parse_error;
      jerror::ParseLog(fname,parse_error);
        delete theparser;
        validdata = false;
        return false;
  }
  // parsing succeeded
  parsedinfo = theparser->GetResult();
  assert(parsedinfo!=NULL);
  delete theparser;
  // next, we postfixize it
  pfix = MakeExprPostfix(parsedinfo,oerr);
  delete parsedinfo;
  parsedinfo = NULL;
  if (pfix==NULL) {
      jerror::ParseLog(fname,oerr);
        parse_error = oerr;
        validdata = false;
        return false;
  }
  // things are okay
  parsedinfo = pfix;
  // done
  validdata = true;
  loaded = true;
  return true;
}
//----------------------------------------------------------------------
QString jfExpressionFilter::ToString() const {
  return srcstring;
}
//----------------------------------------------------------------------
// returning a copy
jfExpressionFilter* jfExpressionFilter::Copy() const {
  return new jfExpressionFilter(*this);
}
//-----------------------------------------------------------------------
jfBaseFilter* jfExpressionFilter::GenCopy() const {
  return Copy();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic constants
//----------------------------------------------------------------------
QString jfExpressionFilter::GetTypeID() const {
  return EXP_FIL_TAG;
}
//----------------------------------------------------------------------
QString jfExpressionFilter::GetTypeDescription() const {
  return "The Expression Filter is rather complicated; it matches \
elements against a boolean expression, the elements of which are themselves \
to be matched. These elements are either string to match against the title or \
summary, or embedded filters, whcih can be references to global filters.";

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
  validdata = tres;
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
  validdata = tres;
  if (tres) nameobj->PopName();
  return tres;
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// sort of like load string, except the string is empty
void jfExpressionFilter::EmptyFilter() {
  // setting the source
  srcstring = "";
  // clearing out the old
  if (parsedinfo!=NULL) delete parsedinfo;
  // creating the empty data
  parsedinfo = new jfElemArray();
  parse_error.clear();
  // done
  validdata = true;
  loaded = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfExpressionFilter::~jfExpressionFilter() {
  if (parsedinfo!=NULL) delete parsedinfo;
  parsedinfo = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------------------------------------------------
// basic matching
bool jfExpressionFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  return FullItemExprMatch(testelem,parsedinfo);
}
//-----------------------------------------------------------------------
// check for validity
void jfExpressionFilter::SetValid() {
  validdata = loaded;
}
//-----------------------------------------------------------------------
size_t jfExpressionFilter::ExtraLines() const {
  return 1;
}
//-----------------------------------------------------------------------
bool jfExpressionFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  buffer = EscapeNL(srcstring,IO_DELIMS);
  (*outfile) << buffer << "\n";
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfExpressionFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfExpressionFilter::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadUnEsc(cline,funcname)) return false;
  // there is only one line, and one filed, so this is pretty simple
  resx = FromString(cline);
  if (!resx) return infile->BuildError("The expression string is invalid!");
  else return true;
}

///////////////////////////////////////////////////////////////////////////////
