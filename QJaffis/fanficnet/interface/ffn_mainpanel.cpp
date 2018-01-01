//**************************************************************************
// Name:        ffn_mainpanel.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net search interface
// Created:     July 1, 2010
// conversion to Qt started July 29, 2014
// Updated:     August 26, 2014
//**************************************************************************
#ifndef FFN_MAINPANEL
  #include "ffn_mainpanel.h"
#endif
#ifndef FFN_PANEL1
  #include "ffn_panel1.h"
#endif
#ifndef JFILTERGLOBALS
  #include "../../core/filters/filter_globals.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED
#ifndef FFN_MULTICOLL
  #include "../data/ffn_mulcoll.h"
#endif
#ifndef FFNITEMS_THREAD_H
  #include "../threads/ffnitems_thread.h"
#endif // FFNITEMS_THREAD_H
//-----------------------------------------
#include <assert.h>
//**************************************************************************
// the constructor
jfFFNSearchGrp::jfFFNSearchGrp(jfFFNSearch* searchin, jfFFNMain* tholder, QWidget* parent):
            jfSearchGrouping(filterglobals::fclist[5],parent) {
  // constants
	const QString fname="jfFFNSearchGrp::jfFFNSearchGrp";
	// getting the ffn search index
  sval = ffn_catorganizer::main_manager->RegisterSearch(tholder);
  // the creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfFFNSearch*>(searchdata);
  }
  else {
    searchdata = searchin;
    typed_search = searchin;
  }
  /**/JDEBUGLOGB(fname,2,searchdata==NULL);
  typed_search->SetData(ffn_catorganizer::main_manager->GetHolder(), sval);
  typed_search->GetSelector()->SetCatLink(ffn_catorganizer::main_manager->GetHolder());
  // making panel 1
  /**/JDEBUGLOG(fname,3);
  panel1 = MakeFirstPanel();
  custinit = true;
  // finishing off
  FinishConstruction();
  /**/JDEBUGLOG(fname,4);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchPanelBase*  jfFFNSearchGrp::MakeFirstPanel() {
  return new jfFFN_Panel1(typed_search);
}
//----------------------------------------------------------------------------
jfSearchCore*       jfFFNSearchGrp::MakeTypedSearch() const {
  jfFFNSearch* temp;
  temp = new jfFFNSearch();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);
  temp->categories->base_outputdir = jglobal::settings.GetDirectory(jglobal::FanficSearchResults);
  temp->SetData(ffn_catorganizer::main_manager->GetHolder(),sval);
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfFFNSearchGrp::MakeTypedCollection() const {
  assert(custinit);
  return new jfFFNResColl(typed_search);
}
//----------------------------------------------------------------------------
jfBaseItemDownloader* jfFFNSearchGrp::MakeTypedThread() const {
  return new jfFFNDownloader(3);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int jfFFNSearchGrp::GetSearchIndex() const {
  return sval;
}
//***************************************************************************
//==============================================================================
// the constructor
jfFFNSearchOnePanel::jfFFNSearchOnePanel(jfFFNSearch* searchin, jfFFNMain* tholder,
           QWidget* parent):jfOnePanelSearch(parent) {
  // constants
	const QString fname = "jfFFNSearchOnePanel::jfFFNSearchOnePanel";
	// getting the ffn search index
  sval = ffn_catorganizer::main_manager->RegisterSearch(tholder);
  // the creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfFFNSearch*>(searchdata);
  }
  else {
    searchdata = searchin;
    typed_search = searchin;
  }
  typed_search->SetData(ffn_catorganizer::main_manager->GetHolder(), sval);
  typed_search->GetSelector()->SetCatLink(ffn_catorganizer::main_manager->GetHolder());
  // making panel 1
  panel1 = MakeFirstPanel();
  custinit = true;
  // finishing off
  FinishConstruction();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom  methods
//---------------------------------------
jfSearchPanelBase*  jfFFNSearchOnePanel::MakeFirstPanel() {
  return new jfFFN_Panel1(typed_search);
}
//--------------------------------------
jfSearchCore*       jfFFNSearchOnePanel::MakeTypedSearch() const {
  jfFFNSearch* temp;
  temp = new jfFFNSearch();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);
  temp->categories->base_outputdir = jglobal::settings.GetDirectory(jglobal::FanficSearchResults);
  temp->SetData(ffn_catorganizer::main_manager->GetHolder(),sval);
  return temp;
}
//--------------------------------------
jfResultCollection* jfFFNSearchOnePanel::MakeTypedCollection() const {
    assert(custinit);
  return new jfFFNResColl(typed_search);
}
//--------------------------------------
jfBaseItemDownloader* jfFFNSearchOnePanel::MakeTypedThread() const {
  return new jfFFNDownloader(3);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
int jfFFNSearchOnePanel::GetSearchIndex() const {
  return sval;
}
/*****************************************************************************/
jfFFNMain::jfFFNMain(jfSearchCore* searchin, bool notebook, QWidget* parent):
    jfMainSearchGroup( searchin , rtype_MCNU , notebook , true , parent ) {
  // the usual constant
	const QString fname = "jfFFNMain::jfFFNMain";
  // preparing notebook and results display
  r_panel = MakeResultGroup(true,rtype_MCNU,NULL);
  if (noteb) container = new QTabWidget();
  else container = NULL;
  /**/JDEBUGLOGB(fname,2,searchin==NULL);
  p_search = MakeSearchGroup(searchin,threepanel);
  ConsStepTwo(filterglobals::fclist[5]);
  JDEBUGLOGS(fname,3,GSearchName())
  TopArranger();
  JDEBUGLOG(fname,4);
  rc_menu = new jfFanficPopupMenu();
  JDEBUGLOG(fname,5);
  connect(rc_menu,SIGNAL(sendDescription(QString)),this,SLOT(HandleAppendText(QString)));
  r_panel->SetPopupMenu(rc_menu);
  JDEBUGLOG(fname,6);
  ConnectResult();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
size_t jfFFNMain::GetSCatIndex() const {
  jfFFNSearch* temp = dynamic_cast<jfFFNSearch*>(search_data);
  return temp->GetSSIndex();
}
//--------------------------------------------------------------------------------
// handles producting updatatble results
bool jfFFNMain::WriteFFNBundle(QString inpath) const {
  // variables and constants
  const QString fname = "jfFFNMain::WriteFFNBundle";
  /*
  jfFFN_UpdateBundle* updat_result;
  QTextStream* outfile;
  // building the result bundle
  updat_result = new jfFFN_UpdateBundle();
  updat_result->search_data = dynamic_cast<jfFFNSearch*>(search_data);
  updat_result->result_data = dynamic_cast<jfFFNResColl*>(results);
  updat_result->lastdate = starttime;
  // opening or creating the output text file
  outfile = GetOpenFile(inpath,true);
  // we produce the result
  updat_result->AddToFile(outfile);
  CloseFileStream(outfile,true);
  */
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
jfSearchGroupingCore* jfFFNMain::MakeSearchGroup(jfSearchCore* xnewsearch, bool threepanels) {
  // local variables
	const QString fname = "jfFFNMain::MakeSearchGroup";
  jfSearchGroupingCore* result;
  jfFFNSearch* temp;
  // starting
  if (xnewsearch!=NULL) temp = dynamic_cast<jfFFNSearch*>(xnewsearch);
  else temp = NULL;
  // creating the panel
  if (threepanels) result = new jfFFNSearchOnePanel(temp,this);
  else result = new jfFFNSearchGrp(temp,this);
  typed_mpanel = result;
  // done
  return result;
}
//-------------------------------------------
jfResultDisplay* jfFFNMain::MakeResultGroup(bool multi_cat, rtype_enum rinval,QWidget* inparent) {
  return new jfResultDisplay(rinval,he_NONEMPTY,inparent);
}

//**************************************************************************

