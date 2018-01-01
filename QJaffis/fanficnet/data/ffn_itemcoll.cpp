/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_itemcoll.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     June 1, 2010
// Conversion to Qt Started July 11, 2014
// Updated:     August 15, 2016
/////////////////////////////////////////////////////////////////////////////
#ifndef FFN_ITEMCOLL
  #include "ffn_itemcoll.h"
#endif
//----------------------------------------------------
#ifndef JF_FFNSEARCH
  #include "ffn_search.h"
#endif
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-----------------------------------------

//***************************************************************************
// alternate constructor for use when loading data from file
jfFFNItemCollection::jfFFNItemCollection():jfTypedCollection<jfFFNItem>() {
  first_id = 0;
  // cat link
  cat_link = NULL;
}
//-------------------------------------------------------------------------------
// the usual constructor
jfFFNItemCollection::jfFFNItemCollection(const jfFFN_CategoryCore* cat_linkin):jfTypedCollection<jfFFNItem>() {
  // constants
  const QString fname = "jfFFNItemCollection::jfFFNItemCollection";
  // checks
  /**/jfAssertLog(cat_linkin!=NULL,fname,"The input category link is NULL!");
  first_id = 0;
  // category link
  cat_link = cat_linkin;
  caturl = cat_link->GetUrl();
  cfinder = cat_link->MakeFinder();
}
//----------------------------------------------------------------------------
// initializes data using a new result collection
jfFFNItemCollection::jfFFNItemCollection(jfFFNItemCollection* insrc, bool copy):jfTypedCollection<jfFFNItem>() {
  // variables
  size_t eloop;
  jfFFNItem* tmp_src;
  jfFFNItem* tmp_res;
  // start the copying
  mainlist.reserve(insrc->item_count);
  first_id = insrc->first_id;
  item_count = insrc->item_count;
  // the main loop. copy signals whether we copy or link the elements
  for (eloop=0;eloop<item_count;eloop++) {
    if (copy) {
      tmp_src = (insrc->mainlist)[eloop];
      tmp_res = new jfFFNItem(*tmp_src);
    }
    else tmp_res = (insrc->mainlist)[eloop];
    mainlist.push_back(tmp_res);
  }
  // assigning some base values
  num_id = insrc->num_id;
  name = insrc->name;
  description = insrc->description;
  validdata = insrc->validdata;
  // we skip the indexes, leaving them default
  // more collection oriented stuff
  grand_total = insrc->grand_total;
  insert_index = insrc->insert_index;
  // stuff that is fanfiction.net specific
  cat_link = insrc->cat_link;
  caturl = insrc->caturl;
  cfinder = insrc->cfinder;
  // finally, if we are not copying, we should empty the source
  if (!copy) {
    insrc->ClearContents(false);
    // finally
    insrc->validdata = false;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented page methods
//-----------------------------------------------------
QString jfFFNItemCollection::GetTypeID() const {
  return "FFNItemCollection";
}
//-----------------------------------------------------
bool jfFFNItemCollection::LoadValues(jfSkeletonParser* inparser,size_t which) const {
  LoadCoreValues(inparser,which);
  inparser->AddText("CATF_MAINLINK",cat_link->GetUrl());
  inparser->AddText("CATF_PERCENT",PercRCategory(which));
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// category manipulation methods
//-----------------------------------------------------
QString jfFFNItemCollection::GetCatUrl() const {
  return cat_link->GetUrl();
}
//-----------------------------------------------------
QString jfFFNItemCollection::GetFinder() const {
  return cfinder;
}
//----------------------------------------------------------------
const jfFFN_CategoryCore* jfFFNItemCollection::GetCat() const {
  return cat_link;
}
//-----------------------------------------------------------
QString jfFFNItemCollection::GetCatName() const {
  if (cat_link==NULL) return "";
  else return cat_link->GetName();
}
//-----------------------------------------------------------
bool jfFFNItemCollection::IsSameCat(const jfFFN_CategoryCore* test_cat) {
  // trivial bad input/situation
  if (test_cat==NULL) return false;
  if (cat_link==NULL) return false;
  // basic testing
  jf_FFCTYPE ctype = cat_link->GetCatType();
  if (ctype != (test_cat->GetCatType())) return false;
  if ((cat_link->GetID()) != (test_cat->GetID())) return false;
  // testing for crossovers
  if ( ctype == jfx_CROSS ){
    const jfFFN_CrossoverCategory* cross_cat_link = dynamic_cast<const jfFFN_CrossoverCategory*>(cat_link);
    const jfFFN_CrossoverCategory* cross_test_cat = dynamic_cast<const jfFFN_CrossoverCategory*>(test_cat);
    return ((cross_cat_link->GetSecondID()) == (cross_test_cat->GetSecondID()));
  }
  else return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNItemCollection::ReplaceCat(const jfFFN_CategoryCore* cat_linkin) {
  // constants
  const QString fname = "jfFFNItemCollection::ReplaceCat";
  // local variables
  size_t lindex, lmax;
  // a check
  assert(cat_linkin!=NULL);
  // replacing the link for the collection
  cat_link = cat_linkin;
  caturl = cat_link->GetUrl();
  cfinder = cat_link->MakeFinder();
  // next, doing it for all of the items in maindata
  lmax = mainlist.size();
  for (lindex = 0; lindex<lmax; lindex++) {
    mainlist[lindex]->SetCatLink(cat_link);
  }
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting more information
jfFicExtract* jfFFNItemCollection::FicExt_AtIndex(size_t i_index) const {
  if (i_index>=item_count) return NULL;
  else return dynamic_cast<jfFFNItemCore*>(mainlist[i_index])->GetExtract();
}
//------------------------------------------------------------------
/* takes the results from insrc and inserts them into this collections, with the
new elements going at the start of the list */
bool jfFFNItemCollection::InsertNewResults(jfFFNItemCollection* insrc) {
  // checks
  assert(insrc!=NULL);
  // variables
  std::vector<jfFFNItem*>::iterator this_start, that_start, that_end;
  // we start with getting the iterators
  this_start = mainlist.begin();
  that_start = (insrc->mainlist).begin();
  that_end = (insrc->mainlist).end();
  // we can now insert in a single op
  mainlist.insert(this_start,that_start,that_end);
  // we next update some counts
  item_count = mainlist.size();
  grand_total += insrc->grand_total;
  insert_index = insrc->insert_index;
  // updating the indexes
  if (item_index>=0) item_index += insrc->item_count;
  if (backup_index>=0) backup_index += insrc->item_count;
  //other values are either temp values, or stuff that should be the same
  // (category and search data links), we will not check though

  //finally, we get rid of the old data, since it will not do to have
  // two collections pointing to the same items
  insrc->ClearContents(false);
  insrc->validdata = false;
  // done
  return true;
}
//------------------------------------------------------
jfResultUnitVector* jfFFNItemCollection::GetContentsAsResults() {
  // variables
  jfFFNItem* curritem;
  // starting
  jfResultUnitVector* result = new jfResultUnitVector();
  // loop that builds up result
  for (size_t rindex = 0 ; rindex < mainlist.size() ; rindex++) {
    curritem = dynamic_cast<jfFFNItem*>(mainlist[rindex]);
    result->push_back(curritem->makeResultUnit());
  }
  result->stotal = grand_total;
  return result;
}
//------------------------------------------------------
jfFFNItemCollection::~jfFFNItemCollection() {
  ClearContents(true);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more virtual i/o methods
//-----------------------------------------------------
bool jfFFNItemCollection::AddDelta(QTextStream* outfile) const {
  // variables
  QString hiid_str;
  // the usual checks
  if (outfile==NULL) return false;
  // the url must be used after loading to find the category object
  (*outfile) << caturl << "\n";
  (*outfile) << cfinder << "\n";
  hiid_str = QString::number(first_id);
  (*outfile) << hiid_str << "\n";
  // done
  return true;
}
//-----------------------------------------------------
bool jfFFNItemCollection::ReadDelta(jfFileReader* infile) {
  const QString funcname = "jfFFNItemCollection::ReadDelta";
  // local variables
  unsigned long qval;
  // starting with line 4
  assert(infile!=NULL);
  if (!infile->ReadLine(caturl,funcname)) return false;
  if (!infile->ReadLine(cfinder,funcname)) return false;
  if (!infile->ReadULong(qval,funcname)) return false;
  first_id = qval;
  return true;
}
//***************************************************************************





