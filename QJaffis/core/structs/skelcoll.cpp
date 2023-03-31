/*****************************************************************************
Name:        skelcoll.cpp
Author :     John Q Metro
Purpose :    Grouping multiple HTML skeltons together
Created:     November 21, 2009
Conversion to QT Started April 14, 2013
Updated:     September 4, 2014
******************************************************************************/
#ifndef SKELCOLL_H_INCLUDED
  #include "skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
#ifndef SKELSTORE_H_INCLUDED
  #include "../objs/skelstore.h"
#endif // SKELSTORE_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
#include <QMessageBox>
/*****************************************************************************/
// we initialize an empty group
jfSkelCollection::jfSkelCollection():jfBaseCollection() {
  datalist = new jfSkelVector();
  item_count = 6;
  datalist->push_back(new jfSkeletonSingle());
  (*datalist)[0]->SetName("Default Google Results Skeleton");
  name = "Global Skeleton store";
  datalist->push_back(new jfSkeletonMultiple());
  (*datalist)[1]->SetName("Default Fanfiction.Net Results Skeleton");
  datalist->push_back(new jfSkeletonFicPart());
  (*datalist)[2]->SetName("Fanfic Downloader Skeleton");
  datalist->push_back(new jfSkeletonSingle());
  (*datalist)[3]->SetName("Fimfiction.Net Fic Results Skeleton");
  datalist->push_back(new jfSkeletonMultiple());
  (*datalist)[4]->SetName("Default Archiveofourown.org Results Skeleton");
  datalist->push_back(new jfSkeletonSingle());
  (*datalist)[5]->SetName("Default Fimfiction.net Groups Skeleton");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented index methods
//-------------------------------------------------------------------
bool jfSkelCollection::NextIndex() {
  if ((item_count==0) || (item_index==-2)) return false;
  if (item_index==(((int)item_count)-1)) {
    curr_item = NULL;
    item_index = -2;
    return false;
  }
  else {
    item_index++;
    curr_item = (*datalist)[item_index];
    return true;
  }
}
//-------------------------------------------------------------------
bool jfSkelCollection::BackIndex() {
  if ((item_count==0) || (item_index==-1)) return false;
  if (item_index==0) {
    curr_item = NULL;
    item_index = -1;
    return false;
  }
  else if (item_index==-2) item_index = item_count-1;
  else item_index--;
  curr_item = (*datalist)[item_index];
  return true;
}
//-------------------------------------------------------------------
void jfSkelCollection::ResetIndex() {
  curr_item = NULL;
  item_index = -1;
}
//-------------------------------------------------------------------
bool jfSkelCollection::ToIndex(const int& newindex) {
  if (newindex<-2) return false;
  if (newindex >= ((int)item_count)) return false;
  if (newindex<0) curr_item = NULL;
  else curr_item = (*datalist)[newindex];
  item_index = newindex;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more former abstract methods
//-------------------------------------------------------------------
QStringList* jfSkelCollection::GetNameList() const {
  // variables
  QStringList* result;
  size_t loopc;
  // starting
  result = new QStringList();
  // the loop
  for (loopc = 0; loopc<item_count; loopc++) {
    result->append((*datalist)[loopc]->GetName());
  }
  // done
  return result;
}
//-------------------------------------------------------------------
QStringList* jfSkelCollection::GetDescriptionList() const {
  // variables
  QStringList* result;
  size_t loopc;
  // starting
  result = new QStringList();
  // the loop
  for (loopc = 0; loopc<item_count; loopc++) {
    result->append((*datalist)[loopc]->GetDescription());
  }
  // done
  return result;
}
//-------------------------------------------------------------------
QStringList* jfSkelCollection::GetTypeList() const {
  // variables
  QStringList* result;
  size_t loopc;
  // starting
  result = new QStringList();
  // the loop
  for (loopc = 0; loopc<item_count; loopc++) {
    result->append((*datalist)[loopc]->GetTypeID());
  }
  // done
  return result;
}
//-------------------------------------------------------------------
// note that really this method should never need to be used
bool jfSkelCollection::DeleteAtIndex(const size_t& dindex) {
  // local varibales
  size_t nmindex, nbindex;
  // special case
  if (dindex>=item_count) return false;
  // index manipulation
  nmindex = CalcNewIndex(item_index);
  nbindex = CalcNewIndex(backup_index);
  // deleting the item
  delete (*datalist)[dindex];
  (*datalist)[dindex];
  datalist->erase((datalist->begin())+dindex);
  // resetting the index
  ToIndex(nmindex);
  if ((nbindex)>=0) backup_item = (*datalist)[nbindex];
  backup_index = nbindex;
  // done
  return true;
}
//-------------------------------------------------------------------
QString jfSkelCollection::GetTypeID() const {
  return "SkeletonCollection";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// additional methods
//-------------------------------------------------
jfSkeletonSingle*   jfSkelCollection::AtIndexS(size_t index) {
  // checking for problems
  if (index>=item_count) return NULL;
  if (GetSkelType(index)!=jfs_SINGLE) return NULL;
  // returning
  return dynamic_cast<jfSkeletonSingle*>((*datalist)[index]);
}
//-------------------------------------------------
jfSkeletonMultiple* jfSkelCollection::AtIndexM(size_t index) {
  // checking for problems
  if (index>=item_count) return NULL;
  if (!IsMultiple(index)) return NULL;
  // returning
  return dynamic_cast<jfSkeletonMultiple*>((*datalist)[index]);
}
//---------------------------------------------------------
jfSkeletonFicPart* jfSkelCollection::AtIndexF(size_t index) {
    // checking for problems
  if (index>=item_count) return NULL;
  if (GetSkelType(index)!=jfs_FICPART) return NULL;
  // returning
  return dynamic_cast<jfSkeletonFicPart*>((*datalist)[index]);
}
//---------------------------------------------------------
bool jfSkelCollection::IsMultiple(size_t index) const {
  assert(index<item_count);
  return (GetSkelType(index)==jfs_MULTIPLE);
}
//------------------------------------------------------------
jfs_Skeltype jfSkelCollection::GetSkelType(size_t index) const {
  assert(index<item_count);
  return (*datalist)[index]->GetSkelType();
}
//----------------------------------------------------------------------------
void jfSkelCollection::AppendBlankSkeleton(jfs_Skeltype intype,QString name) {
  // variables
  jfSkeletonCore* s1;
  // we create
  if (intype==jfs_SINGLE) s1 = new jfSkeletonSingle();
  else if (intype==jfs_MULTIPLE) s1 = new jfSkeletonMultiple();
  else if (intype==jfs_FICPART) s1 = new jfSkeletonFicPart();
  else assert(false);
  // finishing
  s1->SetName(name);
  datalist->push_back(s1);
  item_count++;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSkelCollection::~jfSkelCollection() {
  ClearDatalist();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfSkelCollection::ClearDatalist() {
  for (size_t loopc=0; loopc< item_count; loopc++) {
    delete (*datalist)[loopc];
    (*datalist)[loopc] = NULL;
  }
  delete datalist;
  datalist = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file I/O
//-------------------------------------------------------------------
// this does nothing
bool jfSkelCollection::AddMid2ToFile(QTextStream* outfile) const {
  return true;
}
//-------------------------------------------------------------------
// neither does this
bool jfSkelCollection::ReadMid2FromFile(jfFileReader* infile) {
  return true;
}
//-------------------------------------------------------------------
// writes the contents to file
bool jfSkelCollection::AddRestToFile(QTextStream* outfile) const {
  size_t loopc;
  bool wres;
  // starting
  for (loopc=0;loopc<item_count;loopc++) {
    wres = (*datalist)[loopc]->AddToFile(outfile);
    if (!wres) return false;
  }
  return true;
}
//-------------------------------------------------------------------
// reads the contents from file
bool jfSkelCollection::ReadRestFromFile(jfFileReader* infile) {
    // constants
  const QString fname = "jfFilterMap::ReadRestFromFile";
  // local variables
  size_t loopc;
  QString typid;
  jfSkeletonCore* newitem;
  // the usual initial checks
  assert(infile!=NULL);
  // preparing the datalist
  ClearDatalist();
  if (item_count>0) datalist = new jfSkelVector();
  // the loop
  for (loopc=0;loopc<item_count;loopc++) {
    // gets the filter type string (and also the line index)
    if (!infile->GetType(typid,fname)) return false;
    // we make an empty default filter
    if (typid=="SkeletonSingle") newitem = new jfSkeletonSingle();
    else if (typid=="SkeletonMultiple") {
      newitem = new jfSkeletonMultiple();
    }
    else if (typid=="SkeletonFicPart") {
      newitem = new jfSkeletonFicPart();
    }
    else return infile->BuildError("Unrecognized Type ID");
    assert(newitem!=NULL);
    // and then load
    if (!newitem->GetFromFile(infile)) return false;
    // we add the newly loaded item to the list
    datalist->push_back(newitem);
  }
  if (item_count==1) AppendBlankSkeleton(jfs_MULTIPLE,"Default Fanfiction.Net Results Skeleton");
  if (item_count==2) AppendBlankSkeleton(jfs_FICPART,"Fanfic Downloader Skeleton");
  if (item_count==3) AppendBlankSkeleton(jfs_SINGLE,"Default Fimfiction.net Results Skeleton");
  if (item_count==4) AppendBlankSkeleton(jfs_MULTIPLE,"Default Archiveofourown.org Results Skeleton");
  if (item_count==5) AppendBlankSkeleton(jfs_SINGLE,"Default Fimfiction.net Groups Skeleton");
  // done
  return true;
}
/****************************************************************************/
jfSkelCollection* mainskelc=NULL;
//===========================================================================
bool LoadSkeletons() {
  // local variables
  const QString fname = "LoadSkeletons()";
  jfFileReader* infile;
  bool rres;
  QMessageBox msgBox;
  // initial cleaning
  /**/JDEBUGLOG(fname,1);
  if (mainskelc != NULL) {
    delete mainskelc;
  }
  // creating the list of filterable object type names
  mainskelc = new jfSkelCollection();
  // opening the text file
  infile = new jfFileReader(skelg_fname);
  if (!infile->OpenFile()) {
    msgBox.setText("Skeletons File could not be opened!");
    msgBox.setDetailedText(infile->err);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    rres = false;
  }
  else {
    rres = mainskelc->GetFromFile(infile);
    if (!rres) {
      msgBox.setText("Skeletons File had errors!");
      msgBox.setDetailedText(infile->err);
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
    }
  }
  delete infile;
  return rres;
}
//---------------------------------------------------------------------------
void WriteSkeletons() {
  // variables
  QTextStream* outfile;
  // we open and clear
  outfile = GetOpenFile(skelg_fname,true);
  // writing results
  mainskelc->AddToFile(outfile);
  CloseFileStream(outfile,true);
}
/*****************************************************************************/
