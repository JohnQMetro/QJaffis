/******************************************************************************
Name    :   ao3_specials3.cpp
Author  :   John Q Metro
Purpose :   AO3 fandom and kudos filters
Created :   May 14, 2014
Updated :   May 14, 2014
******************************************************************************/
#ifndef AO3_SPECIALS3_H
  #include "ao3_specials3.h"
#endif // AO3_SPECIALS3_H
//------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "../../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#ifndef JFEXPRMATCH
  #include "../../express/expr_match.h"
#endif
#ifndef JFEXPRPARSE
  #include "../../express/expr_parse.h"
#endif
#include <assert.h>
/*****************************************************************************/
/* Intended replacement for jfACategoriesFilter, with crossover boolean, forall
filter (mainly intended to allow excluding intra-universe crossovers) and the
usual expression filter operating on the names of all fandoms */
//+++++++++++++++++++++++++++++++++++++++
// constructors
//--------------------------------
jfAO3FandomFilter::jfAO3FandomFilter():jfBaseFilter() {
  // setting the booleans
  validdata = false;
  loaded = false;
  allmatch = false;
  must_crossover = false;
  // setting parsed stuff
  perfmatch_parsed = NULL;
  mainmatch_parsed = NULL;
}
//--------------------------------
jfAO3FandomFilter::jfAO3FandomFilter(const jfAO3FandomFilter& source):jfBaseFilter() {
    // copying the most basic items
  name = source.name;
  validdata = source.validdata;
  parse_error = source.parse_error;
  // copying the booleans
  must_crossover = source.must_crossover;
  // copying per fandom stuff
  allmatch = source.allmatch;
  perfmatch_src = source.perfmatch_src;
  if (source.perfmatch_parsed!=NULL) {
    perfmatch_parsed = (source.perfmatch_parsed)->Copy();
  }
  else perfmatch_parsed = NULL;
  // copying general match stuff
  mainmatch_src = source.mainmatch_src;
  if (source.mainmatch_parsed!=NULL) {
    mainmatch_parsed = (source.mainmatch_parsed)->Copy();
  }
  else mainmatch_parsed = NULL;
}
//+++++++++++++++++++++++++++++++++++++++
//checks if the filter has no effect
bool jfAO3FandomFilter::isEmpty() const {
  if (must_crossover) return false;
  // note that the perfmatch stuff only has meaning if must_crossover is true
  return (mainmatch_parsed==NULL)||(mainmatch_parsed->size()==0);
}
//+++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//--------------------------------
bool jfAO3FandomFilter::FromString(const QString& sourcedata) {
  // constants
  const QString fname = "jfAO3FandomFilter::FromString";
  // variables
  QString parse1, parse2, oerr;
  bool altmatch;
  jfElemArray *psd1, *psd2;
  // starting with the parsing
  jfLineParse lparse(sourcedata);
  // we only allow 1 or 3 fields
  size_t fc = lparse.Num();
  if ((fc!=1)&&(fc!=3)) return false;
  // if three fields, then we must have a crossover
  if (fc==3) {
    if (!lparse.BoolVal(0,altmatch)) return false;
    parse1 = lparse.UnEscStrE(2);
    parse2 = lparse.UnEscStrE(1);
  }
  else parse1 = lparse.UnEscStrE(0);
  // we start with parsing the main stuff
  if (!parse1.isEmpty()) {
    psd1 = ParseInput(parse1,oerr);
    if (psd1==NULL) {
      parse_error = oerr;
      return false;
    }
  }
  else psd1 = NULL;
  // next up, parsing the per fandom stuff...
  if (fc==3) {
    if (!parse2.isEmpty()) {
      psd2 = ParseInput(parse2,oerr);
      if (psd1==NULL) {
        parse_error = oerr;
        if (psd1!=NULL) delete psd1;
        return false;
      }
    }
    else psd2 = NULL;
  }
  // if we get here, everything is okay. we assign values
  mainmatch_src = parse1;
  if (mainmatch_parsed!=NULL) delete mainmatch_parsed;
  mainmatch_parsed = psd1;
  must_crossover = (fc==3);
  if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
  if (must_crossover) {
    perfmatch_parsed = psd2;
    perfmatch_src = parse2;
    allmatch = altmatch;
  }
  else {
    perfmatch_parsed = NULL;
    perfmatch_src.clear();
    allmatch = false;
  }
  // done
  validdata = true;
  loaded = true;
  return true;
}
//--------------------------------
QString jfAO3FandomFilter::ToString() const {
  if (!validdata) return "";
  QString result;
  if (must_crossover) {
    result = Bool2Str(allmatch) + ";";
    result += EscapeNL(perfmatch_src,EXPR_DELIMS) + ";";
  }
  result += EscapeNL(mainmatch_src,EXPR_DELIMS);
  return result;
}
//+++++++++++++++++++++++++++++++++++++++
// specific data setting methods
//--------------------------------
bool jfAO3FandomFilter::GeneralFromExpr(jfSimpleExpr* in_source) {
  // checking for false things
  if (in_source==NULL) return false;
  if (!in_source->valid) return false;
  // setting the values
  if (mainmatch_parsed!=NULL) delete mainmatch_parsed;
  mainmatch_parsed = in_source->parsedinfo;
  in_source->parsedinfo = NULL;
  in_source->valid = false;
  mainmatch_src = in_source->srcstring;
  if (in_source->isempty) {
    if (mainmatch_parsed!=NULL) delete mainmatch_parsed;
    mainmatch_parsed = NULL;
  }
  return true;
}
//--------------------------------
bool jfAO3FandomFilter::PerFandomFromExpr(jfSimpleExpr* in_source) {
  // checking for false things
  if (in_source==NULL) return false;
  if (!in_source->valid) return false;
  // setting the values
  if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
  perfmatch_parsed = in_source->parsedinfo;
  in_source->parsedinfo = NULL;
  in_source->valid = false;
  perfmatch_src = in_source->srcstring;
  if (in_source->isempty) {
    if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
    perfmatch_parsed = NULL;
  }
  return true;
}
//--------------------------------
void jfAO3FandomFilter::SetCrossover(bool iscross) {
  must_crossover = iscross;
}
//--------------------------------
void jfAO3FandomFilter::SetAllMatch(bool amatch) {
  allmatch = amatch;
}
//+++++++++++++++++++++++++++++++++++++++
// getting data
//--------------------------------
QString jfAO3FandomFilter::GetMainSrc() const {
  return mainmatch_src;
}
//--------------------------------
QString jfAO3FandomFilter::GetPerFandomSrc() const {
  return perfmatch_src;
}
//--------------------------------
bool jfAO3FandomFilter::GetCrossover() const {
  return must_crossover;
}
//--------------------------------
bool jfAO3FandomFilter::GetAllMatch() const {
  return allmatch;
}
//+++++++++++++++++++++++++++++++++++++++
// Additional implemented methods
QString jfAO3FandomFilter::GetTypeDescription() const {
  return "A Filter that uses the AO3 Fandom Names with the fic.\
The General Expression filter works on the list of fandoms as a whole,\
while the other filter passes fics that either one fandom or all fandoms\
match the expression. This can be used, for example, to make sure\
crossovers are not just within a specific 'verse, like Star Trek.";
}
//--------------------------------
jfBaseFilter* jfAO3FandomFilter::GenCopy() const {
  jfAO3FandomFilter* result;
  result = new jfAO3FandomFilter(*this);
  return result;
}
//--------------------------------
QString jfAO3FandomFilter::GetTypeID() const {
  return "AO3FandomFilter";
}
//+++++++++++++++++++++++++++++++++++++++
// destructor
jfAO3FandomFilter::~jfAO3FandomFilter() {
  if (mainmatch_parsed!=NULL) delete mainmatch_parsed;
  if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
}
//+++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAO3FandomFilter::CoreMatch(const jfBasePD* testelem) const {
  // variables
  const jfAO3Fanfic* typed_item;
  QString currfandom;
  bool expr_res;
  size_t findex,fcount;
  // we start
  assert(testelem!=NULL);
  // starting
  typed_item = dynamic_cast<const jfAO3Fanfic*>(testelem);
  // the simplest test is the crossover test
  if (must_crossover) {
    if ((typed_item->FandomCount())==1) return false;
  }
  // nest up, the mainmatch
  if (mainmatch_parsed!=NULL) {
    currfandom = typed_item->GetFandoms();
    expr_res = StringExprMatch(currfandom,mainmatch_parsed);
    if (!expr_res) return false;
  }
  // testing the crossovers!
  if (must_crossover && (perfmatch_parsed!=NULL)) {
    // prepping for the loop
    fcount = typed_item->FandomCount();
    // the matching loop
    for (findex=0;findex<fcount;findex++) {
      // checking for the match
      currfandom = typed_item->GetFandom(findex);
      expr_res = StringExprMatch(currfandom,perfmatch_parsed);
      // we now use allmatch. if allmatch is true, then one faliure spells doom
      if (allmatch && (!expr_res)) return false;
      // otherwise, one success means success
      if ((!allmatch) && expr_res) break;
    }
    // if we get here..
    if (!expr_res) return false;
  }
  // everything has passed
  return true;
}
//+++++++++++++++++++++++++++++++++++++++
// file i/o
//--------------------------------
bool jfAO3FandomFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding the lines...
  buffer = Bool2Str(must_crossover) + ";";
  buffer += Bool2Str(allmatch);
  (*outfile) << buffer << "\n";
  // per fandom line
  buffer = EscapeNL(perfmatch_src,IO_DELIMS);
  (*outfile) << buffer << "\n";
  // main match line
  buffer = EscapeNL(mainmatch_src,IO_DELIMS);
  (*outfile) << buffer << "\n";
  // done
  return true;
}
//--------------------------------
bool jfAO3FandomFilter::ReadRestFromFile(jfFileReader* infile) {
  // basic stuff
  const QString fname = "jfAO3FandomFilter::ReadRestFromFile";
  assert(infile!=NULL);
  QString oerr,cline;
  // starting the read, boolean values
  if (!infile->ReadParseLine(2,fname)) return false;
  if (!(infile->lp).BoolVal(0,must_crossover)) {
    return infile->BuildError("The crossover field is not a boolean!");
  }
  if (!(infile->lp).BoolVal(1,allmatch)) {
    return infile->BuildError("The all fandoms match field is not a boolean!");
  }
  // next, the per fandom expression
  if (!infile->ReadUnEsc(cline,fname)) return false;
  perfmatch_src = cline;
  if (perfmatch_src.isEmpty()) perfmatch_parsed = NULL;
  else {
    perfmatch_parsed = ParseInput(cline,oerr);
    if (perfmatch_parsed==NULL) {
      return infile->BuildError("PF Parsing Error : "+oerr);
    }
  }
  // next, the main match expression
  if (!infile->ReadUnEsc(cline,fname)) return false;
  mainmatch_src = cline;
  if (mainmatch_src.isEmpty()) mainmatch_parsed = NULL;
  else {
    mainmatch_parsed = ParseInput(cline,oerr);
    if (mainmatch_parsed==NULL) {
      return infile->BuildError("MM Parsing Error : "+oerr);
    }
  }
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++
/* since different types are stored together, the text file reprentation
may have objects of varying length */
size_t jfAO3FandomFilter::ExtraLines() const {
  return 3;
}
//--------------------------------------------
// extra methods
jfElemArray* jfAO3FandomFilter::ParseInput(QString srcstring,QString oerr) {
  // constants
  const QString fname = "jfAO3FandomFilter::ParseInput";
  // variables
  jfExpParserClass* theparser;
  jfElemArray* pfix, *xfix;
  size_t osize;
  // starting
  srcstring = srcstring.trimmed();
  theparser = new jfExpParserClass(srcstring,true,NULL);
  // trying the parse
  if (!theparser->ParseExpression(osize)) {
    // the intial parse has failed
    oerr = theparser->parse_error;
    delete theparser;
    return NULL;
  }
  // parsed okay
  xfix = theparser->GetResult();
  assert(xfix!=NULL);
  delete theparser;
  // next, we postfixize it
  pfix = MakeExprPostfix(xfix,oerr);
  delete xfix;
  if (pfix==NULL) {
    parse_error = oerr;
    return NULL;
  }
  // postfixed okay
  return pfix;
}
//============================================================
// constructors
//---------------------------------------------------
jfAO3KudoFilter::jfAO3KudoFilter():jfMinMaxUFilter() {}
//---------------------------------------------------
jfAO3KudoFilter::jfAO3KudoFilter(size_t inmin, size_t inmax):jfMinMaxUFilter(inmin,inmax) {}
//---------------------------------------------------
jfAO3KudoFilter::jfAO3KudoFilter(const jfAO3KudoFilter& source) {
    // copying the most basic items
  name = source.name;
  validdata = source.validdata;
  description = source.description;
  num_id = source.num_id;
  min = source.min;
  max = source.max;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfAO3KudoFilter::GetTypeID() const {
  return "AO3KudoCountFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------------
QString jfAO3KudoFilter::GetTypeDescription() const {
    return "For items that have word counts, this filter passes if \
the kudo count is between min and max, inclusive, or if max is zero, the \
kudo count is greater than min.";
}
//---------------------------------------------------
jfBaseFilter* jfAO3KudoFilter::GenCopy() const {
  jfAO3KudoFilter* result;
  result = new jfAO3KudoFilter(min,max);
  CopyOver(result);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAO3KudoFilter::CoreMatch(const jfBasePD* testelem) const {
  // variables
  const jfAO3Fanfic* rvalue;
  size_t cvalue;
  // checks
  assert(testelem!=NULL);
  // determining the type
  rvalue = dynamic_cast<const jfAO3Fanfic*>(testelem);
  cvalue = rvalue->GetKudos();
  // checking the wordcount
  return TestMatch(cvalue);
}
/*****************************************************************************/
