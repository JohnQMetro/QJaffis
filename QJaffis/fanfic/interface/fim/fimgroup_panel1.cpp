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
  connect(disable_first, SIGNAL(stateChanged(int)), this, SLOT(HandleCheck(int)));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// methods
//-----------------------------------------
bool jfFIMGroup_SearchOptions::LoadFrom(jfSearchCore* insearch) {
  jfFIMGroupSearch* tval;
  if (insearch==NULL) return false;
  // getting values
  tval = dynamic_cast<jfFIMGroupSearch*>(insearch);
  size_t min1, max1, min2, max2;
  min1 = tval->GetMinGroup(true);
  min2 = tval->GetMinGroup(false);
  max1 = tval->GetMaxGroup(true);
  max2 = tval->GetMaxGroup(false);
  // setting the gui values
  if ((min1==0) && (max1==0)) {
    range_edit1->setEnabled(false);
    disable_first->setChecked(false);
  }
  else range_edit1->SetMinMax(min1,max1);
  range_edit2->SetMinMax(min2,max2);
  // done
  return true;
}

//-----------------------------------------
bool jfFIMGroup_SearchOptions::StoreTo(jfSearchCore* outsearch) {
  jfFIMGroupSearch* tval;
  if (outsearch==NULL) return false;
  tval = dynamic_cast<jfFIMGroupSearch*>(outsearch);
  // gathering the values
  size_t min1, max1, min2, max2;
  if (disable_first->isChecked()) min1 = max1 = 0;
  else {
    min1 = range_edit1->GetMin();
    max1 = range_edit1->GetMax();
  }
  min2 = range_edit2->GetMin();
  max2 = range_edit2->GetMax();
  // checking
  if (min2<=max1) return false;
  // storing
  tval->SetLimits(true,min1,max1);
  tval->SetLimits(false,min2,max2);
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMGroup_SearchOptions::HandleCheck(int newval) {
  if (newval==Qt::Checked) range_edit1->setEnabled(false);
  else if (newval==Qt::Unchecked) range_edit1->setEnabled(true);
  else assert(false);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//-----------------------------------------
void jfFIMGroup_SearchOptions::CreateWidgets() {
  range_edit1 = new jfGenMinMaxEditor("First Group Range",true,true);
  range_edit1->SetValueRange(jglobal::settings.fimlimits[1],jglobal::settings.fimlimits[2]);
  range_edit2 = new jfGenMinMaxEditor("Second Group Range",true,true);
  range_edit2->SetValueRange(jglobal::settings.fimlimits[3],jglobal::settings.fimlimits[4]);
  disable_first = new QCheckBox("No First Range");
  main_range_label = new QLabel("Pick the range of Group ID's to Search :");
}

//-----------------------------------------
void jfFIMGroup_SearchOptions::ArrangeWidgets() {
  main_layout = new QGridLayout();
  main_layout->addWidget(main_range_label,0,0,1,2,Qt::AlignHCenter);
  main_layout->addWidget(range_edit1,1,0);
  main_layout->addWidget(range_edit2,1,1);
  main_layout->addWidget(disable_first,2,0);
  main_layout->setColumnStretch(0,2);
  main_layout->setColumnStretch(1,2);
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

