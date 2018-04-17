/*****************************************************************************
Name    : testthread.cpp
Basic   : Thread Download/Parse for testing (testing download and parse for sites)
Author  : John Q Metro
Started : March 25, 2018
Updated : March 30, 2018
******************************************************************************/
#ifndef TESTTHREAD_H
    #include "testthread.h"
#endif // TESTTHREAD_H
//-----------------------------------------------
#ifndef DOWNLOAD_H
  #include "../fetching/download.h"
#endif // DOWNLOAD_H
#ifndef TESTPARSE_H
    #include "testparse.h"
#endif // TESTPARSE_H
#ifndef TESTURL_H
    #include "testurl.h"
#endif // TESTURL_H
//-----------------------------------------------------
#include <assert.h>
/*****************************************************************************/
jfTestDownloader::jfTestDownloader(size_t in_max_threads):jfBaseDownloader(in_max_threads) {
    pagecount = test::count;
    skip_on_fail = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfTestDownloader::StartProcessing() {
    const QString fname = "jfTestDownloader::StartProcessing";
    // preliminaries
    initLog("Tester");
    emit SendStart(true);
    /**/tLog(fname,1);
    started = true;
    emit SendItemCount(pagecount);
    SetupThreads(false);
    // doing it
    /**/tLog(fname,5);
    bool do_result = LoopGet();
    AllDone(do_result);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString* jfTestDownloader::makeURLforPage(size_t index) const {
    return new QString(test::urls[index-1]);
}
//------------------------------------------------
bool jfTestDownloader::ProcessResults(void* resultdata) {
    // we don't do anything with the results
    DisposeResults(resultdata);
    return true;
}
//------------------------------------------------
void jfTestDownloader::DisposeResults(void* resultdata) {
    test::DeleteResults(result_page_index-1,resultdata);
}
//------------------------------------------------
void jfTestDownloader::PrepareItemInfo(size_t pageIndex) {
    assert(pageIndex >= 1);
    infoToSend.startaction = "Doing";
    infoToSend.item_label = "Test";
    infoToSend.item_index = pageIndex;
    infoToSend.part_count = pagecount;
    infoToSend.item_name = test::labels[pageIndex-1];
}
//------------------------------------------------
bool jfTestDownloader::advanceFetchIndex() {
    url_page_index++;
    return true;
}
//------------------------------------------------
bool jfTestDownloader::advanceResultIndex() {
    result_page_index++;
    return true;
}
//------------------------------------------------
jfPageParserBase* jfTestDownloader::makeParser() {
    return new jfTestWrapParser();
}
/*****************************************************************************/

