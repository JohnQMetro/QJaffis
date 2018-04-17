/******************************************************************************
// Name:        testpanel.h
// Author :     John Q Metro
// Purpose :    The interface for the download/parse testing
// Created:     March 29, 2018
// Updated:     April 17, 2018
******************************************************************************/
#ifndef TESTPANEL_H
    #define TESTPANEL_H
#endif // TESTPANEL_H
//----------------------------------
#ifndef TESTTHREAD_H
    #include "testthread.h"
#endif // TESTTHREAD_H
#ifndef PROGRESS_PANEL2_H
  #include "../fetching/panel/progress_panel2.h"
#endif // PROGRESS_PANEL2_H
//----------------------------------
#include <QWidget>
#include <QPlainTextEdit>
/*****************************************************************************/
// specialized progress panel for test (mainly to relay results)
class jfProgressPanelTest: public jfProgressPanelOne {
    Q_OBJECT
  public:
    jfProgressPanelTest(QWidget* parent = NULL);
    // specials
    bool ConnectAndSetPauser(jfTestDownloader* the_downloader);
  signals:
    void SendChecked(QString what, jfFETCH_AFTERMATH result);
  public slots:
    virtual void HandleItemDone() override;
    virtual void HandleError(struct jfFetchErrorBundle error_info) override;
  protected:
};

//============================================================================
// the main display panel for updating downloaded fanfics
class jfTestDisplay : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfTestDisplay(QWidget* parent = NULL);
  // other methods
    void Reset();
    bool StopUpdate();
  signals:
    void relayDoneSignal();
  public slots:
    void HandleStart();
    void HandleDone(bool okay);
    void ThreadDone();
    void HandleTestAnswerEvent(QString what, jfFETCH_AFTERMATH result);
  protected:
    // custom methods
    bool SetupDownloaderAndThread();
    QString GetCountLabel() const;
    // GUI widgets
    jfProgressPanelTest* progress_bar;
    QLabel* check_count;
    QPlainTextEdit* results_display;
    // layouts
    QVBoxLayout *main_sizer;
    // thread data
    bool launched;
    size_t ttest,dtest,oktest,ftest,pftest;
    jfTestDownloader* downloader;
    QThread* thread_object;
};




/*****************************************************************************/
