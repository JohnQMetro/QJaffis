/*****************************************************************************
Name    : testfetch.cpp
Basic   : An implementation of jfPageFetchInterface that wraps others, for testing
Author  : John Q Metro
Started : June 12, 2022
Updated : June 18, 2022

******************************************************************************/
#ifndef TESTFETCH_H
    #include "testfetch.h"
#endif // TESTFETCH_H



/***************************************************************************/
jfTestsFetcherWrapper::jfTestsFetcherWrapper(const QStringList* user_agents, jfPythonPaths* python_paths):jfFetchWrapBase() {
    default_fetcher = new jfPageFetchWithQt(user_agents);
    python_fetcher = new jfPageFetchWithPython(python_paths);
}
// +++++++++++++++++++++++++++++++++++++
jfTestsFetcherWrapper::~jfTestsFetcherWrapper() {
    default_fetcher->Abort();
    default_fetcher->Abort();
    current_fetcher = NULL;
    delete default_fetcher;
    delete current_fetcher;

}
// +++++++++++++++++++++++++++++++++++++
jfPageFetchInterface* jfTestsFetcherWrapper::FetcherForURL(const QString& inurl) const {
    QUrl fetchUrl = QUrl(inurl);
    if (fetchUrl.isValid()) {
        QString host = fetchUrl.host();
        if (host.compare("fanfiction.net",Qt::CaseInsensitive) == 0) return python_fetcher;
        else if (host.endsWith(".fanfiction.net")) return python_fetcher;
    }
    return default_fetcher;
}


/***************************************************************************/
