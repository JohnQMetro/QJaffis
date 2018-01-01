/******************************************************************************
Name    :   fimparse_compact.h
Author  :   John Q Metro
Purpose :   Fimfiction.net parser which gets summaries and totals from compact
            search results.
Created :   June 29, 2016
Updated :   June 29, 2016
******************************************************************************/
#ifndef FIMPARSE_COMPACT_H
#define FIMPARSE_COMPACT_H
#endif // FIMPARSE_COMPACT_H
//-----------------------------------
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef VECTORS_H_INCLUDED
  #include "../../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
/*****************************************************************************/
class jfFIMCompactParser:public jfPageParserBase {
  public:
    // constructor and data setting methods
    jfFIMCompactParser();
    void SetDoMature(const bool& inmature);
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart);
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    // also
    virtual QString getCookie() const;
  protected:
    // extra internal data
    jfIDStringStore* parsed_data;
    size_t extracted_itemcount;
    bool domature;

};
/*****************************************************************************/
