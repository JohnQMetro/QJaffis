/*****************************************************************************
Name    : testparse.cpp
Basic   : Parsing for unit tests (this is actually an interface to other parsers)
Author  : John Q Metro
Started : March 18, 2018
Updated : March 24, 2018
******************************************************************************/
#ifndef TESTPARSE_H
    #include "testparse.h"
#endif // TESTPARSE_H
//-----------------------------------------

/*****************************************************************************/
jfTestWrapParser::jfTestWrapParser():jfPageParserBase() {
    parsers = test::makeParsers();
    pagecount = test::count;
}
//--------------------------------------------------
jfTestWrapParser::~jfTestWrapParser() {
    test::FreeParsers(parsers);
    delete parsers;
}
//--------------------------------------------------
void jfTestWrapParser::SetLogPointer(jfLoggBase* in_ptr) {
    lpt = in_ptr;
    for (size_t xindex = 0; xindex < test::count ; xindex++ ) {
        (*parsers)[xindex]->SetLogPointer(in_ptr);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse method
void jfTestWrapParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
    const QString fname = "jfTestWrapParser::ParseDownloadedPage";
    /**/lpt->tLogS(fname,1,test_index);
    size_t use_index = test::pageindexes[test_index];
    /**/lpt->tLogS(fname,2,use_index);
    /**/lpt->tLogS(fname,3,parsers != NULL);
    (*parsers)[test_index]->ParseDownloadedPage(inPage,use_index);
    /**/lpt->tLog(fname,4);
    page_parsed = (*parsers)[test_index]->isPageParsed();
    /**/lpt->tLogB(fname,5,page_parsed);
    parseErrorMessage = (*parsers)[test_index]->getParseErrorMessage();
    /**/lpt->tParseError(fname,parseErrorMessage);
}
//----------------------------------------------------------------
void* jfTestWrapParser::getResults() {
    return (*parsers)[test_index]->getResults();
}
//------------------------------------------------------------
// handling redirection, NULL result if faliure
QString* jfTestWrapParser::makeRedirectedURL(const QString& inPart) const {
    return (*parsers)[test_index]->makeRedirectedURL(inPart);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// test methods
//--------------------------------------
bool jfTestWrapParser::testMissing(const QString *page) const {
    return (*parsers)[test_index]->testMissing(page);
}
//--------------------------------------
bool jfTestWrapParser::testIncomplete(const QString *page) const {
    return (*parsers)[test_index]->testIncomplete(page);
}
//---------------------------------------
bool jfTestWrapParser::setPageIndex(const size_t& page_index) {
    test_index = page_index - 1;
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfTestWrapParser::getCookie() const {
    const QString fname = "jfTestWrapParser::getCookie";
    /**/lpt->tLogS(fname,1,test_index);
    return (*parsers)[test_index]->getCookie();
}
/*****************************************************************************/
