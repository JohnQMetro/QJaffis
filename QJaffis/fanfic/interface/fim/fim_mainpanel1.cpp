 /******************************************************************************
Name    :   fim_mainpanel1.cpp
Author  :   John Q Metro
Purpose :   Defines the main interface for fimfiction.net searching
Created :   June 27, 2012
Conversion to Qt started February 3, 2014
Updated :   June 4, 2022
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
  search_and_filter_panel = MakeFirstPanel();
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
  QString search_path = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
  temp->categories->base_outputdir = search_path;
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfFIMSearchGrp::MakeTypedCollection() const {
  assert(custinit);
  return new jfFIMResColl(typed_search);
}
//----------------------------------------------------------------------------
jfDownloadRootItems* jfFIMSearchGrp::MakeTypedThread() const {
  return new jfFIMItemDownloader();
}
/*****************************************************************************/
// the constructor
jfFIMOnePanelSearch::jfFIMOnePanelSearch(jfFIMSearch* searchin, QWidget* parent):jfOnePanelSearch(parent) {
  // constants
	const QString fname="jfFIMOnePanelSearch::jfFIMOnePanelSearch";
	// starting with creation of the search object
    /**/JDEBUGLOG(fname,1)
  if (searchin==NULL) {
        /**/JDEBUGLOG(fname,2)
    searchdata = MakeTypedSearch();
        /**/JDEBUGLOG(fname,3)
    typed_search = dynamic_cast<jfFIMSearch*>(searchdata);
  }
  else {
        /**/JDEBUGLOG(fname,4)
    searchdata = searchin;
    typed_search = searchin;
  }
    /**/JDEBUGLOG(fname,5)
  // making panel 1
  search_and_filter_panel = MakeFirstPanel();
    /**/JDEBUGLOG(fname,6)
  custinit = true;
  // finishing off
  FinishConstruction();
  /**/JDEBUGLOG(fname,7)
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
  QString search_path = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
  temp->categories->base_outputdir = search_path;
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfFIMOnePanelSearch::MakeTypedCollection() const {
  assert(custinit);
  return new jfFIMResColl(typed_search);
}
//----------------------------------------------------------------------------
jfDownloadRootItems* jfFIMOnePanelSearch::MakeTypedThread() const {
  return new jfFIMItemDownloader();
}
/*****************************************************************************/
jfFIMMain::jfFIMMain(jfSearchCore* searchin, bool notebook, QWidget* parent):
                                    jfMainSearchGroup(searchin,notebook,parent) {
  // the usual constant
	const QString fname = "jfFIMMain::jfFIMMain";
  // preparing notebook and results display
    /**/JDEBUGLOG(fname,1)
  if (noteb) container = new QTabWidget();
  else container = NULL;
    /**/JDEBUGLOG(fname,2)
  r_panel = MakeResultGroup(false,rtype_BASIC);
	// making the custom panel
    /**/JDEBUGLOG(fname,3)
  p_search = MakeSearchGroup(searchin,threepanel);
    /**/JDEBUGLOG(fname,4)
  ConsStepTwo(filterglobals::fclist[6]);
    /**/JDEBUGLOG(fname,5)
  TopArranger();
    /**/JDEBUGLOG(fname,6)
  rc_menu = new jfFanficPopupMenu();
    /**/JDEBUGLOG(fname,7)
  connect(rc_menu,SIGNAL(sendDescription(QString)),this,SLOT(HandleAppendText(QString)));
    /**/JDEBUGLOG(fname,8)
  r_panel->SetPopupMenu(rc_menu);
  rc_menu = NULL;
  /**/JDEBUGLOG(fname,9)
  ConnectResult();
  /**/JDEBUGLOG(fname,10)
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
  /**/JDEBUGLOG(fname,1)
  if (xnewsearch!=NULL) temp = dynamic_cast<jfFIMSearch*>(xnewsearch);
  else temp = NULL;
  // creating the panel
  /**/JDEBUGLOGB(fname,2,threepanels)
  if (threepanels) result = new jfFIMOnePanelSearch(temp);
  else result = new jfFIMSearchGrp(temp);
  /**/JDEBUGLOG(fname,3)
  typed_mpanel = result;
  return result;
}
//---------------------------------------------------------------------------
jfResultDisplay* jfFIMMain::MakeResultGroup(bool multi_cat, rtype_enum rinval, QWidget* inparent) {
  return new jfResultDisplay(rinval,he_ONECATEGORY,inparent);
}
/*****************************************************************************/
