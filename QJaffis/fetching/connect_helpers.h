/*****************************************************************************
Name    : connect_helpers.h
Basic   : Declares auxiliary classes and types for downloading
Author  : John Q Metro
Started : March 19, 2013
Updated : June 17, 2016

******************************************************************************/
#ifndef CONNECT_HELPERS_H
#define CONNECT_HELPERS_H
#endif // CONNECT_HELPERS_H

#ifndef PAGEGETTER_H
  #include "pagegetter.h"
#endif // PAGEGETTER_H
//------------------------------------
#include <QObject>
#include <QMutex>
//****************************************************************************
// fetch error to string
QString jf_FetchErr2String(jfFETCH_ERROR inval, bool limit);

/* When it comes to errors, the below is for post fetch and process */
enum jfFETCH_AFTERMATH {jfa_DONE,  // no error
                        jfa_MISSING,
                        jfa_PARSEERR,
                        jfa_FETCHFAIL,
                        jfa_HALT,
                        jfa_SKIP,
                        };
//===============================================================================
/* Interaction object. GUI methods use this object to pause/resume the thread,
  and request stopping. The object is passed to thread data, and functions there
  must call the specified methods to pause or resume, or check for a a stop. */

// we have here an enumerated type for the status
enum jfps_STATUS {jfps_NONE, jfps_PAUSING, jfps_PAUSED, jfps_STOPPING, jfps_STOPPED};

/* note that we have 2 levels here, one regular (stop or pause asap), and one
  for 'after' (stop or pause after we finish the current category/section). */
class jfPauseStop : public QObject {
    Q_OBJECT
  public:
      jfPauseStop(); // the constructor
    // methods used by the gui to request pause/resume or stop
      bool ReqPauseResume();      // general pause or resume request
      bool ReqPauseResumeAfter(); // request pause or resume of after category lock
      bool GeneralResume();       // resume method work on both pause types
      bool ReqStop();             // requests an immediate stop
      bool ReqStopAfter();        // requests stopping after the current category
      bool CancelStopAfter();     // cancels a stop after (if we call it before the stop)
    // methods called by the thread
      bool CheckPauseStop();      // should be called frequently, if true, we halt
      bool CheckPauseStopAfter(); // called only after a category/section
    // informational methods
      jfps_STATUS GetStatus() const;
      jfps_STATUS GetStatusAfter() const;
      void RegisterStop(jfFETCH_AFTERMATH finalerr);
  // signals
  signals:
      void SendPaused(bool is_after);
      void SendStopping(bool is_after);
      void SendStopped(jfFETCH_AFTERMATH final_error);

  protected:
    // the pause-resume mutexes
    QMutex pauser;         // for anything pausing
    QMutex pause_aftercat; // for pausing after we finish a catgeory/group
    // internal status info
    mutable QMutex protect_data;
    jfps_STATUS istatus;
    jfps_STATUS status_after;
};

//========================================================================
/* when sending info from a downloader thread to a progress bar display
  (via signals), some info needs to be sent as well */
struct jfItemInfoMsg {
  int item_index;  // item index
  int part_count;  // part count
  QString item_name;  // name of the item (may be empty)
  QString item_label; // label of the item (like 'page', or 'file', or 'section')
  QString startaction;
};
Q_DECLARE_METATYPE(jfItemInfoMsg);
//=======================================================================
QString jf_Aftermath2String(jfFETCH_AFTERMATH ina);
jfFETCH_AFTERMATH jf_Error2AfterMath(jfFETCH_ERROR inerr);
//=======================================================================
/* When a page fetch fails, we send info on whether we are skipping or halting */
struct jfFetchErrorBundle {
    bool              halt;     // skipping or halting
    jfFETCH_AFTERMATH why;      // what is the error?
    QString           why_more; // extra error message (parse error, etc)
};

Q_DECLARE_METATYPE(jfFetchErrorBundle);

//****************************************************************************
