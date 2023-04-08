/******************************************************************************
// Name:        typedcollect.h
// Author :     John Q Metro
// Purpose :    Declares a templated vector wrapper for descendants of
//              jfBasePD
// Created:     May 7, 2009
// Conversion to Qt Started November 9, 2013
// Updated:     March 11, 2023 (Complete overhaul)
******************************************************************************/
#ifndef TYPEDCOLLECT_H_INCLUDED
#define TYPEDCOLLECT_H_INCLUDED
#endif // TYPEDCOLLECT_H_INCLUDED
//-----------------------------------------------------------------------------
#ifndef ITEMCOLLECT_H_INCLUDED
  #include "itemcollect.h"
#endif // ITEMCOLLECT_H_INCLUDED
#include <vector>
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include "../objs/baseitem.h"
//----------------------------------
#include <assert.h>
/*****************************************************************************/

template <class Q>
class jfSearchResultsCollection : public jfSearchResultItemCollectionBase {
  public:
    // constructors
    jfSearchResultsCollection(QString&& in_name, size_t in_num_id);
    jfSearchResultsCollection(const QString& in_name, size_t in_num_id);
    // Id filtering
    // adding
    size_t Append(Q* in_item, jfItemMetaFlags* in_flags);
    bool InsertNewResults(jfSearchResultsCollection<Q>* insrc);
  protected:
};
// ========================================================================
// constructor

