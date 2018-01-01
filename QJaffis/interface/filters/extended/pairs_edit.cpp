//***************************************************************************
// Name:        pairs_edit.cpp
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters editing, various panels.
// Created:     January 2, 2011
// Conversion to Qt Started October 18, 2013
// Updated:     January 2, 2011
//***************************************************************************
#ifndef PAIRS_EDIT_H_INCLUDED
  #include "pairs_edit.h"
#endif // PAIRS_EDIT_H_INCLUDED
//--------------------------------------------------------------------------
#include <assert.h>
//***************************************************************************
// the default constructor
jfPairFS_Editor::jfPairFS_Editor(const jfFilterMap* infmap, const jfPairFilterSingle* infilt,
        QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
  assert(infmap!=NULL);
  // the typed filter
  // we create the insert.. the *actual* filter editor
  insert_panel = new jfPairFilEditPanelSingle(infilt);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfPairFS_Editor::LoadFilter(const jfBaseFilter* infilter) {
  // checks
  assert(infilter!=NULL);
  // variables
  QString omsg;
  const jfPairFilterCore* typed_fil;
  // loading the data
  filt_pointer = infilter;
  typed_fil = dynamic_cast<const jfPairFilterCore*>(infilter);
  insert_panel->LoadFilterData(typed_fil);
  NameLoad();
}
//------------------------------------------------------------------------------
jfBaseFilter* jfPairFS_Editor::GetFilter() {
  jfPairFilterSingle* toutput;
  // creating the result
  toutput = new jfPairFilterSingle();
  // getting the output
  if (insert_panel->SaveFilterData(toutput)) {
    namedesc_edit->ChangeObj(toutput);
    return toutput;
  }
  else return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfPairFS_Editor::GeneralCheck() const {
  // local variables
  bool resval;
  // first we see if the name is correct
  if (NameNUniq()) return false;
  // next, we check the filter
  resval = insert_panel->CheckValid();
  return resval;
}
//=============================================================================
// the default constructor
jfPairFL_Editor::jfPairFL_Editor(const jfFilterMap* infmap, const jfPairFilterList* infilt,
                QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
  assert(infmap!=NULL);
  // the typed filter
  // we create the insert.. the *actual* filter editor
  insert_panel = new jfPairFilEditPanelList(infilt);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfPairFL_Editor::LoadFilter(const jfBaseFilter* infilter) {
  // checks
  assert(infilter!=NULL);
  // variables
  QString omsg;
  const jfPairFilterCore* typed_fil;
  // loading the data
  filt_pointer = infilter;
  typed_fil = dynamic_cast<const jfPairFilterCore*>(infilter);
  insert_panel->LoadFilterData(typed_fil);
  NameLoad();
}
//------------------------------------------------------------------------------
jfBaseFilter* jfPairFL_Editor::GetFilter() {
  jfPairFilterList* toutput;
  // creating the result
  toutput = new jfPairFilterList();
  // getting the output
  if (insert_panel->SaveFilterData(toutput)) {
    namedesc_edit->ChangeObj(toutput);
    return toutput;
  }
  else return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfPairFL_Editor::GeneralCheck() const {
  // local variables
  bool resval;
  // first we see if the name is correct
  if (NameNUniq()) return false;
  // next, we check the filter
  resval = insert_panel->CheckValid();
  return resval;
}
//=============================================================================
// the default constructor
jfPairFM_Editor::jfPairFM_Editor(const jfFilterMap* infmap, const jfPairFilterMultiple* infilt,
        QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
  assert(infmap!=NULL);
  // the typed filter
  // we create the insert.. the *actual* filter editor
  insert_panel = new jfPairFilEditPanelMulti(infilt);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfPairFM_Editor::LoadFilter(const jfBaseFilter* infilter) {
  // checks
  assert(infilter!=NULL);
  // variables
  QString omsg;
  const jfPairFilterCore* typed_fil;
  // loading the data
  filt_pointer = infilter;
  typed_fil = dynamic_cast<const jfPairFilterCore*>(infilter);
  insert_panel->LoadFilterData(typed_fil);
  NameLoad();
}
//------------------------------------------------------------------------------
jfBaseFilter* jfPairFM_Editor::GetFilter() {
  jfPairFilterMultiple* toutput;
  // creating the result
  toutput = new jfPairFilterMultiple();
  // getting the output
  if (insert_panel->SaveFilterData(toutput)) {
    namedesc_edit->ChangeObj(toutput);
    return toutput;
  }
  else return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfPairFM_Editor::GeneralCheck() const {
  // local variables
  bool resval;
  // first we see if the name is correct
  if (NameNUniq()) return false;
  // next, we check the filter
  resval = insert_panel->CheckValid();
  return resval;
}
//***************************************************************************
