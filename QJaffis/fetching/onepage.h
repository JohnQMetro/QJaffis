/*****************************************************************************
Name    : onepage.h
Basic   : Simplified downloader for one page
Author  : John Q Metro
Started : December 26, 2017
Updated : December 26, 2017

******************************************************************************/
#ifndef ONEPAGE_H
    #define ONEPAGE_H
#endif // ONEPAGE_H
//------------------------
#ifndef THREADBASE_H
  #include "threadbase.h"
#endif // THREADBASE_H
#ifndef BASEPARSE_H
  #include "baseparse.h"
#endif // BASEPARSE_H
//-----------------------------

/*****************************************************************************/
class jfOnePageGetter : public jfLoggBase {
    Q_OBJECT
  public:
    jfOnePageGetter();
    virtual ~jfOnePageGetter();
    bool setParameters(const QString& url, jfPageParserBase* parser, size_t retries);
  public slots:
    void StartProcessing();
  signals:
    void sendStart(bool okay);
    void sendDownloaded();
    void sendFinished(bool okay);
    void sendQuit();
  protected:
    jfFetchPage* getter;
    jfPageParserBase* parser_ptr;
    QString downurl;
    bool started;
    size_t retry_count;
};

/*****************************************************************************/
