/*****************************************************************************
Name    : testthread.h
Basic   : Thread Download/Parse for testing (testing download and parse for sites)
Author  : John Q Metro
Started : March 25, 2018
Updated : May 8, 2022
******************************************************************************/
#ifndef TESTTHREAD_H
  #define TESTTHREAD_H
#endif // TESTTHREAD_H
//-----------------------------------------------
#ifndef DOWNLOADROOT_H
    #include "../fetching/loopget/downloadroot.h"
#endif // DOWNLOADROOT_H
#ifndef TESTPARSE_H
    #include "testparse.h"
#endif // TESTPARSE_H
/*****************************************************************************/
class jfTestDownloader : public jfDownloadRoot {
    Q_OBJECT
  public:
    jfTestDownloader();
    virtual ~jfTestDownloader();
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

    virtual jfParseFetchPackage* MakeParserFetcher();

    jfTestWrapParser* wrap_parser;

};
/*****************************************************************************/
