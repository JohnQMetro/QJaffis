/******************************************************************************
Name    :   fim1_filedit.cpp
Author  :   John Q Metro
Purpose :   Editors for fim filters : thumbs rating
Created :   July 5, 2012
Conversion to Qt started October 15, 2013
Updated :   August 5, 2023
******************************************************************************/
#ifndef FIM1_FILEDIT_H_INCLUDED
  #include "fim1_filedit.h"
#endif // FIM1_FILEDIT_H_INCLUDED
//---------------------------------------------------------------------------
#ifndef FIM_CONSTANTS_H
  #include "../../../fanfic/data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H
#include <QMessageBox>
#include <assert.h>
/*****************************************************************************/
jfFIMThumbsPanel::jfFIMThumbsPanel(bool wrapped, QWidget* parent):QWidget(parent) {
    minmax = new jfMinMaxEditor("Net Thumbs Up/Down",wrapped,false,true);
    include_rdisabled = new QCheckBox("Include Disabled");
    top_layout = new QHBoxLayout();
    top_layout->addWidget(minmax,3,Qt::AlignVCenter);
    top_layout->addWidget(include_rdisabled,2,Qt::AlignVCenter);
    minmax->SetRange(-9999,99999);
    minmax->SetMinMax(20,99999);
    setLayout(top_layout);

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// get
jfFimThumbsFilter* jfFIMThumbsPanel::GetNewFilter(const QString& startName) const {
    // variables
    jfFimThumbsFilter* result;
    bool lres;
    // test
    lres = minmax->CheckMessage();
    if (!lres) return NULL;

    // things are okay
    result = new jfFimThumbsFilter(startName);
    bool chk = include_rdisabled->isChecked();
    result->SetValues(minmax->GetMin(),minmax->GetMax(),chk);
    return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// set
bool jfFIMThumbsPanel::SetFromObj(const jfFimThumbsFilter* inval) {
    // the cases where things are wrong
    if (inval==NULL) return false;
    // setting things
    bool sok = minmax->SetMinMax(inval->GetMin(),inval->GetMax());
    if (!sok) return false;
    include_rdisabled->setChecked(inval->GetIncludeDisabled());
    return true;
}
//===========================================================================
jfFimThumbsFilterEditor::jfFimThumbsFilterEditor(const jfFilterBase* infilt, QWidget* parent):jfBaseFilterEditor(infilt,parent) {
    // most things are handled by the parent constructor
    // we create the insert.. the *actual* expression editor
    insert_panel = new jfFIMThumbsPanel(false);
    if (filt_pointer!=NULL) {
        insert_panel->SetFromObj(dynamic_cast<const jfFimThumbsFilter*>(filt_pointer));
    }
    // finalizing things
    insert = new QBoxLayout(QBoxLayout::TopToBottom);
    insert->addWidget(insert_panel,0);
    insert->addStretch(1);
    // calling the sizers
    ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFimThumbsFilterEditor::LoadFilter(const jfFilterBase* infilter) {
    const QString fname = "jfFimThumbsFilterEditor::LoadFilter";
    assert(infilter!=NULL);
    // loading the data
    filt_pointer = infilter;
    insert_panel->SetFromObj(dynamic_cast<const jfFimThumbsFilter*>(filt_pointer));
    NameLoad();
}
//------------------------------------------------------------------------------
jfFilterBase* jfFimThumbsFilterEditor::GetFilter() {
    jfFimThumbsFilter* toutput;
    // getting the output
    toutput = insert_panel->GetNewFilter(namedesc_edit->TryGetName());
    namedesc_edit->ChangeFilter(toutput);
    // done
    return toutput;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfFimThumbsFilterEditor::GeneralCheck(const jfFilterMap* filter_group) const {
    jfFimThumbsFilter* result = insert_panel->GetNewFilter(namedesc_edit->TryGetName());
    if (result == NULL) return false;
    if (filter_group != NULL) {
        if (NameNUniq(filter_group)) return false;
    }
    return true;
}

//===============================================================================
// the default constructor
jfFimGenreFilterEditor::jfFimGenreFilterEditor(const jfFIMGenreFilter* infilt, QWidget* parent):
                                                jfTagFilterEditor(infilt,parent) {
    CompleteConstruction("Genres",infilt);
    if (infilt==NULL) LoadBlank();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//---------------------------
jfTagFilterCore* jfFimGenreFilterEditor::GetTagFilter() {
    jfFIMGenreFilter* result;
    result = new jfFIMGenreFilter(namedesc_edit->TryGetName());
    result->SetToEmpty();
    return result;
}
//---------------------------
bool jfFimGenreFilterEditor::isListLong() const {
    return false;
}
//---------------------------
void jfFimGenreFilterEditor::LoadBlank() {
    assert(fimcon::tags != NULL);
    jfTagListing* new_blank_list = new jfTagListing();
    fimcon::tags->SetTagsToEmpty(*new_blank_list,FIMT_genre);
    shortlist_panel->SetTagList(new_blank_list);
}
//===============================================================================
// the default constructor
jfFimCharacterFilterEditor::jfFimCharacterFilterEditor(const jfFIMCharacterFilter* infilt,
                                                       QWidget* parent):jfTagFilterEditor(infilt,parent) {
    CompleteConstruction("Characters", infilt);
    if (infilt==NULL) LoadBlank();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//---------------------------
jfTagFilterCore* jfFimCharacterFilterEditor::GetTagFilter() {
    jfFIMCharacterFilter* result;
    result = new jfFIMCharacterFilter(namedesc_edit->TryGetName());
    result->SetToEmpty();
    return result;
}
//---------------------------
bool jfFimCharacterFilterEditor::isListLong() const {
    return true;
}
//---------------------------
void jfFimCharacterFilterEditor::LoadBlank() {
    assert(fimcon::tags != NULL);
    jfTagListing* new_blank_listing = new jfTagListing();
    fimcon::tags->SetTagsToEmpty(*new_blank_listing,FIMT_character);
    longlist_panel->setBoldStrings(fimcon::important_characters);
    longlist_panel->SetInitialData("Characters",new_blank_listing,false);
}
//===============================================================================================================

// constructor
jfFIM_RatingEditPanel::jfFIM_RatingEditPanel(bool horizontal, const jfFimRatingFilter* invalue,
                     QWidget* parent):QWidget(parent) {
  // creating gui elemnts
  ech = new QCheckBox("Everyone");
  tch = new QCheckBox("Teen");
  mch = new QCheckBox("Mature ");
  // sizer creation
  ishoriz = horizontal;
  if (ishoriz) msizer = new QBoxLayout(QBoxLayout::RightToLeft);
  else msizer = new QBoxLayout(QBoxLayout::TopToBottom);
  // adding items in the sizer
  msizer->addWidget(ech,0);
  msizer->addWidget(tch,0);
  msizer->addWidget(mch,0);
  // done
  setLayout(msizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting and getting values
//--------------------------------------------------------------
bool jfFIM_RatingEditPanel::StoreValue(jfFimRatingFilter* saveto) const {
  if (saveto==NULL) return false;
  QString inval;
  if (ech->isChecked()) inval +="E";
  else if (tch->isChecked()) inval +="T";
  else if (mch->isChecked()) inval +="M";
  saveto->FromString(inval);
  return true;
}
//--------------------------------------------------------------
bool jfFIM_RatingEditPanel::LoadValue(const jfFimRatingFilter* getfrom) {
  if (getfrom==NULL) return false;
  ech->setChecked(getfrom->Includes("E"));
  tch->setChecked(getfrom->Includes("T"));
  mch->setChecked(getfrom->Includes("M"));
  return true;
}
//--------------------------------------------------------------
jfFimRatingFilter* jfFIM_RatingEditPanel::GetValue(const QString& name) const {
  jfFimRatingFilter* resval = new jfFimRatingFilter(name);
  StoreValue(resval);
  return resval;
}
/*****************************************************************************/
jfFIM_RatingFilterEditor::jfFIM_RatingFilterEditor(const jfFimRatingFilter* infilt, QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfFIM_RatingEditPanel(false,infilt);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,0);
  insert->addStretch(1);
  // calling the sizers
  ArrangeSizers();
  if (infilt!=NULL) NameLoad();
  if (infilt!=NULL) LoadFilter(infilt);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfFIM_RatingFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  insert_panel->LoadValue(dynamic_cast<const jfFimRatingFilter*>(filt_pointer));
  NameLoad();
}
//-------------------------------------
jfFilterBase* jfFIM_RatingFilterEditor::GetFilter() {
  jfFimRatingFilter* result = insert_panel->GetValue(namedesc_edit->TryGetName());
  namedesc_edit->ChangeFilter(result);
  return result;
}
//--------------------------------------------------------------
bool jfFIM_RatingFilterEditor::GeneralCheck(const jfFilterMap* filter_group) const {
    jfFimRatingFilter* result = insert_panel->GetValue(namedesc_edit->TryGetName());
    if (result == NULL) return false;
    if (filter_group != NULL) {
        if (NameNUniq(filter_group)) return false;
    }
    return true;
}
/*****************************************************************************/
