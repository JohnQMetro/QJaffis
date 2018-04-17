/*****************************************************************************
Name    : onepage.cpp
Basic   : Attempt at a simplified downloader for one page
Author  : John Q Metro
Started : December 28, 2017
Updated : December 29, 2017

******************************************************************************/
#ifndef ONEPAGE_H
    #include "onepage.h"
#endif // ONEPAGE_H
//------------------------
#include <QUrl>
#include <QCoreApplication>
/*****************************************************************************/
jfOnePageGetter::jfOnePageGetter():jfLoggBase() {
    parser_ptr = NULL;
    retry_count = 0;
    getter = NULL;
    started = false;
}
jfOnePageGetter::~jfOnePageGetter() {
    if (getter != NULL) delete getter;
}

//----------------------------------
bool jfOnePageGetter::setParameters(const QString& url, jfPageParserBase* parser, size_t retries) {
    if (parser == NULL) return false;
    QUrl tempurl;
    tempurl.setEncodedUrl(url.toLatin1());
    if (!tempurl.isValid()) return false;
    downurl = url;
    parser_ptr = parser;
    parser_ptr->SetLogPointer(this);
    retry_count = retries;
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// does all of the real work
void jfOnePageGetter::StartProcessing() {
    // initial setup
    bool okay = false;
    if (parser_ptr != NULL) {
        getter = new jfFetchPage();
        okay = getter->SetParams(parser_ptr);
        if (okay) okay = getter->SetURL(downurl,1);
    }
    emit sendStart(okay);
    started = okay;
    if (!okay) {
        emit sendFinished(false);
        emit sendQuit();
        return;
    }
    // we launch the download
    jfFETCH_ERROR currError = jff_NOERROR;
    okay = false;
    // there is a loop because if the download fails, we retry...
    for (size_t attempt_index = 0; attempt_index <= retry_count; attempt_index++ ) {
        getter->StartDownload();
        okay = getter->PrepareResults();  // returns when download is done and converted to a string
        // download successful !
        if (okay) break;
        // download failed !
        else {
            currError = getter->GetError();
            getter->Reset();
            if (currError != jff_TRYAGAIN) break;  // some types of errors mean retrying is futile
        }
        // sleeping for a sec before we retry
        jfSleepThread::msleep(1000);
        QCoreApplication::processEvents();
    }
    // after the download, is everything okay?
    if (!okay) {
        emit sendFinished(false);
        emit sendQuit();
        return;
    }
    else emit sendDownloaded();
    QString *downloaded_data = getter->GetResult();
    // we don't need the getter anymore
    delete getter;
    getter = NULL;
    // we now are at the position where we can try parsing the page
    parser_ptr->ParseDownloadedPage(*downloaded_data,1);
    // done. the sendFinishedHandler can handle success or faliure at parsing
    emit sendFinished(parser_ptr->isPageParsed());
    emit sendQuit();
}

/*****************************************************************************/
