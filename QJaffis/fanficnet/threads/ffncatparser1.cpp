/*****************************************************************************
Name    : ffncatparser1.cpp
Basic   : Fanfiction.Net category parsing, base and non-crossover versions
Author  : John Q Metro
Started : July 19, 2016
Updated : July 19, 2016

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
QString* jfFFNCatParserBase::makeRedirectedURL(const QString& inPart) {
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
  const QString footer1 = "<div id=p_footer class=maxwidth";
  const QString footer2 = "<div id=\"p_footer\" class=\"maxwidth\"";
  if (page->contains(footer1)) return true;
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
  const QString header1 = "<div style='width:100%;' class=xcontrast_outer id=content_parent>";
  const QString colhead = "<TD VALIGN=TOP style='line-height:150%;'>";
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
      /**/lpt->tLog(fname,10,outerr);
      delete temp;
      delete result_data;
      result_data = NULL;
      return parsErr(outerr);
    }
    // everything worked fine
    else {
      if ((result_data->GetCatByName(temp->GetName()))!=NULL) {
        outerr = "Two Categories in the same Section with the SAME NAME! : " + temp->GetName();
        /**/lpt->tLog(fname,11,outerr);
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
  const QString header1 = "<div style='width:100%;' class=xcontrast_outer id=content_parent>";
  const QString header2 = "<div style=\"width:100%;\" class=\"xcontrast_outer\" id=\"content_parent\">";
  const QString colhead1 = "<td style=\"line-height:150%\" valign=\"TOP\">";
  const QString colhead2 = "<TD VALIGN=TOP style='line-height:150%";
  const QString colhead3 = "<td style=\"line-height:150%;\" valign=\"TOP\">";
  // script.png' or arrow_switch.png'
  // local variables
  QString buffer;
  // parsing begins
  out_section_name = "";
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePastAlt(header2,header1)) {
    /**/lpt->tLog(fname,2);
    return parsErr("Cannot find critical header string");
  }
  /**/lpt->tLog(fname,3);
  // once here, we extract the name...
  if (!xparser.GetDelimited("align=\"absmiddle\">","<",buffer)) {
      /**/lpt->tLog(fname,4);
      if (!xparser.GetDelimited("align=absmiddle>","<",buffer)) {
          return parsErr("Problems getting the name!");
      }
  }
  // checking the name
  buffer = buffer.trimmed();
  /**/lpt->tLog(fname,6,buffer);
  if (buffer.isEmpty()) {
    return parsErr("The section name must not be empty!");
  }
  /**/lpt->tLog(fname,7,buffer);
  // next up is processing the list...
  if (!xparser.MovePastAlt(colhead1,colhead2)) {
      if (!xparser.MovePast(colhead3)) {
          /**/lpt->tLog(fname,8,xparser.GetBlock(3000));
            return parsErr("Cannot find start of first category column!");
      }
  }
  /**/lpt->tLog(fname,9);
  // chopping the post list off
  if (!xparser.ChopAfter("</td></tr></tbody></table>",true)) {
    /**/lpt->tLog(fname,10);
    return parsErr("Cannot find end of categories!");
  }
  out_section_name = buffer;
  return true;
}

/*****************************************************************************/
