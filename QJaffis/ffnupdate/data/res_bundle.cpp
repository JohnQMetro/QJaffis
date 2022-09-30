/******************************************************************************
* Name    :  res_bundle.cpp
* Author  :  John Q Metro
* Purpose :  Declares a class that holds all the data related to the update
* Created :  January 20, 2011
* Updated :  August 15, 2016
******************************************************************************/
#ifndef RES_BUNDLE_H_INCLUDED
  #include "res_bundle.h"
#endif // RES_BUNDLE_H_INCLUDED
//------------------------------------------
#include <QMutexLocker>
/****************************************************************************/
// constructors
jfFFN_UpdateBundle::jfFFN_UpdateBundle():jfBaseObj() {
  search_data = NULL;
  result_data = NULL;
  new_data = NULL;
  auth_data = NULL;
  chlink = NULL;
  auth_check = false;
  new_fics = false;
  sindex = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfFFN_UpdateBundle::GetTypeID() const {
  return "jfFFN_UpdateBundle";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// information about the result
//----------------------------------------------------------
size_t jfFFN_UpdateBundle::ResultCatCount() const {
  assert(result_data!=NULL);
  return result_data->CollectionCount();
}
//----------------------------------------------------------
size_t jfFFN_UpdateBundle::GetIDLimit(size_t limindex) const {
  assert(result_data!=NULL);
  assert(limindex<(result_data->CollectionCount()));
  return result_data->FirstIDAtIndex(limindex);
}
//------------------------------------------------------------------
bool jfFFN_UpdateBundle::SameCategory(size_t index, const jfFFN_CategoryCore* testee) {
  // constants
  const QString fname = "jfFFN_UpdateBundle::SameCategory";
  // variables
  const jfFFN_CategoryCore* test1;
  // checks
  /**/JDEBUGLOGST(fname,1,index);
  jerror::AssertLog(result_data!=NULL,fname,"Result Data is NULL!");
  jerror::AssertLog(testee!=NULL,fname,"CategoryCore parameter is null");
  /**/JDEBUGLOGST(fname,2,(result_data->CollectionCount()));
  /**/JDEBUGLOGS(fname,3,testee->GetName());
  if (index==(result_data->CollectionCount())) return false;
  // we produce the result
  test1 = result_data->GetCat(index);
  /**/JDEBUGLOGS(fname,4,test1->GetName());
  return test1==testee;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special information
const jfFFN_CategoryCore* jfFFN_UpdateBundle::GetOldCategory() const {
  size_t oindex = result_data->CollectionIndex();
  if (oindex>=(result_data->CollectionCount())) return NULL;
  else return result_data->GetCat(oindex);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// update status flags
//-----------------------------------
bool jfFFN_UpdateBundle::AuthDone() {
  QMutexLocker mlock(&upd_task_lock);
  auth_check = true;
  return auth_check && new_fics;
}
//-----------------------------------
bool jfFFN_UpdateBundle::NewDone() {
  QMutexLocker mlock(&upd_task_lock);
  new_fics = true;
  return auth_check && new_fics;
}
//--------------------------------------
bool jfFFN_UpdateBundle::CheckSourceNNull() const {
  if (search_data==NULL) return false;
  return (result_data!=NULL);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructors
jfFFN_UpdateBundle::~jfFFN_UpdateBundle() {
  if (search_data!=NULL) delete search_data;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//-----------------------------------
bool jfFFN_UpdateBundle::AddMiddleToFile(QTextStream* outfile) const {
  // constants and variables
  const QString fname = "jfFFN_UpdateBundle::AddMiddleToFile";
  QString buffer;
  bool rval;
  // checking and special conditions
  /**/JDEBUGLOG(fname,1);
  if (outfile==NULL) return false;
  /**/JDEBUGLOG(fname,2);
  // we write the date first
  buffer = lastdate.toString("MM-dd-yyyy");
  (*outfile) << buffer << "\n";
  /**/JDEBUGLOG(fname,3);
  // next, we write the search file
  rval = search_data->AddToFile(outfile);
  /**/JDEBUGLOG(fname,4);
  return rval;
}
//-----------------------------------
bool jfFFN_UpdateBundle::AddRestToFile(QTextStream* outfile) const {
  // constants and variables
  const QString fname = "jfFFN_UpdateBundle::AddRestToFile";
  bool rval;
  // checking and special conditions
  /**/JDEBUGLOG(fname,1);
  if (outfile==NULL) return false;
  /**/JDEBUGLOG(fname,2);
  // here, we write the results collection
  rval = result_data->AddToFile(outfile);
  /**/JDEBUGLOG(fname,3);
  return rval;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o input
//-----------------------------------
bool jfFFN_UpdateBundle::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_UpdateBundle::ReadMiddleFromFile";
  // input data
  QString cline, buffer;
  QDateTime datetime;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadLine(cline,funcname)) return false;
  datetime = QDateTime::fromString(cline,"MM-dd-yyyy");
  if (!datetime.isValid()) {
    /**/JDEBUGLOGS(funcname,2,cline);
    return infile->BuildError("Unable to convert the search date!");
  }
  else lastdate = datetime;
  /**/JDEBUGLOGST(funcname,3,sindex);
  // next we read the search data
  if (search_data!=NULL) delete search_data;
  search_data = new jfFFNSearch(chlink,sindex);
  /**/JDEBUGLOG(funcname,4);
  return search_data->GetFromFile(infile);
}
//-----------------------------------
bool jfFFN_UpdateBundle::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfFFN_UpdateBundle::ReadRestFromFile";
  assert(chlink!=NULL);
  // reading the result data
  /**/JDEBUGLOG(fname,1);
  assert(infile!=NULL);
  if (result_data!=NULL) delete result_data;
  /**/JDEBUGLOG(fname,2);
  result_data = new jfFFNResColl(search_data);
  // we assign the categories before reading
  result_data->SetCatLinks(chlink);
  /**/JDEBUGLOG(fname,3);
  if (!result_data->GetFromFile(infile)) {
    /**/JDEBUGLOGS(fname,4,infile->err);
    delete result_data;
    result_data = NULL;
    return false;
  }
  /**/JDEBUGLOG(fname,5);
  return true;
}
/****************************************************************************/
