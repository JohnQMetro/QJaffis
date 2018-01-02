/******************************************************************************
Name    :   fimparse_compact.cpp
Author  :   John Q Metro
Purpose :   Fimfiction.net parser which gets summaries and totals from compact
            search results.
Created :   June 29, 2016
Updated :   December 31, 2017
******************************************************************************/
#ifndef FIMPARSE_COMPACT_H
  #include "fimparse_compact.h"
#endif // FIMPARSE_COMPACT_H
//-----------------------------------
#include <assert.h>
#include <math.h>
/*****************************************************************************/
// constructor and data setting methods
jfFIMCompactParser::jfFIMCompactParser():jfPageParserBase() {
  parsed_data = NULL;
  extracted_itemcount = 0;
  domature = false;
}
//-------------------------------------
void jfFIMCompactParser::SetDoMature(const bool& inmature) {
  domature = inmature;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse method
void jfFIMCompactParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  const QString fname= "jfFIMCompactParser::ParseDownloadedPage";
  // preparing for new data and resetting
  /**/lpt->tLog(fname,1);
  NewData(inPage);
  extracted_itemcount = 0;
  parsed_data = NULL;
  /**/lpt->tLog(fname,2);
  // moving past much of the pure gunk
  if (!xparser.MovePast("<div class=\"two-columns\"")) {
    parseErrorMessage = "Search Results Count not found!";
    /**/lpt->tLog(fname,3);
    return;
  }
  // setting up some variables...
  ulong tval = 0;
  QString perr, buffer;
  // getting the item count
  if (!xparser.GetDelimitedULong("Found <b>","</b>",tval,perr)) {
    /**/lpt->tLog(fname,4);
    parseErrorMessage = "Count not extractable : " + perr;
    return;
  }
  extracted_itemcount = tval;
  /**/lpt->tLogS(fname,5,extracted_itemcount);
  // estimating the pagecount from the itemcount...
  pagecount = ceil(float(extracted_itemcount)/60.0);
  /**/lpt->tLogS(fname,6,pagecount);
  // next up, the individual stories!
  parsed_data = new jfIDStringStore();
  while(xparser.MovePast("<div class=\"story-card-container\"")) {
    // next up, extracting the fic id
    if (!xparser.GetDelimitedULong("<a class=\"story_link\" href=\"/story/","/",tval,perr)) {
      parseErrorMessage = "Could not get fic id! :" + perr;
      delete parsed_data;
      parsed_data = NULL;
      /**/lpt->tLog(fname,8);
      return;
    }
    // getting the short summary after that...
    if (!xparser.GetDelimited("</ul>","<ul class=\"",buffer)) {
      parseErrorMessage = "Could not extract short description!";
      delete parsed_data;
      parsed_data = NULL;
      /**/lpt->tLog(fname,9);
      return;
    }
    // adding the data to the results
    (*parsed_data)[tval] = buffer.trimmed();
  }
  // the loop is done, things are okay...
  parsed_data->est_itemcount = extracted_itemcount;
  /**/lpt->tLog(fname,12);
  page_parsed = true;
}

//------------------------------
void* jfFIMCompactParser::getResults(){
  return parsed_data;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// FIM currently does not do redirection,ifit does, parsing must be re-written
QString* jfFIMCompactParser::makeRedirectedURL(const QString& inPart) {
  return NULL;
}
//------------------------------
bool jfFIMCompactParser::testMissing(const QString *page) const {
  return true; // not a real test
}
//------------------------------
bool jfFIMCompactParser::testIncomplete(const QString *page) const {
  assert(page!=NULL);
  const QString footer_string = "<!--Footer-->";
  return page->contains(footer_string);
}
//------------------------------
QString jfFIMCompactParser::getCookie() const {
  if (domature) return "view_mature=true";
  else return "";
}
/*****************************************************************************/
