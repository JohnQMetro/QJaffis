/******************************************************************************
* Name:        auth_coll.cpp
* Author :     John Q Metro
* Purpose :    Collections involving author data
* Created:     January 19, 2011
* Updated:     August 8, 2016
******************************************************************************/
#ifndef AUTH_COLL_H_INCLUDED
  #include "auth_coll.h"
#endif // AUTH_COLL_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//--------------------------------------------
#include <assert.h>
/*****************************************************************************/
// construction
//--------------------------------------------------
// defult constructor, SetFromString must be called before object is usable
jfAuthorStories::jfAuthorStories() {
  author_data = NULL;
}
//--------------------------------------------------
// constructor that calls set from string. be sure to check if IsValid is true before using
jfAuthorStories::jfAuthorStories(size_t author_id, const QString& author_name) {
  author_data = new jfAuthorInfo(author_id,author_name);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// inserting new items
bool jfAuthorStories::InsertNewFic(jfFFNItemAuthor* new_item) {
  if (new_item == NULL) return false;
  if (InclFic(new_item->GetID())) return false;
  new_item->ChangeAuthorname(author_data->GetAuthorName());
  new_item->ChangeAuthorID(author_data->GetAuthorID());
  maindata[new_item->GetID()] = new_item;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting the author info
//--------------------------------------------------
size_t jfAuthorStories::GetAuthID() const {
  if (author_data==NULL) return 0;
  else return author_data->GetAuthorID();
}
//--------------------------------------------------
QString jfAuthorStories::GetAuthName() const {
  if (author_data==NULL) return "";
  else return author_data->GetAuthorName();
}
//----------------------------------------------------
const jfAuthorInfo* jfAuthorStories::GetAuthorInfo() const {
  return author_data;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// public checking methods
//--------------------------------------------------
// true if a fic with the given ID is in this object
bool jfAuthorStories::InclFic(size_t ficidin) const {
  // the iterator
  std::map<size_t,jfFFNItemAuthor*>::const_iterator xval;
  // looking
  xval = maindata.find(ficidin);
  return (xval!=maindata.end());
}
//--------------------------------------------------
// returns true if a contained fic is a newer version if the 'tocheck' one
bool jfAuthorStories::IsUpdated(const jfFFNItem* tocheck) const {
  // variables
  size_t infid;
  std::map<size_t,jfFFNItemAuthor*>::const_iterator xval;
  const jfFFNItemAuthor* yval;
  // checks. if the fic is bad or not in here, assertion fail!
  assert(tocheck!=NULL);
  infid = tocheck->GetID();
  assert(InclFic(infid));
  // startups
  xval = maindata.find(infid);
  yval = xval->second;
  // passed the checks. if the new fic has not been updated, return false
  if (!(yval->IsUpdated())) return false;
  // the new fic has been dated, we compare the updates
  if ((yval->GetUpdates())>(tocheck->GetUpdates())) return true;
  return false;
}
//--------------------------------------------------
// returns a fic with the specified ID (NULL if not found)
const jfFFNItemAuthor* jfAuthorStories::GetItemPtr(size_t inficidx) const {
  if (!InclFic(inficidx)) return NULL;
  else return maindata.find(inficidx)->second;
}
//-------------------------------------------------
// gets the number of fics
size_t jfAuthorStories::GetICount() const {
  return maindata.size();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// clearing and emptying data
//-------------------------------------------------
// deletes all the fics
void jfAuthorStories::Clear() {
  size_t mloop, mmax;
  mmax = GetICount();
  for(mloop=0;mloop<mmax;mloop++) {
    delete maindata[mloop];
    maindata[mloop] = NULL;
  }
  maindata.clear();
}
//-------------------------------------------------
// destructor
jfAuthorStories::~jfAuthorStories() {
  Clear();
  if (author_data!=NULL) {
    delete author_data;
    author_data = NULL;
  }
}
//============================================================================
// returns true if data for the given author is in the collection
bool jfAuthorCollection::HasAuthor(const jfAuthorInfo* in_authdata) const {
  const QString fname = "jfAuthorCollection::HasAuthor";
  // variables
  std::map<size_t,jfAuthorStories*>::const_iterator idx;
  // a basic check
  if (in_authdata==NULL) return false;
  // we look for the index
  idx = maindata.find(in_authdata->GetAuthorID());
  // the result
  return (idx!=maindata.end());
}
//----------------------------------------------------
bool jfAuthorCollection::HasAuthor(size_t authorid) const {
  std::map<size_t,jfAuthorStories*>::const_iterator idx;
  idx = maindata.find(authorid);
  return (idx!=maindata.end());
}
//++++++++++++++++++++++++++++++++++++++++++++++
// getting info
//--------------------------------------------
// returns the number of authors
size_t jfAuthorCollection::GetAuthCount() const {
  return maindata.size();
}
//-------------------------------------------
// returns a const pointer to author data
const jfAuthorStories* jfAuthorCollection::GetAuthorStories(size_t inauthid) const {
  // variables
  std::map<size_t,jfAuthorStories*>::const_iterator idx;
  // we look for the index
  idx = maindata.find(inauthid);
  // the result
  if (idx!=maindata.end()) return idx->second;
  else return NULL;
}
//-------------------------------------------------
const jfAuthorInfo* jfAuthorCollection::GetAuthorInfo(size_t inauthid) const {
    // variables
  std::map<size_t,jfAuthorStories*>::const_iterator idx;
  const jfAuthorStories* qval;
  // we look for the index
  idx = maindata.find(inauthid);
  // the result
  if (idx!=maindata.end()) {
    qval = idx->second;
    return qval->GetAuthorInfo();
  }
  else return NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++
// adding an author
bool jfAuthorCollection::AddAuthor(jfAuthorStories* in_authdata, bool replace) {
  // variables
  size_t auth_id;
  // checks and assertions
  assert(in_authdata!=NULL);
  // we start
  auth_id = in_authdata->GetAuthID();
  if (HasAuthor(auth_id)) {
    if (!replace) return false;
    else delete maindata[auth_id];
  }
  maindata[auth_id] = in_authdata;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++
// deleting
//--------------------------------------------
void jfAuthorCollection::Clear() {
  size_t icount,iloop;
  icount = GetAuthCount();
  for (iloop=0;iloop<icount;iloop++) {
    delete maindata[iloop];
    maindata[iloop] = NULL;
  }
  maindata.clear();
}
//--------------------------------------------
jfAuthorCollection::~jfAuthorCollection() {
  Clear();
}
/*****************************************************************************/
