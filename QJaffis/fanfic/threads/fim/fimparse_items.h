/******************************************************************************
Name    :   fimparse_item.h
Author  :   John Q Metro
Purpose :   Fimfiction.net parser for full item pages
            search results.
Created :   June 29, 2016
Updated :   March 20, 2023
******************************************************************************/
#ifndef FIMPARSE_ITEMS_H
#define FIMPARSE_ITEMS_H

#endif // FIMPARSE_ITEMS_H
//-----------------------------------
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef VECTORS_H_INCLUDED
  #include "../../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED

/*****************************************************************************/


class jfFIMPageParser : public jfItemsPageParserBase {
  public:
    // constructor
    jfFIMPageParser();
    bool SetShortSummaries(jfIDStringStore* in_short_summaries);
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual bool testRatelimit(const QString *page) const;
    // also
    virtual QString getCookie() const;
  protected:
    size_t extracted_itemcount;
    jfIDStringStore* short_summaries;
};
/*****************************************************************************/
