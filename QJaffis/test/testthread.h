/*****************************************************************************
Name    : testthread.h
Basic   : Thread Download/Parse for testing (testing download and parse for sites)
Author  : John Q Metro
Started : March 25, 2018
Updated : March 29, 2018
******************************************************************************/
#ifndef TESTTHREAD_H
  #define TESTTHREAD_H
#endif // TESTTHREAD_H
//-----------------------------------------------
#ifndef DOWNLOAD_H
  #include "../fetching/download.h"
#endif // DOWNLOAD_H
/*****************************************************************************/
class jfTestDownloader : public jfBaseDownloader {
    Q_OBJECT
  public:
    jfTestDownloader(size_t in_max_threads);
  public slots:
    virtual void StartProcessing();
  protected:
    // virtual methods that have to be implemented by derived threads
    virtual QString* makeURLforPage(size_t index) const;
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    virtual void PrepareItemInfo(size_t pageIndex);
    virtual bool advanceFetchIndex();
    virtual bool advanceResultIndex();
    virtual jfPageParserBase* makeParser();

};
/*****************************************************************************/
