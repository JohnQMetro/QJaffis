/*****************************************************************************
Name    : baseparse.h
Basic   : Declares a base class that handles page parsing (and redirection)
Author  : John Q Metro
Started : June 13, 2016
Updated : March 11, 2023

******************************************************************************/
#ifndef BASEPARSE_H
  #define BASEPARSE_H
#endif // BASEPARSE_H
//-----------------------------------
#ifndef STRINGPARSER_H_INCLUDED
  #include "../core/utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
#ifndef SEARCHCORE_H_INCLUDED
  #include "../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef THREADBASE_H
  #include "threadbase.h"
#endif // THREADBASE_H

#include "../core/objs/itemutils.h"
/*****************************************************************************/
/* a lower abstract parser class than jfPageParserBase, no redirection / truncate */
class jfPageParserCore {
public:
  // constructor
  jfPageParserCore();
  virtual ~jfPageParserCore();
  virtual void SetLogPointer(jfLoggBase* in_ptr);
  // custom parse method
  virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex) = 0;
  virtual void* getResults() = 0;
  // getting results
  virtual bool setPageIndex(const size_t& page_index) = 0;
  bool isPageParsed() const;
  QString getParseErrorMessage() const;
  size_t getPageCount() const;
protected:
  bool NewData(const QString& inPage);
  bool parsErr(const QString& err_in);
  // internal data
  jfStringParser xparser;
  bool page_parsed;
  QString parseErrorMessage;
  size_t pagecount;
  // external pointer
 jfLoggBase* lpt;

};

//===========================================================
// A base abstract class for parsing downloaded pages.
// Also handles redirection, and checking for truncated/missing pages
class jfPageParserBase: public virtual jfPageParserCore {
  public:
    // constructor
    jfPageParserBase();
    virtual ~jfPageParserBase();
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex) = 0;
    virtual void* getResults() = 0;
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart) const = 0;
    // test methods
    virtual bool testMissing(const QString *page) const = 0;
    virtual bool testIncomplete(const QString *page) const = 0;
    // getting results
    virtual bool setPageIndex(const size_t& page_index);
    // also
    virtual QString getCookie() const = 0;
    // virtual uint getPause() const = 0;
};
//===============================================================================
/* A still abstract derived class, for searches that produce lists of jfSearchResultItem
 * derived items when parsing each page. */
class jfItemsPageParserBase : public jfPageParserBase {
  public:
    // constructor
    jfItemsPageParserBase();
    bool setSearchPointer(jfSearchCore* in_sptr);
  protected:
    // helper methods
    jfResultUnitVector* MakeResults();
    // filtering and removing from the current page
    size_t FilterByList() const;
    size_t CategorySort();
    size_t Condense();
    void PostPageProcessing();

    // list of items produced
    size_t start_itemcount;
    std::vector<jfItemFlagGroup>* page_results;
    // useful external links
    jfSearchCore* search_ptr;

    jfItemParseBase* item_parser;
    jfResultItemOutputter* item_outputter;

};


/*****************************************************************************/
