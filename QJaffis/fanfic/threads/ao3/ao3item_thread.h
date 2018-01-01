/******************************************************************************
Name    :   ao3item_thread.h
Author  :   John Q Metro
Purpose :   Downloader class for AO3 fic listings
Created :   July 17, 2016
Updated :   July 17, 2016
******************************************************************************/
#ifndef AO3ITEM_THREAD_H
  #define AO3ITEM_THREAD_H
#endif // AO3ITEM_THREAD_H
//------------------------------------
#ifndef DOWNLOAD2_H
  #include "../../../fetching/download2.h"
#endif // DOWNLOAD2_H
#ifndef AO3_CATS_H_INCLUDED
  #include "../../data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
#ifndef AO3_SEARCH_H_INCLUDED
  #include "../../data/ao3/ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED
#ifndef AO3_COLL_H_INCLUDED
  #include "../../data/ao3/ao3_coll.h"
#endif // AO3_COLL_H_INCLUDED
/*****************************************************************************/
class jfAO3FandomItemDownloader : public jfMultiCatBaseDownloader {
    Q_OBJECT
  public:
    jfAO3FandomItemDownloader(size_t in_max_threads);

  protected:
    // virtual category related methods
    virtual bool NextCategory();
    virtual void LoadCatInfo();
    virtual size_t EstCatPagecount() const;
    virtual bool InitialSetup();
    // virtual methods that have to be implemented by derived threads
    virtual jfItemsPageParserBase* makeItemParser();
    virtual QString* makeURLforPage(size_t index) const;

    const jfAO3_Category* current_category;
    jfAO3Search* ao3_search;
    jfAO3ResColl* typed_maindata;

};
/*****************************************************************************/
