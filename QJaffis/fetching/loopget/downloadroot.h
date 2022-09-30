/*****************************************************************************
Name    : downloadroot.h
Basic   : New base download controller (for single thread sequential fetch)
Author  : John Q Metro
Started : July 10, 2021
Updated : July 6, 2022
******************************************************************************/
#ifndef DOWNLOADROOT_H
    #define DOWNLOADROOT_H
#endif // DOWNLOADROOT_H
//-------------------------------
#ifndef DOWNBASE_H
    #include "../downbase.h"
#endif // DOWNBASE_H
#ifndef GETPAGE_H
    #include "getpage.h"
#endif // GETPAGE_H
#ifndef TESTSWRAPPER_H
    #include "../core/testswrapper.h"
#endif // TESTSWRAPPER_H
//---------------------
#include <QObject>
/*****************************************************************************/
// a wrapper class for parser and downloader
class jfParseFetchPackage {
  public:
    retry_times retry; // structure with auto-initialized fields
    // constructors
    jfParseFetchPackage(jfPageParserCore* parser_in, jfTestsDelegate* itester_in, jfPageFetchInterface* ifetchcore_in, bool internal_fetch);
    jfParseFetchPackage(jfPageParserBase* old_parser_in, jfPageFetchInterface* ifetchcore_in, bool internal_fetch);
    // access methods
    jfGetPage* GetFetcher() const;
    jfPageParserCore* GetParser() const;
    jfTestsDelegate* GetDownloadTester() const;
    // other
    bool SetupFetcher(bool client_redirection_check);
    bool Shutdown();
    ~jfParseFetchPackage();

  protected:
    jfPageFetchInterface* ifetchcore;
    bool delete_fetchcore;

    jfTestsDelegate* itester;
    jfPageParserCore* parser;
    jfPageParserBase* old_parser;
    jfTestsParserWrapper* old_wrapper;

    jfPageFetcher* inner_fetcher;
    jfGetPage* fetcher;
};

//---------------------------------------------
// core downloader thread base class, only one thread
class jfDownloadRoot : public jfDownloaderBase {
    Q_OBJECT
  public:
    jfDownloadRoot();
  protected:
    // virtual methods that have to be implemented by derived threads
    virtual QString* makeURLforPage(size_t index) const = 0;
    virtual bool ProcessResults(void* resultdata) = 0;
    virtual void DisposeResults(void* resultdata) = 0;
    virtual void PrepareItemInfo(size_t pageIndex) = 0;
    virtual bool advanceFetchIndex() = 0;
    virtual bool advanceResultIndex() = 0;
    virtual jfParseFetchPackage* MakeParserFetcher() = 0;
    // methods implemented here
    virtual void SetupWorkers(bool do_client_redirection_check);
    bool handlePagecount(size_t pagecount_in);
    virtual void ClearWorkers(bool delete_parser);
    bool xLoopGet();
    // indexes
    size_t url_page_index;
    size_t result_page_index;

    // objects...
    jfParseFetchPackage* parse_fetch;
    jfGetPage* fetcher;

};


/*****************************************************************************/

