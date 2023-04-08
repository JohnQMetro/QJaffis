/***************************************************************************
 * Name    : catgroup.cpp
 * Author  : John Q Metro
 * Purpose : Top level FFN category classes
 * Created : June 23, 2014
 * Updated : July 31, 2015
 **************************************************************************/
#ifndef CATGROUP_H
  #include "catgroup.h"
#endif // CATGROUP_H
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../../core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
//----------------------------------
#include <assert.h>
#include <QMessageBox>
/**************************************************************************/
const QString jfFFN_SECDATA::section_names[jfFFN_SECDATA::scount] = {
      "Anime/Manga","Books","Cartoons","Comics","Games","Misc",
      "Movies","Plays/Musicals","TV Shows"};
//---------------------------------------------------
const QString jfFFN_SECDATA::crossover_names[jfFFN_SECDATA::scount] = {
  "Anime/Manga Crossovers","Book Crossovers","Cartoon Crossovers",
  "Comic Book Crossovers","Game Crossovers","Misc Crossovers",
  "Movie Crossovers","Crossovers with Plays/Musicals","TV Show Crossovers"};
//---------------------------------------------------
const QString jfFFN_SECDATA::section_strings[jfFFN_SECDATA::scount] = {
      "anime","book","cartoon","comic","game","misc","movie","play","tv"};
//---------------------------------------------------
int jfFFN_SECDATA::SectionName2Index(const QString& inname) {
  int resval = -1;
  size_t sidx;
  for (sidx = 0; sidx<scount; sidx++) {
    if (section_names[sidx]==inname) break;
  }
  if (sidx<scount) resval = sidx;
  return resval;
}
//------------------------------------------------------
QString jfFFN_SECDATA::IndexToURL(size_t index, bool cross) {
  // bad case
  if (index>=scount) return "";
  // constants and varibales
  const QString ffn_str = "https://www.fanfiction.net/";
  QString reval;
  // building the result
  reval = ffn_str;
  if (cross) reval += "crossovers/";
  reval += section_strings[index] + "/";
  return reval;
}

