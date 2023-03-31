/******************************************************************************
Name    :   catgroupx.cpp
Author  :   John Q Metro
Purpose :   Data classes for making groupings of categories
Created :   September 9, 2016
Updated :   September 9, 2016
******************************************************************************/
#ifndef CATGROUPX_H
  #include "catgroupx.h"
#endif // CATGROUPX_H
//------------------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "../../../core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
//------------------------------------------
#include <assert.h>
/*****************************************************************************/
// The category group stores a set of catgeories, either by id or by name
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//------------------------------------------
jfCategoryGroup::jfCategoryGroup(bool idequiv_in):jfBaseObj() {
  loaddata_ptr = NULL;
  idequiv = idequiv_in;
  validdata = true;
}
//------------------------------------------
jfCategoryGroup::jfCategoryGroup(jfGen_CatHolder* source_ptr) {
  assert(source_ptr !=NULL);
  loaddata_ptr = source_ptr;
  validdata = false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic information
//------------------------------------------
bool jfCategoryGroup::ContainsCategory(const jfGeneralCategory* test_cat) const {
  if (test_cat == NULL) return false;
  if (idequiv) return ((idstore.count(test_cat))>0);
  else return ((namestore.count(test_cat))>0);
}
//------------------------------------------
size_t jfCategoryGroup::CategoryCount() const {
  if (idequiv) return idstore.size();
  else return namestore.size();
}
//------------------------------------------
QStringList* jfCategoryGroup::NamesList() const {
  QStringList* result = new QStringList;
  jfNCSet::const_iterator nameptr;
  jfIDSet::const_iterator idptr;
  // iterating over the set and recording the data
  if (idequiv) {
    idptr = idstore.begin();
    while (idptr != idstore.end()) {
      result->append((*idptr)->GetName());
      idptr++;
    }
  }
  else {
    nameptr = namestore.begin();
    while (nameptr != namestore.end()) {
      result->append((*nameptr)->GetName());
      nameptr++;
    }
  }
  return result;
}
//------------------------------------------
std::vector<size_t>* jfCategoryGroup::IDList() const {
  std::vector<size_t>* result = new std::vector<size_t>();
  jfNCSet::const_iterator nameptr;
  jfIDSet::const_iterator idptr;
  // iterating over the set and recording the data
  if (idequiv) {
    idptr = idstore.begin();
    while (idptr != idstore.end()) {
      result->push_back((*idptr)->GetID());
      idptr++;
    }
  }
  else {
    nameptr = namestore.begin();
    while (nameptr != namestore.end()) {
      result->push_back((*nameptr)->GetID());
      nameptr++;
    }
  }
  return result;

}
//------------------------------------------
QString jfCategoryGroup::GetTypeID() const {
  return "CategoryGrouping";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// change or add data
//------------------------------------------
bool jfCategoryGroup::AddCategory(const jfGeneralCategory* input_cat) {
  if (input_cat == NULL) return false;
  if (ContainsCategory(input_cat)) return false;
  if (idequiv) idstore.insert(input_cat);
  else namestore.insert(input_cat);
  return true;
}
//------------------------------------------
bool jfCategoryGroup::RetargetCategories(jfGen_CatHolder* new_category_source) {
  if (new_category_source == NULL) return false;
  QStringList strings;
  std::vector<size_t> ids;
  bool setcat;
  // recording old data pointerinfo
  jfNCSet::iterator nameptr;
  jfIDSet::iterator idptr;
  // iterating over the set and recording the data
  if (idequiv) {
    idptr = idstore.begin();
    while (idptr != idstore.end()) {
      ids.push_back((*idptr)->GetID());
      strings.append((*idptr)->GetSection());
      idptr++;
    }
  }
  else {
    nameptr = namestore.begin();
    while (nameptr != namestore.end()) {
      strings.append((*nameptr)->GetSection());
      strings.append((*nameptr)->GetName());
      nameptr++;
    }
  }
  // clearing out the old stores
  if (idequiv) idstore.clear();
  else namestore.clear();
  // setting the new
  if (idequiv) setcat = SetFromIDs(strings,ids);
  else setcat = SetFromCombos(strings);
  validdata = setcat;
  return setcat;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//------------------------------------------
bool jfCategoryGroup::AddMiddleToFile(QTextStream* outfile) const {
  jfOutString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  //one line with storage type BOOL and category count
  result << idequiv << CategoryCount();
  (*outfile) << result << '\n';
  return true;
}
//------------------------------------------
bool jfCategoryGroup::AddRestToFile(QTextStream* outfile) const {
  // variables for output
  jfOutString result;
  jfNCSet::const_iterator nameptr;
  jfIDSet::const_iterator idptr;
  // checking and special conditions
  if (outfile==NULL) return false;
  // iterating over the set and recording the data
  if (idequiv) {
    idptr = idstore.begin();
    while (idptr != idstore.end()) {
      result << (*idptr)->GetSection() << (*idptr)->GetID();
      idptr++;
    }
  }
  else {
    nameptr = namestore.begin();
    while (nameptr != namestore.end()) {
      result << (*nameptr)->GetSection() << (*nameptr)->GetName();
      nameptr++;
    }
  }
  // writing the data
  (*outfile) << result << '\n';
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o input
//------------------------------------------
bool jfCategoryGroup::ReadMiddleFromFile(jfFileReader* infile){
  assert(loaddata_ptr !=NULL);
  const QString funcname = "jfCategoryGroup::ReadMiddleFromFile";
  // input data
  QString buffer;

  // reading the line
  if (!infile->ReadParseLine(2,funcname)) return false;
  if (!infile->lp.BoolVal(0,idequiv)) {
    return infile->BuildError("The id/name boolean flag field is bad!");
  }
  if (!infile->lp.SIntVal(1,loadcount)) {
    return infile->BuildError("The number of categories field is bad");
  }
  return true;
}
//------------------------------------------
bool jfCategoryGroup::ReadRestFromFile(jfFileReader* infile) {
  assert(loaddata_ptr !=NULL);
  const QString funcname = "jfCategoryGroup::ReadRestFromFile";
  // variables
  QString lsection, lname;
  size_t lid;
  QStringList* strings;
  std::vector<size_t> ids;
  bool setcat;
  // reading
  if (!infile->ReadParseLine(loadcount*2,funcname)) return false;
  // loading for name based, there is a pre-built function
  if (!idequiv) {
    strings = (infile->lp).UnEscMultiple(0,loadcount*2);
  }
  // otherwise, we do a read loop
  else {
    // preparing for the read loop
    strings = new QStringList();
    // the loading loop
    for (size_t readidx = 0 ; readidx < loadcount*2 ; readidx += 2) {
      lsection = (infile->lp).UnEscStr(readidx);
      if (infile->lp.SIntVal(readidx+1,lid)) {
        delete strings;
        return infile->BuildError("An ID was incorrect!");
      }
      strings->append(lsection);
      ids.push_back(lid);
    }
  }
  // getting the info is done, but we need to set the categories using that info
  if (idequiv) setcat = SetFromIDs(*strings,ids);
  else setcat = SetFromCombos(*strings);
  delete strings;
  if (!setcat) return infile->BuildError("Matching the categories failed!");
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting catlinks from rawdata
//------------------------------------------
bool jfCategoryGroup::SetFromCombos(const QStringList& combolist) {
  assert(loaddata_ptr != NULL);
  assert(!idequiv);
  // variables
  QString cursec, curname;
  const jfGeneralCategory* foundcat = NULL;
  // looping through the combo list
  for (size_t xlist = 0 ; xlist < combolist.count() ; xlist+=2 ) {
    cursec = combolist.at(xlist);
    curname = combolist.at(xlist+1);
    foundcat = loaddata_ptr->FindCatByName(cursec,curname);
    if (foundcat == NULL) return false;
    AddCategory(foundcat);
  }
  return true;
}

//------------------------------------------
bool jfCategoryGroup::SetFromIDs(const QStringList& sectionlist, const std::vector<size_t>& idlist ) {
  // quick checks
  assert(loaddata_ptr != NULL);
  assert(idequiv);
  assert(idlist.size() == sectionlist.count());
  // variables
  QString cursec;
  size_t curid;
  const jfGeneralCategory* foundcat = NULL;
  // looping through the combo list
  for (size_t xlist = 0 ; xlist < sectionlist.count() ; xlist++ ) {
    cursec = sectionlist.at(xlist);
    curid = idlist.at(xlist+2);
    foundcat = loaddata_ptr->FindCatByID(cursec,curid);
    if (foundcat == NULL) return false;
    AddCategory(foundcat);
  }
  return true;
}

//====================================================================
// constructors and destructors
//----------------------------
jfCatGroupCollection::jfCatGroupCollection(bool idequiv_in):jfBaseObj() {
  idequiv = idequiv_in;
  loaddata_ptr = NULL;
  validdata = true;
}
//----------------------------
jfCatGroupCollection::jfCatGroupCollection(jfGen_CatHolder* source_ptr):jfBaseObj() {
  assert(source_ptr != NULL);
  loaddata_ptr = source_ptr;
  validdata = false;
}
//----------------------------
jfCatGroupCollection::~jfCatGroupCollection() {
  jfCategoryGroup* temp;
  name_lookup.clear();
  id_lookup.clear();
  for (size_t gidx = 0 ; gidx < maindata.size() ; gidx++ ) {
    temp = maindata[gidx];
    maindata[gidx] = NULL;
    delete temp;
  }
  maindata.clear();
  loaddata_ptr = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++
// informational methods
//----------------------------
size_t jfCatGroupCollection::CollectionCount() const {
  return maindata.size();
}
//----------------------------
bool jfCatGroupCollection::CategoryInGroupAtIndex(const jfGeneralCategory* categ_in, size_t cindex) {
  if (cindex >= maindata.size()) return false;
  if (categ_in == NULL) return false;
  return maindata[cindex]->ContainsCategory(categ_in);
}
//----------------------------
jfCategoryGroup* jfCatGroupCollection::GroupContainingCategory(jfGeneralCategory* categ_in) {
  if (categ_in == NULL) return NULL;
  if (idequiv) {
    if (0 == id_lookup.count(categ_in->GetID())) return NULL;
    else return id_lookup[categ_in->GetID()];
  }
  else {
    if (0==name_lookup.count(categ_in->GetName())) return NULL;
    else return name_lookup[categ_in->GetName()];
  }
}
//++++++++++++++++++++++++++++++++++++++++++++
// altering info
//----------------------------
jfCategoryGroup* jfCatGroupCollection::MakeNewGroup(const QString& name) {
  if (name.isEmpty()) return NULL;
  jfCategoryGroup* newgroup = new jfCategoryGroup(idequiv);
  newgroup->SetName(name);
  maindata.push_back(newgroup);
  return newgroup;
}
//----------------------------
bool jfCatGroupCollection::InsertCategoryInGroupAt(size_t cindex, const jfGeneralCategory* categ_in ) {
  // false for bad inputs
  if (categ_in == NULL) return false;
  if (cindex >= maindata.size()) return false;
  // false if the category is already in the group
  bool ins_res = maindata[cindex]->ContainsCategory(categ_in);
  if (!ins_res) return false;
  // the id or name
  if (idequiv) {
    if (id_lookup.count(categ_in->GetID()) != 0) return false;
    maindata[cindex]->AddCategory(categ_in);
    id_lookup[categ_in->GetID()] = maindata[cindex];
  }
  else {
    if (name_lookup.count(categ_in->GetName()) != 0) return false;
    maindata[cindex]->AddCategory(categ_in);
    name_lookup[categ_in->GetName()] = maindata[cindex];
  }
  return true;
}
//----------------------------
bool jfCatGroupCollection::InsertCategoryIntoGroup(const jfCategoryGroup* thisgroup, const jfGeneralCategory* categ_in ) {
  if (categ_in == NULL) return false;
  if (thisgroup == NULL) return false;
  size_t mindex;
  for (mindex = 0 ; mindex < maindata.size() ; mindex++ ) {
    if (thisgroup == maindata[mindex]) break;
  }
  if (mindex < maindata.size()) {
    return InsertCategoryInGroupAt(mindex,categ_in);
  }
  else return false;
}

//++++++++++++++++++++++++++++++++++++++++++++
// helper methods
bool jfCatGroupCollection::BuildQuickLookup() {
  size_t mindex;
  size_t lindex;
  QStringList* nameslist;
  std::vector<size_t>* idlist;
  // startprep
  id_lookup.clear();
  name_lookup.clear();
  // looping through the groups
  for (mindex = 0 ; mindex < maindata.size() ; mindex++ ) {
    if (idequiv) {
      idlist = maindata[mindex]->IDList();
      for (lindex = 0 ; lindex < idlist->size() ; lindex++) {
        id_lookup[(*idlist)[lindex]] = maindata[mindex];
      }
      delete idlist;
    }
    else {
      nameslist = maindata[mindex]->NamesList();
      for (lindex = 0 ; lindex < nameslist->size() ; lindex++) {
        name_lookup[(*nameslist)[lindex]] = maindata[mindex];
      }
      delete nameslist;
    }
  }
  // done
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//----------------------------
bool jfCatGroupCollection::AddMiddleToFile(QTextStream* outfile) const {
  jfOutString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  //one line with storage type BOOL and category count
  result << idequiv << CollectionCount();
  (*outfile) << result << '\n';
  return true;
}
//----------------------------
bool jfCatGroupCollection::AddRestToFile(QTextStream* outfile) const {
  // checking and special conditions
  if (outfile==NULL) return false;
  size_t cmax = CollectionCount();
  size_t cidx;
  bool rres;
  // the write loop
  for (cidx = 0 ; cidx < cmax ; cidx++) {
    rres = maindata[cidx]->AddToFile(outfile);
    if (!rres) return false;
  }
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++
// file i/o input
//----------------------------
bool jfCatGroupCollection::ReadMiddleFromFile(jfFileReader* infile) {
  assert(loaddata_ptr !=NULL);
  const QString funcname = " jfCatGroupCollection::ReadMiddleFromFile";
  // reading
  if (!infile->ReadParseLine(2,funcname)) return false;
  if (!(infile->lp).BoolVal(0,idequiv)) return infile->BuildError("ID equivalent flag is not good!");
  if (!(infile->lp).SIntVal(1,loadcount)) return infile->BuildError("The number of groups is bad!");
  return true;
}

//----------------------------
bool jfCatGroupCollection::ReadRestFromFile(jfFileReader* infile) {
  assert(loaddata_ptr !=NULL);
  assert(infile!=NULL);
  const QString funcname = "jfCatGroupCollection::ReadRestFromFile";
  // variables
  size_t gindex;
  bool gload_ok;
  jfCategoryGroup* curr_group;
  // the loop where we load the groups...
  for ( gindex = 0 ; gindex < loadcount ; gindex++ ) {
    // creating and loading a group from file
    curr_group = new jfCategoryGroup(loaddata_ptr);
    gload_ok = curr_group->GetFromFile(infile);
    if (!gload_ok) {
      delete curr_group;
      return false;
    }
    // adding the group
    maindata.push_back(curr_group);
  }
  // if done, we then make the quick lookups
  if (!BuildQuickLookup()) return infile->BuildError("Making Lookup failed! Duplicates?");
  return true;
}

/*****************************************************************************/
