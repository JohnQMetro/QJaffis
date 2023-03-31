/*****************************************************************************
Name    : baseparse.cpp
Basic   : Defines a base class that handles page parsing (and redirection)
Author  : John Q Metro
Started : June 13, 2016
Updated : March 20, 2023

******************************************************************************/
#ifndef BASEPARSE_H
  #include "baseparse.h"
#endif // BASEPARSE_H
#ifndef BASEOBJ_H_INCLUDED
  #include "../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
//-----------------------------------
#include <assert.h>

#include <QtConcurrent/QtConcurrent>

/*****************************************************************************/
// a base abstract class for parsing downloaded pages
//++++++++++++++++++++++++++++++++++++++++++++++++
jfPageParserCore::jfPageParserCore() {
  page_parsed = false;
  pagecount = 0;
}
jfPageParserCore::~jfPageParserCore() {}
//--------------------------------------------
void jfPageParserCore::SetLogPointer(jfLoggBase* in_ptr) {
  lpt = in_ptr;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// getting results
//--------------------------------------------
bool jfPageParserCore::isPageParsed() const {
  return page_parsed;
}
//--------------------------------------------
QString jfPageParserCore::getParseErrorMessage() const {
  return parseErrorMessage;
}
//--------------------------------------------
size_t jfPageParserCore::getPageCount() const {
  return pagecount;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPageParserCore::NewData(const QString& inPage) {
  const QString fname = "jfPageParserBase::NewData";
  /**/lpt->tLog(fname,1);
  xparser.ChangeData(inPage);
  /**/lpt->tLog(fname,2);
  page_parsed = false;
  parseErrorMessage.clear();
  /**/lpt->tLog(fname,3);
  pagecount = 0;
  return (!inPage.isEmpty());
}
//--------------------------------------------
bool jfPageParserCore::parsErr(const QString& err_in) {
  parseErrorMessage = err_in;
  return false;
}
//==============================================================
// a base abstract class for parsing downloaded pages
//++++++++++++++++++++++++++++++++++++++++++++++++
jfPageParserBase::jfPageParserBase():jfPageParserCore() { }
jfPageParserBase::~jfPageParserBase() { }
//++++++++++++++++++++++++++++++++++++++++++++++++
// getting results
//--------------------------------------------
// default does nothing
bool jfPageParserBase::setPageIndex(const size_t& page_index) { return true; }

//============================================================================
//============================================================================

jfItemsPageParserBase::jfItemsPageParserBase():jfPageParserBase() {
  page_results = NULL;
  start_itemcount = 0;
  search_ptr = NULL;
  item_parser = NULL;
  item_outputter = NULL;
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
    assert(item_outputter != NULL);
    if (page_results == NULL) return NULL;
    // local variables
    jfResultUnitVector* result = new jfResultUnitVector();
    jfResultUnit* tresult;
    size_t pr_size = page_results->size();
    // we start
    if (pr_size>0) {
        for (size_t pr_index=0;pr_index<pr_size;pr_index++) {

            const jfSearchResultItem* item = page_results->at(pr_index).item;
            jfItemMetaFlags* flags =  page_results->at(pr_index).flags;
            item_outputter->SetItemSource(item, flags);

            tresult = new jfResultUnit(item, flags,item_outputter->ToDisplayHTML());
            result->push_back(tresult);
        }
    }
    result->stotal = start_itemcount;
    return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// a 'function object' for use in parallel filtering
struct FilterApplyer {
    FilterApplyer(const jfFilterMap* filters_to_use):filters(filters_to_use) {}

    void operator()(jfItemFlagGroup &item) {
        if (item.flags == NULL) item.flags = new jfItemMetaFlags();
        item.flags->included = filters->MatchAll(item.item);
    }

    const jfFilterMap* filters;
    size_t passed_count = 0;
};

// -------------------------------------------------------
// filtering and removing from the current page
size_t jfItemsPageParserBase::FilterByList() const {
    // constants
    const QString fname = "jfItemsPageParserBase::FilterByList";
    // variables and checks
    size_t fsize,rcount;
    /**/lpt->tAssert(page_results!=NULL,fname,"Page Results are NULL");
    /**/lpt->tAssert(search_ptr->Check(),fname,"Search check failed");
    /**/lpt->tLog(fname,1);

    QStringList* scheck;
    // the constructor of the items should have them be included by default
    /**/lpt->tLog(fname,2);


    // we use a special cases first
    if ((search_ptr->def_filtermap)==NULL) return start_itemcount;
    fsize = (search_ptr->def_filtermap)->GetCount();
    if (fsize==0) return start_itemcount;
    /**/lpt->tLogS(fname,3,fsize);
    rcount = 0;
    scheck = search_ptr->def_filtermap->GetNameList();
    delete scheck;
    /**/lpt->tLog(fname,6);
    FilterApplyer applyer = FilterApplyer(search_ptr->def_filtermap);

    QtConcurrent::blockingMap(*page_results, applyer);


    // we assume pr_size is correct
    /*
  for (size_t pr_index = 0 ; pr_index < start_itemcount ; pr_index++) {
    // and then we test against the filter list
    tvector = (*page_results)[pr_index];
    // lpt->tLogL(fname,7,pr_index);
    // lpt->tLog(fname,8,tvector->GetName());
    tvector->included = (search_ptr->def_filtermap)->MatchAll(tvector);
    // lpt->tLogB(fname,9,tvector->included);
    if (tvector->included) rcount++;
  }
  */

    for (size_t pr_index = 0; pr_index < start_itemcount; pr_index++) {
        const jfItemMetaFlags* flags  = page_results->at(pr_index).flags;
        if ((flags == NULL) || (flags->included)) rcount++;
    }

    return rcount;
}
//-----------------------------------------
size_t jfItemsPageParserBase::CategorySort() {
    size_t rcount;
    assert(search_ptr->Check());
    // the main checksing loop
    rcount = start_itemcount;
    for (size_t pr_index = 0 ; pr_index < start_itemcount ; pr_index++) {
        // and then we test against the filter list
        jfItemFlagGroup testee = page_results->at(pr_index);
        (search_ptr->categories)->Sort(&testee);
        if (search_ptr->NoCatTest(testee.flags)) testee.flags->included = false;
        if (!(testee.flags->included)) rcount--;
    }
    return rcount;
}

//-----------------------------------------
size_t jfItemsPageParserBase::Condense() {
    // the result list
    std::vector<jfItemFlagGroup>* newlist;
    newlist = new std::vector<jfItemFlagGroup>();
    newlist->reserve(start_itemcount);

    // the loop
    for (size_t pr_index = 0 ; pr_index < start_itemcount ;pr_index++) {
        jfItemFlagGroup testee = page_results->at(pr_index);
        // and then we check
        if (testee.flags->included) newlist->push_back(testee);
        else testee.Dispose();
    }
    // at the end of the loop, we get rid of the old
    page_results->clear();
    delete page_results;
    // and usher in the new
    page_results = newlist;
    return newlist->size();
}
//-----------------------------------------
void jfItemsPageParserBase::PostPageProcessing() {
    const QString fname = "jfItemsPageParserBase::PostPageProcessing";
  assert(page_results != NULL);
  // if the search is null (only during download and parse tests) we skip.
  if (search_ptr == NULL) return;

  // filtering and sorting
  lpt->tLog(fname,1);
  start_itemcount = page_results->size();
  lpt->tLog(fname,2);
  FilterByList();
  lpt->tLog(fname,3);
  CategorySort();
  lpt->tLog(fname,4);
  Condense();
  lpt->tLog(fname,5);
  // done
}

/*****************************************************************************/
