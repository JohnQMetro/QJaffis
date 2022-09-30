/******************************************************************************
// Name:        ficupdate_panel.cpp
// Author :     John Q Metro
// Purpose :    The interface for updating downloaded fanfics
// Created:     July 26, 2016
// Updated:     June 11, 2022
******************************************************************************/
#ifndef FICUPDATE_PANEL_H
  #include "ficupdate_panel.h"
#endif // FICUPDATE_PANEL_H
//--------------------------------------
#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED
//--------------------------------------
#include <QApplication>
#include <assert.h>
/*****************************************************************************/
// --- [ METHODS for jfProgPanelFicUpdating ] -------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setup stuff
//-----------------------------
jfProgPanelFicUpdating::jfProgPanelFicUpdating(QWidget* parent):jfProgPanelMultiBase(1, parent) {

}
//-----------------------------
bool jfProgPanelFicUpdating::ConnectAndSetPauser(jfFicUpdateThread* the_downloader) {
  if (the_downloader == NULL)  return false;
  bool pstatus = MakePauser();
  if (!pstatus) return false;
  // setting up and connecting the downloader
  the_downloader->setPauser(pauser);
  ConnectThread(the_downloader);
  connect(the_downloader,SIGNAL(sendSectionCount(size_t)),this,SLOT(HandleCatCount(size_t)));
  connect(the_downloader,SIGNAL(sendNewSection(struct jfItemInfoMsg)),this,SLOT(HandleNewCategory(struct jfItemInfoMsg)));
  connect(the_downloader,SIGNAL(sendSectionDone(bool)),this,SLOT(HandleCategoryDone(bool)));
  connect(the_downloader,SIGNAL(sendSectionFail()),this,SLOT(HandleCatFail()));

  connect(the_downloader,SIGNAL(sendOneActionSection(QString,QString)),this,SLOT(HandleOneThingSection(QString,QString)));

  return true;
}
//-----------------------------
void jfProgPanelFicUpdating::DisconnectDownloader(jfFicUpdateThread* the_downloader) {
  if (the_downloader != NULL) {
    disconnect(the_downloader, 0,this, 0);
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slot handling
//-----------------------------
void jfProgPanelFicUpdating::HandleOneThingSection(QString action, QString what) {
  catinfo.startaction = lastinfo.startaction = action;
  catinfo.item_label = lastinfo.item_label = what;
  catinfo.item_name = lastinfo.item_name= "";
  catinfo.item_index = lastinfo.item_index = -1;
  SetCatLabel("");
  item_bar->setValue(0);
  item_bar->setMaximum(1);
  itemindex = 0;
  SetLabel("");
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££
// constructor
jfFic_UpdateDisplay::jfFic_UpdateDisplay(QWidget* parent):QWidget(parent) {
  // initializing
  downloader = NULL;
  thread_object = NULL;
  tfics = dfics = ufics = mfics = ffics = 0;
  launched = false;
  // setting up the main group box
  sbox = new QGroupBox("Result Information");
  sbox_layout = new QVBoxLayout();
  sbox->setLayout(sbox_layout);

  // we start with the GUI elemets
  const QString pickm = "Pick the Directory to search for fanfics to update";
  QString ficdir = jglobal::settings.paths.GetPathFor(jglobal::SAVED_FANFIC_A);
  dir_pick = new jfDirPicker(NULL,false,true,pickm,ficdir);
  split_guide = new jfLabeledIntEdit(NULL,"File Size Guide (KB)",true,10,200000);
  split_guide->SetValue(jglobal::settings.ficsize_hint);
  main_check = new QCheckBox("Check Completed Stories");
  skip_ffn = new QCheckBox("Skip Fanfiction.net Stories");
  main_display = new jfProgPanelFicUpdating();
  fic_count = new QLabel("Nothing Done Yet");
  results_display = new QPlainTextEdit();
  results_display->setLineWrapMode(QPlainTextEdit::NoWrap);
  // arranging in sizers
  top_sizer = new QHBoxLayout();
  check_sizer = new QHBoxLayout();
  check_sizer->setContentsMargins(0,0,0,0);
  top_sizer->addWidget(dir_pick,1,Qt::AlignVCenter);
  top_sizer->addSpacing(6);
  top_sizer->addWidget(split_guide,0,Qt::AlignVCenter);
  main_sizer = new QVBoxLayout();
  sbox_layout->addWidget(fic_count,0);
  sbox_layout->addWidget(results_display,1);
  main_sizer->addLayout(top_sizer,0);
  check_sizer->addWidget(main_check, 0, Qt::AlignLeft);
  check_sizer->addWidget(skip_ffn, 0, Qt::AlignLeft);
  main_sizer->addLayout(check_sizer);
  main_sizer->addWidget(main_display,0);
  main_sizer->addWidget(sbox,1);
  // connections
  connect(main_display,SIGNAL(SendStart()),this,SLOT(HandleStart()));
  connect(main_display,SIGNAL(SendPanelDone(bool)),this,SLOT(HandleDone(bool)));

  // completion
  setLayout(main_sizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
void jfFic_UpdateDisplay::Reset() {
  // status reset
  main_display->DoReset();
  // resetting internal data
  downloader = NULL;
  thread_object = NULL;
  tfics = dfics = ufics = mfics = ffics = 0;
  // clearing the labels and text results
  fic_count->setText("Nothing Done Yet");
  results_display->clear();
  launched = false;
}
//-----------------------------------------------------------
bool jfFic_UpdateDisplay::StopUpdate() {
  return main_display->HaltProgress();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handlers
void jfFic_UpdateDisplay::HandleStart() {
  // variables
  const QString fname = "jfFic_UpdateDisplay::HandlePanelStart";
  int xvalue;
  QString xdir;
  // starting
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  /**/JDEBUGLOG(fname,1);
  if (launched) Reset();
  launched = true;
  // fic parameters
  xvalue = split_guide->GetValue();
  xdir = dir_pick->GetPath();
  downloader = new jfFicUpdateThread();
  downloader->SetParams(xdir,xvalue,main_check->isChecked(), skip_ffn->isChecked());
  // setting the info and launching
  /**/JDEBUGLOG(fname,2)
  SetupDownloaderAndThread();

  connect(downloader,SIGNAL(sendUpdateResult(ficur::jfFICUR, QString)),this,SLOT(HandleUpdateAnswerEvent(ficur::jfFICUR, QString)));
  connect(downloader,SIGNAL(sendNothing(bool)),this,SLOT(HandleNothingSignal(bool)));
  connect(downloader,SIGNAL(sendFicCount(size_t)),this,SLOT(HandleFicCount(size_t)));

  // done
  QApplication::restoreOverrideCursor();
  thread_object->start();
}

//------------------------------------------------
void jfFic_UpdateDisplay::HandleDone(bool okay) {
  QString newlabel;
  // check count
  newlabel = "Check Done. " + GetCountLabel();
  fic_count->setText(newlabel);
  emit relayDoneSignal();
}

//------------------------------------------------
void jfFic_UpdateDisplay::HandleNothingSignal(bool fic) {
  if (fic) {
    fic_count->setText("No properly formatted fanfics found.");
  }
  else {
    fic_count->setText("No HTML files found.");
  }
}
//------------------------------------------------

void jfFic_UpdateDisplay::HandleUpdateAnswerEvent(ficur::jfFICUR uresult, QString ficname) {
  // variables
  QString resmsg;
  // starting
  dfics++;
  if (uresult != ficur::NotUpdated) {
    resmsg = "The Fic : " + ficname;

    if ( uresult == ficur::Updated) {
      resmsg += " has been updated!";
      ufics++;
    }
    else if ( uresult == ficur::Missing) {
      resmsg += " is missing.";
      mfics++;
    }
    else if ( uresult == ficur::ParseError) {
      resmsg += " failed the check due to a parsing error.";
      ffics++;
    }
    else if ( uresult == ficur::Failed) {
      resmsg += " failed to download.";
      ffics++;
    }
    else if ( uresult == ficur::Halted) {
      resmsg += " did not download because the user halted things.";
      ffics++;
    }
    else assert(false);
    results_display->appendPlainText(resmsg);
  }
  fic_count->setText(GetCountLabel());
}
//------------------------------------------------
void jfFic_UpdateDisplay::HandleFicCount(size_t inficc) {
  tfics = inficc;
  QString lab_msg = "A total of ";
  lab_msg += QString::number(tfics) + " fanfic";
  if (tfics==1) lab_msg += " is";
  else lab_msg += "s are";
  lab_msg += " being checked.";
  fic_count->setText(lab_msg);
}
//------------------------------------------------
void jfFic_UpdateDisplay::ThreadDone() {
  // disconnecting
  disconnect(thread_object, 0,downloader ,0);
  disconnect(downloader,0,thread_object,0);
  disconnect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  main_display->DisconnectDownloader(downloader);
  // deleting
  downloader->deleteLater();
  thread_object->deleteLater();
  downloader = NULL;
  thread_object = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods
//------------------------------------------------
bool jfFic_UpdateDisplay::SetupDownloaderAndThread() {
  const QString fname = "jfFFN_CatDownDialog::SetupDownloaderAndThread";
  if (downloader == NULL) return false;
  // setting up the thread and downloader
  thread_object = new QThread();
  downloader->moveToThread(thread_object);
  // connecting signals and slots
  bool testoo = main_display->ConnectAndSetPauser(downloader);
  /**/jerror::AssertLog(testoo,fname,"Connecting and Setting Pauser FAILED!!!!");

  // standard thread/downloader connections
  connect(thread_object, SIGNAL(started()),downloader ,SLOT(StartProcessing()));
  connect(downloader,SIGNAL(AllFinished()),thread_object,SLOT(quit()));
  connect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  return true;
}

//------------------------------------------------
QString jfFic_UpdateDisplay::GetCountLabel() {
  QString reslabel;
  bool extra = false;
  reslabel = QString::number(dfics) + " of " + QString::number(tfics) + " Fic";
  if (tfics > 1) reslabel += "s";
  reslabel  += " checked. ";
  if (ufics>0) {
    reslabel += QString::number(ufics) + " Updated";
    extra = true;
  }
  if (mfics>0) {
    if (extra) reslabel += ", ";
    reslabel += QString::number(mfics) + " Missing";
    extra = true;
  }
  if (ffics>0) {
    if (extra) reslabel += ", ";
    reslabel += QString::number(ffics) + " Failed";
    extra = true;
  }
  if (extra) reslabel += ".";
  return reslabel;
}

/*****************************************************************************/
