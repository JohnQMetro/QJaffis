/*****************************************************************************
Name    : multcatroot.h
Basic   : Downloader class for items broken into multiple categories
Author  : John Q Metro
Started : July 20, 2021
Updated : July 6, 2021
******************************************************************************/
#ifndef MULTICATROOT_H
    #define MULTICATROOT_H
#endif // MULTICATROOT_H
//-------------------------------
#ifndef DOWNLOADROOT2_H
    #include "downloadroot2.h"
#endif // DOWNLOADROOT2_H
//--------------------------------

/*****************************************************************************/

//=============================================================================
/* We have many fetching tasks which require multiple categories. However they are varied
 * enough that some can use jfBaseItemDownloader, others need different stuff.
 * QObject is not compatible with multiple inheritance, or templates, so there must
 * be repeated re-implementation. */

class jfMultiCatRootDownloader : public jfDownloadRootItems {
    Q_OBJECT
  public:
    jfMultiCatRootDownloader();
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
    bool check_redirection;
    jfResultMultiCollection* multi_maindata;
};
