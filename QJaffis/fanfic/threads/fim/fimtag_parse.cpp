/*****************************************************************************
Name    : fimtag_parse.cpp
Basic   : Parser for getting tag data out of a downloaded Fimfiction.net page
Author  : John Q Metro
Started : December 29, 2017
Updated : December 29, 2017

******************************************************************************/
#ifndef FIMTAG_PARSE_H
    #include "fimtag_parse.h"
#endif // FIMTAG_PARSE_H
//-----------------------------------
#include <assert.h>
/*****************************************************************************/
jfFIMTag_Extractor::jfFIMTag_Extractor():jfPageParserBase() {
    tag_data = NULL;
}
//-----------------------------------------------
// custom parse method
void jfFIMTag_Extractor::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
    tag_data = new jfFIMTag_Collection();
    // parsing the data
    page_parsed = tag_data->SetFromSource(inPage);
    pagecount = 1;
    if (!page_parsed) parseErrorMessage = "Parsing Error";
    // we write the report file from the thread...
    tag_data->ReportToFile("fimtag_report.txt");
}
//--------------------------------------
void* jfFIMTag_Extractor::getResults() {
    return tag_data;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// fimfiction.net doies not (currently) do redirection.
QString* jfFIMTag_Extractor::makeRedirectedURL(const QString& inPart) const {
    return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// test methods
//-------------------------
bool jfFIMTag_Extractor::testMissing(const QString *page) const {
    return true; // no special test
}
//------------------------
bool jfFIMTag_Extractor::testIncomplete(const QString *page) const {
    assert(page != NULL);
    // the page only has to be complete enought to include the data we want
    return page->contains("<div class=\"button-group collapsible search-view\"");
}
// ------------------------
bool jfFIMTag_Extractor::testRatelimit(const QString *page) const {
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// also
QString jfFIMTag_Extractor::getCookie() const {
    return "view_mature=true";
}

/*****************************************************************************/
