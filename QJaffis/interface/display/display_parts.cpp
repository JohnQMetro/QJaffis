/******************************************************************************
Name    :   display_parts.cpp
Author  :   John Q Metro
Purpose :   Defines core parts and base classes for the search/filter interface
Created :   August 23, 2009 (originally from display_base.cpp)
Updated :   November 22, 2013 (Qt and redistrib)
******************************************************************************/
#ifndef DISPLAY_PARTS_H
  #include "display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-------------------------------
#include <assert.h>

//****************************************************************************
jfDefaultFilterEditorBase::jfDefaultFilterEditorBase(jfSearchCore* inobj, size_t proph,
              size_t propv, QWidget* parent):QWidget(parent) {
  // constants
  const QString wxmain = "Main Filter :";
  // some sanity checks
  assert(inobj!=NULL);
  mainobj = inobj;
  // creating the expression filter
  mainlabel     = new QLabel(wxmain);
  main_expedit  = new jfExpressionEditor(mainobj->local_map,true,true,false);
  // arraning in a sizer...
  exp_sizer = new QVBoxLayout();
  exp_sizer->addWidget(mainlabel,0);
  exp_sizer->addWidget(main_expedit,2);
  // this sizer goes into two nested sizers
  top_sizer = new QHBoxLayout();
  top_sizer->addLayout(exp_sizer,proph);
  main_sizer = new QVBoxLayout();
  main_sizer->addLayout(top_sizer,propv);
  // done
  setLayout(main_sizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data i/o
//------------------------------
bool jfDefaultFilterEditorBase::SaveFilters() {
  const QString fname = "jfDefaultFilterEditorBase::SaveFilters";
  // local variables
  jfExpressionFilter *temp1;
  QString omsg;
  size_t oindex;
  bool zres = false;
  // the main expression filter
  temp1 = main_expedit->CheckFilter(omsg);
  if (temp1==NULL) {
    return false;
  }
  temp1->SetName(defexprname);
  embedded_filters->ReplaceSame(temp1,oindex);
  // calling the virtual method for the rest
  zres = SaveFiltersExtended();
  return zres;
}
//-------------------------------
bool jfDefaultFilterEditorBase::ChangeSearch(jfSearchCore* obj_data) {
  // constants
  const QString fname = "jfDefaultFilterEditorBase::ChangeSearch";
  // local variables
  jfExpressionFilter *ftemp1 = NULL;
  QString omsg;
  // testing...
  /**/JDEBUGLOG(fname,1);
  if (obj_data==NULL) return false;
  // assigning internal links
  mainobj = obj_data;
  embedded_filters = mainobj->def_filtermap;
  /**/JDEBUGLOG(fname,2);
  // setting the data
  ftemp1 = dynamic_cast<jfExpressionFilter*>(embedded_filters->GetItem(defexprname));
  /**/JDEBUGLOG(fname,3);
  if (ftemp1!=NULL) {
      /**/JDEBUGLOG(fname,4);
    ftemp1->SetFiltermapLink(obj_data->local_map);
    /**/JDEBUGLOG(fname,5);
    if (!(main_expedit->SetData(ftemp1->ToString(),omsg))) return false;
  }
  /**/JDEBUGLOG(fname,6);
  // calling the virtual method for the rest
  return ChangeSearchExtended(obj_data);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper methods
//============================================================================
jfSearchOptionsBase::jfSearchOptionsBase(QWidget* parent):QWidget(parent) {

}
//=============================================================================
jfSearchPanelRoot::jfSearchPanelRoot(jfSearchCore* obj_data, QWidget* parent):QWidget(parent) {
  // starting...
  assert(obj_data!=NULL);
  mainobj = obj_data;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfSearchPanelRoot::ReloadObj() {
  bool cores = ChangeObj(mainobj);
  assert(cores);
}
//=============================================================================
jfSearchPanelBase::jfSearchPanelBase(jfSearchCore* obj_data,bool noteb, bool svert, size_t prop, QWidget* parent):jfSearchPanelRoot(obj_data,parent) {
  // starting...
  propstore = prop;
  // notebook configuration
  if (noteb) {
    nb = new QTabWidget();
    isnotebook = true;
    param_sizer = NULL;
    defil_sizer = NULL;
    param_box = NULL;
    defil_box = NULL;
  }
  // sizer configuration
  else {
    isnotebook = false;
    nb = NULL;
    // sizer creation
    param_sizer = new QVBoxLayout();
    param_box = new QGroupBox("Search Parameters");
    param_box->setLayout(param_sizer);
    defil_sizer = new QVBoxLayout();
    defil_box = new QGroupBox("Included Filters");
    defil_box->setLayout(defil_sizer);
  }
  topsizer = new QBoxLayout((svert)?(QBoxLayout::TopToBottom):(QBoxLayout::LeftToRight));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* handles the task of arranging the top level panels, should be called at the
end of construction for a child class */
bool jfSearchPanelBase::ArrangePanels() {
  // checks
  if (filt_panel==NULL) return false;
  if (SearchPanel==NULL) return false;
  // notebook arranging
  if (isnotebook) {
    nb->addTab(SearchPanel,"Search Parameters");
    nb->addTab(filt_panel,"Included Filters");
    nb->setCurrentIndex(0);
    topsizer->addWidget(nb,1);
  }
  // sizer arranging
  else {
    defil_sizer->addWidget(filt_panel,1);
    param_sizer->addWidget(SearchPanel,0);
    topsizer->addWidget(param_box,propstore);
    topsizer->addWidget(defil_box,10-propstore);
  }
  // done
  setLayout(topsizer);
  return true;
}
//=============================================================================
jfSearchGroupingCore::jfSearchGroupingCore(QWidget* parent):QWidget(parent) {
  searchdata = NULL;
  search_and_filter_panel = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchCore* jfSearchGroupingCore::GetSearchObj() {
  return searchdata;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// causes panel1 to be updated from the search
void jfSearchGroupingCore::ReloadPanel1() {
  search_and_filter_panel->ReloadObj();
}
