/******************************************************************************
Name    :   gcat_dialog.h
Author  :   John Q Metro
Purpose :   Dialog for Generic Catalog Downloading
Created :   August 19, 2012
Conversion to Qt started March 10, 2014
Updated :   July 15, 2016
******************************************************************************/
#ifndef GCAT_DIALOG_H_INCLUDED
#define GCAT_DIALOG_H_INCLUDED
#endif // GCAT_DIALOG_H_INCLUDED
//-----------------------------------------
#ifndef GENCAT_THREAD_H
  #include "../threads/gencat_thread.h"
#endif // GENCAT_THREAD_H
#ifndef PROGRESS_PANEL2_H
  #include "../../fetching/panel/progress_panel2.h"
#endif // PROGRESS_PANEL2_H
//----------------------------------------------
#include <QDialog>
#include <Qthread>
//***************************************************************************
class jfGenCat_DownDialog : public QDialog {
    Q_OBJECT
  public:
    // the constructor
    jfGenCat_DownDialog(QWidget* parent);
  public slots:
    void HandleStart();
    void HandleDone(bool);
    void ThreadDone();
  signals:
    void SendDoneOkay();
  protected:
    // custom methods
    bool SetupDownloaderAndThread();
    // virtual methods
    virtual jfGeneralCategoryDownloaderBase* MakeTypedThread() const = 0;
    // gui elements
    jfProgressPanelSingle* mainstatus;
    // we need quite a number of sizers to arrange these...
    QVBoxLayout* main_sizer;
    // data links
    jfGenCatManager* inholder;
    jfGenSecData *slinks;
    QString dtype;
    // downloader thread stuff
    jfGeneralCategoryDownloaderBase*  main_downloader;
    QThread* fetch_thread;

};
//***************************************************************************
