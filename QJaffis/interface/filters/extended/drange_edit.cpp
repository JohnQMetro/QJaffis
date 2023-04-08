/******************************************************************************
Name    :   drange_edit.cpp
Author  :   John Q Metro
Purpose :   Defines some classes for editing date range filters
Created :   December 8, 2011
Conversion to Qt started October 13, 2013
Updated :   February 15, 2014
******************************************************************************/
#ifndef DRANGE_EDIT_H_INCLUDED
  #include "drange_edit.h"
#endif // DRANGE_EDIT_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------------------------------------------
#include <QDate>
#include <QMessageBox>
#include <assert.h>
/******************************************************************************/
// please note that the first set if for newer dates, the second for older dates
jfDateRangePanel::jfDateRangePanel(QWidget* parent):QWidget(parent) {
  const QString fname = "jfDateRangePanel::jfDateRangePanel";
  // making the labels first
  from_label = new QLabel("From :");
  to_label = new QLabel("Back to :");
  // making the checkboxes
  undefined_new = new QCheckBox("Undefined");
  undefined_old = new QCheckBox("Undefined");
  // making the other gui elements
  MakeBlock(true);
  MakeBlock(false);
  // TOP LEVEL SIZING !!
  topsizer = new QGridLayout();
  topsizer->addLayout(fsizer,0,0);
  topsizer->addLayout(tsizer,0,1);
  topsizer->addWidget(undefined_new,1,0);
  topsizer->addWidget(undefined_old,1,1);
  // done
  connect(undefined_new, SIGNAL(stateChanged(int)), this, SLOT(EvUndefinedFrom(int)));
  connect(undefined_old, SIGNAL(stateChanged(int)), this, SLOT(EvUndefinedTo(int)));
  setLayout(topsizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting and getting values
//---------------------------------
bool jfDateRangePanel::CheckOk() const {
  // undefined is always okay
  if ((undefined_new->isChecked()) || (undefined_old->isChecked())) return true;
  // otherwise, we check the dates
  QDate newdatex, oldatex;
  newdatex = newdate->date();
  oldatex = oldate->date();
  if (oldatex<=newdatex) return true;
  else return false;
}
//---------------------------------
bool jfDateRangePanel::StoreValue(jfDateRangeFilter* saveto) const {
  // returning false...
  if (saveto==NULL) return false;
  if (!CheckOk()) return false;
  // extracting the values
  QDate newdatex, olddatex;
  if (undefined_new->isChecked()) newdatex = QDate();
  else newdatex = newdate->date();
  if (undefined_old->isChecked()) olddatex = QDate();
  else olddatex = oldate->date();
  // entering the valuies
  saveto->SetDates(newdatex,olddatex);
  return true;
}
//---------------------------------
bool jfDateRangePanel::LoadValue(const jfDateRangeFilter* getfrom) {
  // faliure options
  if (getfrom==NULL) return false;
  if (!getfrom->IsValid()) return false;
  // getting the from date
  QDate dvalue = getfrom->GetValue(true);
  if (!dvalue.isValid()) {
    undefined_new->setChecked(true);
    newdate->setEnabled(false);
  }
  else {
    newdate->setEnabled(true);
    undefined_new->setChecked(false);
    newdate->setDate(dvalue);
  }
  // getting the to date
  dvalue = getfrom->GetValue(false);
  if (!dvalue.isValid()) {
    undefined_old->setChecked(true);
    oldate->setEnabled(false);
  }
  else {
    oldate->setEnabled(true);
    undefined_old->setChecked(false);
    oldate->setDate(dvalue);
  }
  // done
  return true;
}
//---------------------------------
void jfDateRangePanel::MakeUndefined() {
  undefined_new->setChecked(true);
  newdate->setEnabled(false);
  undefined_old->setChecked(true);
  oldate->setEnabled(false);
}
//---------------------------------
void jfDateRangePanel::ShowErrorMsg() const {
  // constants and variables
  const QString errtitle = "Date Error";
  const QString errmsg = "The \'From\' date must not be older than the \'Back to\' date!";
  QMessageBox emsg;
  // doing
  emsg.setIcon(QMessageBox::Critical);
  emsg.setText(errmsg);
  emsg.setWindowTitle(errtitle);
  emsg.exec();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// events
//---------------------------------
void jfDateRangePanel::EvUndefinedFrom(int newstate) {
  newdate->setEnabled(!undefined_new->isChecked());
}
//---------------------------------
void jfDateRangePanel::EvUndefinedTo(int newstate) {
  oldate->setEnabled(!undefined_old->isChecked());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// private methods
bool jfDateRangePanel::MakeBlock(bool from) {
  // variables
  QHBoxLayout* thesizer;
  QDateEdit*  thedatepicker;
  QDate mindate = QDate(1969,1,1);
  QDate startdate = QDate(2050,1,1);
  // doing it
  thesizer = new QHBoxLayout();
  thedatepicker = new QDateEdit();
  // adding to the sizer
  thesizer->addWidget((from)?(from_label):(to_label),0);
  thesizer->addWidget(thedatepicker,1);
  // setting the values
  if (from) {
    newdate = thedatepicker;
    fsizer = thesizer;
    newdate->setMinimumDate(mindate);
    newdate->setDate(startdate);
  }
  else {
    oldate = thedatepicker;
    tsizer = thesizer;
    oldate->setMinimumDate(mindate);
    oldate->setDate(mindate);
  }
  return true;
}

//*******************************************************************************
// the default constructor
jfUpdatedDateFilterEditor::jfUpdatedDateFilterEditor(const jfUpdatedDateFilter* infilt,
    const jfFilterMap* infmap, QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // starting...
  insert_panel = new jfDateRangePanel();
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,0);
  insert->addStretch(1);
  insert_panel->LoadValue(dynamic_cast<const jfDateRangeFilter*>(filt_pointer));
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//---------------------------------------------------
void jfUpdatedDateFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  insert_panel->LoadValue(dynamic_cast<const jfUpdatedDateFilter*>(filt_pointer));
  NameLoad();
}
//---------------------------------------------------
jfBaseFilter* jfUpdatedDateFilterEditor::GetFilter() {
  jfUpdatedDateFilter* result = new jfUpdatedDateFilter();
  insert_panel->StoreValue(result);
  namedesc_edit->ChangeObj(result);
  return result;
}
//---------------------------------------------------
bool jfUpdatedDateFilterEditor::GeneralCheck() const {
  bool rval = insert_panel->CheckOk();
  if (!rval) insert_panel->ShowErrorMsg();
  return rval;
}
//======================================================================
// the default constructor
jfPublishedDateFilterEditor::jfPublishedDateFilterEditor(const jfPublishedDateFilter* infilt,
    const jfFilterMap* infmap, QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // starting...
  insert_panel = new jfDateRangePanel();
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,0);
  insert->addStretch(1);
  insert_panel->LoadValue(dynamic_cast<const jfDateRangeFilter*>(filt_pointer));
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//---------------------------------------------------
void jfPublishedDateFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  insert_panel->LoadValue(dynamic_cast<const jfPublishedDateFilter*>(filt_pointer));
  NameLoad();
}
//---------------------------------------------------
jfBaseFilter* jfPublishedDateFilterEditor::GetFilter() {
  jfPublishedDateFilter* result = new jfPublishedDateFilter();
  insert_panel->StoreValue(result);
  namedesc_edit->ChangeObj(result);
  return result;
}
//---------------------------------------------------
bool jfPublishedDateFilterEditor::GeneralCheck() const {
  bool rval = insert_panel->CheckOk();
  if (!rval) insert_panel->ShowErrorMsg();
  return rval;
}

/******************************************************************************/
