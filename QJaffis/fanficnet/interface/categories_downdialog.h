/*****************************************************************************
Name    : categories_downdialog.h
Basic   : Fanfiction.Net category downloader dialog
Author  : John Q Metro
Started : July 22, 2016
Updated : August 5, 2016

******************************************************************************/
#ifndef CATEGORIES_DOWNDIALOG_H
  #define CATEGORIES_DOWNDIALOG_H
#endif // CATEGORIES_DOWNDIALOG_H
//-------------------------------------
#ifndef PROGRESS_PANEL2_H
  #include "../../fetching/panel/progress_panel2.h"
#endif // PROGRESS_PANEL2_H
#ifndef FFNCATTHREAD_H
  #include "../threads/ffncatthread.h"
#endif // FFNCATTHREAD_H
//-------------------------------------
#include <QThread>
#include <QDialog>
#include <QCheckBox>
/*****************************************************************************/
// a modified multiple categories progresspanel for ffn categories downloading
class jfProgPanelFFNCategories : public jfProgPanelMultiBase {
    Q_OBJECT
  public:
    // constructor
    jfProgPanelFFNCategories(QWidget* parent = NULL);
    bool ConnectAndSetPauser(jfFFN_CategoryDownloader* the_downloader);
    void DisconnectDownloader(jfFFN_CategoryDownloader* the_downloader);
  public slots:
    void HandleSkippedPages(size_t skipcount);
    void HandleOneThingSection(QString action, QString what);
  protected:

};
//=================================================================
class jfFFN_CatDownDialog : public QDialog {
    Q_OBJECT
  public:
    jfFFN_CatDownDialog(QWidget* parent);
  signals:
    void SendCatDone(bool isokay);
  public slots:
    virtual void HandleStart();
    virtual void HandleDone(bool isokay);
    void ThreadDone();
  protected:
    // custom methods
    bool SetupDownloaderAndThread();
    // GUI Widgtes
    QCheckBox* update_cbox;
    jfProgPanelFFNCategories* mpanel;
    QVBoxLayout* mlayout;
    // thread objects
    jfFFN_CategoryDownloader* downloader;
    QThread* thread_object;

};
/*****************************************************************************/
