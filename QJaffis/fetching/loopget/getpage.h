/*****************************************************************************
Name    : getpage.h
Basic   : New single threaded replacemenet for jfFetchAndParsePage
Author  : John Q Metro
Started : July 1, 2021
Updated : June 29, 2022

******************************************************************************/
#ifndef GETPAGE_H
#define GETPAGE_H
#endif // GETPAGE_H
//---------------------------------------------
#ifndef LOGBASE_H
    #include "../logbase.h"
#endif // LOGBASE_H
#ifndef CONNECT_HELPERS_H
    #include "../connect_helpers.h"
#endif // CONNECT_HELPERS_H
#ifndef PAGEFETCH_H
    #include "../core/pagefetch.h"
#endif // PAGEFETCH_H
#ifndef THREADBASE_H
    #include "../threadbase.h"
#endif // THREADBASE_H
#ifndef BASEPARSE_H
    #include "../baseparse.h"
#endif // BASEPARSE_H
//---------------------------------------------

/*****************************************************************************/
// retry and wait time strategy
struct retry_times {
    // maximum number of retries before we quit
    uint num_retries{5};
    // initial wait time before fetch (including redirection
    uint start_wait{1};
    // wait time when we need to retry, to start with
    uint wait_increment{5};
    // true if the wait time after sucessive tries doubles instead of being incremented
    bool double_wait_time{false};
    // when waiting after too many requests, we sleep in steps
    uint retry_step_time{10};

};

//-------------------------------------
// a class for fetching and parsing pages repeatedly
class jfGetPage {
  public:
    // setup and setting data
    jfGetPage(retry_times rtimes_in);
    bool setFetcherAndParser(jfPageFetcher* in_fetcher, jfPageParserCore* in_parser);
    bool setAncilliaryLinks(jfPauseStop* input_pauser, jfLoggBase* logger);
    // the main external get method
    jfDownloadResults* GetAndParsePage(QString* url_to_get, size_t pageindex);
    // destructor
    ~jfGetPage();
  protected:
    void iPause(uint secs) const;
    bool WaitToRetry(int full_wait);
    jfFETCH_ERROR DownloadWithRetry(const QString* to_download, size_t pageindex);

    // internal page parsing object
    jfPageParserCore* parser;
    // downloader
    jfPageFetcher* downloader;
    // external links
    jfPauseStop* pauser;
    jfLoggBase* lpt;

    bool fetching;
    QString* fetched_page;

    retry_times rtimes;

};

/*****************************************************************************/
