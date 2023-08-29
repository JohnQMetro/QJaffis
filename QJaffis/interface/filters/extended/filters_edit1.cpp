/******************************************************************************
Name    :   filters_edit1.cpp
Author  :   John Q Metro
Purpose :   Editors for more filters : word count
Created :   December 6, 2010
Conversion to Qt started October 13, 2013
Updated :   April 15, 2023
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
jfTagFilterEditor::jfTagFilterEditor(const jfTagFilterCore* infilt, QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // variables and constants

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//----------------------------------------------
void jfTagFilterEditor::LoadFilter(const jfFilterBase* infilter) {
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
jfFilterBase* jfTagFilterEditor::GetFilter() {
  // variables
  jfTagFilterCore* tvalue;
  const jfTagListing* tcopy;

  // creating filters and loading tags
  tvalue = GetTagFilter();
  if (isListLong()) tcopy = longlist_panel->SaveAndGetTagData();
  else tcopy = shortlist_panel->GetTagList();

  tvalue->SetTags(tcopy);
  // name setting
  namedesc_edit->ChangeFilter(tvalue);
  return tvalue;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking
bool jfTagFilterEditor::GeneralCheck(const jfFilterMap* infmap) const {
    if (infmap == NULL) return true;
    else if (NameNUniq(infmap)) return false;
    else return true;
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
    else saveto->SetCompleted( (mainval1->checkState())==Qt::Checked );
    return true;
  }
}
//--------------------------------------------------------------------
bool jfCFE_Core1::LoadValue(const jfCompletedFilter* getfrom) {
    if (getfrom==NULL) return false;
    else {
        jfCompletedState state = getfrom->GetValue();
        if (state == jfCompletedState::EITHER) {
            mainval1->setCheckState(Qt::PartiallyChecked);
        }
        else if (state == jfCompletedState::COMPLETE) {
            mainval1->setCheckState(Qt::Checked);
        }
        else {
            mainval1->setCheckState(Qt::Unchecked);
        }
        return true;
    }
}
//--------------------------------------------------------------------
jfCompletedFilter* jfCFE_Core1::GetValue() const {
  jfCompletedFilter* result = new jfCompletedFilter("(placeholder)");
  bool qval = StoreValue(result);
  assert(qval);
  return result;
}

//*****************************************************************************

jfCompletedFilterEditor::jfCompletedFilterEditor(const jfCompletedFilter* infilt, QWidget* parent):
                jfBaseFilterEditor(infilt, parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfCFE_Core1(infilt);
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
void jfCompletedFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  insert_panel->LoadValue(dynamic_cast<const jfCompletedFilter*>(filt_pointer));
  NameLoad();
}
//-------------------------------------
jfFilterBase* jfCompletedFilterEditor::GetFilter() {
    jfCompletedFilter* result = insert_panel->GetValue();
    namedesc_edit->ChangeFilter(result);
    return result;
}
// -------------------------------
jfCompletedFilter* jfCompletedFilterEditor::GetCompletedFilter(const QString& name) const {
    jfCompletedFilter* result = insert_panel->GetValue();
    namedesc_edit->ChangeFilter(result);
    result->SetName(name);
    return result;
}
//--------------------------------------------------------------
bool jfCompletedFilterEditor::GeneralCheck(const jfFilterMap* infmap) const {
    if (infmap == NULL) return true;
    else if (NameNUniq(infmap)) return false;
    else return true;
}

//******************************************************************************/

