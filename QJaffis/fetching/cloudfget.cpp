/*****************************************************************************
Name    : cloudfget.cpp
Basic   : Defines a wrapper class for fetching using a python script (which
uses cloudscraper to bypass Cloudflare 'protection').
Author  : John Q Metro
Started : January 1, 2021
Updated : January 2, 2021
Started attempts to get past Cloudflare

******************************************************************************/
#ifndef CLOUDFGET_H
  #include "cloudfget.h"
#endif // CLOUDFGET_H
//---------------------
#ifndef LOGGING_H_INCLUDED
  #include "../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#ifndef THREADBASE_H
  #include "threadbase.h"
#endif // THREADBASE_H

#ifndef UTILS3_H_INCLUDED
  #include "../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//---------------------
#include <QCoreApplication>
#include <QProcess>
#include <QUrl>
/*****************************************************************************/
jfCloudScraperInterface* cloudscrape::ffn_interface = NULL;
bool cloudscrape::StartFFCloudscraper() {
    ffn_interface = new jfCloudScraperInterface();
    return ffn_interface->InitTokensForSite("https://www.fanfiction.net/crossovers/Myth-Adventures/5143/");
}

//================================================
const int pace_ms = 1200;
//------------------------
jfCloudScraperInterface::jfCloudScraperInterface() {
    script_path = QCoreApplication::applicationDirPath() + "/cloudflget.py";
    tloaded = false;
    pacer.invalidate();
    tok_used = 0;
}
//------------------------
bool jfCloudScraperInterface::InitTokensForSite(const QString& site) {
    const QString fname = "jfCloudScraperInterface::InitTokensForSite";
    // check site first
    /**/JDEBUGLOGS(fname,1,site)
    QUrl token_site = QUrl::fromEncoded(site.toAscii());
    if (!token_site.isValid()) return false;
    /**/JDEBUGLOG(fname,2)
    // lock and start
    csi_lock.lock();
    /**/JDEBUGLOG(fname,3)
    PacerWait(true);
    /**/JDEBUGLOG(fname,4)
    QStringList* args = MakeArgsTok(site);
    QStringList* res = RunPythonScript(*args, false);
    /**/JDEBUGLOG(fname,5)
    pacer.start();
    delete args;
    tok_used = 1;
    // checking the result
    if (res == NULL) {
        csi_lock.unlock();
        return false;
    }
    /**/JDEBUGLOG(fname,6)
    // processing results
    bool okay = ProcessTokResults(*res);
    delete res;
    tloaded = okay;
    /**/JDEBUGLOG(fname,7)
    // done
    csi_lock.unlock();
    return okay;
}

//++++++++++++++++++++++++++++++++++++++++++++++
bool jfCloudScraperInterface::AreTokensLoaded() const {
    return tloaded;
}
//------------------------
QByteArray jfCloudScraperInterface::GetUserAgent() const {
    return user_agent;
}
//------------------------
QByteArray jfCloudScraperInterface::GetHeaderCookie() const {
    return cookie_value;
}
//-----------------------
/* Inserts cookie and user-agent inside a QNetworkRequest, therefore hopefully
 * allowing it to be used to fetch pages from the site without issue. */
bool jfCloudScraperInterface::SetTokens(QNetworkRequest* in_here, QString extra_cookie) {
    const QString fname = "jfCloudScraperInterface::SetTokens";
    // initial checks and guards
    jfAssertLog(in_here != NULL, fname, "The request parameter was null!");
    csi_lock.lock();
    /**/JDEBUGLOG(fname,1)
    if (!tloaded) {
        csi_lock.unlock();
        return false;
    }
    /**/JDEBUGLOG(fname,2)
    // since cloudflare blocks too frequent requests, we wait if need be
    PacerWait(false);
    /**/JDEBUGLOG(fname,3)
    // inserting the user agent
    in_here->setRawHeader("User-Agent", user_agent);
    /**/JDEBUGLOG(fname,4)
    // inserting the cookie
    if (extra_cookie.isNull() || extra_cookie.isEmpty()) {
        in_here->setRawHeader("Cookie", cookie_value);
    }
    else {
        in_here->setRawHeader("Cookie", cookie_value + ("; " + extra_cookie).toAscii());
    }
    /**/JDEBUGLOG(fname,5)
    // done
    tok_used += 1;
    csi_lock.unlock();
    return true;
}
//---------------------------------------------------------
QStringList* jfCloudScraperInterface::FetchPages(QStringList* urls) {
    // basic input checking
    if ((urls == NULL) || (urls->count() == 0)) return NULL;
    csi_lock.lock();
    PacerWait(true);
    QStringList* args = MakeArgsPage(*urls);
    QStringList* res = RunPythonScript(*args, false);
    delete args;
    pacer.start();
    // quick checking the result
    if ((res == NULL) || (res->count() < 2)) {
        if (res != NULL) delete res;
        csi_lock.unlock();
        return false;
    }
    // even indexes are urls, which we do not really need, right?
    QStringList* ares = new QStringList();
    for (int xd = 1; xd < res->count(); xd += 2) {
        ares->append(res->at(xd));
    }
    delete res;
    // done
    csi_lock.unlock();
    return ares;
}
//-------------------------------------------------
 QString* jfCloudScraperInterface::FetchOnePage(const QUrl& the_url ) {
     // basic input checking
     if (!the_url.isValid()) return NULL;
     csi_lock.lock();
     QStringList* args = MakeArgs1Page(the_url);
     PacerWait(true);
     QStringList* res = RunPythonScript(*args, true);
     delete args;
     pacer.start();
     // quick checking the result
     if ((res == NULL) || (res->count() < 2)) {
         if (res != NULL) delete res;
         csi_lock.unlock();
         return false;
     }
     // even indexes are urls, which we do not really need, right?
     QString* out = new QString();
     (*out) = res->at(1);
     res->at(1);
     delete res;
     // done
     csi_lock.unlock();
     return out;
 }

