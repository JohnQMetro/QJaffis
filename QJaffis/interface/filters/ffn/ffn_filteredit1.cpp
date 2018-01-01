/******************************************************************************
Name    :   ffn_filteredit1.cpp
Author  :   John Q Metro
Purpose :   Defines some custom FNN Filter Editors
Created :   July 8, 2016 (classes taken from other files)
Updated :   July 8, 2016
******************************************************************************/
#ifndef FFN_FILTEREDIT1_H
  #include "ffn_filteredit1.h"
#endif // FFN_FILTEREDIT1_H
//-----------------------------------
#ifndef FFN_LINKDATA_H_INCLUDED
  #include "../../../fanficnet/data/ffn_linkdata.h"
#endif // FFN_LINKDATA_H_INCLUDED
#include <assert.h>
/*****************************************************************************/
// the default constructor
jfFFN_CategoryFilterEditor::jfFFN_CategoryFilterEditor(const jfFilterMap* infmap,
        const jfFFN_CategoryExprFilter* infilt, QWidget* parent):jfSimpleSFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfFFN_CategoryFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfFFN_CategoryExprFilter* result = new jfFFN_CategoryExprFilter(inpval);
  return result;
}
//===============================================================================
// the default constructor
jfFFNGenresFilterEditor::jfFFNGenresFilterEditor(const jfFilterMap* infmap, const jfFFNGenresFilter* infilt,
                  QWidget* parent): jfTagFilterEditor(infmap,infilt,parent) {
  // starting
  CompleteConstruction("Genres",infilt);
  if (infilt==NULL) LoadBlank();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//---------------------------
jfTagFilterCore* jfFFNGenresFilterEditor::GetTagFilter() {
  jfFFNGenresFilter* result;
  result = new jfFFNGenresFilter();
  result->SetToEmpty();
  return result;
}
//--------------------------------------------
void jfFFNGenresFilterEditor::LoadBlank() {
  shortlist_panel->SetDefault(ffn_consts::taggen_list);
}
//-----------------------------------------------------------------
bool jfFFNGenresFilterEditor::isListLong() const {
  return false;
}
//============================================================================
// constructor
jfFFN_RatingEditPanel::jfFFN_RatingEditPanel(bool horizontal,
            const jfFFNRatingFilter* invalue, QWidget* parent):QWidget(parent) {
  // creating gui elemnts
  kch = new QCheckBox("K");
  kpch = new QCheckBox("K+");
  tch = new QCheckBox("T");
  mch = new QCheckBox("M");
  // sizer creation
  ishoriz = horizontal;
  if (ishoriz) {
    fsizer=NULL;
    bsizer = new QHBoxLayout();
    // adding items to the sizer
    bsizer->addWidget(kch,0);
    bsizer->addWidget(kpch,0);
    bsizer->addWidget(tch,0);
    bsizer->addWidget(mch,0);
    setLayout(bsizer);
  }
  else {
    bsizer = NULL;
    fsizer = new QGridLayout();
    // adding items to the sizer
    fsizer->addWidget(kch,0,0);
    fsizer->addWidget(kpch,0,1);
    fsizer->addWidget(tch,1,0);
    fsizer->addWidget(mch,1,1);
    setLayout(fsizer);
  }
  // done
  LoadValue(invalue);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting and getting values
//--------------------------------------------------------------
bool jfFFN_RatingEditPanel::StoreValue(jfFFNRatingFilter* saveto) const {
  if (saveto==NULL) return false;
  QString inval;
  if (kch->isChecked()) inval +="K";
  if (kpch->isChecked()) inval +="+";
  if (tch->isChecked()) inval +="T";
  if (mch->isChecked()) inval +="M";
  saveto->FromString(inval);
  return true;
}
//--------------------------------------------------------------
bool jfFFN_RatingEditPanel::LoadValue(const jfFFNRatingFilter* getfrom) {
  if (getfrom==NULL) return false;
  kch->setChecked(getfrom->Includes('K'));
  kpch->setChecked(getfrom->Includes('+'));
  tch->setChecked(getfrom->Includes('T'));
  mch->setChecked(getfrom->Includes('M'));
  return true;
}
//--------------------------------------------------------------
jfFFNRatingFilter* jfFFN_RatingEditPanel::GetValue() const {
  jfFFNRatingFilter* resval = new jfFFNRatingFilter();
  StoreValue(resval);
  return resval;
}
/*****************************************************************************/
jfFFN_RatingFilterEditor::jfFFN_RatingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap,
        QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfFFN_RatingEditPanel(false,dynamic_cast<const jfFFNRatingFilter*>(infilt));
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  insert->addStretch(1);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfFFN_RatingFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  insert_panel->LoadValue(dynamic_cast<const jfFFNRatingFilter*>(filt_pointer));
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfFFN_RatingFilterEditor::GetFilter() {
  jfFFNRatingFilter* result = insert_panel->GetValue();
  namedesc_edit->ChangeObj(result);
  return result;
}
//--------------------------------------------------------------
bool jfFFN_RatingFilterEditor::GeneralCheck() const {
  return true;
}
//****************************************************************************
jfFavsCountPanel::jfFavsCountPanel(bool wrapped, QWidget* parent):jfMinMaxEditor("Favs",wrapped,false,true,parent) {
  // setting some critical values
  SetRange(0,500); // anymore than 500 favs is a must-read
  zeromaxokay = true;
  SetMinMax(0,0);
}

//===========================================================================
jfFFNFavsFilterEditor::jfFFNFavsFilterEditor(const jfFilterMap* infmap, const jfFFNFavsFilter* infilt,
                     QWidget* parent):jfBaseFilterEditor(infmap, infilt,parent) {
  // most things are handled by the parent constructor
  // we create the insert.. the *actual* expression editor
  insert_panel = new jfFavsCountPanel(false);
  if (filt_pointer!=NULL) {
    insert_panel->SetMin(infilt->GetMin());
    insert_panel->SetMax(infilt->GetMax());
  }
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,0);
  insert->addStretch(1);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFNFavsFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  const QString fname = "jfFFNFavsFilterEditor::LoadFilter";
  assert(infilter!=NULL);
  const jfFFNFavsFilter* tpointer;
  // loading the data
  filt_pointer = infilter;
  tpointer = dynamic_cast<const jfFFNFavsFilter*>(filt_pointer);
  insert_panel->SetMin(tpointer->GetMin());
  insert_panel->SetMax(tpointer->GetMax());
  NameLoad();
}
//------------------------------------------------------------------------------
jfBaseFilter* jfFFNFavsFilterEditor::GetFilter() {
  jfFFNFavsFilter* toutput;
  // checking
  if (!(insert_panel->CheckMessage())) return NULL;
  // creating and loading the output filter
  toutput = new jfFFNFavsFilter();
  size_t min = insert_panel->GetMin();
  toutput->SetValues(min,insert_panel->GetMax());
  namedesc_edit->ChangeObj(toutput);
  // done
  return toutput;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfFFNFavsFilterEditor::GeneralCheck() const {
  return insert_panel->Check();
}
/*****************************************************************************/

