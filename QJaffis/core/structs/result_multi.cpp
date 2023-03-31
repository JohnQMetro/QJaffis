/*****************************************************************************
Name:        result_multi.cpp
Author :     John Q Metro
Purpose :    A derivation of jfResultCollection for multi-category results
Created:     July 16, 2016
Updated:     March 25, 2023
******************************************************************************/
#ifndef RESULT_MULTI_H
  #include "result_multi.h"
#endif // RESULT_MULTI_H
//-----------------------------------

/*****************************************************************************/
// --- [ METHODS for jfResultMultiCollection ] --------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// core constructor
jfResultMultiCollection::jfResultMultiCollection(jfSearchCore* insearch, bool idfilter):
        jfResultCollection(insearch,idfilter) {

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// public methods for multiple collections
//---------------------------------
size_t jfResultMultiCollection::CollectionCount() const {
  return itemcount;
}
//---------------------------------
int jfResultMultiCollection::CollectionIndex() const {
  if (itemcount == 0) return -1;
  else return cindex;
}
//---------------------------------
bool jfResultMultiCollection::CollectionIndexIsValid() const {
  if (itemcount == 0) return false;
  else return (cindex<itemcount);
}
//---------------------------------
bool jfResultMultiCollection::toFirstCollection() {
  if (itemcount == 0) return false;
  else cindex = 0;
  return true;
}

//---------------------------------
bool jfResultMultiCollection::toNextCollection() {
  if (itemcount == 0) return false;
  if (cindex >= itemcount) return false;
  cindex++;
  return (cindex >= itemcount);
}
//---------------------------------
bool jfResultMultiCollection::NoResults() const {
  return (itemcount == 0);
}
//---------------------------------
jfSearchResultItemCollectionBase* jfResultMultiCollection::AppendNewCollection() {
    jfSearchResultItemCollectionBase* temp = MakeEmptyCollection();
    if (idstore != NULL) temp->SetIDFiltering(idstore);
    AddItem(temp);
    return temp;
}
//---------------------------------

jfSearchResultItemCollectionBase* jfResultMultiCollection::AppendNewCollection(const QString& cname, const size_t& cid) {
    jfSearchResultItemCollectionBase* temp = MakeEmptyCollection(cname, cid);
    if (idstore != NULL) temp->SetIDFiltering(idstore);
    AddItem(temp);
    return temp;
}

/*****************************************************************************/
