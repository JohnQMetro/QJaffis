//**************************************************************************
// Name:        ao3_mainpanel.cpp
// Author :     John Q Metro
// Purpose :    Archiveofourown.org search interface
// Created:     September 3, 2012
// Conversion to Qt started April 13, 2014
// Updated:     April 3, 2022
//**************************************************************************
#ifndef AO3_MAINPANEL_H_INCLUDED
  #include "ao3_mainpanel.h"
#endif // AO3_MAINPANEL_H_INCLUDED
//-----------------------------------------------
#ifndef AO3_SEARCHFILTER_PANEL_H
    #include "ao3_searchfilter_panel.h"
#endif // AO3_SEARCHFILTER_PANEL_H
#ifndef JFILTERGLOBALS
  #include "../../../core/filters/filter_globals.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../../initend.h"
#endif // INITEND_H_INCLUDED
#ifndef AO3_PANEL1_H_INCLUDED
  #include "ao3_panel1.h"
#endif // AO3_PANEL1_H_INCLUDED
#ifndef AO3_COLL_H_INCLUDED
  #include "../../data/ao3/ao3_coll.h"
#endif // AO3_COLL_H_INCLUDED
#ifndef AO3ITEM_THREAD_H
  #include "../../threads/ao3/ao3item_thread.h"
#endif // AO3ITEM_THREAD_H
#ifndef AO3_SEARCHFILTER_PANEL_H
    #include "ao3_searchfilter_panel.h"
#endif // AO3_SEARCHFILTER_PANEL_H
//------------------------------------------
#include <assert.h>

//*************************************************************************
// the constructor
jfAO3SearchGrp::jfAO3SearchGrp(jfAO3Search* searchin, jfMainSearchGroup* holder, QWidget* parent):
          jfSearchGrouping(filterglobals::fclist[7],parent) {
  // constants
	const QString fname="jfAO3SearchGrp::jfAO3SearchGrp";
	// getting the AO3 search index
  sval = ao3_catdata::ao3_catmanager->RegisterSearch(holder);
  // the creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfAO3Search*>(searchdata);
  }
  else {
    searchdata = searchin;
    typed_search = searchin;
  }
  typed_search->SetData(ao3_catdata::ao3_catmanager->GetData(), sval);
  // making panel 1
  search_and_filter_panel = MakeFirstPanel();
  custinit = true;
  // finishing off
  FinishConstruction();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchPanelRoot*  jfAO3SearchGrp::MakeFirstPanel(){
  return new jfAO3_SearchFilterPanel(typed_search);
}
//----------------------------------------------------------------------------
jfSearchCore*       jfAO3SearchGrp::MakeTypedSearch() const {
  jfAO3Search* temp;
  temp = new jfAO3Search();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);

  QString bod = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
  temp->categories->base_outputdir = bod;
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfAO3SearchGrp::MakeTypedCollection() const {
  assert(custinit);
  return new jfAO3ResColl(typed_search);
}
//----------------------------------------------------------------------------
jfDownloadRootItems* jfAO3SearchGrp::MakeTypedThread() const {
  return new jfAO3FandomItemDownloader();
}
//----------------------------------------------------------------------------
int jfAO3SearchGrp::GetSearchIndex() const {
  return sval;
}
//*************************************************************************
// the constructor
jfAO3OnePanelSearch::jfAO3OnePanelSearch(jfAO3Search* searchin, jfMainSearchGroup* holder,
                  QWidget* parent):jfOnePanelSearch(parent) {
  // constants
	const QString fname="jfAO3OnePanelSearch::jfAO3OnePanelSearch";
	// getting the AO3 search index
  sval = ao3_catdata::ao3_catmanager->RegisterSearch(holder);
  // the creation of the search object
  if (searchin==NULL) {
    searchdata = MakeTypedSearch();
    typed_search = dynamic_cast<jfAO3Search*>(searchdata);
  }
  else {
    searchdata = searchin;
    typed_search = searchin;
  }
  typed_search->SetData(ao3_catdata::ao3_catmanager->GetData(), sval);
  // making panel 1
  search_and_filter_panel = MakeFirstPanel();
  custinit = true;
  // finishing off
  FinishConstruction();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchPanelRoot*  jfAO3OnePanelSearch::MakeFirstPanel(){
  return new jfAO3_SearchFilterPanel(typed_search);
}
//----------------------------------------------------------------------------
jfSearchCore*       jfAO3OnePanelSearch::MakeTypedSearch() const {
  jfAO3Search* temp;
  temp = new jfAO3Search();
  temp->MakeEmpty();
  temp->categories->SetUseDefault(true);  
  QString bod = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
  temp->categories->base_outputdir = bod;
  return temp;
}
//----------------------------------------------------------------------------
jfResultCollection* jfAO3OnePanelSearch::MakeTypedCollection() const {
  assert(custinit);
  return new jfAO3ResColl(typed_search);
}
//----------------------------------------------------------------------------
jfDownloadRootItems* jfAO3OnePanelSearch::MakeTypedThread() const {
  return new jfAO3FandomItemDownloader();
}
//----------------------------------------------------------------------------
int jfAO3OnePanelSearch::GetSearchIndex() const {
  return sval;
}
/*****************************************************************************/
jfAO3Main::jfAO3Main(jfSearchCore* searchin, bool notebook, QWidget* parent):
        jfMainSearchGroup(searchin, rtype_MCNU, notebook, true,  parent ) {
  // the usual constant
  const QString fname = "jfAO3Main::jfAO3Main";
  // preparing notebook and results display
  r_panel = MakeResultGroup(true,rtype_MCNU,NULL);
  if (noteb) container = new QTabWidget();
  else container = NULL;
  // threepanel = false;
  p_search = MakeSearchGroup(searchin,threepanel);
  // the middle panel
  ConsStepTwo(filterglobals::fclist[7]);
  TopArranger();
  rc_menu = new jfFanficPopupMenu();
  connect(rc_menu,SIGNAL(sendDescription(QString)),this,SLOT(HandleAppendText(QString)));
  r_panel->SetPopupMenu(rc_menu);
  ConnectResult();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
size_t jfAO3Main::GetSCatIndex() const {
  jfAO3Search* temp = dynamic_cast<jfAO3Search*>(search_data);
  return temp->GetSSIndex();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
jfSearchGroupingCore* jfAO3Main::MakeSearchGroup(jfSearchCore* xnewsearch, bool threepanels) {
  // local variables
	const QString fname = "jfAO3Main::MakeSearchGroup";
  jfSearchGroupingCore* result;
  jfAO3Search* temp;
  // starting
  if (xnewsearch!=NULL) temp = dynamic_cast<jfAO3Search*>(xnewsearch);
  else temp = NULL;
  // creating the panel
  if (threepanels) result = new jfAO3OnePanelSearch(temp,this);
  else result = new jfAO3SearchGrp(temp,this);
  typed_mpanel = result;
  return result;
}
//-------------------------------------------

jfResultDisplay* jfAO3Main::MakeResultGroup( bool multi_cat, rtype_enum rinval, QWidget* inparent) {
  return new jfResultDisplay(rinval,he_NONEMPTY,inparent);
}
//**************************************************************************

