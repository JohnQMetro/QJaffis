/*****************************************************************************
Name    : fetchbase.h
Basic   : Wraps the core page fetcher with some additional functions
Author  : John Q Metro
Started : June 16, 2021
Updated : June 29, 2022

******************************************************************************/
#ifndef PAGEFETCH_H
    #define PAGEFETCH_H
#endif // PAGEFETCH_H
#ifndef FETCHBASE_H
    #include "fetchbase.h"
#endif // FETCHBASE_H
//---------------------------
#include <QString>
#include <QMutex>
/*****************************************************************************/
/* abstract 'interface' for site specific tests and functions used in
 * the page fetcher. */
class jfTestsDelegate {
  public:
    // this is mostly used for the tester, but it could be used for other things
    virtual bool setPageIndex(const size_t& page_index) = 0;

    virtual QString GetCookie() const = 0;
    virtual bool TestIsBlocked(const QString* downpage) const = 0;
    virtual bool TestNotMissing(const QString* downpage) const = 0;
    virtual bool TestNotTruncated(const QString* downpage) const = 0;
    virtual bool ResolveRedirect(const QString& raw_redirect, QString& full_redirect) const = 0;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class jfPageFetcher {
  public:
    // constructor
    jfPageFetcher(jfPageFetchInterface* ifetchcore, jfTestsDelegate* itester, bool docheck_clientredir);

    // the main download method
    jfFETCH_ERROR Download(const QString& inurl, const size_t& page_index);
    bool Abort();

    // status info
    bool IsDownloading() const;
    jfFETCH_ERROR GetLastError() const;
    QUrl GetDownlodUrl() const;
    bool HasResult() const;

    // redirection
    QString* GetResult();
    QString GetFullRedirection() const;

    // specials
    int GetRetryAfter() const;
    size_t GetErrorCount(jfFETCH_ERROR pick_which);

    // destructor
    virtual ~jfPageFetcher();

  protected:
    bool TestPageDelegates(const QString* newpage);
    bool ParseForClient(const QString* newpage); // parses the result for client side redirection
    bool HandleRedirect();
    bool ProcessNoError();

    bool is_downloading;

    // stored parameters
    QString fetch_this;    // url to fetch the webpage
    jfPageFetchInterface* fetchcore;
    jfTestsDelegate* tester;
    bool check_clientred;

    // results
    QString* thepage;       // the result of the download, if things go well
    QString redirectto;     // if there is a redirection, we store it here
    jfFETCH_ERROR theerror; // we store our simplified error here

    // misc extra stuff
    size_t errlog[10];       // counting all the errors for debugging
    mutable QMutex flock;

};
/*****************************************************************************/
