/******************************************************************************
Name    :   mainsearch.cpp
Author  :   John Q Metro
Purpose :   Defines the top level base class for the Seacrh+Results interface
Created :   August 23, 2009 (orginal version)
Conversion to Qt Started January 25, 2014 (and renamed from display_base2.cpp)
Updated :   March 30, 2023
******************************************************************************/
#ifndef MAINSEARCH_H
  #include "mainsearch.h"
#endif // MAINSEARCH_H
//------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//------------------------------------
#include <QApplication>
#include <assert.h>
#include <QMessageBox>
/*****************************************************************************/
// we need a constructor
DisplayPanelCore::DisplayPanelCore(QWidget* parent):QWidget(parent) {
}
/*****************************************************************************/
// the defult constructor
jfMainSearchGroup::jfMainSearchGroup(jfSearchCore* searchin, bool notebook, QWidget* parent):DisplayPanelCore(parent) {
  // starting preps
  p_search = NULL;
  noteb = notebook;
  search_data = searchin;
  thread_holder = NULL;
  results = NULL;
  launched = false;
  beensaved = false;
  rc_menu = NULL;
  threepanel = true;
  panel_two = NULL;
  // the search data has to be completed before construction is done
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns whether downloading and parsing in in progress
bool jfMainSearchGroup::InProgress() const {
  return r_panel->InProgress();
}
//---------------------------------------------------------------------------
QString jfMainSearchGroup::GetSearchName() const {
  if (search_data==NULL) return "";
  else return search_data->GetName();
}
//-------------------------------------------------------------------------
QString jfMainSearchGroup::GetDisplayName() const {
  return nedit->GetName();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// doing output
//-------------------------------------------------------------------------
bool jfMainSearchGroup::WriteAll2Html() {
  QApplication::setOverrideCursor(Qt::WaitCursor);
  if ((search_data->categories)->UsesDefault()) {
    results->WriteToHTML(32);
  }
  size_t ccount = (search_data->categories)->GetCount();
  if (ccount!=0) {
    for (size_t cloop=0;cloop<ccount;cloop++) {
        results->WriteToHTML(cloop);
    }
  }
  QApplication::restoreOverrideCursor();
  return true;
}
//-------------------------------------------------------------------------
bool jfMainSearchGroup::WriteCurrent2Html() {
  QApplication::setOverrideCursor(Qt::WaitCursor);
  size_t curr_res = r_panel->SelectedIndex();
  if ((search_data->categories)->UsesDefault()) {
    if (curr_res==0) curr_res = 32;
    else curr_res--;
  }
  results->WriteToHTML(curr_res);
  QApplication::restoreOverrideCursor();
  return true;
}
//---------------------------------------------------------------------------
// saves the current contents to a data file
bool jfMainSearchGroup::SaveToFile(QString fname) {
  const QString ifname = "jfMainSearchGroup::SaveToFile";
  // local variables
  QTextStream* outfile;
  QString xname;
  bool usd_res;
  // starting
  assert(search_data!=NULL);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  xname = nedit->TryGetName();
  search_data->SetName(xname);
  usd_res = p_search->UpdateSearchData();
  if (usd_res && threepanel) {
    usd_res = panel_two->UpdateSearchData();
  }
  if (!usd_res) {
      QApplication::restoreOverrideCursor();
      return false;
  }
  // file opening
  outfile = GetOpenFile(fname,true);
  // we have the file written
  search_data->AddToFile(outfile);
  // saving file data
  CloseFileStream(outfile,true);
  savfpath = fname;
  beensaved = true;
  // done
  QApplication::restoreOverrideCursor();
  return true;
}
//--------------------------------------------------------------------------
bool jfMainSearchGroup::SaveToFile() {
  if (!beensaved) return false;
  return SaveToFile(savfpath);
}
//--------------------------------------------------------------------------
bool jfMainSearchGroup::HasBeenSaved() const {
  return beensaved;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchCore* jfMainSearchGroup::GetSearchPointer() {
  return search_data;
}
//---------------------------------------------------------------
bool jfMainSearchGroup::CloseSearch() {
  return r_panel->CloseDownload();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfMainSearchGroup::HandleDisplayStart() {
  // constants and variables
  const QString fname="jfMainSearchGroup::HandleDisplayStart";
  bool lres;
  // starting
  ClearOldData();
  launched = true;
  starttime = QDateTime::currentDateTime();
  // we get the search data, if possible
  if (!BuildSearch()) {
    r_panel->SearchAbort();
    // if BuildSearch returns false we abort the search
    launched = false;
  }
  // here, no error has happened, we go ahead with the thread
  else {
    emit SendStartingSignal();
    MakeThreadCollection();
    lres = r_panel->LaunchParameters(results,thread_holder);
    assert(lres);
  }
}
//---------------------------------------------------
// what we do for this is to signal the topmost window
void jfMainSearchGroup::HandleDisplayDone() {
  emit SendDoneSignal(true);
}
//----------------------------------------------------
// handles the append text right click event
void jfMainSearchGroup::HandleAppendText(const QString text2append) {
  // constants
  const QString req_msg = "Edit the text to what you want.";
  const QString req_cap = "Append to Expression Filter";
  const QString inc_msg = "The resulting expression is invalid! Try again?";
  const QString inc_cap = "Result is invalid!";
}
//----------------------------------------------------
void jfMainSearchGroup::HandleNameChange(QString newname) {
  if (threepanel) panel_two->NameChangeForCategory(newname);
  else {
    jfSearchGrouping* ptyp = dynamic_cast<jfSearchGrouping*>(p_search);
    ptyp->NameChangeForCategory(newname);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* finishes construction, should be called by a constructor of a child class
after the calling of (their implemented version of) MakeSearchGroup */
void jfMainSearchGroup::TopArranger() {
  const QString fname = "jfMainSearchGroup::TopArranger";

  top_sizer = new QHBoxLayout();
  if (noteb) {
    if (threepanel) {
      container->addTab(p_search,"Parameters");
      container->addTab(panel_two,"Filters and Categories");
    }
    else {
      container->addTab(p_search,"Params and Filters");
    }
    container->addTab(r_panel,"Results");
    top_sizer->addWidget(container,1);
    nedit->setParent(container);
    container->setCornerWidget(nedit);

  }
  else {
    // nedit??
    top_sizer->addWidget(p_search,1);
    if (threepanel) top_sizer->addWidget(panel_two,1);
    top_sizer->addWidget(r_panel,1);
  }
  setLayout(top_sizer);
  connect(nedit,SIGNAL(SendNewName(QString)),this,SLOT(HandleNameChange(QString)));
 }
//---------------------------------------------------------------------------
// creates (does not launch) the search thread and result collection
void jfMainSearchGroup::MakeThreadCollection() {
  // making the thread
  thread_holder = p_search->MakeTypedThread();
  // making the collection and loading it into the thread
  results = p_search->MakeTypedCollection();
  // thread_holder->AddSearchCollection(results);
}
//-----------------------------------------------------------------------------
bool jfMainSearchGroup::BuildSearch() {
  const QString fname = "jfMainSearchGroup::BuildSearch";
  // variables and asserts
  assert(launched);
  bool rval;
  QString oval, sname;
  // getting the name
  sname = nedit->TryGetName();
  if (sname.isEmpty()) return false;
  // saving the rest of the search
  if (!(p_search->UpdateSearchData())) return false;
  if (threepanel) {
    if (!(panel_two->UpdateSearchData())) return false;
  }
  // verifying and checking filters
  rval = search_data->VerifyNames(oval);
  if (rval) search_data->CopyLoad();
  else QMessageBox::critical(this,"Name Verification Failed!",oval);
  // setting the name
  search_data->SetName(sname);
  return rval;
}
//-----------------------------------------------------------------------------
// when we want to re-do a search, we call this first to clear out the old data
void jfMainSearchGroup::ClearOldData() {
  const QString fname = "jfMainSearchGroup::ClearOldData";
  // resetting the result display
  // iirc, the thread object self-deletes
  if (thread_holder!=NULL) thread_holder = NULL;
  // deleting the old results
  if (results!=NULL) {
    delete results;
    results = NULL;
  }
  // setting a flag
  launched = false;
}
//-------------------------------------------------------------------------
QString jfMainSearchGroup::GSearchName() const {
  if (p_search==NULL) return "";
  else return p_search->GetSearchObj()->GetName();
}
//-------------------------------------------------------------------------------
bool jfMainSearchGroup::ConsStepTwo(const QStringList* infclist) {
  const QString fname = "jfMainSearchGroup::ConsStepTwo";
  // asserts and checks
  jerror::AssertLog(p_search!=NULL,fname,"seatch object is NULL");
  assert(infclist);
  // assifgning search data
  search_data = p_search->GetSearchObj();
  if (noteb) {
      /**/JDEBUGLOG(fname,2)
    nedit =  new jfNameEdit("Name for this Search",search_data->GetName(),5);
    int sh = 30;
    nedit->setMinimumHeight(sh);
    /**/JDEBUGLOGI(fname,2,sh);
    nedit->setMinimumWidth(500);
    /**/JDEBUGLOGI(fname,3,(nedit->sizeHint()).width());
    // nedit->setFixedWidth(300);
    QTabBar* tb = container->findChild<QTabBar *>(QLatin1String("qt_tabwidget_tabbar"));
    tb->setObjectName("SearchTopTabBar");
    QString ssheet = "QTabBar#SearchTopTabBar::tab { height:" + QString::number(sh+8) + "px; }";
    container->setStyleSheet(ssheet);
  }
  else {
    /**/JDEBUGLOG(fname,4)
    nedit =  new jfNameEdit("Search Name",search_data->GetName());
      /**/JDEBUGLOG(fname,5)
  }
  /**/JDEBUGLOG(fname,6)
  // the middle panel
  if (threepanel) panel_two = new jfPanelTwo(search_data,infclist);
  return true;
}
//-----------------------------------------------------------------------
bool jfMainSearchGroup::ConnectResult() {
  if (r_panel==NULL) return false;
  connect(r_panel,SIGNAL(DisplaySendStart()),this,SLOT(HandleDisplayStart()));
  connect(r_panel,SIGNAL(DisplaySendDone()),this,SLOT(HandleDisplayDone()));
  return true;
}

/*****************************************************************************/

