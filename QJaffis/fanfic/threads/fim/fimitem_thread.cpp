/******************************************************************************
Name    :   fimitem_thread.cpp
Author  :   John Q Metro
Purpose :   FIM Groups search thread
Created :   June 30, 3016
Updated :   June 30, 2016
******************************************************************************/
#ifndef FIMITEM_THREAD_H
  #include "fimitem_thread.h"
#endif // FIMITEM_THREAD_H
//-------------------------------------------------
#ifndef FIMPARSE_ITEMS_H
  #include "fimparse_items.h"
#endif // FIMPARSE_ITEMS_H
#ifndef FIMPARSE_COMPACT_H
  #include "fimparse_compact.h"
#endif // FIMPARSE_COMPACT_H
//-------------------------------------------------
#include <assert.h>
#include <math.h>
/*****************************************************************************/
// the constructor
jfFIMItemDownloader::jfFIMItemDownloader(size_t in_max_threads):jfBaseItemDownloader(in_max_threads){
  compact_phase = true;
  fim_search = NULL;
  shortsum_storage = NULL;
  est_itemcount = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++
void jfFIMItemDownloader::StartProcessing() {
  const QString fname = "jfFIMItemDownloader::StartProcessing";

  // some initial setup before we start fetching
  initLog("FIMThread");
  emit SendStart(true);
  /**/tLog(fname,1);
  started = true;
  fim_search = dynamic_cast<jfFIMSearch*>(search_object);
  current_collection = maindata->GetCurrPointer();
  /**/tLog(fname,2);

  // doing the compact fetching (to get short summaries)
  bool result = FetchAllPages(2000);
  /**/tLogB(fname,3,result);
  if (!result) {
    AllDone(false);
    return;
  }

  // switching over to non-compact (full details)
  /**/tLog(fname,4);
  compact_phase = false;
  emit SendReset();
  size_t est_pcount = ceil(float(est_itemcount)/10.0);
  /**/tLogS(fname,5,est_pcount);

  // doing full search
  search_object->DispatchCategory();
  result = FetchAllPages(est_pcount);
  /**/tLogB(fname,6,result);

  // done
  delete shortsum_storage;
  AllDone(result);
}

//+++++++++++++++++++++++++++++++++++++++++++
bool jfFIMItemDownloader::AddToSummaryStore(jfIDStringStore* stuff_to_add) {
  const QString fname = "jfFIMItemDownloader::AddToSummaryStore";
  if (stuff_to_add == NULL) return false;
  if (shortsum_storage == NULL) return false;
  // going ahead
  jfIDStringStore::const_iterator inPtr = stuff_to_add->begin();
  size_t loopc = 0;
  while (inPtr != stuff_to_add->end()) {
    (*shortsum_storage)[inPtr->first] = inPtr->second;
    inPtr++;
    loopc++;
  }
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++
// implemented special virtual first page methods
//---------------------------------------
void jfFIMItemDownloader::makeFirstPageInfo() {
  infoToSend.startaction = "Doing";
  infoToSend.part_count = pagecount;
  infoToSend.item_index = 1;
  if (compact_phase) {
    infoToSend.item_label = "Card View Page";
    infoToSend.item_name = "(Short Summary)";
  }
  else {
    infoToSend.item_label = "Full View Page";
    infoToSend.item_name = "";
  }
}
//---------------------------------------
void jfFIMItemDownloader::ProcessFirstPageResults(void* resultdata) {
  // in the compact phase, we integrate short summaries
  if (compact_phase) {
    jfIDStringStore* stuff_to_add = static_cast<jfIDStringStore*>(resultdata);
    assert(stuff_to_add != NULL);
    shortsum_storage = stuff_to_add;
    est_itemcount = stuff_to_add->est_itemcount;
  }
  // otherwise, we insert and dispatch items...
  else {
    jfBaseItemDownloader::ProcessResults(resultdata);
  }
}
//---------------------------------------
// creates and sets up theparser for full items
jfItemsPageParserBase* jfFIMItemDownloader::makeItemParser() {
  jfFIMPageParser* parser = new jfFIMPageParser();
  parser->SetShortSummaries(shortsum_storage);
  return parser;
}
//+++++++++++++++++++++++++++++++++++++++++++
// overriden methods
//---------------------------------------
// process has two different types, based on phase
bool jfFIMItemDownloader::ProcessResults(void* resultdata) {
  const QString fname = "jfFIMItemDownloader::ProcessResults";
  /**/tLog(fname,1);
  // in the compact phase, we integrate short summaries
  if (compact_phase) {
    jfIDStringStore* stuff_to_add = static_cast<jfIDStringStore*>(resultdata);
    assert(stuff_to_add != NULL);
    est_itemcount = stuff_to_add->est_itemcount;
    AddToSummaryStore(stuff_to_add);
    delete stuff_to_add;
    return true;
  }
  // otherwise, we insert and dispatch items...
  else {
    return jfBaseItemDownloader::ProcessResults(resultdata);
  }
}
//---------------------------------------
// disposing the results (two phases as well)
void jfFIMItemDownloader::DisposeResults(void* resultdata) {
  if (compact_phase) {
    jfIDStringStore* typed_data = static_cast<jfIDStringStore*>(resultdata);
    delete typed_data;
  }
  else jfBaseItemDownloader::DisposeResults(resultdata);
}
//---------------------------------------
// creates the parser, we have 2 different types depending on phase
jfPageParserBase* jfFIMItemDownloader::makeParser() {
  if (compact_phase) {
    jfFIMCompactParser* cparser = new jfFIMCompactParser();
    cparser->SetDoMature(fim_search->GetMature());
    return cparser;
  }
  else return jfBaseItemDownloader::makeParser();
}
//+++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//---------------------------------------
// makes the url (phase dependant)
QString* jfFIMItemDownloader::makeURLforPage(size_t index) const {
  QString* result = new QString;
  if (compact_phase) (*result) = fim_search->GetCompactUrl(index);
  else (*result) = fim_search->GetIndexUrl(index);
  return result;
}
//---------------------------------------
 void jfFIMItemDownloader::PrepareItemInfo(size_t pageIndex) {
   infoToSend.startaction = "Doing";
   infoToSend.part_count = pagecount;
   infoToSend.item_index = pageIndex;
   if (compact_phase) {
     infoToSend.item_label = "Card View Page";
     infoToSend.item_name = "(Short Summary)";
   }
   else {
     infoToSend.item_label = "Full View Page";
     infoToSend.item_name = "";
   }
 }
/*****************************************************************************/
