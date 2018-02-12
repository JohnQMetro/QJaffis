/******************************************************************************
Name    :   ffnitems_thread.cpp
Author  :   John Q Metro
Purpose :   Downloader class for FFN fic listings
Created :   July 24, 2016
Updated :   Febuary 11, 2018
******************************************************************************/
#ifndef FFNITEMS_THREAD_H
  #include "ffnitems_thread.h"
#endif // FFNITEMS_THREAD_H
//------------------------------------
#ifndef FFN_ITEMCOLL
  #include "ffn_itemcoll.h"
#endif
#ifndef FFNITEM_PARSER_H
  #include "ffnitem_parser.h"
#endif // FFNITEM_PARSER_H
/*****************************************************************************/
// --- [ Methods for jfFFNDownloader ] --------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNDownloader::jfFFNDownloader(size_t in_max_threads):jfMultiCatBaseDownloader(in_max_threads) {
  current_category = NULL;
  ffn_search = NULL;
  tlogname = "FFNItemDownloader";
  skip_cat_on_fail = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual category related methods
//-----------------------------------------
bool jfFFNDownloader::NextCategory() {
  if (!ffn_search->NextIndex()) return false;
  current_category = ffn_search->GetCurrCat();
 ffn_search->DispatchCategory();
  jfFFNItemCollection* newcoll = dynamic_cast<jfFFNItemCollection*>(multi_maindata->AppendNewCollection());
  newcoll->ReplaceCat(current_category);
  newcoll->SetName((ffn_search->GetCatName())+" Result Fics");
  newcoll->SetID(current_category->GetID());
  current_collection = newcoll;
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
