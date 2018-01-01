 /******************************************************************************
Name    :   fim_mainpanel1.cpp
Author  :   John Q Metro
Purpose :   Defines the main interface for fimfiction.net searching
Created :   June 27, 2012
Conversion to Qt started February 3, 2014
Updated :   February 22, 2014
******************************************************************************/
#ifndef FIM_MAINPANEL1_H_INCLUDED
  #include "fim_mainpanel1.h"
#endif // FIM_MAINPANEL1_H_INCLUDED
//--------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FIM_PANEL1_H_INCLUDED
  #include "fim_panel1.h"
#endif // FIM_PANEL1_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../../initend.h"
#endif // INITEND_H_INCLUDED
#ifndef FIMITEM_THREAD_H
  #include "../../threads/fim/fimitem_thread.h"
#endif // FIMITEM_THREAD_H
#ifndef JFILTERGLOBALS
  #include "../../../core/filters/filter_globals.h"
#endif

#ifndef FICEXTRACT_H_INCLUDED
  #include "../../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED

#ifndef FIM_COLL_H_INCLUDED
  #include "../../data/fim/fim_coll.h"
#endif // FIM_COLL_H_INCLUDED
/*
#ifndef FFNDOWN_DIALOG_H_INCLUDED
  #include "../../../ficdown/interface/ffndown_dialog.h"
#endif // FFNDOWN_DIALOG_H_INCLUDED
*/
//-----------------------------------------
#include <assert.h>

/*****************************************************************************/
// the constructor
jfFIMSearchGrp::jfFIMSearchGrp(jfFIMSearch* searchin, QWidget* parent):jfSearchGrouping(filterglobals::fclist[6],parent) {
  // constants
	const QString fname="jfFIMSearchGrp::jfFIMSearchGrp";
	// starting with creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfFIMSearch*>(searchdata);
  }
  else {
    searchdata = searchin;
    typed_search = searchin;
  }
  // making panel 1
  panel1 = MakeFirstPanel();
  custinit = true;
  // finishing off
  FinishConstruction();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchPanelBase*  jfFIMSearchGrp::MakeFirstPanel(){
  return new jfFIMPanel1(typed_search);
}
//----------------------------------------------------------------------------
jfSearchCore*       jfFIMSearchGrp::MakeTypedSearch() const {
  jfFIMSearch* temp;
  temp = new jfFIMSearch();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);
  temp->categories->base_outputdir = jglobal::settings.GetDirectory(jglobal::FanficSearchResults);
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfFIMSearchGrp::MakeTypedCollection() const {
  assert(custinit);
  return new jfFIMResColl(typed_search);
}
//----------------------------------------------------------------------------
jfBaseItemDownloader* jfFIMSearchGrp::MakeTypedThread() const {
  return new jfFIMItemDownloader(2);
}
/*****************************************************************************/
// the constructor
jfFIMOnePanelSearch::jfFIMOnePanelSearch(jfFIMSearch* searchin, QWidget* parent):jfOnePanelSearch(parent) {
  // constants
	const QString fname="jfFIMOnePanelSearch::jfFIMOnePanelSearch";
	// starting with creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfFIMSearch*>(searchdata);
  }
  else {
    searchdata = searchin;
    typed_search = searchin;
  }
  // making panel 1
  panel1 = MakeFirstPanel();
  custinit = true;
  // finishing off
  FinishConstruction();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchPanelBase*  jfFIMOnePanelSearch::MakeFirstPanel(){
  return new jfFIMPanel1(typed_search);
}
//----------------------------------------------------------------------------
jfSearchCore*       jfFIMOnePanelSearch::MakeTypedSearch() const {
  jfFIMSearch* temp;
  temp = new jfFIMSearch();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);
  temp->categories->base_outputdir = jglobal::settings.GetDirectory(jglobal::FanficSearchResults);
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfFIMOnePanelSearch::MakeTypedCollection() const {
  assert(custinit);
  return new jfFIMResColl(typed_search);
}
//----------------------------------------------------------------------------
jfBaseItemDownloader* jfFIMOnePanelSearch::MakeTypedThread() const {
  return new jfFIMItemDownloader(3);
}
/*****************************************************************************/
jfFIMMain::jfFIMMain(jfSearchCore* searchin, bool notebook, QWidget* parent):
      jfMainSearchGroup(searchin,rtype_BASIC,notebook,false,parent) {
  // the usual constant
	const QString fname = "jfFIMMain::jfFIMMain";
  // preparing notebook and results display
  if (noteb) container = new QTabWidget();
  else container = NULL;
  r_panel = MakeResultGroup(false,rtype_BASIC);
	// making the custom panel
  p_search = MakeSearchGroup(searchin,threepanel);
  ConsStepTwo(filterglobals::fclist[6]);
  TopArranger();
  rc_menu = new jfFanficPopupMenu();
  connect(rc_menu,SIGNAL(sendDescription(QString)),this,SLOT(HandleAppendText(QString)));
  r_panel->SetPopupMenu(rc_menu);
  rc_menu = NULL;
  ConnectResult();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual method
//-------------------------------------------
jfSearchGroupingCore* jfFIMMain::MakeSearchGroup(jfSearchCore* xnewsearch, bool threepanels) {
  // local variables
	const QString fname = "jfFIMMain::MakeSearchGroup";
  jfSearchGroupingCore* result;
  jfFIMSearch* temp;
  // starting
  if (xnewsearch!=NULL) temp = dynamic_cast<jfFIMSearch*>(xnewsearch);
  else temp = NULL;
  // creating the panel
  if (threepanels) result = new jfFIMOnePanelSearch(temp);
  else result = new jfFIMSearchGrp(temp);
  typed_mpanel = result;
  return result;
}
//---------------------------------------------------------------------------
jfResultDisplay* jfFIMMain::MakeResultGroup(bool multi_cat, rtype_enum rinval, QWidget* inparent) {
  return new jfResultDisplay(rinval,he_ONECATEGORY,inparent);
}
/*****************************************************************************/
