/*****************************************************************************
Name:        result_panel.cpp
Author :     John Q Metro
Purpose :    Decfines a panel for displaying search results
Created:     July 26, 2009
Conversion to Qt Started November 20, 2013
Updated:     May 6, 2022 : change multi-download class
******************************************************************************/
#ifndef RESULT_PANEL_H_INCLUDED
  #include "result_panel.h"
#endif // RESULT_LISTBOX_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef PROGRESS_PANEL2_H
  #include "../../fetching/panel/progress_panel2.h"
#endif // PROGRESS_PANEL2_H
//+++++++++++++++++++++++++++++++++++++++++++++++
#include <assert.h>
//*****************************************************************************
// constructor
jfResultDisplay::jfResultDisplay(rtype_enum rinval, header_enum hinval, QWidget* parent):QWidget(parent) {
  // initial values
  started = false;
  p_menu = NULL;
  hype = hinval;
  multicat = (rinval!=rtype_BASIC);
  // thread NULL startups
  search_data = NULL;
  data_collection = NULL;
  download_ptr_multi = NULL;
  download_ptr_single = NULL;
  thread_object = NULL;
  // start construction gui elements
  if (rinval==rtype_BASIC) {
    hype = he_ONECATEGORY;
    mainviewer = new jfHtmlResultCollectionBasic(hinval);
    progpanel = new jfProgressPanelSingle(true);

  }
  else if (rinval==rtype_MCNU){
    hype = hinval;
    mainviewer = new jfHtmlResultCollectionBasic(hinval);
    progpanel = new jfProgPanelMultiple(1);
  }
  else {
    assert(false); // simple is all for now
  }
  // layout
  topsizer = new QVBoxLayout();
  topsizer->addWidget(mainviewer,1);
  topsizer->addWidget(progpanel,0);
  setLayout(topsizer);
  // basic connections
  connect(progpanel, SIGNAL(SendStart()), this, SLOT(HandleStart()));
  connect(progpanel, SIGNAL(SendPanelDone(bool)), this, SLOT(HandleDone(bool)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating the result displays and returning the links
bool jfResultDisplay::LaunchParameters(jfResultCollection* results, jfDownloadRootItems* in_downloader) {
  const QString fname = "jfResultDisplay::LaunchParameters";
  // local variables
  bool svres;
  // setting some values
  svres = SetupValues(results,in_downloader);
  assert(svres);
  // creating the thread object
  thread_object = new QThread();
  ConnectSignalsAndSlots();
  thread_object->start();
  return svres;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// functions to return info that has to be passed to the search thread
//---------------------------------------------------------------------------
jfPauseStop* jfResultDisplay::GetPauser() {
  return progpanel->GetPauser();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfResultDisplay::SetPopupMenu(jfPopupMenu* inmenu) {
  assert(inmenu!=NULL);
  p_menu = inmenu;
  mainviewer->SetPopupMenu(inmenu);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// we call this when we want to clear old results
void jfResultDisplay::ClearOld() {
  mainviewer->ClearViewers();
  progpanel->DoReset();
  started = false;
  disconnect(search_data,0,mainviewer, 0);
  jfDownloadRootItems* ptrx = (multicat)?(download_ptr_multi):(download_ptr_single);
  progpanel->DisConnectThread(ptrx);
  data_collection = NULL;
  download_ptr_multi = NULL;
  download_ptr_single = NULL;
  search_data = NULL;
}
//------------------------------------------------------------------------
// say after pressing a start. we have to halt before actually starting..
void jfResultDisplay::SearchAbort() {
  progpanel->CancelStart();
	started = false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns whether downloading and parsing in in progress
bool jfResultDisplay::InProgress() const {
  return started;
}
//---------------------------------------------------------------------------
size_t jfResultDisplay::SelectedIndex() const {
  return mainviewer->SelectedIndex();
}
//--------------------------------------------------------------------
bool jfResultDisplay::CloseDownload() {
  return progpanel->HaltProgress();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slots
//------------------------------------
void jfResultDisplay::HandleStart() {
  assert(!started);
  ClearOld();
  started = true;
  emit DisplaySendStart();
}
//-------------------------------------------------
void jfResultDisplay::HandleDone(bool) {
  emit DisplaySendDone();
}
//-------------------------------------------------
void jfResultDisplay::ThreadDone() {
  jfDownloadRoot* ptrx = (multicat)?(download_ptr_multi):(download_ptr_single);
  disconnect(thread_object, SIGNAL(started()),ptrx ,SLOT(StartProcessing()));
  disconnect(ptrx,SIGNAL(AllFinished()),thread_object,SLOT(quit()));
  disconnect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  thread_object->deleteLater();
  started = false;
  emit DisplaySendDone();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods for lauch parameters
//---------------------------------------
bool jfResultDisplay::SetupValues(jfResultCollection* results, jfDownloadRootItems* in_downloader) {
  // constants and variables
  const QString fname = "jfResultDisplay::SetupValues";
  bool svres;
  // special checks
  if (!started) return false;
  if (results == NULL) return false;
  if (in_downloader == NULL) return false;
  // setting variables
  data_collection = results;
  search_data = results->GetSearch();
  if (search_data == NULL) return false;
  in_downloader->SetDataAndSearch(results);
  if (multicat) download_ptr_multi = dynamic_cast<jfMultiCatRootDownloader*>(in_downloader);
  else download_ptr_single = in_downloader;
  svres = mainviewer->SetupViewers(search_data->categories);
  return svres;
}

//---------------------------------------
bool jfResultDisplay::ConnectSignalsAndSlots() {
  // constants and variables
  const QString fname = "jfResultDisplay::ConnectSignalsAndSlots";
  jfProgressPanelSingle* typpan1;
  jfProgPanelMultiple* typpan2;
  bool svres;
  // connecting search object signals to the display slots
  connect(search_data, SIGNAL(SendResults(jfResultUnitVector*)),mainviewer, SLOT(HandleNewResults(jfResultUnitVector*)));
  svres = connect(search_data, SIGNAL(DispatchCount(size_t)),mainviewer, SLOT(HandleIncreaseCount(size_t)));
  connect(search_data, SIGNAL(SendCatInfo(struct jfCategoryInfoMsg)),mainviewer, SLOT(HandleNewCategory(struct jfCategoryInfoMsg)));
  // connecting the downloader objects to the progress panel (and adding the associated pauser)
  if (!multicat) {
    typpan1 = dynamic_cast<jfProgressPanelSingle*>(progpanel);
    download_ptr_single->moveToThread(thread_object);
    typpan1->ConnectAndSetPauser(download_ptr_single);
  }
  else {
    typpan2 = dynamic_cast<jfProgPanelMultiple*>(progpanel);
    download_ptr_multi->moveToThread(thread_object);
    typpan2->ConnectAndSetPauser(download_ptr_multi);
  }
  // connecting thread object and downloader
  jfDownloadRootItems* ptrx = (multicat)?(download_ptr_multi):(download_ptr_single);
  connect(thread_object, SIGNAL(started()),ptrx ,SLOT(StartProcessing()));
  connect(ptrx,SIGNAL(AllFinished()),thread_object,SLOT(quit()));
  connect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  // done
  return true;
}

//******************************************************************************
