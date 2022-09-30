/******************************************************************************
// Name:        ffndown_dialog.h
// Author :     John Q Metro
// Purpose :    Declares the dialogs for downloading fanfics
// Created:     July 16, 2011
// Started Qt conversion May 22, 2013
// Updated:     October 13, 2019
******************************************************************************/
#ifndef FFNDOWN_DIALOG_H_INCLUDED
  #define FFNDOWN_DIALOG_H_INCLUDED
#endif // FFNDOWN_DIALOG_H_INCLUDED
//-------------------------------------
#ifndef WIDGETS3_H_INCLUDED
  #include "../../interface/controls/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef PROGRESS_PANEL2_H
  #include "../../fetching/panel/progress_panel2.h"
#endif // PROGRESS_PANEL2_H
#ifndef FICDOWN_SINGLE_H
  #include "../threads/ficdown_single.h"
#endif // FICDOWN_SINGLE_H
#ifndef FICURL_PARSING_H_INCLUDED
  #include "../data/ficurl_parsing.h"
#endif // FICURL_PARSING_H_INCLUDED
//----------------------------------------------
#include <QGroupBox>
#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QThread>
//***************************************************************
class jfFicDownDialog_Core : public QDialog {
    Q_OBJECT
  public:
    jfFicDownDialog_Core(QWidget* parent);
    bool MakeParams();
  public slots:
    void HandleNewExtract(jfFicExtract* new_extract);
    void HandlePathChanged();
    void ThreadDone();
    virtual void HandleStart() = 0;
    virtual void HandleDone(bool) = 0;
  protected:
    // HELPER METHODS
      void CreateLayout();
      void MakeConnects();
      QString MakeFullDirectory();
      void EnableWidgets(bool enable=true);
      void LoadSubDirs();
      bool MakeAndStartDownloader();
      void DisplayEmptyMsg();
    // GUI ELEMENTS
      QLabel* fic_display; // displays fic data
    // output file info
      jfDirPicker* poutput_dir; // displays output directory and allows changing it
      QLabel* sdir_label;
      QComboBox* pick_subdir;  // quick picking of destination sudirectory
      jfLabeledEdit* name_edit; // allows editing output filename base
      jfLabeledIntEdit* split_guide; // split the fic into sections size guide
    // layout
      QVBoxLayout* main_layout;
      QGroupBox* nonp_box;
      QVBoxLayout* gbox_layout;
      QHBoxLayout* directory_layout;
      QHBoxLayout* file_layout;
    // also...
      jfProgressPanelSingle* progress;
    // internal data
      jfBadCharsFilter* filter;
      jfFicExtract* xmain_data;
      jf_FICTYPE ftype;
      jfFicDownParameters* fparams;
    // internal thread data
      QThread* the_thread;
      jfSingleFicDownloader* downloader;

};
//=====================================================
/* This is the version of the download dialog intended for use from a right menu popup
  from search results, the info on the fic to download is passed as a constuctor parameter */
class jfFicDownDialog : public jfFicDownDialog_Core {
    Q_OBJECT
  public:
    jfFicDownDialog(QWidget* parent, jfFicExtract* infic);
  public slots:
    virtual void HandleStart();
    virtual void HandleDone(bool);
};
//====================================================
/* the version the the dialog that downloads fics from an URL */
class jfFicDownDialog2 : public jfFicDownDialog_Core {
    Q_OBJECT
  public:
    jfFicDownDialog2(QWidget* parent);
  public slots:
    void HandleC(bool checked);
    virtual void HandleStart();
    virtual void HandleDone(bool);
  protected:
    // helper methods
    bool ProcessUrl();
    // extra GUI elements
    jfLabeledEdit* urlentry;
    QPushButton* cbtn;
    QHBoxLayout* urlbar;
    // special data
    QString firsturl;
    jfFicURLParser urlparser;
};
/*****************************************************************************/
