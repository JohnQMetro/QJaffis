/***************************************************************************
 * Name    : crossover_groups.cpp
 * Author  : John Q Metro
 * Purpose : Indirect indexes of FFN crossover categories.
 * Created : May 29, 2014
 * Updated : December 9, 2018
 **************************************************************************/
#ifndef CROSSOVER_GROUPS_H
  #include "crossover_groups.h"
#endif // CROSSOVER_GROUPS_H
//------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#include <assert.h>
/**************************************************************************/
bool jfFFN_CrossCatOtherMap::ContainsTag(const QString& intag) const {
  return (count(intag)!=0);
}
//---------------------------------------------------------------
bool jfFFN_CrossCatOtherMap::AddCheck(size_t this_id, jfFFN_CrossoverCategory* toadd) {
  const QString fname = "jfFFN_CrossCatOtherMap::AddCheck";
  if (toadd==NULL) return false;
  if (!toadd->ContainsID(this_id)) return false;
  QString xtag = toadd->GetOtherCombo(this_id);
  if (count(xtag)!=0) return false;
  (*this)[xtag] = toadd;
  return true;
}
//--------------------------------------------------------------
bool jfFFN_CrossCatOtherMap::UnmarkAndRemove(const QString& ftag, size_t which) {
  if (which>=32) return false;
  if (count(ftag)==0) return false;
  jfFFN_CrossoverCategory* xf = (*this)[ftag];
  (xf->mark).set(which,false);
  erase(ftag);
  return true;
}
//--------------------------------------------------------------
size_t jfFFN_CrossCatOtherMap::AppendToList(jfFFN_CatList& inlist) {
  if (size()==0) return 0;
  jfFFN_CrossCatOtherMap::iterator idx = begin();
  // looping
  while (idx!=end()) {
    inlist.push_back(idx->second);
    idx++;
  }
  return size();
}
//==========================================================================
// constructor...
jfFFN_CrossoverGroup::jfFFN_CrossoverGroup():jfBaseObj() {
  unsorted_links = NULL;
  cat_store = NULL;
  before_first = true;
  sorted = false;
}
//----------------------------------------------------
jfFFN_CrossoverGroup::jfFFN_CrossoverGroup(QString snamein, jfFFN_CrossCatStore* incatstore):jfBaseObj() {
  assert(incatstore!=NULL);
  cat_store = incatstore;
  secname = snamein;
  unsorted_links = NULL;
  before_first = true;
  sorted = false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
QString jfFFN_CrossoverGroup::GetTypeID() const {
  return "FFN_CrossoverGroup";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting info
//------------------------
bool jfFFN_CrossoverGroup::IsSorted() const {
  return sorted;
}
//------------------------
QStringList* jfFFN_CrossoverGroup::GetKeyNames() const {
  QStringList* result = NULL;
  if ((!sorted) && (unsorted_links!=NULL)) {
    if (unsorted_links->size()!=0) {
      result = new QStringList();
      for (size_t vindex=0;vindex<(unsorted_links->size());vindex++) {
        result->append((*unsorted_links)[vindex]->GetOtherCombo(num_id));
      }
    }
  }
  else {
    if (sorted_links.size()>0) {
      result = new QStringList();
      jfFFN_CrossCatOtherMap::const_iterator qx = sorted_links.begin();
      while (qx!=sorted_links.end()) {
        result->append(qx->first);
        qx++;
      }
    }
  }
  return result;
}
//-------------------------
QString jfFFN_CrossoverGroup::GetCatName() const {
  return catname;
}
//-------------------------
size_t jfFFN_CrossoverGroup::GetCount() const {
  if (sorted) return sorted_links.size();
  else if (unsorted_links==NULL) return 0;
  else return unsorted_links->size();
}
//-------------------------
bool jfFFN_CrossoverGroup::ContainsKey(const QString& sec1, const QString& cat1) const {
  const QString fname = "jfFFN_CrossoverGroup::ContainsKey";
  if (!sorted) return false;
  QString xkey = cat1 + " (" + sec1 + ")";
  bool qval = sorted_links.ContainsTag(xkey);
  return qval;
}
//------------------------------------------------
int jfFFN_CrossoverGroup::AppendSection(const QString sec_name, jfStrBoolList* destination) const {
  // default inapplicable cases
  if (!sorted) return -1;
  if (destination==NULL) return -1;
  if (sec_name.isEmpty()) return -1;
  // variables
  jfFFN_CrossCatOtherMap::const_iterator xi;
  QString curr_sec;
  bool match_sec;
  size_t res_count;
  // starting
  res_count = 0;
  if (sorted_links.size()==0) return 0;
  xi = sorted_links.begin();
  // the main loop
  while (xi!=sorted_links.end()) {
    curr_sec = xi->second->GetOtherSection(num_id);
    match_sec = (sec_name==curr_sec);
    if (match_sec) {
      destination->AddItem(xi->first,true);
      res_count++;
    }
    xi++;
  }
  // done
  return res_count;
}
//---------------------------------------------------------
const jfFFN_CrossoverCategory* jfFFN_CrossoverGroup::GetCatPointer(const QString& sec1, const QString& cat1) const {
  // default inapplicable cases
  if (!sorted) return NULL;
  if (sec1.isEmpty()) return NULL;
  if (cat1.isEmpty()) return NULL;
  // starting ...
  QString xkey = cat1 + " (" + sec1 + ")";
  jfFFN_CrossCatOtherMap::const_iterator twi;
  // doing
  if (!sorted_links.ContainsTag(xkey)) return NULL;
  else {
    twi = sorted_links.find(xkey);
    return twi->second;
   }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// marking
//--------------------------
bool jfFFN_CrossoverGroup::MarkAndAddByCombo(jfFFN_CrossCatOtherMap& mark_cgroup, const QString& incombo, size_t which) {
  // special cases
  if (which>=32) return false;
  if (!sorted) return false;
  if (!sorted_links.ContainsTag(incombo)) return false;
  // getting the crossover cat, and it's mark status
  jfFFN_CrossoverCategory* xam = sorted_links[incombo];
  bool mk = (xam->mark).test(which);
  // case already marked
  if (mk) return false;
  // case not marked
  else {
    (xam->mark).set(which,true);
    mark_cgroup.AddCheck(num_id,xam);
    return true;
  }
}
//--------------------------
size_t jfFFN_CrossoverGroup::MarkAndAddAll(jfFFN_CrossCatOtherMap& mark_cgroup, size_t which) {
  const QString fname = "jfFFN_CrossoverGroup::MarkAndAddAll";
  // special cases
  if (which>=32) return false;
  if (!sorted) return false;
  // loop variables
  jfFFN_CrossCatOtherMap::iterator lptr;
  jfFFN_CrossoverCategory* xam;
  size_t acount; bool mk;
  // starting
  lptr = sorted_links.begin();
  acount = 0;
  // the loop
  while (lptr!=sorted_links.end()) {
    // getting and testing the category
    xam = lptr->second;
    lptr++;
    mk = (xam->mark).test(which);
    if (mk) continue;
    // the category tests false, we change to true and add
    (xam->mark).set(which,true);
    mk = mark_cgroup.AddCheck(num_id,xam);
    assert(mk);
    acount++;
  }
  // done
  return acount;
}
//--------------------------
bool jfFFN_CrossoverGroup::MarkAndAddCurrent(jfFFN_CrossCatOtherMap& mark_cgroup, size_t which) {
  // special cases
  if (which>=32) return false;
  if (!sorted) return false;
  if (before_first) return false;
  if (cptr==sorted_links.end()) return false;
  // variables
  jfFFN_CrossoverCategory* xam;
  bool mk;
  // getting and checking
  xam = cptr->second;
  if (xam==NULL) return false;
  mk = (xam->mark).test(which);
  // finishing based on the check
  if (mk) return false;
  else {
    (xam->mark).set(which,true);
    mk = mark_cgroup.AddCheck(num_id,xam);
    return mk; // should be always true
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// altering data
//------------------------
void jfFFN_CrossoverGroup::SetSectionName(QString insection) {
  secname = insection;
}
//------------------------
void jfFFN_CrossoverGroup::SetCatName(const QString& incname) {
  catname = incname;
  name = incname + " Crossovers";
}
//------------------------
/* Transfers category links from the vector to the map. Since the map uses section names for the
 * other category, this should only be called after *ALL* crossover pages have been processed to
 * guarantee that that SetSectionByID had been called for all IDs, and section info is complete. */
bool jfFFN_CrossoverGroup::SortLinks() {
  const QString fname = "jfFFN_CrossoverGroup::SortLinks";
  bool rval;
  jfFFN_CrossoverCategory* currcat;
  // we do not sort twice
  /**/JDEBUGLOGS(fname,1,catname);
  if (sorted) return true;
  // also, we do not sort nothing
  if (unsorted_links==NULL) return false;
  if (unsorted_links->size()==0) return false;
  // more checks
  if (!validdata) return false;
  /**/JDEBUGLOG(fname,2);
  sorted_links.clear();
  // finally, we do the sorting via inserting into the empty map
  for (size_t sx = 0; sx < (unsorted_links->size());sx++) {
    currcat = (*unsorted_links)[sx];
    rval = sorted_links.AddCheck(num_id,currcat);
    assert(rval);
  }
  /**/JDEBUGLOG(fname,3);
  // getting rid of the old stuff
  delete unsorted_links;
  unsorted_links = NULL;
  sorted = true;
  return true;
}

//------------------------
void jfFFN_CrossoverGroup::ClearContents() {
  if (sorted) {
    sorted_links.clear();
    sorted = false;
  }
  if (unsorted_links!=NULL) {
    delete unsorted_links;
    unsorted_links = NULL;
  }
  num_id = 0;
  validdata = false;
  before_first = true;
}
//-------------------------
bool jfFFN_CrossoverGroup::AddCategoryWhenUnsorted(jfFFN_CrossoverCategory* incat) {
  if (sorted) return false;
  if (incat == NULL) return false;
  if (unsorted_links==NULL) unsorted_links = new std::vector<jfFFN_CrossoverCategory*>();
  unsorted_links->push_back(incat);
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++
// indexing (sorted only)
//----------------------------
void jfFFN_CrossoverGroup::ResetIndex() {
  before_first = true;
}
//----------------------------
bool jfFFN_CrossoverGroup::NextIndex() {
  if (!sorted) return false;
  if (before_first) {
    cptr = sorted_links.begin();
    before_first = false;
  }
  else if (cptr==sorted_links.end()) return false;
  else cptr++;
  return (cptr!=sorted_links.end());
}
//----------------------------
QString jfFFN_CrossoverGroup::CurrentName() const {
  if ((!sorted) || before_first) return "";
  if (cptr==sorted_links.end()) return "";
  // general case
  return (cptr->second)->GetName();
}
//+++++++++++++++++++++++++++++++++++++++++++
/* Full Copy (sorted only). Used when copying old results to new results, so
 * we have to copy all the individual jfFFN_CrossoverCategory objects and
 * insert them into the catstore for the new result, making sure the links
 * are working got the new group */
bool jfFFN_CrossoverGroup::GetFullIntegratedCopy(jfFFN_CrossoverGroup*& out_group, jfFFN_CrossCatStore* new_catstore) const {
  // quick invalid checks
  if (new_catstore==NULL) return false;
  if (!sorted) return false;
  // variables
  jfFFN_CrossoverCategory* old_tptr, *copy_tptr;
  jfFFN_CrossCatOtherMap::const_iterator old_ptr;
  bool added_val;
  // starting
  out_group = new jfFFN_CrossoverGroup();
  // copying jfBaseObj stuff
  out_group->num_id = num_id;
  out_group->validdata = validdata;
  out_group->name = name;
  out_group->description = description;
  // copying basic internal data
  out_group->secname = secname;
  out_group->catname = catname;
  out_group->tcatc = tcatc; // only correct when read from a file?
  // setting some defaults
  out_group->sorted = true;
  out_group->unsorted_links = NULL;
  out_group->cat_store = new_catstore;
  // now, time to copy the categories!
  old_ptr = sorted_links.begin();
  while (old_ptr!=sorted_links.end()) {
    // copying the actual object...
    old_tptr = old_ptr->second;
    copy_tptr = new jfFFN_CrossoverCategory(*old_tptr);
    // storing
    old_tptr = new_catstore->AddUpdateCategory(copy_tptr,added_val);
    if (!added_val) delete copy_tptr;
    (out_group->sorted_links).AddCheck(num_id,old_tptr);
    old_ptr++;
  }
  // finishing touches
  out_group->before_first = true;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++
jfFFN_CrossoverGroup::~jfFFN_CrossoverGroup() {
  ClearContents();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//-------------------------
bool jfFFN_CrossoverGroup::AddMiddleToFile(QTextStream* outfile) const {
  const QString fname = "jfFFN_CrossoverGroup::AddMiddleToFile";
  jfOutString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  if (!sorted) return false;
  // section name, catname, count
  result << secname << catname << sorted_links.size();
  (*outfile) << QString(result) << "\n";
  return true;
}
//-------------------------
bool jfFFN_CrossoverGroup::AddRestToFile(QTextStream* outfile) const {
  jfOutString result;
  jfFFN_CrossCatOtherMap::const_iterator qx;
  // checking and special conditions
  if (outfile==NULL) return false;
  if (!sorted) return false;
  // writing the output in the form of the id's of the other categories
  qx = sorted_links.begin();
  while (qx!=sorted_links.end()) {
    result << (qx->second)->GetOtherID(num_id);
    qx++;
  }
  (*outfile) << QString(result) << "\n";
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o input
//-------------------------
bool jfFFN_CrossoverGroup::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_CrossoverGroup::ReadMiddleFromFile";
  // input data
  QString cname;
  // reading the line
  assert(infile!=NULL);
  if (!infile->ReadParseLine(3,funcname)) return false;
  // field 0 is the section name
  secname = infile->lp.UnEscStr(0);
  // field 1 is the category name
  catname = infile->lp.UnEscStr(1);
  // field 2 is the category count
  if (!infile->lp.SIntVal(2,tcatc)) return infile->BuildError("Category count is bad!");
  return true;
}
//-------------------------
bool jfFFN_CrossoverGroup::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_CrossoverGroup::ReadRestFromFile";
  // input data
  size_t tcat_idx, rid;
  jfFFN_CrossoverCategory* crc;
  bool ires;
  // reading the line
  assert(infile!=NULL);
  if (!infile->ReadParseLine(tcatc,funcname)) return false;
  for (tcat_idx = 0; tcat_idx<tcatc; tcat_idx++) {
    // looking for each category
    if (!infile->lp.SIntVal(tcat_idx,rid)) return infile->BuildError("Could Not read ID at : " + QString::number(tcat_idx));
    crc = cat_store->GetByIDs(num_id,rid);
    if (crc==NULL) return infile->BuildError("Crossover ID combo : " + QString::number(num_id) +
                                          " " + QString::number(rid)+ ", had not been found!");
    // inserting in the list
    ires = sorted_links.AddCheck(num_id,crc);
    if (!ires) return infile->BuildError("Propblem with inserting crossover category, duplicate?");
  }
  // done here, with no errors, we set some values
  sorted = true;
  unsorted_links = NULL;
  return true;
}
//*************************************************************************
bool jfFFN_CrossSectionMap::AddHCross(jfFFN_HalfCrossover* incat) {
  jf_HCC_Pair insertee;
  QString ccatname;
  if (incat==NULL) return false;
  ccatname = incat->GetName();
  if (count(ccatname)!=0) return false;
  insertee.catlink = incat;
  insertee.grouplink = NULL;
  insertee.markmap = NULL;
  (*this)[ccatname] = insertee;
  return true;
}

//============================================================================
// constructor...
jfFFN_CrossoverSection::jfFFN_CrossoverSection():jfBaseObj() {
  before_first = true;
  index = crossgroups.end();
  nullmode = true;
  before_first_url = true;
}
//----------------------------------------------------
jfFFN_CrossoverSection::jfFFN_CrossoverSection(jfFFN_CrossCatStore* incat_store):jfBaseObj() {
  cat_store = incat_store;
  before_first = true;
  index = crossgroups.end();
  nullmode = true;
  before_first_url = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
QString jfFFN_CrossoverSection::GetTypeID() const {
  return "FFN_CrossoverSection";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// indexes
//-------------------------
void jfFFN_CrossoverSection::ResetIndex() {
  before_first = true;
  index = crossgroups.end();
}
//-------------------------
bool jfFFN_CrossoverSection::NextIndex() {
  if (crossgroups.size()==0) return false;
  if (before_first) {
    index = crossgroups.begin();
    before_first = false;
  }
  else index++;
  return (index!=crossgroups.end());
}
//------------------------
size_t jfFFN_CrossoverSection::CatCount() const {
  return crossgroups.size();
}
//-------------------------
size_t jfFFN_CrossoverSection::UCatCount() const {
  return uccount;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// url indexes
//----------------------------
void jfFFN_CrossoverSection::ResetUrlIndex() {
  before_first_url= true;
}

//----------------------------
bool jfFFN_CrossoverSection::NextUrlIndex() {
  if (before_first_url) {
    url_index = crossgroups.begin();
    before_first_url = false;
    return (url_index != crossgroups.end());
  }
  else if (url_index == crossgroups.end()) return false;
  else {
    url_index++;
    return (url_index != crossgroups.end());
  }
}
//----------------------------
bool jfFFN_CrossoverSection::NextUrlIndex(bool& isupdatable) {
  bool xres = NextUrlIndex();
  if (xres) isupdatable = UpdatableAtUrlIndex();
  else isupdatable = false;
  return xres;
}
//----------------------------
bool jfFFN_CrossoverSection::UpdatableAtUrlIndex() const {
  if (before_first_url) return false;
  if (url_index == crossgroups.end()) return false;
  return (url_index->second).catlink->doupdate;
}
//----------------------------
QString* jfFFN_CrossoverSection::DownURLAtIndex() const {
  const QString fname = "jfFFN_CrossoverSection::DownURLAtIndex";
  if (before_first_url) return NULL;
  if (url_index == crossgroups.end()) return NULL;
  QString xres = (url_index->second).catlink->GetUrl();
  xres = CrossLinkModify(xres);
  return new QString(xres);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFN_CrossoverSection::IndexLink(bool next, bool plink) const {
  // plink looks like this
  // https://www.fanfiction.net/Ouran-High-School-Host-Club-Crossovers/2480/0/
  // variables
  QString result;
  jfFFN_CrossSectionMap::iterator zogdex;
  // cases where we return nothing
  if (before_first) return "";
  if (index==crossgroups.end()) return "";
  // setting up the pointer
  zogdex = index;
  if (next) {
    zogdex++;
    if (zogdex==crossgroups.end()) return "";
  }
  // starting
  result = (zogdex->second).catlink->GetUrl();
  // Return a link to the crossover category index
  if (!plink) result = CrossLinkModify(result);
  return result;
}
//------------------------------------
int jfFFN_CrossoverSection::AppendSection(const QString& subcat, const QString& sec_name, jfStrBoolList* destination) const {
  // faliure conditions
  if (nullmode) return -1;
  if (subcat.isEmpty()) return -1;
  if (crossgroups.count(subcat)==0) return -1;
  // finding the category
  jfFFN_CrossSectionMap::const_iterator xi = crossgroups.find(subcat);
  return (xi->second).grouplink->AppendSection(sec_name,destination);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// altering data
//----------------------------------------------------
bool jfFFN_CrossoverSection::AddHCrossCategory(jfFFN_HalfCrossover* input) {
  if (!nullmode) return false;
  if (input==NULL) return false;
  bool ares = crossgroups.AddHCross(input);
  if (!ares) return false;
  uccount = tcount = crossgroups.size();
  return true;
}
//-------------------------------------------------
bool jfFFN_CrossoverSection::InsertGroupAtIndex(jfFFN_CrossoverGroup* in_group) {
  nullmode = false;
  // the usual checks for bad things
  if (in_group==NULL) return false;
  if (before_first) return false;
  if (index==crossgroups.end()) return false;
  //deleting any previous group
  if ((index->second.grouplink)!=NULL) {
    delete (index->second).grouplink;
    index->second.grouplink = NULL;
  }
  // inserting
  (index->second).grouplink = in_group;
  return true;
}
//------------------------------------------------
bool jfFFN_CrossoverSection::SortAll() {
  // constants and variables
  const QString fname = "jfFFN_CrossoverSection::SortAll";
  if (nullmode) return false;
  bool tx,isempty;
  // starting
  /**/JDEBUGLOGST(fname,1,crossgroups.size());
  ResetIndex();
  while (NextIndex()) {
    isempty = (index->second.grouplink) == NULL;
    /**/JDEBUGLOGB(fname,2,(index->second.grouplink)==NULL);
    while (EmptyAtIndex()) {
      /**/JDEBUGLOG(fname,3)
      if (!RemoveAtIndex()) return true;
    }
    tx = index->second.grouplink->SortLinks();
    /**/JDEBUGLOGB(fname,4,tx);
    if (!tx) return false;
  }
  return true;
}
//----------------------------------------------------
bool jfFFN_CrossoverSection::SetNoUpdateAtIndex() {
  // out of bounds
  if (index==crossgroups.end()) return false;
  if (before_first) return false;
  if ((index->second).catlink==NULL) return false;
  // setting
  ((index->second).catlink)->doupdate = false;
  return true;
}
//------------------------------------------------------
bool jfFFN_CrossoverSection::SetUpdatesByComparing(const jfFFN_CrossoverSection* old_data) {
  // quick checks
  if (old_data == NULL) return false;
  if (old_data == this) return false;
  // variables
  int old_esc, new_esc;
  size_t nuc;
  QString new_name;
  // preparing to loop
  ResetIndex();
  nuc = 0;
  // the checking and un-updating loop
  while (NextIndex()) {
    new_name = NameAtIndex(false);
    new_esc = old_data->EstStoryCount(new_name);
    if (new_esc!=-1) {
      old_esc = StoryCountAtIndex();
      if ((old_esc==new_esc) && (old_esc<50)) {
        SetNoUpdateAtIndex();
      }
    }
  }
  // finally
  uccount = crossgroups.size() - nuc;
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFN_CrossoverSection::ContainsCat(const QString& cat1, const QString& sec2, const QString& cat2) const {
  const QString fname = "jfFFN_CrossoverSection::ContainsCat";
  /**/JDEBUGLOGS(fname,1,cat1);
  if (nullmode) return false;
  /**/JDEBUGLOGS(fname,2,sec2);
  if (crossgroups.count(cat1)==0) return false;
  /**/JDEBUGLOGS(fname,3,cat2);
  jfFFN_CrossSectionMap::const_iterator xi = crossgroups.find(cat1);
  /**/JDEBUGLOGB(fname,4,xi!=crossgroups.end());
  return (xi->second).grouplink->ContainsKey(sec2,cat2);
}
//---------------------------------------------------
QStringList* jfFFN_CrossoverSection::GetCrossList(const QString& catx) const {
  if (nullmode) return NULL;
  jfFFN_CrossSectionMap::const_iterator xi = crossgroups.find(catx);
  return (xi->second).grouplink->GetKeyNames();
}
//---------------------------------------------------
QStringList* jfFFN_CrossoverSection::GetCatNames() const {
  // variables
  jfFFN_CrossSectionMap::const_iterator xi;
  QStringList* result;
  // starting
  result = new QStringList();
  xi = crossgroups.begin();
  // the loop
  while (xi!=crossgroups.end()) {
    result->append(xi->first);
    xi++;
  }
  return result;
}
//---------------------------------------------------
QString jfFFN_CrossoverSection::NameAtIndex(bool addcross) const {
  if (index==crossgroups.end()) return "";
  if (before_first) return "";
  QString bname = ((index->second).catlink)->GetName();
  if (addcross) bname += " Crossovers";
  return bname;
}
//---------------------------------------------------
int jfFFN_CrossoverSection::StoryCountAtIndex() const {
  if (index==crossgroups.end()) return -1;
  if (before_first) return -1;
  return ((index->second).catlink)->GetEstStoryCount();
}
//---------------------------------------------------
int jfFFN_CrossoverSection::EstStoryCount(const QString& hcross_name) const {
  if (crossgroups.size()==0)  return -1;
  if (crossgroups.count(hcross_name)==0) return -1;
  jfFFN_CrossSectionMap::const_iterator twi;
  twi = crossgroups.find(hcross_name);
  return (twi->second).catlink->GetEstStoryCount();
}
//---------------------------------------------------
bool jfFFN_CrossoverSection::GetDoUpdateAtIndex() const {
    if (index==crossgroups.end()) return false;
    if (before_first) return false;
    return ((index->second).catlink)->doupdate;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// getting crossover pointers
//-----------------------------------------
const jfFFN_CrossoverCategory* jfFFN_CrossoverSection::GetCrossoverCatPointer(QString cat1, QString sec2, QString cat2) const {
  if (nullmode) return NULL;
  if (crossgroups.count(cat1)==0) return NULL;
  else {
    jfFFN_CrossSectionMap::const_iterator twi;
    twi = crossgroups.find(cat1);
    return (twi->second).grouplink->GetCatPointer(sec2,cat2);
  }
}
//-----------------------------------------
const jfFFN_HalfCrossover* jfFFN_CrossoverSection::GetCrossoverIndex(const QString& cat1) const {
  if (crossgroups.count(cat1)==0) return NULL;
  else {
    jfFFN_CrossSectionMap::const_iterator twi;
    twi = crossgroups.find(cat1);
    return (twi->second).catlink;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// marking
//-------------------
size_t jfFFN_CrossoverSection::ApplyOrAtIndex(const jfSimpleExpr* or_exp, size_t which, bool& okay) {
  const QString fname = "jfFFN_CrossoverSection::ApplyOrAtIndex";
  // bad cases
  okay = false;
  if (nullmode) return 0;
  if (which>=32) return 0;
  if (before_first) return 0;
  if (index==crossgroups.end()) return 0;
  if (!(index->second).grouplink->IsSorted()) return 0;
  // simple non-bad zero result cases
  okay = true;
  if (or_exp->isEmpty()) return 0;
  if ((index->second).grouplink->GetCount()==0) return 0;
  // variables for the general case
  bool xm, zm;
  size_t mcount;
  QString current_name;
  jfFFN_CrossCatOtherMap* mmap;
  jfFFN_CrossoverGroup* xl;
  // loop preparation
  if ((index->second).markmap == NULL ) {
    (index->second).markmap = new jfFFN_CrossCatOtherMap();
  }
  mmap = (index->second).markmap;
  xl = (index->second).grouplink;
  xl->ResetIndex();
  mcount = 0;
  // looping
  while (xl->NextIndex()) {
    current_name = xl->CurrentName();
    xm = or_exp->InternalMatch(current_name);
    if (xm) {
      zm = xl->MarkAndAddCurrent(*mmap,which);
      if (zm) mcount++;
    }
  }
  // final stuff
  if (mmap->size()==0) {
    delete mmap;
    (index->second).markmap = NULL;
  }
  return mcount;
}

//-------------------
size_t jfFFN_CrossoverSection::ApplyAndAtIndex(const jfSimpleExpr* and_exp, size_t which, bool& okay) {
  const QString fname = "jfFFN_CrossoverSection::ApplyAndAtIndex";
  // bad cases
  okay = false;
  if (nullmode) return 0;
  if (which>=32) return 0;
  if (before_first) return 0;
  if (index==crossgroups.end()) return 0;
  // simple non-bad zero result cases
  okay = true;
  if (and_exp->isEmpty()) return 0;
  // variables for the general case
  jfFFN_CrossCatOtherMap* mmap;
  jfFFN_CrossCatOtherMap::iterator idx;
  size_t mcount, kcount;  bool xm,zm;
  QString current_name, ctag;
  size_t kindex;
  // starting
  mmap = (index->second).markmap;
  if (mmap==NULL) return 0;
  if (mmap->size()==0) return 0;
  // we loop over the mark map
  idx = mmap->begin();
  mcount = 0;
  kcount = 0;
  while (idx!=mmap->end()) {
    ctag = idx->first;
    current_name = idx->second->GetName();
    xm = and_exp->InternalMatch(current_name);
    if (!xm) {
      zm = mmap->UnmarkAndRemove(ctag,which);
      assert(zm);
      mcount++;
      if (mmap->size()==0) break;
      idx = mmap->begin();
      for (kindex=0;kindex<kcount;kindex++) idx++;
    }
    else {
      kcount++;
      idx++;
    }
  }
  // final stuff
  if (mmap->size()==0) {
    delete mmap;
    (index->second).markmap = NULL;
  }
  return mcount;
}
//---------------------------------
bool jfFFN_CrossoverSection::MarkCombo(const QString& fcatname, const QString& markcombo, size_t which) {
  // quick falses
  if (which>=32) return false;
  if (nullmode) return false;
  if (crossgroups.count(fcatname)==0) return false;
  if (markcombo.isEmpty()) return false;
  // finding
  jfFFN_CrossoverGroup* grouplink = crossgroups[fcatname].grouplink;
  if (crossgroups[fcatname].markmap==NULL) crossgroups[fcatname].markmap = new jfFFN_CrossCatOtherMap();
  bool zval = grouplink->MarkAndAddByCombo(*crossgroups[fcatname].markmap,markcombo,which);
  return zval;
}
//---------------------------------
bool jfFFN_CrossoverSection::MarkSpecificCrossover(const QString& fcatname, const QString& ssecname,
                  const QString& scatname, size_t which) {
  // quick falses
  if (ssecname.isEmpty()) return false;
  if (scatname.isEmpty()) return false;
  // setting up the combo
  QString combo_tag = scatname + " (" + ssecname + ")";
  return MarkCombo(fcatname,combo_tag,which);
}
//---------------------------------
bool jfFFN_CrossoverSection::MarkAllCategory(const QString& fcatname, size_t which) {
  size_t mcount;
  // quick falses
  if (which>=32) return false;
  if (nullmode) return false;
  if (crossgroups.count(fcatname)==0) return false;
  jfFFN_CrossoverGroup* grouplink = crossgroups[fcatname].grouplink;
  if (crossgroups[fcatname].markmap==NULL) crossgroups[fcatname].markmap = new jfFFN_CrossCatOtherMap();
  mcount = grouplink->MarkAndAddAll(*crossgroups[fcatname].markmap,which);
  return (mcount>0);
}
//---------------------------------
bool jfFFN_CrossoverSection::MarkEverything(size_t which) {
  const QString fname = "jfFFN_CrossoverSection::MarkEverything";
  // quick falses
  if (which>=32) return false;
  if (nullmode) return false;
  /**/JDEBUGLOG(fname,1);
  // looping variables
  size_t mcount;
  jfFFN_CrossSectionMap:: iterator idx;
  // starting
  idx = crossgroups.begin();
  // the main loop
  while (idx!=crossgroups.end()) {
    if (((idx->second).markmap)==NULL) (idx->second).markmap = new jfFFN_CrossCatOtherMap();
    mcount = ((idx->second).grouplink)->MarkAndAddAll(*((idx->second).markmap),which);
    idx++;
  }
  // done
  return true;
}

//---------------------------------
bool jfFFN_CrossoverSection::ApplyExpressionMarking(const jfSimpleExpr* or_exp, const jfSimpleExpr* and_exp, size_t which) {
  const QString fname = "jfFFN_CrossoverSection::ApplyExpressionMarking";
  // simple bad cases
  if (which>=32) return false;
  if (nullmode) return false;
  // variables
  bool isokay; size_t rcount;
  // looping...
  /**/JDEBUGLOG(fname,1);
  ResetIndex();
  while (NextIndex()) {
    if (and_exp!=NULL) {
      rcount = ApplyAndAtIndex(and_exp,which,isokay);
      if (!isokay) return false;
    }
    if (or_exp!=NULL) {
      rcount = ApplyOrAtIndex(or_exp,which,isokay);
      if (!isokay) return false;
    }
  }
  return true;
}

//---------------------------------
bool jfFFN_CrossoverSection::AddToCategoryList(jfFFN_CatList& inlist) {
  if (nullmode) return false;
  jfFFN_CrossCatOtherMap* mmap;
  ResetIndex();
  while (NextIndex()) {
    mmap = (index->second).markmap;
    if (mmap!=NULL) {
      mmap->AppendToList(inlist);
      delete mmap;
      (index->second).markmap = NULL;
    }
  }
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// an update special
bool jfFFN_CrossoverSection::GetIntegratedCopyByName(const QString& oldname, jfFFN_CrossoverGroup*& out_group, jfFFN_CrossCatStore* new_catstore) const {
  // the usual checks for bad things
  if (oldname.isEmpty()) return false;
  if (new_catstore==NULL) return false;
  if (crossgroups.size()==0)  return false;
  if (crossgroups.count(oldname)==0) return false;
  // getting a pointer to the right group
  jfFFN_CrossSectionMap::const_iterator twi;
  twi = crossgroups.find(oldname);
  // some more checks
  if ((twi->second).grouplink==NULL) return false;
  if (!((twi->second).grouplink->IsSorted())) return false;
  // finally, doing it!
  return ((twi->second).grouplink)->GetFullIntegratedCopy(out_group,new_catstore);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFFN_CrossoverSection::~jfFFN_CrossoverSection() {
  ResetIndex();
  while (NextIndex()) {
    if ((index->second.grouplink)!=NULL) {
      delete (index->second).grouplink;
      index->second.grouplink = NULL;
    }
    if ((index->second.catlink)!=NULL) {
      delete (index->second).catlink;
      index->second.catlink = NULL;
    }
    if ((index->second.markmap)!=NULL) {
      delete (index->second).markmap;
      index->second.markmap = NULL;
    }
  }
  crossgroups.clear();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFN_CrossoverSection::CrossLinkModify(QString srclink) const {
  // variables
  int slen = QString("https://www.fanfiction.net/").length();
  int fpos;
  QString buffer1,buffer2;
  // we extract critical data and rebuild the url
  // getting link name
  fpos = srclink.indexOf("-Crossovers/",slen);
  assert(fpos>=0);
  buffer1 = srclink.mid(slen,fpos-slen);
  // link id
  slen = fpos+12;
  fpos = srclink.indexOf("/",slen);
  assert(fpos>=0);
  buffer2 = srclink.mid(slen,fpos-slen);
  // resuilding
  srclink = "https://www.fanfiction.net/crossovers/" + buffer1;
  return (srclink + "/" + buffer2 + "/");
}
//----------------------------------------------------
// used to help in discarding empty crossovers
bool jfFFN_CrossoverSection::EmptyAtIndex() const {
    assert(!before_first);
    assert(index != crossgroups.end());
    if ((index->second.grouplink) == NULL) return true;
    if (((index->second.grouplink)->GetCount()) == 0) return true;
    else return false;
}
//-----------------------------------------------------
bool jfFFN_CrossoverSection::RemoveAtIndex() {
    if (before_first) return false;
    if (index == crossgroups.end()) return false;
    jfFFN_CrossSectionMap::iterator rem_index = index;
    bool res_next = NextIndex();
    crossgroups.erase(rem_index);
    return res_next;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//--------------------------
bool jfFFN_CrossoverSection::AddMiddleToFile(QTextStream* outfile) const {
  jfOutString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // writing the count first
  result << crossgroups.size() << nullmode;
  (*outfile) << QString(result) << "\n";
  return true;
}
//--------------------------
bool jfFFN_CrossoverSection::AddRestToFile(QTextStream* outfile) const {
  // write variables
  jfFFN_CrossSectionMap::const_iterator widx;
  bool resx;
  // checking
  if (outfile==NULL) return false;
  // writing the half crossovers
  if (crossgroups.size()>0) {
    for (widx=crossgroups.begin();widx!=crossgroups.end();widx++) {
      resx = (widx->second).catlink->AddToFile(outfile);
      if (!resx) return false;
    }
  }
  // next up, the crossover groups (if we need them)
  if (!nullmode) {
    for (widx=crossgroups.begin();widx!=crossgroups.end();widx++) {
      resx = (widx->second).grouplink->AddToFile(outfile);
      if (!resx) return false;
    }
  }
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o input
//--------------------------
bool jfFFN_CrossoverSection::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_CrossoverSection::ReadMiddleFromFile";
  // result << crossgroups.size() << nullmode;
  // reading the line
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,funcname)) return false;
  // field 0 is the count
  if (!infile->lp.SIntVal(0,tcount)) return infile->BuildError("Group count is bad!");
  // field 1 is whether the group links are null, or should we read in the groups
  if (!infile->lp.BoolVal(1,nullmode)) return infile->BuildError("Nullmode setting could not be read!");
  // done
  return true;
}

//--------------------------
bool jfFFN_CrossoverSection::ReadRestFromFile(jfFileReader* infile) {
  // constants
  const QString funcname = "jfFFN_CrossoverSection::ReadRestFromFile";
  // variables
  size_t tidx;
  QString rname, rlinkfrag;
  bool xres;
  jfFFN_CrossSectionMap::iterator ridx;
  jfFFN_CrossoverGroup* cgroup;
  jfFFN_HalfCrossover* hcross;
  // starting
  assert(infile!=NULL);
  // reading the half crossover categories

  for (tidx=0;tidx<tcount;tidx++) {
    hcross = new jfFFN_HalfCrossover();
    xres = hcross->GetFromFile(infile);
    if (!xres) return false;
    xres = crossgroups.AddHCross(hcross);
    if (!xres) return infile->BuildError("Could not insert crossover listing!");
  }
  // now, if needed, we read in the category groups
  if ((!nullmode) && (tcount!=0)) {
    ridx = crossgroups.begin();
    for (tidx=0;tidx<tcount;tidx++) {
      // creating and reading the category group
      cgroup = new jfFFN_CrossoverGroup(name,cat_store);
      xres = cgroup->GetFromFile(infile);
      if (!xres) return false;
      // cheking if the names match (just in case)
      if ((ridx->first)!=(cgroup->GetCatName())) {
        rname = "The names " + ridx->first + " and " + cgroup->GetCatName();
        rname += " are supposed to match!";
        delete cgroup;
        return infile->BuildError(rname);
      }
      // otherwise, we stick it in
      (ridx->second).grouplink = cgroup;
      ridx++;
    }
  }
  uccount = tcount;
  // done
  ResetIndex();
  return true;
}

/**************************************************************************/
