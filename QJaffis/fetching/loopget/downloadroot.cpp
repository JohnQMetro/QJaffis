/*****************************************************************************
Name    : downloadroot.cpp
Basic   : New base download controller (for single thread sequential fetch)
Author  : John Q Metro
Started : July 10, 2021
Updated : June 22, 2022
******************************************************************************/
#ifndef DOWNLOADROOT_H
    #include "downloadroot.h"
#endif // DOWNLOADROOT_H
//-------------------------------
#ifndef DOWNBASE_H
    #include "../downbase.h"
#endif // DOWNBASE_H
#ifndef LOGGING_H_INCLUDED
    #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//---------------------
#include <QObject>
/*****************************************************************************/
/*
class jfParseFetchPackage {
  public:
    retry_times retry; // structure with auto-initialized fields
  */
// constructors
jfParseFetchPackage::jfParseFetchPackage(jfPageParserCore* parser_in, jfTestsDelegate* itester_in, jfPageFetchInterface* ifetchcore_in, bool internal_fetch) {
    bool any_null = (parser_in == NULL) || (itester_in == NULL) || (ifetchcore_in == NULL);
    jerror::AssertLog(any_null == false, "jfParseFetchPackage::jfParseFetchPackage (A)", "a parameter was null!");
    parser = parser_in;
    itester = itester_in;
    ifetchcore = ifetchcore_in;
    delete_fetchcore = internal_fetch;
    old_parser = NULL;
    old_wrapper = NULL;

    inner_fetcher = NULL;
    fetcher = NULL;
}
jfParseFetchPackage::jfParseFetchPackage(jfPageParserBase* old_parser_in, jfPageFetchInterface* ifetchcore_in, bool internal_fetch) {
    bool any_null = (old_parser_in == NULL) || (ifetchcore_in == NULL);
    jerror::AssertLog(any_null == false, "jfParseFetchPackage::jfParseFetchPackage (B)", "a parameter was null!");
    parser = NULL;
    itester = NULL;
    ifetchcore = ifetchcore_in;
    delete_fetchcore = internal_fetch;
    old_parser = old_parser_in;
    old_wrapper = new jfTestsParserWrapper(old_parser_in);

    inner_fetcher = NULL;
    fetcher = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// access methods
jfGetPage* jfParseFetchPackage::GetFetcher() const {
    return fetcher;
}
jfPageParserCore* jfParseFetchPackage::GetParser() const {
    if (old_parser == NULL) return  parser;
    else return old_parser;
}
jfTestsDelegate* jfParseFetchPackage::GetDownloadTester() const {
    if (old_wrapper == NULL) return itester;
    else return old_wrapper;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfParseFetchPackage::SetupFetcher(bool client_redirection_check) {
    if (ifetchcore == NULL) return false;
    if (old_parser == NULL) {
        if ((parser == NULL) || (itester == NULL)) {
            return false;
        }
    }
    inner_fetcher = new jfPageFetcher(ifetchcore, GetDownloadTester(), client_redirection_check);
    fetcher = new jfGetPage(retry);
    fetcher->setFetcherAndParser(inner_fetcher, GetParser());
    return true;
}

// other
bool jfParseFetchPackage:: Shutdown() {
    if (ifetchcore == NULL) return false;
    ifetchcore->Shutdown();
    return true;
}

jfParseFetchPackage::~jfParseFetchPackage() {
    // since the fetch core, parser, and tester are not created inside this class, we will not delete them here
    if (fetcher != NULL) {
        delete fetcher;
        fetcher = NULL;
    }
    if (inner_fetcher != NULL) {
        inner_fetcher->Abort();
        delete inner_fetcher;
        inner_fetcher = NULL;
    }
    if ((ifetchcore != NULL) && delete_fetchcore) {
        delete ifetchcore;
        ifetchcore = NULL;
    }
    if (old_wrapper != NULL) {
        delete old_wrapper;
        old_wrapper = NULL;
    }
    old_parser = NULL;
    itester = NULL;
    parser = NULL;

}

//=============================================================================
jfDownloadRoot::jfDownloadRoot():jfDownloaderBase() {
  url_page_index = 0;
  result_page_index = 0;
  parse_fetch = NULL;
  fetcher = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// initializes the downloaders, parser, and other objects
void jfDownloadRoot::SetupWorkers(bool do_client_redirection_check) {
    parse_fetch = MakeParserFetcher();
    parse_fetch->SetupFetcher(do_client_redirection_check);
    fetcher = parse_fetch->GetFetcher();
    fetcher->setAncilliaryLinks(pauser, this);
    // ??
}
//----------------------------------------
bool jfDownloadRoot::handlePagecount(size_t pagecount_in) {
    const QString fname = "jfDownloadRoot::handlePagecount";
    // logging old and new pagecount
    /**/tLogS(fname,1,pagecount);
    /**/tLogS(fname,2,pagecount_in);
    if (pagecount_in == pagecount) return true;
    // a pagecount of 0 is a signal that parsing cannot alter the pagecount
    if (pagecount_in == 0) return true;
    // pagecount has increased...
    else if (pagecount_in > pagecount) {
        /**/tLog(fname,3);
        pagecount = pagecount_in;
        emit SendItemCount(pagecount);
        // done
        return true;
    }
    // pagecount has decreased
    else {
        /**/tLog(fname,4);
        /* Not really a problem unless the new pagecount is less than the page
         * we have just finished parsing. I'm going to assume this must be in
         * the page data, so ProcessResults() will handle it. */
        if (pagecount_in < result_page_index) pagecount_in = result_page_index;
        pagecount = pagecount_in;
        /**/tLog(fname,5);
        emit SendItemCount(pagecount);
        return (pagecount != result_page_index);
    }
}

//----------------------------------------------
void jfDownloadRoot::ClearWorkers(bool delete_parser) {
    const QString fname = "jfDownloadRoot::ClearWorkers";
    parse_fetch->Shutdown();
    fetcher = NULL;
    if (delete_parser) {
        jfPageParserCore* parser = parse_fetch->GetParser();
        if (parser != NULL) delete parser;
    }
    delete parse_fetch;
    parse_fetch = NULL;
}

//----------------------------------------------
bool jfDownloadRoot::xLoopGet(){
    const QString fname = "jfDownloadRoot::LoopGet";
    // variables
    QString* url_to_fetch;
    jfDownloadResults* current_results;
    bool haltmode = false;
    size_t tpagecount;
    bool pcountr;
    bool halting;
    /**/tLog(fname,1);

    while(url_page_index < pagecount) {
        if (!advanceFetchIndex()) {
            /**/tLog(fname,2);
            break;
        }
        /**/tLog(fname,3);
        url_to_fetch = makeURLforPage(url_page_index);
        /**/tLog(fname,4,*url_to_fetch);

        // preparing to get the results (and we do send a message)
        if (!advanceResultIndex()) break;
        /**/tLogL(fname,5,result_page_index);
        PrepareItemInfo(result_page_index);
        /**/tLog(fname,6);
        emit SendNewItem(infoToSend);

        /**/tLogS(fname,7,result_page_index);
        // getting and handling the results
        current_results = fetcher->GetAndParsePage(url_to_fetch, url_page_index);
        tpagecount = current_results->pagecount;
        /**/tLogS(fname,8,tpagecount);
        haltmode = (!HandleResults(current_results)) && (!skip_on_fail);
        delete current_results;
        current_results = NULL;
        pcountr = handlePagecount(tpagecount);
        /**/tLogB(fname,9,pcountr);
        // if we halt, we break out of the loop
        if (haltmode || (!pcountr)) break;

        /**/tLogB(fname,10,pauser == NULL);
        // here, checking the pause/stop
        halting = pauser->CheckPauseStop();
        /**/tLog(fname,11);
        if (halting) {
            /**/tLog(fname,12);
            haltmode = true;
            current_error = jfa_HALT;
            SendHaltMessage();
            /**/tLog(fname,13);
            break;
        }
        /**/tLog(fname,14);

    }
    /**/tLog(fname,16);
    // done
    return !haltmode;
}
