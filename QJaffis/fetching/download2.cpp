/*****************************************************************************
Name    : download2.cpp
Basic   : More specialized download controller threads
Author  : John Q Metro
Started : June 20, 2015
Updated : July 16, 2016
******************************************************************************/
#ifndef DOWNLOAD2_H
  #include "download2.h"
#endif // DOWNLOAD2_H
//-------------------------------
#include <assert.h>
#include <QCoreApplication>
/*****************************************************************************/
jfBaseItemDownloader::jfBaseItemDownloader(size_t in_max_threads):jfBaseFirstPageDownloader(in_max_threads){
  current_collection = NULL;
  maindata = NULL;
  search_object = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfBaseItemDownloader::SetDataAndSearch(jfResultCollection* in_data) {
  // testing for rejection
  if (in_data == NULL) return false;
  jfSearchCore* xtemp = in_data->GetSearch();
  if (xtemp == NULL) return false;
  // setting
  maindata = in_data;
  search_object = xtemp;
  return true;
  doprocess = false;
}
//----------------------------------
void jfBaseItemDownloader::WakeAfter() {
  syncher.wakeAll();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
bool jfBaseItemDownloader::SendResults(jfResultUnitVector* res_vector) const {
  const QString fname = "jfBaseItemDownloader::SendResults";
  assert(res_vector!=NULL);
  // starting
  size_t lstotal = res_vector->stotal;
  /**/tLog(fname,1);
  // we have the proper list, but 0 items is treated differently
  if (res_vector->size() == 0) {
    /**/tLog(fname,2);
    // inserting the count
    current_collection->AddToGrandTotal(lstotal);
    // sending the count
    search_object->SendNewcount(lstotal);
    /**/tLog(fname,3);
    return false;
  }
  else {
    // with the proper list, we insert
    /**/tLog(fname,4);
    current_collection->AddResults(res_vector);
    res_vector->source = const_cast<jfBaseItemDownloader*>(this);
    // now we have to dispatch the result info to the view
    search_object->DispatchResults(lstotal,res_vector);
    /**/tLog(fname,5);
    return true;
  }
}
//----------------------------------------------------------
void jfBaseItemDownloader::WaitAfterSend() {
  wlock.lock();
  QCoreApplication::processEvents();
  syncher.wait(&wlock,1500);
  wlock.unlock();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
bool jfBaseItemDownloader::ProcessResults(void* resultdata) {
  const QString fname = "jfBaseItemDownloader::ProcessResults";
  assert(resultdata!=NULL);
  jfResultUnitVector* res_vector = static_cast<jfResultUnitVector*>(resultdata);
  // insering the results in the collection
  /**/tLog(fname,1);
  // we still might have to filter for duplicates
  if ((current_collection->hasIDFiltering()) && ((res_vector->size())>0)) {
    /**/tLog(fname,2);
    size_t filtered_out = current_collection->IDFilterMarkResults(res_vector);
    /**/tLogS(fname,3,filtered_out);
    if (filtered_out>0) {
      /**/tLog(fname,4);
      jfResultUnitVector* new_vector = new jfResultUnitVector();
      new_vector->stotal = res_vector->stotal;
      jfBasePD* curr_item = NULL;
      /**/tLog(fname,5);
      // looping through the results, transferring those we keep
      for (size_t old_index = 0; old_index < res_vector->size() ; old_index++) {
        /**/tLogL(fname,6,old_index);
        curr_item = const_cast<jfBasePD*>((*res_vector)[old_index]->Sourcelink());
        if (curr_item->included) {
          /**/tLog(fname,7);
          new_vector->push_back((*res_vector)[old_index]);
        }
        else {
          /**/tLog(fname,8);
          delete curr_item;
          delete ((*res_vector)[old_index]);
          /**/tLog(fname,9);
          (*res_vector)[old_index] = NULL;
        }
      }
      // done with the deletions and loop, nopw we clean and tranfer
      res_vector->clear();
      delete res_vector;
      res_vector = new_vector;
      /**/tLog(fname,10);
    }
  }
  /**/tLog(fname,11);
  // finishing
  if (!SendResults(res_vector)) {
    delete res_vector;
  }
  else WaitAfterSend();
  /**/tLog(fname,12);
  // done
  return true;
}

//------------------------------------
void jfBaseItemDownloader::DisposeResults(void* resultdata) {
  const QString fname = "jfBaseItemDownloader::DisposeResults";
  jfResultUnitVector* data_to_delete = static_cast<jfResultUnitVector*>(resultdata);
  jfBasePD* item;
  // looping over each item
  /**/tLog(fname,1);
  for (size_t item_index = 0; item_index < data_to_delete->size() ; item_index++) {
    /**/tLogL(fname,2,item_index);
    item = const_cast<jfBasePD*>((*data_to_delete)[item_index]->Sourcelink());
    /**/tLog(fname,3);
    delete item;
    delete ((*data_to_delete)[item_index]);
    /**/tLog(fname,4);
    (*data_to_delete)[item_index] = NULL;
  }
  // finishing off
  data_to_delete->clear();
  /**/tLog(fname,5);
  delete data_to_delete;
}

//------------------------------------
jfPageParserBase* jfBaseItemDownloader::makeParser() {
  jfItemsPageParserBase* cparser = makeItemParser();
  cparser->setSearchPointer(search_object);
  return cparser;
}
//=============================================================================
jfMultiCatBaseDownloader::jfMultiCatBaseDownloader(size_t in_max_threads):jfBaseItemDownloader(in_max_threads) {
  max_catcount = 0;
  catindex = 0;
  skip_cat_on_fail = false;
  multi_maindata = NULL;
  tlogname = "PleaseReplaceThis";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfMultiCatBaseDownloader::StartProcessing() {
  const QString fname = "jfMultiCatBaseDownloader::StartProcessing";
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
    while(NextCategory()) {
      catindex++;
      /**/tLogL(fname,4,catindex);
      result = DoCategory();
      /**/tLogB(fname,5,result);
      if (!result) break;
      if (InterCatCheckStop()) break;
    }
    /**/tLog(fname,6);
  }
  // done
  /**/tLog(fname,7);
  AllDone(result);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* NextCategory() is assumed to have been called, and the catindex
 * is assumed to be for the category we are downloading. */
bool jfMultiCatBaseDownloader::DoCategory() {
  // sending initial messages
  LoadCatInfo();
  emit SendNewCategory(catdata);
  size_t epc = EstCatPagecount();
  // fetching the individual pages
  bool fetchresult = FetchAllPages(epc);
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
bool jfMultiCatBaseDownloader::InterCatCheckStop() {
  if ((pauser->CheckPauseStop()) || (pauser->CheckPauseStopAfter())) {
    current_error = jfa_HALT;
    emit SendHaltMessage();
    return true;
  }
  return false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfMultiCatBaseDownloader::PrepareItemInfo(size_t pageIndex) {
  infoToSend.startaction = "Doing";
  infoToSend.part_count = pagecount;
  infoToSend.item_index = pageIndex;
  infoToSend.item_label = "Page";
  infoToSend.item_name = "";
}
//-------------------------------------
void jfMultiCatBaseDownloader::makeFirstPageInfo() {
  PrepareItemInfo(1);
}
//-------------------------------------
void jfMultiCatBaseDownloader::ProcessFirstPageResults(void* resultdata) {
  ProcessResults(resultdata);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
jfItemInfoMsg catdata;
size_t catindex;
size_t max_catcount;
bool skip_cat_on_fail;
*/
/*****************************************************************************/
