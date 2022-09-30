/******************************************************************************
Name    :   fimitem_thread.h
Author  :   John Q Metro
Purpose :   FIM Groups search thread
Created :   June 30, 3016
Updated :   June 11, 2022
******************************************************************************/
#ifndef FIMITEM_THREAD_H
  #define FIMITEM_THREAD_H
#endif // FIMITEM_THREAD_H
//-------------------------------------------------

#ifndef FIM_SEARCH_H_INCLUDED
  #include "../../data/fim/fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
#ifndef DOWNLOADROOT2_H
    #include "../../../fetching/loopget/downloadroot2.h"
#endif // DOWNLOADROOT2_H
#ifndef BASEPARSE_H
  #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
/*****************************************************************************/
class jfFIMItemDownloader : public jfDownloadRootItems {
  public:
    jfFIMItemDownloader();
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

    virtual jfParseFetchPackage* MakeParserFetcher();

    // internal data
    size_t est_itemcount;
    bool compact_phase;
    jfFIMSearch* fim_search;
    jfIDStringStore* shortsum_storage;
};


/*****************************************************************************/
