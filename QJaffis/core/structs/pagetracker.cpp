/*****************************************************************************
Name:        pagetracker.cpp
Author :     John Q Metro
Purpose :    Defines a class to keep track of top-level notebook pages
Created:     November 8, 2009
Qt Conversion Started September 14, 2013
Updated:     February 16, 2013
******************************************************************************/
#ifndef PAGETRACKER_H_INCLUDED
  #include "pagetracker.h"
#endif // PAGETRACKER_H_INCLUDED

#include <assert.h>
/******************************************************************************/
// the constructor
jfPageTracker::jfPageTracker() {
  num_items = 0;
  pagelist = NULL;
  htmlready = NULL;
  typelist = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic information
//--------------------------------------------------------------------
size_t jfPageTracker::GetCount() const {
  return num_items;
}
//--------------------------------------------------------------------
int jfPageTracker::GetWindowIndex(QWidget* incheck) const {
  // variables
  int result;
  size_t loopc;
  // a special case
  if (num_items==0) return -1;
  // starting
  result = -1;
  for (loopc=0;loopc<num_items;loopc++) {
    if (((*pagelist)[loopc])==incheck) {
      result = loopc;
      break;
    }
  }
  // done
  return result;
}
//--------------------------------------------------------------------
bool jfPageTracker::IsHtmlReady(size_t index) const {
  // check that the index is in range
  assert(index<num_items);
  // look up the result
  return (*htmlready)[index];
}
//--------------------------------------------------------------------
// returns the index of the global filter editor
int jfPageTracker::FindGlobEdit() const {
  // starting
  if (num_items==0) return -1;
  // the loop
  for (size_t loopc=0;loopc<num_items;loopc++) {
    if (((*typelist)[loopc])==jftl_GLOBFILEDIT) return loopc;
  }
  return -1;
}
//--------------------------------------------------------------------
int jfPageTracker::FindSkelEdit() const {
  // starting
  if (num_items==0) return -1;
  // the loop
  for (size_t loopc=0;loopc<num_items;loopc++) {
    if (((*typelist)[loopc])==jftl_SKELEDIT) return loopc;
  }
  return -1;
}
//--------------------------------------------------------------------
// return true if one of the pages is the Global Filter Editor
bool jfPageTracker::GlobEditLoaded() const {
  return (FindGlobEdit()!=-1);
}
//---------------------------------------------------------------------
bool jfPageTracker::SkelEditLoaded() const {
    return (FindSkelEdit()!=-1);
}
//---------------------------------------------------------------------
bool jfPageTracker::IsGlobEdit(size_t index) const {
  assert(index<num_items);
  return (((*typelist)[index])==jftl_GLOBFILEDIT);
}
//---------------------------------------------------------------------
bool jfPageTracker::IsGlobSkel(size_t index) const {
  assert(index<num_items);
  return (((*typelist)[index])==jftl_SKELEDIT);
}
//-------------------------------------------------------------------
bool jfPageTracker::NotHtml(size_t index) const {
  jf_tlPageType cenum;
  assert(index<num_items);
  // we extract
  cenum = (*typelist)[index];
  // and test
  return ((cenum==jftl_GLOBFILEDIT) || (cenum==jftl_SKELEDIT)  || (cenum==jftl_FICUPDATE) || (cenum==jftl_TESTS));
}
//------------------------------------------------------------
bool jfPageTracker::IsUpdater(size_t index) const {
  assert(index<num_items);
  return (((*typelist)[index])==jftl_FFNUPDATE);
}
//-------------------------------------------------------------------
bool jfPageTracker::IsFFNSearch(size_t index) const {
  jf_tlPageType cenum;
  assert(index<num_items);
  // we extract
  cenum = (*typelist)[index];
  // and test
  return ((cenum==jftl_FFNUPDATE) || (cenum==jftl_FFNSEARCH));
}
//-----------------------------------------------------------
bool jfPageTracker::IsFicUpdate(size_t index) const {
  assert(index<num_items);
  return (((*typelist)[index])==jftl_FICUPDATE);
}
//-----------------------------------------------------------
bool jfPageTracker::IsTests(size_t index) const {
    assert(index<num_items);
    return (((*typelist)[index])==jftl_TESTS);
}
//------------------------------------------------------------
jf_tlPageType jfPageTracker::GetType(size_t index) const {
  assert(index<num_items);
  // we extract
  return (*typelist)[index];
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// maniupulating pages
//--------------------------------------------------------------------
// adds a page to the tracker
bool jfPageTracker::AddPage(QWidget* inpage,jf_tlPageType ptype) {
  // the global filter editor must be unique, it must not be added twice
  if (ptype==jftl_GLOBFILEDIT) {
    if (GlobEditLoaded()) return false;
  }
  // the global skeleton editor must also be unique.
  if (ptype==jftl_SKELEDIT) {
    if (SkelEditLoaded()) return false;
  }
  // we start...
  // if the current number of pages is 0, we must create the vectors
  if (num_items==0) {
    pagelist    = new std::vector<QWidget*>();
    htmlready = new std::vector<bool>();
    typelist  = new jfPTypeVector();
  }
  // adding the pages
  pagelist->push_back(inpage);
  htmlready->push_back(false);
  typelist->push_back(ptype);
  num_items++;
  // done..
  return true;
}
//--------------------------------------------------------------------
// removes a page from the tracker
bool jfPageTracker::RemovePage(size_t index) {
  // the basic check
  if (index>=num_items) return false;
  // otherwise, we go ahead
  pagelist->erase((pagelist->begin())+index);
  htmlready->erase((htmlready->begin())+index);
  typelist->erase((typelist->begin())+index);
  num_items--;
  // if num_items is now zero, we delete the vectors as well
  if (num_items==0) {
    delete pagelist;
    delete htmlready;
    delete typelist;
    pagelist = NULL;
    htmlready = NULL;
    typelist = NULL;
  }
  return true;
}
//--------------------------------------------------------------------
// sets the htmlready value
bool jfPageTracker::SetHtmlReady(QWidget* inpage, bool inval) {
  int idcheck = GetWindowIndex(inpage);
  if (idcheck==-1) return false;
  (*htmlready)[idcheck] = inval;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfPageTracker::~jfPageTracker() {
  if (num_items!=0) {
    delete pagelist;
    delete htmlready;
    delete typelist;
    pagelist = NULL;
    htmlready = NULL;
    typelist = NULL;
  }
}
/******************************************************************************/
