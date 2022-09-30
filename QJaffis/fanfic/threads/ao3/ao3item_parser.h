/******************************************************************************
Name    :   ao3item_parser.h
Author  :   John Q Metro
Purpose :   Parser for AO3 fandompages, to get lists offanfics
Created :   July 16, 2016
Updated :   July 30, 2022 (added string counter)
******************************************************************************/
#ifndef AO3ITEM_PARSER_H
  #define AO3ITEM_PARSER_H
#endif // AO3ITEM_PARSER_H
//-----------------------------------
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef AO3_CATS_H_INCLUDED
  #include "../../data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED


#include "../../../core/tagfilter/stringcounter.h"
/*****************************************************************************/
class jfAO3ItemParser : public jfItemsPageParserBase {
  public:
    // constructor
    jfAO3ItemParser();
    bool SetCategory(const jfAO3_Category* in_category);
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    // getting tag counts
    bool SaveCounts() const;
    // also
    virtual QString getCookie() const;
    virtual ~jfAO3ItemParser();
  protected:
    // helper methods for parsing
    bool ProcessStart(size_t& itemcount);
    bool FicParsingLoop();

    const jfAO3_Category* this_category;

    jfStringCounter* extra_tag_counter;

};
/*****************************************************************************/
