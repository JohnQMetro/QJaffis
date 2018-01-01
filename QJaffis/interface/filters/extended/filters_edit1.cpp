/******************************************************************************
Name    :   filters_edit1.cpp
Author  :   John Q Metro
Purpose :   Editors for more filters : word count
Created :   December 6, 2010
Conversion to Qt started October 13, 2013
Updated :   July 8, 2016 (big reorganization)
******************************************************************************/
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------
#include <QMessageBox>
#include <assert.h>
//******************************************************************************/

// the default constructor
jfTagFilterEditor::jfTagFilterEditor(const jfFilterMap* infmap, const jfTagFilterCore* infilt,
        QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // variables and constants

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//----------------------------------------------
void jfTagFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  // variables and constants
  const jfTagFilterCore* tvalue;
  jfTagListing* tcopy;
  // c hecking and converting the input
  assert(infilter!=NULL);
  filt_pointer = infilter;
  tvalue = dynamic_cast<const jfTagFilterCore*>(infilter);
  tcopy = tvalue->GetTagsCopy();
  if (isListLong()) longlist_panel->ChangeTagData(tcopy,false,false);
  else shortlist_panel->SetOrChangeTags(tcopy);
  NameLoad();
}
//---------------------------------------------
jfBaseFilter* jfTagFilterEditor::GetFilter() {
  // variables
  jfTagFilterCore* tvalue;
  const jfTagListing* tcopy;
  bool testval;
  // creating filters and loading tags
  tvalue = GetTagFilter();
  if (isListLong()) tcopy = longlist_panel->SaveAndGetTagData();
  else tcopy = shortlist_panel->GetTagList();
  testval = tvalue->SetTags(tcopy);
  // name setting
  namedesc_edit->ChangeObj(tvalue);
  return tvalue;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking
bool jfTagFilterEditor::GeneralCheck() const {
  return true;
}
//-----------------------------------------------------
void jfTagFilterEditor::CompleteConstruction(QString name, const jfTagFilterCore* infilt) {
  jfTagListing* tvalue;
  // we create the insert.. the *actual* expression editor
  if (isListLong()) {
    shortlist_panel = NULL;
    longlist_panel = new jfTagSorter(true,true);
  }
  else {
    shortlist_panel = new jfTagStatusPicker(name,true);
    longlist_panel = NULL;
  }
  // checking the inputted filter
  if (infilt!=NULL) {
    assert(infilt->IsValid());
    tvalue = infilt->GetTagsCopy();
    if (isListLong()) longlist_panel->ChangeTagData(tvalue,false,false);
    else shortlist_panel->SetTagList(tvalue);
  }
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  if (isListLong()) insert->addWidget(longlist_panel,1);
  else insert->addWidget(shortlist_panel,1);
  // calling the sizers
  ArrangeSizers();
}

//===========================================================================================
jfCFE_Core1::jfCFE_Core1(const jfCompletedFilter* inmap, QWidget* parent):QWidget(parent) {
  // we start
  mainval1 = new QCheckBox("Completed Stories (check to include, blank to exclude)");
  mainval1->setTristate();
  mainsizer = new QHBoxLayout();
  mainsizer->addWidget(mainval1,0);
  setLayout(mainsizer);
  // setting the value
  if (inmap==NULL) mainval1->setCheckState(Qt::PartiallyChecked);
  else LoadValue(inmap);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting and getting values
//--------------------------------------------------------------------
bool jfCFE_Core1::StoreValue(jfCompletedFilter* saveto) const {
  if (saveto==NULL) return false;
  else {
    if ((mainval1->checkState())==Qt::PartiallyChecked) saveto->SetEmpty();
    else saveto->SetValue((mainval1->checkState())==Qt::Checked);
    return true;
  }
}
//--------------------------------------------------------------------
bool jfCFE_Core1::LoadValue(const jfCompletedFilter* getfrom) {
  if (getfrom==NULL) return false;
  else {
    if (getfrom->isEmpty()) mainval1->setCheckState(Qt::PartiallyChecked);
    else {
      mainval1->setCheckState((getfrom->GetValue())?(Qt::Checked):(Qt::Unchecked));
    }
    return true;
  }
}
//--------------------------------------------------------------------
jfCompletedFilter* jfCFE_Core1::GetValue() const {
  jfCompletedFilter* result = new jfCompletedFilter();
  bool qval = StoreValue(result);
  assert(qval);
  return result;
}
//=============================================================================
jfComplFiltEdit::jfComplFiltEdit(const jfCompletedFilter* inmap, QWidget* parent):jfCFE_Core1(inmap,parent) {}
//*****************************************************************************
jfCompletedFilterEditor::jfCompletedFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap,
                               QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfComplFiltEdit(dynamic_cast<const jfCompletedFilter*>(infilt));
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,0);
  insert->addStretch(1);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfCompletedFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  insert_panel->LoadValue(dynamic_cast<const jfCompletedFilter*>(filt_pointer));
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfCompletedFilterEditor::GetFilter() {
  jfCompletedFilter* result = insert_panel->GetValue();
  namedesc_edit->ChangeObj(result);
  return result;
}
//--------------------------------------------------------------
bool jfCompletedFilterEditor::GeneralCheck() const {
  return true;
}

//******************************************************************************/

