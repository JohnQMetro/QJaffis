/******************************************************************************
Name    :   fimgroup_mainpanel1.cpp
Author  :   John Q Metro
Purpose :   Defines the main interface for fimfiction.net group searching
Created :   August 10, 2013
// conversion to Qt satrted August 4, 2015
Updated :   March 27, 2022
******************************************************************************/
#ifndef FIMGROUP_MAINPANEL1_H_INCLUDED
  #include "fimgroup_mainpanel1.h"
#endif // FIMGRIUP_MAINPANEL1_H_INCLUDED
//--------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FIMGROUP_PANEL1_H_INCLUDED
  #include "fimgroup_panel1.h"
#endif // FIMGROUP_PANEL1_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../../initend.h"
#endif // INITEND_H_INCLUDED
#ifndef JFILTERGLOBALS
  #include "../../../core/filters/filter_globals.h"
#ifndef FIMGROUP_THREAD_H
#include "../../threads/fim/fimgroup_thread.h"
#endif // FIMGROUP_THREAD_H
#endif

#include <assert.h>

/*****************************************************************************/
// the constructor
jfFIMGroupSearchGrp::jfFIMGroupSearchGrp(jfFIMGroupSearch* searchin, QWidget* parent):jfSearchGrouping(filterglobals::fclist[8],parent) {
  // constants
  const QString fname = "jfFIMGroupSearchGrp::jfFIMGroupSearchGrp";
  // starting with creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfFIMGroupSearch*>(searchdata);
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
jfSearchPanelBase*  jfFIMGroupSearchGrp::MakeFirstPanel(){
  return new jfFIMGroupPanel1(typed_search);
}
//----------------------------------------------------------------------------
jfSearchCore*       jfFIMGroupSearchGrp::MakeTypedSearch() const {
  jfFIMGroupSearch* temp;
  temp = new jfFIMGroupSearch();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);
  QString search_path = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
  temp->categories->base_outputdir = search_path;
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfFIMGroupSearchGrp::MakeTypedCollection() const {
  assert(custinit);
  return new jfFIMGroupResColl(typed_search);
}
//----------------------------------------------------------------------------
jfDownloadRootItems* jfFIMGroupSearchGrp::MakeTypedThread() const {
  return new jfFIMGroup_DownThread();
}
/*****************************************************************************/
// the constructor
jfFIMGroupOnePanelSearch::jfFIMGroupOnePanelSearch(jfFIMGroupSearch* searchin, QWidget* parent):jfOnePanelSearch(parent) {
  // constants
  const QString fname = "jfFIMGroupOnePanelSearch::jfFIMGroupOnePanelSearch";
  // starting with creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfFIMGroupSearch*>(searchdata);
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
jfSearchPanelBase*  jfFIMGroupOnePanelSearch::MakeFirstPanel(){
  return new jfFIMGroupPanel1(typed_search);
}
//----------------------------------------------------------------------------
jfSearchCore*       jfFIMGroupOnePanelSearch::MakeTypedSearch() const {
  jfFIMGroupSearch* temp;
  temp = new jfFIMGroupSearch();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);
  QString search_path = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
  temp->categories->base_outputdir = search_path;
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfFIMGroupOnePanelSearch::MakeTypedCollection() const {
  assert(custinit);
  return new jfFIMGroupResColl(typed_search);
}
//----------------------------------------------------------------------------
jfDownloadRootItems* jfFIMGroupOnePanelSearch::MakeTypedThread() const {
  return new jfFIMGroup_DownThread();
}
/*****************************************************************************/
jfFIMGroupMain::jfFIMGroupMain(jfSearchCore* searchin, bool notebook, QWidget* parent):
                                jfMainSearchGroup(searchin,notebook, parent ) {
  // the usual constant
  const QString fname = "jfFIMGroupMain::jfFIMGroupMain";

  // preparing notebook and results display
  if (noteb) container = new QTabWidget();
  else container = NULL;
  r_panel = MakeResultGroup(false,rtype_BASIC);
        // making the custom panel
  p_search = MakeSearchGroup(searchin,threepanel);
  ConsStepTwo(filterglobals::fclist[8]);
  TopArranger();
  rc_menu = new jfFIMGPopupMenu();
  connect(rc_menu,SIGNAL(sendDescription(QString)),this,SLOT(HandleAppendText(QString)));
  r_panel->SetPopupMenu(rc_menu);
  rc_menu = NULL;
  ConnectResult();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual method
//-------------------------------------------
jfSearchGroupingCore* jfFIMGroupMain::MakeSearchGroup(jfSearchCore* xnewsearch, bool threepanels) {
  // local variables
  const QString fname = "jfFIMGroupMain::MakeSearchGroup";
  jfSearchGroupingCore* result;
  jfFIMGroupSearch* temp;
  // starting
  if (xnewsearch!=NULL) temp = dynamic_cast<jfFIMGroupSearch*>(xnewsearch);
  else temp = NULL;
  // creating the panel
  if (threepanels) result = new jfFIMGroupOnePanelSearch(temp);
  else result = new jfFIMGroupSearchGrp(temp);
  typed_mpanel = result;
  return result;
}
//---------------------------------------------------------------------------
jfResultDisplay* jfFIMGroupMain::MakeResultGroup( bool multi_cat, rtype_enum rinval, QWidget* inparent) {
  return new jfResultDisplay(rinval,he_ONECATEGORY,inparent);
}
/*****************************************************************************/
