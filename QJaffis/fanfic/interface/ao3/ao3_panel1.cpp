//**************************************************************************
// Name:        ao3_panel1.cpp
// Author :     John Q Metro
// Purpose :    agentofourown.org interface
// Created:     September 3, 2012
// Conversion to Qt Started April 6, 2014
// Updated:     July 17, 2016
//**************************************************************************
#ifndef AO3_PANEL1_H_INCLUDED
  #include "ao3_panel1.h"
#endif // AO3_PANEL1_H_INCLUDED
//------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef AO3_CONSTS_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED
//--------------------------------------
#ifndef AO3_SPECIAL1_H_INCLUDED
  #include "../../../interface/filters/ao3/ao3_special1.h"
#endif // AO3_SPECIAL1_H_INCLUDED
#ifndef EXTENDED_SEXP_FILTER_H
  #include "../../../core/filters/extended/extended_sexp.h"
#endif // EXTENDED_SEXP_FILTER_H
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../../../core/filters/extended/morefilters1.h"
#endif
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "../../../core/filters/ao3/ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
//--------------------------------------
#include <assert.h>
//**************************************************************************
// the constructor
jfAO3_DFE::jfAO3_DFE(jfAO3Search* inobj, QWidget* parent):jfDefaultFilterEditorBase(inobj,1,1,parent) {

  // the author and character filters
  auth_fillabel = new QLabel("Author Filter :");
  char_fillabel = new QLabel("Character Filter :");
  auth_filedit = new jfSimpleExprEdit(false);
  char_filedit = new jfSimpleExprEdit(false);
  lsizer1 = new QGridLayout();
  lsizer1->addWidget(auth_fillabel,0,0,Qt::AlignVCenter|Qt::AlignRight);
  lsizer1->addWidget(auth_filedit,0,1);
  lsizer1->addWidget(char_fillabel,1,0,Qt::AlignVCenter);
  lsizer1->addWidget(char_filedit,1,1);
  lsizer1->setColumnStretch(1,1);
  // word count and kudo count
  wc_picker = new jfZeroToMaxEditor("Word Count Range",true,false);
  kc_picker = new jfZeroToMaxEditor("Kudo Count Range",true,false);
  lsizer2 = new QHBoxLayout();
  lsizer2->addWidget(wc_picker,1);
  lsizer2->addWidget(kc_picker,1);
  // the completed filter editor
  cpicker = new jfComplFiltEdit(NULL);
  // additional rating and orientation filter
  ratpicker = new jfCharCheckBoxGroup("Ratings", 5,ratinglist,ao3con::rating_ac,0);
  orientpick = new jfCharCheckBoxGroup("Sexual Orientation", 7,orientlist,ao3con::orient_ac,4);
  // extra tags filter
  taglabel = new QLabel("Extra Tags Filter :");
  tag_filedit = new jfSimpleExprEdit(false);
  // arranging
  lsizer3 = new QVBoxLayout();
  lsizer3->addLayout(lsizer2);
  lsizer3->addWidget(cpicker,0);
  lsizer3->addWidget(ratpicker,0);
  lsizer3->addWidget(orientpick,0);
  lsizer3->addSpacing(8);
  lsizer3->addWidget(taglabel,0,Qt::AlignLeft);
  lsizer3->addWidget(tag_filedit,0);
  lsizer3->addStretch(2);
  top_sizer->addLayout(lsizer3,1);
  // arranging the extra filters
  // adding to the main sizer
  main_sizer->addLayout(lsizer1,0);
  // finishing off
  ChangeSearch(inobj);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
void jfAO3_DFE::Disable(bool yes) {
  // extra internal gui items
  auth_filedit->setEnabled(!yes);
  char_filedit->setEnabled(!yes);
  wc_picker->setEnabled(!yes);
  kc_picker->setEnabled(!yes);
  cpicker->setEnabled(!yes);
  ratpicker->setEnabled(!yes);
  orientpick->setEnabled(!yes);
  tag_filedit->setEnabled(!yes);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methds
//--------------------------------------------------------------------
bool jfAO3_DFE::SaveFiltersExtended() {
  // local variables
  size_t oindex;
  QString omsg;
  jfSimpleExpr* exprval;
  jfAuthExprFilter* author_filter;
  jfCharExprFilter* char_filter;
  jfWordCountFilter* wc_filter;
  jfCompletedFilter* cm_filter;
  jfExtraTagFilter* et_filter;
  jfAO3KudoFilter* kc_filter;
  size_t wmin, wmax;
  // the author filter
  exprval = auth_filedit->CheckFilter(omsg);
  assert(exprval!=NULL);
  author_filter = new jfAuthExprFilter(exprval);
  author_filter->SetName(DEF_ao3author_name);
  embedded_filters->ReplaceSame(author_filter,oindex);
  // the character filter
  exprval = char_filedit->CheckFilter(omsg);
  assert(exprval!=NULL);
  char_filter = new jfCharExprFilter(exprval);
  char_filter->SetName(DEF_ao3chars_name);
  embedded_filters->ReplaceSame(char_filter,oindex);
  // the word count filter
  wmin = wc_picker->GetMin();
  wmax = wc_picker->GetMax();
  wc_filter = new jfWordCountFilter(wmin,wmax);
  wc_filter->SetName(DEF_ao3wc_name);
  embedded_filters->ReplaceSame(wc_filter,oindex);
  // the kudo count filter
  wmin = kc_picker->GetMin();
  wmax = kc_picker->GetMax();
  kc_filter = new jfAO3KudoFilter(wmin,wmax);
  kc_filter->SetName(DEF_ao3kudof_name);
  embedded_filters->ReplaceSame(kc_filter,oindex);
  // completed filter
  cm_filter = cpicker->GetValue();
  cm_filter->SetName(DEF_ao3com_name);
  embedded_filters->ReplaceSame(cm_filter,oindex);
  // the rating and orientation filter
  jfAO3RatingFilter* rat_filter = new jfAO3RatingFilter(ratpicker->GetStringValue());
  rat_filter->SetName(DEF_ao3rat_name);
  embedded_filters->ReplaceSame(rat_filter,oindex);
  jfAO3OrientationFilter* ori_filter = new jfAO3OrientationFilter(orientpick->GetStringValue());
  ori_filter->SetName(DEF_ao3ori_name);
  embedded_filters->ReplaceSame(ori_filter,oindex);
  // extra tags filter
  exprval = tag_filedit->CheckFilter(omsg);
  assert(exprval!=NULL);
  et_filter = new jfExtraTagFilter(exprval);
  et_filter->SetName(DEF_ao3etf_name);
  embedded_filters->ReplaceSame(et_filter,oindex);
  // done
  return true;
}
//--------------------------------------------------------------------
bool jfAO3_DFE::ChangeSearchExtended(jfSearchCore* obj_data) {
  const QString fname = "jfAO3_DFE::ChangeSearchExtended";
  // local variables
  QString omsg;
  jfAuthExprFilter* author_filter;
  jfCharExprFilter* char_filter;
  jfWordCountFilter* wc_filter;
  jfCompletedFilter* cm_filter;
  jfAO3RatingFilter* rat_filter;
  jfAO3OrientationFilter* ori_filter;
  jfExtraTagFilter* et_filter;
  jfAO3KudoFilter* kc_filter;
  jfIntPair* xvalue;
  bool atest;
  /**/JDEBUGLOG(fname,1);
  // The author expression filter
  author_filter = dynamic_cast<jfAuthExprFilter*>(embedded_filters->GetItem(DEF_ao3author_name));
  if (author_filter!=NULL) {
    atest = auth_filedit->SetData(author_filter->ToString(),omsg);
    assert(atest);
  }
  // then, the character expression filter
  char_filter = dynamic_cast<jfCharExprFilter*>(embedded_filters->GetItem(DEF_ao3chars_name));
  if (char_filter!=NULL) {
    atest = char_filedit->SetData(char_filter->ToString(),omsg);
    assert(atest);
  }
  // the word count filter
  wc_filter = dynamic_cast<jfWordCountFilter*>(embedded_filters->GetItem(DEF_ao3wc_name));
  if (wc_filter!=NULL) {
    xvalue = wc_filter->GetMinMax();
    atest = wc_picker->SetMinMax(xvalue);
    assert(atest);
    delete xvalue;
  }
  // the kudo count filter
  kc_filter = dynamic_cast<jfAO3KudoFilter*>(embedded_filters->GetItem(DEF_ao3kudof_name));
  if (kc_filter!=NULL) {
    xvalue = kc_filter->GetMinMax();
    atest = kc_picker->SetMinMax(xvalue);
    assert(atest);
    delete xvalue;
  }
  // the completed filter
  cm_filter = dynamic_cast<jfCompletedFilter*>(embedded_filters->GetItem(DEF_ao3com_name));
  if (cm_filter!=NULL) {
    atest = cpicker->LoadValue(cm_filter);
    assert(atest);
  }
  // orientation filter
  ori_filter = dynamic_cast<jfAO3OrientationFilter*>(embedded_filters->GetItem(DEF_ao3ori_name));
  if (ori_filter!=NULL) {
    atest = orientpick->SetFromString(ori_filter->ToString());
    assert(atest);
  }
  else orientpick->CheckAll();
  // ratingd filter
  rat_filter = dynamic_cast<jfAO3RatingFilter*>(embedded_filters->GetItem(DEF_ao3rat_name));
  if (rat_filter!=NULL) {
    atest = ratpicker->SetFromString(rat_filter->ToString());
    assert(atest);
  }
  else ratpicker->CheckAll();
  // The extra tags filter
  et_filter = dynamic_cast<jfExtraTagFilter*>(embedded_filters->GetItem(DEF_ao3etf_name));
  if (et_filter!=NULL) {
    atest = tag_filedit->SetData(et_filter->ToString(),omsg);
    assert(atest);
  }
   /**/JDEBUGLOG(fname,2);
  // done
  return true;
}
//=========================================================================
// the constructor
jfAO3_ESearchOptions::jfAO3_ESearchOptions(jfAO3Search* insearch, bool dowrapper,
       bool fvertical, QWidget* parent):QWidget(parent) {
  const QString fname = "jfAO3_ESearchOptions::jfAO3_ESearchOptions";
  // basics checks and setup
  wrapped = dowrapper;
  vertflex = fvertical;
  // starting
  CreateSizers();
  MakeControls();
  ArrangeControls();
  // loading things
  if (insearch!=NULL) LoadFromSearch(insearch);
  else SetDefaults();
  // finally
  if (wrapped) setLayout(topwrap);
  else setLayout(mainsizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// io
//------------------------------
bool jfAO3_ESearchOptions::SaveToSearch(jfAO3Search* outsearch) const {
  // bad input
  if (outsearch==NULL) return false;
  // variables
  bool cval;
  int rv[4];
  QString tagval;
  // the completed value
  cval = completed->isChecked();
  // the extra tag
  tagval = extratags->GetText();
  // the other values
  rv[0] = ratings->currentIndex();
  if (rv[0]<0) return false;
  rv[1] = sex_orient->currentIndex();
  if (rv[1]<0) return false;
  rv[2] = warnings->currentIndex();
  if (rv[2]<0) return false;
  rv[3] = result_order->currentIndex();
  if (rv[3]<0) return false;

  // setting the value
  outsearch->SetSearchOptions(rv[0],rv[1],rv[2],tagval,cval);
  outsearch->SetOrder(rv[3]);
  return true;
}
//------------------------------
bool jfAO3_ESearchOptions::LoadFromSearch(jfAO3Search* insearch) {
  if (insearch==NULL) return false;
  // ratings
  int gvalue = insearch->GetSearchValue(0);
  if (gvalue==-1) return false;
  ratings->setCurrentIndex(gvalue);
  // sexal orientation
  gvalue = insearch->GetSearchValue(1);
  if (gvalue==-1) return false;
  sex_orient->setCurrentIndex(gvalue);
  // warnings
  gvalue = insearch->GetSearchValue(2);
  if (gvalue==-1) return false;
  warnings->setCurrentIndex(gvalue);
  // result order
  gvalue = insearch->GetSearchValue(3);
  if (gvalue==-1) return false;
  result_order->setCurrentIndex(gvalue);
  // completed status
  completed->setChecked(insearch->GetCompletedValue());
  // extra tags
  extratags->SetText(insearch->GetExtraSTag());
  // done
  return true;
}
//------------------------------
void jfAO3_ESearchOptions::SetDefaults() {
  completed->setChecked(false);
  ratings->setCurrentIndex(0);
  sex_orient->setCurrentIndex(0);
  warnings->setCurrentIndex(0);
  result_order->setCurrentIndex(0);
  extratags->SetText("");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special
void jfAO3_ESearchOptions::Disable(bool dis) {
  completed->setEnabled(!dis);
  ratings->setEnabled(!dis);
  sex_orient->setEnabled(!dis);
  warnings->setEnabled(!dis);
  result_order->setEnabled(!dis);
  extratags->setEnabled(!dis);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setup methods
//------------------------------
void jfAO3_ESearchOptions::CreateSizers() {
  // starting with the optional wrapper
  if (wrapped) {
    topwrap = new QHBoxLayout();
    sbwrapper = new QGroupBox("Built-in Archive Of Our Own Filters");
  }
  else {
    topwrap = NULL;
    sbwrapper = NULL;
  }
  // the grid sizer
  mainsizer = new QGridLayout();
  // possible nesting
  if (wrapped) {
    sbwrapper->setLayout(mainsizer);
    topwrap->addWidget(sbwrapper,1);
  }
}
//------------------------------
void jfAO3_ESearchOptions::MakeControls() {
   rat_label = new QLabel("Ratings : ");
   ratings = new QComboBox();
   ratings->addItems(*ao3con::rat_list);
   soc_label = new QLabel("Sex Orientation : ");
   sex_orient = new QComboBox();
   sex_orient->addItems(*ao3con::soc_list);
   warn_label  = new QLabel("Warnings : ");
   warnings = new QComboBox();
   warnings->addItems(*ao3con::warn_list);
   reso_label = new QLabel("Result Order : ");
   result_order = new QComboBox();
   result_order->addItems(*ao3con::order_list);
   completed = new QCheckBox("Completed");
   extratags = new jfLabeledEdit(NULL,"Tag Search",true);
}
//------------------------------
void jfAO3_ESearchOptions::ArrangeControls() {

  mainsizer->addWidget(rat_label,0,0,Qt::AlignRight);
  mainsizer->addWidget(ratings,0,1);
  mainsizer->addWidget(soc_label,0,2,Qt::AlignRight);
  mainsizer->addWidget(sex_orient,0,3);
  mainsizer->addWidget(warn_label,0,4,Qt::AlignRight);
  mainsizer->addWidget(warnings,0,5);
  mainsizer->addWidget(completed,0,6,Qt::AlignVCenter);
  mainsizer->addWidget(reso_label,1,0,Qt::AlignRight);
  mainsizer->addWidget(result_order,1,1);
  mainsizer->addWidget(extratags,1,2,1,5);
}
//=========================================================================
jfAO3_SearchOptions::jfAO3_SearchOptions(jfAO3Search* insearch, QWidget* parent):jfSearchOptionsBase(parent) {
  assert(insearch!=NULL);
  istore = insearch;
  main_picker = new jf_Gen_CatPicker(insearch->GetHolder() ,insearch->GetSelector(),7);
  soptions = new jfAO3_ESearchOptions(insearch,true,false);
  mlayout = new QVBoxLayout();
  mlayout->addWidget(main_picker,1);
  mlayout->addWidget(soptions,0);
  setLayout(mlayout);
}

//+++++++++++++++++++++++++++++++++++++++++++++++
// methods
//---------------------
bool jfAO3_SearchOptions::LoadFrom(jfSearchCore* insearch) {
  const QString fname = "jfAO3_SearchOptions::LoadFrom";

  // starting...
  istore = dynamic_cast<jfAO3Search*>(insearch);
  main_picker->ChangeSelector(istore->GetSelector());
  soptions->LoadFromSearch(istore);
  // done
  return true;
}
//---------------------
bool jfAO3_SearchOptions::StoreTo(jfSearchCore* outsearch) {
  const QString fname = "jfAO3_SearchOptions::StoreTo";
  // local variables
  jfAO3Search* temp;
  size_t ncats;
  // setting the category selection
  main_picker->SaveFilters();
  temp = dynamic_cast<jfAO3Search*>(outsearch);
  soptions->SaveToSearch(temp);
  temp->ApplySelection();
  ncats = temp->GetNumCats();
  return (ncats!=0);
}

//=========================================================================
// the constructor
jfAO3_Panel1::jfAO3_Panel1(jfAO3Search* insearch, QWidget* parent):jfSearchPanelBase(insearch,true,true,0,parent) {
// constants
  const QString fname="jfAO3_Panel1::jfAO3_Panel1";
  // start
  SearchPanel = new jfAO3_SearchOptions(insearch);
  // sizer arranging
  filt_panel = MakeTypedFilEdit();
  ArrangePanels();
  // finishing off
  ChangeObj(mainobj);

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other external methods
bool jfAO3_Panel1::LoadToObj() {
  const QString fname = "jfAO3_Panel1::LoadToObj";
  // starting
  if (mainobj==NULL) return false;
  if (!filt_panel->SaveFilters()) return false;
  return SearchPanel->StoreTo(mainobj);
}
//-----------------------------------------------------------------------
bool jfAO3_Panel1::ChangeObj(jfSearchCore* obj_data) {
  const QString fname = "jfAO3_Panel1::ChangeObj";
  // local variables
  jfAO3Search* temp;
  // starting...
  temp = dynamic_cast<jfAO3Search*>(obj_data);
  mainobj = obj_data;
  // filters
  filt_panel->ChangeSearch(obj_data);
  SearchPanel->LoadFrom(obj_data);
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfDefaultFilterEditorBase* jfAO3_Panel1::MakeTypedFilEdit() {
  jfAO3_DFE* result;
  result = new jfAO3_DFE(dynamic_cast<jfAO3Search*>(mainobj));
  return result;
}
//**************************************************************************
