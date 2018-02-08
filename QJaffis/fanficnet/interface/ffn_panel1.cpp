//**************************************************************************
// Name:        ffn_panel1.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net search, first panel
// Created:     June 25, 2010
// conversion to Qt started July 27, 2014
// Updated:     February 8, 2018 (exclude genre built-in Fanfiction.Net filter)
//**************************************************************************
#ifndef FFN_PANEL1
  #include "ffn_panel1.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FFN_LINKDATA_H_INCLUDED
  #include "../data/ffn_linkdata.h"
#endif // FFN_LINKDATA_H_INCLUDED
#ifndef EXTENDED_SEXP_FILTER_H
  #include "../../core/filters/extended/extended_sexp.h"
#endif // EXTENDED_SEXP_FILTER_H
#ifndef MINMAX_FILTER_H
  #include "../../core/filters/extended/minmax.h"
#endif // MINMAX_FILTER_H
#ifndef FFN_SPECIAL1_H
  #include "../../core/filters/ffn/ffn_special1.h"
#endif // FFN_SPECIAL1_H
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../../core/filters/extended/morefilters1.h"
#endif
//-----------------------------
#include <assert.h>
//**************************************************************************
// the constructor
jfFFN_DFE::jfFFN_DFE(jfFFNSearch* inobj, QWidget* parent):jfDefaultFilterEditorBase(inobj,1,1,parent) {
  // creating new filters items
  // the genre picker, completed tag, and language filter
  genre_picker = new jfTagStatusPicker("Genres",false);
  cpicker = new jfComplFiltEdit(NULL);
  lang_fillabel = new QLabel("Language Filter :");
  lang_filedit = new jfSimpleExprEdit(false);
  lsizer4 = new QBoxLayout(QBoxLayout::LeftToRight);
  lsizer4->addWidget(lang_fillabel,0,Qt::AlignVCenter);
  lsizer4->addWidget(lang_filedit,1);
  lsizer3 = new QBoxLayout(QBoxLayout::TopToBottom);
  lsizer3->addWidget(genre_picker,1);
  lsizer3->addLayout(lsizer4,0);
  lsizer3->addWidget(cpicker,0);
  top_sizer->addLayout(lsizer3,1);
  // the author and character filters
  auth_fillabel = new QLabel("Author Filter :");
  char_fillabel = new QLabel("Character Filter :");
  auth_filedit = new jfSimpleExprEdit(false);
  char_filedit = new jfSimpleExprEdit(false);
  lsizer1 = new QGridLayout();
  lsizer1->addWidget(auth_fillabel,0,0,Qt::AlignRight);
  lsizer1->addWidget(auth_filedit,0,1);
  lsizer1->addWidget(char_fillabel,1,0,Qt::AlignRight);
  lsizer1->addWidget(char_filedit,1,1);
  lsizer1->setColumnStretch(1,1);

  // the word count filter
  wc_picker = new jfGenMinMaxEditor("Word Count",true,false);
  wc_picker->SetValueRange(0,9999999);
  // faborites filter
  favs_filedit = new jfFavsCountPanel(true);
  // arranging the extra filters
  lsizer2 = new QBoxLayout(QBoxLayout::LeftToRight);
  lsizer2->addLayout(lsizer1,3);
  lsizer2->addWidget(wc_picker,1);
  lsizer2->addWidget(favs_filedit,1);
  // adding to the main sizer
  main_sizer->addLayout(lsizer2,0);
  // finishing off
  ChangeSearch(inobj);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
void jfFFN_DFE::Disable(bool yes) {
  // extra internal gui items
  main_expedit->setEnabled(!yes);
  genre_picker->setEnabled(!yes);
  auth_filedit->setEnabled(!yes);
  char_filedit->setEnabled(!yes);
  wc_picker->setEnabled(!yes);
  favs_filedit->setEnabled(!yes);
  cpicker->setEnabled(!yes);
  lang_filedit->setEnabled(!yes);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methds
//--------------------------------------------------------------------
bool jfFFN_DFE::SaveFiltersExtended() {
  // local variables
  size_t oindex;
  QString omsg;
  const jfTagListing* listing;
  jfSimpleExpr* exprval;
  jfFFNGenresFilter* tag_filter;
  jfAuthExprFilter* author_filter;
  jfCharExprFilter* char_filter;
  jfWordCountFilter* wc_filter;
  jfCompletedFilter* cm_filter;
  jfLanguageExprFilter* lang_filter;
  jfFFNFavsFilter* favs_filter;
  bool atest;
  int wmin, wmax;
  // saving the tag filter
  listing = genre_picker->GetTagList();
  tag_filter = new jfFFNGenresFilter();
  atest = tag_filter->SetTags(listing);
  assert(atest);
  tag_filter->SetName(DEF_genretag_name);
  embedded_filters->ReplaceSame(tag_filter,oindex);
  // the author filter
  exprval = auth_filedit->CheckFilter(omsg);
  assert(exprval!=NULL);
  author_filter = new jfAuthExprFilter(exprval);
  author_filter->SetName(DEF_ffnauthor_name);
  embedded_filters->ReplaceSame(author_filter,oindex);
  // the character filter
  exprval = char_filedit->CheckFilter(omsg);
  assert(exprval!=NULL);
  char_filter = new jfCharExprFilter(exprval);
  char_filter->SetName(DEF_ffnchars_name);
  embedded_filters->ReplaceSame(char_filter,oindex);
  // the word count filter
  wmin = wc_picker->GetMin();
  wmax = wc_picker->GetMax();
  wc_filter = new jfWordCountFilter(wmin,wmax);
  wc_filter->SetName(DEF_ffnwc_name);
  embedded_filters->ReplaceSame(wc_filter,oindex);
  // the favorites filter
  wmin = favs_filedit->GetMin();
  wmax = favs_filedit->GetMax();
  favs_filter = new jfFFNFavsFilter(wmin,wmax);
  favs_filter->SetName(DEF_ffnfavs_name);
  embedded_filters->ReplaceSame(favs_filter,oindex);
  // completed filter
  cm_filter = cpicker->GetValue();
  cm_filter->SetName(DEF_ffncom_name);
  embedded_filters->ReplaceSame(cm_filter,oindex);
  // the language filter
  exprval = lang_filedit->CheckFilter(omsg);
  assert(exprval!=NULL);
  lang_filter = new jfLanguageExprFilter(exprval);
  lang_filter->SetName(DEF_ffnlang_name);
  embedded_filters->ReplaceSame(lang_filter,oindex);

  // done
  return true;
}
//--------------------------------------------------------------------
bool jfFFN_DFE::ChangeSearchExtended(jfSearchCore* obj_data) {
  const QString fname = "jfFFN_DFE::ChangeSearchExtended";
  // local variables
  bool notagfil;
  QString omsg;
  jfBaseFilter* test1;
  jfTagListing* listing;
  jfFFNGenresFilter* tag_filter;
  jfAuthExprFilter* author_filter;
  jfCharExprFilter* char_filter;
  jfWordCountFilter* wc_filter;
  jfCompletedFilter* cm_filter;
  jfLanguageExprFilter* lang_filter;
  jfFFNFavsFilter* favs_filter;
  bool atest;
  IntPair* wcpair;
  // we start with the genres filter
  test1 = embedded_filters->GetItem(DEF_genretag_name);
  tag_filter = dynamic_cast<jfFFNGenresFilter*>(test1);
  notagfil = (tag_filter==NULL);
  // if we have no genres filter, we make a blank one
  if (notagfil) {
    tag_filter = new jfFFNGenresFilter();
    tag_filter->SetToEmpty();
  }
  listing = tag_filter->GetTagsCopy();
  genre_picker->SetOrChangeTags(listing);
  if (notagfil) delete tag_filter;
  // next up, the author expression filter
  author_filter = dynamic_cast<jfAuthExprFilter*>(embedded_filters->GetItem(DEF_ffnauthor_name));
  if (author_filter!=NULL) {
    atest = auth_filedit->SetData(author_filter->ToString(),omsg);
    assert(atest);
  }
  // then, the character expression filter
  char_filter = dynamic_cast<jfCharExprFilter*>(embedded_filters->GetItem(DEF_ffnchars_name));
  if (char_filter!=NULL) {
    atest = char_filedit->SetData(char_filter->ToString(),omsg);
    assert(atest);
  }
  // the word count filter
  wc_filter = dynamic_cast<jfWordCountFilter*>(embedded_filters->GetItem(DEF_ffnwc_name));
  if (wc_filter!=NULL) {
    wcpair = wc_filter->GetMinMax();
    atest = wc_picker->SetMinMax(wcpair);
    delete wcpair;
    assert(atest);
  }
  // the favs filter
  favs_filter = dynamic_cast<jfFFNFavsFilter*>(embedded_filters->GetItem(DEF_ffnfavs_name));
  if (favs_filter !=NULL) {
    wcpair = favs_filter->GetMinMax();
    atest = favs_filedit->SetMinMax(wcpair);
    delete wcpair;
    assert(atest);
  }
  // the completed filter
  cm_filter = dynamic_cast<jfCompletedFilter*>(embedded_filters->GetItem(DEF_ffncom_name));
  if (cm_filter!=NULL) {
    atest = cpicker->LoadValue(cm_filter);
    assert(atest);
  }
  // next up, the language expression filter
  lang_filter = dynamic_cast<jfLanguageExprFilter*>(embedded_filters->GetItem(DEF_ffnlang_name));
  if (lang_filter!=NULL) {
    atest = lang_filedit->SetData(lang_filter->ToString(),omsg);
    assert(atest);
  }
  // done
  return true;
}
//=========================================================================
// the constructor
jfFFN_ESearchOptions::jfFFN_ESearchOptions(jfFFNSearch* insearch, bool dowrapper,
       bool fvertical, QWidget* parent):QWidget(parent) {
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
bool jfFFN_ESearchOptions::SaveToSearch(jfFFNSearch* outsearch) const {
  // bad input
  if (outsearch==NULL) return false;
  // variables
  Qt::CheckState cboxst;
  size_t cval;
  int rv[5];
  // the completed value
  cboxst = completed->checkState();
  if (cboxst==Qt::PartiallyChecked) cval = 0;
  else if (cboxst==Qt::Unchecked) cval = 1;
  else cval = 2;
  // the other values
  rv[0] = ratings->currentIndex();
  if (rv[0]<0) return false;
  rv[1] = genres->currentIndex();
  if (rv[1]<0) return false;
  rv[4] = excl_genres->currentIndex();
  if (rv[4]<0) return false;
  rv[2] = wordcount->currentIndex();
  if (rv[2]<0) return false;
  rv[3] = languages->currentIndex();
  if (rv[3]<0) return false;
  // setting the value
  return outsearch->SetSearchValues(rv[0],rv[1],rv[2],rv[3],rv[4],cval);
}
//------------------------------
bool jfFFN_ESearchOptions::LoadFromSearch(jfFFNSearch* insearch) {
  if (insearch==NULL) return false;
  // ratings
  int gvalue = insearch->GetSearchValue(0);
  if (gvalue==-1) return false;
  ratings->setCurrentIndex(gvalue);
  // include genres
  gvalue = insearch->GetSearchValue(1);
  if (gvalue==-1) return false;
  genres->setCurrentIndex(gvalue);
  // exclude genres
  gvalue = insearch->GetSearchValue(5);
  if (gvalue==-1) return false;
  excl_genres->setCurrentIndex(gvalue);
  // wordcount
  gvalue = insearch->GetSearchValue(2);
  if (gvalue==-1) return false;
  wordcount->setCurrentIndex(gvalue);
  // wordcount
  gvalue = insearch->GetSearchValue(3);
  if (gvalue==-1) return false;
  languages->setCurrentIndex(gvalue);
  // completed status
  gvalue = insearch->GetSearchValue(4);
  if (gvalue==-1) return false;
  if (gvalue==0) completed->setCheckState(Qt::PartiallyChecked);
  else if (gvalue==1) completed->setCheckState(Qt::Unchecked);
  else completed->setCheckState(Qt::Checked);
  // done
  return true;
}
//------------------------------
void jfFFN_ESearchOptions::SetDefaults() {
  completed->setCheckState(Qt::PartiallyChecked);
  ratings->setCurrentIndex(0);
  genres->setCurrentIndex(0);
  excl_genres->setCurrentIndex(0);
  wordcount->setCurrentIndex(0);
  languages->setCurrentIndex(0);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special
void jfFFN_ESearchOptions::Disable(bool dis) {
  completed->setEnabled(!dis);
  ratings->setEnabled(!dis);
  genres->setEnabled(!dis);
  excl_genres->setEnabled(!dis);
  wordcount->setEnabled(!dis);
  languages->setEnabled(!dis);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setup methods
//------------------------------
void jfFFN_ESearchOptions::CreateSizers() {
  // starting with the optional wrapper
  if (wrapped) {
    topwrap = new QHBoxLayout();
    sbwrapper = new QGroupBox("Built-in Fanfiction.Net Filters");
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
void jfFFN_ESearchOptions::MakeControls() {
   rat_label = new QLabel("Ratings : ");
   ratings = new QComboBox();
   ratings->addItems(*ffn_consts::fr_list);
   gen_label = new QLabel("Include Genre : ");
   genres = new QComboBox();
   genres->addItems(*ffn_consts::gen_list);
   wc_label  = new QLabel("Wordcount : ");
   wordcount = new QComboBox();
   wordcount->addItems(*ffn_consts::wc_list);
   lan_label = new QLabel("Languages : ");
   languages = new QComboBox();
   languages->addItems(*ffn_consts::lan_list);
   excl_gen_label = new QLabel("Exclude Genre : ");
   excl_genres = new QComboBox();
   excl_genres->addItems(*ffn_consts::gen_list);
   excl_genres->setItemText(0,"None");

   completed = new QCheckBox("Completed");
   completed->setTristate(true);
}
//------------------------------
void jfFFN_ESearchOptions::ArrangeControls() {

  mainsizer->addWidget(rat_label,0,0,Qt::AlignRight);
  mainsizer->addWidget(ratings,0,1);
  mainsizer->addWidget(wc_label,0,2,Qt::AlignRight);
  mainsizer->addWidget(wordcount,0,3);
  mainsizer->addWidget(lan_label,0,4,Qt::AlignRight);
  mainsizer->addWidget(languages,0,5);
  mainsizer->addWidget(gen_label,1,0,Qt::AlignRight);
  mainsizer->addWidget(genres,1,1);
  mainsizer->addWidget(excl_gen_label,1,2,Qt::AlignRight);
  mainsizer->addWidget(excl_genres,1,3);
  mainsizer->addWidget(completed,1,5);
}
//=========================================================================
// constructor
jfFFN_SearchPanel::jfFFN_SearchPanel(jfFFNSearch* insearch, QWidget* parent):jfSearchOptionsBase(parent) {
  const QString fname = "jfFFN_SearchPanel::jfFFN_SearchPanel";
  // setting up
  bsizer = new QVBoxLayout();
  main_picker = new jf_FFN_CatPicker(insearch->GetHolder() ,insearch->GetSelector());
  soptions = new jfFFN_ESearchOptions(insearch,true,false);
  // arranging
  bsizer->addWidget(main_picker,1);
  bsizer->addWidget(soptions,0);
  setLayout(bsizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
// object i/o
//-------------------------
bool jfFFN_SearchPanel::LoadFrom(jfSearchCore* insearch) {
  const QString fname = "jfFFN_SearchPanel::LoadFrom";
  if (insearch==NULL) return false;
  jfFFNSearch* zsearch = dynamic_cast<jfFFNSearch*>(insearch);
  main_picker->ChangeSelector(zsearch->GetSelector());
  soptions->LoadFromSearch(zsearch);
  return true;
}
//--------------------------
bool jfFFN_SearchPanel::StoreTo(jfSearchCore* outsearch) {
  if (outsearch==NULL) return false;
  jfFFNSearch* ozsearch = dynamic_cast<jfFFNSearch*>(outsearch);
  soptions->SaveToSearch(ozsearch);
  main_picker->SaveFilters();
  return true;
}
//=========================================================================
// the constructor
jfFFN_Panel1::jfFFN_Panel1(jfFFNSearch* insearch, QWidget* parent):jfSearchPanelBase(insearch,true,true,7,parent) {
// constants
  const QString fname="jfFFN_Panel1::jfFFN_Panel1";
  // start
  mpanel = new jfFFN_SearchPanel(insearch);
  SearchPanel = mpanel;
  // sizer arranging
  filt_panel = MakeTypedFilEdit();
  ArrangePanels();
  // finishing off
  ChangeObj(mainobj);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other external methods
bool jfFFN_Panel1::LoadToObj() {
  // local variables
  jfFFNSearch* temp;
  // starting
  if (mainobj==NULL) return false;
  filt_panel->SaveFilters();
  // setting the category selection
  temp = dynamic_cast<jfFFNSearch*>(mainobj);
  SearchPanel->StoreTo(mainobj);
  temp->ApplySelection();
  // done
  return true;
}
//-----------------------------------------------------------------------
bool jfFFN_Panel1::ChangeObj(jfSearchCore* obj_data) {
  const QString fname = "jfFFN_Panel1::ChangeObj";
  // local variables
  jfFFNSearch* temp;
  // starting...
  temp = dynamic_cast<jfFFNSearch*>(obj_data);
  mainobj = obj_data;
  // filters
  filt_panel->ChangeSearch(obj_data);
  mpanel->LoadFrom(temp);
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfDefaultFilterEditorBase* jfFFN_Panel1::MakeTypedFilEdit() {
  jfFFN_DFE* result;
  result = new jfFFN_DFE(dynamic_cast<jfFFNSearch*>(mainobj));
  return result;
}
//**************************************************************************
