/*****************************************************************************
Name    : fetchbase.cpp
Basic   : Wraps the core page fetcher with some additional functions
Author  : John Q Metro
Started : June 16, 2021
Updated : June 29, 2022

******************************************************************************/
#ifndef PAGEFETCH_H
    #include "pagefetch.h"
#endif // PAGEFETCH_H
#ifndef LOGGING_H_INCLUDED
    #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//---------------------------
#include <QMutexLocker>
/*****************************************************************************/
// constructor
jfPageFetcher::jfPageFetcher(jfPageFetchInterface* ifetchcore, jfTestsDelegate* itester, bool docheck_clientredir) {
    is_downloading = false;
    fetch_this = "";

    // stored parameters
    fetchcore = ifetchcore;
    tester = itester;
    check_clientred = docheck_clientredir;
    // results
    thepage = NULL;
    redirectto = "";
    theerror = jff_NOERROR;

    // misc extra stuff
    for (int x = 0; x < 10; x++) errlog[x] = 0u;

}
//++++++++++++++++++++++++++
// the main download method
jfFETCH_ERROR jfPageFetcher::Download(const QString& inurl, const size_t& page_index) {
    // initial flag/variable setting
    flock.lock();
    fetch_this = inurl;
    is_downloading = true;
    QString cookie;
    if (tester == NULL) cookie = "";
    else {
        tester->setPageIndex(page_index);
        cookie = tester->GetCookie();
    }
    flock.unlock();

    // the actual download
    jfFETCH_ERROR ierror = fetchcore->Download(inurl, cookie);

    // handling results
    flock.lock();
    // clearing some values
    if (thepage != NULL) {
        delete thepage;
        thepage = NULL;
    }
    redirectto = "";
    // checking the results
    if (ierror == jff_NOERROR) {
        // we have a page, but the results may still be bad...
        ProcessNoError();
    }
    else if (ierror == jff_REDIRECTION) {
        // redirection
        redirectto = fetchcore->GetRedirection();
        HandleRedirect();
    }
    else {
        // other errors
        theerror = ierror;
    }
    // final steps
    jfFETCH_ERROR oerror = theerror;
    errlog[(int)oerror] += 1u;
    is_downloading = false;
    flock.unlock();
    // done
    return oerror;
}
bool jfPageFetcher::Abort() {
    return fetchcore->Abort();
}
//+++++++++++++++++++++++++
// status info
bool jfPageFetcher::IsDownloading() const {
    QMutexLocker mlock(&flock);
    return is_downloading;
}
jfFETCH_ERROR jfPageFetcher::GetLastError() const {
    QMutexLocker mlock(&flock);
    return theerror;
}
QUrl jfPageFetcher::GetDownlodUrl() const {
    return fetchcore->GetDownlodUrl();
}
bool jfPageFetcher::HasResult() const {
    QMutexLocker mlock(&flock);
    return (thepage != NULL);
}
//++++++++++++++++++++++++++
// redirection
QString* jfPageFetcher::GetResult() {
    QMutexLocker mlock(&flock);
    QString* zpage = thepage;
    thepage = NULL;
    return zpage;
}
QString jfPageFetcher::GetFullRedirection() const {
    QMutexLocker mlock(&flock);
    return redirectto;
}
//++++++++++++++++++++++++++
// specials
int jfPageFetcher::GetRetryAfter() const {
    return fetchcore->GetLimitWaitTime();
}
size_t jfPageFetcher::GetErrorCount(jfFETCH_ERROR pick_which) {
    QMutexLocker mlock(&flock);
    return errlog[(int)pick_which];
}
//++++++++++++++++++++++++++
// destructor
jfPageFetcher::~jfPageFetcher() {
    fetchcore = NULL;
    tester = NULL;
    if (thepage != NULL) {
        delete thepage;
        thepage = NULL;
    }
}
//++++++++++++++++++++++++++++
bool jfPageFetcher::TestPageDelegates(const QString* newpage) {
    if (tester == NULL) return true;
    // blocked test
    bool is_blocked = tester->TestIsBlocked(newpage);
    if (is_blocked) {
        theerror = jff_BLOCKED;
        return false;
    }
    // missing test
    bool miss_test = tester->TestNotMissing(newpage);
    if (!miss_test) {
        theerror = jff_MISSING;
        return false;
    }
    // truncated
    bool trunc_test = tester->TestNotTruncated(newpage);
    if (!trunc_test) theerror = jff_TRYAGAIN;
    return trunc_test;
}
//------------------------
bool jfPageFetcher::ParseForClient(const QString* newpage) {
    // variables
    QString buffer, exturl;
    int strpos, endpos;
    // basic task
    if (newpage==NULL) return false;
    // starting
    strpos = newpage->indexOf("<META HTTP-EQUIV=\"refresh\"",0,Qt::CaseInsensitive);
    if (strpos==-1) return false;
    strpos += 26;
    endpos = newpage->indexOf(">",strpos);
    if (endpos == -1) return false;
    // extracting and then whitting things down to just the content
    buffer = newpage->mid(strpos,endpos-strpos);
    strpos = buffer.indexOf("CONTENT=",0,Qt::CaseInsensitive);
    if (strpos == -1) return false;
    strpos += 8;
    endpos = buffer.indexOf("\"",strpos);
    if (endpos == -1) return false;
    buffer = buffer.mid(strpos,endpos-strpos);
    buffer = buffer.trimmed();
    if (buffer.isEmpty()) return false;
    // the actual url should be found after the ;
    strpos = buffer.indexOf(';');
    if (strpos == -1) return false;
    buffer = buffer.mid(strpos+1);
    buffer = buffer.trimmed();
    if (buffer.isEmpty()) return false;
    // finally...
    redirectto = buffer;
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPageFetcher::HandleRedirect() {
    const QString fname = "jfPageFetcher::HandleRedirect";
    if (tester != NULL) {
        // option 1: use the delegate
        QString ntarget = "";
        if (tester->ResolveRedirect(redirectto, ntarget)) {
            redirectto = ntarget;
            theerror = jff_REDIRECTION;
            return true;
        }
        else {
            jerror::Log(fname,"Figuring out redirection failed, TO : " + redirectto);
            redirectto = "";
            theerror = jff_FALIURE;
            return false;
        }
    }
    else {
        // option 2: use fetchcore
        redirectto = fetchcore->GetRedirectTarget();
        if (redirectto == "") {
            theerror = jff_FALIURE;
            return false;
        }
        else {
            theerror = jff_REDIRECTION;
            return true;
        }
    }

}
//--------------------------------------
/* Here, the downloaded has reported success, but there still may be
 * something wrong with the downloaded page. */
bool jfPageFetcher::ProcessNoError() {
    const QString fname = "jfPageFetcher::ProcessNoError";
    QString* pagehere = fetchcore->GetResult();
    jerror::AssertLog(pagehere != NULL, fname, "The downloaded page was NULL despite no error!");
    // client-side redirection check
    if (check_clientred) {
        if (ParseForClient(pagehere)) {
            // we have client-side redirection
            HandleRedirect();
            delete pagehere;
            return false;
        }
    }
    // special tests for missing or truncated
    bool pass_test = TestPageDelegates(pagehere);
    if (pass_test) {
        // things are fine!
        theerror = jff_NOERROR;
        thepage = pagehere;
        return true;
    }
    // otherwise, the errors have already been set
    else return false;
}


/*****************************************************************************/
