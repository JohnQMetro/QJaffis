/*****************************************************************************
Name    : download.h
Basic   : Download controller threads
Author  : John Q Metro
Started : June 16, 2015
Updated : September 4, 2016
******************************************************************************/
#ifndef DOWNLOAD_H
  #define DOWNLOAD_H
#endif // DOWNLOAD_H
//-------------------------------
#ifndef THREADBASE_H
  #include "threadbase.h"
#endif // THREADBASE_H
#ifndef FETCHANDPARSE_H
  #include "fetchandparse.h"
#endif // FETCHANDPARSE_H
/*****************************************************************************/
// core downloader thread base class, managing multiple downloader threads
class jfBaseDownloader : public jfThreadBase {
    Q_OBJECT
  public:
    jfBaseDownloader(size_t in_max_threads);
    bool setPauser(jfPauseStop* in_pauser);
  protected slots:
    void handleThreadDone();
  protected:
    // virtual methods that have to be implemented by derived threads
    virtual QString* makeURLforPage(size_t index) const = 0;
    virtual bool ProcessResults(void* resultdata) = 0;
    virtual void DisposeResults(void* resultdata) = 0;
    virtual void PrepareItemInfo(size_t pageIndex) = 0;
    virtual bool advanceFetchIndex() = 0;
    virtual bool advanceResultIndex() = 0;
    virtual jfPageParserBase* makeParser() = 0;
    // methods implemented here
    void CalcLoopMax();
    void incrThreadIndex(size_t &thread_index);
    void SetupThreads(bool firstpage);
    bool HandleResults(jfDownloadResults* in_results);
    bool handlePagecount(size_t pagecount_in, size_t cindex);
    void SendHaltMessage();
    void EndWaitAndClean();
    bool LoopGet();
    void AllDone(bool okay);
    // sub thread 'constants'
    size_t max_threads;
    size_t actual_threads;
    size_t pagecount;
    size_t loopmax;
    // indexes
    size_t url_page_index;
    size_t result_page_index;
    size_t current_index;
    size_t done_threads;
    bool all_done;
    // thread handling
    std::vector<jfFetchAndParseHolder*> threads;
    // other stuff
    bool started;
    bool skip_on_fail;
    jfFETCH_AFTERMATH current_error;
    jfItemInfoMsg infoToSend;
    jfPauseStop* pauser;
};
//==========================================================================
/* A derived downloader base class, which always downloads the first page,
 * and checks the results before doing anything else */
class jfBaseFirstPageDownloader : public jfBaseDownloader {
  public:
    jfBaseFirstPageDownloader(size_t in_max_threads);
  protected:
    // implemented virtual methods
    virtual bool advanceFetchIndex();
    virtual bool advanceResultIndex();
    // first page abstract methods
    virtual void makeFirstPageInfo() = 0;
    virtual void ProcessFirstPageResults(void* resultdata) = 0;
    // special methods
    bool SetupFirstThread();
    bool DoFirstPage();
    bool FetchAllPages(size_t pagecount_estimate);
    // internal data
    bool abort_fetch; // set to true if we are not to download any more

};

/*****************************************************************************/
