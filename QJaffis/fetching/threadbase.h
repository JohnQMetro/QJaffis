/*****************************************************************************
Name    : threadbase.h
Basic   : Core thread class, with per thread logging and signals
Author  : John Q Metro
Started : October 16, 2015
Updated : May 22, 2021

******************************************************************************/
#ifndef THREADBASE_H
#define THREADBASE_H
#endif // THREADBASE_H
//----------------------------------------
#ifndef CONNECT_HELPERS_H
#include "connect_helpers.h"
#endif // CONNECT_HELPERS_H
#ifndef PAGEGETTER_H
  #include "pagegetter.h"
#endif // PAGEGETTER_H
#ifndef LOGBASE_H
    #include "logbase.h"
#endif // LOGBASE_H
//----------------------------------------
#include <QObject>
#include <QString>
#include <QSemaphore>
#include <QThread>
/*****************************************************************************/


//============================================================================
/* Base class for action threads. The signals are intended for progress
 * checking, and we include logging. */
class jfThreadBase : public jfLoggBase {
    Q_OBJECT
  public:
  public slots:
    virtual void StartProcessing() = 0;
  signals:
    void SendStart(bool all_okay);
    void SendItemCount(int eitemcount);
    void SendNewItem(struct jfItemInfoMsg itemdata);
    void SendNewItemAction(QString action_name);
    void SendItemDone();
    void SendReset();
    void SendError(struct jfFetchErrorBundle error_info);
    void SendAllDone(bool isfinished);
    void AllFinished();
  protected:

};
//============================================================================
/* When fetching and parsing data, we might need to send back not just results,
 * but possibly error information as well. */
class jfDownloadResults {
  public:
    // constructor
    jfDownloadResults();
    jfDownloadResults(void* newResultData);
    jfDownloadResults(const QString& failRedirection);
    // utility methods
    bool isOkay() const;
    // non-result data
    bool wasRedirected;
    QString newURL;
    jfFetchErrorBundle fetchResults;
    // result data
    void *resultData;
    size_t pagecount;
};

//============================================================================
/* For the new model, fetch and parse are done in the same thread, but we have
 * multiple threads. We send urls to these threads and get results back */
class jfThreadDataPasser : public QObject {
    Q_OBJECT
  public:
    jfThreadDataPasser();
    // sending url to download-parser
    void SendUrl(QString *URL_toGet, size_t in_pageindex);  // used by controller
    const QString* GetUrl(size_t &out_pageindex); // used by download-parser
    // returning results from download-parser
    void ReturnResults(jfDownloadResults* inResults);
    void ResultResultData(void* returnData, size_t new_pagecount);
    jfDownloadResults* GetResults();  // used by controller

  protected:
    // url sending
    QString* URL_Storage;
    size_t pageindex;
    QSemaphore dataEmpty;
    QSemaphore dataSet;
    // result returning
    jfDownloadResults* resultData;
    QSemaphore resultDataEmpty;
    QSemaphore resultDataSet;
};
//==========================================================================
// annoyingly, QThread sleep methods are not externally accessible
class jfSleepThread : public QThread {
public:
   static inline void msleep(unsigned long msecs) {
       QThread::msleep(msecs);
   }
};

//*****************************************************************************/
