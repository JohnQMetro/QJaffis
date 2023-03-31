/***************************************************************************
 * Name    : cat_collections.cpp
 * Author  : John Q Metro
 * Purpose : Defines the groupings / collections that directly hold FFN
 *            Category objects.
 * Created : May 25, 2014
 * Updated : July 20, 2016
 **************************************************************************/
#ifndef CAT_COLLECTIONS_H
  #include "cat_collections.h"
#endif // CAT_COLLECTIONS_H
//---------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//---------------------------------------
#include <assert.h>
#include <QMutexLocker>
/**************************************************************************/
// section map convenience methods
//---------------------------------------
bool jfFFN_SectionMap::Contains(const QString& cckey) const {
  return (count(cckey)!=0);
}
//---------------------------------------
bool jfFFN_SectionMap::AddCheck(jfFFN_SectionCategory* incat) {
  if (incat==NULL) return false;
  QString keyn = incat->GetName();
  if (Contains(keyn)) return false;
  (*this)[keyn] = incat;
  return true;
}
//---------------------------------------
void jfFFN_SectionMap::Add(jfFFN_SectionCategory* incat) {
  QString keyn = incat->GetName();
  (*this)[keyn] = incat;
}
//---------------------------------------
bool jfFFN_SectionMap::MarkByName(const QString& inname, bool value, size_t which) {
  if (!Contains(inname)) return false;
  jfFFN_SectionCategory* catptr = (*this)[inname];
  if (catptr==NULL) return false; //this should never happen
  (catptr->mark).set(which,value);
  return true;
}
//---------------------------------------
bool jfFFN_SectionMap::GetMarkByName(const QString& inname, size_t which) const {
  if (!Contains(inname)) return false;
  jfFFN_SectionMap::const_iterator catptr = find(inname);
  if ((catptr->second)==NULL) return false; //this should never happen
  return ((catptr->second)->mark).test(which);
}
//---------------------------------------
bool jfFFN_SectionMap::TestAndMarkByName(const QString& inname, size_t which, bool value, int& result) {
  if (!Contains(inname)) return false;
  jfFFN_SectionCategory* catptr = (*this)[inname];
  bool oldval = (catptr->mark).test(which);
  if (oldval==value) result = 0;
  else {
    if (value) result = 1;
    else result = -1;
    (catptr->mark).set(which,value);
  }
  return true;
}
//---------------------------------------
size_t jfFFN_SectionMap::DeleteAndClear() {
  if (empty()) return 0;
  jfFFN_SectionMap::iterator idxer;
  size_t xcount = 0;
  idxer = begin();
  while (idxer!=end()) {
    delete idxer->second;
    idxer->second = NULL;
    idxer++;
    xcount++;
  }
  clear();
  return xcount;
}
//---------------------------------------
jfFFN_SectionMap::iterator jfFFN_SectionMap::IteratorByIndex(size_t idxval) {
  if (idxval>=size()) return end();
  jfFFN_SectionMap::iterator output;
  size_t idxer;
  // iterating
  output = begin();
  for (idxer = 1; idxer<=idxval; idxer++) ++output;
  return output;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods for the marked categories storage
size_t jfFFN_SectionMap::AppendToList(jfFFN_CatList& inlist) {
  if (size()==0) return 0;
  jfFFN_SectionMap::iterator cpy_idx;
  // starting
  cpy_idx = begin();
  // the loop
  while (cpy_idx!=end()) {
    inlist.push_back(cpy_idx->second);
    cpy_idx++;
  }
  return size();
}
//=========================================================================
jfFFNSection::jfFFNSection():jfBaseCollection() {
  for (size_t mindex=0;mindex<32;mindex++) markcount[mindex] = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNSection::GetTypeID() const {
  return "FFNSection";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting info
//--------------------------
bool jfFFNSection::AddCategory(jfFFN_SectionCategory* cat_to_add) {
  if (cat_to_add == NULL) return false;
  if (!(cat_to_add->IsValid())) return false;
  maindata.Add(cat_to_add);
  item_count++;
  validdata = true;
  return true;
}
//--------------------------
bool jfFFNSection::MarkAll(size_t which, bool value) {
  if (which>32) return false;
  jfFFN_SectionMap::iterator ival;
  ival = maindata.begin();
  while (ival!=maindata.end()) {
    ((ival->second)->mark).set(which,value);
    ival++;
  }
  if (value) markcount[which] = item_count;
  else markcount[which] = 0;
  return true;
}
//--------------------------
bool jfFFNSection::MarkByName(const QString& inname, bool value, size_t which) {
  if (which>32) return false;
  int rval;
  if (maindata.TestAndMarkByName(inname,which,value,rval)) {
    markcount[which] += rval;
    return true;
  }
  else return false;
}
//--------------------------
bool jfFFNSection::MarkAndAddByName(jfFFN_SectionMap& marked_collection,const QString& inname, size_t which) {
  // some special cases which give false results
  if (which>=32) return false;
  if (!maindata.Contains(inname)) return false;
  // variables
  jfFFN_SectionCategory* tp;
  // starting
  tp = maindata[inname];
  // testing
  bool tval = (tp->mark).test(which);
  if (!tval) {
    (tp->mark).set(which,true);
    markcount[which]++;
  }
  // finally
  return marked_collection.AddCheck(tp);
}
//--------------------------
bool jfFFNSection::MarkAndAddAll(jfFFN_SectionMap& marked_collection, size_t which) {
  // special cases
  if (which>=32) return false;
  if (maindata.size()==0) return false;
  // variables
  jfFFN_SectionMap::iterator xi;
  jfFFN_SectionCategory* tp;
  // starting
  xi = maindata.begin();
  // looping
  while (xi!=maindata.end()) {
    tp = xi->second;
    (tp->mark).set(which,true);
    marked_collection.Add(tp);
    xi++;
  }
  // done
  markcount[which] = maindata.size();
  return true;
}
//--------------------------
bool jfFFNSection::UnmarkAndRemoveByName(jfFFN_SectionMap& marked_collection,const QString& inname, size_t which) {
  // bad cases
  if (which>=32) return false;
  if (!maindata.Contains(inname)) return false;
  if (!marked_collection.Contains(inname)) return false;
  // general case, get the the item
  jfFFN_SectionCategory* tp = marked_collection[inname];
  // unmarking the flag
  bool tval = (tp->mark).test(which);
  if (tval) {
    markcount[which]--;
    (tp->mark).set(which,false);
  }
  // removing the item
  marked_collection.erase(inname);
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// index manipulation
//--------------------------
void jfFFNSection::SaveIndex() {
  backup_index = item_index;
  backup_item = curr_item;
  bptr = cptr;
}
//--------------------------
bool jfFFNSection::RestoreIndex() {
  item_index = backup_index;
  curr_item = backup_item;
  cptr = bptr;
  return ItemV();
}
//--------------------------
bool jfFFNSection::NextIndex() {
  if (item_count==0) {
    if (item_index==-1) {
      item_index = -2;
      cptr = maindata.end();
    }
    return false;
  }
  // if there are items...
  else {
    // already at the end...
    if (item_index == -2) return false;
    // moving off the end
    if (item_index == (item_count-1)) {
      item_index = -2;
      curr_item = NULL;
      cptr = maindata.end();
      return false;
    }
    // the normal case..
    item_index++;
    if (item_index==0) cptr = maindata.begin();
    else cptr++;
    curr_item = cptr->second;
    return true;
  }
}
//--------------------------
bool jfFFNSection::BackIndex() {
  // if there are no items, a special case...
  if (item_count==0) {
    if (item_index==-2) item_index = -1;
    return false;
  }
  // if there are items...
  else {
    // already at the start
    if (item_index == -1) return false;
    // moving off start..
    if (item_index == 0) {
      item_index = -1;
      curr_item = NULL;
      return false;
    }
    // moving off the end..
    if (item_index == -2)  {
      item_index = item_count-1;
      cptr = maindata.end();
      cptr--;
    }
    // or a regular middle index..
    else {
      item_index--;
      cptr--;
    }
    // we set the object
    curr_item = cptr->second;
    return true;
  }
}
//--------------------------
void jfFFNSection::ResetIndex() {
  item_index = -1;
  curr_item = NULL;
}
//--------------------------
bool jfFFNSection::ToIndex(const int& newindex) {
  // out of bounds ..
  if (newindex>=item_count) return false;
  if (newindex<-2) return false;
  // special cases
  if (newindex<0) {
    curr_item = NULL;
    if (newindex==-2) cptr = maindata.end();
    item_index = newindex;
  }
  // more general case
  cptr = maindata.IteratorByIndex(newindex);
  curr_item = cptr->second;
  item_index = newindex;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// current item operations
//------------------------------
jfFFN_SectionCategory* jfFFNSection::GetCurrent() {
  return dynamic_cast<jfFFN_SectionCategory*>(curr_item);
}
//------------------------------
QString jfFFNSection::GetCurrentName() const {
  if (curr_item==NULL) return "";
  else return curr_item->GetName();
}
//------------------------------
bool jfFFNSection::MarkCurrentAndAdd(jfFFN_SectionMap& marked_collection, size_t which) {
  jfFFN_SectionCategory* tp;
  // special case...
  if (which>=32) return false;
  if (curr_item==NULL) return false;
  // testing
  tp = dynamic_cast<jfFFN_SectionCategory*>(curr_item);
  bool tval = (tp->mark).test(which);
  if (!tval) {
    (tp->mark).set(which,true);
    markcount[which]++;
  }
  // finally
  return marked_collection.AddCheck(tp);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more abstract methods
//--------------------------
QStringList* jfFFNSection::GetNameList() const {
  // local variables
  QStringList* result;
  jfFFN_SectionMap::const_iterator nlptr;
  // we start...
  if (item_count==0) return NULL;
  result = new QStringList();
  nlptr = maindata.begin();
  // the main loop
  while (nlptr!=maindata.end()) {
    result->append(nlptr->first);
    nlptr++;
  }
  // done
  return result;
}
//--------------------------
QStringList* jfFFNSection::GetDescriptionList() const {
  // local variables
  QStringList* result;
  jfFFN_SectionMap::const_iterator nlptr;
  // we start...
  if (item_count==0) return NULL;
  result = new QStringList();
  nlptr = maindata.begin();
  // the main loop
  while (nlptr!=maindata.end()) {
    result->append((nlptr->second)->GetDescription());
    nlptr++;
  }
  // done
  return result;
}
//--------------------------
// unlike filters, we only have one type here
QStringList* jfFFNSection::GetTypeList() const {
  // local variables
  size_t iindex;
  QStringList* result;
  jfFFN_SectionMap::const_iterator nlptr;
  // we start...
  if (item_count==0) return NULL;
  result = new QStringList();
  for (iindex=0;iindex<item_count;iindex++) {
    result->append("FFN_SectionCategory");
  }
  return result;
}
//--------------------------
bool jfFFNSection::DeleteAtIndex(const size_t& dindex) {
  // first, we do the usual checks
  if (dindex<0) return false;
  if (dindex>=item_count) return false;
  // pointers
  bool clast, blast, cdel, bdel;
  jfFFN_SectionMap::iterator dptr;
  // we get index info
  clast = (item_index==(item_count-1));
  blast = (backup_index==(item_count-1));
  cdel = (dindex<=item_index);
  bdel = (dindex<=backup_index);
  dptr = maindata.IteratorByIndex(dindex);
  // deleting the item
  delete dptr->second;
  dptr->second = NULL;
  maindata.erase(dptr);
  // redoing the indexes...
  if (clast) item_index--;
  if (blast) backup_index--;
  if (item_index==-1) curr_item = NULL;
  else if (cdel){
    cptr = maindata.IteratorByIndex(item_index);
    curr_item = cptr->second;
  }
  if (backup_index==-1) backup_item = NULL;
  else if (bdel){
    bptr = maindata.IteratorByIndex(backup_index);
    backup_item = bptr->second;
  }
  // done
  item_count--;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
const jfFFN_SectionCategory* jfFFNSection::GetCatByName(const QString& catname) const {
  // trivially bad cases
  if (catname.isEmpty()) return NULL;
  if (maindata.size()==0) return NULL;
  if (!maindata.Contains(catname)) return NULL;
  // non bad case
  jfFFN_SectionMap::const_iterator qxi;
  qxi = maindata.find(catname);
  return qxi->second;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFFNSection::~jfFFNSection() {
  maindata.DeleteAndClear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual io methods
//--------------------------------------
bool jfFFNSection::AddMid2ToFile(QTextStream* outfile) const {
  return true;
}
//--------------------------------------
bool jfFFNSection::AddRestToFile(QTextStream* outfile) const {
  // local variables
  jfFFN_SectionMap::const_iterator descdex,enddex;
  // the usual checks
  if (outfile==NULL) return false;
  // we prepare for the loop
  enddex = maindata.end();
  // the loop
  for (descdex = maindata.begin() ; descdex != enddex ; descdex++) {
    // writing
    (descdex->second)->AddToFile(outfile);
  }
  // done
  return true;
}
//--------------------------------------
bool jfFFNSection::ReadMid2FromFile(jfFileReader* infile) {
  return true;
}

//--------------------------------------
bool jfFFNSection::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfTypedCollection< Q >::ReadRestFromFile";
  // local variables
  size_t loopc;
  jfFFN_SectionCategory* initem;
  /* item count is already read, we assume the line index is just before the first item */
  for (loopc=0;loopc<item_count;loopc++) {
    // we prepare to read the item
    initem = new jfFFN_SectionCategory();
    // reading the item
    if (!initem->GetFromFile(infile)) {
      delete initem;
      return false;
    }
    // testing just in case
    if (maindata.Contains(initem->GetName())) {
      infile->BuildError("Duplicate Category Name! : " + (initem->GetName()));
      delete initem;
      return false;
    }
    // adding the item to our list
    maindata.Add(initem);
  }
  // done
  return true;
}
//*************************************************************************
// some convenience methods
//------------------------------------
bool jfFFN_CrossoverSet::Contains(const jfFFN_CrossoverCategory* look) const {
  if (look==NULL) return false;
  return (count(look)!=0);
}
//------------------------------------
bool jfFFN_CrossoverSet::AddCheck(jfFFN_CrossoverCategory* incat) {
  if (incat==NULL) return false;
  if (Contains(incat)) return false;
  insert(incat);
  return true;
}
//------------------------------------
size_t jfFFN_CrossoverSet::DeleteAndClear() {
  const jfFFN_CrossoverCategory* item;
  jfFFN_CrossoverSet::iterator zi = begin();
  size_t res = size();
  while (zi != end()) {
    item = (*zi);
    erase(zi);
    delete item;
    zi = begin();
  }
  clear();
  return res;
}

//------------------------------------
jfFFN_CrossoverSet::iterator jfFFN_CrossoverSet::IteratorByIndex(size_t idxval) {
  if (idxval>=size()) return end();
  jfFFN_CrossoverSet::iterator output;
  size_t idxer;
  // iterating
  output = begin();
  for (idxer = 1; idxer<=idxval; idxer++) ++output;
  return output;
}
//==========================================================================
jfFFN_CrossCatStore::jfFFN_CrossCatStore():jfBaseCollection() {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
QString jfFFN_CrossCatStore::GetTypeID() const {
  return "FFN_CrossCatStore";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// manipulating contents
//------------------------------
/* If toadd is NULL, returns NULL. If a category equivalent to toadd is already stored, returns
 * a pointer to that category (and success is false). Otherwise, the category is added, a link
 * to that category is returned, and success is true */
jfFFN_CrossoverCategory* jfFFN_CrossCatStore::AddCategory(const jfFFN_CrossoverCategory* toadd, bool& success) {
  const QString fname = "jfFFN_CrossCatStore::AddCategory";
  jfFFN_CrossoverSet::iterator xi;
  QMutexLocker xlk(&mlock);
  // trying to add a NULL value, bad!
  if (toadd==NULL) {
    success = false;
    return NULL;
  }
  // trying to add an eqivalent element
  if (maindata.Contains(toadd)) {
    success = false;
    xi = maindata.find(toadd);
    return const_cast<jfFFN_CrossoverCategory*>(*xi);
  }
  // adding a new value
  else {
    success = true;
    xi = (maindata.insert(toadd)).first;
    item_count++;
    return const_cast<jfFFN_CrossoverCategory*>(*xi);
  }
}
//-----------------------------
jfFFN_CrossoverCategory* jfFFN_CrossCatStore::AddCategoryAndSetSection(const
      jfFFN_CrossoverCategory* toadd,bool& success,size_t idptr, const QString& insection) {
  //--- Variables ----
  jfFFN_CrossoverSet::iterator xi;
  jfFFN_CrossoverCategory* ccptr = NULL;
  QMutexLocker xlk(&mlock);
  // -- Starting ---
  // trying to add a NULL value, bad!
  if (toadd==NULL) {
    success = false;
    return NULL;
  }
  // trying to add an eqivalent element
  if (maindata.Contains(toadd)) {
    success = false;
    xi = maindata.find(toadd);
  }
  // adding a new value
  else {
    success = true;
    xi = (maindata.insert(toadd)).first;
    item_count++;
  }
  ccptr = const_cast<jfFFN_CrossoverCategory*>(*xi);
  // setting extra section info
  if ((idptr == 8) || (idptr == 1955)) {
      validdata = true;
  }

  ccptr->SetSectionByID(idptr,insection);
  validdata = true;
  return ccptr;
}
//------------------------------
/* Adds a category, if it already exists, the one there has the section data
 * altered to match the new one. */
jfFFN_CrossoverCategory* jfFFN_CrossCatStore::AddUpdateCategory(jfFFN_CrossoverCategory* toadd, bool& newinsert) {
  const QString fname = "jfFFN_CrossCatStore::AddReplaceCategory";
  QMutexLocker xlk(&mlock);
  // working variables
  jfFFN_CrossoverSet::iterator xi;
  jfFFN_CrossoverCategory* oldcat;

  // trying to add a NULL value, bad!
  assert(toadd != NULL);
  // if there is an equivalent category already, we erase the existing one
  if (maindata.Contains(toadd)) {
    xi = maindata.find(toadd);
    oldcat = const_cast<jfFFN_CrossoverCategory*>(*xi);
    oldcat->CopySections(*toadd);
    newinsert = false;
  }
  // adding the new category
  else {
    xi = (maindata.insert(toadd)).first;
    item_count++;
    newinsert = true;
    oldcat = const_cast<jfFFN_CrossoverCategory*>(*xi);
  }
  return oldcat;
}

//------------------------------
/* Returns a crossover category object with the specified ids. If
 *none is found, returns NULL. */
jfFFN_CrossoverCategory* jfFFN_CrossCatStore::GetByIDs(size_t inid1, size_t inid2) {
  // variables
  jfFFN_CrossoverCategory* dummy;
  jfFFN_CrossoverSet::iterator xptr;
  QMutexLocker xlk(&mlock);
  // building a dummy object to look for
  dummy = new jfFFN_CrossoverCategory(inid1,inid2);
  // looking
  xptr = maindata.find(dummy);
  delete dummy;
  // checking the results
  if (xptr==maindata.end()) return NULL;
  else return const_cast<jfFFN_CrossoverCategory*>(*xptr);
}
//----------------------------------
void jfFFN_CrossCatStore::ClearMarks(const size_t& which) {
  jfFFN_CrossoverCategory* zog;
  QMutexLocker xlk(&mlock);
  jfFFN_CrossoverSet::iterator xptr = maindata.begin();
  while (xptr!=maindata.end()) {
    zog = const_cast<jfFFN_CrossoverCategory*>(*xptr);
    (zog->mark).reset(which);
    xptr++;
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// index manipulation
//--------------------------
void jfFFN_CrossCatStore::SaveIndex() {
  QMutexLocker xlk(&mlock);
  backup_index = item_index;
  backup_item = curr_item;
  bptr = cptr;
}
//--------------------------
bool jfFFN_CrossCatStore::RestoreIndex() {
  QMutexLocker xlk(&mlock);
  item_index = backup_index;
  curr_item = backup_item;
  cptr = bptr;
  return ItemV();
}
//--------------------------
bool jfFFN_CrossCatStore::NextIndex() {
  QMutexLocker xlk(&mlock);
  if (item_count==0) {
    if (item_index==-1) {
      item_index = -2;
      cptr = maindata.end();
    }
    return false;
  }
  // if there are items...
  else {
    // already at the end...
    if (item_index == -2) return false;
    // moving off the end
    if (item_index == (item_count-1)) {
      item_index = -2;
      curr_item = NULL;
      cptr = maindata.end();
      return false;
    }
    // the normal case..
    item_index++;
    if (item_index==0) cptr = maindata.begin();
    else cptr++;
    curr_item = const_cast<jfFFN_CrossoverCategory*>(*cptr);
    return true;
  }
}
//--------------------------
bool jfFFN_CrossCatStore::BackIndex() {
  QMutexLocker xlk(&mlock);
  // if there are no items, a special case...
  if (item_count==0) {
    if (item_index==-2) item_index = -1;
    return false;
  }
  // if there are items...
  else {
    // already at the start
    if (item_index == -1) return false;
    // moving off start..
    if (item_index == 0) {
      item_index = -1;
      curr_item = NULL;
      return false;
    }
    // moving off the end..
    if (item_index == -2)  {
      item_index = item_count-1;
      cptr = maindata.end();
      cptr--;
    }
    // or a regular middle index..
    else {
      item_index--;
      cptr--;
    }
    // we set the object
    curr_item = const_cast<jfFFN_CrossoverCategory*>(*cptr);
    return true;
  }
}
//--------------------------
void jfFFN_CrossCatStore::ResetIndex() {
  QMutexLocker xlk(&mlock);
  item_index = -1;
  curr_item = NULL;
}
//--------------------------
bool jfFFN_CrossCatStore::ToIndex(const int& newindex) {
  QMutexLocker xlk(&mlock);
  // out of bounds ..
  if (newindex>=item_count) return false;
  if (newindex<-2) return false;
  // special cases
  if (newindex<0) {
    curr_item = NULL;
    if (newindex==-2) cptr = maindata.end();
    item_index = newindex;
  }
  // more general case
  cptr = maindata.IteratorByIndex(newindex);
  curr_item = const_cast<jfFFN_CrossoverCategory*>(*cptr);
  item_index = newindex;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more abstract methods
//--------------------------
QStringList* jfFFN_CrossCatStore::GetNameList() const {
  // local variables
  QStringList* result;
  jfFFN_CrossoverSet::const_iterator nlptr;
  // we start...
  QMutexLocker xlk(&mlock);
  if (item_count==0) return NULL;
  result = new QStringList();
  nlptr = maindata.begin();
  // the main loop
  while (nlptr!=maindata.end()) {
    result->append((*nlptr)->GetName());
    nlptr++;
  }
  // done
  return result;
}
//--------------------------
QStringList* jfFFN_CrossCatStore::GetDescriptionList() const {
  // local variables
  QStringList* result;
  jfFFN_CrossoverSet::const_iterator nlptr;
  // we start...
  QMutexLocker xlk(&mlock);
  if (item_count==0) return NULL;
  result = new QStringList();
  nlptr = maindata.begin();
  // the main loop
  while (nlptr!=maindata.end()) {
    result->append((*nlptr)->GetDescription());
    nlptr++;
  }
  // done
  return result;
}
//--------------------------
// unlike filters, we only have one type here
QStringList* jfFFN_CrossCatStore::GetTypeList() const {
  // local variables
  size_t iindex;
  QStringList* result;
  // we start...
  QMutexLocker xlk(&mlock);
  if (item_count==0) return NULL;
  result = new QStringList();
  for (iindex=0;iindex<item_count;iindex++) {
    result->append("FFN_SectionCategory");
  }
  return result;
}
//--------------------------
bool jfFFN_CrossCatStore::DeleteAtIndex(const size_t& dindex) {
  QMutexLocker xlk(&mlock);
  // first, we do the usual checks
  if (dindex<0) return false;
  if (dindex>=item_count) return false;
  // pointers
  bool clast, blast, cdel, bdel;
  jfFFN_CrossoverSet::iterator dptr;
  jfFFN_CrossoverCategory* xitem;
  // we get index info
  clast = (item_index==(item_count-1));
  blast = (backup_index==(item_count-1));
  cdel = (dindex<=item_index);
  bdel = (dindex<=backup_index);
  dptr = maindata.IteratorByIndex(dindex);
  // deleting the item
  xitem = const_cast<jfFFN_CrossoverCategory*>(*dptr);
  maindata.erase(dptr);
  delete xitem;
  // redoing the indexes...
  if (clast) item_index--;
  if (blast) backup_index--;
  if (item_index==-1) curr_item = NULL;
  else if (cdel){
    cptr = maindata.IteratorByIndex(item_index);
    curr_item = const_cast<jfFFN_CrossoverCategory*>(*cptr);
  }
  if (backup_index==-1) backup_item = NULL;
  else if (bdel){
    bptr = maindata.IteratorByIndex(backup_index);
    backup_item = const_cast<jfFFN_CrossoverCategory*>(*bptr);
  }
  // done
  item_count--;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFFN_CrossCatStore::~jfFFN_CrossCatStore() {
  maindata.DeleteAndClear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual io methods
//--------------------------------------
bool jfFFN_CrossCatStore::AddMid2ToFile(QTextStream* outfile) const {
  return true;
}
//--------------------------------------
bool jfFFN_CrossCatStore::AddRestToFile(QTextStream* outfile) const {
  // local variables
  jfFFN_CrossoverSet::const_iterator descdex,enddex;
  // the usual checks
  if (outfile==NULL) return false;
  // we prepare for the loop
  enddex = maindata.end();
  // the loop
  for (descdex = maindata.begin() ; descdex != enddex ; descdex++) {
    // writing
    (*descdex)->AddToFile(outfile);
  }
  // done
  return true;
}
//--------------------------------------
bool jfFFN_CrossCatStore::ReadMid2FromFile(jfFileReader* infile) {
  return true;
}

//--------------------------------------
bool jfFFN_CrossCatStore::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfTypedCollection< Q >::ReadRestFromFile";
  // local variables
  size_t loopc;
  jfFFN_CrossoverCategory* initem;
  jfFFN_CrossoverSet::iterator hint;
  /* item count is already read, we assume the line index is just before the first item */
  for (loopc=0;loopc<item_count;loopc++) {
    // we prepare to read the item
    initem = new jfFFN_CrossoverCategory();
    // reading the item
    if (!initem->GetFromFile(infile)) {
      delete initem;
      return false;
    }
    // testing just in case
    if (maindata.Contains(initem)) {
      infile->BuildError("Duplicate Category Name! : " + (initem->GetName()));
      delete initem;
      return false;
    }
    // adding the item to our list
    if (loopc==0) {
      maindata.insert(initem);
      hint = maindata.begin();
    }
    else {
      maindata.insert(hint,initem);
      hint++;
    }
  }
  // done
  return true;
}

/**************************************************************************/