template <class Q>
jfSearchResultsCollection< Q >::jfSearchResultsCollection(QString&& in_name, size_t in_num_id):jfSearchResultItemCollectionBase(in_name, in_num_id) {

}
// ------------------------------
template <class Q>
jfSearchResultsCollection< Q >::jfSearchResultsCollection(const QString& in_name, size_t in_num_id):jfSearchResultItemCollectionBase(in_name,in_num_id) {

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// appending an item
template <class Q>
size_t jfSearchResultsCollection< Q >::Append(Q* in_item, jfItemMetaFlags* in_flags) {
  assert(in_item !=NULL);
  mainlist.push_back(jfItemFlagGroup{ in_item, in_flags } );
  return mainlist.size();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// inserting a list, usually for updating (FFN)
template <class Q>
bool jfSearchResultsCollection< Q >::InsertNewResults(jfSearchResultsCollection<Q>* insrc) {
    // checks
    assert(insrc!=NULL);
    // variables
    i_const_iterator this_start, that_start, that_end;
    // we start with getting the iterators
    this_start = mainlist.begin();
    that_start = (insrc->mainlist).begin();
    that_end = (insrc->mainlist).end();
    // we can now insert in a single op
    mainlist.insert(this_start,that_start,that_end);
    // we next update some counts
    grand_total += insrc->grand_total;
    insert_index = insrc->insert_index;
    // updating the indexes
    /*
    if (item_index>=0) item_index += insrc->item_count;
    if (backup_index>=0) backup_index += insrc->item_count;
    */
    //other values are either temp values, or stuff that should be the same
    // (category and search data links), we will not check though

    //finally, we get rid of the old data, since it will not do to have
    // two collections pointing to the same items
    insrc->ClearContents(false);
    // done
    return true;
}

// ================================================================================

/*
class jfTypedCollection : public jfUrlItemCollection {
  public:
    typedef typename std::vector<Q*>::const_iterator q_const_iterator;
	// constructor
    jfTypedCollection();
    jfTypedCollection(size_t src_id, const QString& nname);
  // extra methods
    size_t Append(Q* initem);
    const Q* GetItem(const size_t& dindex) const;
    const Q* GetCurrentC() const;
    Q* GetCurrent();
  // virtual methods, now defined here
    virtual bool AddResults(jfResultUnitVector* new_items);
    virtual bool NextIndex();
    virtual bool BackIndex();
    virtual void ResetIndex();
    virtual bool ToIndex(const int& newindex);
    virtual size_t CountRCategory(size_t which) const;
    virtual QString PercRCategory(size_t which) const;
    // extracting info at the specified index
    virtual QString NameAtIndex(size_t i_index) const;
    virtual QString DescriptionAtIndex(size_t i_index) const;
    virtual QString UrlAtIndex(size_t i_index) const;
    // some more abstract methods
    virtual QStringList* GetNameList() const;
    virtual QStringList* GetDescriptionList() const;
    virtual QStringList* GetTypeList() const;
    virtual bool DeleteAtIndex(const size_t& dindex);
    // clearing out stuff (for destruction!)
    void ClearContents(bool destroy_too);
  protected:
    // appends page_results onto maindata
    size_t AppendPageResults();
    //  implemented virtual i/o method
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    virtual bool ToHTMLList(jfHtmlParams* indata, bool single) const;

    // the data
    std::vector<Q*> mainlist;
};
//==============================================================================
// constructor
template <class Q>
jfTypedCollection< Q >::jfTypedCollection():jfUrlItemCollection() {}
template <class Q>
jfTypedCollection< Q >::jfTypedCollection(size_t src_id, const QString& nname):
  jfUrlItemCollection(src_id,nname) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// appending an item
template <class Q>
size_t jfTypedCollection< Q >::Append(Q* initem) {
  assert(initem!=NULL);
  mainlist.push_back(initem);
  item_count++;
  return item_count;
}
//-----------------------------------------------------------------------------
// getting a const item
template <class Q>
const Q* jfTypedCollection< Q >::GetItem(const size_t& dindex) const {
  // we first do the usual checks
  assert(dindex>=0);
  assert(dindex<item_count);
  return mainlist[dindex];
}
//-----------------------------------------------------------------------------
// getting a const item at the current index
template <class Q>
const Q* jfTypedCollection< Q >::GetCurrentC() const {
  if (curr_item==NULL) return NULL;
  else return dynamic_cast<const Q*>(curr_item);
}
//-----------------------------------------------------------------------------
// getting a modifiable item at the current index
template <class Q>
Q* jfTypedCollection< Q >::GetCurrent() {
  if (curr_item==NULL) return NULL;
  else return dynamic_cast<Q*>(curr_item);
}
//===============================================================================
// virtual methods, now defined here
//-----------------------------------------------------------------------------
// adding results
template <class Q>
bool jfTypedCollection< Q >::AddResults(jfResultUnitVector* new_items) {
  if (new_items == NULL) return false;
  jfBasePD* non_const_item;
  Q* temp_item;
  // adding to the grand total
  grand_total += new_items->stotal;
  // getting links to the items from the new_items list
  for (size_t cur_index = 0 ; cur_index < new_items->size() ; cur_index++) {
    non_const_item = const_cast<jfBasePD*>((*new_items)[cur_index]->Sourcelink());
    temp_item = dynamic_cast<Q*>(non_const_item);
    Append(temp_item);
  }
  // done
  insert_index++;
  return true;
}
//--------------------------------------------------------------------
// incrementing the index
template <class Q>
bool jfTypedCollection< Q >::NextIndex() {
  // already off the end
  if (item_index==-2) return false;
  // going off the end
  else if (item_index==(item_count-1)) {
    item_index = -2;
    curr_item = NULL;
    return false;
  }
  // going forward
  else {
    item_index++;
    curr_item = mainlist[item_index];
    return true;
  }
}
//-----------------------------------------------------------------------------
// decrementing the index
template <class Q>
bool jfTypedCollection< Q >::BackIndex() {
  // already before the start
  if (item_index==-1) return false;
  // going off the start
  if (item_index==0) {
    item_index = -1;
    curr_item = NULL;
    return false;
  }
  // moving onto the end
  if (item_index==-2) item_index = (item_count-1);
  else item_index--;
  if (item_index!=-1) {
    curr_item = mainlist[item_index];
    return true;
  }
  else return false;
}
//------------------------------------------------------------------------------
// sets the index to -1
template <class Q>
void jfTypedCollection< Q >::ResetIndex() {
  item_index = -1;
  curr_item = NULL;
}
//------------------------------------------------------------------------------
// we move to a particular index
template <class Q>
bool jfTypedCollection< Q >::ToIndex(const int& newindex) {
  if (newindex<-2) return false;
  if (newindex>=item_count) return false;
  item_index = newindex;
  curr_item = mainlist[item_index];
  return true;
}
*/
//-------------------------------------------------------------------------------
/* counts how many items are marked as belonging to a particular result
category. 32 counts as no category at all */
/*
template <class Q>
size_t jfTypedCollection< Q >::CountRCategory(size_t which) const {
  assert(which<33);
  // variables
  size_t rcount = 0;
  size_t lcount;
  // the main loop
  for (lcount=0;lcount<item_count;lcount++) {
    if (which==32) {
      if (!(mainlist[lcount]->mark).any()) rcount++;
    }
    else {
      if ((mainlist[lcount]->mark).test(which)) rcount++;
    }
  }
  // done
  return rcount;
}
//----------------------------------------------------------------------------
// outputs the percentage of the total fetched that beglong to a particular result category
template <class Q>
QString jfTypedCollection< Q >::PercRCategory(size_t which) const {
  // variables
  size_t rtotal;
  QString resval;
  double pval;
  // we start
  assert(which<33);
  if (grand_total==0) resval = "0.00%";
  else {
    rtotal = CountRCategory(which);
    pval = (double(rtotal))/(double(grand_total));
    resval = ToPercent(pval,2);
  }
  // done
  return resval;
}
//===============================================================================
// extracting info at the specified index
//------------------------------------------------------------------------------
template <class Q>
QString jfTypedCollection< Q >::NameAtIndex(size_t i_index) const {
  if (i_index>=item_count) return "";
  else return mainlist[i_index]->GetName();
}
//------------------------------------------------------------------------------
template <class Q>
QString jfTypedCollection< Q >::DescriptionAtIndex(size_t i_index) const {
  if (i_index>=item_count) return "";
  else return mainlist[i_index]->GetDescription();
}
//------------------------------------------------------------------------------
template <class Q>
QString jfTypedCollection< Q >::UrlAtIndex(size_t i_index) const {
  if (i_index>=item_count) return "";
  else return mainlist[i_index]->GetUrl();
}
//===============================================================================
// some more abstract methods
//-----------------------------------------------------------------------------
// return a lsit of all the names
template <class Q>
QStringList* jfTypedCollection< Q >::GetNameList() const {
  // the variables
  size_t loopc,lmax;
  QStringList* result;
  // we start the preparation
  lmax = item_count-1;
  if (item_count==0) return NULL;
  result = new QStringList();
  // the loop
  for (loopc=0;loopc<lmax;loopc++) {
    result->append(mainlist[loopc]->GetName());
  }
  // done
  return result;
}
//-----------------------------------------------------------------------------
template <class Q>
QStringList* jfTypedCollection< Q >::GetDescriptionList() const {
  // the variables
  size_t loopc,lmax;
  QStringList* result;
  // we start the preparation
  lmax = item_count-1;
  if (item_count==0) return NULL;
  result = new QStringList();
  // the loop
  for (loopc=0;loopc<lmax;loopc++) {
    result->append(mainlist[loopc]->GetDescription());
  }
  // done
  return result;
}
//-----------------------------------------------------------------------------
template <class Q>
QStringList* jfTypedCollection< Q >::GetTypeList() const {
  // the variables
  size_t loopc,lmax;
  QStringList* result;
  // we start the preparation
  lmax = item_count-1;
  if (item_count==0) return NULL;
  result = new QStringList();
  // the loop
  for (loopc=0;loopc<lmax;loopc++) {
    result->append(mainlist[loopc]->GetTypeID());
  }
  // done
  return result;
}
*/
//-----------------------------------------------------------------------------
/* when deleting at an index, we keep the stored indexes the same unless the
stored index points towards the last item, in which case we move it to the
previosu index. The actual items will change, though */
/*
template <class Q>
bool jfTypedCollection< Q >::DeleteAtIndex(const size_t& dindex) {
  // data values
  size_t nlast;
  // first, we do the usual checks
  if (dindex<0) return false;
  if (dindex>=item_count) return false;
  // we delete the item
  delete mainlist[dindex];
  mainlist[dindex] = NULL;
  // we we shorten the list
  mainlist.erase(mainlist.begin()+dindex);
  nlast = item_count-2;
  // a special case... no item left!
  if (item_count==1) {
    item_count=0;
    item_index = -1;
    curr_item = NULL;
    backup_index = -1;
    backup_item = NULL;
    return true;
  }
  // next, we redo the indexes
  // if the index is the last, we leave it as the last
  if (item_index==(item_count-1)) {
    item_index = nlast;
    curr_item = mainlist[nlast];
  }
  if (backup_index==(item_count-1)) {
    backup_index = nlast;
    backup_item = mainlist[nlast];
  }
  // otherwise, we keep the actual index the same, but change the ite,
  if (item_index>=0) curr_item = mainlist[item_index];
  if (backup_index>=0) backup_item = mainlist[item_index];
  // finishing off
  item_count--;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// clearing out stuff (for destruction!)
template <class Q>
void jfTypedCollection< Q >::ClearContents(bool destroy_too) {
  // variables
  size_t rloop;
  // if we destroy, we do that first
  if (destroy_too) {
    for (rloop=0;rloop<item_count;rloop++) {
      delete mainlist[rloop];
      mainlist[rloop] = NULL;
    }
  }
  // getting rid of the list
  mainlist.clear();
  //getting rid of the indexes
  item_index = -1;
  curr_item = NULL;
  backup_index = -1;
  backup_item = NULL;
  // resetting the counts
  item_count = 0;
  grand_total = 0;
  insert_index = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  implemented virtual i/o method
//-----------------------------------------------------------------------
template <class Q>
bool jfTypedCollection< Q >::AddRestToFile(QTextStream* outfile) const {
  size_t loopc;
  // the usual checks
  if (outfile==NULL) return false;
  // next, we call a method that must be implemented by any non-abstract descendant
  if (!AddDelta(outfile)) return false;
  // writing the results loop
  for (loopc=0;loopc<item_count;loopc++) {
    if (!(mainlist[loopc]->AddToFile(outfile))) return false;
  }
  // done
  return true;
}
//---------------------------------------------------------------------------
template <class Q>
bool jfTypedCollection< Q >::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfTypedCollection< Q >::ReadRestFromFile";
  // local variables
  size_t loopc;
  Q* initem;
  // starting
  if (!ReadDelta(infile)) return false;
  // item count is already read, we assume the line index is just before the first item
  for (loopc=0;loopc<item_count;loopc++) {
    // we prepare to read the item
    initem = new Q();
    // reading the item
    if (!initem->GetFromFile(infile)) {
      delete initem;
      return false;
    }
    // adding the item to our list
    mainlist.push_back(initem);
  }
  // done
  return true;
}
//----------------------------------------------------------------------------
*/
/* iterates over the items, producing a QString contatining an HTML representation
suitable for an HTML file */
/*
template <class Q>

bool jfTypedCollection< Q >::ToHTMLList(jfHtmlParams* indata, bool single) const {
  // local variables
  QString result;
  q_const_iterator findex;
  bool use_it;
  size_t numresults;
  QString tvalue;
  // we start
  findex = mainlist.begin();
  numresults = 0;
  // the main loop
  while (findex<mainlist.end()) {
    if ((*findex)->ignore) use_it = false;
    else {
        // we check if we use the current item
        if (indata->GetResCat()==32) {
            use_it = (!(*findex)->mark.any());
        }
        else {
            use_it = (*findex)->mark.test(indata->GetResCat());
        }
    }
    // we only do the next if use_it is true
    if (use_it) {
      // we add a separator first (since there is no way to tell if there are any following items.
      if (numresults!=0) {
        tvalue = (single)?(indata->Single()->item_separator):(indata->Multiple()->item_separator);
        result = indata->MakeResult(tvalue);
        indata->AppendLine(result);
      }
      // getting and writing the html data for the item
      (*findex)->LoadValues(indata->parse);
      tvalue = (single)?(indata->Single()->item_skel):(indata->Multiple()->item_skel);
      result = indata->MakeResult(tvalue);
      indata->AppendLine(result);
      // index increments
      numresults++;
    }
    findex++;
  }
  // done
  return true;
}
*/
/*****************************************************************************/
