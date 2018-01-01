/******************************************************************************
Name    :   ao3_special1.cpp
Author  :   John Q Metro
Purpose :   Declares some base class for filter diting
Created :   September 10, 2012
Conversion to Qt Started October 18, 2013
Updated :   September 10, 2012
******************************************************************************/
#ifndef AO3_SPECIAL1_H_INCLUDED
  #include "ao3_special1.h"
#endif // AO3_SPECIAL1_H_INCLUDED
//--------------------------------------------
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "../../../core/filters/ao3/ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
#ifndef AO3_CONSTS_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED
//-----------------------------------
#include <assert.h>
/*****************************************************************************/
const QString ratinglist[] = { "General","Teen","Mature","Explicit","Unspecified"};
const QString orientlist[] = { "Femslash","General","Heterosexual","Multiple",
      "Other","Slash","Unspecified"};
//=============================================================================
jfAO3_RatingFilterEditor::jfAO3_RatingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap,
         QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfCharCheckBoxGroup("Ratings",5,ratinglist,ao3con::rating_ac,3);
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
void jfAO3_RatingFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  const jfAO3RatingFilter* temp = dynamic_cast<const jfAO3RatingFilter*>(infilter);
  insert_panel->SetFromString(temp->ToString());
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfAO3_RatingFilterEditor::GetFilter() {
  jfAO3RatingFilter* result = new jfAO3RatingFilter(insert_panel->GetStringValue());
  namedesc_edit->ChangeObj(result);
  return result;
}
//--------------------------------------------------------------
bool jfAO3_RatingFilterEditor::GeneralCheck() const {
  return true;
}
//=============================================================================
jfAO3_OrientFilterEditor::jfAO3_OrientFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap,
       QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfCharCheckBoxGroup("Sexual Orientation",7,orientlist,ao3con::orient_ac,3);
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
void jfAO3_OrientFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  const jfAO3OrientationFilter* temp = dynamic_cast<const jfAO3OrientationFilter*>(infilter);
  insert_panel->SetFromString(temp->ToString());
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfAO3_OrientFilterEditor::GetFilter() {
  jfAO3OrientationFilter* result = new jfAO3OrientationFilter(insert_panel->GetStringValue());
  namedesc_edit->ChangeObj(result);
  return result;
}
//--------------------------------------------------------------
bool jfAO3_OrientFilterEditor::GeneralCheck() const {
  return true;
}
/*****************************************************************************/
