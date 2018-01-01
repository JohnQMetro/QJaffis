/***************************************************************
 * Name:      dirupdate_dialog.h
 * Purpose:   Declares dialog for updating directories
 * Author:    John Q Metro (johnqmetro@hotmail.com)
 * Created:   June 30, 2015
 * Updated:   August 3, 2016
 *
**************************************************************/
#ifndef DIRUPDATE_DIALOG_H_INCLUDED
  #include "dirupdate_dialog.h"
#endif // DIRUPDATE_DIALOG_H_INCLUDED
//-------------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED
//-------------------------------------------
#include <QMessageBox>
#include <QApplication>
//****************************************************************************
// constructor
jfUpdateDirectoryPickPanel::jfUpdateDirectoryPickPanel(QWidget* parent):QWidget(parent) {
  // starting...
  MakeWidgets();
  // creating the sizer
  layout = new QVBoxLayout();
  layout->addWidget(sourcedir,0);
  layout->addWidget(destdir,0);
  // finishing
  setLayout(layout);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// load and save
//---------------------------------------------
QString jfUpdateDirectoryPickPanel::GetValue(bool source) const {
  if (source) return sourcedir->GetPath();
  else return destdir->GetPath();
}
//---------------------------------------------
void jfUpdateDirectoryPickPanel::SetValue(bool source, QString inval) {
  if (source) sourcedir->SetPathUnchecked(inval);
  else destdir->SetPathUnchecked(inval);
}
//---------------------------------------------
int jfUpdateDirectoryPickPanel::TestDirs() const {
  // variables
  QString tval;
  QDir dummy;
  // checking
  tval = sourcedir->GetPath();
  if (!dummy.exists(tval)) return 0;
  tval = destdir->GetPath();
  if (!dummy.exists(tval)) return 1;
  return -1;
}
//-------------------------------------------
void jfUpdateDirectoryPickPanel::SetLEnabled(bool enabled) {
  sourcedir->SetLEnabled(enabled);
  sourcedir->SetLEnabled(enabled);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gui builder methods
//---------------------------------------------
void jfUpdateDirectoryPickPanel::MakeWidgets() {
  QString sdir = jglobal::settings.GetDirectory(jglobal::SourceForSync);
  sourcedir = new jfDirPicker(NULL,false,false,"Source Directory",sdir);
  sdir = jglobal::settings.GetDirectory(jglobal::DestinationForSync);
  destdir = new jfDirPicker(NULL,false,false,"Target Directory",sdir);
}

//=====================================================================================
jfProgressPanelSyncDirectory::jfProgressPanelSyncDirectory(QWidget* parent):
      jfProgressPanelOne(false,parent) {}
//--------------------------------
bool jfProgressPanelSyncDirectory::ConnectAndSetPauser(jfDirUpdate_Thread* the_downloader) {
  const QString fname = "jfProgressPanelOne::ConnectAndSetPauser";
  if (the_downloader == NULL) return false;
  bool pstatus = MakePauser();
  if (!pstatus) return false;
  // setting up and connecting the downloader
  pstatus = the_downloader->setPauser(pauser);
  if (!pstatus) return false;
  connect(the_downloader,SIGNAL(itemFailed(bool)),this,SLOT(HandleItemFailed(bool)));
  return ConnectThread(the_downloader);
}
//------------------------------------
void jfProgressPanelSyncDirectory::HandleItemFailed(bool halt) {
  // variables
  QString errmsg;
  // halting with error
  if (halt) errmsg = "Directory update/sync halted by user.";
  // skipping with errror
  else errmsg = "Directory update/sync has failed.";
  SetLabel(errmsg);
  MoreError(lasterror.halt);
}
//=====================================================================================
jfUpdateDirectory_Dialog::jfUpdateDirectory_Dialog(QWidget* parent):QDialog(parent) {
  // doing stuff
  dpanel = new jfUpdateDirectoryPickPanel();
  mpanel = new jfProgressPanelSyncDirectory();

  FinishSizers();
  connect(mpanel,SIGNAL(SendStart()),this,SLOT(HandleStart()));
  connect(mpanel,SIGNAL(SendPanelDone(bool)),this,SLOT(HandleDone(bool)));

  QSize xsize = QSize(705,180);
  resize(xsize);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfUpdateDirectory_Dialog::ThreadDone() {
  // disconnecting
  disconnect(thread_object, 0,sync_runner ,0);
  disconnect(sync_runner,0,thread_object,0);
  disconnect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  mpanel->DisConnectThread(sync_runner);
  // deleting
  sync_runner->deleteLater();
  thread_object->deleteLater();
  sync_runner = NULL;
  thread_object = NULL;
}
//--------------------------------------------------------
// button handler events
void jfUpdateDirectory_Dialog::HandleStart() {
  // constnats and variables
  const QString fname = "jfUpdateDirectory_Dialog::HandleStart";
  QString dir_err;
  bool testres;
  QMessageBox emsg;
  // starting
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  testres = GetDirectories(dir_err);
  if (!testres) {
    // preparing the faliure (beacause of bad source or target dir)
    emsg.setIcon(QMessageBox::Critical);
    emsg.setWindowTitle("Directory Error!");
    emsg.setText("Update cannot go ahead : " + dir_err);
    emsg.setStandardButtons(QMessageBox::Abort);
    // showing the faliure
    QApplication::restoreOverrideCursor();
    emsg.show();
  }
  else {
    dpanel->SetLEnabled(false);
    sync_runner = new jfDirUpdate_Thread();
    sync_runner->SetDirectories(sdir,tdir,false);
    SetupThread();
    QApplication::restoreOverrideCursor();
    thread_object->start();
  }
}
//-------------------------------------------------
void jfUpdateDirectory_Dialog::HandleDone(bool) {
  dpanel->SetLEnabled(true);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfUpdateDirectory_Dialog::SetupThread() {
  const QString fname = "jfFFN_CatDownDialog::SetupDownloaderAndThread";
  if (sync_runner == NULL) return false;
  // setting up the thread and downloader
  thread_object = new QThread();
  sync_runner->moveToThread(thread_object);
  // connecting signals and slots
  bool testoo = mpanel->ConnectAndSetPauser(sync_runner);
  /**/jfAssertLog(testoo,fname,"Connecting and Setting Pauser FAILED!!!!");

  // standard thread/downloader connections
  connect(thread_object, SIGNAL(started()),sync_runner ,SLOT(StartProcessing()));
  connect(sync_runner,SIGNAL(AllFinished()),thread_object,SLOT(quit()));
  connect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  return true;
}

//--------------------------------------------------------------
//private helpher methods
void jfUpdateDirectory_Dialog::FinishSizers() {
  mainsizer = new QVBoxLayout();
  mainsizer->addWidget(dpanel,0);
  mainsizer->addWidget(mpanel,0);
  setLayout(mainsizer);
}
//------------------------------------------------------------------------
bool jfUpdateDirectory_Dialog::GetDirectories(QString& out_err) {
  sdir = dpanel->GetValue(true);
  tdir = dpanel->GetValue(false);
  QDir source_dir = QDir(sdir);
  QDir dest_dir = QDir(tdir);
  if (!source_dir.exists()) {
    out_err = "Source Directory does not exist!";
    return false;
  }
  if (!dest_dir.exists()) {
    out_err = "Target Directory does not exist!";
    return false;
  }
  if (sdir==tdir) {
    out_err = "The directories must be different!";
    return false;
  }
  if (!source_dir.isReadable()) {
    out_err = "The Source Directory cannot be read!";
    return false;
  }
  QString destdirm = tdir;
  if (!destdirm.endsWith('/')) destdirm += '/';
  destdirm += '.';
  QFileInfo fx = QFileInfo(destdirm);
  if (!fx.isWritable()) {
    out_err = "The Target Directory cannot be written to!";
    return false;
  }
  out_err = "No Problem";
  return true;
}
//*********************************************************************************************
