/******************************************************************************
Name    :   ao3item_thread.cpp
Author  :   John Q Metro
Purpose :   Downloader class for AO3 fic listings
Created :   July 17, 2016
Updated :   July 17, 2016
******************************************************************************/
#ifndef AO3ITEM_THREAD_H
  #include "ao3item_thread.h"
#endif // AO3ITEM_THREAD_H
//------------------------------------
#ifndef AO3ITEM_PARSER_H
  #include "ao3item_parser.h"
#endif // AO3ITEM_PARSER_H
/*****************************************************************************/
// --- [ Methods for jfAO3FandomItemDownloader ] --------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfAO3FandomItemDownloader::jfAO3FandomItemDownloader(size_t in_max_threads):
              jfMultiCatBaseDownloader(in_max_threads) {
  current_category = NULL;
  ao3_search = NULL;
  tlogname = "AO3ItemDownloader";
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

//-----------------------------------------
QString* jfAO3FandomItemDownloader::makeURLforPage(size_t index) const {
  return new QString(ao3_search->GetIndexUrl(index));
}

/*****************************************************************************/

