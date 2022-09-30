/******************************************************************************
Name    :   fimgroup_thread.h
Author  :   John Q Metro
Purpose :   FIM Groups search thread
Created :   August 4, 2015
Updated :   June 11, 2022
******************************************************************************/
#ifndef FIMGROUP_THREAD_H
#define FIMGROUP_THREAD_H
#endif // FIMGROUP_THREAD_H
//----------------------------------------------
#ifndef DOWNLOADROOT2_H
    #include "../../../fetching/loopget/downloadroot2.h"
#endif // DOWNLOADROOT2_H
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
/* for the late Jan 2018 revision, we do a two phase process (which is kind of awkward).
~ Phase 1 : Do a card search using the new parser, but unlike the usual, do NOT
Process the results (override the method). Instead we split the jfResultUnitVector*
into jfResultUnitVector* with only one item (this is because we download one result at a
time during phase 2). For phase 2, we download the individual group page for each
result, parse it, and merge the data into the original result data, and then possibly filter it.
Only then (if not filtered), do we call the ProcessResult(...) of the parent class.
extra detail: if do_card is true, we skip phase 2, and phase 1 gets it's results sent to
the display.
*/

class jfFIMGroup_DownThread : public jfDownloadRootItems {
  public:
    // constructor and data setting
    jfFIMGroup_DownThread();
  public slots:
    virtual void StartProcessing();
  protected:
    // special methods
    size_t StoreCardResults(jfResultUnitVector* in_vector);
    void DeleteCardStore();
    // custom virtual methods
    virtual jfItemsPageParserBase* makeItemParser();
    virtual jfParseFetchPackage* MakeParserFetcher();
    virtual void makeFirstPageInfo();
    virtual void ProcessFirstPageResults(void* resultdata);
    // overriden methods
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    // implemented virtual methods
    virtual QString* makeURLforPage(size_t index) const;
    virtual void PrepareItemInfo(size_t pageIndex);
    // internal data
    jfFIMGroupSearch* search;
    // phase
    bool card_phase;
    bool do_card;
    std::vector<jfResultUnitVector*> card_store;
};
/*****************************************************************************/
