/******************************************************************************
Name    :   ao3_catdialog.cpp
Author  :   John Q Metro
Purpose :   Dialog for AO3 Catalog Downloading
Created :   August 25, 2012
Conversion to Qt Started March 15, 2014
Updated :   August 25, 2012
******************************************************************************/
#ifndef AO3_CATDIALOG_H_INCLUDED
  #include "ao3_catdialog.h"
#endif // AO3_CATDIALOG_H_INCLUDED
//---------------------------------------
#ifndef AO3CAT_THREAD_H
  #include "../../threads/ao3/ao3cat_thread.h"
#endif // AO3CAT_THREAD_H
#ifndef AO3_CATS_H_INCLUDED
  #include "../../data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED

//---------------------------------------
#include <assert.h>
/******************************************************************************/
// the constructor
jfAO3Cat_DownDialog::jfAO3Cat_DownDialog(QWidget* parent):jfGenCat_DownDialog(parent) {
  setWindowTitle("Archive Of Our Own fandom downloader");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods
//---------------------------------
jfGeneralCategoryDownloaderBase* jfAO3Cat_DownDialog::MakeTypedThread() const {
  jfAO3CategoryDownloader* res = new jfAO3CategoryDownloader();
  res->SetInitialData();
  return res;
}
/******************************************************************************/
