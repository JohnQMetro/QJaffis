/******************************************************************************
// Name:        ficupdate_panel.h
// Author :     John Q Metro
// Purpose :    The interface for updating downloaded fanfics
// Created:     July 26, 2016
// Updated:     December 23, 2021
******************************************************************************/
#ifndef FICUPDATE_PANEL_H
  #define FICUPDATE_PANEL_H
#endif // FICUPDATE_PANEL_H
//--------------------------------------
#ifndef PROGRESS_PANEL2_H
  #include "../../fetching/panel/progress_panel2.h"
#endif // PROGRESS_PANEL2_H
#ifndef FICUPDATE_THREAD_H
  #include "../threads/ficupdate_thread.h"
#endif // FICUPDATE_THREAD_H
#ifndef WIDGETS3_H_INCLUDED
  #include "../../interface/misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
//--------------------------------------
#include <QLabel>
#include <QCheckBox>
#include <QBoxLayout>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QThread>
/*****************************************************************************/
// a modified multiple categories progress panel for fanfic updates
class jfProgPanelFicUpdating : public jfProgPanelMultiBase {
    Q_OBJECT
  public:
    // constructor
    jfProgPanelFicUpdating(QWidget* parent = NULL);
    bool ConnectAndSetPauser(jfFicUpdateThread* the_downloader);
    void DisconnectDownloader(jfFicUpdateThread* the_downloader);
  public slots:
    void HandleOneThingSection(QString action, QString what);
  protected:

};
//=======================================================================
// the main display panel for updating downloaded fanfics
class jfFic_UpdateDisplay : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfFic_UpdateDisplay(QWidget* parent = NULL);
  // other methods
    void Reset();
    bool StopUpdate();
  signals:
    void relayDoneSignal();
  public slots:
    void HandleStart();
    void HandleDone(bool okay);
    void HandleNothingSignal(bool fic);
    void HandleUpdateAnswerEvent(ficur::jfFICUR uresult, QString ficname);
    void HandleFicCount(size_t inficc);
    void ThreadDone();
  protected:
    // custom methods
    bool SetupDownloaderAndThread();
    QString GetCountLabel();
    // GUI widgets
    jfDirPicker* dir_pick;
    jfLabeledIntEdit* split_guide;
    QCheckBox* main_check;
    QCheckBox* skip_ffn;
    jfProgPanelFicUpdating* main_display;
    QLabel* fic_count;
    QPlainTextEdit* results_display;
    // layouts
    QGroupBox* sbox;
    QVBoxLayout* sbox_layout, *main_sizer;
    QHBoxLayout* check_sizer;
    QHBoxLayout* top_sizer;
    // thread data
    bool launched;
    size_t tfics, dfics, ufics, mfics, ffics;
    jfFicUpdateThread* downloader;
    QThread* thread_object;
};
/*****************************************************************************/
