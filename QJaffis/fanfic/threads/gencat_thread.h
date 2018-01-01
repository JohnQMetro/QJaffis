/******************************************************************************
Name    :   gencat_thread.h
Author  :   John Q Metro
Purpose :   Base downloader class for sections & categories
Created :   July 13, 2016
Updated :   September 4, 2016
******************************************************************************/
#ifndef GENCAT_THREAD_H
  #define GENCAT_THREAD_H
#endif // GENCAT_THREAD_H
//------------------------------------
#ifndef DOWNLOAD_H
  #include "../../fetching/download.h"
#endif // DOWNLOAD_H
#ifndef CAT_MISC_H_INCLUDED
  #include "../data/cats/cat_misc.h"
#endif // CAT_MISC_H_INCLUDED
/*****************************************************************************/
// core downloader thread base class, managing multiple downloader threads
class jfGeneralCategoryDownloaderBase : public jfBaseDownloader {
    Q_OBJECT
  public:
    jfGeneralCategoryDownloaderBase(size_t in_max_threads);
    bool SetCategoryDataPointers(jfGenCatManager* in_result_holder, jfGenSecData* in_url_source);
  public slots:
    // the top level process method
    virtual void StartProcessing();
  protected:
    // helper methods

    // virtual base methods implemented here
    virtual QString* makeURLforPage(size_t index) const;
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    virtual bool advanceFetchIndex();
    virtual bool advanceResultIndex();
    virtual void PrepareItemInfo(size_t pageIndex);

    // virtual methods that have to be implemented by derived threads
    virtual jfPageParserBase* makeParser() = 0;

    // custom data
    jfGenCatManager* result_holder;
    jfGen_CatHolder* working_data;
    jfGenSecData* url_source;
    QString tlogname;
};
/*****************************************************************************/
