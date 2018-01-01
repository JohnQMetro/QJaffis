/*****************************************************************************
Name    : download2.h
Basic   : More specialized download controller threads
Author  : John Q Metro
Started : June 20, 2015
Updated : July 30, 2016
******************************************************************************/
#ifndef DOWNLOAD2_H
  #define DOWNLOAD2_H
#endif // DOWNLOAD2_H
//-------------------------------
#ifndef DOWNLOAD_H
  #include "download.h"
#endif // DOWNLOAD_H
#ifndef ITEMCOLLECT_H_INCLUDED
  #include "../core/structs/itemcollect.h"
#endif // ITEMCOLLECT_H_INCLUDED
#ifndef RESULTCOLLECT_H_INCLUDED
  #include "../core/structs/resultcollect.h"
#endif // RESULTCOLLECT_H_INCLUDED
#ifndef SEARCHCORE_H_INCLUDED
  #include "../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef RESULT_MULTI_H
  #include "../core/structs/result_multi.h"
#endif // RESULT_MULTI_H
//--------------------------------
#include <QMutex>
#include <QWaitCondition>
/*****************************************************************************/
// downloader base class for searches that produce lists of items
class jfBaseItemDownloader : public jfBaseFirstPageDownloader {
  public:
    // the constructor
    jfBaseItemDownloader(size_t in_max_threads);
    bool SetDataAndSearch(jfResultCollection* in_data);
    // wake
    void WakeAfter();
  protected:
    // custom virtual methods
    virtual jfItemsPageParserBase* makeItemParser() = 0;
    // helper methods
    bool SendResults(jfResultUnitVector* res_vector) const;
    void WaitAfterSend();
    // implemented virtual methods
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    virtual jfPageParserBase* makeParser();
    // current collection
    jfUrlItemCollection* current_collection;
    jfResultCollection* maindata;
    jfSearchCore* search_object;
    bool doprocess;
    // wait!
    QMutex wlock;
    QWaitCondition syncher;
};
//=============================================================================
/* We have many fetching tasks which require multiple categories. However they are varied
 * enough that some can use jfBaseItemDownloader, others need different stuff.
 * QObject is not compatible with multiple inheritance, or templates, so there must
 * be repeated re-implementation. */

class jfMultiCatBaseDownloader : public jfBaseItemDownloader {
    Q_OBJECT
  public:
    jfMultiCatBaseDownloader(size_t in_max_threads);
  signals:
    void SendCatCount(size_t ccount);
    void SendNewCategory(struct jfItemInfoMsg catdat);
    void SendCategoryDone(bool skip);
    void SendCatFail();
  public slots:
    virtual void StartProcessing();
  protected:
    // implemented category methods
    bool DoCategory();
    bool InterCatCheckStop();
    // virtual category related methods
    virtual bool NextCategory() = 0;
    virtual void LoadCatInfo() = 0;
    virtual size_t EstCatPagecount() const = 0;
    virtual bool InitialSetup() = 0;
    // virtual methods that have to be implemented by derived threads
    virtual jfItemsPageParserBase* makeItemParser() = 0;
    virtual QString* makeURLforPage(size_t index) const = 0;
    // implemented ex-virtualmethods
    virtual void PrepareItemInfo(size_t pageIndex);
    virtual void makeFirstPageInfo();
    virtual void ProcessFirstPageResults(void* resultdata);
    // internal data
    QString tlogname;
    jfItemInfoMsg catdata;
    size_t catindex;
    size_t max_catcount;
    bool skip_cat_on_fail;
    jfResultMultiCollection* multi_maindata;
};

/*****************************************************************************/
