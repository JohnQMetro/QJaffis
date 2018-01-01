/******************************************************************************
Name    :   gencat_thread.cpp
Author  :   John Q Metro
Purpose :   Base downloader class for sections & categories
Created :   July 13, 2016
Updated :   September 4, 2016
******************************************************************************/
#ifndef GENCAT_THREAD_H
  #include "gencat_thread.h"
#endif // GENCAT_THREAD_H
//------------------------------------

/*****************************************************************************/
// -- [ METHODS forjfGeneralCategoryDownloaderBase ] ---------------
//+++++++++++++++++++++++++++++++++++++++++++++++++
// base constructor
jfGeneralCategoryDownloaderBase::jfGeneralCategoryDownloaderBase(size_t in_max_threads):
        jfBaseDownloader(in_max_threads) {
  result_holder =NULL;
  working_data = NULL;
  url_source = NULL;
}

//---------------------------------------
// the external data/results holder we need
bool jfGeneralCategoryDownloaderBase::SetCategoryDataPointers(jfGenCatManager* in_result_holder,
                      jfGenSecData* in_url_source) {
  if (in_result_holder == NULL) return false;
  if (in_url_source == NULL) return false;
  // setting
  result_holder = in_result_holder;
  url_source = in_url_source;
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
// the top level process method
void jfGeneralCategoryDownloaderBase::StartProcessing() {
  const QString fname = "jfGeneralCategoryDownloaderBase::StartProcessing";
  // starting
  initLog(tlogname);
  emit SendStart(true);
  /**/tLog(fname,1);
  // itemcount
  pagecount = url_source->ItemCount();
  emit SendItemCount(pagecount);
  /**/tLogS(fname,2,pagecount);
  // setup before fetching the pages
  url_source->ResetIndex();
  working_data = result_holder->StartNew();

  // the main tasks
  SetupThreads(false);
  /**/tLog(fname,3);
  bool main_results = LoopGet();

  // handling the aftermath
  if (main_results) result_holder->NewDone();
  else result_holder->DiscardNew();
  working_data = NULL;
  /**/tLogB(fname,4,main_results);

  // done
  AllDone(main_results);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
// virtual base methods implemented here
//---------------------------------------
QString* jfGeneralCategoryDownloaderBase::makeURLforPage(size_t index) const {
  return new QString(url_source->CurrUrl());
}
//---------------------------------------
bool jfGeneralCategoryDownloaderBase::ProcessResults(void* resultdata) {
  jfGenSection* section_res = static_cast<jfGenSection*>(resultdata);
  working_data->AddSection(section_res);
  return true;
}
//---------------------------------------
void jfGeneralCategoryDownloaderBase::DisposeResults(void* resultdata) {
  jfGenSection* section_res = static_cast<jfGenSection*>(resultdata);
  delete section_res;
}
//---------------------------------------
bool jfGeneralCategoryDownloaderBase::advanceFetchIndex() {
  url_page_index++;
  url_source->NextIndex();
  return true;
}
//---------------------------------------
bool jfGeneralCategoryDownloaderBase::advanceResultIndex() {
  result_page_index++;
  return true;
}
//---------------------------------------
void jfGeneralCategoryDownloaderBase::PrepareItemInfo(size_t pageIndex) {
  // setting up the label values
  infoToSend.startaction = "Getting";
  infoToSend.part_count = pagecount;
  infoToSend.item_name = url_source->NameAtIndex(pageIndex-1);
  infoToSend.item_index = pageIndex;
  infoToSend.item_label = "Section";
}
/*****************************************************************************/
