/******************************************************************************
Name    :   cat_misc.cpp
Author  :   John Q Metro
Purpose :   Misc category data stuff
Created :   August 19, 2012
Conversion to Qt Started September 28, 2013
Updated :   July 13, 2016 (added NameAtIndex)
******************************************************************************/
#ifndef CAT_MISC_H_INCLUDED
  #include "cat_misc.h"
#endif // CAT_MISC_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-----------------------------------------------------------------
#include <QMutexLocker>
#include <assert.h>
//***************************************************************************
jfGenSecData::jfGenSecData() {
  item_count = 0;
  secname = NULL;
  sectype = NULL;
  securl = NULL;
  item_index = -1;
}
//=======================================================================
// indexes
//-----------------------
void jfGenSecData::ResetIndex() {
  item_index = -1;
}
//-----------------------
bool jfGenSecData::NextIndex() {
  if (item_index==-2) return false;
  if (item_index==((int)(item_count)-1)) {
    item_index = -2;
    return false;
  }
  item_index++;
  return true;
}
//-----------------------
QString jfGenSecData::NextName() {
  if (NextIndex()) return secname->at(item_index);
  else return "";
}
//=======================================================================
// Getting Data
//-----------------------
QString jfGenSecData::CurrName() const {
  if (item_index>=0) return secname->at(item_index);
  else return "";
}
//-----------------------
QString jfGenSecData::CurrUrl() const {
  if (item_index>=0) return securl->at(item_index);
  else return "";
}
//-----------------------
QString jfGenSecData::CurrType() const {
  if (item_index>=0) return sectype->at(item_index);
  else return "";
}
//-----------------------
size_t jfGenSecData::ItemCount() const {
  return item_count;
}
//-----------------------
QString jfGenSecData::NameAtIndex(size_t index) const {
  QMutexLocker alock(&acc);
  if (index >= item_count) return "";
  if (secname == NULL) return "";
  return secname->at(index);
}
//=======================================================================
// Adding Data
bool jfGenSecData::AddStrings(size_t INBLOCK, const QString inname[], const QString inurls[], QString intype) {
  if (INBLOCK==0) return false;
  if (item_count==0) {
    secname = new QStringList();
    sectype = new QStringList();
    securl = new QStringList();
  }
  // the loop
  for (size_t iindex=0; iindex<INBLOCK; iindex++) {
    secname->append(inname[iindex]);
    securl->append(inurls[iindex]);
    sectype->append(intype);
  }
  if (item_index==-2) item_index = item_count;
  item_count += INBLOCK;
  return true;
}
//=======================================================================
// destuctor
jfGenSecData::~jfGenSecData() {
  if (secname!=NULL) delete secname;
  if (sectype!=NULL) delete sectype;
  if (securl!=NULL) delete securl;
}
//***************************************************************************
jfGenCatManager::jfGenCatManager(QString infilepath, QString inname) {
  filepath = infilepath;
  name = inname;
  current = NULL;
  newdata = NULL;
  for (int xval=0;xval<32;xval++) searchers[xval] = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// basic info
//------------------------------
QString jfGenCatManager::GetName() const {
  return name;
}
//------------------------------
QString jfGenCatManager::GetFilename() const {
  return filepath;
}
//------------------------------
bool jfGenCatManager::HasData() const {
  return (current!=NULL);
}
//------------------------------
jfGen_CatHolder* jfGenCatManager::GetData() {
  return current;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//------------------------------
void jfGenCatManager::ToFile() const {
  const QString fname = "jfGenCatManager::ToFile";
  QTextStream* ofile;
  if (HasData()) {
    ofile = GetOpenFile(filepath,true);
    current->AddToFile(ofile);
    CloseFileStream(ofile,true);
  }
}
//---------------------------------------------
bool jfGenCatManager::FromFile() {
  const QString fname = "jfGenCatManager::FromFile";
  QMutexLocker mlock(&locker);
  jfFileReader* infile;
  // starting with the file
  infile = new jfFileReader(filepath);
  if (!infile->OpenFile()) {
    err = infile->err;
    delete infile;
    return false;
  }
  // file gotten, we read...
  newdata = new jfGen_CatHolder();
  if (!newdata->GetFromFile(infile)) err = infile->err;
  delete infile;
  // checking the results
  if (newdata->IsValid()) {
    if (current!=NULL) delete current;
    current = newdata;
    newdata = NULL;
    return true;
  }
  else {
    delete newdata;
    newdata = NULL;
    return false;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// handling New
//------------------------------
void jfGenCatManager::NewDone() {
  QMutexLocker mlock(&locker);
  if (current!=NULL) delete current;
  current = newdata;
  newdata = NULL;
}
//------------------------------
void jfGenCatManager::DiscardNew() {
  QMutexLocker mlock(&locker);
  if (newdata!=NULL) delete newdata;
  newdata = NULL;
}
//------------------------------
jfGen_CatHolder* jfGenCatManager::StartNew() {
  QMutexLocker mlock(&locker);
  if (newdata!=NULL) delete newdata;
  newdata = new jfGen_CatHolder();
  newdata->SetName(name);
  return newdata;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// registering searchers
//-----------------------------------
int jfGenCatManager::RegisterSearch(const QWidget* in_search) {
  assert(in_search!=NULL);
  size_t rqval;
  for (rqval=0;rqval<32;rqval++) {
    if (searchers[rqval]==NULL) {
      searchers[rqval] = in_search;
      break;
    }
  }
  if (rqval==32) return -1;
  else return rqval;
}
//-------------------------------------
bool jfGenCatManager::RemoveSearch(size_t which) {
  if (which>=32) return false;
  if (searchers[which]!=NULL) {
    searchers[which] = NULL;
    return true;
  }
  else return false;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfGenCatManager::~jfGenCatManager() {
  if (newdata!=NULL) delete newdata;
  if (current!=NULL) delete current;
}
//***************************************************************************
