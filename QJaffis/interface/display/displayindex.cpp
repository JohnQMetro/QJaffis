/*****************************************************************************
Name:        displayindex.cpp
Author :     John Q Metro
Purpose :    Organizing the indexes of results for the viewing listboxes
Created:     September 1, 2013
Updated:     March 25, 2012
******************************************************************************/
#ifndef DISPLAYINDEX_H
  #include "displayindex.h"
#endif // DISPLAYINDEX_H
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//---------------------------------
#include <assert.h>
#include <QMutexLocker>

//*****************************************************************************
// constructor
jfResultIndexes_Category::jfResultIndexes_Category(header_enum inhead, bool in_usedefault, size_t in_numrcats) {
  const QString fname = "jfResultIndexes_Category::jfResultIndexes_Category";
  // basic value setup
  hval = inhead;
  use_default = in_usedefault;
  rcat_count = in_numrcats;
  if (rcat_count==0) assert(use_default);
  // setting up the indexes
  for (size_t qval=0;qval<33;qval++) {
    cat_offset[qval] = 0;
    num_results[qval] = 0;
  }
  item_count = 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
bool jfResultIndexes_Category::InitializeFromEndpoints(std::vector<size_t>* inpoints) {
  if (inpoints==NULL) return false;
  if (inpoints->size()!=33) return false;
  for (size_t qval=0;qval<33;qval++) {
    cat_offset[qval] = inpoints->at(qval);
  }
  return true;
}
//--------------------------------------------
void jfResultIndexes_Category::SetCatData(struct jfCategoryInfoMsg in_catdata) {
  catdata = in_catdata;
}
//--------------------------------------------
size_t jfResultIndexes_Category::AddResults(jfResultUnitVector* inresults) {
  const QString fname = "jfResultIndexes_Category::AddResults";
  // special case
  if (inresults==NULL) return 0;
  // variables for more general case
  size_t inr_cnt, inr_idx;
  size_t rc_idx;
  jfResultUnit* wval;
  // setting up for the loop
  inr_cnt = inresults->size();
  // the main loop
  for (inr_idx=0;inr_idx<inr_cnt;inr_idx++) {
    wval = inresults->at(inr_idx);
    // incrementing the counts
    for (rc_idx=0;rc_idx<rcat_count;rc_idx++) {
      if (wval->InCategory(rc_idx)) num_results[rc_idx]++;
    }
    if (use_default) {
      if (!(wval->HasCategory())) num_results[32]++;
    }
    // appending the value
    results.push_back(wval);
  }
  item_count += inresults->stotal;
  // done
  return inr_cnt;
}
//-------------------------------------------------------------
size_t jfResultIndexes_Category::AddToTotal(size_t increment) {
  item_count += increment;
  return item_count;
}

//++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
std::vector<size_t>* jfResultIndexes_Category::GetEndpoints() const {
  // variables
  std::vector<size_t>* resval;
  size_t rc_idx;
  size_t rcval;
  // special case
  assert(hval!=he_ONECATEGORY);
  // setting up...
  resval = new std::vector<size_t>();
  // the loop
  for (rc_idx=0;rc_idx<33;rc_idx++) {
    rcval = cat_offset[rc_idx]+num_results[rc_idx];
    if (hval==he_ALWAYS) rcval++;
    if (hval==he_NONEMPTY) {
      if (num_results[rc_idx]!=0) rcval++;
    }
    resval->push_back(rcval);
  }
  // done
  return resval;
}
//--------------------------------------------
size_t jfResultIndexes_Category::GetNumres(size_t index) const {
  assert(index<33);
  return num_results[index];
}
//--------------------------------------------
QString jfResultIndexes_Category::GetCatname() const {
  return catdata.catname;
}
//--------------------------------------------
QString jfResultIndexes_Category::GetCatHtml() const {
  return catdata.catheader;
}
//------------------------------------------------------
QString jfResultIndexes_Category::GetResCatHeader(size_t rcidx) const {
  assert(rcidx<33);
  return catdata.catheader + MakeCountLine(rcidx);
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfResultIndexes_Category::~jfResultIndexes_Category() {
  // variables
  size_t res_num, res_idx;
  // starting
  res_num = results.size();
  for (res_idx=0;res_idx<res_num;res_idx++) {
    delete results[res_idx];
    results[res_idx] = NULL;
  }
  results.clear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfResultIndexes_Category::MakeCountLine(size_t rcidx) const {
  assert(rcidx<33);
  QString result;
  double pcalc;
  // building the result
  result = "<font size=+1 color=\"red\">";
  result += QString::number(num_results[rcidx]) + " items shown, out of ";
  result += QString::number(item_count) + " total (";
  pcalc = ((double)(num_results[rcidx]))/((double)(item_count));
  pcalc *= 100;
  result += QString::number(pcalc,'f',2) + "%).</font>";
  return result;
}
//****************************************************************************
jfDisplayIndexGroup::jfDisplayIndexGroup(header_enum inhead, bool in_usedefault, size_t in_numrcats) {
  // basic value setup
  hval = inhead;
  use_default = in_usedefault;
  num_rcats = in_numrcats;
  if (num_rcats==0) assert(use_default);
  // if we have one catgeory, ...
  catdex = 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
//-----------------------------------------------
void jfDisplayIndexGroup:: NewCategory(struct jfCategoryInfoMsg in_catdata) {
  const QString fname = "jfDisplayIndexGroup:: NewCategory";
  QMutexLocker mlocker(&mlock);
  bool tval;
  // creating and setting the initial data for the new category
  jfResultIndexes_Category* newcat = new jfResultIndexes_Category(hval,use_default,num_rcats);
  newcat->SetCatData(in_catdata);
  if (catresults.size()>0) {
    tval = newcat->InitializeFromEndpoints(catresults[catdex]->GetEndpoints());
    assert(tval);
  }
  // adding the category to our list
  catresults.push_back(newcat);
  catdex = (catresults.size()-1);
}

//-----------------------------------------------
size_t jfDisplayIndexGroup::AddResults(jfResultUnitVector* inresults) {
  QMutexLocker mlocker(&mlock);
  assert(catresults.size()>0);
  return catresults[catdex]->AddResults(inresults);
}
//---------------------------------------------
size_t jfDisplayIndexGroup::AddToTotal(size_t increment) {
  QMutexLocker mlocker(&mlock);
  assert(catresults.size()>0);
  return catresults[catdex]->AddToTotal(increment);
}
//----------------------------------------------
// getting data
//----------------------
// returns true if the number of results is empty
bool jfDisplayIndexGroup::NoCurrentResults(size_t rcat_index) const {
  QMutexLocker mlocker(&mlock);
  // the usual checks
  assert(catresults.size()>0);
  assert(rcat_index<33);
  // getting the result
  return ((catresults[catdex]->GetNumres(rcat_index))==0);
}
//---------------------------------------------
QString jfDisplayIndexGroup::GetCurrentCatHtml(size_t rcidx) const {
  QMutexLocker mlocker(&mlock);
  assert(catresults.size()>0);
  return catresults[catdex]->GetResCatHeader(rcidx);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfDisplayIndexGroup::~jfDisplayIndexGroup() {
  size_t xmax, xloop;
  xmax = catresults.size();
  for (xloop=0;xloop<xmax;xloop++) {
    delete catresults[xloop];
    catresults[xloop] = NULL;
  }
  catresults.clear();
}


//*****************************************************************************
