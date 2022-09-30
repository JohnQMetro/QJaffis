/*****************************************************************************
Name    : progress_panel.cpp
Basic   : Defines abstract panel with progress bar that shows download info
Author  : John Q Metro
Started : April 1, 2013
Updated : July 4, 2021 (wait and stop wait)

******************************************************************************/
#ifndef PROGRESS_PANEL_H
  #include "progress_panel.h"
#endif // PROGRESS_PANEL_H
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------
#include <assert.h>
#include <QApplication>
/******************************************************************************/
/* Base class for a GUI item that displays download progress, and can be used to
  launch, pause, and stop the download. Note that it is up to the child classes to
  do any arranging of widgets */
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfProgressPanelBase::jfProgressPanelBase(QWidget* parent):QWidget(parent) {
  // initializing values
  pauser = NULL;
  Reset();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// getting info
bool jfProgressPanelBase::HasStarted() const {
  QApplication::restoreOverrideCursor();
  return started;
}
//--------------
bool jfProgressPanelBase::IsWorking() const {
  return working;
}
//--------------
bool jfProgressPanelBase::Error() const {
  return (lasterror.why!=jff_NOERROR);
}
//--------------
jfFETCH_AFTERMATH jfProgressPanelBase::GetLastError() const {
  return lasterror.why;
}
//---------------
jfPauseStop* jfProgressPanelBase::GetPauser() {
  return pauser;
}
//--------------
bool jfProgressPanelBase::ConnectThread(jfThreadBase* thread_in) {
  if (thread_in == NULL) return false;
  connect(thread_in, SIGNAL(SendStart(bool)),this, SLOT(HandleStart(bool)));
  connect(thread_in, SIGNAL(SendItemCount(int)),this, SLOT(HandleItemCount(int)));
  connect(thread_in, SIGNAL(SendNewItem(struct jfItemInfoMsg)),this, SLOT(HandleNewItem(struct jfItemInfoMsg)));
  connect(thread_in, SIGNAL(SendNewItemAction(QString)),this, SLOT(HandleNewItemAction(QString)));
  connect(thread_in, SIGNAL(SendItemDone()),this, SLOT(HandleItemDone()));
  connect(thread_in, SIGNAL(SendError(struct jfFetchErrorBundle)),this, SLOT(HandleError(struct jfFetchErrorBundle)));
  connect(thread_in, SIGNAL(SendAllDone(bool)),this, SLOT(HandleAllDone(bool)));
  connect(thread_in, SIGNAL(SendReset()),this, SLOT(HandleReset()));
  return true;
}
//--------------
bool jfProgressPanelBase::DisConnectThread(jfThreadBase* thread_in) {
  if (thread_in == NULL) return false;
  disconnect(thread_in, 0,this, 0);
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfProgressPanelBase::CancelStart() {
  if (!sig_start) return false;
  // changing the button info
  start_stop->setEnabled(true);
  start_stop->setText("Start");
  statstring = "NOT STARTED";
  main_status->setText(statstring);
  sig_start = false;
  QApplication::restoreOverrideCursor();
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// called externally to halt the downloads
bool jfProgressPanelBase::HaltProgress() {
  // starting
  if (!working) return false;
  // stopping
  else {
    StartStop();
    return true;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// slots for the buttons
//--------------
void jfProgressPanelBase::HandleStartStop(bool checked) {
  // starting
  if (!working) {
    // changing the button info
    start_stop->setEnabled(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    start_stop->setText("Stop");
    statstring = "STARTING";
    main_status->setText(statstring);
    // sending the special signal
    sig_start = true;
    emit SendStart();
  }
  // stopping
  else StartStop();
}

//--------------
void jfProgressPanelBase::HandlePauseResume(bool checked) {
  const QString fname = "jfProgressPanelBase::HandlePauseResume";
  bool ispaused = ((pause_resume->text())=="Resume");
  // resuming
  if (ispaused) {
    pause_resume->setText("Pause");
    statstring = "WORKING";
  }
  // pausing
  else {
    pause_resume->setText("Resume");
    statstring = "PAUSING";
    pause_resume->setEnabled(false);
  }
  main_status->setText(statstring);
  pauser->ReqPauseResume();

}

//++++++++++++++++++++++++++++++++++++++++++++++++++
// slots for handling the pauser
//--------------
void jfProgressPanelBase::HandlePaused(bool is_after) {
  statstring = "PAUSED";
  main_status->setText(statstring);
  pause_resume->setEnabled(true);
}
//--------------
void jfProgressPanelBase::HandleStopping(bool is_after) {

}
//--------------
// halt
void jfProgressPanelBase::HandleStopped(jfFETCH_AFTERMATH final_error) {
  lasterror.why = final_error;
  QString errtrans = jf_Aftermath2String(lasterror.why);
  statstring = "HALTED";
  SetLabel(errtrans);
  pause_resume->setEnabled(false);
  main_status->setText(statstring);
  working = false;
  emit SendPanelDone(final_error==jfa_DONE);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// slots for waiting and stopped waiting
//---------------
void jfProgressPanelBase::HandleWaiting(int amount) {
    main_status->setText("WAITING " + QString::number(amount) + "s");
}
//---------------
void jfProgressPanelBase::HandleStoppedWaiting() {
    main_status->setText("WORKING");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// slots for handling thread messages
//--------------
void jfProgressPanelBase::HandleStart(bool all_okay) {
    const QString fname = "jfProgressPanelBase::HandleStart";
    /**/JDEBUGLOG(fname, 1)
  SetWorking();
  working = true;
  start_stop->setEnabled(true);
  pause_resume->setEnabled(true);
}
//--------------
void jfProgressPanelBase::HandleItemCount(int eitemcount) {
  item_bar->setMaximum(eitemcount);
}
//--------------
void jfProgressPanelBase::HandleNewItem(struct jfItemInfoMsg itemdata) {
  lastinfo = itemdata;
  SetLabel("");
}
//--------------
void jfProgressPanelBase::HandleNewItemAction(QString action_name) {
  lastinfo.startaction = action_name;
  SetLabel("");
}
//--------------
void jfProgressPanelBase::HandleItemDone() {
  itemindex++;
  item_bar->setValue(itemindex);
  lastinfo.startaction = "Done with";
  SetLabel("");
  SetWorking();
}
//--------------
void jfProgressPanelBase::HandleError(struct jfFetchErrorBundle error_info) {
  // variables
  QString errtrans;
  // we start
  lasterror = error_info;
  // halting with error
  if (lasterror.halt) {
    errtrans = jf_Aftermath2String(lasterror.why);
    if (!lasterror.why_more.isEmpty()) errtrans += " " + lasterror.why_more;
    SetLabel(errtrans);
  }
  // skipping with errror
  else {
    lastinfo.startaction = "Skipped";
    itemindex++;
    item_bar->setValue(itemindex);
    SetLabel("");
  }
  MoreError(lasterror.halt);
}
//--------------
void jfProgressPanelBase::HandleAllDone(bool isfinished) {
  statstring = "FINISHED";
  main_status->setText(statstring);
  pause_resume->setEnabled(false);
  start_stop->setText("Start");
  start_stop->setEnabled(true);
  working = false;
  emit SendPanelDone(isfinished);
}
//------------------------------------------------
void jfProgressPanelBase::HandleReset() {
  item_bar->setValue(0);
  item_status->setText("");
  itemindex = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//--------------
void jfProgressPanelBase::SetWorking() {
  statstring = "WORKING";
  main_status->setText(statstring);
  QApplication::restoreOverrideCursor();
}
//--------------
bool jfProgressPanelBase::Reset() {
  // initializing values
  DeletePauser();
  started = false;
  working = false;
  statstring = "NOT STARTED";
  itemindex = 0;
  lasterror.why = jfa_DONE;
  sig_start = true;
  return true;
}

//--------------
// this method builds the more specififc status label
void jfProgressPanelBase::SetLabel(QString extra) {
  // building the label, bit by bit
  QString themsg = lastinfo.startaction + " " + lastinfo.item_label;
  if (lastinfo.item_index!=-1) {
    themsg += " " + QString::number(lastinfo.item_index) + " of ";
    if (lastinfo.part_count!=-1) themsg += QString::number(lastinfo.part_count);
    else themsg += "an Unknown amount";
  }
  if (!(lastinfo.item_name.isEmpty())) {
    themsg += " : " + lastinfo.item_name;
  }
  if (!extra.isEmpty()) {
    themsg += " , " + extra;
  }
  // setting the label
  item_status->setText(themsg);
}
//--------------------
void jfProgressPanelBase::StartStop() {
  assert(working);
  // changing the button info
  start_stop->setEnabled(false);
  pause_resume->setEnabled(false);
  start_stop->setText("Start");
  statstring = "STOPPING";
  main_status->setText(statstring);
  // signalling the pauser
  pauser->ReqStop();
  pause_resume->setText("Pause");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
// internal gui methods
void jfProgressPanelBase::CreateWidgets() {
  const QString fname = "jfProgressPanelBase::CreateWidgets";
  main_status = new QLabel("NOT STARTED",this);
  item_bar = new QProgressBar(this);
  item_status = new QLabel("Nothing here yet",this);
  start_stop = new QPushButton("Start",this);
  pause_resume = new QPushButton("Pause",this);
  pause_resume->setEnabled(false);
  connect(start_stop,SIGNAL(clicked(bool)),this,SLOT(HandleStartStop(bool)));
  connect(pause_resume,SIGNAL(clicked(bool)),this,SLOT(HandlePauseResume(bool)));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
// thread startup methods
//--------------
// created the pauser and connects the slots
bool jfProgressPanelBase::MakePauser() {
  if (working) return false;
  pauser = new jfPauseStop();
  connect(pauser, SIGNAL(SendPaused(bool)),this, SLOT(HandlePaused(bool)));
  connect(pauser, SIGNAL(SendStopping(bool)),this, SLOT(HandleStopping(bool)));
  connect(pauser, SIGNAL(SendStopped(jfFETCH_AFTERMATH)),this, SLOT(HandleStopped(jfFETCH_AFTERMATH)));
  connect(pauser, SIGNAL(SendWaiting(int)), this, SLOT(HandleWaiting(int)));
  connect(pauser, SIGNAL(SendStoppedWaiting()), this, SLOT(HandleStoppedWaiting()));
  // DONE
  return true;
}
//-----------------
bool jfProgressPanelBase::DeletePauser() {
  if (pauser!=NULL) {
    disconnect(pauser, SIGNAL(SendPaused(bool)),this, SLOT(HandlePaused(bool)));
    disconnect(pauser, SIGNAL(SendStopping(bool)),this, SLOT(HandleStopping(bool)));
    disconnect(pauser, SIGNAL(SendStopped(jfFETCH_AFTERMATH)),this, SLOT(HandleStopped(jfFETCH_AFTERMATH)));
    delete pauser;
    pauser = NULL;
    return true;
  }
  else return false;
}

/******************************************************************************/

