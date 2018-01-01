/******************************************************************************
Name    :   ffnitems_thread.h
Author  :   John Q Metro
Purpose :   Downloader class for FFN fic listings
Created :   July 24, 2016
Updated :   July 24, 2016
******************************************************************************/
#ifndef FFNITEMS_THREAD_H
  #define FFNITEMS_THREAD_H
#endif // FFNITEMS_THREAD_H
//------------------------------------
#ifndef DOWNLOAD2_H
  #include "../../fetching/download2.h"
#endif // DOWNLOAD2_H
#ifndef JFFFNSCATCORE
  #include "../categories/ffn_catcore.h"
#endif // JFFFNSCATCORE
#ifndef JF_FFNSEARCH
  #include "../data/ffn_search.h"
#endif
#ifndef FFN_MULTICOLL
  #include "../data/ffn_mulcoll.h"
#endif
/*****************************************************************************/
class jfFFNDownloader : public jfMultiCatBaseDownloader {
    Q_OBJECT
  public:
    jfFFNDownloader(size_t in_max_threads);

  protected:
    // virtual category related methods
    virtual bool NextCategory();
    virtual void LoadCatInfo();
    virtual size_t EstCatPagecount() const;
    virtual bool InitialSetup();
    // virtual methods that have to be implemented by derived threads
    virtual jfItemsPageParserBase* makeItemParser();
    virtual QString* makeURLforPage(size_t index) const;
    // overriding a parent method
    virtual void ProcessFirstPageResults(void* resultdata);

    const jfFFN_CategoryCore* current_category;
    jfFFNSearch* ffn_search;
    jfFFNResColl* typed_maindata;

};
/*****************************************************************************/


