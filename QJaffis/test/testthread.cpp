/*****************************************************************************
Name    : testthread.cpp
Basic   : Thread Download/Parse for testing (testing download and parse for sites)
Author  : John Q Metro
Started : March 25, 2018
Updated : July 7, 2022
******************************************************************************/
#ifndef TESTTHREAD_H
    #include "testthread.h"
#endif // TESTTHREAD_H
//-----------------------------------------------

#ifndef DOWNLOADROOT_H
    #include "../fetching/loopget/downloadroot.h"
#endif // DOWNLOADROOT_H
#ifndef TESTPARSE_H
    #include "testparse.h"
#endif // TESTPARSE_H
#ifndef TESTURL_H
    #include "testurl.h"
#endif // TESTURL_H
#ifndef TESTFETCH_H
    #include "testfetch.h"
#endif // TESTFETCH_H

#ifndef GLOBALSETTINGS_H
    #include "../globalsettings.h"
#endif // GLOBALSETTINGS_H

#ifndef DEFAULTPATHS_H
    #include "../defaultpaths.h"
#endif // DEFAULTPATHS_H

#ifndef INITEND_H_INCLUDED
  #include "../initend.h"
#endif // INITEND_H_INCLUDED
//-----------------------------------------------------
#include <assert.h>
/*****************************************************************************/
jfTestDownloader::jfTestDownloader():jfDownloadRoot() {
    pagecount = test::count;
    skip_on_fail = true;
    wrap_parser = NULL;
}
jfTestDownloader::~jfTestDownloader() {
    if (wrap_parser != NULL) {
        delete wrap_parser;
        wrap_parser = NULL;
    }
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
    // doing it
    /**/tLog(fname,2);
    SetupWorkers(false);
    /**/tLog(fname,5);
    bool do_result = xLoopGet();
    ClearWorkers(true);
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
jfParseFetchPackage* jfTestDownloader::MakeParserFetcher() {
    // hardcoded fanfiction.net python fetcher
    jglobal::jfFetchBasics fetch_type = jglobal::settings.FindFetchTypeFor(jfft_FFN, jglobal::FPT_FICPART_PAGE);
    jfPythonPaths* ffn_path = jglobal::settings.getFetchPackageMaker()->makePathsFor(fetch_type);
    if (ffn_path == NULL) return NULL;

    // finishing
    jfTestsFetcherWrapper* fetcher = new jfTestsFetcherWrapper(NULL,ffn_path);
    if (wrap_parser == NULL) wrap_parser = new jfTestWrapParser();

    return new jfParseFetchPackage(wrap_parser,fetcher,true);
}

/*****************************************************************************/