//========================================================
void jfCloudScraperInterface::PacerWait(bool nostart) {
    if (pacer.isValid()) {
        qint64 tdiff = 1200 - pacer.elapsed();
        /**/JDEBUGLOGI("jfCloudScraperInterface::PacerWait",1,tdiff)
        if (tdiff > 0) jfSleepThread::msleep(tdiff);
    }
    if (!nostart) pacer.start();
}
//--------------------------------------------------------
QStringList* jfCloudScraperInterface::MakeArgsTok(const QString& url) const {
    QStringList* res = new QStringList(script_path);
    res->append("a");
    res->append(url);
    return res;
}
//-------------------------
QStringList* jfCloudScraperInterface::MakeArgsPage(const QStringList& urls) const {
    QStringList* res = new QStringList(script_path);
    res->append("b");
    for (int x = 0; x < urls.count(); x++) {
        res->append(urls.at(x));
    }
    return res;
}
//-------------------------
QStringList* jfCloudScraperInterface::MakeArgs1Page(const QUrl& the_url) const {
    QStringList* res = new QStringList(script_path);
    res->append("b");
    res->append(the_url.toEncoded());
    return res;
}
//-------------------------
QStringList* jfCloudScraperInterface::RunPythonScript(const QStringList& arg, bool readfiles) const {
    const QString fname = "jfCloudScraperInterface::RunPythonScript";
    QProcess runscrape;
    /**/JDEBUGLOG(fname,1)
    runscrape.start("python",arg);
    runscrape.waitForFinished();
    /**/JDEBUGLOG(fname,2)
    // for now, assume console output is in UTF-8
    QByteArray rawout = runscrape.readAllStandardOutput();
    QByteArray errout = runscrape.readAllStandardError();
    /**/JDEBUGLOGS(fname,3,QString::fromAscii(errout))
    QString data = QString::fromUtf8(rawout);

    if (readfiles) {
        bool read_okay = FromFile(data,"transfer.txt");
        /**/JDEBUGLOGB(fname,4,read_okay)
         if (!read_okay) return false;
    }

    /**///JDEBUGLOGS(fname,5,data)
    // parsing by a delimiter, the should be at least one of these
    if ((data.isNull()) || (data.isEmpty())) return NULL;
    /**/JDEBUGLOG(fname,6)
    QStringList rparts = data.split("#~~~___~~~#");
    /**/JDEBUGLOGI(fname,7,rparts.count())
    if (rparts.count() < 2) return NULL;
    // making a trimmed output list that skips part 0
    QStringList* res = new QStringList();
    /**/JDEBUGLOG(fname,8)
    for (int x = 1; x < rparts.count(); x++) {
        res->append(rparts.at(x).trimmed());
    }
    /**/JDEBUGLOG(fname,9)
    return res;
}
//-------------------------------------------------------
bool jfCloudScraperInterface::ProcessTokResults(const QStringList& results) {
    const QString fname = "jfCloudScraperInterface::ProcessTokResults";
    /**/JDEBUGLOG(fname,1)
    if (results.count() < 3) return false;
    /**/JDEBUGLOGS(fname,2,results.at(0))
    user_agent = results.at(0).toAscii();
    // cookie 1 should be __cfduid
    /**/JDEBUGLOG(fname,3)
    QStringList cook1 = results.at(1).split(QRegExp("[\n\r]+"),QString::SkipEmptyParts);
    if (cook1.count() != 2) return false;
    // cookie2 should be cf_clearance, which may be empty
    /**/JDEBUGLOGS(fname,4,results.at(2))
    QStringList cook2 = results.at(2).split(QRegExp("[\n\r]+"),QString::SkipEmptyParts);
    // making the merged cookie
    QString mcook;
    /**/JDEBUGLOG(fname,5)
    // cf_clearance empty?
    if ((cook2.count() == 1) || cook2.at(1).isEmpty()) mcook = "";
    else mcook = cook2.at(0) + "=" + cook2.at(1) + "; ";
    mcook += cook1.at(0) + "=" + cook1.at(1);
    /**/JDEBUGLOGS(fname,6,mcook)
    cookie_value = mcook.toAscii();
    return true;
}


/*****************************************************************************/
