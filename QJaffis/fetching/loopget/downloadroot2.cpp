/*****************************************************************************
Name    : downloadroot2.cpp
Basic   :
Author  : John Q Metro
Started : July 17, 2021
Updated : July 6, 2022
******************************************************************************/
#ifndef DOWNLOADROOT2_H
    #include "downloadroot2.h"
#endif // DOWNLOADROOT2_H
//-------------------------
#ifndef GLOBALSETTINGS_H
    #include "../../globalsettings.h"
#endif // GLOBALSETTINGS_H
#ifndef DEFAULTPATHS_H
    #include "../../defaultpaths.h"
#endif // DEFAULTPATHS_H

#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED

//-------------------------
#include <QCoreApplication>
/*****************************************************************************/
// constructor
jfDownloadRootFirstPage::jfDownloadRootFirstPage():jfDownloadRoot() {
  abort_fetch = false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfDownloadRootFirstPage::advanceFetchIndex() {
  url_page_index++;
  return true;
}
//-----------------------------------------------
bool jfDownloadRootFirstPage::advanceResultIndex() {
  result_page_index++;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

jfParseFetchPackage* jfDownloadRootFirstPage::DefaultParseFetchMaker(jf_FICTYPE site, jglobal::jfFICPAGE_TYPE page_type, jfPageParserBase* page_parser) const {
    if (page_parser == NULL) return NULL;
    jfParseFetchPackageMaker* parse_fetch_maker = jglobal::settings.getFetchPackageMaker();
    if (parse_fetch_maker == NULL) return NULL;
    else {
        jglobal::jfFetchBasics fetch_type = jglobal::settings.FindFetchTypeFor(site, page_type);
        if (fetch_type.IsValid()) {
            return parse_fetch_maker->makeFetchPackage(fetch_type, page_parser);
        }
        else {
            return parse_fetch_maker->makeGeneralFetchPackage(page_parser);
        }
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
bool jfDownloadRootFirstPage::DoFirstPage() {
  size_t new_pagecount;
  const QString fname = "jfDownloadRootFirstPage::DoFirstPage";
  // preparing for getting the data of the first page
  url_page_index = 1;
  /**/tLog(fname,1);
  makeFirstPageInfo();
  emit SendNewItem(infoToSend);
  // fetching and parsing the first page
  QString* url_to_fetch = makeURLforPage(1);
  /**/tLog(fname,2,*url_to_fetch);

  result_page_index = 1;
  // getting the results
  jfDownloadResults* page_one_results = fetcher->GetAndParsePage(url_to_fetch, 1);
  // handling the results
  current_error = (page_one_results->fetchResults).why;
  /**/tLog(fname,3);

  // case 1: first page was downloaded and parsed okay
  if (current_error == jfa_DONE) {
    /**/tLog(fname,4);
    // handling the results
    void* tdata = page_one_results->resultData;
    new_pagecount = page_one_results->pagecount;
    ProcessFirstPageResults(tdata);
    emit SendItemDone();
    // pagecount updating
    handlePagecount(new_pagecount);
    /**/tLogS(fname,5,pagecount);
  }
  // case 2: the fetch/parse has failed in some way...
  else {
    (page_one_results->fetchResults).halt = !skip_on_fail;
    if (page_one_results->wasRedirected) {
      /**/tLog(fname,6,page_one_results->newURL);
      (page_one_results->fetchResults).why_more = "Redirected to : " + (page_one_results->newURL);
    }
    /**/tLog(fname,7,(page_one_results->fetchResults).why_more);
    emit SendError(page_one_results->fetchResults);
  }
  // done
  delete page_one_results;
  /**/tLog(fname,8);
  return (current_error == jfa_DONE);
}
//-------------------------------
bool jfDownloadRootFirstPage::xFetchAllPages(size_t pagecount_estimate) {
  // initial page count setup
  const QString fname = "jfDownloadRootFirstPage::FetchAllPages";

  all_done = false;
  pagecount = pagecount_estimate;
  /**/tLogS(fname,1,pagecount);
  emit SendItemCount(pagecount_estimate);
  // handling the first page and it's results
  abort_fetch = false;
  bool page_one_result = DoFirstPage();
  /**/tLog2B(fname,2,page_one_result,abort_fetch);
  /**/tLogS(fname,3,pagecount);
  bool pagefail = ((!page_one_result) && (!skip_on_fail));
  /**/tLogB(fname,4,pagefail);

  // there are many reasons to end after the first page
  if (abort_fetch || (pagecount==1) || pagefail) {
    /**/tLog(fname,5);
    /**/tLog(fname,6);
    return page_one_result;
  }
  // checking if the user has asked for a halt
  /**/tLog(fname,7);
  if (pauser->CheckPauseStop()) {
    current_error = jfa_HALT;
    SendHaltMessage();
    /**/tLog(fname,8);
    return false;
  }
  // if we get here, we download more pages...
  /**/tLog(fname,9);
  bool main_results = xLoopGet();
  /**/tLogB(fname,10,main_results);
  return main_results;
}
//===============================================================

jfDownloadRootItems::jfDownloadRootItems():jfDownloadRootFirstPage(){
  current_collection = NULL;
  maindata = NULL;
  search_object = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfDownloadRootItems::SetDataAndSearch(jfResultCollection* in_data) {
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
void jfDownloadRootItems::WakeAfter() {
  syncher.wakeAll();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
bool jfDownloadRootItems::SendResults(jfResultUnitVector* res_vector) const {
  const QString fname = "jfDownloadRootItems::SendResults";
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
    res_vector->setWake(const_cast<jfDownloadRootItems*>(this));
    // now we have to dispatch the result info to the view
    search_object->DispatchResults(lstotal,res_vector);
    /**/tLog(fname,5);
    return true;
  }
}
//----------------------------------------------------------
void jfDownloadRootItems::WaitAfterSend() {
  wlock.lock();
  QCoreApplication::processEvents();
  syncher.wait(&wlock,1500);
  wlock.unlock();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
bool jfDownloadRootItems::ProcessResults(void* resultdata) {
  const QString fname = "jfDownloadRootItems::ProcessResults";
  assert(resultdata!=NULL);
  jfResultUnitVector* res_vector = static_cast<jfResultUnitVector*>(resultdata);
  // insering the results in the collection
  // we still might have to filter for duplicates
  if ((current_collection->hasIDFiltering()) && ((res_vector->size())>0)) {
    size_t filtered_out = current_collection->IDFilterMarkResults(res_vector);
    if (filtered_out>0) {
      jfResultUnitVector* new_vector = new jfResultUnitVector();
      new_vector->stotal = res_vector->stotal;
      jfBasePD* curr_item = NULL;
      // looping through the results, transferring those we keep
      for (size_t old_index = 0; old_index < res_vector->size() ; old_index++) {
        curr_item = const_cast<jfBasePD*>((*res_vector)[old_index]->Sourcelink());
        if (curr_item->included) {
          new_vector->push_back((*res_vector)[old_index]);
        }
        else {
          delete curr_item;
          delete ((*res_vector)[old_index]);
          (*res_vector)[old_index] = NULL;
        }
      }
      // done with the deletions and loop, nopw we clean and tranfer
      res_vector->clear();
      delete res_vector;
      res_vector = new_vector;
    }
  }
  // finishing
  if (!SendResults(res_vector)) {
    delete res_vector;
  }
  else WaitAfterSend();
  // done
  return true;
}

//------------------------------------
void jfDownloadRootItems::DisposeResults(void* resultdata) {
  const QString fname = "jfDownloadRootItems::DisposeResults";
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
jfPageParserBase* jfDownloadRootItems::makeParser() {
  jfItemsPageParserBase* cparser = makeItemParser();
  cparser->setSearchPointer(search_object);
  return cparser;
}
