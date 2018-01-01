/******************************************************************************
Name    :   dirupdate_thread.h
Author  :   John Q Metro
Purpose :   Declares the thread that does directory updates (intended for
              syncing directories of downloaded fanfics).
Created :   July 5, 2015
Updated :   August 2, 2016
******************************************************************************/
#ifndef DIRUPDATE_THREAD_H_INCLUDED
#define DIRUPDATE_THREAD_H_INCLUDED
#endif // DIRUPDATE_THREAD_H_INCLUDED
//----------------------------------
#ifndef DIRUPDATE_H_INCLUDED
  #include "../../core/utils/dirupdate.h"
#endif // DIRUPDATE_H_INCLUDED
#ifndef THREADBASE_H
  #include "../../fetching/threadbase.h"
#endif // THREADBASE_H

/*****************************************************************************/
class jfDirUpdate_Thread : public jfThreadBase {
    Q_OBJECT
  public:
    // constructor
    jfDirUpdate_Thread();
    // implemented virtual methods
    // virtual void SendDone(jfDown::ResultAction done_status);
    // passing parameters
    void SetDirectories(QString sdir_in, QString tdir_in, bool casesen);
    bool setPauser(jfPauseStop* in_pauser);
  signals:
    void itemFailed(bool halt);
  public slots:
     virtual void StartProcessing();
  protected:
    void AllDone(bool okay);
    // internal methods
    bool Startup();
    bool TextResults();
    bool DoUpdateActions();
  // thread data
    bool started;
    jfPauseStop* pauser;
    jfItemInfoMsg infoToSend;
  // action data
    QString sdirx, tdirx;
    jfDirectoryUpdater* dupdate_obj;

};
/*****************************************************************************/
