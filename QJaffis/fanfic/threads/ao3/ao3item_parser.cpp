/******************************************************************************
Name    :   ao3item_parser.cpp
Author  :   John Q Metro
Purpose :   Parser for AO3 section pages to lists of categories/fandoms
Created :   July 16, 2016
Updated :   February 29, 2020
******************************************************************************/
#ifndef AO3ITEM_PARSER_H
  #include "ao3item_parser.h"
#endif // AO3ITEM_PARSER_H
//-----------------------------------
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#include <assert.h>
#include <math.h>
/*****************************************************************************/
// --- [ METHODS for jfAO3ItemParser ] ---------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfAO3ItemParser::jfAO3ItemParser():jfItemsPageParserBase() {
  this_category = NULL;
}
//------------------------------------------------------
bool jfAO3ItemParser::SetCategory(const jfAO3_Category* in_category){
  if (in_category==NULL) return false;
  this_category = in_category;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse methods
//----------------------------------
void jfAO3ItemParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  const QString fname = "jfAO3ItemParser::ParseDownloadedPage";
  size_t newcount;
  bool fpl_res = true;
  // starting...
  /**/lpt->tLog(fname,1);
  NewData(inPage);
  /**/lpt->tLog(fname,2);
  // the start
  if (!ProcessStart(newcount)) {
    /**/lpt->tLog(fname,3);
    return;
  }
  // preparing for the loop
  page_results = new jfPDVector();

  /**/lpt->tLogS(fname,4,newcount);
  // handling nothing
  if (newcount==0) pagecount = 1;
  else {
    /**/lpt->tLog(fname,5);
    // the loop
    fpl_res = FicParsingLoop();
  }
  // post loop
  /**/lpt->tLog(fname,6);
  // results
  if (!fpl_res) return;
  // post page processing and getting the result
  PostPageProcessing();
  page_parsed = true;
  /**/lpt->tLog(fname,7);
}

//----------------------------------
void* jfAO3ItemParser::getResults() {
  return MakeResults();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods for download
//----------------------------------
QString* jfAO3ItemParser::makeRedirectedURL(const QString& inPart) {
  // the AO3 storiesmight redirect, but the AO3 fandom story listings do not
  return NULL;
}

//----------------------------------
bool jfAO3ItemParser::testMissing(const QString *page) const{
  return true;
}
//----------------------------------
bool jfAO3ItemParser::testIncomplete(const QString *page) const {
  assert(page!=NULL);
  return page->contains("<!-- BEGIN footer -->");
}
//----------------------------------
QString jfAO3ItemParser::getCookie() const {
  return "";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods for parsing
bool jfAO3ItemParser::ProcessStart(size_t& itemcount) {
  // constants and asserts
  const QString funcname = "jfAO3ItemParser::ProcessStart";
  // values
  unsigned long val1,val2,val3;
  QString outerr;
  size_t whicho, outloco;
  float tpagecount;
  // start parsing
  if (!xparser.MovePast("<!-- END header -->")) {
    return parsErr("Cannot find end of header!");
  }
  if (!xparser.MovePast("<h2 class=\"heading\">")) {
    return parsErr("Cannot find works count line!");
  }
  // getting the itemcounts
  if (!xparser.Find1stPair("-","Work",whicho,outloco)) {
    return parsErr("Cannot find works!");
  }
  if (whicho==1) {  // multi-page
    if (!xparser.GetMovePastULong("-",val1,outerr)) {
      return parsErr("Cannot extract/convert start work index! "+outerr);
    }
    if (!xparser.GetMovePastULong("of",val2,outerr)) {
      return parsErr("Cannot extract/convert end work index! "+outerr);
    }
    if (!xparser.GetMovePastULong("Works",val3,outerr)) {
      return parsErr("Cannot extract/convert total work count! "+outerr);
    }
    // checking the item counts
    if (val1>val2) return parsErr("The start index is after the end index!");
    if (val2>val3) return parsErr("The end index is more than the item count!");
  }
  else if (whicho==2) { // single page
    if (!xparser.GetMovePastULong("Work",val3,outerr)) {
      return parsErr("Cannot extract/convert total work count! "+outerr);
    }
  }
  else assert(false);
  // processing the itemcounts
  itemcount = val3;
  tpagecount = float(val3) / float(20);
  pagecount = ceil(tpagecount);
  // moving past the other gunk
  if (!xparser.MovePast("<h3 class=\"landmark heading\">Listing Works</h3>")) {
    return parsErr("Cannot start of works list!");
  }
  // done
  return true;
}
//----------------------------------------------
bool jfAO3ItemParser::FicParsingLoop() {
  // constants and asserts
  const QString funcname = "jfAO3ItemParser::FicParsingLoop";
  const QString fps = "<!--title, author, fandom-->";
  const QString fpe1 = "<li class=\"work blurb group\" ";
  // variables
  jfAO3Fanfic* temp;
  QString buffer, zbuffer;
  QString errout;
  // starting
  // the loop
  while(xparser.GetDelimitedEndPair(fps,fpe1,"<!--/content-->",buffer)) {
    // building
    temp = new jfAO3Fanfic();
    temp->SetFromString(buffer,this_category,errout);
    // success
    if (temp->IsValid()) page_results->push_back(temp);
    // the item is not okay
    else {
      /**/lpt->tLog(funcname,1);
      if (!(temp->included)) delete temp;
      else {
        /**/lpt->tLog(funcname,2,buffer);
        delete temp;
        return parsErr("AO3 item " + errout);
      }
    }
    // done here
  }
  // done
  return true;
}

/*****************************************************************************/

