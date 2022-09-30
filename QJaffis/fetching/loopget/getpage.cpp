/*****************************************************************************
Name    : getpage.cpp
Basic   : New single threaded replacemeet for jfFetchAndParsePage
Author  : John Q Metro
Started : July 2, 2021
Updated : June 29, 2022

******************************************************************************/
#ifndef GETPAGE_H
    #include "getpage.h"
#endif // GETPAGE_H
//---------------------------------
#ifndef LOGGING_H_INCLUDED
    #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef BASEPARSE_H
    #include "../baseparse.h"
#endif // BASEPARSE_H
/*****************************************************************************/
jfGetPage::jfGetPage(retry_times rtimes_in) {
    fetching = false;
    rtimes = rtimes_in;
    parser = NULL;
    downloader = NULL;
    // external links
    pauser = NULL;
    lpt = NULL;
}
//------------------
bool jfGetPage::setFetcherAndParser(jfPageFetcher* in_fetcher, jfPageParserCore* in_parser) {
    const QString fname = "jfGetPage::setFetcherAndParser";
    jerror::AssertLog((in_fetcher != NULL) && (in_parser != NULL),fname,"Fetcher or Parser is NULL!");
    downloader = in_fetcher;
    parser = in_parser;
    return true;
}
//------------------
bool jfGetPage::setAncilliaryLinks(jfPauseStop* input_pauser, jfLoggBase* logger) {
    const QString fname = "jfGetPage::setAncilliaryLinks";
    jerror::AssertLog((input_pauser != NULL) && (logger != NULL),fname,"Pauser or Logger is NULL!");
    pauser = input_pauser;
    lpt = logger;
    parser->SetLogPointer(logger);
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the main external get method
jfDownloadResults* jfGetPage::GetAndParsePage(QString* url_to_get, size_t pageindex) {
    const QString fname = "jfGetPage::GetAndParsePage";

    void* res_data;
    size_t new_pagecount;
    jfDownloadResults* resultData;

    /**/lpt->tLog(fname,1);
    jfFETCH_ERROR currError = DownloadWithRetry(url_to_get, pageindex);

    if (currError==jff_NOERROR) {
        /**/lpt->tLog(fname,2);
        // time to parse!
        parser->setPageIndex(pageindex);
        parser->ParseDownloadedPage((*fetched_page), pageindex);
        delete fetched_page;
        fetched_page = NULL;
        // parsing went okay
        if (parser->isPageParsed()) {
            /**/lpt->tLog(fname,3);
            res_data = parser->getResults();
            new_pagecount = parser->getPageCount();
            /**/lpt->tLogS(fname,4,new_pagecount);

            resultData = new jfDownloadResults(res_data);
            resultData->pagecount = new_pagecount;
            return resultData;
        }
        // parsing did not go okay
        else {
            QString parse_err;
            parse_err = parser->getParseErrorMessage();
            /**/lpt->tParseError(fname,parse_err);
            resultData = new jfDownloadResults();
            (resultData->fetchResults).why_more = parse_err;
            (resultData->fetchResults).why = jfa_PARSEERR;
            (resultData->fetchResults).halt = true;
            /**/lpt->tLog(fname,5);
            return resultData;
        }
    }
    else {
        // sending back an error message instead
        /**/lpt->tLog(fname,8);
        // redirection
        if (currError == jff_REDIRECTION) {
            /**/lpt->tLog(fname,9);
            resultData = new jfDownloadResults(downloader->GetFullRedirection());
        }
        // some other problem
        else {
            /**/lpt->tLog(fname,10,jf_FetchErr2String(currError,false));
            resultData = new jfDownloadResults();
            (resultData->fetchResults).why = jf_Error2AfterMath(currError);
            /**/lpt->tLog(fname,11);
            (resultData->fetchResults).halt = true;
        }
        /**/lpt->tLog(fname,12);
        // passing back the result
        return resultData;
    }

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfGetPage::~jfGetPage() {
    // detaching external links
    parser = NULL;
    downloader = NULL;
    pauser = NULL;
    lpt = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGetPage::iPause(uint secs) const {
    unsigned long stime = secs*1000;
    jfSleepThread::msleep(stime);
}
//------------------------------------
// waiting to reetry in steps, with the opportunity to halt, pause, etc
bool jfGetPage::WaitToRetry(int full_wait) {
    unsigned long full_sleep = 1000 * rtimes.retry_step_time;
    int sleep_left = full_wait;
    while(sleep_left > 0) {
        pauser->SignalWaiting(sleep_left);
        if (sleep_left >= rtimes.retry_step_time) {
            jfSleepThread::msleep(full_sleep);
        }
        else {
            jfSleepThread::msleep(sleep_left*1000);
        }
        sleep_left -= rtimes.retry_step_time;
        if (pauser->CheckPauseStop()) return false; // halt opportunity
    }
    pauser->SignalStoppedWaiting();
    return true;
}
//------------------------------------
jfFETCH_ERROR jfGetPage::DownloadWithRetry(const QString* to_download, size_t pageindex) {
    const QString fname = "jfGetPage::DownloadWithRetry";
    // variables
    bool halting = false;
    uint sleep_time = rtimes.start_wait;
    uint retry_count = 0;
    bool download_okay = false;
    QString downurl = QString(*to_download);
    jfFETCH_ERROR currError = jff_NOERROR;
    /**/lpt->tLog(fname,1);
    // download
    while (retry_count <= rtimes.num_retries) {
        /**/lpt->tLogS(fname,2,retry_count);
        if (sleep_time > 0) iPause(sleep_time);
        halting = pauser->CheckPauseStop();
        /**/lpt->tLogB(fname,3,halting);
        if (halting) return jff_HALTED;
        currError = downloader->Download(downurl, pageindex);
        download_okay = (currError == jff_NOERROR);
        // if the download is fine, we get the page and exit the loop
        if (download_okay) {
            /**/lpt->tLog(fname,4);
            fetched_page = downloader->GetResult();
            break;
        }
        // handling a redirection...
        else if (currError == jff_REDIRECTION) {
            downurl = downloader->GetFullRedirection();
            /**/lpt->tLog(fname,5,downurl);
            sleep_time = rtimes.start_wait;
        }
        // handling rate limit
        else if (currError == jff_RATELIMIT) {
            int wait_time = downloader->GetRetryAfter();
            /**/lpt->tLogI(fname,6,wait_time);
            // unfortunatly, ao3 no longer gives a retry after time
            if (wait_time <= 0) wait_time = 600; // assume 10 min wait
            // special method
            bool dont_halt = WaitToRetry(wait_time);
            if (!dont_halt) {
                currError = jff_HALTED;
                break;
             }
             sleep_time = rtimes.start_wait;
             /**/lpt->tLog(fname,7);
        }
        else if (currError != jff_TRYAGAIN) {
            /**/lpt->tLog(fname,9);
            break;
        }
        // here, if we try again, it might work!
        else {
            /**/lpt->tLog(fname,10);
            retry_count++;
            // adjusting the sleep time
            if (retry_count == 1) sleep_time = rtimes.wait_increment;
            else if (rtimes.double_wait_time) sleep_time = 2*sleep_time;
            else sleep_time = sleep_time += sleep_time;
        }
        /**/lpt->tLog(fname,11);
        // QCoreApplication::processEvents();
    }
    halting = pauser->CheckPauseStop();
    if (halting) return jff_HALTED;
    /**/lpt->tLogB(fname,12,download_okay);
    if (download_okay) return jff_NOERROR;
    else return currError;

}

//************************************************************************************************************
