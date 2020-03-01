/*****************************************************************************
Name    : fetchandparse.h
Basic   : Basal threads for download and parsing
Author  : John Q Metro
Started : October 17, 2015
Updated : February 28, 2020

******************************************************************************/
#ifndef FETCHANDPARSE_H
#define FETCHANDPARSE_H
#endif // FETCHANDPARSE_H
//---------------------------------------------
#ifndef THREADBASE_H
  #include "threadbase.h"
#endif // THREADBASE_H
#ifndef PAGEGETTER_H
  #include "pagegetter.h"
#endif // PAGEGETTER_H
#ifndef BASEPARSE_H
  #include "baseparse.h"
#endif // BASEPARSE_H
//---------------------------------------------
#include <QThread>
#include <vector>
/*****************************************************************************/
// a class for fetching and parsing pages repeatedly
class jfFetchAndParsePage : public jfLoggBase {
    Q_OBJECT
  public:
    // setup and setting data
    jfFetchAndParsePage();
    bool setChannel(jfThreadDataPasser* in_channel);
    bool setParser(jfPageParserBase* in_parser);
    bool setPauser(jfPauseStop* input_pauser);
    // destructor
    ~jfFetchAndParsePage();
   signals:
    void processingFinished();
  public slots:
    void StartProcessing();
  protected:
    // helper methods
    bool DownloadPage();
    jfFETCH_ERROR DownloadMethod(uint pre_pause);
    void iPause(uint secs);
    // internal data storage
    QString urlToGet;
    size_t urlIndex;
    bool started;
    uint pause_before;
    QString *fetched_page;
    QString redirectionResult;
    // internal page parsing object
    jfPageParserBase* parser;
    // downloader
    jfFetchPage* downloader;
    // external links
    jfPauseStop* pauser;
    jfThreadDataPasser* channel;

};
//=========================================================
// for each fetch and download thread, we need to store some information...
class jfFetchAndParseHolder : public QObject {
    Q_OBJECT
  public:
    // constructor and destructor
    jfFetchAndParseHolder();
    void SetupLinks();
    ~jfFetchAndParseHolder();
    // info methods
    bool isActive() const;
    bool isFetching() const;
    // thread manipulation methods
    bool Start();
    bool SignalEnd();
    // thread data methods
    bool SetPauserAndParser(jfPauseStop* in_pauser, jfPageParserBase* in_parser);
    bool SendURL(QString* url_to_send, const size_t& fetch_page_index);
    jfDownloadResults* GetResults();
  signals:
    void ThreadDone();
  protected slots:
    void ThreadFinished();
  protected:
    jfThreadDataPasser* channel;
    QString* currentURL;
    QThread* thread_ptr;
    jfFetchAndParsePage* thread_object;
    bool thread_active;
    bool is_fetching;
    bool thread_finished;
};
//==============================================


/*****************************************************************************/
