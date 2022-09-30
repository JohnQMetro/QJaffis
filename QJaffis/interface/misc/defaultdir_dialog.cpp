/***************************************************************
 * Name:      defaultdir_dilaog.cpp
 * Purpose:   Declares dialog for default directory editing
 * Author:    John Q Metro (davidj_faulks@yahoo.ca)
 * Created:   December 18, 2011
 * Conversion to Qt : Started September 7, 2013
 * Updated:   July 1, 2022
 *
**************************************************************/
#ifndef DEFAULTDIR_DIALOG_H_INCLUDED
  #include "defaultdir_dialog.h"
#endif // DEFAULTDIR_DIALOG_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------
#include <QDir>
#include <QFileDialog>
#include <assert.h>
#include <QSize>
#include <QApplication>
#include <QMessageBox>
//****************************************************************************
// constructor
jfDefaultDirDisplay::jfDefaultDirDisplay(QWidget* parent):QWidget(parent) {
  // starting...
  MakeLabels();
  MakeRest();
  // creating the sizer
  main_sizer = new QGridLayout();
  for(size_t loopv = 0; loopv < jglobal::path_type_count ; loopv++) {
    InsertRow(loopv);
  }
  main_sizer->setColumnStretch(2,1);
  // finishing
  setLayout(main_sizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// load and save
//---------------------------------------------
void jfDefaultDirDisplay::LoadValues() {
  size_t lloop;
  QString currdir;
  jglobal::DEFAULT_PATH_TYPE  xdirtype;
  for (lloop=0; lloop < jglobal::path_type_count ;lloop++) {
    xdirtype = static_cast<jglobal::DEFAULT_PATH_TYPE>(lloop);
    currdir = jglobal::settings.paths.GetPathFor(xdirtype);
    displays[lloop]->setText(currdir);
  }
}
//---------------------------------------------
void jfDefaultDirDisplay::SaveValues() const {
  size_t lloop;
  jglobal::DEFAULT_PATH_TYPE  xdirtype;
  QString ctext;
  // the saving loop
  for (lloop=0 ; lloop < jglobal::path_type_count ; lloop++) {
    xdirtype = static_cast<jglobal::DEFAULT_PATH_TYPE>(lloop);
    ctext = displays[lloop]->text();
    jglobal::settings.paths.SetPathFor(xdirtype,ctext);
  }
}
//---------------------------------------------
QString jfDefaultDirDisplay::GetValue(size_t tindex) const {
  if (tindex > jglobal::path_type_count) return "";
  else return displays[tindex]->text();
}
//---------------------------------------------
bool jfDefaultDirDisplay::SetValue(size_t tindex, QString inval) {
  if (tindex > jglobal::path_type_count) return false;
  displays[tindex]->setText(inval);
  return true;
}
//---------------------------------------------
int jfDefaultDirDisplay::TestDirs() const {
  // variables
  size_t lloop;
  QString tval;
  QDir zval;
  // the test loop
  for (lloop=0 ; lloop < jglobal::path_type_count ; lloop++) {
    tval = displays[lloop]->text();
    zval.setPath(tval);
    if (!zval.exists()) break;
  }
  if (lloop == jglobal::path_type_count) return -1;
  else return lloop;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// events
void jfDefaultDirDisplay::HandleDisplayDirpicker(bool ischecked) {
  // variables
  const QString fname = "jfDefaultDirDisplay::HandleDisplayDirpicker";
  int event_zindex;
  size_t btn_idx;
  QPushButton* source_button;
  QString result;
  // getting the specified control
  source_button = dynamic_cast<QPushButton*>(sender());
  event_zindex = -1;
  for ( btn_idx = 0 ; btn_idx < jglobal::path_type_count ; btn_idx++) {
    if (source_button==dirpickers[btn_idx]) {
      event_zindex = btn_idx;
      break;
    }
  }
  assert(event_zindex>=0);
  // showing the dialog
  result = QFileDialog::getExistingDirectory(this,"Choose a directory",displays[event_zindex]->text());
  if (!result.isEmpty()) displays[event_zindex]->setText(result);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gui builder methods
//---------------------------------------------
void jfDefaultDirDisplay::MakeLabels() {
  // variables
  size_t lloop;
  QString label;
  jglobal::DEFAULT_PATH_TYPE path_type;
  // the label making loop
  for (lloop = 0; lloop < jglobal::path_type_count ; lloop++) {
      path_type = static_cast<jglobal::DEFAULT_PATH_TYPE>(lloop);
      label = jglobal::settings.paths.LabelFor(path_type);
      labels[lloop] = new QLabel(label);
      colons[lloop] = new QLabel(" : ");
  }
}
//---------------------------------------------
void jfDefaultDirDisplay::MakeRest() {
  // variables
  size_t lloop;
  jglobal::DEFAULT_PATH_TYPE  xdirtype;
  // the label making loop
  for (lloop = 0 ; lloop < jglobal::path_type_count ; lloop++) {
    xdirtype = static_cast<jglobal::DEFAULT_PATH_TYPE>(lloop);
    displays[lloop]   = new QLineEdit(jglobal::settings.paths.GetPathFor(xdirtype));
    dirpickers[lloop] = new QPushButton("Pick...");
    connect(dirpickers[lloop],SIGNAL(clicked(bool)),this,SLOT(HandleDisplayDirpicker(bool)));
  }
}
//---------------------------------------------
bool jfDefaultDirDisplay::InsertRow(size_t rowval) {
  // the usual asserts
  assert(rowval < jglobal::path_type_count);
  assert(main_sizer!=NULL);
  // after that, we assume things are okay
  main_sizer->addWidget(labels[rowval],rowval,0,Qt::AlignVCenter);
  main_sizer->addWidget(colons[rowval],rowval,1,Qt::AlignVCenter);
  main_sizer->addWidget(displays[rowval],rowval,2,Qt::AlignVCenter);
  main_sizer->addWidget(dirpickers[rowval],rowval,3);
  // done
  return true;
}

//============================================================================
// constructor
jfDefaultDirsDialog::jfDefaultDirsDialog(QWidget* parent):QDialog(parent) {
  // inititalize sizer
  main_sizer = new QVBoxLayout();
  // the topbar values
  loggingon_cb = new QCheckBox("Do Logging (changes take effect after restart)");
  loggingon_cb->setChecked(jglobal::settings.DoLogging());
  shint = new jfLabeledIntEdit(this,"Default filesize hint",true,30,30000);
  shint->SetValue(jglobal::settings.ficsize_hint);
  // adding to the topbar
  topbar = new QHBoxLayout();
  topbar->addWidget(shint);
  topbar->addWidget(loggingon_cb,0,Qt::AlignVCenter);
  // visual elements
  contents = new jfDefaultDirDisplay(this);
  saveclose_btn = new QPushButton("Save and Close");
  // arranging
  main_sizer->addLayout(topbar);
  main_sizer->addWidget(contents);
  main_sizer->addWidget(saveclose_btn,0,Qt::AlignRight);
  // finsihing
  setLayout(main_sizer);
  connect(saveclose_btn,SIGNAL(clicked(bool)),this,SLOT(HandleSaveButton(bool)));
  QSize thesize = size();
  thesize.setWidth(thesize.width()*3);
  contents->LoadValues();
  resize(thesize);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// events
void jfDefaultDirsDialog::HandleSaveButton(bool ischecked) {
  // variables
  int indexval = contents->TestDirs();
  long xres;
  // starting
  if (indexval<0) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    jglobal::settings.SetDoLogging(loggingon_cb->isChecked());
    xres = shint->GetValue();
    jglobal::settings.ficsize_hint = xres;
    contents->SaveValues();
    QApplication::restoreOverrideCursor();
    accept();
  }
  else ShowErrorMessage(indexval);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfDefaultDirsDialog::ShowErrorMessage(int dindex) {
  // asserts
  assert(dindex>=0);
  assert(dindex < jglobal::path_type_count);
  // constants
  const QString main_title = "Save Aborted";
  const QString errpart1 = "Error: The directory '";
  const QString errpart2 = "' does not exist. Please correct the Error.";
  // variables
  QMessageBox msgBox;
  QString thedir, themsg;
  // building the output info
  thedir = contents->GetValue(dindex);
  themsg = errpart1 + thedir + errpart2;
  msgBox.setText(main_title);
  msgBox.setInformativeText(themsg);
  msgBox.setStandardButtons(QMessageBox::Abort);
  // done
  int ret = msgBox.exec();
  reject();
}
//****************************************************************************
