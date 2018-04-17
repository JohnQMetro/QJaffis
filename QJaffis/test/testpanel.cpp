/******************************************************************************
// Name:        testpanel.cpp
// Author :     John Q Metro
// Purpose :    The interface for the download/parse testing
// Created:     March 29, 2018
// Updated:     April 17, 2018
******************************************************************************/
#ifndef TESTPANEL_H
    #include "testpanel.h"
#endif // TESTPANEL_H
#ifndef TESTURL_H
    #include "testurl.h"
#endif // TESTURL_H
//----------------------------------
#include <QApplication>
/*****************************************************************************/
jfProgressPanelTest::jfProgressPanelTest(QWidget* parent):jfProgressPanelOne(true,parent) {

}
//-------------------------------------------------------------------------------
// specials
bool jfProgressPanelTest::ConnectAndSetPauser(jfTestDownloader* the_downloader) {
    const QString fname = "jfProgressPanelTest::ConnectAndSetPauser";
    if (the_downloader == NULL) return false;
    bool pstatus = MakePauser();
    if (!pstatus) return false;
    // setting up and connecting the downloader
    pstatus = the_downloader->setPauser(pauser);
    if (!pstatus) return false;
    return ConnectThread(the_downloader);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfProgressPanelTest::HandleItemDone() {
    jfProgressPanelOne::HandleItemDone();
    emit SendChecked(lastinfo.item_name,jfa_DONE);
}
//--------------------------------------------------------------------
void jfProgressPanelTest::HandleError(struct jfFetchErrorBundle error_info) {
    jfProgressPanelOne::HandleError(error_info);
    emit SendChecked(lastinfo.item_name,error_info.why);
}
//============================================================================
// constructor
jfTestDisplay::jfTestDisplay(QWidget* parent):QWidget(parent) {
  // initializing
  downloader = NULL;
  thread_object = NULL;
  ttest = test::count;
  dtest = oktest = ftest = pftest = 0;
  launched = false;

  // we start with the GUI elemets
  progress_bar = new jfProgressPanelTest();
  check_count = new QLabel(QString::number(ttest) + " Tests to do.");
  results_display = new QPlainTextEdit();
  results_display->setLineWrapMode(QPlainTextEdit::NoWrap);
  // arranging in sizers
  main_sizer = new QVBoxLayout();
  main_sizer->addWidget(progress_bar,0);
  main_sizer->addWidget(check_count,0);
  main_sizer->addWidget(results_display,2);

  // connections
  connect(progress_bar,SIGNAL(SendStart()),this,SLOT(HandleStart()));
  connect(progress_bar,SIGNAL(SendPanelDone(bool)),this,SLOT(HandleDone(bool)));
  connect(progress_bar,SIGNAL(SendChecked(QString,jfFETCH_AFTERMATH)),this,SLOT(HandleTestAnswerEvent(QString,jfFETCH_AFTERMATH)));

  // completion
  setLayout(main_sizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
void jfTestDisplay::Reset() {
  // status reset
  progress_bar->DoReset();
  // resetting internal data
  downloader = NULL;
  thread_object = NULL;
  ttest = test::count;
  dtest = oktest = ftest = pftest = 0;
  // clearing the labels and text results
  check_count->setText(QString::number(ttest) + " Tests to do.");
  results_display->clear();
  launched = false;
}
//-----------------------------------------------------------
bool jfTestDisplay::StopUpdate() {
  return progress_bar->HaltProgress();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handlers
void jfTestDisplay::HandleStart() {
  // variables
  const QString fname = "jfTestDisplay::HandlePanelStart";
  int xvalue;
  QString xdir;
  // starting
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  /**/JDEBUGLOG(fname,1);
  if (launched) Reset();
  launched = true;
  // fic parameters
  downloader = new jfTestDownloader(1);
  // setting the info and launching
  /**/JDEBUGLOG(fname,2)
  SetupDownloaderAndThread();

  // done
  QApplication::restoreOverrideCursor();
  thread_object->start();
}

//------------------------------------------------
void jfTestDisplay::HandleDone(bool okay) {
  QString newlabel;
  // check count
  newlabel = "Check Done. " + GetCountLabel();
  check_count->setText(newlabel);
  emit relayDoneSignal();
}
//------------------------------------------------
void jfTestDisplay::HandleTestAnswerEvent(QString what, jfFETCH_AFTERMATH result) {
  // variables
  QString resmsg;
  // starting
  dtest++;

  resmsg = "The Test " + what + " has ";

  if (result == jfa_DONE) {
      resmsg += "PASSED.";
      oktest++;
  }
  else {
      resmsg += "failed because the ";
      if (result == jfa_PARSEERR) {
          resmsg += "of a PARSING ERROR.";
          pftest++;
      }
      else {
          ftest++;
          if (result == jfa_MISSING) resmsg += "page was MISSING.";
          else if (result == jfa_FETCHFAIL) resmsg += "page failed to download.";
          else if (result == jfa_HALT) resmsg += "use halted things.";
          else resmsg += "page was skipped.";
      }
  }
  results_display->appendPlainText(resmsg);
  check_count->setText(GetCountLabel());
}
//------------------------------------------------
void jfTestDisplay::ThreadDone() {
  // disconnecting
  disconnect(thread_object, 0,downloader ,0);
  disconnect(downloader,0,thread_object,0);
  disconnect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  progress_bar->DisConnectThread(downloader);
  // deleting
  downloader->deleteLater();
  thread_object->deleteLater();
  downloader = NULL;
  thread_object = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods
//------------------------------------------------
bool jfTestDisplay::SetupDownloaderAndThread() {
  const QString fname = "jfTestDisplay::SetupDownloaderAndThread";
  if (downloader == NULL) return false;
  // setting up the thread and downloader
  thread_object = new QThread();
  downloader->moveToThread(thread_object);
  // connecting signals and slots
  bool testoo = progress_bar->ConnectAndSetPauser(downloader);
  /**/jfAssertLog(testoo,fname,"Connecting FAILED!!!!");
  // standard thread/downloader connections
  connect(thread_object, SIGNAL(started()),downloader ,SLOT(StartProcessing()));
  connect(downloader,SIGNAL(AllFinished()),thread_object,SLOT(quit()));
  connect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  return true;
}
//------------------------------------------------
QString jfTestDisplay::GetCountLabel() const {
  QString reslabel;
  bool extra = false;
  reslabel = QString::number(dtest) + " of " + QString::number(ttest) + " Tests done. ";
  if (oktest>0) {
    reslabel += QString::number(oktest) + " Passed";
    extra = true;
  }
  if (ftest>0) {
    if (extra) reslabel += ", ";
    reslabel += QString::number(ftest) + " Failed Downloading";
    extra = true;
  }
  if (pftest>0) {
    if (extra) reslabel += ", ";
    reslabel += QString::number(pftest) + " Failed Parsing";
    extra = true;
  }
  if (extra) reslabel += ".";
  return reslabel;
}
/*****************************************************************************/
