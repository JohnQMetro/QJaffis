/******************************************************************************
Name    :   ffnitems_thread.cpp
Author  :   John Q Metro
Purpose :   Downloader class for FFN fic listings
Created :   July 24, 2016
Updated :   March 26, 2023
******************************************************************************/
#ifndef FFNITEMS_THREAD_H
  #include "ffnitems_thread.h"
#endif // FFNITEMS_THREAD_H
//------------------------------------
#ifndef FFN_ITEMCOLL
  #include "../data/ffn_itemcoll.h"
#endif

//-----------------------------------
#include <math.h>
/*****************************************************************************/
// --- [ Methods for jfFFNDownloader ] --------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNDownloader::jfFFNDownloader():jfMultiCatRootDownloader() {
  current_category = NULL;
  ffn_search = NULL;
  tlogname = "FFNItemDownloader";
  skip_cat_on_fail = true;
  ffn_parser = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual category related methods
//-----------------------------------------
bool jfFFNDownloader::NextCategory() {
    if (!ffn_search->NextIndex()) return false;
    current_category = ffn_search->GetCurrCat();
    ffn_search->DispatchCategory();

    QString new_name = (ffn_search->GetCatName())+" Result Fics";
    size_t new_id = current_category->GetID();

    jfFFNItemCollection* newcoll = dynamic_cast<jfFFNItemCollection*>(multi_maindata->AppendNewCollection(new_name, new_id));
    newcoll->ReplaceCat(current_category);
    current_collection = newcoll;
    ffn_parser->SetCategory(current_category);
    return true;
}
//-----------------------------------------
void jfFFNDownloader::LoadCatInfo() {
  catdata.startaction = "Searching";
  catdata.part_count = max_catcount;
  catdata.item_index = catindex;
  catdata.item_label = "Category";
  catdata.item_name = ffn_search->GetCatName();
}

//-----------------------------------------
size_t jfFFNDownloader::EstCatPagecount() const {
  size_t rcount = current_category->GetEstStoryCount();
  size_t pcount = ceil(float(rcount)/25.0);
  return pcount;
}
//-----------------------------------------
bool jfFFNDownloader::InitialSetup() {
  ffn_search = dynamic_cast<jfFFNSearch*>(search_object);
  multi_maindata = dynamic_cast<jfResultMultiCollection*>(maindata);
  max_catcount = ffn_search->GetNumCats();
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods that have to be implemented by derived threads
//-----------------------------------------
jfItemsPageParserBase* jfFFNDownloader::makeItemParser() {
  jfFFNItemParser* result = new jfFFNItemParser();
  result->SetCategory(current_category);
  return result;
}
// ----------------------------------------
jfParseFetchPackage* jfFFNDownloader::MakeParserFetcher() {
    ffn_parser = dynamic_cast<jfFFNItemParser*>(makeParser());
    if (ffn_parser == NULL) return NULL;
    jfParseFetchPackage* result = DefaultParseFetchMaker(jfft_FFN, jglobal::FPT_LISTING_PAGE, ffn_parser);
    if (result == NULL) delete ffn_parser;
    return result;
}
//-----------------------------------------
QString* jfFFNDownloader::makeURLforPage(size_t index) const {
  return new QString(ffn_search->GetIndexUrl(index));
}
//-----------------------------------------
void jfFFNDownloader::ProcessFirstPageResults(void* resultdata) {
  jfResultUnitVector* res_vector = static_cast<jfResultUnitVector*>(resultdata);
  size_t first_id = res_vector->firstid;
  ProcessResults(resultdata);
  jfFFNItemCollection* typed_collection = dynamic_cast<jfFFNItemCollection*>(current_collection);
  typed_collection->first_id = first_id;
}

/*****************************************************************************/
