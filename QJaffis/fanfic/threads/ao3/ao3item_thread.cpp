/******************************************************************************
Name    :   ao3item_thread.cpp
Author  :   John Q Metro
Purpose :   Downloader class for AO3 fic listings
Created :   July 17, 2016
Updated :   July 30, 2022
******************************************************************************/
#ifndef AO3ITEM_THREAD_H
  #include "ao3item_thread.h"
#endif // AO3ITEM_THREAD_H

//------------------------------------
#include <math.h>
/*****************************************************************************/
// --- [ Methods for jfAO3FandomItemDownloader ] --------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfAO3FandomItemDownloader::jfAO3FandomItemDownloader():jfMultiCatRootDownloader() {
  current_category = NULL;
  ao3_search = NULL;
  tlogname = "jfAO3FandomItemDownloader";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual category related methods
//-----------------------------------------
bool jfAO3FandomItemDownloader::NextCategory() {
  if (!ao3_search->NextIndex()) return false;
  current_category = ao3_search->GetCurrCat();
  ao3_search->DispatchCategory();
  jfAO3_FicList* newcoll = dynamic_cast<jfAO3_FicList*>(multi_maindata->AppendNewCollection());
  newcoll->ReplaceCat(current_category);
  newcoll->SetName((ao3_search->GetCatName())+" Result Fics");
  newcoll->SetID(current_category->GetID());
  current_collection = newcoll;
  ao3_parser->SetCategory(current_category);
  return true;
}
//-----------------------------------------
void jfAO3FandomItemDownloader::LoadCatInfo() {
  catdata.startaction = "Searching";
  catdata.part_count = max_catcount;
  catdata.item_index = catindex;
  catdata.item_label = "Fandom";
  catdata.item_name = ao3_search->GetCatName();
}

//-----------------------------------------
size_t jfAO3FandomItemDownloader::EstCatPagecount() const {
  size_t rcount = current_category->GetRCount();
  size_t pcount = ceil(float(rcount)/20.0);
  return pcount;
}
//-----------------------------------------
bool jfAO3FandomItemDownloader::InitialSetup() {
  ao3_search = dynamic_cast<jfAO3Search*>(search_object);
  multi_maindata = dynamic_cast<jfResultMultiCollection*>(maindata);
  max_catcount = ao3_search->GetNumCats();
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods that have to be implemented by derived threads
//-----------------------------------------
jfItemsPageParserBase* jfAO3FandomItemDownloader::makeItemParser() {
  jfAO3ItemParser* result = new jfAO3ItemParser();
  result->SetCategory(current_category);
  return result;
}
// ----------------------------------
jfParseFetchPackage* jfAO3FandomItemDownloader::MakeParserFetcher() {
    ao3_parser = dynamic_cast<jfAO3ItemParser*>(makeParser());
    if (ao3_parser != NULL) {
        jfParseFetchPackage* result = DefaultParseFetchMaker(jfft_AO3, jglobal::FPT_LISTING_PAGE, ao3_parser);
        if (result == NULL) delete ao3_parser;
        return result;
    }
    else return NULL;
}

//-----------------------------------------
QString* jfAO3FandomItemDownloader::makeURLforPage(size_t index) const {
  return new QString(ao3_search->GetIndexUrl(index));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfAO3FandomItemDownloader::ClearWorkers(bool delete_parser) {
    if (ao3_parser != NULL) ao3_parser->SaveCounts();
    jfMultiCatRootDownloader::ClearWorkers(delete_parser);
}

/*****************************************************************************/

