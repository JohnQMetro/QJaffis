/******************************************************************************
Name    :   gencat_thread.cpp
Author  :   John Q Metro
Purpose :   Base downloader class for sections & categories
Created :   July 13, 2016
Updated :   July 7, 2022
******************************************************************************/
#ifndef GENCAT_THREAD_H
  #include "gencat_thread.h"
#endif // GENCAT_THREAD_H
//------------------------------------
#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED

/*****************************************************************************/
// -- [ METHODS forjfGeneralCategoryDownloaderBase ] ---------------
//+++++++++++++++++++++++++++++++++++++++++++++++++
// base constructor
jfGeneralCategoryDownloaderBase::jfGeneralCategoryDownloaderBase():jfDownloadRoot() {
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
  SetupWorkers(false);
  /**/tLog(fname,3);
  bool main_results = xLoopGet();

  // handling the aftermath
  if (main_results) result_holder->NewDone();
  else result_holder->DiscardNew();
  working_data = NULL;
  /**/tLogB(fname,4,main_results);
  ClearWorkers(true);

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
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfParseFetchPackage* jfGeneralCategoryDownloaderBase::CategoryParseFetchMaker(jf_FICTYPE site, jfPageParserBase* page_parser) const {
    if (page_parser == NULL) return NULL;
    jfParseFetchPackageMaker* parse_fetch_maker = jglobal::settings.getFetchPackageMaker();
    if (parse_fetch_maker == NULL) return NULL;
    else {
        jglobal::jfFetchBasics fetch_type = jglobal::settings.FindFetchTypeFor(site, jglobal::FPT_CATEGORY_PAGE);
        if (fetch_type.IsValid()) {
            return parse_fetch_maker->makeFetchPackage(fetch_type, page_parser);
        }
        else {
            return parse_fetch_maker->makeGeneralFetchPackage(page_parser);
        }
    }
}
/*****************************************************************************/
