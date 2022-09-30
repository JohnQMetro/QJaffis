/***************************************************************
 * Name:      defaultdir_dilaog.h
 * Purpose:   Declares dialog for default directory editing
 * Author:    John Q Metro
 * Created:   December 18, 2011
 * Conversion to Qt : Started September 7, 2013
 * Updated:   March 27, 2022
 *
**************************************************************/
#ifndef DEFAULTDIR_DIALOG_H_INCLUDED
#define DEFAULTDIR_DIALOG_H_INCLUDED
#endif // DEFAULTDIR_DIALOG_H_INCLUDED
//---------------------------------------------
#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../controls/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
//-------------------------------------
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QCheckBox>
#include <QDialog>
//****************************************************************************
// display panel with directory editors
class jfDefaultDirDisplay : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfDefaultDirDisplay(QWidget* parent = 0);
    // load and save
    void LoadValues();
    void SaveValues() const;
    QString GetValue(size_t tindex) const;
    bool SetValue(size_t tindex, QString inval);
    int TestDirs() const;
  public slots:
    void HandleDisplayDirpicker(bool ischecked);
  protected:
    // gui builder methods
    void MakeLabels();
    void MakeRest();
    bool InsertRow(size_t rowval);
    // GUI ELEMENTS (the + 1 is the python transfer/script folder)

    QLabel* labels[jglobal::path_type_count];
    QLabel* colons[jglobal::path_type_count];
    QLineEdit*   displays[jglobal::path_type_count];
    QPushButton*     dirpickers[jglobal::path_type_count];
    // other things
    QGridLayout* main_sizer;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// dialog for the directory editor
class jfDefaultDirsDialog : public QDialog {
    Q_OBJECT
  public:
    // constructor
    jfDefaultDirsDialog( QWidget* parent = 0);
  public slots:
    void HandleSaveButton(bool ischecked);
  protected:
    void ShowErrorMessage(int dindex);
    // GUI elements
    QCheckBox* loggingon_cb;
    jfLabeledIntEdit* shint;
    QHBoxLayout* topbar;
    jfDefaultDirDisplay* contents;
    QPushButton* saveclose_btn;
    // top level sizer
    QVBoxLayout* main_sizer;

};
//****************************************************************************
