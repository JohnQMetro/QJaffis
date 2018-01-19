/******************************************************************************
// Name:        basecollect.cpp
// Author :     John Q Metro
// Purpose :    Defines core item collection classes
// Created:     March 19, 2009
// Conversion to QT Started April 8, 2013
// Updated:     January 18, 2018 (added isEmpty to jfFilterMap)
******************************************************************************/
#ifndef BASECOLLECT_H_INCLUDED
  #include "basecollect.h"
#endif // BASECOLLECT_H_INCLUDED

#ifndef JFILTERGLOBALS
  #include "../filters/filter_globals.h"
#endif

#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED

#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED
/*
#ifndef PAIRFILTER_H_INCLUDED
  #include "../filters/extended/pairfilter.h"
#endif // PAIRFILTER_H_INCLUDED
*/
//-----------------------
#include <assert.h>
//*****************************************************************************
// defines some common things for an ancestor collection class
jfBaseCollection::jfBaseCollection():jfBaseObj() {
  name = "Default Collection Name";
  item_count = 0;
  item_index = -1;
  curr_item = NULL;
  backup_index = -1;
  backup_item = NULL;
}
//---------------------------------------------------------------------
jfBaseCollection::jfBaseCollection(size_t src_id, const QString& nname):jfBaseObj(src_id,nname) {
  item_count = 0;
  item_index = -1;
  curr_item = NULL;
  backup_index = -1;
  backup_item = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementable data methods
size_t jfBaseCollection::GetCount() const {
  return item_count;
}
//---------------------------------------------------------------------------
bool jfBaseCollection::isEmpty() const {
  return (item_count==0);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// general index info
//---------------------------------------------------------------------------
int jfBaseCollection::GetIndex() const {
  return item_index;
}
//---------------------------------------------------------------------------
bool jfBaseCollection::ItemV() const {
  return (item_index<0);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// stuff for the current index
//---------------------------------------------------------------------------
QString jfBaseCollection::CurrName() const {
  return (curr_item->GetName());
}
//---------------------------------------------------------------------------
size_t jfBaseCollection::CurrID() const {
  return (curr_item->GetID());
}
//---------------------------------------------------------------------------
QString jfBaseCollection::CurDesc() const {
  return (curr_item->GetDescription());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// index manipulation
//---------------------------------------------------------------------------
void jfBaseCollection::SaveIndex() {
  backup_index = item_index;
  backup_item = curr_item;
}
//---------------------------------------------------------------------------
bool jfBaseCollection::RestoreIndex() {
  item_index = backup_index;
  curr_item = backup_item;
  return ItemV();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// calculates a replacement index if an item is being deleted
size_t jfBaseCollection::CalcNewIndex(size_t oindex) const {
  if (oindex<0) return oindex;
  /* the general rule is the index remains the same, unless the index is the
  last, in which case it is decremented */
  if (oindex==(item_count-1)) oindex--;
  return oindex;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the only thing we really do here is write the number or items
bool jfBaseCollection::AddMiddleToFile(QTextStream* outfile) const {
  QString buffer;
  bool rbuf;
  // checking and special conditions
  if (outfile==NULL) return false;
  // writing
  buffer = QString::number(item_count);
  (*outfile) << buffer << '\n';
  // virtual calls
  rbuf = AddMid2ToFile(outfile);
  if (!rbuf) return false;
  else return true;
}
//-------------------------------------------------------------------------
// we read the number of items
bool jfBaseCollection::ReadMiddleFromFile(jfFileReader* infile) {
  // local variables
  const QString fname = "jfBaseCollection::ReadMiddleFromFile";
  QString cline;
  unsigned long tval;
  // we do checks and read the line
  if (!infile->ReadULong(tval,fname)) return false;
  item_count = tval;
  // finishing off...
  return ReadMid2FromFile(infile);
}
//=============================================================================
// the default constructor
jfFilterMap::jfFilterMap():jfBaseCollection() {
  curr_iterat = coredata.end();
  backup_iterat = coredata.end();
  exlocmap = NULL;
}
//--------------------------------------------------------
bool jfFilterMap::SetMapPtr(jfFilterMap* nval) {
  if (nval==NULL) return false;
  exlocmap = nval;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns true if the map has an element with the name in lookfor, false otherwise
bool jfFilterMap::NameExists(const QString& lookfor) const {
  stl_FilterMap::const_iterator findex;
  // we do it
  if (!isEmpty()) {
    findex = coredata.find(lookfor);
    if (findex!=(coredata.end())) return true;
    else return false;
  }
  else return false;
}//---------------------------------------------------------------------------
// renaming map elements via removal and reinsertion
stl_FilterMap::iterator jfFilterMap::RenameElem(bool& ok, const QString& oldname, const QString& newname) {
  // local varaibles
  jfBaseFilter* temp;
  // checks
  assert(NameExists(oldname));
  // the new name is already in?
  if (NameExists(newname)) {
    ok = false;
    return coredata.end();
  }
  // the names are the same, nothing is done
  if (oldname==newname) {
    ok = true;
    return coredata.find(oldname);
  }
  // we are ready to change the name. It seems we must remove the element,
  temp = coredata[oldname];
  coredata.erase(oldname);
  // rename the element,
  temp->SetName(newname);
  // and re-add it to the map
  coredata[newname] = temp;
  // done
  ok = true;
  return coredata.find(newname);
}
//--------------------------------------------------------------------------
// returns the filter at the name
jfBaseFilter* jfFilterMap::GetItem(const QString& itemname) {
  // constants
  const QString fname = "jfFilterMap::GetItem";
  // variables
  stl_FilterMap::iterator findex;
  // starting
  if (isEmpty()) return NULL;
  // we get an iterator
  findex = coredata.find(itemname);
  if (findex==coredata.end()) return NULL;
  else return findex->second;
}
//-----------------------------------------------------------------------------
/* returns true iff all the filters in this map return true when using inval
as an input to thier Match() methods */
bool jfFilterMap::MatchAll(const jfBasePD* inval) const {
  const QString fname = "jfFilterMap::MatchAll";
  // local variables
  stl_FilterMap::const_iterator findex;
  bool iresult;
  QString mstring;
  // a check
  assert(inval!=NULL);
  // the loop
  for (findex = coredata.begin(); findex!=coredata.end(); findex++) {
    mstring = (findex->second)->ToString();
    iresult = (findex->second)->Match(inval);
    if (!iresult) return false;
  }
  return true;
}
//---------------------------------------------------------------------
/* Returns true if there are no filters, or all of the filters have isEmpty
 * methods that also return true. */
bool jfFilterMap::isEmpty() const {
    // local variables
    stl_FilterMap::const_iterator findex;
    const jfBaseFilter* cfilter;
    // a check
    if (coredata.empty()) return true;
    // the loop
    for (findex = coredata.begin(); findex!=coredata.end(); findex++) {
      cfilter = findex->second;
      if (!(cfilter->isEmpty())) return false;
    }
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some additonal methods
//------------------------------------------------------------------------
// adds a filter to the map, the position is given in outindex
bool jfFilterMap::AddFilter(jfBaseFilter* infilter, size_t& outindex) {
  // local variables
  QString filtername;
  bool xres;
  size_t qindex=88;
  // the two case where we return false
  if (infilter==NULL) return false;
  filtername = infilter->GetName();
  if (NameExists(filtername)) return false;
  // otherwise, we do the normal insertion...
  coredata[filtername] = infilter;
  // and get the index
  xres = GetIndexByName(filtername,qindex);
  item_count++;
  // repositioning the index
  if (qindex<=item_index) ToIndex(item_index+1);
  outindex = qindex;
  return xres;
}
//-----------------------------------------------------------------------------
// returns the index of a filter with a particular name
bool jfFilterMap::GetIndexByName(const QString& findname, size_t& outindex) const {
  stl_FilterMap::const_iterator findex;
  // finding the name...
  outindex = 0;
  for (findex=coredata.begin();findex!=coredata.end();findex++) {
    if ((findex->first)==findname) break;
    outindex++;
  }
  return (findex!=coredata.end());
}
//-----------------------------------------------------------------------------
jfBaseFilter* jfFilterMap::GetItem(const size_t& findex) {
  stl_FilterMap::const_iterator xfindex;
  size_t loopind;
  if (findex>=GetCount()) return NULL;
  // finding the item
  xfindex=coredata.begin();
  for (loopind=0;loopind<findex;loopind++) xfindex++;
  // getting the item
  return xfindex->second;
}
//-----------------------------------------------------------------------------
bool jfFilterMap::DeleteByName(const QString& findname) {
  stl_FilterMap::iterator findex;
  // if empty, return false
  if (isEmpty()) return false;
  // we try to get an iterator
  findex = coredata.find(findname);
  if (findex==coredata.end()) return false;
  // deleting
  delete findex->second;
  findex->second = NULL;
  coredata.erase(findname);
  item_count--;
  return true;
}
//----------------------------------------------------------------------------
// replaces a filter, if need be removing and re-inserting
bool jfFilterMap::ReplaceByName(QString nameold, jfBaseFilter* newfilter, size_t& outindex) {
  // local variables
  stl_FilterMap::iterator findex;
  QString newname;
  bool indexp = false;
  bool indexpbak = false;
  // starting checks and searches
  if (newfilter==NULL) return false;
  findex = coredata.find(nameold);
  if (findex==coredata.end()) return false;
  // we check to see if we need to alter any of the indexes
  indexp = (curr_item == findex->second);
  indexpbak = (backup_item == findex->second);
  // we delete the old filter
  delete findex->second;
  findex->second = NULL;
  // we now check if the name is the same
  newname = newfilter->GetName();
  if (nameold==newname) {
    // same name means we just stick newfilter in the old element
    findex->second = newfilter;
  }
  else {
    // otherwise, we erase the old element, the reinsert
    coredata.erase(nameold);
    coredata[newname] = newfilter;
  }
  // updating the indexes, if need be
  if (indexp) curr_item = newfilter;
  if (indexpbak) backup_item = newfilter;
  // finding the location
  outindex = 0;
  for (findex=coredata.begin();findex!=coredata.end();findex++) {
    if ((findex->first)==newname) break;
    outindex++;
  }
  if (indexp) curr_iterat = findex;
  if (indexpbak) backup_iterat = findex;
  // done
  return true;

}
//-----------------------------------------------------------------------------
/* Adds a filter if a filter with the same name is not in a map, otherwise
there is a replacement */
bool jfFilterMap::ReplaceSame(jfBaseFilter* newfilter, size_t& outindex) {
    // local variables
  stl_FilterMap::iterator findex;
  QString newname;
  bool indexp,indexpbak;
  // starting checks and searches
  if (newfilter==NULL) return false;
  newname = newfilter->GetName();
  findex = coredata.find(newname);
  // two options: the item is not here...
  if (findex==coredata.end()) {
    return AddFilter(newfilter,outindex);
  }
  // or it is...
  else {
    // we check to see if we need to alter any of the indexes
    indexp = (curr_item == findex->second);
    indexpbak = (backup_item == findex->second);
    // we delete the old filter
    delete findex->second;
    findex->second = NULL;
    // we replace it
    findex->second = newfilter;
    // updating the indexes, if need be
    if (indexp) curr_item = newfilter;
    if (indexpbak) backup_item = newfilter;
    // finding the location
    outindex = 0;
    for (findex=coredata.begin();findex!=coredata.end();findex++) {
      if ((findex->first)==newname) break;
      outindex++;
    }
    // done
    return true;
  }

}
//---------------------------------------------------------------------
size_t jfFilterMap::CopyLoad() {
  // local variables
  stl_FilterMap::iterator findex;
  stl_FilterMap::iterator thelast;
  QString tagtype;
  jfExpressionFilter* exprfil;
  size_t resval;
  // intialization
  thelast = coredata.end();
  resval = 0;
  // the loop
  for (findex=coredata.begin();findex!=thelast;findex++) {
    tagtype = findex->second->GetTypeID();
    if (tagtype=="ExprFilter") {
      exprfil = dynamic_cast<jfExpressionFilter*>(findex->second);
      exprfil->CopyLoad();
      resval++;
    }
  }
  // done
  return resval;
}
//--------------------------------------------------------
bool jfFilterMap::VerifyNames(QString& omsg, bool notinmap) {
  // local variables
  stl_FilterMap::iterator findex;
  stl_FilterMap::iterator thelast;
  QString tagtype;
  jfExpressionFilter* exprfil;
  bool resval;
  jfNameVerif* vval;
  // intialization
  thelast = coredata.end();
  resval = 0;
  vval = new jfNameVerif();
  resval = true;
  // the loop
  for (findex=coredata.begin();findex!=thelast;findex++) {
    tagtype = findex->second->GetTypeID();
    if (tagtype=="ExprFilter") {
      exprfil = dynamic_cast<jfExpressionFilter*>(findex->second);
      resval = exprfil->VerifyNames(vval,notinmap);
      if (!resval) {
        omsg = vval->GetError();
        break;
      }
    }
  }
  delete vval;
  // done
  return resval;
}
//---------------------------------------------------------
// calls pairfilter methods regarding statistics logging
size_t jfFilterMap::PairFilterLogging(bool write) {
  // local variables
  stl_FilterMap::iterator findex;
  stl_FilterMap::iterator thelast;
  /*
  jfPairFilterCore* exprfil;
  size_t tagval, resval;
  // intialization
  thelast = coredata.end();
  resval = 0;
  // the loop
  for (findex=coredata.begin();findex!=thelast;findex++) {
    tagval = findex->second->GetFilType();
    if (tagval==2) {
      exprfil = dynamic_cast<jfPairFilterCore*>(findex->second);
      if (write) exprfil->WriteLog();
      else exprfil->StartLogging();
      resval++;
    }
  }
  */
  // done
  return false;
  //return resval;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFilterMap::SaveIndex() {
  jfBaseCollection::SaveIndex();
  backup_iterat = curr_iterat;
}
//-----------------------------------------------------------------------
bool jfFilterMap::RestoreIndex() {
  bool res;
  res = jfBaseCollection::RestoreIndex();
  if (res) curr_iterat = backup_iterat;
  else curr_iterat = coredata.end();
  return res;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterMap::NextIndex() {
  // 4 cases here
  if (item_index==-2) return false;
  else if (item_index==(item_count-1)) {
    item_index = -2;
    curr_item = NULL;
    curr_iterat = coredata.end();
    return false;
  }
  else if (item_index==-1) {
    item_index = 0;
    curr_iterat = coredata.begin();
    curr_item = curr_iterat->second;
    return true;
  }
  else {
    item_index++;
    ++curr_iterat;
    curr_item = curr_iterat->second;
    return true;
  }
}
//----------------------------------------------------------------------------
bool jfFilterMap::BackIndex() {
  // 4 cases here
  if (item_index==-1) return false;
  else if (item_index==0) {
    item_index = -1;
    curr_item = NULL;
    return false;
  }
  else if (item_index==-2) {
    item_index = item_count-1;
    curr_iterat = coredata.end();
    curr_iterat--;
    curr_item = curr_iterat->second;
    return true;
  }
  else {
    item_index--;
    --curr_iterat;
    curr_item = curr_iterat->second;
    return true;
  }
}
//----------------------------------------------------------------------------
void jfFilterMap::ResetIndex() {
  item_index = -1;
  curr_item = NULL;
}
//----------------------------------------------------------------------------
bool jfFilterMap::ToIndex(const int& newindex) {
  size_t q;
  // basic checks
  if ((newindex<-2) || (newindex>=item_count)) return false;
  if (newindex<0) {
    item_index = newindex;
    curr_item = NULL;
    return false;
  }
  else {
    // as far as I can tell, there is no random access except via name
    curr_iterat = coredata.begin();
    item_index=0;
    for (q=0;q<(item_count-1);q++) {
      ++curr_iterat;
      ++item_index;
    }
    // at the item
    curr_item = curr_iterat->second;
    return true;
  }
}
//----------------------------------------------------------------------------
QString jfFilterMap::GetTypeID() const {
  return QString("jfFilterMap");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QStringList* jfFilterMap::GetNameList()  const {
  stl_FilterMap::const_iterator namedex;
  size_t qval;
  QStringList* result;
  QString nbuf;
  // special case
  if (item_count==0) return NULL;
  // we start
  namedex = coredata.begin();
  qval = 0;
  result = new QStringList();
  // the name appending loop
  while (true) {
    nbuf = (namedex->second)->GetName();
    result->append(nbuf);
    // the exit condition
    if (qval==(item_count-1)) break;
    // incrementing
    qval++;
    ++namedex;
  }
  // done
  return result;
}
//-----------------------------------------------------------------------------
QStringList* jfFilterMap::GetDescriptionList() const {
  stl_FilterMap::const_iterator descdex;
  size_t qval;
  QStringList* result;
  QString nbuf;
  // special case
  if (item_count==0) return NULL;
  // we start
  descdex = coredata.begin();
  qval = 0;
  result = new QStringList();
  // the name appending loop
  while (true) {
    nbuf = (descdex->second)->GetDescription();
    result->append(nbuf);
    // the exit condition
    if (qval==(item_count-1)) break;
    // incrementing
    qval++;
    ++descdex;
  }
  // done
  return result;
}
//-----------------------------------------------------------------------------
QStringList* jfFilterMap::GetTypeList() const {
  stl_FilterMap::const_iterator descdex;
  size_t qval;
  QStringList* result;
  QString nbuf;
  // special case
  if (item_count==0) return NULL;
  // we start
  descdex = coredata.begin();
  qval = 0;
  result = new QStringList();
  // the name appending loop
  while (true) {
    nbuf = (descdex->second)->GetTypeID();
    result->append(nbuf);
    // the exit condition
    if (qval==(item_count-1)) break;
    // incrementing
    qval++;
    ++descdex;
  }
  // done
  return result;
}
//-----------------------------------------------------------------------------
bool jfFilterMap::DeleteAtIndex(const size_t& dindex) {
  stl_FilterMap::iterator deldex;
  size_t q;
  int old_itemindex;
  // we check the index against the size
  if (dindex>=item_count) return false;
  // we now check the index to remove against the current index
  // if the index is the index to remove, we move it to the next one
  if (dindex==item_index) {
    deldex = curr_iterat;
    NextIndex();
  }
  // otherwise, we have to loop to get the deldex
  else {
    deldex = coredata.begin();
    for (q=0;q<=dindex;q++) deldex++;
  }
  // we also check the saved index, if it matches, we just remove that index
  if (dindex==backup_index) {
    backup_index = -1;
    backup_item = NULL;
  }
  // then we remove the filter
  delete deldex->second;
  deldex->second = NULL;
  coredata.erase(deldex);
  item_count--;
  // resetting the pointers
  old_itemindex = item_index;
  if (backup_index>dindex) {
    ToIndex(backup_index-1);
    SaveIndex();
  }
  if (old_itemindex>dindex) ToIndex(backup_index-1);
  else ToIndex(backup_index);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// deletes all filters, emptying the map
void jfFilterMap::DeleteAll() {
  stl_FilterMap::iterator deldex;
  if (item_count!=0) {
    // deleting all filters
    for (deldex=coredata.begin();deldex!=coredata.end();deldex++) {
      delete (deldex->second);
      deldex->second = NULL;
    }
    // emptying the list
    coredata.clear();
    item_count = 0;
    // setting the indexes
    curr_iterat = coredata.end();
    backup_iterat = coredata.end();
    item_index = -1;
    curr_item = NULL;
    backup_index = -1;
    backup_item = NULL;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//--------------------------------------------------------------------
// this method actually does nothing
bool jfFilterMap::AddMid2ToFile(QTextStream* outfile) const {
  return true;
}
//--------------------------------------------------------------------
// we write the contents of the map to file
bool jfFilterMap::AddRestToFile(QTextStream* outfile) const {
  // local variables
  stl_FilterMap::const_iterator descdex,enddex;
  // the usual checks
  if (outfile==NULL) return false;
  // we prepare for the loop
  enddex = coredata.end();
  // the loop
  for (descdex = coredata.begin() ; descdex != enddex ; descdex++) {
    // writing
    (descdex->second)->AddToFile(outfile);
  }
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// like the write version, this method does nothing
bool jfFilterMap::ReadMid2FromFile(jfFileReader* infile) {
  return true;
}
//--------------------------------------------------------------------
/* reads in the list. slighty complicated because the items can be of different
types */
bool jfFilterMap::ReadRestFromFile(jfFileReader* infile) {
  // constants
  const QString fname = "jfFilterMap::ReadRestFromFile";
  // local variables
  size_t loopc;
  QString typid;
  jfBaseFilter* newitem;
  jfExpressionFilter* exp_item;
  // the usual initial checks
  /**/JDEBUGLOGST(fname,1,item_count)
  assert(infile!=NULL);
  // the loop
  for (loopc=0;loopc<item_count;loopc++) {
    /**/JDEBUGLOGST(fname,3,loopc)
    // gets the filter type string (and also the line index)
    if (!infile->GetType(typid,fname)) return false;
    /**/JDEBUGLOGS(fname,4,typid)
    // we make an empty default filter
    newitem = MakeFilter(typid,QString::number(loopc));
    // expression filters
    if (typid=="ExprFilter") {
      exp_item = dynamic_cast<jfExpressionFilter*>(newitem);
      if (exlocmap!=NULL) exp_item->SetFiltermapLink(exlocmap);
      else exp_item->SetFiltermapLink(this);
    }
    /**/JDEBUGLOG(fname,5)
    if (newitem==NULL) return infile->BuildError("Unrecognized Filter type!");
    assert(newitem!=NULL);
    /**/JDEBUGLOG(fname,6)
    // and then load
    if (!newitem->GetFromFile(infile)) {
      delete newitem;
      return false;
    }
    /**/JDEBUGLOGS(fname,7,newitem->GetName())
    // we add the newly loaded item to the list
    coredata[newitem->GetName()] = newitem;
    /**/JDEBUGLOG(fname,8)
  }
  return true;
  // done
}
//*****************************************************************************
