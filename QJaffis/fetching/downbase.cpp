/*****************************************************************************
Name    : downbase.cpp
Basic   : Base class for download controller
Author  : John Q Metro
Started : July 7, 2021
Updated :
******************************************************************************/
#ifndef DOWNBASE_H
    #include "downbase.h"
#endif // DOWNBASE_H
//-------------------------------
#include <assert.h>
#include <math.h>
#include <QCoreApplication>
/*****************************************************************************/

jfDownloaderBase::jfDownloaderBase():jfThreadBase() {
  pagecount = 0;
  started = false;
  skip_on_fail = false;
  all_done = false;
}
//----------------------------------------------------------
bool jfDownloaderBase::setPauser(jfPauseStop* in_pauser) {
  if (in_pauser==NULL) return false;
  if (started) return false;
  pauser = in_pauser;
  return true;
}
//----------------------------------------------------------
jfDownloaderBase::~jfDownloaderBase() {

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfDownloaderBase::HandleResults(jfDownloadResults* in_results) {
    const QString fname = "jfDownloaderBase::HandleResults";

    assert(in_results != NULL);
    current_error = (in_results->fetchResults).why;
    /**/tLog(fname,1,jf_Aftermath2String(current_error));
    // case 1 : okay result
    if (current_error == jfa_DONE) {
        /**/tLog(fname,2);
        void* tdata = in_results->resultData;
        bool xres = ProcessResults(tdata);
        /**/tAssert(xres,fname,"Processing downloaded results failed!");
        assert(xres);
        emit SendItemDone();
        /**/tLog(fname,4);
        return true;
    }
    // case 2 : slip
    else if (current_error == jfa_SKIP) {
      /**/tLog(fname,5);
      (in_results->fetchResults).halt = false;
      emit SendError(in_results->fetchResults);
      /**/tLog(fname,6);
      return true;
    }
    // otherwise, case 3: fail!
    else {
      /**/tLog(fname,7);
      (in_results->fetchResults).halt = !skip_on_fail;
      if (in_results->wasRedirected) {
        /**/tLog(fname,8);
        (in_results->fetchResults).why_more = "Redirected to : " + (in_results->newURL);
      }
      /**/tLog(fname,9);
      emit SendError(in_results->fetchResults);
      return false;
    }
}
//----------------------------------------------
void jfDownloaderBase::SendHaltMessage() {
  jfFetchErrorBundle out_error;
  out_error.halt = true;
  out_error.why = jfa_HALT;
  out_error.why_more = "User requested halt.";
  emit SendError(out_error);
}

//------------------------------------
void jfDownloaderBase::AllDone(bool okay) {
  const QString fname = "jfDownloaderBase::AllDone";
  /**/tLog(fname,1);
  emit SendAllDone(okay);
  /**/tLog(fname,2);
  pauser->RegisterStop(current_error);
  /**/tLog(fname,3);
  started = false;
  CloseLog();
  /**/tLog(fname,4);
  emit AllFinished();
}
/*****************************************************************************/
