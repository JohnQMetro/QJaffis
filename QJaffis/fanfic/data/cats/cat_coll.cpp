/******************************************************************************
Name    :   cat_coll.cpp
Author  :   John Q Metro
Purpose :   General classes and methods for holding site category info (multiple
            section, indexing, etc).
Created :   August 11, 2012
Conversion to Qt Started September 28, 2013
Updated :   September 10, 2016
******************************************************************************/
#ifndef CAT_COLL_H_INCLUDED
  #include "cat_coll.h"
#endif // CAT_COLL_H_INCLUDED
#ifndef AO3_CATS_H_INCLUDED
  #include "../ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------------------------------
#include <assert.h>
//****************************************************************************
jfGenCat_PickData::jfGenCat_PickData():jfBaseObj(0,"Category Pick Data") {
  item_count = 0;
  item_index = -1;
  and_filter = NULL;
  or_filter = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// index methods
//--------------------------------------------------------
void jfGenCat_PickData::ResetIndex() {
  item_index = -1;
}
//--------------------------------------------------------
bool jfGenCat_PickData::NextIndex() {
  if (item_index==-2) return false;
  int csize = sec_name.count();
  if (csize == (item_index+1)) {
    item_index = -2;
    return false;
  }
  // the normal case
  item_index++;
  return true;
}
//--------------------------------------------------------
bool jfGenCat_PickData::BackIndex() {
  if (item_index==-1) return false;
  int csize = sec_name.count();
  if ((csize == 0) || (item_index==0)) {
    item_index = -1;
    return false;
  }
  // the normal case
  item_index--;
  return true;
}
//--------------------------------------------------------
bool jfGenCat_PickData::ToIndex(size_t toindex) {
  if (toindex<-2) return false;
  int csize = sec_name.count();
  if (toindex>=csize) return false;
  item_index = toindex;
  return true;
}
//--------------------------------------------------------
int jfGenCat_PickData::WhichIndex() const {
  return item_index;
}
//--------------------------------------------------------
size_t jfGenCat_PickData::categoryCount() const {
  return cat_name.size();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info at the current index
//--------------------------------------------------------
QString jfGenCat_PickData::GetSection() const {
  if (item_index<0) return "";
  return sec_name[item_index];
}
//--------------------------------------------------------
QString jfGenCat_PickData::GetCatName() const {
  if (item_index<0) return "";
  return cat_name[item_index];
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
QStringList* jfGenCat_PickData::GetListing() const {
  // local vars
  QStringList* result;
  size_t iloop;
  // we get things
  result = new QStringList();
  for (iloop=0; iloop<item_count; iloop++ ) {
    result->append(Item2Str(iloop));
  }
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting the expression filters
bool jfGenCat_PickData::SetAndFilter(jfSimpleExpr* in_value) {
  if (in_value==NULL) return false;
  if (!(in_value->IsValid())) return false;
  and_filter = in_value;
  return true;
}
//-----------------------------------------------------------------------------
bool jfGenCat_PickData::SetOrFilter(jfSimpleExpr* in_value) {
  if (in_value==NULL) return false;
  if (!(in_value->IsValid())) return false;
  or_filter = in_value;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfGenCat_PickData::GetAnd() const {
  if (and_filter==NULL) return "";
  else return and_filter->GetSrc();
}
//-----------------------------------------------------------------------------
QString jfGenCat_PickData::GetOr() const {
  if (or_filter==NULL) return "";
  else return or_filter->GetSrc();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// evaluating the filters
//----------------------------------------------------------------------------
bool jfGenCat_PickData::EvalAndExpr(QString insrc) const {
  if (and_filter==NULL) return true;
  else if (and_filter->isEmpty()) return true;
  else return and_filter->InternalMatch(insrc);
}
//----------------------------------------------------------------------------
bool jfGenCat_PickData::EvalOrExpr(QString insrc) const {
  if (or_filter==NULL) return false;
  else if (or_filter->isEmpty()) return false;
  else return or_filter->InternalMatch(insrc);
}
//-------------------------------------------------------------------------
bool jfGenCat_PickData::NotEmptyFil() const {
  if ((and_filter!=NULL) && (!and_filter->isEmpty())) return true;
  if ((or_filter!=NULL) && (!or_filter->isEmpty())) return true;
  return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// adding data
//--------------------------------------------------------
void jfGenCat_PickData::AddSection(const QString& secname, bool duplicheck) {
  size_t notused;
  if (!(duplicheck && FindItem(notused,secname,"*"))) {
    RemoveSection(secname);
    sec_name.append(secname);
    cat_name.append("*");
  }
  item_count++;
}
//--------------------------------------------------------
void jfGenCat_PickData::AddCategory(const QString& secname, const QString& catname, bool duplicheck) {
  size_t notused;
  if (!(duplicheck && FindItem(notused,secname,catname))) {
    sec_name.append(secname);
    cat_name.append(catname);
  }
  item_count++ ;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// removing data
//--------------------------------------------------------
// resmoves a section (including any individual categories from that section) from the search list
bool jfGenCat_PickData::RemoveSection(const QString& secname) {
  const QString fname="jfGenCat_PickData::RemoveSection";
  bool result = false;
  ResetIndex();
  // we iterate over the stored data. removing anything which belongs to the section
  while (NextIndex()) {
    if (sec_name[item_index]==secname) {
      RemoveAtIndex(item_index);
      item_index--;
      result = true;
    }
  }
  return result;
}
//--------------------------------------------------------
// removes a specific category
bool jfGenCat_PickData::RemoveCategory(const QString& secname, const QString& catname) {
  size_t locx;
  bool rval = FindItem(locx,secname,catname);
  if (rval) {
    RemoveAtIndex(locx);
    ResetIndex();
  }
  return rval;
}
//--------------------------------------------------------------------------
// removes a search, but one size fits all here... except we only remove specific searches, not whole sections or such
bool jfGenCat_PickData::RemoveByString(const QString& inval) {
  // constants
  const QString fname = "jfGenCat_PickData::RemoveByString";
  // local variables
  jfLineParse* parser;
  size_t floc;

  // parsing the input
  parser = new jfLineParse(inval);
  // special case faliures: the input is invalid
  if ((parser->Num())!=2) {
    delete parser;
    return false;
  }
  else {
      bool fval = FindItem(floc,parser->GetT(0),parser->GetT(1));
      delete parser;
      if (!fval) return false;
      else return RemoveAtIndex(floc);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Looks for categories within the specified group/section. Retuns NULL if nothing is
found, an empty result if we're supposed to search all of them, or a list of the
specific categories to search */
QStringList* jfGenCat_PickData::FindSection(const QString& inname, QString top) const {
  // local variables
  QStringList* result;
  size_t loopc;
  // we start
  result = NULL;
  for (loopc=0; loopc<item_count; loopc++) {
    if (sec_name[loopc]==inname) {
      if (result==NULL) result = new QStringList;
      if (cat_name[loopc]=="*") {
        assert(result->count()==0);
        return result;
      }
      result->append(cat_name[loopc]);
    }
  }
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
QString jfGenCat_PickData::GetTypeID() const {
  return "GeneralCategoryPickerResults";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfGenCat_PickData::~jfGenCat_PickData() {
  if (and_filter!=NULL) delete and_filter;
  if (or_filter!=NULL) delete or_filter;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// private helper methods
//--------------------------------------------------------
// removes the search at the specified index
bool jfGenCat_PickData::RemoveAtIndex(size_t inindex) {
  if ((inindex<0) || (inindex>=item_count)) return false;
  sec_name.removeAt(inindex);
  cat_name.removeAt(inindex);
  item_count--;
  return true;
}
//-------------------------------------------------------------------
// looks for a search
bool jfGenCat_PickData::FindItem(size_t& outloc, QString first, QString second) {
  const QString fname="jfGenCat_PickData::FindItem";
  for (size_t loopc=0; loopc<item_count; loopc++) {
    if ((sec_name[loopc]==first) && (cat_name[loopc]==second)) {
      outloc = loopc;
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
QString jfGenCat_PickData::Item2Str(size_t which) const {
  QString result = "";
  if (which>=item_count) return "";
  result = sec_name[which] + " ; ";
  result += cat_name[which];
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// private implemented virtual methods
//-------------------------------------------------------------------
bool jfGenCat_PickData::AddMiddleToFile(QTextStream* outfile) const {
  QString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding the number of items
  (*outfile) << QString::number(item_count) << "\n";
  // adding the and_filter
  if (and_filter==NULL) result = "";
  else result = EscapeNL(and_filter->GetSrc(),IO_DELIMS);
  (*outfile) << result << "\n";
  // adding the or_filter
  if (or_filter==NULL) result = "";
  else result = EscapeNL(or_filter->GetSrc(),IO_DELIMS);
  (*outfile) << result << "\n";
  // done
  return true;
}
//-------------------------------------------------------------------
bool jfGenCat_PickData::AddRestToFile(QTextStream* outfile) const {
  jfOutString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // the main loop
  for (size_t loopc=0; loopc<item_count; loopc++) {
      result << sec_name[loopc] << cat_name[loopc];
      (*outfile) << result << "\n";
      result.FullClear();
  }
  return true;
}
//-------------------------------------------------------------------
bool jfGenCat_PickData::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfGenCat_PickData::ReadMiddleFromFile";
  // local variables
  QString cline;
  unsigned long oval;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  validdata = false;
  if (!infile->ReadULong(oval,funcname)) return false;
  item_count = oval;
  // getting the and_filter
  if (!infile->ReadUnEsc(cline,funcname)) return false;
  if (and_filter!=NULL) delete and_filter;
  if (cline.isEmpty()) and_filter=NULL;
  else and_filter = new jfSimpleExpr(cline);
  if ((and_filter!=NULL) && !(and_filter->IsValid())) {
    return infile->BuildError("The AND Category expression does not parse properly!");
  }
  // getting the or_filter
  if (!infile->ReadUnEsc(cline,funcname)) return false;
  if (or_filter!=NULL) delete or_filter;
  if (cline.isEmpty()) or_filter=NULL;
  else or_filter = new jfSimpleExpr(cline);
  if ((or_filter!=NULL) && (!(or_filter->IsValid()))) {
    return infile->BuildError("The OR Category expression does not parse properly!");
  }
  // done
  return true;
}
//-------------------------------------------------------------------
bool jfGenCat_PickData::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfGenCat_PickData::ReadRestFromFile";
  // input data
  size_t iloop;
  // we read in data via a loop
  for (iloop = 0; iloop < item_count ; iloop++) {
    // getting the line
    if (!infile->ReadParseLine(2,funcname)) return false;
    // 2 entries
    sec_name.append(infile->lp.GetT(0));
    cat_name.append(infile->lp.GetT(1));
  }
  // done
  item_count = iloop;
  ResetIndex();
  validdata = true;
  return true;
}
//**************************************************************************
jfGenSection* MakeTypedSection(const QString& intype ) {
  if (intype==AO3_STYPES) return new jfAO3Section();
  else return NULL;
}
//****************************************************************************
jfGen_CatHolder::jfGen_CatHolder() {
  ResetAllIndexes();
  sec_count = 0;
  fbu_cache = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Index Manipulation
//--------------------------------------
void jfGen_CatHolder::ResetIndex(const size_t& which) {
  assert(which<32);
  sec_idxs[which] = -1;
  cat_idxs[which] = -1;
}
//--------------------------------------
bool jfGen_CatHolder::NextIndex(const size_t& which) {
  assert(which<32);
  bool imark;
  while (NextIndexNoSkip(which)) {
    imark = sections[sec_idxs[which]]->GetMarkByIndex(cat_idxs[which],which);
    if (imark) return true;
  }
  return false;
}
//--------------------------------------
int jfGen_CatHolder::GetSindex(const size_t& which) const {
  assert(which<32);
  return sec_idxs[which];
}
//--------------------------------------
int jfGen_CatHolder::GetCindex(const size_t& which) const {
  assert(which<32);
  return cat_idxs[which];
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// category getting
//--------------------------------------
const jfGeneralCategory* jfGen_CatHolder::CatAtIndex(const size_t& which) const {
  assert(which<32);
  if ((cat_idxs[which]==-1) || (cat_idxs[which]==-2)) return NULL;
  return sections[sec_idxs[which]]->ItemAtIndex(cat_idxs[which]);
}
//--------------------------------------
const jfGeneralCategory* jfGen_CatHolder::GetNextCat(const size_t& which) {
  assert(which<32);
  NextIndex(which);
  return CatAtIndex(which);
}
//--------------------------------------
const jfGeneralCategory* jfGen_CatHolder::FindCatFromFinder(const QString& infind) const {
  // special case
  if (sections.size()==0) return NULL;
  // variables
  int floc1,floc2;
  QStringList* the_data;
  the_data = EscListToStrArr(infind,IO_DELIMS);
  // error checking
  if (the_data==NULL) return NULL;
  if (the_data->count()!=2) return NULL;
  // looking
  floc1 = FindSection(the_data->at(0));
  if (floc1<0) {
    delete the_data;
    return NULL;
  }
  // we have found the section, look for the category!
  floc2 = sections[floc1]->FindByName(the_data->at(1));
  delete the_data;
  // found or not?
  if (floc2<0) return NULL;
  else return sections[floc1]->ItemAtIndex(floc2);
}
//--------------------------------------------------------------------------------
const jfGeneralCategory* jfGen_CatHolder::FindByUrl(const QString& infind) const {
  const QString fname = "jfGen_CatHolder::FindByUrl";
  // asserts and variables
  assert(!infind.isEmpty());
  size_t sloop, cloop;
  QString curl;
  // look in the cache
  if (fbu_cache!=NULL) {
    if ((fbu_cache->GetUrl())==infind) return fbu_cache;
  }
  // iterate
  for (sloop=0;sloop<sec_count;sloop++) {
    for (cloop=0;cloop<sections[sloop]->GetCount();cloop++) {
      curl = sections[sloop]->UrlAtIndex(cloop);
      if (curl==infind) {
        fbu_cache = const_cast<jfGeneralCategory*>(sections[sloop]->ItemAtIndex(cloop));
        break;
      }
      else fbu_cache = NULL;
    }
    if (fbu_cache!=NULL) break;
  }
  // done with the loop
  return fbu_cache;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// more category getting
//--------------------------------------
const jfGeneralCategory* jfGen_CatHolder::FindCatByName(const QString& section, const QString& catname) const {
  // special case
  if (sections.size()==0) return NULL;
  // variables
  int floc1,floc2;
  // looking
  floc1 = FindSection(section);
  if (floc1<0) return NULL;
  // we have found the section, look for the category!
  floc2 = sections[floc1]->FindByName(catname);
  // found or not?
  if (floc2<0) return NULL;
  else return sections[floc1]->ItemAtIndex(floc2);
}
//--------------------------------------
const jfGeneralCategory* jfGen_CatHolder::FindCatByID(const QString& section, const size_t& id_to_find) const {
  int spos = FindSection(section);
  if (spos < 0) return NULL;
  else return sections[spos]->LookupByID(id_to_find);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting info
//--------------------------------------
void jfGen_CatHolder::ClearOld() {
  ResetAllIndexes();
  for (size_t sindex=0;sindex<sections.size();sindex++) {
    delete sections[sindex];
    sections[sindex] = NULL;
  }
  sections.clear();
  sec_count = 0;
}
//--------------------------------------
bool jfGen_CatHolder::AddSection(jfGenSection* insection) {
  // basic checks
  if (insection==NULL) return false;
  if (insection->isEmpty()) return false;
  // variables
  int floc;
  // duplicate section names are not allowed
  floc = FindSection(insection->GetName());
  if (floc>=0) return false;
  // the test are passed, we add the section
  sections.push_back(insection);
  sec_count++;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting the marks
bool jfGen_CatHolder::ClearMarks(size_t which) {
  assert(which<32);
  for (size_t xi=0;xi<sections.size();xi++) {
    sections[xi]->MarkAll(which,false);
  }
  return true;
}
//--------------------------------------
size_t jfGen_CatHolder::ApplyCatPicker(jfGenCat_PickData* catpicker, size_t which) {
  const QString fname = "jfGen_CatHolder::ApplyCatPicker";
  // basic checks
  assert(catpicker!=NULL);
  assert(which<32);
  // variables
  int secloc;
  size_t rtotal = 0;
  QString catval;
  bool catinc, result;
  // clearing the old stuff
  ClearMarks(which);
  catpicker->ResetIndex();
  // we do the regular options
  while (catpicker->NextIndex()) {
    secloc = FindSection(catpicker->GetSection());
    if (secloc<0) continue;
    catval = catpicker->GetCatName();
    if (catval=="*") {
      sections[secloc]->MarkAll(which);
      rtotal += sections[secloc]->GetCount();
    }
    else {
      if (sections[secloc]->MarkByName(catval,true,which)) rtotal++;
    }
  }
  // next up, the filters
  if (catpicker->NotEmptyFil()) {
    // we loop over the catgories rather than the search entries this time
    ResetIndex(which);
    while (NextIndexNoSkip(which)) {
      catval = sections[sec_idxs[which]]->NameAtIndex(cat_idxs[which]);
      catinc = sections[sec_idxs[which]]->GetMarkByIndex(cat_idxs[which],which);
      // checking versus the AND filter
      result = catpicker->EvalAndExpr(catval);
      if (catinc && (!result)) {
        sections[sec_idxs[which]]->MarkByIndex(cat_idxs[which],which,false);
        rtotal--;
      }
      // checking versus the OR filter
      result = catpicker->EvalOrExpr(catval);
      if (result && (!catinc)) {
        sections[sec_idxs[which]]->MarkByIndex(cat_idxs[which],which,true);
        rtotal++;
      }
    }
    // done with loop
  }
  // done
  return rtotal;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other special methods
QString jfGen_CatHolder::GetTypeID() const {
  return "Gen_CatHolder";
}
//------------------------------------------------------------------
QStringList* jfGen_CatHolder::GetCatNames(size_t sindex) const {
  assert(sindex<sec_count);
  return sections[sindex]->GetNameList();
}
//-----------------------------------------------------------
QStringList* jfGen_CatHolder::GetSecNames() const {
  QStringList* result;
  result = new QStringList();
  for (size_t sindex=0;sindex<sec_count;sindex++) {
    result->append(sections[sindex]->GetName());
  }
  return result;
}
//-----------------------------------------------------------
size_t jfGen_CatHolder::SCount() const {
  return sec_count;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfGen_CatHolder::~jfGen_CatHolder() {
  ClearOld();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//------------------------------------------------------------
void jfGen_CatHolder::ResetAllIndexes() {
  for (size_t xo=0;xo<32;xo++) {
    sec_idxs[xo] = -1;
    cat_idxs[xo] = -1;
  }
}
//-------------------------------------------------------------
bool jfGen_CatHolder::NextIndexNoSkip(const size_t& which) {
  assert(which<32);
  // special cases
  if (sec_idxs[which]==-2) return false;
  if (sections.empty()) return false;
  // moving onto the first
  if (sec_idxs[which]==-1) {
    sec_idxs[which] = 0;
    cat_idxs[which] = 0;
    return true;
  }
  // we check the category index
  if (cat_idxs[which]==((sections[sec_idxs[which]]->GetCount())-1)) {
    // either we move past the end
    if (sec_idxs[which]==(sections.size()-1)) {
      sec_idxs[which] = -2;
      cat_idxs[which] = -2;
      return false;
    }
    // or move to a new section
    else {
      sec_idxs[which]++;
      cat_idxs[which] = 0;
      return true;
    }
  }
  // the simple case
  else cat_idxs[which]++;
  return true;
}
//--------------------------------------
int jfGen_CatHolder::FindSection(const QString& snamein) const {
  bool found;
  // duplicate section names are not allowed
  for (size_t sindex=0;sindex<sections.size();sindex++) {
    found = ((sections[sindex]->GetName())==snamein);
    if (found) return sindex;
  }
  return -1;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//-----------------------------------------------------
bool jfGen_CatHolder::AddMiddleToFile(QTextStream* outfile) const {
  const QString fname = "jfGen_CatHolder::AddMiddleToFile";
  // the usual checks...
  if (outfile==NULL) return false;
  // writing
  (*outfile) << QString::number(sections.size()) << "\n";
  return true;
}
//--------------------------------------
bool jfGen_CatHolder::AddRestToFile(QTextStream* outfile) const {
  const QString fname = "jfGen_CatHolder::AddRestToFile";
  size_t secindex;
  // the usual checks...
  if (outfile==NULL) return false;
  // writing
  for (secindex=0;secindex<sections.size();secindex++) {
    sections[secindex]->AddToFile(outfile);
  }
  return true;
}
//--------------------------------------
bool jfGen_CatHolder::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfGen_CatHolder::ReadMiddleFromFile";
  unsigned long tval;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadULong(tval,funcname)) return false;
  if (tval==0) return infile->BuildError("The section count is zero!");
  sec_count = tval;
  return true;
}
//--------------------------------------
bool jfGen_CatHolder::ReadRestFromFile(jfFileReader* infile) {
  // constants
  const QString fname = "jfGen_CatHolder::ReadRestFromFile";
  // local variables
  size_t loopc;
  QString typid;
  jfGenSection* newitem;
  // the usual initial checks
  assert(infile!=NULL);
  JDEBUGLOG(fname,1)
  // the loop
  for (loopc=0;loopc<sec_count;loopc++) {
    // gets the filter type string (and also the line index)
    if (!infile->GetType(typid,fname)) return false;
    // we make an empty default filter
    newitem = MakeTypedSection(typid);
    if (newitem==NULL) return infile->BuildError("Cannot make section of type "+ typid);
    // and then load
    if (!newitem->GetFromFile(infile)) {
      delete newitem;
      return false;
    }
    // we add the newly loaded item to the list
    sections.push_back(newitem);
  }
  // done
  return true;
}
//*****************************************************************************
