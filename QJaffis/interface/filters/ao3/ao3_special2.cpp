/******************************************************************************
Name    :   ao3_special2.cpp
Author  :   John Q Metro
Purpose :   Defines some interface panels for some AO3 Filters
Created :   October 6, 2012
Conversion to Qt Started October 19, 2013
Updated :   September 10, 2020
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
  readerbox = new QCheckBox("Check to match any pairing including 'Reader' or 'You'.");
  readerbox->setChecked(false);
  originalbox = new QCheckBox("Check to match any original character pairing.");
  originalbox->setChecked(false);
  pairlistbox = new jfLabeledEdit(NULL,"Pairings to find (comma separated, use '/' (romantic/sexual) or '&' (other) to separate the names.",(omode)?(false):(true));
  // arranging
  bsizer = new QVBoxLayout();
  bsizer->addWidget(altbox,0);
  bsizer->addWidget(readerbox,0);
  bsizer->addWidget(originalbox,0);
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
void jfAO3PairingPanel::SetReaderCheck(bool cval) {
    readerbox->setChecked(cval);
}

void jfAO3PairingPanel::SetOriginalCheck(bool cval) {
    originalbox->setChecked(cval);
}
//-------------------------------------
QString jfAO3PairingPanel::GetStringValue() const {
  return pairlistbox->GetText();
}
//-------------------------------------
bool jfAO3PairingPanel::GetAlternateCheck() const {
  return altbox->isChecked();
}
bool jfAO3PairingPanel::GetReaderCheck() const {
    return readerbox->isChecked();
}
bool jfAO3PairingPanel::GetOriginalCheck() const {
    return originalbox->isChecked();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// filter based i/o
//-------------------------------------
jfAO3PairFilter* jfAO3PairingPanel::GetFilter() const {
  jfAO3PairFilter* result = new jfAO3PairFilter();
  bool rb = readerbox->isChecked();
  bool ob = originalbox->isChecked();
  bool hn = result->SetNamesData(pairlistbox->GetText());
  if ((!hn) && (!rb) && (!ob)) {
    delete result;
    return NULL;
  }
  result->SetAlternates(altbox->isChecked());
  result->SetMatchReader(rb);
  result->SetMatchOriginal(ob);
  return result;
}
//-------------------------------------
bool jfAO3PairingPanel::StoreToFilter(jfAO3PairFilter* outval) {
  assert(outval!=NULL);
  outval->SetNamesData(pairlistbox->GetText());
  outval->SetAlternates(altbox->isChecked());
  outval->SetMatchReader(readerbox->isChecked());
  outval->SetMatchOriginal(originalbox->isChecked());
  return true;
}
//-------------------------------------
bool jfAO3PairingPanel::LoadFromFilter(const jfAO3PairFilter* inval) {
  assert(inval!=NULL);
  if (!inval->IsValid()) return false;
  pairlistbox->SetText(inval->GetNamesData());
  altbox->setChecked(inval->GetAlternate());
  readerbox->setChecked(inval->GetMatchReader());
  originalbox->setChecked(inval->GetMatchOriginal());
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
  if (result == NULL) return false;
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
