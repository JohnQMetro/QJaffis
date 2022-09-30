//**************************************************************************
// Name:        ao3_panel1.cpp
// Author :     John Q Metro
// Purpose :    agentofourown.org interface
// Created:     September 3, 2012
// Conversion to Qt Started April 6, 2014
// Updated:     July 16, 2022
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
#ifndef AO3_LOOKVALS_H
    #include "../../../fanfic/data/ao3/ao3_lookvals.h"
#endif // AO3_LOOKVALS_H
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
#include <QSizePolicy>
//**************************************************************************
// the constructor
jfAO3_DFE::jfAO3_DFE(jfAO3Search* inobj, QWidget* parent):jfDefaultFilterEditorBase(inobj,1,0,parent) {

    // the column on the right
    lsizer3 = new QVBoxLayout();
    //  kudo count
    kc_picker = new jfZeroToMaxEditor("Kudo Count Range",true,true);
    lsizer3->addWidget(kc_picker);

    // author and character filters
    auth_fillabel = new QLabel("Author Filter :");
    char_fillabel = new QLabel("Character Filter :");
    auth_filedit = new jfSimpleExprEdit(false);
    char_filedit = new jfSimpleExprEdit(false);

    lsizer3->addSpacing(8);
    lsizer3->addWidget(auth_fillabel,0,Qt::AlignLeft);
    lsizer3->addWidget(auth_filedit,0);
    lsizer3->addSpacing(8);
    lsizer3->addWidget(char_fillabel,0,Qt::AlignLeft);
    lsizer3->addWidget(char_filedit,0);


    // the extra tags filter
    lsizer1 = new QGridLayout();
    // extra tags filter
    taglabel = new QLabel("Extra Tags Filter :");
    tag_filedit = new jfSimpleExprEdit(false);
    lsizer1->addWidget(taglabel,0,0,Qt::AlignLeft);
    lsizer1->addWidget(tag_filedit,0,1);
    lsizer1->setColumnStretch(1,1);


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
  kc_picker->setEnabled(!yes);
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

  // the kudo count filter
  wmin = kc_picker->GetMin();
  wmax = kc_picker->GetMax();
  kc_filter = new jfAO3KudoFilter(wmin,wmax);
  kc_filter->SetName(DEF_ao3kudof_name);
  embedded_filters->ReplaceSame(kc_filter,oindex);

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

  // the kudo count filter
  kc_filter = dynamic_cast<jfAO3KudoFilter*>(embedded_filters->GetItem(DEF_ao3kudof_name));
  if (kc_filter!=NULL) {
    xvalue = kc_filter->GetMinMax();
    atest = kc_picker->SetMinMax(xvalue);
    assert(atest);
    delete xvalue;
  }

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
    const QString fname = "jfAO3_ESearchOptions::SaveToSearch";
  // bad input
  if (outsearch==NULL) return false;
    // orientation
    orient_entry->GetTagList();
    // rating combo
    outsearch->SetRatingIndex(ratings->currentIndex());
    // result order
    outsearch->SetOrder(result_order->currentIndex());
    // warning excludes
    outsearch->SetWarningExcludes(wx_violence->isChecked(), wx_death->isChecked(), wx_rape->isChecked(), wx_underage_sex->isChecked());
    // word min and max
    outsearch->SetWordLimits(words_min->GetValue(), words_max->GetValue());
    // other stuff
    outsearch->SetExtras(completed->isChecked(), english_only->isChecked());
    outsearch->SetCrossoverOnly(crossover_only->isChecked());

    // tags
    tag_parameters->SaveToSearch(outsearch);

  return true;
}
//------------------------------
bool jfAO3_ESearchOptions::LoadFromSearch(jfAO3Search* insearch) {
  if (insearch==NULL) return false;
  // orientation
  orient_entry->SetOrChangeTags(insearch->GetOrientation());
  // rating index
  ratings->setCurrentIndex(insearch->GetRatingIndex());
  // result order
  result_order->setCurrentIndex(insearch->GetOrderIndex());
  // warning excludes
  wx_violence->setChecked(insearch->GetExcludedViolence());
  wx_death->setChecked(insearch->GetExcludedDeath());
  wx_rape->setChecked(insearch->GetExcludedRape());
  wx_underage_sex->setCheckable(insearch->GetExcludedUnderageSex());

  // min max word count
  words_min->SetValue(insearch->GetMinWordCount());
  words_max->SetValue(insearch->GetMaxWordCount());
  // flags
  english_only->setChecked(insearch->IsEnglishOnly());
  completed->setChecked(insearch->IsCompletedOnly());
  crossover_only->setChecked(insearch->IsCrossoverOnly());

  tag_parameters->LoadFromSearch(insearch);

  adjustSize();
  // done
  return true;
}
//------------------------------
void jfAO3_ESearchOptions::SetDefaults() {
    /*
  completed->setChecked(false);
  ratings->setCurrentIndex(0);
  sex_orient->setCurrentIndex(0);
  warnings->setCurrentIndex(0);
  result_order->setCurrentIndex(0);
  extratags->SetText("");
    */
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special
void jfAO3_ESearchOptions::Disable(bool dis) {
  completed->setEnabled(!dis);
  ratings->setEnabled(!dis);
  result_order->setEnabled(!dis);
  orient_entry->setEnabled(!dis);
  ratings->setEnabled(!dis);
  result_order->setEnabled(!dis);
  wx_violence->setEnabled(!dis);
  wx_death->setEnabled(!dis);
  wx_rape->setEnabled(!dis);
  wx_underage_sex->setEnabled(!dis);

  words_min->setEnabled(!dis);
  words_max->setEnabled(!dis);
  english_only->setEnabled(!dis);
  completed->setEnabled(!dis);
  crossover_only->setEnabled(!dis);

  tag_parameters->Disable(dis);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setup methods
//------------------------------
void jfAO3_ESearchOptions::CreateSizers() {
  // starting with the optional wrapper
  if (wrapped) {
    topwrap = new QHBoxLayout();
    sbwrapper = new QGroupBox("Built-in Archive Of Our Own Filters");
    sbwrapper->setContentsMargins(0,0,0,0);

    #ifdef Q_OS_WIN
        sbwrapper->setMaximumHeight(220);
    #else
        sbwrapper->setMaximumHeight(250);
    #endif
  }
  else {
    topwrap = NULL;
    sbwrapper = NULL;
  }
  // primary grid, 3 columns, 2 rows
  mainsizer = new QGridLayout();
  mainsizer->setContentsMargins(5,5,5,5);
  // possible nesting
  if (wrapped) {
    sbwrapper->setLayout(mainsizer);
    topwrap->addWidget(sbwrapper,1);
  }
  // making some of the columns
  warnex_wrapper = new QGroupBox("Exclude Warnings");
  warnex_wrapper->setContentsMargins(0,0,0,0);
  warnex_column = new QVBoxLayout();
  warnex_column->setContentsMargins(10,5,10,5);
  warnex_column->setSpacing(0);
  warnex_wrapper->setLayout(warnex_column);

  // the top row
  toprow = new QHBoxLayout();


}
//------------------------------
void jfAO3_ESearchOptions::MakeControls() {

   orient_entry = new jfTagStatusPicker("Sex Orientation",false, true);


   // rating combo picker
   rat_label = new QLabel("Ratings :");
   ratings = new QComboBox();
   ratings->addItems(ao3values::ratingMaker.GetPredefined());
   // result order picker
   reso_label = new QLabel("Result Order :");
   result_order = new QComboBox();
   result_order->addItems(ao3values::moreMaker.GetOrderingList());
   // warning excludes
   wx_violence = new QCheckBox("Violence");
   wx_death = new QCheckBox("Major Character Death");
   wx_rape = new QCheckBox("Rape");
   wx_underage_sex = new QCheckBox("Underage Sex");

   words_min = new jfLabeledIntEdit(NULL,"Min Words",true,0,2000000000);
   words_max = new jfLabeledIntEdit(NULL, "Max Words", true, 0, 2000000000);

   english_only = new QCheckBox("English Only");
   completed = new QCheckBox("Completed Only");
   crossover_only = new QCheckBox("Crossover Only");

   tag_parameters = new jfAO3_TagParameters();

}
//------------------------------
void jfAO3_ESearchOptions::ArrangeControls() {
    int rowspace = 10;
    // filling in the top row
    toprow->addWidget(rat_label, 0, Qt::AlignLeft);
    toprow->addWidget(ratings, 0, Qt::AlignLeft);
    toprow->addSpacing(rowspace);
    toprow->addWidget(reso_label, 0, Qt::AlignLeft);
    toprow->addWidget(result_order, 0 , Qt::AlignLeft);
    toprow->addSpacing(rowspace);
    toprow->addWidget(words_min);
    toprow->addWidget(words_max);
    toprow->addSpacing(rowspace);
    toprow->addWidget(completed);
    toprow->addWidget(english_only);
    toprow->addWidget(crossover_only);

    mainsizer->addLayout(toprow,0,0,1,3);

    // first column is orientation
    mainsizer->addWidget(orient_entry,1,0);

    // warning excludes
    warnex_column->addWidget(wx_violence, 0);
    warnex_column->addWidget(wx_death, 0);
    warnex_column->addWidget(wx_rape, 0);
    warnex_column->addWidget(wx_underage_sex, 0);
    mainsizer->addWidget(warnex_wrapper,1,1);

    // tags
    mainsizer->addWidget(tag_parameters,1,2);
    mainsizer->setColumnStretch(2,1);
    mainsizer->setRowStretch(1,1);

}

//**************************************************************************
