/******************************************************************************
Name    :   fim_panel1.cpp
Author  :   John Q Metro
Purpose :   Defines the custom first panel for fimfiction.net searching
Created :   June 23, 2012
Conversion to Qt started February 3, 2014
Updated :   October 15, 2019
******************************************************************************/
#ifndef FIM_PANEL1_H_INCLUDED
  #include "fim_panel1.h"
#endif // FIM_PANEL1_H_INCLUDED
//--------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FIM_CONSTANTS_H
  #include "../../data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H
#ifndef WIDGETS3_H_INCLUDED
  #include "../../../interface/misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef FIMFILTERS2_H_INCLUDED
  #include "../../../core/filters/fim/fimfilters2.h"
#endif // FIMFILTERS2_H_INCLUDED

#include <assert.h>
//======================================
//
// the constructor
jfFIM_DFE::jfFIM_DFE(jfFIMSearch* inobj, QWidget* parent):jfDefaultFilterEditorBase(inobj,4,1,parent) {
  // gui elements
  // thumbs up or down filter
  tud_filedit = new jfFIMThumbsPanel(true);
  // percentage-based thumbs filter
  tper_edit = new jfFimThumbPercentPanel();
  // new expression filter
  dual_label = new QLabel("Expression Filter matching either short or long descriptions :");
  dual_expredit = new jfSimpleExprEdit(true);
  // arraging
  exp_sizer->addSpacing(10);
  exp_sizer->addWidget(dual_label,0,Qt::AlignLeft);
  exp_sizer->addWidget(dual_expredit,2);

  sidesizer = new QVBoxLayout();
  sidesizer->addWidget(tud_filedit,0,Qt::AlignTop);
  sidesizer->addWidget(tper_edit,0,Qt::AlignTop);
  // sidesizer->addStretch(1);
  // top_sizer->addLayout(sidesizer,1);
  exp_sizer->addLayout(sidesizer,2);
  // loading data
  ChangeSearch(inobj);
  typed_search = inobj;
  // finishing

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methds
bool jfFIM_DFE::SaveFiltersExtended() {
  // local variables
  jfFimThumbsFilter* thumbs_filter;
  jfFimThumbPercentFilter* thumb_perfilter;
  jfSimpleExpr* testval;
  jfFIM_DualDesc_ExprFilter* dualexpr_filter;
  size_t oindex;
  int gvalue;
  QString test_msg;
  // saving general thumbs data
  thumbs_filter = tud_filedit->GetNewFilter();
  thumbs_filter->SetName(DEF_fimthumbs_name);
  embedded_filters->ReplaceSame(thumbs_filter,oindex);
  // saving percent thumbs stuff
  thumb_perfilter = tper_edit->GetNewFilter();
  thumb_perfilter->SetName(DEF_fimpthumbs_name);
  embedded_filters->ReplaceSame(thumb_perfilter,oindex);
  // saving dual expr filter stuff
  testval = dual_expredit->CheckFilter(test_msg);
  if (testval==NULL) return false;
  dualexpr_filter = new jfFIM_DualDesc_ExprFilter(testval);
  dualexpr_filter->SetName(DEF_fimdualdesc_name);
  embedded_filters->ReplaceSame(dualexpr_filter,oindex);
  // done
  return true;
}
//--------------------------------------------------------------------
bool jfFIM_DFE::ChangeSearchExtended(jfSearchCore* obj_data) {
    const QString fname = "jfFIM_DFE::ChangeSearchExtended";
  // variables
  jfFimThumbsFilter* thumbs_filter;
  jfFimThumbPercentFilter* thumb_perfilter;
  jfFIM_DualDesc_ExprFilter* qvalue;
  QString exp_value, err_msg;
  bool atest;  size_t pvalue;
  // thumbs filter
  /**/JDEBUGLOG(fname,1)
  thumbs_filter = dynamic_cast<jfFimThumbsFilter*>(embedded_filters->GetItem(DEF_fimthumbs_name));
  /**/JDEBUGLOG(fname,1)
  if (thumbs_filter!=NULL) {
      /**/JDEBUGLOG(fname,2)
    atest = tud_filedit->SetFromObj(thumbs_filter);
    assert(atest);
  }
  /**/JDEBUGLOG(fname,3)
  // thumbs percent filter
  thumb_perfilter = dynamic_cast<jfFimThumbPercentFilter*>(embedded_filters->GetItem(DEF_fimpthumbs_name));
  /**/JDEBUGLOG(fname,4)
  if (thumb_perfilter!=NULL) {
      /**/JDEBUGLOG(fname,5)
      tper_edit->SetFromObj(thumb_perfilter);
  }
  /**/JDEBUGLOG(fname,6)
  // dual summary expression match filter
  jfBaseFilter* bddesc = embedded_filters->GetItem(DEF_fimdualdesc_name);
  /**/JDEBUGLOGB(fname,7,(bddesc == NULL))
  qvalue = dynamic_cast<jfFIM_DualDesc_ExprFilter*>(bddesc);
  if (qvalue!=NULL) {
    exp_value = qvalue->ToString();
    atest = dual_expredit->SetData(exp_value,err_msg);
    assert(atest);
  }
  /**/JDEBUGLOG(fname,12)
  return true;
}
//===============================================================================
jfFIM_SearchOptions::jfFIM_SearchOptions(QWidget* parent):jfSearchOptionsBase(parent) {
  CreateWidgets();
  ArrangeWidgets();
  setLayout(stack);
  egcbChanged(0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// load and store data
//---------------------------
bool jfFIM_SearchOptions::LoadFrom(jfSearchCore* insearch) {
    const QString fname = "jfFIM_SearchOptions::LoadFrom";
  // local variables
  QString omsg,tltag;
  jfFIMSearch* typed_insearch;
  IntPair* nwc;
  int rindex;
  size_t oindex;
  QChar rval;
  bool tval;
  // starting...
  /**/JDEBUGLOG(fname,1)
  typed_insearch = dynamic_cast<jfFIMSearch*>(insearch);
  // loading up the strings
  search_edit->setText(typed_insearch->GetSString());
  /**/JDEBUGLOG(fname,2)
  // loading the other values
  genres_entry->SetOrChangeTags(typed_insearch->GetGenres());
  /**/JDEBUGLOG(fname,3)
  content_entry->SetOrChangeTags(typed_insearch->GetContentType());
  /**/JDEBUGLOG(fname,4)
  char_sentry->setBoldStrings(fimcon::important_characters);
  /**/JDEBUGLOG(fname,5)
  char_sentry->SetInitialData("Characters",typed_insearch->GetCharacters(),false);
  /**/JDEBUGLOG(fname,6)
  cbox->setChecked(typed_insearch->GetCompleted());
  /**/JDEBUGLOG(fname,7)
  // setting the word count filter
  nwc = typed_insearch->GetWC();
  tval = wcount->SetMinMax(nwc);
  /**/JDEBUGLOGB(fname,8,tval)
  assert(tval);
  delete nwc;
  // setting the ratings value
  rval = typed_insearch->GetRating();
  rindex =fimcon::ratchars.indexOf(rval);
  /**/JDEBUGLOGI(fname,9,rindex)
  assert(rindex>=0);
  rating_picker->setCurrentIndex(rindex);
  // order value
  oindex = typed_insearch->GetOrder();
  /**/JDEBUGLOGI(fname,10,oindex)
  order_picker->setCurrentIndex(oindex);
  // other
  matures->setChecked(typed_insearch->GetMature());
  /**/JDEBUGLOG(fname,11)
  warnings_entry->SetOrChangeTags(typed_insearch->GetWarnings());
  /**/JDEBUGLOG(fname,12)
  // equestria girl tristate cb
  jfTAG_STATUS egstatus = typed_insearch->GetEGStatus();
  /**/JDEBUGLOG(fname,13)
  if (egstatus == jfts_NONE) equestria_girls_cb->setCheckState(Qt::Unchecked);
  else if (egstatus == jfts_EXCLUDE) equestria_girls_cb->setCheckState(Qt::PartiallyChecked);
  else if (egstatus == jfts_INCLUDE) equestria_girls_cb->setCheckState(Qt::Checked);
  else assert(false);
  /**/JDEBUGLOG(fname,14)
  egcbChanged(0);
  /**/JDEBUGLOG(fname,15)
  // done
  return true;
}

//---------------------------
bool jfFIM_SearchOptions::StoreTo(jfSearchCore* outsearch) {
  // constants
  const QString fname = "jfFIM_SearchOptions::StoreTo";
  // local variables
  jfFIMSearch* typed_outsearch;
  bool rval;
  QString omsg, ltag;
  QChar rchar;
  size_t oindex;
   // starting
  if (outsearch==NULL) return false;
  typed_outsearch = dynamic_cast<jfFIMSearch*>(outsearch);
  // setting the edit values
  // search string
  typed_outsearch->SetSString(search_edit->text());
  // word count
  rval = wcount->Check();
  if (!rval) return false;
  typed_outsearch->SetWordcount(wcount->GetMin(),wcount->GetMax());
  // completed
  typed_outsearch->SetCompleted(cbox->isChecked());
  // rating and order picker
  rchar = (rating_picker->currentText())[0];
  oindex = order_picker->currentIndex();
  typed_outsearch->SetMinor(oindex,rchar);
  // other
  typed_outsearch->SetMature(matures->isChecked());
  // equestria girld tristate cb
  Qt::CheckState egcheck = equestria_girls_cb->checkState();
  if (egcheck == Qt::Unchecked) typed_outsearch->SetEGStatus(jfts_NONE);
  else if (egcheck == Qt::PartiallyChecked) typed_outsearch->SetEGStatus(jfts_EXCLUDE);
  else typed_outsearch->SetEGStatus(jfts_INCLUDE);
  // save tag listings from the specialist widgets
  char_sentry->SaveAndGetTagData();
  genres_entry->GetTagList();
  content_entry->GetTagList();
  warnings_entry->GetTagList();
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIM_SearchOptions::egcbChanged(int state) {
    QString eglabel = "Equestria Girls (";
    Qt::CheckState egstate = equestria_girls_cb->checkState();
    if (egstate == Qt::Unchecked) eglabel += "Ignored)";
    else if (egstate == Qt::PartiallyChecked) eglabel += "Excluded)";
    else eglabel += "Required)";
    equestria_girls_cb->setText(eglabel);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//---------------------------
// to avoid cluttering the constructor, the widgets are made here
void jfFIM_SearchOptions::CreateWidgets() {
  // making the tag wrappers
  char_wrap = new QGroupBox("Character Parameters");
  char_layt = new QHBoxLayout();
  char_wrap->setLayout(char_layt);
  // making the search box
  search_label    = new QLabel("Search String :");
  search_edit     = new QLineEdit();
  // making the genre/character search pickers
  genres_entry = new jfTagStatusPicker("Genres", false);
  content_entry = new jfTagStatusPicker("Content Type",false);
  char_sentry = new jfTagSorter(true,false);

  char_layt->addWidget(char_sentry,1);
  // extra stuff
  wcount = new jfZeroToMaxEditor("Word Count Range",true,false);
  cbox = new QCheckBox("Completed Only");
  rat_label = new QLabel("Allowed Ratings :");
  rating_picker = new QComboBox();
  LoadCBoxFromArray(rating_picker,fimcon::ratlist,(fimcon::ratchars).length());
  order_label = new QLabel("Order of Results :");
  order_picker = new QComboBox();
  LoadCBoxFromArray(order_picker,fimcon::orderlabel,fimcon::ordercount);
  matures = new QCheckBox("Include Mature Rated");
  equestria_girls_cb = new QCheckBox("Equestria Girls");
  equestria_girls_cb->setTristate(true);
  connect(equestria_girls_cb,SIGNAL(stateChanged(int)),this,SLOT(egcbChanged(int)));
  // warnings
  warnings_entry = new jfTagStatusPicker("Warnings",false);

}

//---------------------------
// after the widgets are made, we create the layouts below
void jfFIM_SearchOptions::ArrangeWidgets() {
  // making the sizers
  topbar = new QHBoxLayout();
  bottombar = new QHBoxLayout();
  pre_sidebar = new QVBoxLayout();
  sidebar = new QVBoxLayout();
  // adding the items
  topbar->addWidget(search_label,0);
  topbar->addWidget(search_edit,1);
  pre_sidebar->addWidget(genres_entry,5);
  pre_sidebar->addSpacing(6);
  pre_sidebar->addWidget(content_entry,2);

  sidebar->addWidget(order_label,0,Qt::AlignTop);
  sidebar->addWidget(order_picker,0,Qt::AlignTop);
  sidebar->addSpacing(6);
  sidebar->addWidget(wcount,0,Qt::AlignTop);
  sidebar->addSpacing(6);
  sidebar->addWidget(cbox,0,Qt::AlignTop);
  sidebar->addSpacing(6);
  sidebar->addWidget(rat_label,0,Qt::AlignTop);
  sidebar->addWidget(rating_picker,0,Qt::AlignTop);
  sidebar->addWidget(matures,0,Qt::AlignTop);
  sidebar->addSpacing(6);
  sidebar->addWidget(equestria_girls_cb,0,Qt::AlignTop);
  sidebar->addSpacing(6);
  sidebar->addWidget(warnings_entry,2);
  // sidebar->addStretch(1);

  bottombar->addLayout(pre_sidebar,5);
  bottombar->addWidget(char_wrap,10);
  bottombar->addLayout(sidebar,6);
  // finishing
  stack = new QVBoxLayout();
  stack->addLayout(topbar,0);
  stack->addLayout(bottombar);
}

//===============================================================================
// the constructor
jfFIMPanel1::jfFIMPanel1(jfFIMSearch* insearch, QWidget* parent):jfSearchPanelBase(insearch,false,false,7,parent) {
  const QString fname = "jfFIMPanel1::jfFIMPanel1";
  // starting
  /**/JDEBUGLOG(fname,1)
  typed_search = insearch;
  /**/JDEBUGLOG(fname,2)
  filt_panel = MakeTypedFilEdit();
  /**/JDEBUGLOG(fname,3)
  SearchPanel = new jfFIM_SearchOptions();
  /**/JDEBUGLOG(fname,4)
  // starting...
  // finishing off
  ArrangePanels();
  /**/JDEBUGLOG(fname,5)
  ChangeObj(mainobj);
  /**/JDEBUGLOG(fname,6)
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other external methods
bool jfFIMPanel1::LoadToObj() {
  bool sval;
  sval = filt_panel->SaveFilters();
  if (!sval) return false;
  return SearchPanel->StoreTo(typed_search);
}
//----------------------------------------------------------------------------
bool jfFIMPanel1::ChangeObj(jfSearchCore* obj_data) {
  // local variables
    const QString fname = "jfFIMPanel1::ChangeObj";
  bool rval;
  // starting...
  /**/JDEBUGLOG(fname,1)
  typed_search = dynamic_cast<jfFIMSearch*>(obj_data);
  mainobj = obj_data;
  /**/JDEBUGLOG(fname,2)
  // filters
  rval = filt_panel->ChangeSearch(obj_data);
  /**/JDEBUGLOGB(fname,3,rval)
  if (!rval) return false;
  return SearchPanel->LoadFrom(obj_data);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfDefaultFilterEditorBase* jfFIMPanel1::MakeTypedFilEdit() {
  jfFIM_DFE* result;
  result = new jfFIM_DFE(typed_search);
  return result;
}
/*****************************************************************************/

