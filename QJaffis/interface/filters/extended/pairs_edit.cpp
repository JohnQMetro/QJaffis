//***************************************************************************
// Name:        pairs_edit.cpp
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters editing, various panels.
// Created:     January 2, 2011
// Conversion to Qt Started October 18, 2013
// Updated:     August 19, 2023
//***************************************************************************
#ifndef PAIRS_EDIT_H_INCLUDED
  #include "pairs_edit.h"
#endif // PAIRS_EDIT_H_INCLUDED
//--------------------------------------------------------------------------
#include <assert.h>
//***************************************************************************
// the default constructor
jfPairFS_Editor::jfPairFS_Editor(const jfPairFilterSingle* infilt, QWidget* parent):
                                 jfBaseFilterEditor(infilt, parent) {
  // most things are handled by the parent constructor
  QString omsg;
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
void jfPairFS_Editor::LoadFilter(const jfFilterBase* infilter) {
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
jfFilterBase* jfPairFS_Editor::GetFilter() {
    jfPairFilterSingle* toutput;
    // creating the result
    toutput = new jfPairFilterSingle(namedesc_edit->TryGetName());
    // getting the output
    if (insert_panel->SaveFilterData(toutput)) {
        namedesc_edit->ChangeFilter(toutput);
        return toutput;
    }
    else return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfPairFS_Editor::GeneralCheck(const jfFilterMap* filter_group) const {
    if (NameNUniq(filter_group)) return false;
    else return insert_panel->CheckValid();
}
//=============================================================================
// the default constructor
jfPairFL_Editor::jfPairFL_Editor(const jfPairFilterList* infilt, QWidget* parent):
                                 jfBaseFilterEditor( infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
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
void jfPairFL_Editor::LoadFilter(const jfFilterBase* infilter) {
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
jfFilterBase* jfPairFL_Editor::GetFilter() {
    jfPairFilterList* toutput;
    // creating the result
    toutput = new jfPairFilterList(namedesc_edit->TryGetName());
    // getting the output
    if (insert_panel->SaveFilterData(toutput)) {
        namedesc_edit->ChangeFilter(toutput);
        return toutput;
    }
    else return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFL_Editor::GeneralCheck(const jfFilterMap* filter_group) const {
    if (NameNUniq(filter_group)) return false;
    else return insert_panel->CheckValid();
}
//=============================================================================
// the default constructor
jfPairFM_Editor::jfPairFM_Editor(const jfPairFilterMultiple* infilt, QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
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
void jfPairFM_Editor::LoadFilter(const jfFilterBase* infilter) {
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
jfFilterBase* jfPairFM_Editor::GetFilter() {
    jfPairFilterMultiple* toutput;
    // creating the result
    toutput = new jfPairFilterMultiple(namedesc_edit->TryGetName());
    // getting the output
    if (insert_panel->SaveFilterData(toutput)) {
        namedesc_edit->ChangeFilter(toutput);
        return toutput;
    }
    else return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfPairFM_Editor::GeneralCheck(const jfFilterMap* filter_group) const {
    if (NameNUniq(filter_group)) return false;
    else return insert_panel->CheckValid();
}
//***************************************************************************
