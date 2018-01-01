/******************************************************************************
Name    :   dirupdate_thread.cpp
Author  :   John Q Metro
Purpose :   Defines the thread that does directory updates (intended for
              syncing directories of downloaded fanfics).
Created :   July 5, 2015
Updated :   August 2, 2016
******************************************************************************/
#ifndef DIRUPDATE_THREAD_H_INCLUDED
  #include "dirupdate_thread.h"
#endif // DIRUPDATE_THREAD_H_INCLUDED
//----------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
/*****************************************************************************/
// constructor
jfDirUpdate_Thread::jfDirUpdate_Thread():jfThreadBase() {
  dupdate_obj = NULL;
  started = false;
  pauser = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// passing parameters
void jfDirUpdate_Thread::SetDirectories(QString sdir_in, QString tdir_in, bool casesen) {
  if (started) return;
  if ((!sdir_in.isEmpty()) && (!tdir_in.isEmpty())) {
    sdirx = sdir_in;
    tdirx = tdir_in;
  }
}
//------------------------------------------------
bool jfDirUpdate_Thread::setPauser(jfPauseStop* in_pauser) {
  if (in_pauser==NULL) return false;
  if (started) return false;
  pauser = in_pauser;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfDirUpdate_Thread::StartProcessing() {
  started = true;
  bool xres;
  started = true;
  initLog("DirectoryUpdater");
  xres = Startup();
  jfSleepThread::msleep(1000);
  // if (xres) xres = TextResults();
  if (xres) xres = DoUpdateActions();

  AllDone(xres);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfDirUpdate_Thread::AllDone(bool okay) {
  emit SendAllDone(okay);
  pauser->RegisterStop((okay)?jfa_DONE:jfa_FETCHFAIL);
  started = false;
  CloseLog();
  emit AllFinished();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal methods
//---------------------------------------
bool jfDirUpdate_Thread::Startup() {
    const QString funcname = "jfDirUpdate_Thread::Startup";
  bool xres;
  // initials
  emit SendStart(true);
  emit SendItemCount(4);
  // creating
  dupdate_obj = new jfDirectoryUpdater(false);
  // loading source directory
  /**/tLog(funcname,1);
  infoToSend.item_index = -1;
  infoToSend.part_count = -1;
  infoToSend.item_name = "";
  infoToSend.startaction = "Loading";
  infoToSend.item_label = "Source directory content info";
    /**/tLog(funcname,2);
  emit SendNewItem(infoToSend);

  xres = dupdate_obj->LoadSource(sdirx);
  /**/tLogB(funcname,3,xres);
  if (!xres) {
    emit itemFailed(false);
    return false;
  }
  emit SendItemDone();
  /**/tLog(funcname,4);
  // loading target directory
  infoToSend.item_label = "Target directory content info";
  emit SendNewItem(infoToSend);
  /**/tLog(funcname,5);
  xres = dupdate_obj->LoadTarget(tdirx);
  /**/tLogB(funcname,6,xres);
  if (!xres) {
    emit itemFailed(false);
    return false;
  }
  emit SendItemDone();

  jfSleepThread::msleep(1000);
  /**/tLog(funcname,7);
  // calling the comparison function
  infoToSend.startaction = "Comparing";
  infoToSend.item_label = "the directory contents";


  xres = dupdate_obj->CompareSourceAndTarget();
  /**/tLogB(funcname,8,xres);
  if (!xres) {
    emit itemFailed(false);
    return false;
  }
  emit SendItemDone();
  /**/tLog(funcname,9);
  // done
  return true;
}
//------------------------------------------------
bool jfDirUpdate_Thread::TextResults() {
  // constants and variables
  const QString du_filename = "dirupdate.txt";
  const QString funcname = "jfDirUpdate_Thread::TextResults()";
  QString redata;
  bool xres;
  // calling the comparison function
  /**/tLog(funcname,1);
  infoToSend.startaction = "Writing";
  infoToSend.item_label = "Comparison results to a file";
  emit SendNewItem(infoToSend);
  /**/tLog(funcname,2);
  // the loop
  dupdate_obj->ResetTargetIndex();
  /**/tLog(funcname,3);
  while (dupdate_obj->NextTarget()) {
      /**/tLog(funcname,4);
    redata += dupdate_obj->GetCurrentTargetInfo();
    /**/tLog(funcname,5);
  }
  // writing
  xres = ToFile(redata,du_filename);
  /**/tLogB(funcname,6,xres);
  if (!xres) {
    emit itemFailed(false);
    return false;
  }
  emit SendItemDone();
  // done
  return true;
}
//------------------------------------------------------
bool jfDirUpdate_Thread::DoUpdateActions() {
  const QString fname = "jfDirUpdate_Thread::DoUpdateActions";
  // variables
  size_t tindex, tcount;
  QString action, filename;
  bool checkval = true;
  // preparing for the loop
  /**/tLog(fname,1);
  tcount = dupdate_obj->TargetCount();
  emit SendItemCount(3+tcount);

  dupdate_obj->ResetTargetIndex();
  /**/tLogS(fname,2,tcount);
  infoToSend.part_count = tcount;
  infoToSend.item_label = "file";
  // the loop
  for (tindex = 1; tindex<=tcount; tindex++) {
    // setting up for action
    dupdate_obj->NextTarget();
    action = dupdate_obj->GetCurrentTargetAction();
    /**/tLog(fname,3,action);
    filename = dupdate_obj->CurrentTargetPath();
    /**/tLog(fname,4,filename);
    infoToSend.item_index = tindex;
    infoToSend.startaction = action;
    infoToSend.item_name = filename;
    emit SendNewItem(infoToSend);

    // action time!
    checkval = dupdate_obj->CurrentTargetOkay();
    /**/tLogB(fname,5,checkval);
    if (checkval) checkval = dupdate_obj->DoCurrentTargetAction();
    // post check
    if (checkval) emit SendItemDone();
    else {
      emit itemFailed(false);
      break;
    }
    /**/tLog(fname,6);
  }
  // after loop
  return checkval;
}
/*****************************************************************************/
