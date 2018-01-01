/******************************************************************************
Name    :   search_panels.cpp
Author  :   John Q Metro
Purpose :   Defines the search interface classes, assembled from display_base.cpp
            and display_base1.cpp for the Qt version
Created :   Originally Septmber 2009, new Qt version, November 23, 2013
Updated :   August 1, 2016
******************************************************************************/
#ifndef SEARCH_PANELS_H
  #include "search_panels.h"
#endif // SEARCH_PANELS_H
//-------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-------------------------------
#include <assert.h>
/*****************************************************************************/
jfOnePanelSearch::jfOnePanelSearch(QWidget* parent):jfSearchGroupingCore(parent) {
  custinit = false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the puropse of this method is to update the search data with GUI elements
bool jfOnePanelSearch::UpdateSearchData() {
  return panel1->LoadToObj();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfOnePanelSearch::FinishConstruction() {
  const QString fname = "jfOnePanelSearch::FinishConstruction";
  assert(custinit);
  // the notebook
  // top level arranging
  top_sizer = new QVBoxLayout();
  top_sizer->addWidget(panel1,1);
  // finishing
  setLayout(top_sizer);
}
//==========================================================================
jfSearchGrouping::jfSearchGrouping(const QStringList* infclist, QWidget* parent):jfSearchGroupingCore(parent) {
  custinit = false;
  assert(infclist!=NULL);
  fclist = infclist;
  panel_holder = new QTabWidget();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the puropse of this method is to update the search data with GUI elements
bool jfSearchGrouping::UpdateSearchData() {
  const QString fname = "jfSearchGrouping::UpdateSearchData";
  if (searchdata!=NULL) searchdata->SetDescription(desc_entry->text());
  if (!(panel1->LoadToObj())) return false;
  panel2->CoreSave();
  (searchdata->categories)->base_outputdir = dirpicker->GetPath();
  return panel3->SaveCategories();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSearchGrouping::NameChangeForCategory(const QString& newname_in) {
  return panel3->ChangeSearchName(newname_in);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slots
//------------------------------------------------------------------------
void jfSearchGrouping::HandleNewDirectory() {
  searchdata->categories->base_outputdir = dirpicker->GetPath();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfSearchGrouping::FinishConstruction() {
  const QString fname = "jfSearchGrouping::FinishConstruction";
  assert(custinit);
  ninit = true;
  // panel 2: local filters, either new or from searchin
  if (searchdata->local_map == NULL) searchdata->local_map = new jfFilterMap();
  panel2 = new jfFilterMapEditor(searchdata->local_map,false,fclist);
  // panel 3: categories
  panel3 = new jfMainCategoriesEditor(searchdata->categories);
  // the directory picker
  dirpicker = new jfDirPicker(NULL,false,false,"Output Directory",(searchdata->categories)->base_outputdir);
  // directory entry
  desc_label = new QLabel("Search Description : ");
  desc_entry = new QLineEdit();
  top_bar = new QHBoxLayout();
  top_bar->addWidget(dirpicker,4);
  top_bar->addStretch(1);
  top_bar->addWidget(desc_label,0);
  top_bar->addWidget(desc_entry,3);

  // the notebook
  panel_holder->addTab(panel1,"Main");
  panel_holder->addTab(panel2,"Filters");
  panel_holder->addTab(panel3,"Categories");
  // top level arranging
  top_sizer = new QVBoxLayout();
  top_sizer->addLayout(top_bar,0);
  // top_sizer->addWidget(dirpicker,0);
  top_sizer->addWidget(panel_holder,1);
  // finishing
  setLayout(top_sizer);
  ninit = false;
  if (searchdata!=NULL) desc_entry->setText(searchdata->GetDescription());

  connect(dirpicker, SIGNAL(SendPathChanged()), this, SLOT(HandleNewDirectory()));
}
//==========================================================================
jfPanelTwo::jfPanelTwo(jfSearchCore* onsearch, const QStringList* infclist, QWidget* parent):QWidget(parent) {
  assert(infclist!=NULL);
  assert(onsearch!=NULL);
  fclist = infclist;
  searchdata_link = onsearch;
  panel_holder = new QTabWidget();
  FinishConstruction();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchCore* jfPanelTwo::GetSearchObj() {
  return searchdata_link;
}
//------------------------------------------------------------------------
// the puropse of this method is to update the search data with GUI elements
bool jfPanelTwo::UpdateSearchData() {
  const QString fname = "jfPanelTwo::UpdateSearchData";
  if (searchdata_link!=NULL) searchdata_link->SetDescription( desc_entry->text() );
  panel2->CoreSave();
  (searchdata_link->categories)->base_outputdir = dirpicker->GetPath();
  return panel3->SaveCategories();
}
//----------------------------------------------------------------
//extra methods
bool jfPanelTwo::NameChangeForCategory(const QString& newname_in) {
  return panel3->ChangeSearchName(newname_in);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slots
//------------------------------------------------------------------------
void jfPanelTwo::HandleNewDirectory() {
  searchdata_link->categories->base_outputdir = dirpicker->GetPath();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfPanelTwo::FinishConstruction() {
  const QString fname = "jfPanelTwo::FinishConstruction";
  ninit = true;
  // panel 2: local filters, either new or from searchin
  if (searchdata_link->local_map == NULL) searchdata_link->local_map = new jfFilterMap();
  panel2 = new jfFilterMapEditor(searchdata_link->local_map,false,fclist);
  // panel 3: categories
  panel3 = new jfMainCategoriesEditor(searchdata_link->categories);
  // the name editor
  // nam_desc_e = new jfNameDescEditor(NULL,false,false);
  // the directory picker
  dirpicker = new jfDirPicker(NULL,false,false,"Output Directory",(searchdata_link->categories)->base_outputdir);
  // description entry
  desc_label = new QLabel("Search Description : ");
  desc_entry = new QLineEdit();
  // arranging the directory picker and description
  top_bar = new QHBoxLayout();
  top_bar->addWidget(dirpicker,4);
  top_bar->addStretch(1);
  top_bar->addWidget(desc_label,0);
  top_bar->addWidget(desc_entry,3);

  // the notebook
  panel_holder->addTab(panel2,"Filters");
  panel_holder->addTab(panel3,"Categories");
  // top level arranging
  top_sizer = new QVBoxLayout();
  top_sizer->addLayout(top_bar,0);
  top_sizer->addWidget(panel_holder,1);
  // finishing
  setLayout(top_sizer);
  ninit = false;
  if (searchdata_link!= NULL) desc_entry->setText(searchdata_link->GetDescription());

  connect(dirpicker, SIGNAL(SendPathChanged()), this, SLOT(HandleNewDirectory()));
}
/*****************************************************************************/
