/*****************************************************************************
Name    : downbase.h
Basic   : A new downloader base class, implemented to be a common base
          for the old jfBaseDownloader and new downloaders.
Author  : John Q Metro
Started : July 7, 2021
Updated :
******************************************************************************/
#ifndef DOWNBASE_H
    #define DOWNBASE_H
#endif // DOWNBASE_H
//-------------------------------
#ifndef THREADBASE_H
    #include "threadbase.h"
#endif // THREADBASE_H
#ifndef FETCHANDPARSE_H
    #include "fetchandparse.h"
#endif // FETCHANDPARSE_H
#ifndef CONNECT_HELPERS_H
    #include "connect_helpers.h"
#endif // CONNECT_HELPERS_H
/*****************************************************************************/
// core downloader thread base class,
class jfDownloaderBase : public jfThreadBase {
  public:
    jfDownloaderBase();
    bool setPauser(jfPauseStop* in_pauser);
    virtual ~jfDownloaderBase();
  protected:
    // virtual methods that have to be implemented by derived threads
    virtual QString* makeURLforPage(size_t index) const = 0;
    virtual bool ProcessResults(void* resultdata) = 0;
    virtual void DisposeResults(void* resultdata) = 0;
    virtual void PrepareItemInfo(size_t pageIndex) = 0;
    virtual bool advanceFetchIndex() = 0;
    virtual bool advanceResultIndex() = 0;

    // methods implemented here
    bool HandleResults(jfDownloadResults* in_results);
    void SendHaltMessage();
    void AllDone(bool okay);

    size_t pagecount;
    bool all_done;

    // other stuff
    bool started;
    bool skip_on_fail;
    jfFETCH_AFTERMATH current_error;
    jfItemInfoMsg infoToSend;
    jfPauseStop* pauser;
};

/*****************************************************************************/

