/******************************************************************************
Name    :   fimitem_thread.h
Author  :   John Q Metro
Purpose :   FIM Groups search thread
Created :   June 30, 3016
Updated :   June 30, 2016
******************************************************************************/
#ifndef FIMITEM_THREAD_H
  #define FIMITEM_THREAD_H
#endif // FIMITEM_THREAD_H
//-------------------------------------------------
#ifndef DOWNLOAD2_H
  #include "../../../fetching/download2.h"
#endif // DOWNLOAD2_H
#ifndef FIM_SEARCH_H_INCLUDED
  #include "../../data/fim/fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
/*****************************************************************************/
class jfFIMItemDownloader : public jfBaseItemDownloader {
  public:
    jfFIMItemDownloader(size_t in_max_threads);
  public slots:
    virtual void StartProcessing();
  protected:
    // extra methods
    bool AddToSummaryStore(jfIDStringStore* stuff_to_add);
    // implemented special virtual first page methods
    virtual void makeFirstPageInfo();
    virtual void ProcessFirstPageResults(void* resultdata);
    virtual jfItemsPageParserBase* makeItemParser();
    // overriden methods
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    virtual jfPageParserBase* makeParser();
    // implemented virtual methods
    virtual QString* makeURLforPage(size_t index) const;
    virtual void PrepareItemInfo(size_t pageIndex);

    // internal data
    size_t est_itemcount;
    bool compact_phase;
    jfFIMSearch* fim_search;
    jfIDStringStore* shortsum_storage;
};


/*****************************************************************************/
