/******************************************************************************
Name    :   ffnitem_parser.h
Author  :   John Q Metro
Purpose :   Parser for Fanfiction.Net Category pages, to get lists of fanfics
Created :   July 22, 2016
Updated :   March 26, 2023
******************************************************************************/
#ifndef FFNITEM_PARSER_H
#define FFNITEM_PARSER_H
#endif // FFNITEM_PARSER_H
//-----------------------------------
#ifndef BASEPARSE_H
  #include "../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef JFFFNSCATCORE
  #include "../categories/ffn_catcore.h"
#endif // JFFFNSCATCORE
#ifndef JFFNFICOBJ
  #include "../data/ffn_ficobj.h"
#endif
#ifndef UPD_TYPES_H_INCLUDED
  #include "../../ffnupdate/data/upd_types.h"
#endif // UPD_TYPES_H_INCLUDED
//--------------------------------------
#include <QRegularExpression>
/*****************************************************************************/
class jfFFNItemParser : public jfItemsPageParserBase {
  public:
    // constructor
    jfFFNItemParser();
    bool SetCategory(const jfFFN_CategoryCore* in_category);
    bool setLimit(jfFicLimit* in_limit);
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual bool testRatelimit(const QString *page) const;
    // getting results
    // also
    virtual QString getCookie() const;
  protected:
    // helper methods for parsing
    bool CheckNames();
    bool CheckCrossoverName();
    bool PageAndItemCount(size_t& itemcount);
    virtual bool ExitCheck(const jfFFNItem* tocheck) const;

    const jfFFN_CategoryCore* this_category;
    bool newfic;
    size_t firstid;
    jfFicLimit* limit;
    bool limitreached;

    QRegularExpression page_header;
    QRegularExpression cross_link;
    QRegularExpression pagehead;
    QRegularExpression zlist_start;
    QRegularExpression item_ico;
};
/*****************************************************************************/

