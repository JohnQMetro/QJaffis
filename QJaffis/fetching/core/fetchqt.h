/*****************************************************************************
Name    : fetchqt.h
Basic   : An implementation of jfPageFetchInterface using Qt downloaders
Author  : John Q Metro
Started : June 20, 2021
Updated : May 14, 2022

******************************************************************************/
#ifndef FETCHQT_H
    #define FETCHQT_H
#endif // FETCHQT_H
//----------------------
#ifndef FETCHBASE_H
    #include "fetchbase.h"
#endif // FETCHBASE_H
//----------------------
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QByteArray>
#include <vector>
#include <QTextCodec>
/*****************************************************************************/
class jfPageFetchWithQt : public virtual jfPageFetchInterface {
  public:

    jfPageFetchWithQt(const QStringList* user_agents);

    // implemented public methods
    virtual jfFETCH_ERROR Download(const QString& inurl, const QString& cookie = "");
    virtual bool Abort();
    virtual void Shutdown();

    ~jfPageFetchWithQt();

  protected:
    const QByteArray* GetUserAgent() const;
    QNetworkRequest* MakeRequest(const QString& cookie) const;
    // result helper methods
    bool CheckInterpretError();  // sets theerror based upon QNetworkReply::NetworkError
    bool ProcessResult(); // read and convert the BytesRead, and check for redirection, etc.
    QTextCodec* FigureOutCodec(const QByteArray& body_bytes) const;
    bool CheckWin1252(const QByteArray& body_bytes) const;
    bool ClearObjects(); // used to delete the qt objects after download

    // Qt objects used for the download
    QNetworkAccessManager* intermediary; // uses req to make raw_result
    QTextCodec *defaultcodec, *win1252codec;
    QNetworkReply* raw_result;

    // flags and other misc data
    std::vector<const QByteArray*>* useragents;
    const QByteArray* default_user_agent;
    char* w1252c;
    const int wsize = 5;
};

/*****************************************************************************/
