/***************************************************************
 * Name:      dirupdate_dialog.h
 * Purpose:   Declares dialog for updating directories
 * Author:    John Q Metro (johnqmetro@hotmail.com)
 * Created:   June 30, 2015
 * Updated:   August 2, 2016
 *
**************************************************************/
#ifndef DIRUPDATE_DIALOG_H_INCLUDED
  #define DIRUPDATE_DIALOG_H_INCLUDED
#endif // DIRUPDATE_DIALOG_H_INCLUDED
//---------------------------------------------------
#ifndef DIRUPDATE_THREAD_H_INCLUDED
  #include "../../ficdown/threads/dirupdate_thread.h"
#endif // DIRUPDATE_THREAD_H_INCLUDED

#ifndef PROGRESS_PANEL2_H
  #include "../../fetching/panel/progress_panel2.h"
#endif // PROGRESS_PANEL2_H

#ifndef WIDGETS3_H_INCLUDED
  #include "../../interface/misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
//-------------------------------------------------------
#include <QLabel>
#include <QBoxLayout>
#include <QDialog>


//**********************************************************************
// display panel with directory editors
class jfUpdateDirectoryPickPanel : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfUpdateDirectoryPickPanel(QWidget* parent = NULL);
    // load and save
    QString GetValue(bool source) const;
    void SetValue(bool source, QString inval);
    int TestDirs() const;
    void SetLEnabled(bool enabled);
  protected:
    // gui builder methods
    void MakeWidgets();
    // GUI ELEMENTS
    jfDirPicker* sourcedir;
    jfDirPicker* destdir;
    QVBoxLayout* layout;
};
//====================================================================
// progress panel for a Search that has no categories, just pages
class jfProgressPanelSyncDirectory : public jfProgressPanelOne {
    Q_OBJECT
  public:
    jfProgressPanelSyncDirectory(QWidget* parent = NULL);
    // specials
    bool ConnectAndSetPauser(jfDirUpdate_Thread* the_downloader);
  public slots:
    void HandleItemFailed(bool halt);
  protected:
};

//=====================================================================
class jfUpdateDirectory_Dialog : public QDialog {
    Q_OBJECT
  public:
    // constructor
    jfUpdateDirectory_Dialog(QWidget* parent = NULL);
  public slots:
    void ThreadDone();
    void HandleStart();
    void HandleDone(bool);
  protected:
    //private helpher methods
    bool SetupThread();
    void FinishSizers();
    bool GetDirectories(QString& out_err);
    // GUI ELEMENTS
    jfUpdateDirectoryPickPanel* dpanel;
    jfProgressPanelSyncDirectory* mpanel;
    // sizer
    QVBoxLayout* mainsizer;
    // internal elements
    QString sdir, tdir;
    QThread* thread_object;
    jfDirUpdate_Thread* sync_runner;
  // event handler table
};
//**********************************************************************
