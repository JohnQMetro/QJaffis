/*****************************************************************************
Name    : ffncatparser1.cpp
Basic   : Fanfiction.Net category parsing, base and non-crossover versions
Author  : John Q Metro
Started : July 19, 2016
Updated : September 11, 2022

******************************************************************************/
#ifndef FFNCATPARSER1_H
  #include "ffncatparser1.h"
#endif // FFNCATPARSER1_H
//------------------------------
#include <assert.h>
/*****************************************************************************/
// --- [ METHODS for jfFFNCatParserBase ] -------------
// constructor
jfFFNCatParserBase::jfFFNCatParserBase():jfPageParserBase() {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse method, this is only a wrapper for a custom abstract parse method
void jfFFNCatParserBase::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  // clearing out old data and setting the parser
  NewData(inPage);
  page_parsed = ParsePageCore(pageindex);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented methods (pretty trivial)
//--------------------------
QString* jfFFNCatParserBase::makeRedirectedURL(const QString& inPart) const {
  return NULL;
}
//--------------------------
bool jfFFNCatParserBase::testMissing(const QString *page) const {
  assert(page != NULL);
  return !(page->contains("<h2>Oops! We cannot find this page"));
}
//--------------------------
bool jfFFNCatParserBase::testIncomplete(const QString *page) const {
  assert(page != NULL);
  const QString no_footer = "An error has occurred while processing your request.<br><br>";
  const QString footer1 = "<div id=p_footer class=maxwidth";
  const QString footer2 = "<div id=\"p_footer\" class=\"maxwidth\"";
  if (page->contains(no_footer)) return false;
  else if (page->contains(footer1)) return true;
  else return page->contains(footer2);
}
//--------------------------
QString jfFFNCatParserBase::getCookie() const {
  return "";
}
//===========================================================================
//===========================================================================
// --- [jfFFNSectionParser Methods ] -------------------------------
jfFFNSectionParser::jfFFNSectionParser():jfFFNCatParserBase() {
  result_data =NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods
//--------------------------------------
bool jfFFNSectionParser::ParsePageCore(size_t pageindex) {
  // constants
  const QString fname = "jfFFNSection::SetFromPage";
  // script.png' or arrow_switch.png'
  // local variables
  QString buffer, secname, outerr;
  jfFFN_SectionCategory* temp;
  // parsing begins
  if (!ParseStart(secname)) return false;

  // if we have gotten this far, start to setup the result...
  result_data = new jfFFNSection();
  result_data->SetName(secname);

  // category extraction ...
  while (xparser.GetMovePast("</SPAN>", buffer)) {
    // if (RejectTest(buffer)) continue;
    temp = new jfFFN_SectionCategory();
    temp->SetFromSource(secname,buffer,outerr);
    // the category is in error
    if (!temp->IsValid()) {
        /**/lpt->tParseError(fname,outerr);
      delete temp;
      delete result_data;
      result_data = NULL;
      return parsErr(outerr);
    }
    // everything worked fine
    else {
      if ((result_data->GetCatByName(temp->GetName()))!=NULL) {
        outerr = "Two Categories in the same Section with the SAME NAME! : " + temp->GetName();
        /**/lpt->tParseError(fname,outerr);
        delete temp;
        delete result_data;
        result_data = NULL;
        return parsErr(outerr);
      }
      result_data->AddCategory(temp);
    }
  }
  /**/lpt->tLogS(fname,12,result_data->GetCount());
  // if we get here, eveything is ok!
  return true;
}

//--------------------------------------
void* jfFFNSectionParser::getResults() {
  return result_data;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNSectionParser::ParseStart(QString& out_section_name) {
  // constants
  const QString fname = "jfFFNSectionParser::ParseStart";
  // script.png' or arrow_switch.png'
  // local variables
  QString buffer;
  // parsing begins
  out_section_name = "";
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(regex.section_headerA)) {
    /**/lpt->tParseError(fname,"Cannot find critical header string");
    return parsErr("Cannot find critical header string");
  }
  /**/lpt->tLog(fname,3);
  // once here, we extract the name...
  if (!xparser.GetUsingRegex(regex.section_name, true, buffer)) {
      /**/lpt->tParseError(fname,"Problems getting the name!");
      return parsErr("Problems getting the name!");
  }
  // checking the name
  buffer = buffer.trimmed();
  /**/lpt->tLog(fname,6,buffer);
  if (buffer.isEmpty()) {
      /**/lpt->tParseError(fname,"The section name must not be empty!");
    return parsErr("The section name must not be empty!");
  }
  /**/lpt->tLog(fname,7,buffer);
  // next up is processing the list...
  if (!xparser.MovePast(regex.section_column_headerB)) {
      if (!xparser.MovePast(regex.section_column_headerA)) {
          /**/lpt->tParseError(fname,"Cannot find start of first category column!");
          /**/lpt->tParseError(fname, "BLOCK: " + xparser.GetBlock(1000));
          return parsErr("Cannot find start of first category column!");
      }
  }
  /**/lpt->tLog(fname,8);
  // chopping the post list off
  if (!xparser.ChopAfter("</td></tr></tbody></table>",true)) {
    /**/lpt->tLog(fname,9);
    return parsErr("Cannot find end of categories!");
  }
  out_section_name = buffer;
  return true;
}

/*****************************************************************************/
