/*****************************************************************************
Name    : connect_helpers.cpp
Basic   : Defines auxiliary classes and types for downloading
Author  : John Q Metro
Started : March 20, 2013
Updated : April 6, 2013

******************************************************************************/
#ifndef CONNECT_HELPERS_H
  #include "connect_helpers.h"
#endif // CONNECT_HELPERS_H
//------------------------------------
#include <QMutexLocker>
#include <QCoreApplication>
//-----------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//****************************************************************************
// fetch error to string
QString jf_FetchErr2String(jfFETCH_ERROR inval, bool limit) {
  switch (inval) {
    case jff_NOERROR  : return "No Error";
    case jff_TRYAGAIN :
      if (limit) return "Retry Limit Reached";
      else return "Retryable Error";
    case jff_REDIRECTION :
      if (limit) return "Retry Limit Reached with Redirection";
      else return "Request Redirected";
    case jff_MISSING    : return "Page is Missing";
    case jff_FALIURE    : return "Non-Temporary Error";
    default : return "Halted by User";
  }
}

//============================================================================
/* Interaction object. GUI methods use this object to pause/resume the thread,
  and request stopping. The object is passed to thread data, and functions there
  must call the specified methods to pause or resume, or check for a a stop. */
//+++++++++++++++++++++++++++++++++++++++++++
// the constructor
jfPauseStop::jfPauseStop() {
  QMutexLocker lk(&protect_data);
  istatus = jfps_NONE;
  status_after = jfps_NONE;
}
//+++++++++++++++++++++++++++++++++++++++++++
// methods used by the gui to request pause/resume or stop
//---------------------
// general pause or resume request
bool jfPauseStop::ReqPauseResume() {
  QMutexLocker lk(&protect_data);
  // no pause or resume if we are stopping
  if ((istatus==jfps_STOPPING) || (istatus==jfps_STOPPED)) return false;
  // if we are pausing, we undo the pause (resume)
  if ((istatus==jfps_PAUSING) || (istatus==jfps_PAUSED)) {
    istatus = jfps_NONE;
    pauser.unlock();
  }
  // otherwise we pause
  else {
    istatus = jfps_PAUSING;
    pauser.lock();
  }
  return true;
}
//---------------------
// request pause or resume of after category lock
bool jfPauseStop::ReqPauseResumeAfter() {
  QMutexLocker lk(&protect_data);
  // no pause or resume if we are stopping
  if ((status_after==jfps_STOPPING) || (status_after==jfps_STOPPED)) return false;
  // if we are pausing, we undo the pause (resume)
  if ((status_after==jfps_PAUSING) || (status_after==jfps_PAUSED)) {
    status_after = jfps_NONE;
    pause_aftercat.unlock();
  }
  // otherwise we pause
  else {
    status_after = jfps_PAUSING;
    pause_aftercat.lock();
  }
  return true;
}
//---------------------
// resume method work on both pause types
bool jfPauseStop::GeneralResume() {
  QMutexLocker lk(&protect_data);
  bool change = false;
  // checking the immediate one...
  if ((istatus==jfps_PAUSING) || (istatus==jfps_PAUSED)) {
    change = true;
    istatus = jfps_NONE;
    pauser.unlock();
  }
  // checking the after one
  if ((status_after==jfps_PAUSING) || (status_after==jfps_PAUSED)) {
    change = true;
    status_after = jfps_NONE;
    pause_aftercat.unlock();
  }
  return change;
}
//---------------------
// requests an immediate stop
bool jfPauseStop::ReqStop() {
  QMutexLocker lk(&protect_data);
  // already stopping
  if ((istatus==jfps_STOPPING) || (istatus==jfps_STOPPED)) return false;
  // we are pasused or have requested a pause
  bool pausing = ((istatus==jfps_PAUSING) || (istatus==jfps_PAUSED));
  istatus = jfps_STOPPING;
  if (pausing) pauser.unlock();
  // note... what if there is a pause after...?
  pausing = ((status_after==jfps_PAUSING) || (status_after==jfps_PAUSED));
  status_after = jfps_STOPPING;
  if (pausing) pause_aftercat.unlock();
  return true;
}
//---------------------
// requests stopping after the current category
bool jfPauseStop::ReqStopAfter() {
  QMutexLocker lk(&protect_data);
  // already stopping
  if ((status_after==jfps_STOPPING) || (status_after==jfps_STOPPED)) return false;
  bool pausing = ((status_after==jfps_PAUSING) || (status_after==jfps_PAUSED));
  status_after = jfps_STOPPING;
  if (pausing) pause_aftercat.unlock();
  return true;
}
//---------------------
bool jfPauseStop::CancelStopAfter() {
  QMutexLocker lk(&protect_data);
  if (status_after!=jfps_STOPPING) return false;
  status_after = jfps_NONE;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++
// methods called by the thread
//---------------------
// should be called frequently, if true, we halt
bool jfPauseStop::CheckPauseStop() {
  const QString fname = "jfPauseStop::CheckPauseStop";
  protect_data.lock();
  if (istatus==jfps_STOPPING) {
    protect_data.unlock();
    emit SendStopping(false);
    return true;
  }
  if (istatus==jfps_PAUSING) {
    istatus = jfps_PAUSED;
    protect_data.unlock();
    emit SendPaused(false);
    pauser.lock();
    protect_data.lock();
    pauser.unlock();
    if (istatus==jfps_STOPPING) {
      protect_data.unlock();
      emit SendStopping(false);
      return true;
    }
    else {
      protect_data.unlock();
      return false;
    }
  }
  else {
    protect_data.unlock();
    return false;
  }
}
//---------------------
// called only after a category/section
bool jfPauseStop::CheckPauseStopAfter() {
  protect_data.lock();
  if (status_after==jfps_STOPPING) {
    protect_data.unlock();
    emit SendStopping(true);
    return true;
  }
  if (status_after==jfps_PAUSING) {
    status_after = jfps_PAUSED;
    protect_data.unlock();
    emit SendPaused(true);
    pause_aftercat.lock();
    protect_data.lock();
    pause_aftercat.unlock();
    if (status_after==jfps_STOPPING) {
      protect_data.unlock();
      emit SendStopping(true);
      return true;
    }
    else {
      protect_data.unlock();
      return false;
    }
  }
  else {
    protect_data.unlock();
    return false;
  }
}

//---------------------
// used to set the status to stop
void jfPauseStop::RegisterStop(jfFETCH_AFTERMATH finalerr) {
  QMutexLocker lk(&protect_data);
  istatus = jfps_STOPPED;
  status_after = jfps_STOPPED;
  emit SendStopped(finalerr);
}
//+++++++++++++++++++++++++++++++++++++++++++
// informational methods
//------------------------
jfps_STATUS jfPauseStop::GetStatus() const {
  QMutexLocker lk(&protect_data);
  return istatus;
}
//------------------------
jfps_STATUS jfPauseStop::GetStatusAfter() const {
  QMutexLocker lk(&protect_data);
  return status_after;
}
//============================================================================
QString jf_Aftermath2String(jfFETCH_AFTERMATH inval) {
    switch (inval) {
        case jfa_DONE      : return "No Error";
        case jfa_FETCHFAIL : return "Failed to get page";
        case jfa_MISSING   : return "Page is Missing";
        case jfa_PARSEERR  : return "Parse Error";
        case jfa_SKIP      : return "Skipping";
        default            : return "Halted by User";
    }
}
//-------------------------------------------
jfFETCH_AFTERMATH jf_Error2AfterMath(jfFETCH_ERROR inerr) {
    switch (inerr) {
        case jff_NOERROR : return jfa_DONE;
        case jff_MISSING : return jfa_MISSING;
        case jff_HALTED  : return jfa_HALT;
        default          : return jfa_FETCHFAIL;
    }
}

//****************************************************************************
