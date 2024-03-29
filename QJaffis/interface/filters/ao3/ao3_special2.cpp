/******************************************************************************
Name    :   ao3_special2.cpp
Author  :   John Q Metro
Purpose :   Defines some interface panels for some AO3 Filters
Created :   October 6, 2012
Conversion to Qt Started October 19, 2013
Updated :   August 12 2023
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

jfAO3_WarnFilterEditor::jfAO3_WarnFilterEditor(const jfAO3WarnFilter* infilt, QWidget* parent):jfBaseFilterEditor(infilt,parent) {
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
void jfAO3_WarnFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  const jfAO3WarnFilter* temp = dynamic_cast<const jfAO3WarnFilter*>(infilter);
  insert_panel->SetFromString(temp->ToString());
  NameLoad();
}
//-------------------------------------
jfFilterBase* jfAO3_WarnFilterEditor::GetFilter() {
  jfAO3WarnFilter* result = new jfAO3WarnFilter(insert_panel->GetStringValue());
  namedesc_edit->ChangeFilter(result);
  return result;
}
//--------------------------------------------------------------
bool jfAO3_WarnFilterEditor::GeneralCheck(const jfFilterMap* infmap) const {
    if (infmap == NULL) return true;
    else if (NameNUniq(infmap)) return false;
    else return true;
}
/*****************************************************************************/
