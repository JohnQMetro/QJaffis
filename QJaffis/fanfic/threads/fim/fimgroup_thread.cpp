/******************************************************************************
Name    :   fimgroup_thread.cpp
Author  :   John Q Metro
Purpose :   FIM Groups search thread
Created :   August 4, 2015
Updated :   June 27, 2015
******************************************************************************/
#ifndef FIMGROUP_THREAD_H
  #include "fimgroup_thread.h"
#endif // FIMGROUP_THREAD_H
//----------------------------------------------
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "../../data/fim/fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
#ifndef FIMGROUP_PARSER_H
  #include "fimgroup_parser.h"
#endif // FIMGROUP_PARSER_H
//-----------------------------------------
#include <assert.h>
/*****************************************************************************/
// the constructor
jfFIMGroup_DownThread::jfFIMGroup_DownThread(size_t in_max_threads):jfBaseItemDownloader(in_max_threads) {
  maindata = NULL;
  current_collection = NULL;
  search_object = NULL;
  skip_on_fail = true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMGroup_DownThread::StartProcessing() {
  const QString fname = "jfFIMGroup_DownThread::StartProcessing";
  initLog("FIMGroup");  
  emit SendStart(true);
  /**/tLog(fname,1);
  started = true;
  search = dynamic_cast<jfFIMGroupSearch*>(search_object);
  current_collection = maindata->GetCurrPointer();
  // setting up some initial counts and offsets
  count1 = search->GroupsToSearch(true);
  count2 = search->GroupsToSearch(false);
  /**/tLog2S(fname,2,count1,count2);
  pagecount = count1 + count2;
  /**/tLog(fname,3);
  emit SendItemCount(pagecount);
  search->DispatchCategory();
  pageoffset1 = search->GetMinGroup(true);
  pageoffset2 = search->GetMinGroup(false);
  /**/tLog2I(fname,4,pageoffset1,pageoffset2);
  // downloading
  // this search, we do not need to download the first separatly
  bool result = FetchAllPages(pagecount);
  /**/tLogB(fname,5,result);
  // done
  AllDone(result);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
jfItemsPageParserBase* jfFIMGroup_DownThread::makeItemParser() {
  return new jfFIMGroupParser();
}
//------------------------------------------------
void jfFIMGroup_DownThread::makeFirstPageInfo() {
  PrepareItemInfo(1);
}
//------------------------------------------------
void jfFIMGroup_DownThread::ProcessFirstPageResults(void* resultdata) {
  assert(resultdata!=NULL);
  jfResultUnitVector* res_vector = static_cast<jfResultUnitVector*>(resultdata);
  // with the proper list, we insert
  if (!SendResults(res_vector)) delete res_vector;
}
//--------------------------------------------------
// helper method
size_t jfFIMGroup_DownThread::CalculateURLOffset(const size_t& in_index) const {
  assert(in_index>0);
  size_t url_index;
  if (in_index > count1) {
    url_index = (in_index - count1) + pageoffset2 -1;
  }
  else {
    url_index = in_index + pageoffset1 -1;
  }
  return url_index;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//----------------------------
QString* jfFIMGroup_DownThread::makeURLforPage(size_t index) const {
  size_t new_index = CalculateURLOffset(index);
  return new QString(search->GetIndexUrl(new_index));
}
//----------------------------
void jfFIMGroup_DownThread::PrepareItemInfo(size_t pageIndex) {
  bool first = pageIndex <= count1;
  size_t pindex = CalculateURLOffset(pageIndex);
  // setting up the label values
  infoToSend.startaction = "Doing";
  infoToSend.part_count = (first)?(count1):(count2);
  infoToSend.item_name = "";
  infoToSend.item_index = pindex;
  infoToSend.item_label = "Group";
}
/*****************************************************************************/
