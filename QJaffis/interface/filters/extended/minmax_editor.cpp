/******************************************************************************
Name    :   minmax_editor.cpp
Author  :   John Q Metro
Purpose :   Defines some editors for numeric range filters
Created :   June 8, 2016 (Taken from other sources)
Updated :   April 15, 2023
******************************************************************************/
#ifndef MINMAX_EDITOR_H
  #include "minmax_editor.h"
#endif // MINMAX_EDITOR_H
//-----------------------------------
#include <assert.h>
/*****************************************************************************/

// the default constructor
jfZeroToMaxFilterEditor::jfZeroToMaxFilterEditor(QString typname, size_t xmax,
            const jfMinMaxUFilter* infilt, QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
  assert(xmax!=0);
  // we create the insert.. the *actual* panel
  insert_panel = new jfZeroToMaxEditor(typname,true,true);
  insert_panel->SetMaxVal(xmax);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
  LoadFilter(infilt);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//---------------------------------------------
void jfZeroToMaxFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  const jfMinMaxUFilter* xfilt;
  if (infilter!=NULL) {
    xfilt = dynamic_cast<const jfMinMaxUFilter*>(infilter);
    insert_panel->SetMinMax(xfilt->GetMin(),xfilt->GetMax());
  }
}
//-----------------------------------------------------
jfFilterBase* jfZeroToMaxFilterEditor::GetFilter() {
  jfMinMaxUFilter* result;
  result = MakeTypedMinMax();
  result->SetValues(insert_panel->GetMin(),insert_panel->GetMax());
  namedesc_edit->ChangeFilter(result);
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking
bool jfZeroToMaxFilterEditor::GeneralCheck(const jfFilterMap *filter_group) const {
    if (filter_group == NULL) return true;
    else if (NameNUniq(filter_group)) return false;
    else return insert_panel->Check();
}
//========================================================================
jfWCFilterEditor::jfWCFilterEditor(const jfWordCountFilter* infilt, QWidget* parent):
        jfZeroToMaxFilterEditor("Word Count Range",999999999,infilt,parent) {
  // not much
  insert_panel->SetMinMax(0,0);
}

//--------------------------------------------------------------------
jfMinMaxUFilter* jfWCFilterEditor::MakeTypedMinMax() const {
  return new jfWordCountFilter("(dummy name");
}
/*****************************************************************************/

