/******************************************************************************
// Name:        ffndown_dialog.cpp
// Author :     John Q Metro
// Purpose :    Defines the dialog for downloading Fanfiction.Net fics
// Created:     July 17, 2011
// Started Qt conversion May 25, 2013
// Updated:     July 12, 2016
******************************************************************************/
#ifndef FFNDOWN_DIALOG_H_INCLUDED
  #include "ffndown_dialog.h"
#endif // FFNDOWN_DIALOG_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED
//---------------------------------------
#include <QDir>
#include <assert.h>
#include <QMessageBox>
#include <QSize>
/*****************************************************************************/
// the constructor
jfFicDownDialog_Core::jfFicDownDialog_Core(QWidget* parent):QDialog(parent) {
  // nulling data links
  xmain_data = NULL;
  fparams = NULL;
  xmain_data = NULL;
  the_thread =NULL;
  downloader = NULL;
  // creating the common widgets
  fic_display = new QLabel("",this);
  QString xoutdir= jglobal::settings.GetDirectory(jglobal::DownloadedFics);
  poutput_dir = new jfDirPicker(this,false,true,"Output Directory",xoutdir);
  sdir_label = new QLabel("Subfolder :",this);
  pick_subdir = new QComboBox(this);
  name_edit = new jfLabeledEdit(this,"Filename base",true);
  filter = new jfBadCharsFilter("\\/:*?\"<>|");
  name_edit->SetValidator(filter);
  split_guide = new jfLabeledIntEdit(this,"File Size Guide",true,100,9999);
  // setting some values
  split_guide->SetValue(jglobal::settings.ficsize_hint);
  LoadSubDirs();
}
//------------------------------------------
bool jfFicDownDialog_Core::MakeParams() {
  QString ficfname = (name_edit->GetText()).trimmed();
  if (ficfname.isEmpty()) return false;
  if(xmain_data != NULL) {
    xmain_data->fname_core = ficfname;
    fparams = new jfFicDownParameters(xmain_data);

  }
  else {
    fparams = new jfFicDownParameters();
    fparams->filename_base = ficfname;
  }
  fparams->sizeguide = split_guide->GetValue();
  fparams->write_to_directory = MakeFullDirectory();
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slots
//-------------------------
void jfFicDownDialog_Core::HandleNewExtract(jfFicExtract* new_extract) {
  assert(new_extract !=NULL);
  // setting the label display
  QString estring = new_extract->ToLabelString();
  fic_display->setText(estring);
  // clearing the old extract, if there...
  if (xmain_data != NULL) {
    jfFicExtract* bak = xmain_data;
    xmain_data = (fparams->extract) = new_extract;
    delete bak;
  }
  else{
    xmain_data = (fparams->extract) = new_extract;
  }
}
//-------------------------
void jfFicDownDialog_Core::HandlePathChanged() {
  LoadSubDirs();
}
//-------------------------
void jfFicDownDialog_Core::ThreadDone() {
  // disconnecting
  disconnect(downloader,SIGNAL(sendFicExtract(jfFicExtract*)),this,SLOT(HandleNewExtract(jfFicExtract*)));
  disconnect(the_thread, 0,downloader ,0);
  disconnect(downloader,0,the_thread,0);
  disconnect(the_thread,SIGNAL(finished()),this,SLOT(ThreadDone()));
  progress->DisConnectThread(downloader);
  // deleting
  downloader->deleteLater();
  the_thread->deleteLater();
  downloader = NULL;
  the_thread = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// HELPER METHODS
//-------------------------
void jfFicDownDialog_Core::CreateLayout() {
  const QString fname = "jfFicDownDialog_Core::CreateLayout";
  // when doing nested layouts, one has to go top down
  main_layout = new QVBoxLayout(this);
  setLayout(main_layout);
  // creating the group box and group box layout
  nonp_box = new QGroupBox("Fanfic Information");
  gbox_layout = new QVBoxLayout();
  // adding to the main layout
  main_layout->addWidget(nonp_box,0);
  main_layout->addWidget(progress,0);
  nonp_box->setLayout(gbox_layout);
  // creating and adding to the group box layouts
  directory_layout = new QHBoxLayout();
  file_layout = new QHBoxLayout();
  gbox_layout->addWidget(fic_display,0,Qt::AlignLeft);
  gbox_layout->addLayout(directory_layout,0);
  gbox_layout->addLayout(file_layout,0);
  // directory picking...
  directory_layout->addWidget(poutput_dir,3,Qt::AlignLeft);
  directory_layout->addSpacing(10);
  directory_layout->addWidget(sdir_label,0,Qt::AlignVCenter);
  directory_layout->addWidget(pick_subdir,1,Qt::AlignVCenter);
  // result file editing
  file_layout->addWidget(name_edit,1,Qt::AlignVCenter);
  file_layout->addSpacing(10);
  file_layout->addWidget(split_guide,0,Qt::AlignVCenter);

}
//-------------------------
void jfFicDownDialog_Core::MakeConnects() {
  connect(poutput_dir, SIGNAL(SendPathChanged()),this, SLOT(HandlePathChanged()));
}
//-------------------------
QString jfFicDownDialog_Core::MakeFullDirectory() {
  QString result = poutput_dir->GetPath();
  QString subdirt = pick_subdir->currentText();
  if (!subdirt.isEmpty()) {
    result += QDir::separator();
    result += subdirt;
  }
  return result;
}
//-------------------------
void jfFicDownDialog_Core::EnableWidgets(bool enable) {
  poutput_dir->setEnabled(enable);
  pick_subdir->setEnabled(enable);
  name_edit->setEnabled(enable);
  split_guide->setEnabled(enable);
}

//-------------------------
void jfFicDownDialog_Core::LoadSubDirs() {
  QDir xdir = QDir(poutput_dir->GetPath());
  QStringList sdirlist = xdir.entryList(QDir::Dirs|QDir::Writable,QDir::Name);
  pick_subdir->addItems(sdirlist);
}
//-------------------------
bool jfFicDownDialog_Core::MakeAndStartDownloader() {
  // setting up the thread and downloader
  downloader = new jfSingleFicDownloader(3);
  downloader->SetFicParameters(fparams);
  the_thread = new QThread();
  downloader->moveToThread(the_thread);
  // connecting signals and slots
  bool testoo = progress->ConnectAndSetPauser(downloader);
  /**/jfAssertLog(testoo,"jfFicDownDialog_Core::MakeAndStartDownloader","Connecting and Setting Pauser FAILED!!!!");
  connect(downloader,SIGNAL(sendFicExtract(jfFicExtract*)),this,SLOT(HandleNewExtract(jfFicExtract*)));

  // standard thread/downloader connections
  connect(the_thread, SIGNAL(started()),downloader ,SLOT(StartProcessing()));
  connect(downloader,SIGNAL(AllFinished()),the_thread,SLOT(quit()));
  connect(the_thread,SIGNAL(finished()),this,SLOT(ThreadDone()));

  // doing!
  the_thread->start();
  return true;
}
//----------------------------------------------------
void jfFicDownDialog_Core::DisplayEmptyMsg() {
  QMessageBox noEmpty;
  noEmpty.setText("The filename base cannot be empty!");
  noEmpty.setIcon(QMessageBox::Critical);
  noEmpty.exec();
}
//==============================================================================
// constrcutor
jfFicDownDialog::jfFicDownDialog(QWidget* parent, jfFicExtract* infic):jfFicDownDialog_Core(parent) {
  // assigning information
  assert(infic!=NULL);
  xmain_data = infic;
  ftype = xmain_data->GetFicType();
  // creating the progress panel
  progress = new jfProgressPanelSingle(false,this);
  // loading data
  fic_display->setText(xmain_data->ToLabelString());
  name_edit->SetFiletag(xmain_data->fic_title);
  // finishing off
  CreateLayout();
  MakeConnects();
  // finishing the connections
  connect(progress, SIGNAL(SendStart()),this, SLOT(HandleStart()));
  connect(progress, SIGNAL(SendPanelDone(bool)),this, SLOT(HandleDone(bool)));
  QSize thesize = size();
  thesize.setHeight(thesize.height()/2);
  resize(thesize);
}
//+++++++++++++++++++++++++++++++
// start button has been pressed
void jfFicDownDialog::HandleStart() {
  if (!MakeParams()) {
    progress->CancelStart();
    DisplayEmptyMsg();
    delete fparams;
    fparams = NULL;
  }
  else {
    EnableWidgets(false);
    MakeAndStartDownloader();
  }
}
//-------------------------------
void jfFicDownDialog::HandleDone(bool) {
  // no need to do anything?
  delete fparams;
  fparams = NULL;
}
//==================================================================
// constructor
jfFicDownDialog2::jfFicDownDialog2(QWidget* parent):jfFicDownDialog_Core(parent) {
  const QString fname = "jfFicDownDialog2::jfFicDownDialog2";
  xmain_data = NULL;
  // extra GUI elements
  progress = new jfProgressPanelSingle(true,this);
  urlentry = new jfLabeledEdit(this,"URL to Download",true);
  cbtn = new QPushButton("c",this);
  // setting the layout
  CreateLayout();
  urlbar = new QHBoxLayout(this);
  main_layout->insertLayout(0,urlbar);
  urlbar->addWidget(urlentry,1);
  urlbar->addWidget(cbtn,0,Qt::AlignVCenter);
  // connections
  MakeConnects();
  // finishing the connections
  connect(progress, SIGNAL(SendStart()),this, SLOT(HandleStart()));
  connect(progress, SIGNAL(SendPanelDone(bool)),this, SLOT(HandleDone(bool)));
  connect(cbtn,SIGNAL(clicked(bool)),this,SLOT(HandleC(bool)));
  QSize thesize = size();
  thesize.setWidth(thesize.width()+600);
  resize(thesize);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++
// copies the part of the url past the last slash to the filename base box
void jfFicDownDialog2::HandleC(bool checked) {
  QString gurl = (urlentry->GetText()).trimmed();
  urlparser.ParseURL(gurl);
  if (urlparser.isValid()) {
    QString tx = urlparser.FilenameBase();
    if (!tx.isEmpty()) name_edit->SetFiletag(tx);
  }
}
//-----------------------------
void jfFicDownDialog2::HandleStart() {
  // variables
  QMessageBox badurl;
  // handling the filename base
  QString ficfname = (name_edit->GetText()).trimmed();
  if (ficfname.isEmpty()) {
    progress->CancelStart();
    DisplayEmptyMsg();
    return;
  }
  // checking the url
  if (!ProcessUrl()) {
    progress->CancelStart();
    badurl.setText("Jaffis cannot fetch a fic from this URL!");
    badurl.setIcon(QMessageBox::Critical);
    badurl.exec();
    return;
  }
  // proceeding
  MakeParams();
  fparams->firsturl = urlparser.firstURL();
  fparams->fictype = ftype;
  EnableWidgets(false);
  MakeAndStartDownloader();
}
//----------------------------
void jfFicDownDialog2::HandleDone(bool) {
  delete fparams;
  fparams = NULL;
  delete xmain_data;
  xmain_data = NULL;
  EnableWidgets(true);
  progress->DoReset();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking and processing the url
bool jfFicDownDialog2::ProcessUrl() {
  QString gurl = (urlentry->GetText()).trimmed();
  if (!urlparser.ParseURL(gurl)) return false;
  ftype = urlparser.getType();
  urlentry->SetText(urlparser.firstURL());
  return true;
}

/*****************************************************************************/
