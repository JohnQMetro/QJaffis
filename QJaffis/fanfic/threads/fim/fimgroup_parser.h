/******************************************************************************
Name    :   fimgroup_parser.h
Author  :   John Q Metro
Purpose :   Group object for fimfiction.net
Created :   June 21, 2016
Updated :   June 22, 2016
******************************************************************************/
#ifndef FIMGROUP_PARSER_H
  #define FIMGROUP_PARSER_H
#endif // FIMGROUP_PARSER_H
//------------------------------------
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
/*****************************************************************************/
class jfFIMGroupParser : public jfItemsPageParserBase {
  public:
    // constructor
    jfFIMGroupParser();
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual bool testRatelimit(const QString *page) const;
    // another implemented method
    virtual QString getCookie() const;
  protected:
};

/*****************************************************************************/
