/*****************************************************************************
Name    : cloudfget.h
Basic   : Fetching using a python cloudscraper script. A class wrapper
Author  : John Q Metro
Started : January 1, 2021
Updated : January 2, 2021
Note: using tokens does not work, so fetching pages via the script directly
******************************************************************************/
#ifndef CLOUDFGET_H
#define CLOUDFGET_H
#endif // CLOUDFGET_H
//------------------------------
#include <QString>
#include <QUrl>
#include <QStringList>
#include <QMutex>
#include <QByteArray>
#include <QElapsedTimer>
#include <QNetworkRequest>
/*****************************************************************************/
class jfCloudScraperInterface;

namespace cloudscrape {
    extern jfCloudScraperInterface* ffn_interface;
    bool StartFFCloudscraper();
}
//++++++++++++++++++++++++++++++++++
class jfCloudScraperInterface {
  public:
    // constructor
    jfCloudScraperInterface();
    bool InitTokensForSite(const QString& site);
    //----------------------------------------
    bool AreTokensLoaded() const;
    QByteArray GetUserAgent() const;
    QByteArray GetHeaderCookie() const;
    bool SetTokens(QNetworkRequest* in_here, QString extra_cookie);
    //-----------------------------------------
    QStringList* FetchPages(QStringList* urls);
    QString* FetchOnePage(const QUrl& the_url );
  protected:
    void PacerWait(bool nostart);
    // a means fetch 1 url and also return tokens, b means fetch multiple urls, no tokens
    QStringList* MakeArgsTok(const QString& url) const;
    QStringList* MakeArgsPage(const QStringList& urls) const;
    QStringList* MakeArgs1Page(const QUrl& the_url) const;
    QStringList* RunPythonScript(const QStringList& args, bool readfile) const;
    bool ProcessTokResults(const QStringList& results);

    QMutex csi_lock;
    QElapsedTimer pacer;
    QString script_path;

    QByteArray user_agent;
    QByteArray cookie_value;
    bool tloaded;
    int tok_used;
};

/*****************************************************************************/
