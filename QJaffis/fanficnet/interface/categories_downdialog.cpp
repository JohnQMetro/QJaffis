/*****************************************************************************
Name    : categories_downdialog.cpp
Basic   : Fanfiction.Net category downloader dialog
Author  : John Q Metro
Started : July 22, 2016
Updated : July 22, 2016

******************************************************************************/
#ifndef CATEGORIES_DOWNDIALOG_H
  #include "categories_downdialog.h"
#endif // CATEGORIES_DOWNDIALOG_H
//-------------------------------------
#ifndef CATGROUP_H
  #include "../categories/catgroup.h"
#endif // CATGROUP_H
//-------------------------------------

/*****************************************************************************/
// --- [ METHODS for jfProgPanelFFNCategories ] -------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setup stuff
//-----------------------------
jfProgPanelFFNCategories::jfProgPanelFFNCategories(QWidget* parent):jfProgPanelMultiBase(1, parent) {

}
//-----------------------------
bool jfProgPanelFFNCategories::ConnectAndSetPauser(jfFFN_CategoryDownloader* the_downloader) {
  if (the_downloader == NULL)  return false;
  bool pstatus = MakePauser();
  if (!pstatus) return false;
  // setting up and connecting the downloader
  the_downloader->setPauser(pauser);
  ConnectThread(the_downloader);
  connect(the_downloader,SIGNAL(SendSectionCount(size_t)),this,SLOT(HandleCatCount(size_t)));
  connect(the_downloader,SIGNAL(SendNewSection(struct jfItemInfoMsg)),this,SLOT(HandleNewCategory(struct jfItemInfoMsg)));
  connect(the_downloader,SIGNAL(SendSectionDone(bool)),this,SLOT(HandleCategoryDone(bool)));
  connect(the_downloader,SIGNAL(SendSectionFail()),this,SLOT(HandleCatFail()));

  connect(the_downloader,SIGNAL(SendSkippedPages(size_t)),this,SLOT(HandleSkippedPages(size_t)));
  connect(the_downloader,SIGNAL(SendOneThingSection(QString,QString)),this,SLOT(HandleOneThingSection(QString,QString)));

  return true;
}
//-----------------------------
void jfProgPanelFFNCategories::DisconnectDownloader(jfFFN_CategoryDownloader* the_downloader) {
  if (the_downloader != NULL) {
    disconnect(the_downloader, 0,this, 0);
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slot handling
//-----------------------------
void jfProgPanelFFNCategories::HandleSkippedPages(size_t skipcount){
  itemindex+=skipcount;
  item_bar->setValue(itemindex);
  QString msg = "Skipped " + QString::number(skipcount) + " ";
  msg += lastinfo.item_label;
  if (skipcount > 1) msg += "s";
  item_status->setText(msg);
  SetWorking();
}
//-----------------------------
void jfProgPanelFFNCategories::HandleOneThingSection(QString action, QString what) {
  catinfo.startaction = lastinfo.startaction = action;
  catinfo.item_label = lastinfo.item_label = what;
  catinfo.item_name = lastinfo.item_name= "";
  catinfo.item_index = lastinfo.item_index = -1;
  SetCatLabel("");
  item_bar->setValue(0);
  item_bar->setMaximum(1);
  itemindex = 0;
  SetLabel("");
}
//=================================================================
// --- [ METHODS for jfFFN_CatDownDialog ] ------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFN_CatDownDialog::jfFFN_CatDownDialog(QWidget* parent):QDialog(parent) {
  // creating and setting the panel
  update_cbox = new QCheckBox("Update the FFN Categories if possible");
  mpanel = new jfProgPanelFFNCategories();
  //layout
  mlayout = new QVBoxLayout();
  mlayout->addWidget(update_cbox);
  mlayout->addWidget(mpanel);
  setLayout(mlayout);
  // connecting
  connect(mpanel,SIGNAL(SendStart()),this,SLOT(HandleStart()));
  connect(mpanel,SIGNAL(SendPanelDone(bool)),this,SLOT(HandleDone(bool)));
  // resizing
  QSize thesize = sizeHint();
  thesize.setWidth(750);
  resize(thesize);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFN_CatDownDialog::HandleStart() {
  // constants
  const QString fname = "jfGenCat_DownDialog::StartButton";
  // starting
  // basically, we create the thread...
  bool update = update_cbox->isChecked();
  downloader = new jfFFN_CategoryDownloader();
  downloader->SetCatManagerPtr(ffn_catorganizer::main_manager,update);
  SetupDownloaderAndThread();
  thread_object->start();
}
//--------------------------------------
void jfFFN_CatDownDialog::HandleDone(bool isokay) {
  emit SendCatDone(isokay);
}
//---------------------------------------------------
void jfFFN_CatDownDialog::ThreadDone() {
  // disconnecting
  disconnect(thread_object, 0,downloader ,0);
  disconnect(downloader,0,thread_object,0);
  disconnect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));
  mpanel->DisconnectDownloader(downloader);
  // deleting
  downloader->deleteLater();
  thread_object->deleteLater();
  downloader = NULL;
  thread_object = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods
bool jfFFN_CatDownDialog::SetupDownloaderAndThread() {
  const QString fname = "jfFFN_CatDownDialog::SetupDownloaderAndThread";
  if (downloader == NULL) return false;
  // setting up the thread and downloader
  thread_object = new QThread();
  downloader->moveToThread(thread_object);
  // connecting signals and slots
  bool testoo = mpanel->ConnectAndSetPauser(downloader);
  /**/jerror::AssertLog(testoo,fname,"Connecting and Setting Pauser FAILED!!!!");

  // standard thread/downloader connections
  connect(thread_object, SIGNAL(started()),downloader ,SLOT(StartProcessing()));
  connect(downloader,SIGNAL(AllFinished()),thread_object,SLOT(quit()));
  connect(thread_object,SIGNAL(finished()),this,SLOT(ThreadDone()));

  return true;
}
/*****************************************************************************/

