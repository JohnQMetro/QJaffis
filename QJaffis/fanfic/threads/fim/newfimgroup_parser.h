/******************************************************************************
Name    :   newfimgroup_parser.h
Author  :   John Q Metro
Purpose :   Now that there is a group search interface at Fimfiction.Net, the
old direct parser can be mostly replaced by a more typical item parser.
Created :   January 6, 2018
Updated :   January 6, 2018
******************************************************************************/
#ifndef NEWFIMGROUP_PARSER_H
    #define NEWFIMGROUP_PARSER_H
#endif // NEWFIMGROUP_PARSER_H
//----------------------------------
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
/*****************************************************************************/
class jfNewFIMGroupParser : public jfItemsPageParserBase {
  public:
    // constructor
    jfNewFIMGroupParser();
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart);
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    // another implemented method
    virtual QString getCookie() const;
  protected:
};
/*****************************************************************************/
