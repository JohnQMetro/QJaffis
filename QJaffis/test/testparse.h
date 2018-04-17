/*****************************************************************************
Name    : testparse.h
Basic   : Parsing for unit tests (this is actually an interface to other parsers)
Author  : John Q Metro
Started : March 18, 2018
Updated : March 30, 2018
******************************************************************************/
#ifndef TESTPARSE_H
    #define TESTPARSE_H
#endif // TESTPARSE_H
//-----------------------------------------
#ifndef BASEPARSE_H
    #include "../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef TESTURL_H
    #include "testurl.h"
#endif // TESTURL_H
//-----------------------------------

/*****************************************************************************/
class jfTestWrapParser : virtual public jfPageParserBase {
  public:
    // constructor
    jfTestWrapParser();
    virtual ~jfTestWrapParser();
    virtual void SetLogPointer(jfLoggBase* in_ptr);
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart);
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual bool setPageIndex(const size_t& page_index);
    // getting results
    size_t getPageCount() const;
    // also
    virtual QString getCookie() const;
  protected:
    // data
    size_t test_index;
    ParserList* parsers;

};
/*****************************************************************************/
