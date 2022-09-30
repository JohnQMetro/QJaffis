/*****************************************************************************
Name    : fetchbase.cpp
Basic   : Core enums and abstract page fetch
Author  : John Q Metro
Started : June 15, 2021
Updated : June 28, 2021

******************************************************************************/
#ifndef FETCHBASE_H
    #include "fetchbase.h"
#endif // FETCHBASE_H

//----------------------------
#include <QMutexLocker>
/******************************************************************************/
jfPageFetchInterface::jfPageFetchInterface() {
    isdownloading = false;
    fetch_this = QUrl();
    thepage = NULL;
    redirectto = "";
    theerror = jff_NOERROR;
    limit_wait_time = -1;
}
//++++++++++++++++++++++++++++++++++
bool jfPageFetchInterface::IsDownloading() const {
    QMutexLocker tl(&pfilock);
    return isdownloading;
}
jfFETCH_ERROR jfPageFetchInterface::GetError() const {
    QMutexLocker tl(&pfilock);
    return theerror;
}
QUrl jfPageFetchInterface::GetDownlodUrl() const {
    QMutexLocker tl(&pfilock);
    return fetch_this;
}
//++++++++++++++++++++++++++++++++++
// result info
bool jfPageFetchInterface::HasResult() const {
    QMutexLocker tl(&pfilock);
    return (thepage != NULL);
}
QString* jfPageFetchInterface::GetResult() {
    QMutexLocker tl(&pfilock);
    QString* tpage = thepage;
    thepage = NULL;
    return tpage;
}
QString jfPageFetchInterface::GetRedirection() const {
    QMutexLocker tl(&pfilock);
    return redirectto;
}

QString jfPageFetchInterface::GetRedirectTarget() const {
    QMutexLocker tl(&pfilock);
    if (fetch_this.isValid() && (!redirectto.isEmpty())) {
        QUrl relative,result;
        relative.setUrl(redirectto);
        result = fetch_this.resolved(relative);
        return result.toString();
    }
    else return "";
}
int jfPageFetchInterface::GetLimitWaitTime() const {
    QMutexLocker tl(&pfilock);
    return limit_wait_time;
}
//++++++++++++++++++++++++++++++++++
// destructor
jfPageFetchInterface::~jfPageFetchInterface() {
    if (thepage != NULL) {
        delete thepage;
        thepage = NULL;
    }
}
//+++++++++++++++++++++++++++++++++++
bool jfPageFetchInterface::SetCheckUrl(const QString& inurl, bool http_only) {
    fetch_this.clear();
    QUrl temp = QUrl(inurl,QUrl::StrictMode);
    if ((!temp.isValid()) || (temp.isRelative())) return false;
    if (http_only) {
        QString sch = temp.scheme().toLower();
        if ((sch != "http") && (sch != "https")) return false;
    }
    fetch_this = temp;
    return true;
}
//-------------------------------------
bool jfPageFetchInterface::ClearOld() {
    if (thepage != NULL) {
        delete thepage;
        thepage = NULL;
    }
    redirectto = "";
    limit_wait_time = -1;
    return true;
}
//----------------------------------------
jfFETCH_ERROR jfPageFetchInterface::PreDownloadChecks(const QString& inurl) {
    if (isdownloading) return jff_BUSY;
    if ( !SetCheckUrl(inurl, true) ) {
        jfFETCH_ERROR terr = jff_BADURL;
        theerror = terr;
        ClearOld();
        return terr;
    }
    else return jff_NOERROR;
}


/******************************************************************************/
