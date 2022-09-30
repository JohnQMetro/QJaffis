/******************************************************************************
Name    :   gen_cat.cpp
Author  :   John Q Metro
Purpose :   General category data and info (non fanfiction.net)
Created :   August 8, 2012
// Conversion to Qt Started September 26, 2013
Updated :   September 14, 2016
******************************************************************************/
#ifndef GEN_CAT_H_INCLUDED
  #include "gen_cat.h"
#endif // GEN_CAT_H_INCLUDED
//-------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
//****************************************************************************
// constructors
//----------------------------
jfGeneralCategory::jfGeneralCategory():jfUrlBase() {
  multisection = false;
}
//----------------------------
jfGeneralCategory::jfGeneralCategory(const jfGeneralCategory& inval):jfUrlBase() {
  num_id = inval.num_id;
  name = inval.name;
  description = inval.description;
  validdata = inval.validdata;
  primarylink = inval.primarylink;
  section_names = inval.section_names;
  multisection = inval.multisection;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
//------------------------------------------
QString jfGeneralCategory::GetSection(size_t index) const {
  assert(index<section_names.count());
  return section_names[index];
}
//------------------------------------------
size_t jfGeneralCategory::SectionCount() const {
  return section_names.count();
}
//------------------------------------------
bool jfGeneralCategory::AddNewSection(const QString& new_section) {
  for (size_t sn_idx = 0 ; sn_idx < section_names.count() ; sn_idx++) {
    if (section_names[sn_idx] == new_section) return false;
  }
  section_names.push_back(new_section);
  return true;
}
//------------------------------------------
QString jfGeneralCategory::GetCombo() const {
  QString result = EscapeNL(section_names[0], IO_DELIMS);
  result += ';';
  result += EscapeNL(name, IO_DELIMS);
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// tests
bool jfGeneralCategory::IsCategory(QString tsname, QString tcatname) const {
  if (name != tcatname) return false;
  return section_names.contains(tsname);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//----------------------------
bool jfGeneralCategory::AddMiddleToFile(QTextStream* outfile) const {
  jfOutString resval;
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 3
  resval << primarylink << section_names.count();
  (*outfile) << resval << "\n";
  // line 4
  resval.FullClear();
  resval << section_names;
  (*outfile) << resval << "\n";
  return true;
}
//----------------------------
bool jfGeneralCategory::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfGeneralCategory::ReadMiddleFromFile";
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,funcname)) return infile->BuildError("CatDaataBadd!");
  primarylink = infile->lp.UnEscStr(0);
  size_t snamecount;
  if (!infile->lp.SIntVal(1,snamecount)) return infile->BuildError("Section Names Count Bad!");
  // reading the line with the section names
  if (!infile->ReadParseLine(snamecount,funcname)) return false;
  if ((infile->lp).UnEscAppendRest(0,section_names) != snamecount) {
    return infile->BuildError("Section Name count mismatch!");
  }
  multisection = snamecount > 1;
  return true;
}
//****************************************************************************
// constructors
//-----------------------------------------------------------
jfGenSection::jfGenSection(bool idlookup_in):jfBaseCollection() {
  for (size_t mindex=0;mindex<32;mindex++) matchcount[mindex] = 0;
  item_count = 0;
  validdata = false;
  idlookup = idlookup_in;
}
//-------------------------------------------------------------
jfGenSection::jfGenSection(QString& inname, bool idlookup_in):jfBaseCollection() {
  for (size_t mindex=0;mindex<32;mindex++) matchcount[mindex] = 0;
  name = inname;
  item_count = 0;
  validdata = false;
  idlookup = idlookup_in;
}
//------------------------------------------------------
bool jfGenSection::AppendCategory(jfGeneralCategory* category_in) {
  if(category_in == NULL) return false;
  if (!category_in->IsValid()) return false;
  maindata.push_back(category_in);
  item_count++;
  if (!idlookup) name_lookup[category_in->GetName()] = item_count-1;
  else id_lookup[category_in->GetID()] = item_count-1;
  validdata = true;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting some extra info
int jfGenSection::FindByName(const QString& inname) const {
  // constants
  if (idlookup) return -1;
  if (name_lookup.count(inname) == 0) return -1;
  return name_lookup[inname];
}
//-------------------------------------------
QString jfGenSection::NameAtIndex(const size_t& inindex) const {
  if (inindex>=maindata.size()) return "";
  return maindata[inindex]->GetName();
}
//--------------------------------------------
QString jfGenSection::UrlAtIndex(const size_t& inindex) const {
  if (inindex>=maindata.size()) return "";
  return maindata[inindex]->GetUrl();
}
//-------------------------------------------
const jfGeneralCategory* jfGenSection::ItemAtIndex(const size_t& inindex) const {
  if (inindex>=maindata.size()) return NULL;
  return maindata[inindex];
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// lookup methods
//-------------------------------------------
const jfGeneralCategory* jfGenSection::LookupByID(const size_t& idin) const {
  if (!idlookup) return NULL;
  if (id_lookup.count(idin) == 0) return NULL;
  size_t lindex = id_lookup[idin];
  return maindata[lindex];
}
//-------------------------------------------
const jfGeneralCategory* jfGenSection::LookupByName(const QString& namein) const {
  if (idlookup) return NULL;
  if (name_lookup.count(namein) == 0) return NULL;
  size_t lindex = name_lookup[namein];
  return maindata[lindex];
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// mark manipulation
//-------------------------------------------
bool jfGenSection::MarkByName(const QString& inname, bool value, size_t which) {
  // basic checks and assrts
  assert(which<32);
  // starting
  int loc = FindByName(inname);
  if (loc<0) return false;
  // getting the current value
  return MarkByIndex(loc,which,value);
}
//-------------------------------------------
bool jfGenSection::MarkByIndex(size_t inindex, size_t which, bool value) {
  if (inindex>=maindata.size()) return false;
  // checking for a do-nothing case
  bool cval = (maindata[inindex]->mark).test(which);
  if (cval==value) return true;
  // do something
  maindata[inindex]->mark.set(which,value);
  if (value) matchcount[which]++;
  else matchcount[which]--;
  return true;
}
//-------------------------------------------
bool jfGenSection::GetMarkByIndex(size_t inindex, size_t which) const {
  assert(inindex<maindata.size());
  return maindata[inindex]->mark.test(which);
}
//-------------------------------------------
bool jfGenSection::GetMarkByName(const QString& inname, size_t which) const {
  int mloc = FindByName(inname);
  if (mloc<0) return false;
  else return GetMarkByIndex(mloc,which);
}
//-------------------------------------------
size_t jfGenSection::MarkCount(size_t which) const {
  assert(which<32);
  return matchcount[which];
}
//--------------------------------------------
size_t jfGenSection::MarkAll(size_t which, bool value) {
  assert(which<32);
  for (size_t zindex=0;zindex<maindata.size();zindex++) {
    maindata[zindex]->mark.set(which,value);
  }
  if (value) matchcount[which] = maindata.size();
  else matchcount[which] = 0;
  return matchcount[which];
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented index methods
//-------------------------------------------
bool jfGenSection::NextIndex() {
  // if there are no items, a special case...
  if (item_count==0) {
    if (item_index==-1) item_index = -2;
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
      return false;
    }
    // the normal case..
    item_index++;
    curr_item = maindata[item_index];
    return true;
  }
}
//-------------------------------------------
bool jfGenSection::BackIndex() {
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
    if (item_index == -2)  item_index = item_count-1;
    // or a regular middle index..
    else item_index--;
    // we set the object
    curr_item = maindata[item_index];
    return true;
  }
}
//-------------------------------------------
void jfGenSection::ResetIndex() {
  item_index = -1;
  curr_item = NULL;
}
//-------------------------------------------
bool jfGenSection::ToIndex(const int& newindex) {
  // out of bounds ..
  if (newindex>=item_count) return false;
  if (newindex<-2) return false;
  // null indexes
  if (newindex<0) curr_item = NULL;
  // non-null indexes
  else curr_item = maindata[newindex];
  // finishing
  item_index = newindex;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more abstract methods
//-------------------------------------------
QStringList* jfGenSection::GetNameList() const {
  // local variables
  QStringList* result;
  size_t loopc;
  // we start...
  if (item_count==0) return NULL;
  result = new QStringList();
  // the main loop
  for (loopc = 0; loopc<item_count;loopc++) {
    result->append(maindata[loopc]->GetName());
  }
  // done
  return result;
}
//-------------------------------------------
QStringList* jfGenSection::GetDescriptionList() const {
  // local variables
  QStringList* result;
  size_t loopc;
  // we start...
  if (item_count==0) return NULL;
  result = new QStringList();
  // the main loop
  for (loopc = 0; loopc<item_count;loopc++) {
    result->append(maindata[loopc]->GetDescription());
  }
  // done
  return result;
}
//-------------------------------------------
QStringList* jfGenSection::GetTypeList() const {
  // local variables
  QStringList* result;
  size_t loopc;
  // we start...
  if (item_count==0) return NULL;
  result = new QStringList();
  // the main loop
  for (loopc = 0; loopc<item_count;loopc++) {
    result->append(maindata[loopc]->GetTypeID());
  }
  // done
  return result;
}
//-------------------------------------------
bool jfGenSection::DeleteAtIndex(const size_t& dindex) {
  // first, we do the usual checks
  if (dindex<0) return false;
  if (dindex>=item_count) return false;
  // we delete the item
  delete maindata[dindex];
  maindata[dindex] = NULL;
  // we we shorten the list
  maindata.erase(maindata.begin()+dindex);
  // we have to redo the indexes
  item_index = CalcNewIndex(item_index);
  ToIndex(item_index);
  backup_index = CalcNewIndex(backup_index);
  if (backup_index>=0) backup_item = maindata[backup_index];
  else backup_item = NULL;
  // done
  item_count--;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfGenSection::~jfGenSection() {
  if (idlookup) id_lookup.clear();
  else name_lookup.clear();
  for (size_t vindex=0;vindex<maindata.size();vindex++) {
    delete maindata[vindex];
  }
  maindata.clear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file I/O
//-------------------------------------------
bool jfGenSection::AddMid2ToFile(QTextStream* outfile) const {
  jfOutString resval;
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 3
  resval << idlookup;
  (*outfile) << resval << "\n";
  return true;
}
//-------------------------------------------
bool jfGenSection::AddRestToFile(QTextStream* outfile) const {
  size_t loopc;
  // the usual checks...
  if (outfile==NULL) return false;
  // the writing loop
  for (loopc=0; loopc<item_count;loopc++) {
    maindata[loopc]->AddToFile(outfile);
  }
  return true;
}
//-------------------------------------------
bool jfGenSection::ReadMid2FromFile(jfFileReader* infile) {
  QString tfval;
  // local variables
  const QString fname = "jfGenSection::ReadMid2FromFile";
  // we do checks and read the line
  JDEBUGLOG(fname,1)
  if (!infile->ReadUnEsc(tfval,fname)) return false;
  if (tfval == "false") idlookup = false;
  else if (tfval == "true") idlookup = true;
  else return infile->BuildError(fname + " : Line is supposed to be boolean");
  return true;
}
//-------------------------------------------
bool jfGenSection::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfGenSection::ReadRestFromFile";
  // local variables
  jfGeneralCategory* tempin = NULL;
  size_t expected_count,loopc;
  // the main reading loop...
  expected_count = item_count;
  item_count = 0;
  JDEBUGLOGI(fname,1,expected_count)
  for (loopc = 0; loopc<expected_count; loopc++) {
    tempin = MakeTypedCategory();
    if (!tempin->GetFromFile(infile)) {
      delete tempin;
      return false;
    }
    AppendCategory(tempin);
  }
  return true;
}
//****************************************************************************
