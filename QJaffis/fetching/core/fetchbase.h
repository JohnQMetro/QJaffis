/*****************************************************************************
Name    : fetchbase.h
Basic   : Core enums and abstract page fetch
Author  : John Q Metro
Started : June 15, 2021
Updated : June 28, 2021

******************************************************************************/
#ifndef FETCHBASE_H
    #define FETCHBASE_H
#endif // FETCHBASE_H
//----------------------------
#include <QString>
#include <QUrl>
#include <QMutex>
#include <QObject>
/******************************************************************************/
// an enumerated type for page fetch error
enum jfFETCH_ERROR {
        jff_BADURL,         // url is not valid or not supported
        jff_BUSY,           // already at work
        jff_NOERROR,        // no error
        jff_TRYAGAIN,       // some sort of error, but not serious
        jff_RATELIMIT,      // rate limit error, try again but with more delay...
        jff_REDIRECTION,    // redirection, change the url and try again
        jff_MISSING,        // 404 not found (or some equivalent). Don't try again
        jff_FALIURE,        // an error that means trying again is futile
        jff_BLOCKED,        // blocked by cloudflare, or similar
        jff_HALTED,
        jff_INTERNAL
    };
//=============================================
// an abstract class for synchronously fetching pages
class jfPageFetchInterface : public QObject {
  public:
    // constructor
    jfPageFetchInterface();
    // main functional methods
    virtual jfFETCH_ERROR Download(const QString& inurl, const QString& cookie = "") = 0;
    virtual bool Abort() = 0;
    virtual void Shutdown() = 0;

    // status
    bool IsDownloading() const;
    jfFETCH_ERROR GetError() const;
    QUrl GetDownlodUrl() const;

    // result info
    bool HasResult() const; // true if thepage (result) is not NULL, we have a string
    QString* GetResult();   // if successful, returns the page and Resets the Results
    QString GetRedirection() const;  // returns the redirection result, which may be empty
    QString GetRedirectTarget() const; // like GetRedirection, but resolves the redirection first
    int GetLimitWaitTime() const;
    // destructor
    virtual ~jfPageFetchInterface();

  protected:
    bool SetCheckUrl(const QString& inurl, bool http_only);
    bool ClearOld();
    jfFETCH_ERROR PreDownloadChecks(const QString& inurl);
    bool isdownloading; // flag, are we downloading?
    QUrl fetch_this;    // url to fetch the webpage

  // results
    QString* thepage;       // the result of the download, if things go well
    QString redirectto;     // if there is a redirection, we store it here
    jfFETCH_ERROR theerror; // we store our simplified error here
    int limit_wait_time;

    mutable QMutex pfilock;

};
/******************************************************************************/
