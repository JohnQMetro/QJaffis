/******************************************************************************
Name    :   ao3cat_parser.cpp
Author  :   John Q Metro
Purpose :   Parser for AO3 section pages to lists of categories/fandoms
Created :   June 21, 2016
Updated :   August 27, 2023
******************************************************************************/
#ifndef AO3CAT_PARSER_H
  #include "ao3cat_parser.h"
#endif // AO3CAT_PARSER_H
#ifndef UTILS3_H_INCLUDED
  #include "../../../core/utils/utils3.h"
#endif
//-----------------------------------
#include <assert.h>
/*****************************************************************************/
// --- [ jfAO3CatParser METHODS ] -------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfAO3CatParser::jfAO3CatParser():jfPageParserBase(){
  catsection= NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse related methods
//---------------------------
void jfAO3CatParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  // constants
  const QString func = "jfAO3CatParser::ParseDownloadedPage";
  const QString startag = "<ol class=\"alphabet fandom index group \">";
  // variables
  jfAO3_Category* temp;
  bool parseres;
  QString secname,buffer;
  size_t itemcount = 0;
  // starting...
  page_parsed = false;
  NewData(inPage);
  /**/lpt->tLog(func,1);
  if (!xparser.MovePast(startag)) {
    parsErr("Missing header list!");
  }
  xparser.ChopAfter("<!-- BEGIN footer -->",true);
  secname = ao3_catdata::ao3_nameslinks->NameAtIndex(pageindex-1);
  /**/lpt->tLog(func,2,secname);
  catsection = new jfAO3Section(secname);
  // looping to get the categories
  while (xparser.MovePast("<li>")) {
    if (xparser.GetMovePast("</li>",buffer)){
      temp = new jfAO3_Category();
      parseres = temp->SetFromString(buffer,secname);
      if (!parseres) {
        parseErrorMessage = temp->GetParseErr();
        delete temp;
        break;
      }
      itemcount++;
      catsection->AppendCategory(temp);
    }
    else {
      parseErrorMessage = "Malformed Category Entry!";
      break;
    }
  }
  // afterchecking
  if (!parseErrorMessage.isEmpty()) {
    delete catsection;
    catsection = NULL;
  }
  else {
    catsection->ResetIndex();
    page_parsed = true;
  }
}

//---------------------------
void* jfAO3CatParser::getResults() {
  return catsection;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString* jfAO3CatParser::makeRedirectedURL(const QString& inPart) const {
    return NULL;
}
//---------------------------
QString jfAO3CatParser::getCookie() const { return ""; }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfAO3CatParser::testMissing(const QString *page) const {
  return true;
}
//---------------------------
bool jfAO3CatParser::testIncomplete(const QString *page) const {
  assert(page!=NULL);

  const QString trunc1 = "<!-- BEGIN footer -->";
  const QString trunc2 = "<title>You're clicking too fast!</title>";
  // checking
  assert(page!=NULL);
  if (page->contains(trunc1)) return true;
  return (page->contains(trunc2));
}
//------------------------------------
bool jfAO3CatParser::testRatelimit(const QString *page) const {
    const QString trunc2 = "<title>You're clicking too fast!</title>";
    return !(page->contains(trunc2));
}
/*****************************************************************************/
