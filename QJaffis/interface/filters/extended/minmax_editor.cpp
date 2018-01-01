/******************************************************************************
Name    :   minmax_editor.cpp
Author  :   John Q Metro
Purpose :   Defines some editors for numeric range filters
Created :   June 8, 2016 (Taken from other sources)
Updated :   June 8, 2016
******************************************************************************/
#ifndef MINMAX_EDITOR_H
  #include "minmax_editor.h"
#endif // MINMAX_EDITOR_H
//-----------------------------------
#include <assert.h>
/*****************************************************************************/

// the default constructor
jfZeroToMaxFilterEditor::jfZeroToMaxFilterEditor(QString typname, size_t xmax,
       const jfFilterMap* infmap, const jfMinMaxUFilter* infilt, QWidget* parent):
  jfBaseFilterEditor(infmap,infilt,parent) {
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
void jfZeroToMaxFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  const jfMinMaxUFilter* xfilt;
  if (infilter!=NULL) {
    xfilt = dynamic_cast<const jfMinMaxUFilter*>(infilter);
    insert_panel->SetMinMax(xfilt->GetMin(),xfilt->GetMax());
  }
}
//-----------------------------------------------------
jfBaseFilter* jfZeroToMaxFilterEditor::GetFilter() {
  jfMinMaxUFilter* result;
  result = MakeTypedMinMax();
  result->SetValues(insert_panel->GetMin(),insert_panel->GetMax());
  namedesc_edit->ChangeObj(result);
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking
bool jfZeroToMaxFilterEditor::GeneralCheck() const {
  return insert_panel->Check();
}
//========================================================================
jfWCFilterEditor::jfWCFilterEditor(const jfFilterMap* infmap, const jfWordCountFilter* infilt,
          QWidget* parent):jfZeroToMaxFilterEditor("Word Count Range",999999999,infmap,infilt,parent) {
  // not much
  insert_panel->SetMinMax(0,0);
}

//--------------------------------------------------------------------
jfMinMaxUFilter* jfWCFilterEditor::MakeTypedMinMax() const {
  return new jfWordCountFilter();
}
/*****************************************************************************/

