/******************************************************************************
Name    :   ao3_special2.cpp
Author  :   John Q Metro
Purpose :   Defines some interface panels for some AO3 Filters
Created :   October 6, 2012
Conversion to Qt Started October 19, 2013
Updated :   September 6, 2020
******************************************************************************/
#ifndef AO3_SPECIAL2_H_INCLUDED
  #include "ao3_special2.h"
#endif // AO3_SPECIAL2_H_INCLUDED
#ifndef AO3_SPECIALS2_H_INCLUDED
  #include "../../../core/filters/ao3/ao3_specials2.h"
#endif // AO3_SPECIALS2_H_INCLUDED
//--------------------------------------
#include <assert.h>
/*****************************************************************************/
jfAO3PairingPanel::jfAO3PairingPanel(bool orient, QWidget* parent):QWidget(parent) {
  // starting...
  omode = orient;
  // creating GUI elemnts
  altbox = new QCheckBox("Check if only one pairing needs to match");
  altbox->setChecked(true);
  pairlistbox = new jfLabeledEdit(NULL,"Pairings to find (comma separated, use '/' (romantic/sexual) or '&' (other) to separate the names.",(omode)?(false):(true));
  // arranging
  bsizer = new QVBoxLayout();
  bsizer->addWidget(altbox,0);
  bsizer->addWidget(pairlistbox,0);
  // finishing
  setLayout(bsizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o
//-------------------------------------
bool jfAO3PairingPanel::SetFromString(const QString& inval) {
  pairlistbox->SetText(inval);
  return true;
}
//-------------------------------------
void jfAO3PairingPanel::SetAlternateCheck(bool cval) {
  altbox->setChecked(cval);
}
//-------------------------------------
QString jfAO3PairingPanel::GetStringValue() const {
  return pairlistbox->GetText();
}
//-------------------------------------
bool jfAO3PairingPanel::GetAlternateCheck() const {
  return altbox->isChecked();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// filter based i/o
//-------------------------------------
jfAO3PairFilter* jfAO3PairingPanel::GetFilter() const {
  jfAO3PairFilter* result;
  result = new jfAO3PairFilter();
  if (!result->SetNamesData(pairlistbox->GetText())) {
    delete result;
    return NULL;
  }
  result->SetAlternates(altbox->isChecked());
  return result;
}
//-------------------------------------
bool jfAO3PairingPanel::StoreToFilter(jfAO3PairFilter* outval) {
  assert(outval!=NULL);
  if (!outval->SetNamesData(pairlistbox->GetText())) return false;
  outval->SetAlternates(altbox->isChecked());
  return true;
}
//-------------------------------------
bool jfAO3PairingPanel::LoadFromFilter(const jfAO3PairFilter* inval) {
  assert(inval!=NULL);
  if (!inval->IsValid()) return false;
  pairlistbox->SetText(inval->GetNamesData());
  altbox->setChecked(inval->GetAlternate());
  return true;
}
//=============================================================================
jfAO3_PairingFilterEditor::jfAO3_PairingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap,
               QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfAO3PairingPanel(true);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
  if (infilt!=NULL) LoadFilter(infilt);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfAO3_PairingFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  const jfAO3PairFilter* temp = dynamic_cast<const jfAO3PairFilter*>(infilter);
  insert_panel->LoadFromFilter(temp);
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfAO3_PairingFilterEditor::GetFilter() {
  jfAO3PairFilter* result = insert_panel->GetFilter();
  namedesc_edit->ChangeObj(result);
  return result;
}
//--------------------------------------------------------------
bool jfAO3_PairingFilterEditor::GeneralCheck() const {
  jfAO3PairFilter* result = insert_panel->GetFilter();
  bool resval = result->IsValid();
  delete result;
  return resval;
}
//=============================================================================
jfAO3_WarnFilterEditor::jfAO3_WarnFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap,
     QWidget* parent): jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfCharCheckBoxGroup("Warnings to Exclude",5,warnlist,warn3_ac,3);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
  if (infilt!=NULL) LoadFilter(infilt);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfAO3_WarnFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  const jfAO3WarnFilter* temp = dynamic_cast<const jfAO3WarnFilter*>(infilter);
  insert_panel->SetFromString(temp->ToString());
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfAO3_WarnFilterEditor::GetFilter() {
  jfAO3WarnFilter* result = new jfAO3WarnFilter(insert_panel->GetStringValue());
  namedesc_edit->ChangeObj(result);
  return result;
}
//--------------------------------------------------------------
bool jfAO3_WarnFilterEditor::GeneralCheck() const {
  return true;
}
/*****************************************************************************/
