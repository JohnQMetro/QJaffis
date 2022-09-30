/*****************************************************************************
Name    : multcatroot.cpp
Basic   : Downloader class for items broken into multiple categories
Author  : John Q Metro
Started : July 20, 2021
Updated :
******************************************************************************/
#ifndef MULTICATROOT_H
    #include "multicatroot.h"
#endif // MULTICATROOT_H
//----------------------------

/******************************************************************************/
jfMultiCatRootDownloader::jfMultiCatRootDownloader():jfDownloadRootItems() {
  max_catcount = 0;
  catindex = 0;
  skip_cat_on_fail = false;
  multi_maindata = NULL;
  tlogname = "PleaseReplaceThis";
  check_redirection = true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfMultiCatRootDownloader::StartProcessing() {
  const QString fname = "jfMultiCatRootDownloader::StartProcessing";
  initLog(tlogname);
  emit SendStart(true);
  /**/tLog(fname,1);
  started = true;
  multi_maindata = dynamic_cast<jfResultMultiCollection*>(maindata);
  bool result = false;
  // details of the setup are left to the child class
  /**/tLog(fname,2);
  if (InitialSetup()) {
    /**/tLog(fname,3);
    emit SendCatCount(max_catcount);
    SetupWorkers(check_redirection);
    while(NextCategory()) {
      catindex++;
      /**/tLogL(fname,4,catindex);
      result = DoCategory();
      /**/tLogB(fname,5,result);
      if (!result) break;
      if (InterCatCheckStop()) break;
    }
    /**/tLog(fname,6);
    ClearWorkers(true);
    /**/tLog(fname,7);
  }
  // done
  /**/tLog(fname,8);
  AllDone(result);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* NextCategory() is assumed to have been called, and the catindex
 * is assumed to be for the category we are downloading. */
bool jfMultiCatRootDownloader::DoCategory() {
  // sending initial messages
  LoadCatInfo();
  emit SendNewCategory(catdata);
  size_t epc = EstCatPagecount();
  // fetching the individual pages
  bool fetchresult = xFetchAllPages(epc);
  // handling fetchresult = false
  if (!fetchresult){
    if (skip_cat_on_fail) emit SendCategoryDone(true);
    else emit SendCatFail();
    return skip_cat_on_fail;
  }
  else{
    emit SendCategoryDone(false);
    return true;
  }
  // I think that is all ...
}
//------------------------------------
bool jfMultiCatRootDownloader::InterCatCheckStop() {
  if ((pauser->CheckPauseStop()) || (pauser->CheckPauseStopAfter())) {
    current_error = jfa_HALT;
    emit SendHaltMessage();
    return true;
  }
  return false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfMultiCatRootDownloader::PrepareItemInfo(size_t pageIndex) {
  infoToSend.startaction = "Doing";
  infoToSend.part_count = pagecount;
  infoToSend.item_index = pageIndex;
  infoToSend.item_label = "Page";
  infoToSend.item_name = "";
}
//-------------------------------------
void jfMultiCatRootDownloader::makeFirstPageInfo() {
  PrepareItemInfo(1);
}
//-------------------------------------
void jfMultiCatRootDownloader::ProcessFirstPageResults(void* resultdata) {
  ProcessResults(resultdata);
}
/******************************************************************************/
