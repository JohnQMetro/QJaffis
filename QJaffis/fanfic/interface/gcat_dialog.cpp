/******************************************************************************
Name    :   gcat_dialog.cpp
Author  :   John Q Metro
Purpose :   Dialog for Generic Catalog Downloading
Created :   August 19, 2012
Conversion to Qt started March 12, 2014
Updated :   July 15, 2016
******************************************************************************/
#ifndef GCAT_DIALOG_H_INCLUDED
  #include "gcat_dialog.h"
#endif // GCAT_DIALOG_H_INCLUDED
//*************************************************************************
// the constructor
jfGenCat_DownDialog::jfGenCat_DownDialog(QWidget* parent):QDialog(parent) {
// starting assignments
  main_downloader = NULL;
  fetch_thread = NULL;
// we start with the part status display
  mainstatus = new jfProgressPanelSingle(true);
// we arrange the main sizer
  main_sizer = new QVBoxLayout();
  main_sizer->addWidget(mainstatus,1);
  // thread stuff
  main_downloader = NULL;
  connect(mainstatus, SIGNAL(SendStart()),this, SLOT(HandleStart()));
  connect(mainstatus, SIGNAL(SendPanelDone(bool)),this, SLOT(HandleDone(bool)));
// done, sizers and size now
  setLayout(main_sizer);
  QSize zsize = sizeHint();
  zsize.setWidth(750);
  resize(zsize);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// button events
//-----------------------------------------------------------------
// the start button
void jfGenCat_DownDialog::HandleStart() {
  // constants
  const QString fname = "jfGenCat_DownDialog::StartButton";
  // starting
  // basically, we create the thread...
  main_downloader = MakeTypedThread();
  SetupDownloaderAndThread();
  fetch_thread->start();
}
//---------------------------------------------------------------
void jfGenCat_DownDialog::HandleDone(bool okay) {


}
//--------------------------------------------------------
void jfGenCat_DownDialog::ThreadDone() {
  // disconnecting
  disconnect(fetch_thread, 0,main_downloader ,0);
  disconnect(main_downloader,0,fetch_thread,0);
  disconnect(fetch_thread,SIGNAL(finished()),this,SLOT(ThreadDone()));
  mainstatus->DisConnectThread(main_downloader);
  // deleting
  main_downloader->deleteLater();
  fetch_thread->deleteLater();
  main_downloader = NULL;
  fetch_thread = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfGenCat_DownDialog::SetupDownloaderAndThread() {
  const QString fname = "jfGenCat_DownDialog::SetupDownloaderAndThread";
  if (main_downloader == NULL) return false;
  // setting up the thread and downloader
  fetch_thread = new QThread();
  main_downloader->moveToThread(fetch_thread);
  // connecting signals and slots
  bool testoo = mainstatus->ConnectAndSetPauser(main_downloader);
  jerror::AssertLog(testoo,fname,"Connecting and Setting Pauser FAILED!!!!");

  // standard thread/downloader connections
  connect(fetch_thread, SIGNAL(started()),main_downloader ,SLOT(StartProcessing()));
  connect(main_downloader,SIGNAL(AllFinished()),fetch_thread,SLOT(quit()));
  connect(fetch_thread,SIGNAL(finished()),this,SLOT(ThreadDone()));

  return true;
}

//****************************************************************************
