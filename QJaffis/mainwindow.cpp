//***************************************************************************
// Name:        mainwindow.cpp
// Author :     John Q Metro
// Purpose :    Guess
// Created:     By QtCreator, forgot when
// Updated:     March 27, 2022
//***************************************************************************
#ifndef MAINWINDOW_H
  #include "mainwindow.h"
#endif // MAINWINDOW_H
//----------------------------------------
#ifndef MW_INCLUDES_H
  #include "mw_includes.h"
#endif // MW_INCLUDES_H
//----------------------------------------
#include <QSize>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <assert.h>
#include <QMessageBox>
#include <QScreen>
#include <iostream>

//***************************************************************************
// stores action links so I do not have to use changeable indexes to disable or enable
namespace mac {
  // file menu
  QAction* ffns, *ao3s, *fims;
  QAction* savs, *wcrs, *wres;
  QAction* cltab;
  // tools menu
  QAction* gfil, *sked;
  QAction* ao3c, *ffnc;
  QAction* fimgs;
  QAction* defdir, *dwnffic;
  QAction* syncdir;
  QAction* fimtag;
  QAction* tests;
}
//***************************************************************************
jfMainWindow::jfMainWindow(QWidget *parent):QMainWindow(parent) {
  SetupMainMenu();
  main_display = new QTabWidget(this);
  main_display->setTabPosition(QTabWidget::West);
  setCentralWidget(main_display);

  // setting up the size
  QRect qval = QGuiApplication::primaryScreen()->availableGeometry();
  if ((qval.width()<1024) || (qval.height()<750)) {
    showMaximized();
  }
  else {
    QSize thesize = size();
    thesize.setWidth(1200);
    thesize.setHeight(750);
    resize(thesize);
  }
  ptracker = new jfPageTracker();
  searchcount = 0;
  closestop = false;
  // connect slots
  connect(main_display, SIGNAL(currentChanged(int)), this, SLOT(handleTabChanged(int)));
}
//-----------------------------------------------------------------------------
jfMainWindow::~jfMainWindow() {

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// -- File Menu Slots --
//--------------------------------------------------------
void jfMainWindow::launchNewFFNSearch() {
  // variables
  jfFFNMain* ffn_search;
  // creating the new search panel
  ffn_search = new jfFFNMain(NULL,true);
  PrepNewSearch(ffn_search,jftl_FFNSEARCH,false,"Fanfiction.net");
}
//-----------------------------------------------------
// starting a FIMfiction.Net search
void jfMainWindow::launchNewFIMSearch() {
  // variables
  jfFIMMain* fimsearch;
  // creating the new search panel
  fimsearch = new jfFIMMain(NULL,true);
  PrepNewSearch(fimsearch,jftl_FIMSEARCH,false,"Fimfiction.net");
}
//------------------------------------------------------
void jfMainWindow::launchNewAO3Search() {
  // variables
  jfAO3Main* ao3search;
  // creating the new search panel
  ao3search = new jfAO3Main(NULL,true);
  PrepNewSearch(ao3search,jftl_AO3SEARCH,false,"AO3");
}
//------------------------------------------------------
void jfMainWindow::openSavedSearch() {
  // local variables
  const QString fname = "jfMainWindow::openSavedSearch";
  QString xfilen, typstr, outerr;
  jfSearchCore* loadres;
  // jfGoogleMain* google_edit;

  jfFFNMain* ffn_edit;
  jfFIMMain* fim_edit;
  jfAO3Main* ao3_edit;

  jfFIMGroupMain * fimg_edit;
  QString tabname;
    // we start with the filename
  /**/JDEBUGLOG(fname,1)
  if (GetJSSFilename(xfilen,false)) {
    /**/JDEBUGLOG(fname,2)
    loadres = MakeSearchFromFile(xfilen,outerr);
    /**/JDEBUGLOG(fname,3)
    // the load suceeded...
    if (loadres!=NULL) {
      /**/JDEBUGLOG(fname,4)
      // however, we do have to check the type
      typstr = loadres->GetTypeID();
      /**/JDEBUGLOGS(fname,5,typstr)
      if (typstr == "FFNSearch") {
        // creating a FFN Search tab
        ffn_edit = new jfFFNMain(loadres);
        /**/JDEBUGLOG(fname,6)
        PrepNewSearch(ffn_edit,jftl_FFNSEARCH,true,"Fanfiction.Net");
      }
      else if (typstr == "FIMSearch") {
        // creating a FIM Search tab
        fim_edit = new jfFIMMain(loadres,true);
        /**/JDEBUGLOG(fname,7)
        PrepNewSearch(fim_edit,jftl_FIMSEARCH,true,"Fimfiction.net");
      }
      else if (typstr == "AO3Search") {
        // creating a AO3 Search tab
        ao3_edit = new jfAO3Main(loadres);
        PrepNewSearch(ao3_edit,jftl_AO3SEARCH,true,"AO3");
      }
      else if (typstr == "FIMGroupSearch") {
        // creating a FIM Group Search tab
        fimg_edit = new jfFIMGroupMain(loadres,true);
        PrepNewSearch(fimg_edit,jftl_FIMGSEARCH,true,"FIM Group");
      }
      // unrecognized type, this is a real failure
      else assert(false);
    }
    // the load failed, we show a message before exiting
    else {
      jerror::Log(fname,outerr);
      QMessageBox::critical(this,"File Open failed!",outerr);
    }
    /**/JDEBUGLOG(fname,9);
  }
}

//------------------------------------------------------
void jfMainWindow::saveSearch() {
  // the objects
  QString xfilen;
  jfMainSearchGroup* qval;
  bool rvres;
  // doing the operation
  if (GetJSSFilename(xfilen,true)) {
    qval = dynamic_cast<jfMainSearchGroup*>(main_display->currentWidget());
    rvres = qval->SaveToFile(xfilen);
    assert(rvres);
  }
}
//------------------------------------------------------
void jfMainWindow::writeCurrent2Html() {
  DisplayPanelCore* cpanel;
  cpanel = dynamic_cast<DisplayPanelCore*>(main_display->currentWidget());
  cpanel->WriteCurrent2Html();
}

//------------------------------------------------------
void jfMainWindow::writeAll2Html() {
  DisplayPanelCore* cpanel;
  cpanel = dynamic_cast<DisplayPanelCore*>(main_display->currentWidget());
  cpanel->WriteAll2Html();
}
//------------------------------------------------------
void jfMainWindow::CloseCurrentTab() {
  // local variables
  DisplayPanelCore* qval = NULL;
  jfFic_UpdateDisplay* sval = NULL;
  // getting the selected tab
  int sindex = main_display->currentIndex();
  assert(sindex!=-1);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  // checking to see if the item we abe about to delete is the Global Filter Editor
  bool rgfilt = ptracker->IsGlobEdit(sindex);
  bool rgskel = ptracker->IsGlobSkel(sindex);
  // checking for stopping a search
  if (!ptracker->NotHtml(sindex)) {
     qval = dynamic_cast<DisplayPanelCore*>(main_display->currentWidget());
     closestop = qval->CloseSearch();
  }
  else if (ptracker->IsFicUpdate(sindex)) {
    sval = dynamic_cast<jfFic_UpdateDisplay*>(main_display->currentWidget());
    closestop = sval->StopUpdate();
  }
  // afterwards if closestop is true, the close is delayed until we get a signal...
  if (!closestop) {
    // what if we *have* removed the global filter editor?
    if (rgfilt) mac::gfil->setEnabled(true);
    // what if we *have* removed the global skeleton editor?
    if (rgskel) mac::sked->setEnabled(true);
    // close now
    CloseSearch();
    QApplication::restoreOverrideCursor();
  }
}
//-------------------------------------------------------

void jfMainWindow::doExit() {
  // constants
  const QString wmsg = "Please wait while information is saved. This could take a few minutes.";
  const QString cmsg = "The Application is closing";
  // variables
  bool wafr;
  jfTextStatusDialog* text_msg;
  QSize dsize(400,80);
  // starting
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  text_msg = new jfTextStatusDialog(this, cmsg, wmsg, dsize);
  text_msg->show();
  QCoreApplication::processEvents();
  wafr = jglobal::startstop.WriteAtFinish();
  assert(wafr);
  text_msg->close();
  delete ptracker;
  QApplication::restoreOverrideCursor();
  close();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// tools slots
//---------------------
void jfMainWindow::showGlobalSkeletons() {
  jfGlobalSkeletonEdit* sgshow;
  int gskw;
  if (!(ptracker->SkelEditLoaded())) {
    sgshow = new jfGlobalSkeletonEdit(mainskelc);
    ptracker->AddPage(sgshow,jftl_SKELEDIT);
    gskw = main_display->addTab(sgshow,"Skeleton Editor");
    main_display->setCurrentIndex(gskw);
    mac::sked->setEnabled(false);
    EnableClose();
  }
}
//---------------------

void jfMainWindow::launchFanficDownload() {
  jfFicDownDialog2* downdialog;
  downdialog = new jfFicDownDialog2(this);
  downdialog->show();
  downdialog->raise();
  downdialog->activateWindow();
}

//-------------------------------------------------------
void jfMainWindow::showPreferences() {
  jfDefaultDirsDialog* defdirs;
  defdirs = new jfDefaultDirsDialog(this);
  defdirs->show();
  defdirs->raise();
  defdirs->activateWindow();
}
//-------------------------------------------------------
void jfMainWindow::showGlobalFilterEditor() {
  jfGlobalFilterEditor* gfilt_edit;
  int gfew;
  if (!(ptracker->GlobEditLoaded())) {
    gfilt_edit = new jfGlobalFilterEditor(filterglobals::global_filters);
    ptracker->AddPage(gfilt_edit,jftl_GLOBFILEDIT);
    gfew = main_display->addTab(gfilt_edit,"Global Filter Editor");
    main_display->setCurrentIndex(gfew);
    mac::gfil->setEnabled(false);
    EnableClose();
  }
}
//-------------------------------------------------------

void jfMainWindow::launchAO3FandomDownload() {
  jfAO3Cat_DownDialog* ao3_dialog;
  ao3_dialog = new jfAO3Cat_DownDialog(this);
  connect(ao3_dialog,SIGNAL(SendDoneOkay()),this,SLOT(handleAO3FEnd()));
  ao3_dialog->show();
  ao3_dialog->raise();
  ao3_dialog->activateWindow();
}
//------------------------------------------------------
void jfMainWindow::launchFicUpdater() {
  jfFic_UpdateDisplay* fic_upd;
  QString tabname = "Downloaded Fanfic Updater";
  fic_upd = new jfFic_UpdateDisplay();
  ptracker->AddPage(fic_upd,jftl_FICUPDATE);
  int ficu_idx = main_display->addTab(fic_upd,tabname);
  connect(fic_upd, SIGNAL(relayDoneSignal()), this, SLOT(handleFicUpdateEnd()));
  main_display->setCurrentIndex(ficu_idx);
  EnableClose();
}
//-------------------------------------------------------
void jfMainWindow::launchFFNCategoryDownload() {
  jfFFN_CatDownDialog* ffn_dialog;
  ffn_dialog = new jfFFN_CatDownDialog(this);
  connect(ffn_dialog,SIGNAL(SendCatDone(bool)),this,SLOT(handleFFNCEnd(bool)));
  ffn_dialog->show();
  ffn_dialog->raise();
  ffn_dialog->activateWindow();
}
//-------------------------------------------------------
void jfMainWindow::launchFIMGroupSearch() {
  jfFIMGroupMain * fimg_edit;
  // creating the new search panel
  fimg_edit = new jfFIMGroupMain(NULL,true);
  PrepNewSearch(fimg_edit,jftl_FIMGSEARCH,true,"FIM Group");
}
//-------------------------------------------------------
void jfMainWindow::launchDirectorySync() {
    // variables
    jfUpdateDirectory_Dialog* dirsync;
    // creating and showing
    dirsync = new jfUpdateDirectory_Dialog(this);
    dirsync->show();
    dirsync->raise();
    dirsync->activateWindow();
}
//----------------------------------------------------------
void jfMainWindow::fimTagDownload() {
    // the window
    jfFIMTag_DownloadDialog* fimtag_dialog;

    fimtag_dialog = new jfFIMTag_DownloadDialog(this);
    fimtag_dialog->show();
    fimtag_dialog->raise();
    fimtag_dialog->activateWindow();
}
//----------------------------------------------------------
void jfMainWindow::launchTestsPanel() {
    jfTestDisplay* test_panel;
    QString tabname = "Tests";
    test_panel = new jfTestDisplay();
    ptracker->AddPage(test_panel,jftl_TESTS);
    int tests_idx = main_display->addTab(test_panel,tabname);
    // connect(fic_upd, SIGNAL(relayDoneSignal()), this, SLOT(handleFicUpdateEnd()));
    main_display->setCurrentIndex(tests_idx);
    EnableClose();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// non menu slots
//-------------------------------------------------------
void jfMainWindow::handleTabChanged(int index) {
  MenuEnabler(index);
}
//-------------------------------------------------------
void jfMainWindow::handleSearchStart() {
  QWidget* xsender;
  xsender = dynamic_cast<QWidget*>(sender());
  ptracker->SetHtmlReady(xsender,false);
  MenuEnabler(main_display->currentIndex());
}

//-------------------------------------------------------
void jfMainWindow::handleSearchEnd(bool complete) {
  QWidget* xsender;
  if (closestop) {
    CloseSearch();
    QApplication::restoreOverrideCursor();
  }
  else {
    xsender = dynamic_cast<QWidget*>(sender());
    ptracker->SetHtmlReady(xsender,complete);
    MenuEnabler(main_display->currentIndex());
  }
}
//-------------------------------------------------------
void jfMainWindow::handleFicUpdateEnd() {
  if (closestop) {
    CloseSearch();
    QApplication::restoreOverrideCursor();
  }
}
//-------------------------------------------------------
void jfMainWindow::handleAO3FEnd() {
  mac::ao3s->setEnabled(true);
}
//------------------------------------------------------
void jfMainWindow::handleFFNCEnd(bool xin) {
  mac::ffns->setEnabled(xin);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
bool jfMainWindow::GetJSSFilename(QString& outname, bool save) {
  // constants
  const QString wildstr = "Jaffis Saved Search Files (*.jss)";
  const QString savemsg = "Specify the Save filename";
  const QString openmsg = "Specify the File to open";
  // variables
  QString result,xfilen,savestring;
  jfMainSearchGroup* qsearch;
  int sresult;
  // options
  if (save) {
    qsearch = dynamic_cast<jfMainSearchGroup*>(main_display->currentWidget());
    xfilen = qsearch->GetDisplayName() + ".jss";
    sresult = main_display->currentIndex();
    savestring = jglobal::settings.paths.GetPathFor(jglobal::SAVED_SEARCHES);
    savestring += QDir::separator() + xfilen;
    result = QFileDialog::getSaveFileName(this,savemsg,savestring,wildstr);

  }
  else {
    QString odir = jglobal::settings.paths.GetPathFor(jglobal::SAVED_SEARCHES);
    result = QFileDialog::getOpenFileName(this,openmsg,odir,wildstr);
  }
  // handing the result
  if (result.isEmpty()) return false;
  else {
    outname = result;
    return true;
  }
}
//------------------------------------------------------
bool jfMainWindow::MenuEnabler(int menu_index) {
  if (menu_index<0) {
    // disable menus
    mac::savs->setEnabled(false);
    mac::wcrs->setEnabled(false);
    mac::wres->setEnabled(false);
    mac::cltab->setEnabled(false);
    return false;
  }
  else {
    // special case
    if (main_display->count()==0) return false;
    // html menus
    bool html_e = ptracker->IsHtmlReady(menu_index);
    mac::wcrs->setEnabled(html_e);
    mac::wres->setEnabled(html_e);
    // save menu
    html_e = ptracker->NotHtml(menu_index);
    mac::savs->setEnabled(!html_e);
    // done
    return true;
  }
}
//------------------------------------------------------
void jfMainWindow::ConnectSearch(jfMainSearchGroup* newpanel) {
  assert(newpanel!=NULL);
  connect(newpanel, SIGNAL(SendStartingSignal()), this, SLOT(handleSearchStart()));
  connect(newpanel, SIGNAL(SendDoneSignal(bool)), this, SLOT(handleSearchEnd(bool)));
}
//------------------------------------------------------
bool jfMainWindow::PrepNewSearch(jfMainSearchGroup* nsearch, jf_tlPageType stype, bool usname, const QString& site_name) {
    const QString fname = "jfMainWindow::PrepNewSearch";
    // special checks
    /**/JDEBUGLOG(fname,1)
    if (nsearch==NULL) {
        jerror::Log(fname,"Search is null");
        return false;
    }
    if (site_name.isEmpty()) {
        jerror::Log(fname,"The site name is empty!");
        return false;
    }
  // variables
  int fimsw;
  QString tabname;
  searchcount++;
   /**/JDEBUGLOG(fname,2)
  tabname = site_name + " Search : ";
  if (usname) tabname += nsearch->GetSearchName();
  else tabname += QString::number(searchcount);
   /**/JDEBUGLOG(fname,5)
  // adding to tracker and notebook
  ptracker->AddPage(nsearch,stype);
   /**/JDEBUGLOG(fname,6)
  fimsw = main_display->addTab(nsearch,tabname);
   /**/JDEBUGLOG(fname,1)
  // finishing
  main_display->setCurrentIndex(fimsw);
   /**/JDEBUGLOG(fname,7)
  ConnectSearch(nsearch);
   /**/JDEBUGLOG(fname,8)
  EnableClose();
   /**/JDEBUGLOG(fname,9)
  // done
  return true;
}
//------------------------------------------------------
void jfMainWindow::CloseSearch() {
  // variables
  const QString fname = "jfMainWindow::CloseSearch";
  int sindex;
  jfFFNMain* zval;
  jfAO3Main* qval;
  QWidget* xpage;
  size_t scat_index, pcount;
  // starting
  sindex = main_display->currentIndex();
  assert(sindex!=-1);
  pcount = main_display->count();
  //if we are doing an FFN search, we remove the search from the category organizer
  if (ptracker->GetType(sindex)==jftl_FFNSEARCH) {
    zval = dynamic_cast<jfFFNMain*> (main_display->currentWidget());
    scat_index = zval->GetSCatIndex();
    ffn_catorganizer::main_manager->RemoveSearch(scat_index);
  }
  // we also do it for AO3
  if (ptracker->GetType(sindex)==jftl_AO3SEARCH) {
    qval = dynamic_cast<jfAO3Main*> (main_display->currentWidget());
    scat_index = qval->GetSCatIndex();
    ao3_catdata::ao3_catmanager->RemoveSearch(scat_index);
  }
  // removing the page
  /**/JDEBUGLOG(fname,1);
  ptracker->RemovePage(sindex);
  xpage = main_display->currentWidget();
  if (pcount==1) main_display->blockSignals(true);
  main_display->removeTab(sindex);
  if (pcount==1) main_display->blockSignals(false);
  delete xpage;
  closestop = false;
  sindex = main_display->currentIndex();
  MenuEnabler(sindex);
}
//-------------------------------------------------------
void jfMainWindow::EnableClose() {
  mac::cltab->setEnabled(true);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfMainWindow::SetupMainMenu() {
  const QString fname = "jfMainWindow::SetupMainMenu";
  main_menu = menuBar();
  // building the file menu
  fmenu = main_menu->addMenu("&File");
  mac::ffns = fmenu->addAction("New Fanfiction.net Search...",this,SLOT(launchNewFFNSearch()));
  mac::fims = fmenu->addAction("New FIMfiction.net Search...",this,SLOT(launchNewFIMSearch()));
  mac::ao3s = fmenu->addAction("New Archive of Our Own Search...",this,SLOT(launchNewAO3Search()));
  fmenu->addSeparator();
  fmenu->addAction("Open Saved Search...",this,SLOT(openSavedSearch()));
  mac::savs = fmenu->addAction("Save Search As...",this,SLOT(saveSearch()));
  mac::savs->setEnabled(false);
  mac::wcrs = fmenu->addAction("Current Result Category to HTML",this,SLOT(writeCurrent2Html()));
  mac::wcrs->setEnabled(false);
  mac::wres = fmenu->addAction("Write Results to HTML",this,SLOT(writeAll2Html()));
  mac::wres->setEnabled(false);
  fmenu->addSeparator();
  mac::cltab = fmenu->addAction("Close Tab",this,SLOT(CloseCurrentTab()));
  mac::cltab->setEnabled(false);
  fmenu->addAction("Exit",this,SLOT(doExit()));

  // building the tools menu
  tmenu = main_menu->addMenu("&Tools");
  mac::sked = tmenu->addAction("Skeleton Editor...",this,SLOT(showGlobalSkeletons()));
  mac::gfil = tmenu->addAction("Global Filter Editor...",this,SLOT(showGlobalFilterEditor()));
  mac::defdir = tmenu->addAction("Default Directories...",this,SLOT(showPreferences()));
  mac::tests = tmenu->addAction("Test download and parse...",this,SLOT(launchTestsPanel()));
  tmenu->addSeparator();
  mac::ffnc = tmenu->addAction("Download FFN Catgeories...",this,SLOT(launchFFNCategoryDownload()));
  mac::ao3c = tmenu->addAction("Download AO3 Fandoms...",this,SLOT(launchAO3FandomDownload()));
  mac::fimgs = tmenu->addAction("Search FIM Groups...",this,SLOT(launchFIMGroupSearch()));
  mac::fimtag = tmenu->addAction("Fetch FIM Tags...",this,SLOT(fimTagDownload()));
  tmenu->addSeparator();
  mac::dwnffic = tmenu->addAction("Download Fanfic...",this,SLOT(launchFanficDownload()));
  tmenu->addAction("Update Downloaded Fics...",this,SLOT(launchFicUpdater()));
  mac::syncdir = tmenu->addAction("Synchronize Directories...",this,SLOT(launchDirectorySync()));
  // ao3 enabling
  bool atest = (ao3_catdata::ao3_catmanager->HasData());
  mac::ao3s->setEnabled(atest);
  // ffn enabling
  atest = (ffn_catorganizer::main_manager->Empty());
  mac::ffns->setEnabled(!atest);
}
//***************************************************************************
