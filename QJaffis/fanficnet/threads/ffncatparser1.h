/*****************************************************************************
Name    : ffncatparser1.h
Basic   : Fanfiction.Net category parsing, base and non-crossover versions
Author  : John Q Metro
Started : July 19, 2016
Updated : September 5, 2022

******************************************************************************/
#ifndef FFNCATPARSER1_H
#define FFNCATPARSER1_H
#endif // FFNCATPARSER1_H
//------------------------------
#ifndef BASEPARSE_H
  #include "../../fetching/baseparse.h"
#endif // BASEPARSE_H
//----------------------------------
#ifndef CAT_COLLECTIONS_H
  #include "../categories/cat_collections.h"
#endif // CAT_COLLECTIONS_H

#include "ffncatparser_exp1.h"
/*****************************************************************************/
class jfFFNCatParserBase : public jfPageParserBase {
  public:
    // constructor
    jfFFNCatParserBase();

    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual bool ParsePageCore(size_t pageindex) = 0;
    virtual void* getResults() = 0;
    // implemented methods (pretty trivial)
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual QString getCookie() const;
  protected:

    jfFFNCategory_ParserExpressions regex;

};
//========================================================================
// this parser is supposed to parse FFN non-crossover section pages
// the result type is jfFFNSection
class jfFFNSectionParser : public jfFFNCatParserBase {
  public:
    // constructor
    jfFFNSectionParser();
    // custom methods
    virtual bool ParsePageCore(size_t pageindex);
    virtual void* getResults();
  protected:
    bool ParseStart(QString& out_section_name);
    jfFFNSection* result_data;
};

/*****************************************************************************/
