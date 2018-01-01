/******************************************************************************
Name    :   fimgroup_thread.h
Author  :   John Q Metro
Purpose :   FIM Groups search thread
Created :   August 4, 2015
Updated :   June 24, 2016
******************************************************************************/
#ifndef FIMGROUP_THREAD_H
#define FIMGROUP_THREAD_H
#endif // FIMGROUP_THREAD_H
//----------------------------------------------
#ifndef DOWNLOAD2_H
  #include "../../../fetching/download2.h"
#endif // DOWNLOAD2_H
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "../../data/fim/fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "../../data/fim/fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
/*****************************************************************************/
// the 'check if a page is okay'  method
bool FIMGroup_PageCheck(const QString* ival);
//==========================================================================

class jfFIMGroup_DownThread : public jfBaseItemDownloader {
  public:
    // constructor and data setting
    jfFIMGroup_DownThread(size_t in_max_threads);
  public slots:
    virtual void StartProcessing();
  protected:    
    // custom virtual methods
    virtual jfItemsPageParserBase* makeItemParser();
    virtual void makeFirstPageInfo();
    virtual void ProcessFirstPageResults(void* resultdata);
    // helper method
    size_t CalculateURLOffset(const size_t& in_index) const;
    // implemented virtual methods
    virtual QString* makeURLforPage(size_t index) const;
    virtual void PrepareItemInfo(size_t pageIndex);
    // internal data
    jfFIMGroupSearch* search;
    // special offsets and indexes
    int pageoffset1, pageoffset2;
    size_t count1, count2;
};
/*****************************************************************************/
