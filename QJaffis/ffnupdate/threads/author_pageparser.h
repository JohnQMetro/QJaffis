/*****************************************************************************
Name    : author_pageparser.h
Basic   : parserclass for downloaded Fanfiction.Net author pages
Author  : John Q Metro
Started : August 8, 2016
Updated : March 26, 2023

******************************************************************************/
#ifndef AUTHOR_PAGEPARSER_H
  #define AUTHOR_PAGEPARSER_H
#endif // AUTHOR_PAGEPARSER_H
//---------------------------------------------
#ifndef BASEPARSE_H
  #include "../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef AUTH_COLL_H_INCLUDED
  #include "../data/auth_coll.h"
#endif // AUTH_COLL_H_INCLUDED

#include "../data/upd_utils.h"
#include "../../fanficnet/data/ffn_ficutils.h"
//---------------------------------------------

/*****************************************************************************/
class jfFFNAuthorPageParser : public jfPageParserBase {
  public:
    // constructor
    jfFFNAuthorPageParser();
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    // also
    virtual QString getCookie() const;
    virtual ~jfFFNAuthorPageParser();
  protected:
    // parser helper methods
    bool ParseStart(size_t& authorid_out, QString& authorname_out);
    bool ParseTheRest();

    //completed data pointer
    jfAuthorStories* author_data;
    QRegularExpression footer;
    QRegularExpression item_start;

    jfFFNAuthorFanficParser* item_parser;

};
/*****************************************************************************/
