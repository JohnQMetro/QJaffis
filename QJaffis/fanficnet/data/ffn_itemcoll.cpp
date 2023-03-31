/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_itemcoll.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     June 1, 2010
// Conversion to Qt Started July 11, 2014
// Updated:     March 26, 2023
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
jfFFNItemCollection::jfFFNItemCollection():jfSearchResultsCollection<jfFFNItem>("FFN Item Collection",1) {
    first_id = 0;
    // cat link
    cat_link = NULL;
}
//-------------------------------------------------------------------------------
jfFFNItemCollection::jfFFNItemCollection(const QString& name, size_t num_id):jfSearchResultsCollection<jfFFNItem>(name, num_id) {
    first_id = 0;
    cat_link = NULL;
}
//-------------------------------------------------------------------------------
// the usual constructor
jfFFNItemCollection::jfFFNItemCollection(const jfFFN_CategoryCore* cat_linkin)
    :jfSearchResultsCollection<jfFFNItem>("FFN Item Collection for " + cat_linkin->GetName(), cat_linkin->GetID()) {

    first_id = 0;
    // category link
    cat_link = cat_linkin;
    caturl = cat_link->GetUrl();
    cfinder = cat_link->MakeFinder();
}
//----------------------------------------------------------------------------
// initializes data using a new result collection
jfFFNItemCollection::jfFFNItemCollection(jfFFNItemCollection* insrc, bool copy)
    :jfSearchResultsCollection<jfFFNItem>(insrc->name, (insrc->num_id) + 1) {
    // variables
    size_t eloop;
    jfItemFlagGroup tmp_res;
    // start the copying
    size_t source_size = (insrc->mainlist).size();
    mainlist.reserve(source_size);
    first_id = insrc->first_id;

    // the main loop. copy signals whether we copy or link the elements
    for (eloop=0 ; eloop < source_size ; eloop++) {
        if (copy) {
            tmp_res = CopyGroup((insrc->mainlist).at(eloop));
        }
        else tmp_res = (insrc->mainlist)[eloop];
        mainlist.push_back(tmp_res);
    }
    // assigning some base values
    num_id = insrc->num_id;
    name = insrc->name;

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
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented page methods
//-----------------------------------------------------
QString jfFFNItemCollection::TypeId() const {
    return "FFNItemCollection";
}
//-----------------------------------------------------
bool jfFFNItemCollection::LoadValues(jfSkeletonParser* inparser,size_t which) const {
    LoadCoreValues(inparser,which);
    inparser->AddText("CATF_MAINLINK",cat_link->GetUrl());
    inparser->AddText("CATF_PERCENT",PercentResultCategory(which));
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
    for (lindex = 0; lindex < lmax; lindex++) {
        const jfFFNItem* const_fic = dynamic_cast<const jfFFNItem*>(mainlist[lindex].item);
        jfFFNItem* mutable_fic = const_cast<jfFFNItem*>(const_fic);
        mutable_fic->SetCatLink(cat_link);
    }
    // done
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting more information
jfFicExtract* jfFFNItemCollection::FicExtractAtIndex(size_t i_index) const {
  if (i_index >= mainlist.size()) return NULL;
  else {
      return dynamic_cast<const jfFFNItem*>(mainlist.at(i_index).item)->GetExtract();
  }
}
//------------------------------------------------------------------

jfFFNItemCollection::~jfFFNItemCollection() {
  ClearContents(true);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfItemFlagGroup jfFFNItemCollection::CopyGroup(const jfItemFlagGroup& source) {
    const jfFFNItem* source_fic = dynamic_cast<const jfFFNItem*>(source.item);
    const jfFFNItem* fic_copy = new jfFFNItem(*source_fic);
    jfItemFlagGroup result;
    result.item = fic_copy;
    if (source.flags == NULL) result.flags = NULL;
    else result.flags = new jfItemMetaFlags(*(source.flags));
    return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more virtual i/o methods
//-----------------------------------------------------
bool jfFFNItemCollection::ReadItemFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const {
    jfFFNItem* result = new jfFFNItem();
    if (result->GetFromFile(infile)) {
        target_group.item = result;
        result->SetCatLink(cat_link);
        return true;
    }
    else {
        delete result;
        return false;
    }
}
// ----------------------------------------------------
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





