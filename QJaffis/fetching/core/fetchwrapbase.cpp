/*****************************************************************************
Name    : fetchwrapbase.cpp
Basic   : A base implementation of jfPageFetchInterface that wraps others.
Author  : John Q Metro
Started : June 18, 2022
Updated : June 18, 2022

******************************************************************************/
#ifndef FETCHWRAPBASE_H
    #include "fetchwrapbase.h"
#endif // FETCHWRAPBASE_H
//----------------------


/*****************************************************************************/
jfFetchWrapBase::jfFetchWrapBase():jfPageFetchInterface() {
    current_fetcher = NULL;
}
//++++++++++++++++++++++++++++++++++++++++
// implemented public methods
jfFETCH_ERROR jfFetchWrapBase::Download(const QString& inurl, const QString& cookie) {
    pfilock.lock();
    jfFETCH_ERROR startc = PreDownloadChecks(inurl);
    if (startc != jff_NOERROR) {
        pfilock.unlock();
        return startc;
    }
    // proceed with downloading
    isdownloading = true;
    jfPageFetchInterface* fetcher = FetcherForURL(inurl);
    current_fetcher = fetcher;
    jfFETCH_ERROR result = fetcher->Download(inurl, cookie);

    // setting from inner values
    theerror = result;
    if (result == jff_NOERROR) {
        thepage = fetcher->GetResult();
    }
    else if (result == jff_REDIRECTION) {
        redirectto = fetcher->GetRedirection();
    }
    else if (result == jff_RATELIMIT) {
        limit_wait_time = fetcher->GetLimitWaitTime();
    }
    isdownloading = false;
    // final steps
    jfFETCH_ERROR temp = theerror;
    pfilock.unlock();
    return temp;
}
//--------------------------
bool jfFetchWrapBase::Abort() {
    QMutexLocker mlock(&pfilock);
    if (current_fetcher == NULL) return false;
    else return current_fetcher->Abort();
}
// --------------------------
void jfFetchWrapBase::Shutdown() {
    QMutexLocker mlock(&pfilock);
    if (current_fetcher != NULL) {
        current_fetcher->Shutdown();
    }
}
//++++++++++++++++++++++++++++++++++++++++
jfFetchWrapBase::~jfFetchWrapBase() {
    current_fetcher = NULL;
}

/*****************************************************************************/
