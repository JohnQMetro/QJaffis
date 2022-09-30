/*****************************************************************************
Name:        typed_display.cpp
Author :     John Q Metro
Purpose :    Typed HTML Listbox results
Created:     April 27, 2011
Conversion to Qt started, November 2013
Updated:     June 4, 2022
******************************************************************************/
#ifndef TYPED_DISPLAY_H_INCLUDED
  #include "typed_display.h"
#endif // TYPED_DISPLAY_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

//-------------------------------------------
#include <assert.h>
/*****************************************************************************/
jfHtmlResultCollectionBasic::jfHtmlResultCollectionBasic(header_enum inhtype, QWidget* parent):jfHtmlResultCollection(inhtype,parent) {}
//++++++++++++++++++++++++++++++++++++++
void jfHtmlResultCollectionBasic::HandleNewResults(jfResultUnitVector* inresults) {
  if (inresults==NULL) return;
  assert(tpcount!=0);
  // the two choices
  if (tpcount==1) HandleResultsOnePanel(inresults);
  else HandleResultsMultiPanel(inresults);
  // finishing
  UpdateResultsLabel();
  inresults->WakeAfter();
}
//---------------------------------------
void jfHtmlResultCollectionBasic::HandleIncreaseCount(size_t nitem_count) {
  const QString fname = "jfHtmlResultCollectionBasic::HandleIncreaseCount";
  mgroup->AddToTotal(nitem_count);
  if ((hype==he_ALWAYS) || (hype==he_NONEMPTY)) {
    for (size_t rcidx=0;rcidx<num_rcats;rcidx++) {
      panels[rcidx]->UpdateCatHeader(mgroup->GetCurrentCatHtml(rcidx));
    }
    if (use_default) {
      panels[32]->UpdateCatHeader(mgroup->GetCurrentCatHtml(32));
    }
  }
}
//---------------------------------------
void jfHtmlResultCollectionBasic::HandleNewCategory(struct jfCategoryInfoMsg incat) {
  const QString fname = "jfHtmlResultCollectionBasic::HandleNewCategory";
  size_t rcidx;
  mgroup->NewCategory(incat);
  for (rcidx=0;rcidx<num_rcats;rcidx++) {
    panels[rcidx]->SetNoCatHeader();
  }
  if (use_default) {
    panels[32]->SetNoCatHeader();
  }
  if (hype==he_ALWAYS) {
    for (rcidx=0;rcidx<num_rcats;rcidx++) {
      panels[rcidx]->AppendCatHeader(mgroup->GetCurrentCatHtml(rcidx));
    }
    if (use_default) {
      panels[32]->AppendCatHeader(mgroup->GetCurrentCatHtml(32));
    }
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* If there is only one panel, there is no need to check the results for which
 * category they belong to. jfHtmlListView now has a special method for adding
 * items in that case. It seems clearer to setup a separate method to use it. */
void jfHtmlResultCollectionBasic::HandleResultsOnePanel(jfResultUnitVector* inresults) {
  const QString fname = "%%% jfHtmlResultCollectionBasic::HandleResultsOnePanel";
  // some basic checks
  assert(tpcount==1);
  if (inresults==NULL) return;
  // variables
  size_t addcount;
  QString headhtml;
  // The panel index, with only one panel, there are only two options.
  size_t pindex = (use_default)?(32):(0);
  // do we add a header?
  bool add_header = ((hype==he_NONEMPTY) && (mgroup->NoCurrentResults(pindex)));
  // adding items
  numitems += mgroup->AddResults(inresults);
  headhtml = mgroup->GetCurrentCatHtml(pindex);
  // adding header and items
  if (add_header) addcount = panels[pindex]->AppendItemsSimple(inresults,true,headhtml);
  // here, we add the items and update the header
  else {
    addcount = panels[pindex]->AppendItemsSimple(inresults,false);
    panels[pindex]->UpdateCatHeader(headhtml);
  }
}
//------------------------------------------------------------
/* the generic multiple panel case is moved to here (and cleaned up) */
void jfHtmlResultCollectionBasic::HandleResultsMultiPanel(jfResultUnitVector* inresults) {
  const QString fname = "%%% jfHtmlResultCollectionBasic::HandleResultsMultiPanel";
  // varibales
  size_t rcidx;
  size_t addcount;
  QString headhtml;
  bool add_header[33];
  // bad case below
  assert(inresults!=NULL);
  assert(tpcount>1);
  // checking to see if we might need to add headers
  for (rcidx=0;rcidx<33;rcidx++) {
    add_header[rcidx] = ((hype==he_NONEMPTY) && (mgroup->NoCurrentResults(rcidx)));
  }
  // adding to the storage unit
  numitems += mgroup->AddResults(inresults);
  // looping at result category level
  for (rcidx=0;rcidx<num_rcats;rcidx++) {
    headhtml = mgroup->GetCurrentCatHtml(rcidx);
    if (add_header[rcidx]) addcount = panels[rcidx]->AppendItems(inresults,true,headhtml);
    else {
      addcount = panels[rcidx]->AppendItems(inresults,false);
      panels[rcidx]->UpdateCatHeader(headhtml);
    }
  }
  // finally, if we use default
  if (use_default) {
    headhtml = mgroup->GetCurrentCatHtml(32);
    if (add_header[32]) addcount = panels[32]->AppendItems(inresults,true,headhtml);
    else {
      addcount = panels[32]->AppendItems(inresults,false);
      panels[32]->UpdateCatHeader(headhtml);
    }
  }
}

//***********************************************************************************
