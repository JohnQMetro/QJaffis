/*****************************************************************************
Name:        vectors.cpp
Author :     John Q Metro
Purpose :    vector declarations
Created:     April 29, 2009
Conversion to QT started : April 18, 2013
Updated:     August 7, 2015
******************************************************************************/
#ifndef VECTORS_H_INCLUDED
  #include "vectors.h"
#endif // VECTORS_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef BASEOBJ_H_INCLUDED
  #include "../objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED

#include <assert.h>
/******************************************************************************/
// constructor
jfResultUnitVector::jfResultUnitVector():std::vector<jfResultUnit*>() {
  stotal = 0;
  firstid = 0;
  source = NULL;
  limitreached = false;
}
//==============================================================================
QString jfIndexVector::ToDelimString(QChar dval) const {
  // constants and variables
  const QString num_str = "0123456789";
  QString result;
  size_t qindex;
  // checking the delimiter
  assert(!num_str.contains(dval));
  // going ahead
  for (qindex=0;qindex<size();qindex++) {
    result += QString::number(at(qindex));
    if (qindex!=(size()-1)) result += dval;
  }
  // done
  return result;
}
//---------------------------------------------------------------
bool jfIndexVector::SetFromString(QString inval, QChar dval) {
  // variables
  QStringList* was = NULL;
  unsigned long qval;
  size_t lindex, lsize;
  // converting
  was = ListToStrArr(inval,dval);
  clear();
  if (was==NULL) return true;
  lsize = was->size();
  if (lsize==0) {
    delete was;
    return true;
  }
  // now we loop and try to convert each string to a value
  for (lindex=0;lindex<lsize;lindex++) {
    if (!Str2ULong(was->at(lindex),qval)) {
      delete was;
      return false;
    }
    else push_back(qval);
  }
  // done
  delete was;
  return true;

}
//==============================================================================
/* DiscardTest returns true if the input is in the set, otherwise it adds the
 input to the set and returns false */
//--------------------------------------------
bool jfIDStore::DiscardTest(size_t testval) {
  // special case
  if (empty()) {
    insert(testval);
    return false;
  }
  // general
  else {
    if (find(testval)==end()) {
      insert(testval);
      return false;
    }
    else return true;
  }
}
//--------------------------------------------
bool jfIDStore::DiscardTest(jfBaseObj* testobj) {
  assert(testobj!=NULL);
  size_t testval = testobj->GetID();
  // special case
  if (empty()) {
    insert(testval);
    return false;
  }
  // general
  else {
    if (find(testval)==end()) {
      insert(testval);
      return false;
    }
    else return true;
  }
}
//===============================================================
void jfStrBoolList::AddItem(const QString& inname, bool indis) {
  jfStrBoolPair nv;
  nv.name = inname;
  nv.dis = indis;
  push_back(nv);
}
//===============================================================
jfIDStringStore::jfIDStringStore():std::map<size_t,QString>() {
  est_itemcount = 0;
}
//-------------------------------------------------------------
bool jfIDStringStore::Contains(const size_t &inid) const {
  return (count(inid)!=0);
}
//-------------------------------------------------------------
QString jfIDStringStore::GetString(const size_t& idkey) const {
  jfIDStringStore::const_iterator xkey;
  xkey = find(idkey);
  if (xkey==end()) return "";
  else return xkey->second;
}

/******************************************************************************/
