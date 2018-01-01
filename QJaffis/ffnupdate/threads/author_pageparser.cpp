/*****************************************************************************
Name    : author_pageparser.cpp
Basic   : parserclass for downloaded Fanfiction.Net author pages
Author  : John Q Metro
Started : August 8, 2016
Updated : August 8, 2016

******************************************************************************/
#ifndef AUTHOR_PAGEPARSER_H
  #include "author_pageparser.h"
#endif // AUTHOR_PAGEPARSER_H
//---------------------------------------------

//---------------------------------------------
#include <assert.h>
/*****************************************************************************/
//+++ [ Methods for jfFFNAuthorPageParser ] +++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfFFNAuthorPageParser::jfFFNAuthorPageParser():jfPageParserBase() {
  author_data = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse method
void jfFFNAuthorPageParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  // constants
  const QString func = "jfFFNAuthorPageParser::ParseDownloadedPage";
  // variables
  size_t ext_authid;
  QString ext_authname;
  // starting...
  page_parsed = false;
  NewData(inPage);
  // calling the parse start to get some basic stuff
  if (!ParseStart(ext_authid,ext_authname)) return;
  // setting up for the more serious parsing
  author_data = new jfAuthorStories(ext_authid, ext_authname);
  bool result = ParseTheRest();
  // handling the result
  if (result) page_parsed = true;
  else {
    delete author_data;
    author_data = NULL;
  }
}
//-------------------------------------------
void* jfFFNAuthorPageParser::getResults() {
  return author_data;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// handling redirection, NULL result if faliure
QString* jfFFNAuthorPageParser::makeRedirectedURL(const QString& inPart) {
  return NULL;  // FFN does not do any redirection on valid pages (currently)
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// test methods
//-----------------------------------
bool jfFFNAuthorPageParser::testMissing(const QString *page) const {
  assert(page!=NULL);
  return !(page->contains("<hr size=1 noshade>User does not exist"));
}
//-----------------------------------
bool jfFFNAuthorPageParser::testIncomplete(const QString *page) const {
  assert(page !=NULL);
  return page->contains("<div id=p_footer");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNAuthorPageParser::getCookie() const { return ""; }
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parser helper methods
//-------------------------------
bool jfFFNAuthorPageParser::ParseStart(size_t& authorid_out, QString& authorname_out) {
  // variables
  QString outerr,buffer;
  unsigned long tval;

  // author id
  if (!xparser.MovePast("<link rel=\"canonical\"")) return parsErr("Cannot find author link!");
  if (!xparser.GetDelimitedULong("href=\"//www.fanfiction.net/u/","/",tval,outerr)) {
    return parsErr("Cannot extract author id: " +outerr);
  }
  // author name
  if (!xparser.GetDelimited("<title>","|",buffer)) {
    return parsErr("Cannot get author name!");
  }
  // skipping past much header gunk
  if (!xparser.MovePastTwo("<!-- new stuff -->","<ul class=\"nav nav-tabs\" id=mytab>")) {
    return parsErr("Formatting Errors in Author Page start! : " + buffer);
  }
  // we want to extrat the author's stories, not favorite stories which are listed first
  if (!xparser.MovePast("<div id='st_inside'>")) {
    return parsErr("Missing author stories!");
  }
  // we have gotten here, so we finish successfully
  authorid_out = tval;
  authorname_out = buffer.trimmed();
  return true;
}

//-------------------------------
bool jfFFNAuthorPageParser::ParseTheRest() {
  QString raw_fic;
  jfFFNItemAuthor* temp_author;
  QString outerr;

  while(xparser.MovePast("<div class='z-list mystories'")) {
    if (!xparser.GetDelimited(">","</div></div>",raw_fic)) {
      return parsErr("Problems extracting fic");
    }
    temp_author = new jfFFNItemAuthor();
    temp_author->SetFromString(raw_fic,outerr);
    if (!temp_author->IsValid()) {
      delete temp_author;
      return parsErr("Faliure while parsing fic : " + outerr);
    }
    author_data->InsertNewFic(temp_author);
  }
  return true;
}

/*****************************************************************************/
