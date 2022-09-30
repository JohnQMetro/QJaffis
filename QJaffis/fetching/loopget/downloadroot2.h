/*****************************************************************************
Name    : downloadroot2.h
Basic   :
Author  : John Q Metro
Started : July 17, 2021
Updated : June 5, 2022
******************************************************************************/
#ifndef DOWNLOADROOT2_H
    #define DOWNLOADROOT2_H
#endif // DOWNLOADROOT2_H
//-------------------------
#ifndef DOWNLOADROOT_H
    #include "downloadroot.h"
#endif // DOWNLOADROOT_H
#ifndef RESULT_MULTI_H
  #include "../../core/structs/result_multi.h"
#endif // RESULT_MULTI_H
#ifndef DEFAULTPATHS_H
    #include "../../defaultpaths.h"
#endif // DEFAULTPATHS_H
#ifndef FICEXTRACT_H_INCLUDED
  #include "../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
//-------------------------
#include <QWaitCondition>
/*****************************************************************************/

class jfDownloadRootFirstPage : public jfDownloadRoot {
  public:
    jfDownloadRootFirstPage();
  protected:
    // implemented virtual methods
    virtual bool advanceFetchIndex();
    virtual bool advanceResultIndex();
    // first page abstract methods
    virtual void makeFirstPageInfo() = 0;
    virtual void ProcessFirstPageResults(void* resultdata) = 0;

    // virtual methods that are not defined here
    virtual QString* makeURLforPage(size_t index) const = 0;
    virtual void PrepareItemInfo(size_t pageIndex) = 0;

    virtual jfParseFetchPackage* MakeParserFetcher() = 0;

    // can be used to implement MakeParserFetcher
    jfParseFetchPackage* DefaultParseFetchMaker(jf_FICTYPE site, jglobal::jfFICPAGE_TYPE page_type, jfPageParserBase* page_parser) const;
    // special methods
    bool DoFirstPage();
    bool xFetchAllPages(size_t pagecount_estimate);
    // internal data
    bool abort_fetch; // set to true if we are not to download any more
};
//=======================================================
// downloader base class for searches that produce lists of items
class jfDownloadRootItems : public jfDownloadRootFirstPage {
  public:
    // the constructor
    jfDownloadRootItems();
    bool SetDataAndSearch(jfResultCollection* in_data);
    // wake
    void WakeAfter();
  protected:
    // virtual methods that are not defined here
    virtual QString* makeURLforPage(size_t index) const = 0;
    virtual void PrepareItemInfo(size_t pageIndex) = 0;
    virtual void makeFirstPageInfo() = 0;
    virtual void ProcessFirstPageResults(void* resultdata) = 0;

    virtual jfParseFetchPackage* MakeParserFetcher() = 0;

    // custom virtual methods
    virtual jfItemsPageParserBase* makeItemParser() = 0;

    // helper methods
    bool SendResults(jfResultUnitVector* res_vector) const;
    void WaitAfterSend();
    // implemented virtual methods
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    virtual jfPageParserBase* makeParser();
    // current collection
    jfUrlItemCollection* current_collection;
    jfResultCollection* maindata;
    jfSearchCore* search_object;
    bool doprocess;
    // wait!
    QMutex wlock;
    QWaitCondition syncher;

};


/*****************************************************************************/


