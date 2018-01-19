/******************************************************************************
Name    :   fimgroup_panel1.cpp
Author  :   John Q Metro
Purpose :   Defines the custom first panel for fimfiction.net group searching
Created :   August 10, 2013
Conversion to Qt Started August 3, 2015
Updated :   August 4, 2015
******************************************************************************/
#ifndef FIMGROUP_PANEL1_H_INCLUDED
  #include "fimgroup_panel1.h"
#endif // FIMGROUP_PANEL1_H_INCLUDED
//--------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FIMFILTERS2_H_INCLUDED
  #include "../../../core/filters/fim/fimfilters2.h"
#endif // FIMFILTERS2_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../../initend.h"
#endif // INITEND_H_INCLUDED
#ifndef FIM_CONSTANTS_H
  #include "../../data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H

#include <assert.h>

//======================================
//
// the constructor
jfFIMGroup_DFE::jfFIMGroup_DFE(jfFIMGroupSearch* inobj, QWidget* parent):
      jfDefaultFilterEditorBase(inobj,4,1,parent) {
  // story count filter
  sc_filedit = new jfMinMaxEditor("Story Count Filter",true,false);
  // arraging
  sidesizer = new QVBoxLayout();
  sidesizer->addWidget(sc_filedit,0,Qt::AlignTop);
  top_sizer->addLayout(sidesizer,1);
  // loading data
  ChangeSearch(inobj);
  typed_search = inobj;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methds
bool jfFIMGroup_DFE::SaveFiltersExtended() {
  // local variables
  jfFIMGroupSCFilter* storycount_filter;
  size_t oindex;
  // getting data
  storycount_filter = new jfFIMGroupSCFilter();
  storycount_filter->SetValues(sc_filedit->GetMin(),sc_filedit->GetMax());
  storycount_filter->SetName(DEF_fimgstoryc_name);
  embedded_filters->ReplaceSame(storycount_filter,oindex);
  return true;
}
//--------------------------------------------------------------------
bool jfFIMGroup_DFE::ChangeSearchExtended(jfSearchCore* obj_data) {
  // variables
  jfFIMGroupSCFilter* storycount_filter;
  bool atest;
  // the word count filter
  storycount_filter= dynamic_cast<jfFIMGroupSCFilter*>(embedded_filters->GetItem(DEF_fimgstoryc_name));
  if (storycount_filter!=NULL) {
    atest = sc_filedit->SetMinMax(storycount_filter->GetMin(),storycount_filter->GetMax());
    assert(atest);
  }
  return true;
}
//===============================================================================
jfFIMGroup_SearchOptions::jfFIMGroup_SearchOptions(QWidget* parent):jfSearchOptionsBase(parent) {
  CreateWidgets();
  ArrangeWidgets();
  setLayout(main_layout);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// methods
//-----------------------------------------
bool jfFIMGroup_SearchOptions::LoadFrom(jfSearchCore* insearch) {
  jfFIMGroupSearch* tval;
  if (insearch==NULL) return false;
  // getting values
  tval = dynamic_cast<jfFIMGroupSearch*>(insearch);
  search_term_entry->setText(tval->GetSearchString());
  search_result_order->setCurrentIndex(tval->GetOrder());
  // done
  return true;
}

//-----------------------------------------
bool jfFIMGroup_SearchOptions::StoreTo(jfSearchCore* outsearch) {
  jfFIMGroupSearch* tval;
  if (outsearch==NULL) return false;
  tval = dynamic_cast<jfFIMGroupSearch*>(outsearch);
  // gathering the values
  QString sstring = (search_term_entry->text()).trimmed();
  size_t xorder = search_result_order->currentIndex();
  tval->SetSearchString(sstring,xorder);
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//-----------------------------------------
void jfFIMGroup_SearchOptions::CreateWidgets() {
  main_label = new QLabel("Enter the search term :");
  search_term_entry = new QLineEdit();
  result_order_label = new QLabel("Result Order");
  search_result_order = new QComboBox();
  for (size_t cbindex = 0; cbindex < fimcon::group_ordercount; cbindex++) {
      search_result_order->addItem(fimcon::group_orderlabel[cbindex]);
  }
  search_result_order->setCurrentIndex(0);
}
//-----------------------------------------
void jfFIMGroup_SearchOptions::ArrangeWidgets() {
  main_layout = new QGridLayout();
  main_layout->addWidget(main_label,0,0);
  main_layout->addWidget(result_order_label,0,1);
  main_layout->addWidget(search_term_entry,1,0);
  main_layout->addWidget(search_result_order,1,1);
  main_layout->setColumnStretch(0,5);
  main_layout->setColumnStretch(1,1);
}
//===============================================================================
// the constructor
jfFIMGroupPanel1::jfFIMGroupPanel1(jfFIMGroupSearch* insearch, QWidget* parent):
        jfSearchPanelBase(insearch,false,true,1, parent) {
  const QString fname = "jfFIMGroupPanel1::jfFIMGroupPanel1";
  // sizer arranging
  typed_search = insearch;
  filt_panel = MakeTypedFilEdit();
  SearchPanel = new jfFIMGroup_SearchOptions();
  // finishing off
  ArrangePanels();
  ChangeObj(mainobj);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other external methods
bool jfFIMGroupPanel1::LoadToObj() {
  // constants
  // starting
  if (mainobj==NULL) return false;
  filt_panel->SaveFilters();
  SearchPanel->StoreTo(typed_search);
  // done
  return true;
}
//----------------------------------------------------------------------------
bool jfFIMGroupPanel1::ChangeObj(jfSearchCore* obj_data) {
  const QString fnamex = "jfFIMGroupPanel1::ChangeObj";
  // starting...
  typed_search = dynamic_cast<jfFIMGroupSearch*>(obj_data);
  mainobj = obj_data;
  // filters
  filt_panel->ChangeSearch(obj_data);
  SearchPanel->LoadFrom(obj_data);
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfDefaultFilterEditorBase* jfFIMGroupPanel1::MakeTypedFilEdit() {
  jfFIMGroup_DFE* result;
  result = new jfFIMGroup_DFE(typed_search);
  return result;
}
/*****************************************************************************/

