/*****************************************************************************
Name    : baseparse.cpp
Basic   : Defines a base class that handles page parsing (and redirection)
Author  : John Q Metro
Started : June 13, 2016
Updated : June 20, 2016

******************************************************************************/
#ifndef BASEPARSE_H
  #include "baseparse.h"
#endif // BASEPARSE_H
#ifndef BASEOBJ_H_INCLUDED
  #include "../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
//-----------------------------------
#include <assert.h>

/*****************************************************************************/
// a base abstract class for parsing downloaded pages
//++++++++++++++++++++++++++++++++++++++++++++++++
jfPageParserBase::jfPageParserBase() {
  page_parsed = false;
  pagecount = 0;
}
//--------------------------------------------
void jfPageParserBase::SetLogPointer(jfLoggBase* in_ptr) {
  lpt = in_ptr;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// getting results
//--------------------------------------------
bool jfPageParserBase::isPageParsed() const {
  return page_parsed;
}
//--------------------------------------------
QString jfPageParserBase::getParseErrorMessage() const {
  return parseErrorMessage;
}
//--------------------------------------------
size_t jfPageParserBase::getPageCount() const {
  return pagecount;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPageParserBase::NewData(const QString& inPage) {
  xparser.ChangeData(inPage);
  page_parsed = false;
  parseErrorMessage.clear();
  pagecount = 0;
  return (!inPage.isEmpty());
}
//--------------------------------------------
bool jfPageParserBase::parsErr(const QString& err_in) {
  parseErrorMessage = err_in;
  return false;
}
//============================================================================
jfItemsPageParserBase::jfItemsPageParserBase():jfPageParserBase() {
  page_results = NULL;
  start_itemcount = 0;
  search_ptr = NULL;
}
//-----------------------------------
bool jfItemsPageParserBase::setSearchPointer(jfSearchCore* in_sptr) {
  if (in_sptr == NULL) return false;
  search_ptr = in_sptr;
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
jfResultUnitVector* jfItemsPageParserBase::MakeResults() {
  if (page_results == NULL) return NULL;
  // local variables
  jfResultUnitVector* result = new jfResultUnitVector();
  jfResultUnit* tresult;
  size_t pr_size = page_results->size();
  // we start
  if (pr_size>0) {
    for (size_t pr_index=0;pr_index<pr_size;pr_index++) {
      tresult = (*page_results)[pr_index]->makeResultUnit();
      result->push_back(tresult);
    }
  }
  result->stotal = start_itemcount;
  return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// filtering and removing from the current page
size_t jfItemsPageParserBase::FilterByList() {
  // constants
  const QString fname = "jfItemsPageParserBase::FilterByList";
  // variables and checks
  size_t fsize,rcount;
  assert(page_results!=NULL);
  assert(search_ptr->Check());
  jfBasePD* tvector;
  QStringList* scheck;
  // the constructor of the items should have them be included by default
  lpt->tLog(fname,1);
  // we use a special cases first
  if ((search_ptr->def_filtermap)==NULL) return start_itemcount;
  fsize = (search_ptr->def_filtermap)->GetCount();
  if (fsize==0) return start_itemcount;
  rcount = 0;
  scheck = search_ptr->def_filtermap->GetNameList();
  delete scheck;
  // we assume pr_size is correct
  for (size_t pr_index = 0 ; pr_index < start_itemcount ; pr_index++) {
    // and then we test against the filter list
    tvector = (*page_results)[pr_index];
    tvector->included = (search_ptr->def_filtermap)->MatchAll(tvector);
    if (tvector->included) rcount++;
  }
  return rcount;
}
//-----------------------------------------
size_t jfItemsPageParserBase::CategorySort() {
  jfBasePD* tvector;
  size_t rcount;
  assert(search_ptr->Check());
  // the main checksing loop
  rcount = start_itemcount;
  for (size_t pr_index = 0 ; pr_index < start_itemcount ; pr_index++) {
    // and then we test against the filter list
    tvector = (*page_results)[pr_index];
    (search_ptr->categories)->Sort(tvector);
    if (search_ptr->NoCatTest(tvector)) tvector->included = false;
    if (!(tvector->included)) rcount--;
  }
  return rcount;
}

//-----------------------------------------
size_t jfItemsPageParserBase::Condense() {
  // the result list
  jfPDVector* newlist;
  jfBasePD* tvector;
  // we start
  newlist = new jfPDVector();
  // the loop
  for (size_t pr_index = 0 ; pr_index < start_itemcount ;pr_index++) {
    // and then we check
    tvector = (*page_results)[pr_index];
    if (tvector->included) newlist->push_back(tvector);
    else {
      delete tvector;
      (*page_results)[pr_index] = NULL;
    }
  }
  // at the end of the loop, we get rid of the old
  delete page_results;
  // and usher in the new
  page_results = newlist;
  return newlist->size();
}
//-----------------------------------------
void jfItemsPageParserBase::PostPageProcessing() {
  assert(page_results != NULL);
  jfBasePD* worki;
  // filtering and sorting
  start_itemcount = page_results->size();
  FilterByList();
  CategorySort();
  Condense();
  // processing the summary (removing bad tags, etc)
  if ((page_results!=NULL) && (start_itemcount>0)) {
    for (size_t loopc=0 ; loopc<(page_results->size()) ; loopc++) {
      worki = (*page_results)[loopc];
      worki->ProcessDescription();
    }
  }
  // done
}

/*****************************************************************************/
