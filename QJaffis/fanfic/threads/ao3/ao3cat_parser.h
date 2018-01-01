/******************************************************************************
Name    :   ao3cat_parser.h
Author  :   John Q Metro
Purpose :   Parser for AO3 section pages to lists of categories/fandoms
Created :   June 21, 2016
Updated :   June 22, 2016
******************************************************************************/
#ifndef AO3CAT_PARSER_H
  #define AO3CAT_PARSER_H
#endif // AO3CAT_PARSER_H
//-----------------------------------
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef AO3_CATS_H_INCLUDED
  #include "../../data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
/*****************************************************************************/
class jfAO3CatParser : public jfPageParserBase {
  public:
    // constructor
    jfAO3CatParser();
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart);
    virtual QString getCookie() const;
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
  protected:
    jfAO3Section* catsection;

};
/*****************************************************************************/
