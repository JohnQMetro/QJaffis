/******************************************************************************
Name    :   ao3_special1.cpp
Author  :   John Q Metro
Purpose :   Declares some base class for filter diting
Created :   September 10, 2012
Conversion to Qt Started October 18, 2013
Updated :   August 12, 2023
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
#ifndef AO3_LOOKVALS_H
    #include "../../../fanfic/data/ao3/ao3_lookvals.h"
#endif // AO3_LOOKVALS_H
//-----------------------------------
#include <assert.h>
/*****************************************************************************/
const QString ratinglist[] = { "General","Teen","Mature","Explicit","Unspecified"};
//=============================================================================
jfAO3_RatingFilterEditor::jfAO3_RatingFilterEditor(const jfAO3RatingFilter* infilt, QWidget* parent)
                                :jfBaseFilterEditor(infilt,parent) {
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
void jfAO3_RatingFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  const jfAO3RatingFilter* temp = dynamic_cast<const jfAO3RatingFilter*>(infilter);
  insert_panel->SetFromString(temp->ToString());
  NameLoad();
}
//-------------------------------------
jfFilterBase* jfAO3_RatingFilterEditor::GetFilter() {
  jfAO3RatingFilter* result = new jfAO3RatingFilter(insert_panel->GetStringValue());
  namedesc_edit->ChangeFilter(result);
  return result;
}
//--------------------------------------------------------------
bool jfAO3_RatingFilterEditor::GeneralCheck(const jfFilterMap* infmap) const {
    if (infmap == NULL) return true;
    else if (NameNUniq(infmap)) return false;
    else return true;
}

//===============================================================================
// the default constructor
jfAO3_OrientFilterEditor::jfAO3_OrientFilterEditor( const jfAO3OrientationFilter* infilt,
                             QWidget* parent):jfTagFilterEditor(infilt,parent) {
  CompleteConstruction("Orientation",infilt);
  if (infilt==NULL) LoadBlank();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//---------------------------
jfTagFilterCore* jfAO3_OrientFilterEditor::GetTagFilter() {
  jfAO3OrientationFilter* result;
  result = new jfAO3OrientationFilter(namedesc_edit->TryGetName());
  result->SetToEmpty();
  return result;
}
//---------------------------
bool jfAO3_OrientFilterEditor::isListLong() const {
  return false;
}
//---------------------------
void jfAO3_OrientFilterEditor::LoadBlank() {
  jfTagListing* new_blank_list = new jfTagListing();
  ao3values::orientMaker.SetTagsToEmpty(*new_blank_list);
  shortlist_panel->SetTagList(new_blank_list);
}
/*****************************************************************************/
