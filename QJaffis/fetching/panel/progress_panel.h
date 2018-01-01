/*****************************************************************************
Name    : progress_panel.h
Basic   : Defines abstract panel with progress bar that shows download info
Author  : John Q Metro
Started : March 31, 2013
Updated : June 23, 2016

******************************************************************************/
#ifndef PROGRESS_PANEL_H
#define PROGRESS_PANEL_H
#endif // PROGRESS_PANEL_H
//------------------------------------
#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
//-------------------------------------
#ifndef CONNECT_HELPERS_H
  #include "../connect_helpers.h"
#endif // CONNECT_HELPERS_H
#ifndef THREADBASE_H
  #include "../threadbase.h"
#endif // THREADBASE_H
/******************************************************************************/
/* Base class for a GUI item that displays download progress, and can be used to
  launch, pause, and stop the download. Note that it is up to the child classes to
  do any arranging of widgets */
class jfProgressPanelBase : public QWidget {
    Q_OBJECT
  public:
    // constructor
       jfProgressPanelBase(QWidget* parent);
    // getting info
       bool HasStarted() const;
       bool IsWorking() const;
       bool Error() const;
       jfFETCH_AFTERMATH GetLastError() const;
       jfPauseStop* GetPauser();
       bool ConnectThread(jfThreadBase* thread_in);
       bool DisConnectThread(jfThreadBase* thread_in);
    // because sometimes the parent needs to do things before launch
       bool CancelStart();
       virtual bool DoReset() = 0;
    // special methods
       bool HaltProgress();
  signals:
       void SendStart();
       void SendPanelDone(bool okay);
  public slots:
    // slots for the buttons
      void HandleStartStop(bool checked);
      void HandlePauseResume(bool checked);
    // slots for handling the pauser
      void HandlePaused(bool is_after);
      void HandleStopping(bool is_after);
      void HandleStopped(jfFETCH_AFTERMATH final_error);
    // slots for handling thread messages
      void HandleStart(bool all_okay);
      void HandleItemCount(int eitemcount);
      void HandleNewItem(struct jfItemInfoMsg itemdata);
      void HandleNewItemAction(QString action_name);
      void HandleItemDone();
      void HandleError(struct jfFetchErrorBundle error_info);
      void HandleAllDone(bool isfinished);
      void HandleReset();
  protected:
    // helper methods
      void SetWorking();
      bool Reset();
      void SetLabel(QString extra);
      virtual void MoreError(bool dohalt) = 0;
      void StartStop();
    // internal gui methods
      void CreateWidgets();
      virtual void DoLayout() = 0;
    // thread startup methods
      bool MakePauser(); // created the pauser and connects the slots
      bool DeletePauser();
    // gui elements
      QLabel* main_status; // holds a 1 word (usually) status word
      QProgressBar* item_bar; // progress bar for pages/items
      QLabel* item_status; // more detailed status info
      QPushButton* start_stop;
      QPushButton* pause_resume;
    // internal thread related data
      jfPauseStop* pauser;
    // internal status data
      jfItemInfoMsg lastinfo;
      size_t itemindex;
      QString statstring;
      jfFetchErrorBundle lasterror;
      bool working;
      bool started;
      bool sig_start;

};

/******************************************************************************/
