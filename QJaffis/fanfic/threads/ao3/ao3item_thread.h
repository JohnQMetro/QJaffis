/******************************************************************************
Name    :   ao3item_thread.h
Author  :   John Q Metro
Purpose :   Downloader class for AO3 fic listings
Created :   July 17, 2016
Updated :   July 30, 2022 (adding logging extra tags)
******************************************************************************/
#ifndef AO3ITEM_THREAD_H
  #define AO3ITEM_THREAD_H
#endif // AO3ITEM_THREAD_H
//------------------------------------
#ifndef MULTICATROOT_H
    #include "../../../fetching/loopget/multicatroot.h"
#endif // MULTICATROOT_H

#ifndef AO3_CATS_H_INCLUDED
  #include "../../data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
#ifndef AO3_SEARCH_H_INCLUDED
  #include "../../data/ao3/ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED
#ifndef AO3_COLL_H_INCLUDED
  #include "../../data/ao3/ao3_coll.h"
#endif // AO3_COLL_H_INCLUDED
//------------------------------------
#ifndef AO3ITEM_PARSER_H
  #include "ao3item_parser.h"
#endif // AO3ITEM_PARSER_H
/*****************************************************************************/
class jfAO3FandomItemDownloader : public jfMultiCatRootDownloader {
    Q_OBJECT
  public:
    jfAO3FandomItemDownloader();

  protected:
    // virtual category related methods
    virtual bool NextCategory() override;
    virtual void LoadCatInfo() override;
    virtual size_t EstCatPagecount() const override;
    virtual bool InitialSetup() override;
    // virtual methods that have to be implemented by derived threads
    virtual jfItemsPageParserBase* makeItemParser() override;
    virtual jfParseFetchPackage* MakeParserFetcher() override;
    virtual QString* makeURLforPage(size_t index) const override;

    virtual void ClearWorkers(bool delete_parser) override;

    const jfAO3_Category* current_category;
    jfAO3ItemParser* ao3_parser;
    jfAO3Search* ao3_search;
    jfAO3ResColl* typed_maindata;

};
/*****************************************************************************/
