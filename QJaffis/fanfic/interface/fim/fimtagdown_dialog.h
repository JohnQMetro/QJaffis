/******************************************************************************
Name    :   fimtagdown_dialog.h
Author  :   John Q Metro
Purpose :   Dialog displayed when downloading FIM Tag Data
Created :   December 29, 2017
Updated :   December 29, 2017
******************************************************************************/
#ifndef FIMTAGDOWN_DIALOG_H
    #define FIMTAGDOWN_DIALOG_H
#endif // FIMTAGDOWN_DIALOG_H
//---------------------------------------
#ifndef FIMTAG_PARSE_H
    #include "../../threads/fim/fimtag_parse.h"
#endif // FIMTAG_PARSE_H
#ifndef ONEPAGE_H
    #include "../../../fetching/onepage.h"
#endif // ONEPAGE_H
//---------------------------------------
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
/*****************************************************************************/
class jfFIMTag_DownloadDialog : public QDialog {
    Q_OBJECT
  public:
    jfFIMTag_DownloadDialog(QWidget* parent);
  public slots:
    void HandleStartClick(bool checked);
    void HandleStart(bool okay);
    void DownloadDone();
    void HandleDone(bool okay);
    void ThreadDone();
  protected:
    // helper methods for setup and completion
    void SetupObjectsAndConnect();
    void ThreadFinished();
    // GUI Stuff
    QVBoxLayout* main_sizer;
    QLabel* info;
    QPushButton* doButton;
    // necessary objects
    jfFIMTag_Extractor* parser;
    jfOnePageGetter* downloader;
    QThread* fetch_thread;
};

/*****************************************************************************/
