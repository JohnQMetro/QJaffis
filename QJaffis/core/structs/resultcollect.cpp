/*****************************************************************************
Name:        resultcollect.cpp
Author :     John Q Metro
Purpose :    A structure for holding one or more item collections
Created:     May 27, 2009
Conversion to Qt Started November 9, 2013
Updated:     July 16, 2016
******************************************************************************/
#ifndef RESULTCOLLECT_H_INCLUDED
  #include "resultcollect.h"
#endif // RESULTCOLLECT_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//-------------------------------
#include <assert.h>
#include <QDateTime>
//*****************************************************************************
// the defult constructor... just helps with the nulls
jfResultCollection::jfResultCollection(jfSearchCore* insearch, bool idfilter):jfBaseObj() {
  // initializations
  assert(insearch!=NULL);
  collections = NULL;
  cprotect = NULL;
  itemcount = 0;
  search = insearch;
  cindex = 0;
  if (idfilter) idstore = new jfIDStore();
  else idstore = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// public methods
jfSearchCore* jfResultCollection::GetSearch() const {
  return search;
}
//-------------------------------------------------------------------------
bool jfResultCollection::HasCollections() const {
  if (collections==NULL) return false;
  else return ((collections->size())!=0);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// wrapper to access the collection at the current index
//-----------------------------------------------------------------------
size_t jfResultCollection::PageIndex() const {
  if (itemcount==0) return 0;
  assert(cindex<itemcount);
  return (*collections)[cindex]->InsertIndex();
}
//-----------------------------------------------------------------------
size_t jfResultCollection::ItemCount() const {
  if (itemcount==0) return 0;
  assert(cindex<itemcount);
  return (*collections)[cindex]->GetCount();
}
//----------------------------------------------------------------------
jfSearchResultItemCollectionBase* jfResultCollection::GetCurrPointer() {
  // checks
  if (itemcount==0) return NULL;
  assert(cindex<itemcount);
  // getting the values
  return (*collections)[cindex];
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extracting information from a particular item
QString jfResultCollection::GetItemDescription(size_t c_index, size_t i_index) const {
  // checking the collections...
  assert(c_index<itemcount);
  (*cprotect)[c_index]->lock();
  // getting the string
  QString xres = (*collections)[c_index]->SummaryAtIndex(i_index);
  (*cprotect)[c_index]->unlock();
  // done
  return xres;
}
//--------------------------------------------------------------------------------
// extracting information from a particular item
QString jfResultCollection::GetItemUrl(size_t c_index, size_t i_index) const {
  // checking the collections...
  assert(c_index<itemcount);
  (*cprotect)[c_index]->lock();
  // getting the string
  QString xres = (*collections)[c_index]->UrlAtIndex(i_index);
  (*cprotect)[c_index]->unlock();
  // done
  return xres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfResultCollection::~jfResultCollection() {
  DestroyAll();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper methods
//-----------------------------------------------------------------------------
// checking an index
bool jfResultCollection::CheckIndex(size_t index) const {
  if (itemcount==0) return false;
  if (index>=itemcount) return false;
  if (collections==NULL) return false;
  if (cprotect==NULL) return false;
  if (index>=(collections->size())) return false;
  if (index>=(cprotect->size())) return false;
  if ((*collections)[index]==NULL) return false;
  return true;
}
//-------------------------------------------------------------------------------
bool jfResultCollection::DestroyAll() {
  size_t loopc;
  if (itemcount>0) {
    for (loopc=0;loopc<itemcount;loopc++) {
      delete (*collections)[loopc];
      delete (*cprotect)[loopc];
      (*collections)[loopc] = NULL;
      (*cprotect)[loopc] = NULL;
    }
    delete collections;
    delete cprotect;
    collections = NULL;
    cprotect = NULL;
    itemcount = 0;
    cindex = 0;
  }
  if (idstore!=NULL) {
    delete idstore;
    idstore = NULL;
  }
  return true;
}
//-------------------------------------------------------------------------------
bool jfResultCollection::AddItem(jfSearchResultItemCollectionBase* newitem) {
  const QString fname = "jfResultCollection::AddItem";
  assert (newitem!=NULL);
  // if we are empty, we add the vectors
  if (itemcount==0) {
    collections = new jfItemVector();
    cprotect = new jfMutexVector();
  }
  // adding
  itemcount++;
  collections->push_back(newitem);
  cprotect->push_back(new QMutex);
  // done
  return true;
}
//------------------------------------------------------------------------------
// inserts an item at the current pointer
bool jfResultCollection::InsertItem(jfSearchResultItemCollectionBase* newitem) {
  // checks for special conditions
  if (newitem==NULL) return false;
  // variables
  jfItemVector::iterator main_insert;
  jfMutexVector::iterator lock_insert;
  /* special case 1 and 2, the collection is empty or the index is path the
   end. We call Add Item.  */
  if ((itemcount==0) || (cindex==itemcount)) {
    bool aires = AddItem(newitem);
    assert(aires);
  }
  // general case, insertion
  else {
    // making the insertion point
    main_insert = (collections->begin())+cindex;
    lock_insert = (cprotect->begin())+cindex;
    // inserting
    collections->insert(main_insert,newitem);
    cprotect->insert(lock_insert,new QMutex);
    itemcount++;
  }
  // we update the index to point the the item after
  cindex++;
  // done
  return true;
}
//------------------------------------------------------------------------------
bool jfResultCollection::DeleteAtIndex(size_t dindex) {
  // checks
  if (dindex>=itemcount) return false;
  // variables
  jfItemVector::iterator c_index;
  jfMutexVector::iterator m_index;
  bool dlast;
  // starting...
  dlast = (dindex==(itemcount-1));
  c_index = (collections->begin()) + dindex;
  m_index = (cprotect->begin()) + dindex;
  collections->erase(c_index);
  cprotect->erase(m_index);
  itemcount--;
  if (dlast) cindex--;
  else if (cindex>dindex) cindex--;
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal file i/o methods
// creates/reopens a file using a name for a specific result category
jfHtmlParams* jfResultCollection::OpenHTMLOutfile(size_t index) {
  // local variables
  jfHtmlParams* result = NULL;
  QString path;
  // some basic checks
  assert(index<33);
  assert(search!=NULL);
  // category checks
  assert((search->categories)!=NULL);
  if (index==32) assert((search->categories)->UsesDefault());
  else assert(index<((search->categories)->GetCount()));
  // now that the checks are over, we get the path an location for the output file
  path = (search->categories)->GetFullPath(index);
  // creating the output object
  result = new jfHtmlParams();
  // creating the file
  result->outfile = GetOpenFile(path,true);
  // filling in some additional data
  result->SetResCat(index);
  result->parse = new jfSkeletonParser();
  // done
  return result;
}
//----------------------------------------------------------------------------
void jfResultCollection::WriteFooterClose(jfHtmlParams* indata) {
  // local variables
  QString buffer;
  QDateTime xnow;
  // some checks
  assert(indata!=NULL);
  assert(indata->CheckOk());
  // loading some time and date data
  xnow = QDateTime::currentDateTime();
  indata->parse->AddText("TIME",xnow.toString("hh:mm"));
  indata->parse->AddText("DATE",xnow.toString("MMM d','yyyy"));
  // next up, we produce the output from the skeleton
  buffer = indata->MakeResult(indata->base->footer_skel);
  indata->AppendLine(buffer);
  // done, we write and close
  CloseFileStream(indata->outfile,true);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfResultCollection::AddMiddleToFile(QTextStream* outfile) const {
    // local variables
  QString result;
  size_t windex;
  // checking and special conditions
  if (outfile==NULL) return false;
  // we convert the count first
  result = QString::number(itemcount);
  (*outfile) << result << "\n";
  // writing the rest
  for (windex=0;windex<itemcount;windex++) {
    (*collections)[windex]->AddToFile(outfile);
  }
  // done
  return true;
}
//-------------------------------------------------------------------------
bool jfResultCollection::ReadMiddleFromFile(jfFileReader* infile) {
  // constants
  const QString fname = "jfResultCollection::ReadMiddleFromFile";
  // local variables
  jfSearchResultItemCollectionBase* tcoll;
  unsigned long oval;
  size_t iloop;
  // starting
  /**/JDEBUGLOG(fname,1)
  assert(infile!=NULL);
  // we read in the item count
  if (!infile->ReadULong(oval,fname)) return false;
  DestroyAll();
  itemcount = 0;
  /**/JDEBUGLOG(fname,2)
  // the collection reading loop
  for (iloop=0;iloop<oval;iloop++) {
    /**/JDEBUGLOGST(fname,3,iloop)
    tcoll = MakeEmptyCollection();
    if (!tcoll->GetFromFile(infile)) {
        jerror::Log(fname,infile->err);
      delete tcoll;
      return false;
    }
    /**/JDEBUGLOGST(fname,5,iloop)
    AddItem(tcoll);
  }
  /**/JDEBUGLOG(fname,6)
  // done
  return true;
}
//*****************************************************************************