//==================================================================
// constructor
jfFFN_Categories::jfFFN_Categories():jfBaseObj() {
  dataset = false;
  old_data = NULL;
  cross_store = NULL;
  for (size_t idx = 0; idx < jfFFN_SECDATA::scount; idx++) {
    sections[idx] = NULL;
    cross_sections[idx] = NULL;
    csecmap[idx] = NULL;
  }
  phase = 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// checking crossovers for contents
//------------------------------------
bool jfFFN_Categories::ContainsCat(const QString& sec1, const QString& cat1, const QString& sec2, const QString& cat2) const {
  const QString fname = "jfFFN_Categories::ContainsCat";
  if (!dataset) return false;
  int sidxx = jfFFN_SECDATA::SectionName2Index(sec1);
  if (sidxx<0) return false;
  return cross_sections[sidxx]->ContainsCat(cat1,sec2,cat2);
}
//------------------------------------
int jfFFN_Categories::AppendSection(const QString& sec1, const QString& cat1, const QString& sec2, jfStrBoolList* destination) const {
  if (!dataset) return false;
  int sidxx = jfFFN_SECDATA::SectionName2Index(sec1);
  if (sidxx<0) return false;
  return cross_sections[sidxx]->AppendSection(cat1,sec2,destination);
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// Getting Lists of Names
//------------------------------------
QStringList* jfFFN_Categories::GetCategories(size_t sec_idx) const {
  if (sec_idx>=jfFFN_SECDATA::scount) return NULL;
  if (sections[sec_idx]==NULL) return NULL;
  return (sections[sec_idx]->GetNameList());
}
//------------------------------------
QStringList* jfFFN_Categories::GetCrossoverCategories(size_t sec_idx, const QString& cross_name) const {
  if (sec_idx>=jfFFN_SECDATA::scount) return NULL;
  if (cross_sections[sec_idx]==NULL) return NULL;
  return (cross_sections[sec_idx])->GetCrossList(cross_name);
}
//------------------------------------
QStringList* jfFFN_Categories::GetCrossSectionList(size_t sec_idx) const {
  if (sec_idx>=jfFFN_SECDATA::scount) return NULL;
  if (cross_sections[sec_idx]==NULL) return NULL;
  return (cross_sections[sec_idx])->GetCatNames();
}
//----------------------------------
// Adding downloaded data
//----------------------------------
bool jfFFN_Categories::SetSection(jfFFNSection* new_section) {
  if (new_section == NULL) return false;
  if (phase != 1) return false;
  if (sections[sidx] !=NULL) delete sections[sidx];
  sections[sidx] = new_section;
  return true;
}
//----------------------------------
bool jfFFN_Categories::SetCrossoverSection(jfFFN_CrossoverSection* new_cross_section) {
  if (new_cross_section == NULL) return false;
  if (phase != 2) return false;
  if (cross_sections[sidx] !=NULL) delete cross_sections[sidx];
  cross_sections[sidx] = new_cross_section;
  return true;
}
//----------------------------------
bool jfFFN_Categories::SetCrossoverGroup(jfFFN_CrossoverGroup* new_group) {
  if (phase != 3) return false;
  return cross_sections[sidx]->InsertGroupAtIndex(new_group);
}

//++++++++++++++++++++++++++++++++++++++++++++++++
// indexing
//----------------
void jfFFN_Categories::ResetIndex() {
  phase = 0;
}
//----------------
bool jfFFN_Categories::NextIndex() {
  const QString fname = "jfFFN_Categories::NextIndex";
  if (phase==0) {
    phase = 1;
    sidx = 0;
  }
  // in phase 1 (sections)
  else if (phase==1) {
    if (sidx<jfFFN_SECDATA::scountm) sidx++;
    else {
      phase = 2;
      sidx = 0;
    }
    /**/JDEBUGLOGST(fname,3,sidx);
  }
  // phase 2 (top-level crossover sections)
  else if (phase==2) {
    if (sidx<jfFFN_SECDATA::scountm) sidx++;
    else {
      phase = 3;
      sidx = 0;
      ccount = cross_sections[sidx]->CatCount();
      cross_sections[sidx]->ResetIndex();
      cross_sections[sidx]->NextIndex();
      assert(ccount!=0);
      cidx = 0;
    }
  }
  // individual crossovers
  else if (phase==3) {
    // moving past the end
    if ((sidx==jfFFN_SECDATA::scountm) && (cidx==(ccount-1))) {
      phase = 4;
      return false;
    }
    // moving to the next crossover section
    else if (cidx==(ccount-1)) {
      sidx++;
      ccount = cross_sections[sidx]->CatCount();
      cross_sections[sidx]->ResetIndex();
      cross_sections[sidx]->NextIndex();
      assert(ccount!=0);
      cidx = 0;
    }
    // general case
    else {
      cidx++;
      cross_sections[sidx]->NextIndex();
    }
  }
  else return false;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// url indexes
//----------------
QString jfFFN_Categories::UrlAtIndex() const {
  if ((phase==0) || (phase==4)) return "";
  if (phase==1) return jfFFN_SECDATA::IndexToURL(sidx,false);
  if (phase==2) return jfFFN_SECDATA::IndexToURL(sidx,true);
  if (phase==3) {
    return cross_sections[sidx]->IndexLink(false,false);
  }
  return ""; // never
}
//----------------
QString jfFFN_Categories::UrlAtNextIndex() const {
  const QString fname = "jfFFN_Categories::UrlAtNextIndex";
  /**/JDEBUGLOGST(fname,1,sidx);
  if ((phase==0) || (phase==4)) return "";
  if (phase==1) return jfFFN_SECDATA::IndexToURL(sidx+1,false);
  if (phase==2) return jfFFN_SECDATA::IndexToURL(sidx+1,true);
  if (phase==3) {
    return cross_sections[sidx]->IndexLink(true,false);
  }
  return ""; // never
}
//----------------
QString jfFFN_Categories::NameAtIndex() const {
  QString xres;
  if ((phase==0) || (phase==4)) return "";
  if (phase==1) return jfFFN_SECDATA::section_names[sidx];
  if (phase==2) {
    xres = jfFFN_SECDATA::section_names[sidx];
    xres += " Crossovers";
  }
  else if (phase==3) {
    xres = cross_sections[sidx]->NameAtIndex(true);
  }
  else assert(false);
  return xres;
}
// ---------------------------------
QString jfFFN_Categories::NameAtUrlIndex(size_t section_index) const {
    if (section_index >= jfFFN_SECDATA::scount) return "";
    return cross_sections[section_index]->NameAtUrlIndex();
}
//----------------
bool jfFFN_Categories::UpdatableAtIndex() const {
  if (phase==3) {
    return cross_sections[sidx]->GetDoUpdateAtIndex();
  }
  else assert(false);
  return false;
}
//--------------------
QString* jfFFN_Categories::CurrentCrossoverURL(size_t section_index) const {
  if (section_index >= jfFFN_SECDATA::scount) return NULL;
  else return cross_sections[section_index]->DownURLAtIndex();
}
//-------------------------------
bool jfFFN_Categories::UrlIndexToStart(size_t section_index) {
  if (section_index >= jfFFN_SECDATA::scount) return false;
  cross_sections[section_index]->ResetUrlIndex();
  return true;
}
//-------------------------------
bool jfFFN_Categories::NextUrlIndex(size_t section_index) {
  if (section_index >= jfFFN_SECDATA::scount) return false;
  return cross_sections[section_index]->NextUrlIndex();
}
//-------------------------------
bool jfFFN_Categories::NextUrlIndex(size_t section_index, bool& updatable) {
  if (section_index >= jfFFN_SECDATA::scount) return false;
  return cross_sections[section_index]->NextUrlIndex(updatable);
}
//-------------------------------
size_t jfFFN_Categories::CrossoverIndex() const {
    return cidx;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// item info
//--------------------
size_t jfFFN_Categories::ItemsInSection() const {
  if ((phase==1) || (phase==2)) return (jfFFN_SECDATA::scount);
  if (phase==3) return cross_sections[sidx]->CatCount();
  assert(false);
  return 0;
}
//--------------------
size_t jfFFN_Categories::ItemsInSection(size_t sec_idx) const {
  if (cross_sections[sec_idx]==NULL) return 0;
  else return cross_sections[sec_idx]->CatCount();
}
//----------------
bool jfFFN_Categories::SortAndComplete() {
  const QString fname = "jfFFN_Categories::SortAndComplete";
  // variables
  size_t sidx, snncount = 0;
  bool sres;
  // checking
  /**/JDEBUGLOG(fname,1)
  for (sidx = 0; sidx<(jfFFN_SECDATA::scount) ; sidx++ ) {
    if (cross_sections[sidx]!=NULL) snncount++;
  }
  /**/JDEBUGLOGST(fname,2,snncount)
  if (snncount!=(jfFFN_SECDATA::scount)) return false;
  // sorting
  /**/JDEBUGLOG(fname,3)
  for (sidx = 0; sidx<(jfFFN_SECDATA::scount) ; sidx++) {
    /**/JDEBUGLOGST(fname,4,sidx)
    sres = cross_sections[sidx]->SortAll();
    /**/JDEBUGLOGB(fname,5,sres)
    if (!sres) return false;
  }
  // done
  /**/JDEBUGLOG(fname,6)
  dataset = true;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// applying the category selector
jfFFN_CatList* jfFFN_Categories::MakeSearchList(jfFFN_CatP_SelCat* selector, size_t which) {
  const QString fname = "jfFFN_Categories::MakeSearchList";
  // special cases
  if (selector==NULL) return NULL;
  if (which>=32) return NULL;
  // search variables
  jfFFN_CatList* result = new jfFFN_CatList();
  int secindex; bool markres;
  QString sec_cname, sec2_name, cgroup_name, catname;
  // starting
  selector->ResetIndex();
  for (secindex = 0; secindex < (jfFFN_SECDATA::scount) ; secindex++) {
    csecmap[secindex] = new jfFFN_SectionMap();
  }
  // looping over the individual sections strings
  while (selector->NextIndex()) {
    sec_cname = selector->GetCrossSection();
    // section-based marking
    if (sec_cname=="") {
      // finding the section
      sec2_name = selector->GetGroupSection();
      secindex = jfFFN_SECDATA::SectionName2Index(sec2_name);
      if (secindex<0) assert(false); // should never happen
      // marking
      catname = selector->GetCatName();
      if (catname=="*") {
        markres = sections[secindex]->MarkAndAddAll(*(csecmap[secindex]),which);
      }
      else {
        markres = sections[secindex]->MarkAndAddByName(*(csecmap[secindex]),catname,which);
        assert(markres); // should never happen
      }
    }
    // crossover-based marking
    else {
      // finding the section
      secindex = jfFFN_SECDATA::SectionName2Index(sec_cname);
      if (secindex<0) assert(false); // should never happen
      // next catgeory group
      cgroup_name = selector->GetGroupSection();
      if (cgroup_name!="*") {
        catname = selector->GetCatName();
        if (catname!="*") {
          markres = cross_sections[secindex]->MarkCombo(cgroup_name,catname,which);
        }
        else {
          markres = cross_sections[secindex]->MarkAllCategory(cgroup_name,which);
        }
      }
      else {
        markres = cross_sections[secindex]->MarkEverything(which);
        assert(markres);
      }
    }
  }
  // afterwards, we apply AND / OR matchers...
  const jfSimpleExpr *orexp, *andexp;
  size_t apres;
  if (selector->FiltersNotEmpty()) {
    andexp = selector->GetAndFilter();
    orexp = selector->GetOrFilter();
    // applying the filters to sections
    for (secindex = 0 ; secindex < (jfFFN_SECDATA::scount) ; secindex++ ) {
      apres = ApplySectionAnd(andexp,which,secindex);
      assert(apres>=0);
      apres = ApplySectionOr(orexp,which,secindex);
      assert(apres>=0);
    }
    // next up, we apply the filters to the crossovers
    for (secindex = 0 ; secindex < (jfFFN_SECDATA::scount) ; secindex++ ) {
      cross_sections[secindex]->ApplyExpressionMarking(orexp,andexp,which);
    }
  }

  // finally, we add the the final list of results and clear up intermediate stuff
  for (secindex = 0 ; secindex < (jfFFN_SECDATA::scount) ; secindex++ ) {
    csecmap[secindex]->AppendToList(*result);
    delete csecmap[secindex];
    csecmap[secindex] = NULL;
  }
  for (secindex = 0 ; secindex < (jfFFN_SECDATA::scount) ; secindex++ ) {
    cross_sections[secindex]->AddToCategoryList(*result);
  }
  // done
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFN_Categories::GetTypeID() const {
  return "FFN_Categories";
}
//++++++++++++++++++++++++++++++++++++++++++++
// basic info
size_t jfFFN_Categories::GetPhase() const { return phase; }
bool jfFFN_Categories::IsDataSet() const { return dataset; }
//++++++++++++++++++++++++++++++++++++++++++++
// --- [ update mode methods ] ---
//---------------------------------------
void jfFFN_Categories::Set_OldDataLink(const jfFFN_Categories* in_old_data) {
  old_data = in_old_data;
}
//---------------------------------------
// this should only be called between phase 2 and 3, when updating old results
bool jfFFN_Categories::CompareAndSetUpdates() {
  if (old_data==NULL) return false;
  for (size_t cloop = 0; cloop < jfFFN_SECDATA::scount; cloop++) {
    cross_sections[cloop]->SetUpdatesByComparing((*old_data).cross_sections[cloop]);
  }
  return true;
}
//--------------------------------------------
bool jfFFN_Categories::CopyDataAtIndex() {
  // variables
  QString ccat_name;
  jfFFN_CrossoverGroup* copy_data;
  bool tres;
  // checks
  if (phase!=3) return false;
  tres = cross_sections[sidx]->GetDoUpdateAtIndex();
  if (tres) return false;
  // here, there is no update, so we copy the old data over...
  ccat_name = cross_sections[sidx]->NameAtIndex(false);
  copy_data = NULL;
  jfFFN_CrossoverSection* old_section = (old_data->cross_sections)[sidx];
  tres = old_section->GetIntegratedCopyByName(ccat_name,copy_data,cross_store);
  if (!tres) return false;
  tres = cross_sections[sidx]->InsertGroupAtIndex(copy_data);
  return tres;
}
//++++++++++++++++++++++++++++++++++++++++++++
const jfFFN_CategoryCore* jfFFN_Categories::FindCatFromFinder(const QString& infind) const {
  // bad cases
  if (!dataset) return NULL;
  if (infind.isEmpty()) return NULL;
  // variables
  jfLineParse* oval;
  size_t pcount;
  int secindex, crospos;
  QString secname;
  const jfFFN_CategoryCore* xresult = NULL;
  // starting
  oval = new jfLineParse(infind);
  pcount = oval->Num();
  // case non-crossover or crossover index
  if (pcount==2) {
    // checking for a crossover section name (and getting the non-crossover part)
    secname = oval->UnEscStr(0);
    crospos = secname.lastIndexOf("Crossovers");
    if (crospos>=0) {
      secname.truncate(crospos);
      secname = secname.trimmed();
    }
    // getting the section index
    secindex = jfFFN_SECDATA::SectionName2Index(secname);
    if (secname>=0) {
      // getting the crossover index
      if (crospos<0) xresult = cross_sections[secindex]->GetCrossoverIndex(oval->UnEscStr(1));
      // getting a section category
      else xresult = sections[secindex]->GetCatByName(oval->UnEscStr(1));
    }
  }
  // full crossover
  else if (pcount==4) {
    secindex = jfFFN_SECDATA::SectionName2Index(oval->UnEscStr(0));
    if (secindex>=0) {
      xresult = cross_sections[secindex]->GetCrossoverCatPointer(oval->UnEscStr(1),oval->UnEscStr(2),oval->UnEscStr(3));
    }
  }
  // finishing off
  delete oval;
  return xresult;
}
//--------------------------------------------
void jfFFN_Categories::MakeCrossStore() {
  cross_store = new jfFFN_CrossCatStore();
}
//--------------------------------------------
jfFFN_CrossCatStore* jfFFN_Categories::GetCrossoverStoragePtr(){
  return cross_store;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFFN_Categories::~jfFFN_Categories() {
  const QString fname = "jfFFN_Categories::~jfFFN_Categories";
  for (size_t idx = 0; idx < jfFFN_SECDATA::scount; idx++) {
    if (cross_sections[idx]!=NULL) delete cross_sections[idx];
    if (sections[idx]!=NULL) delete sections[idx];
  }
  if (cross_store!=NULL) delete cross_store;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// marking helper methods
//-------------------------------
// applying AND expression to previous marked non-crossover results
int jfFFN_Categories::ApplySectionAnd(const jfSimpleExpr* in_andexp, size_t which, size_t sindex) {
  // bad cases
  if (which>=32) return -1;
  if (sindex>jfFFN_SECDATA::scount) return -1;
  if (csecmap[sindex]==NULL) return -1;
  // trivially no change cases
  if (in_andexp==NULL) return 0;
  if (in_andexp->isEmpty()) return 0;
  if ((csecmap[sindex]->size())==0) return 0;
  // variables for the usual case
  jfFFN_SectionMap::iterator xitr;
  QString curr_name;
  QStringList* rem_list;
  bool exp_res;
  size_t and_idx, and_count;
  // starting for the first loop
  rem_list = new QStringList();
  xitr = csecmap[sindex]->begin();
  // the first loop, finding out what to remove
  while (xitr!=(csecmap[sindex]->end())) {
    curr_name = xitr->first;
    exp_res = in_andexp->InternalMatch(curr_name);
    if (!exp_res) rem_list->append(curr_name);
    xitr++;
  }
  and_count = rem_list->count();
  // looping over the removal names
  for ( and_idx=0 ; and_idx<and_count ; and_idx++ ) {
    exp_res = sections[sindex]->UnmarkAndRemoveByName(*(csecmap[sindex]),rem_list->at(and_idx),which);
    assert(exp_res);
  }
  // done
  delete rem_list;
  return and_count;
}
//-------------------------------
// applying AND expression to previous marked non-crossover results
int jfFFN_Categories::ApplySectionOr(const jfSimpleExpr* in_orexp, size_t which, size_t sindex) {
  // bad cases
  if (which>=32) return -1;
  if (sindex>jfFFN_SECDATA::scount) return -1;
  if (csecmap[sindex]==NULL) return -1;
  // trivially no change cases
  if (in_orexp==NULL) return 0;
  if (in_orexp->isEmpty()) return 0;
  if (sections[sindex]->GetCount()==0) return 0;
  // variables
  size_t addcount = 0;
  QString cname;
  bool or_res;
  // starting
  sections[sindex]->ResetIndex();
  // the main loop
  while (sections[sindex]->NextIndex()) {
    cname = sections[sindex]->GetCurrentName();
    or_res = in_orexp->InternalMatch(cname);
    if (or_res) {
      or_res = sections[sindex]->MarkCurrentAndAdd(*csecmap[sindex],which);
      assert(or_res);
      addcount++;
    }
  }
  // afterloop
  return addcount;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//-------------------------------
bool jfFFN_Categories::AddMiddleToFile(QTextStream* outfile) const {
  size_t sindex;
  // checking and special conditions
  if (outfile==NULL) return false;
  if (!dataset) return false;
  // writing the section data
  for (sindex=0; sindex < jfFFN_SECDATA::scount ; sindex++) {
    if (!(sections[sindex]->AddToFile(outfile))) return false;
  }
  // writing crossovers
  cross_store->AddToFile(outfile);
  return true;
}
//-------------------------------
bool jfFFN_Categories::AddRestToFile(QTextStream* outfile) const {
  size_t sindex;
  // checking and special conditions
  if (outfile==NULL) return false;
  if (!dataset) return false;
  // writing the crossover indexes
  for (sindex=0; sindex < jfFFN_SECDATA::scount ; sindex++) {
    if (!(cross_sections[sindex]->AddToFile(outfile))) return false;
  }
  return true;
}
//-------------------------------
bool jfFFN_Categories::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_Categories::ReadMiddleFromFile";
  // input data
  size_t sindex;
  bool rval;
  // starting
  dataset = false;
  // reading the section data
  for (sindex=0; sindex < jfFFN_SECDATA::scount ; sindex++) {
    sections[sindex] = new jfFFNSection();
    rval = sections[sindex]->GetFromFile(infile);
    if (!rval) {
      delete sections[sindex];
      sections[sindex] = NULL;
      return false;
    }
  }
  // reading in the crossovers
  cross_store = new jfFFN_CrossCatStore();
  rval = cross_store->GetFromFile(infile);
  if (!rval) {
    delete cross_store;
    cross_store = NULL;
    return false;
  }
  // done
  return true;
}

//-------------------------------
bool jfFFN_Categories::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_Categories::ReadMiddleFromFile";
  // input data
  size_t sindex;
  bool rval;
  // reading the section crossover indexes
  for (sindex=0; sindex < jfFFN_SECDATA::scount ; sindex++) {
    cross_sections[sindex] = new jfFFN_CrossoverSection(cross_store);
    rval = cross_sections[sindex]->GetFromFile(infile);
    if (!rval) {
      delete cross_sections[sindex];
      cross_sections[sindex] = NULL;
      return false;
    }
  }
  // done
  dataset = true;
  return true;
}
//***************************************************************************
jfFFN_CatManager* ffn_catorganizer::main_manager;
const QString ffn_catorganizer::fname = "config/ffnscat.dat";
//==================================================================


// the constructor
jfFFN_CatManager::jfFFN_CatManager() {
  // initialization
  isempty = true;
  changed = false;
  cur_holder = NULL;
  new_holder = NULL;
  number_of_searches = 0;
  // the searches
  for (size_t sindex=0; sindex<32; sindex++) {
    searchers[sindex] = NULL;
  }

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic info and simple methods
//---------------------------------------------------------
bool jfFFN_CatManager::Empty() const {
  return isempty;
}
//---------------------------------------------------------
jfFFN_Categories* jfFFN_CatManager::GetNewHolder() {
  // we should never call this if a previous new is not done or non-existant
  assert(new_holder==NULL);
  new_holder = new jfFFN_Categories();
  return new_holder;
}
//---------------------------------------------------------
jfFFN_Categories* jfFFN_CatManager::GetHolder() {
  return cur_holder;
}
//---------------------------------------------------------
bool jfFFN_CatManager::NewDone() {
  //constants
  const QString fname = "jfFFN_CatManager::NewDone";
  // assertions
  if (new_holder==NULL) return false;
  if (cur_holder!=NULL) {
    new_holder->Set_OldDataLink(NULL);
    delete cur_holder;
  }
  // assignments
  cur_holder = new_holder;
  new_holder = NULL;
  isempty = false;
  changed = true;
  // next, we send messages to update the searchers
  // done
  return true;
}
//---------------------------------------------------------
bool jfFFN_CatManager::DiscardNew() {
  if (new_holder==NULL) return false;
  delete new_holder;
  new_holder = NULL;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//---------------------------------------------------------
/* Takes the downloaded data and writes it to a file. refurns false if the downloaded
data is not complete enough */
bool jfFFN_CatManager::WriteToFile() const {
  // constants
  const QString fname = "jfFFN_CatManager::WriteToFile";
  // the only variable
  QTextStream* outfile;
  bool owres;
  // we check if there is anything to write
  if (cur_holder==NULL) return false;
  else if (!changed) return false;
  else {
    // creating or opening the outfile
    outfile = GetOpenFile(ffn_catorganizer::fname,true);
    // writing
    cur_holder->AddToFile(outfile);
    owres = CloseFileStream(outfile,true);
    assert(owres);
    // writing done, we close the file
    delete outfile;
    // done
    return true;
  }
}
//---------------------------------------------------------
// reads the category data from fname
bool jfFFN_CatManager::ReadFromFile() {
  // constants
  const QString fname = "jfFFN_CatManager::ReadFromFile";
  const QString err_title = "Error in loading FFN Category data!";
  // local variables
  jfFileReader* infile;
  QMessageBox emsg;
  // creating and checking the text file
  infile = new jfFileReader(ffn_catorganizer::fname);
  if (infile->OpenFile()) {
    // making the data structure...
    if (new_holder!=NULL) delete new_holder;
    new_holder = new jfFFN_Categories();

    if (!new_holder->GetFromFile(infile)) {
      DiscardNew();
      /**/JDEBUGLOGS(fname,1,infile->err)
      emsg.setIcon(QMessageBox::Critical);
      emsg.setText(infile->err);
      emsg.setWindowTitle(err_title);
      emsg.exec();
      delete infile;
      return false;

    }
    // done
    NewDone();
    delete infile;
    changed = false;
    return true;
  }
  else {
      jerror::Log(fname,"Failed to open FFN category file.");
    /**/JDEBUGLOG(fname,7);
    delete infile;
    return false;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// registering searchers
//---------------------------------------------------------
size_t jfFFN_CatManager::RegisterSearch(const QWidget* in_search) {
  if (number_of_searches==32) return -1;
  for (size_t sloop=0; sloop<32; sloop++) {
    if (searchers[sloop] == NULL) {
      searchers[sloop] = in_search;
      number_of_searches++;
      return sloop;
    }
  }
  assert(false);
  return 0;
}
//---------------------------------------------------------
bool jfFFN_CatManager::RemoveSearch(size_t which) {
  // checking, the input is wrog somehow
  if (which >=32) return false;
  if (searchers[which]==NULL) return false;
  // we remove
  searchers[which] = NULL;
  number_of_searches--;
  // we also update the category info!
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFFN_CatManager::~jfFFN_CatManager() {
  // initialization
  isempty = true;
  if (cur_holder!=NULL) delete cur_holder;
  cur_holder = NULL;
  if (new_holder!=NULL) delete new_holder;
  new_holder = NULL;
  number_of_searches = 0;
  // the searches
  for (size_t sindex=0; sindex<32; sindex++) {
    searchers[sindex] = NULL;
  }
}

/**************************************************************************/
