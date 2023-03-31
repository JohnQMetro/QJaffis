/******************************************************************************
Name    :   fimparse_item.cpp
Author  :   John Q Metro
Purpose :   Fimfiction.net parser for full item pages
            search results.
Created :   June 30, 2016
Updated :   March 20, 2023
******************************************************************************/
#ifndef FIMPARSE_ITEMS_H
  #include "fimparse_items.h"
#endif // FIMPARSE_ITEMS_H
//-----------------------------------
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef FIM_SEARCH_H_INCLUDED
  #include "../../data/fim/fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED
#ifndef FIM_FICOBJ_H_INCLUDED
  #include "../../data/fim/fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED

#include "../../data/fim/fim_ficutils.h"
//--------------------------------
#include <assert.h>
#include <math.h>
/*****************************************************************************/
// theconstructor
jfFIMPageParser::jfFIMPageParser():jfItemsPageParserBase() {
  extracted_itemcount = 0;
  short_summaries = NULL;
  item_outputter = new jfFIMFanficOutputter();
  item_parser = new jfFIMFanficItemParser();
}
//-------------------------------------------
bool jfFIMPageParser::SetShortSummaries(jfIDStringStore* in_short_summaries) {
  if (in_short_summaries == NULL) return false;
  short_summaries = in_short_summaries;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// custom parse method
void jfFIMPageParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
    const QString fname = "jfFIMPageParser::ParseDownloadedPage";
    // starting...
    NewData(inPage);
    // string constants
    // const QString rerror = "jfFIMItemCollection::ProcessPage";
    const QString partstart = "<article class=\"story_container\"";
    const QString partend   = "</article>";
    const QString avtag     = "<img src=\"//www.fimfiction-static.net/images/avatars/";
    // working variables
    jfFIMFanficItemParser* parser = dynamic_cast<jfFIMFanficItemParser*>(item_parser);
    ulong tval;

    // starting...
    /**/lpt->tLog(fname,1);
    // moving to the sidebar which includes the story count...
    if (!xparser.MovePastAlt("<!-- Sidebar Responsive -->","<div class=\"two-columns\"")) {
        parseErrorMessage = "Cannot find sidebar!";
        return;
    }
    /**/lpt->tLog(fname,2);
    // finding the search results count, which has now been moved to the bottom of the page
    if (!xparser.MovePast("Search Statistics</h1>")) {
        parseErrorMessage = "Statistics not found!";
        return;
    }
    /**/lpt->tLog(fname,3);
    // setting up some variables...
    QString perr;
    // getting the item count
    if (!xparser.GetDelimitedULong("Found <b>","</b>",tval,perr)) {
        parseErrorMessage = "Count not extractable : " + perr;
        return;
    }
    extracted_itemcount = tval;
    /**/lpt->tLogS(fname,4,extracted_itemcount);
    // estimating the pagecount from the itemcount...
    pagecount = ceil(float(extracted_itemcount)/10.0);
    // moving past most of the pure gunk
    if (!xparser.MovePast("<div class=\"topbar-shadow\">")) {
        parseErrorMessage = "Topbar-shadow marker not found!";
        return;
    }
    // we no proceed to extracting the fics
    page_results = new std::vector<jfItemFlagGroup>();
    /**/lpt->tLog(fname,5);
    QString buffer;
    jfItemFlagGroup temp;

    while (xparser.GetDelimited(partstart,partend,buffer)) {
        jfItemParseResultState result = parser->ParseFromSource(buffer);

        // the item is okay
        if (result == jfItemParseResultState::SUCCEESS) {
            jfFIM_Fanfic* fanfic = parser->GetFanfic();
            if (short_summaries!=NULL) {
                fanfic->SetCompactSummary(short_summaries->GetString(fanfic->GetId()));
            }
            temp.item = fanfic;
            temp.flags = new jfItemMetaFlags();
            // adding
            page_results->push_back(temp);
            parser->Clear();
        }
        // parsing went fine, but the item was bad...
        else if (result == jfItemParseResultState::DEFECTIVE) {
            // ignore
            parser->Clear();
        }
        // parsing failed
        else {
            parseErrorMessage = parser->LastError() + "\nIN :\n" + buffer;
            /**/lpt->tParseError(fname,parseErrorMessage);
            parser->Clear();
            return;
        }
    }
    page_parsed = true;
    /**/lpt->tLog(fname,7);
    // post page processing and getting the result
    PostPageProcessing();
    /**/lpt->tLog(fname,8);
}
//--------------------------------------------
void* jfFIMPageParser::getResults() {
  return MakeResults();
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// handling redirection, NULL result if faliure
QString* jfFIMPageParser::makeRedirectedURL(const QString& inPart) const{
  return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// test methods
//----------------------------
bool jfFIMPageParser::testMissing(const QString *page) const {
  return true;
}
//----------------------------
bool jfFIMPageParser::testIncomplete(const QString *page) const {
  assert(page != NULL);
  return page->contains("<!--Footer-->");
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// the cookie
QString jfFIMPageParser::getCookie() const {
  jfFIMSearch* typed_search = dynamic_cast<jfFIMSearch*>(search_ptr);
  if (typed_search == NULL)return "view_mature=true";
  else if (typed_search->GetMature()) return "view_mature=true";
  else return "";

}

/*****************************************************************************/
