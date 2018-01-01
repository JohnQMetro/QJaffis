/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_catpdata.cpp
// Author :     John Q Metro
// Purpose :    Holding data on which categories have been selected for searching
// Created:     May 10, 2010
// Conversion to Qt Started June 14, 2014
// Updated:     August 26, 2014
/////////////////////////////////////////////////////////////////////////////
#ifndef FFNCATPDATA
  #include "ffn_catpdata.h"
#endif

#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef CATGROUP_H
  #include "catgroup.h"
#endif // CATGROUP_H
//-------------------------------------
#include <assert.h>
//****************************************************************************
jfFFN_CatP_SelCat::jfFFN_CatP_SelCat():jfBaseObj(0,"FFN Category Pick Data") {
  item_count = 0;
  item_index = -1;
  and_filter = NULL;
  or_filter = NULL;
  catdatlink = NULL;
}
//-----------------------------------------------------------------------
jfFFN_CatP_SelCat::jfFFN_CatP_SelCat(jfFFN_Categories* incatdatlink):jfBaseObj(0,"FFN Category Pick Data") {
  item_count = 0;
  item_index = -1;
  and_filter = NULL;
  or_filter = NULL;
  catdatlink = incatdatlink;
  assert(catdatlink!=NULL);
}
//-----------------------------------------------------------------------
bool jfFFN_CatP_SelCat::SetCatLink(jfFFN_Categories* incatdatlink) {
  if (incatdatlink==NULL) return false;
  catdatlink = incatdatlink;
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// index methods
//--------------------------------------------------------
void jfFFN_CatP_SelCat::ResetIndex() {
  item_index = -1;
}
//--------------------------------------------------------
bool jfFFN_CatP_SelCat::NextIndex() {
  if (item_index==-2) return false;
  int csize = cs_name.count();
  if (csize == (item_index+1)) {
    item_index = -2;
    return false;
  }
  // the normal case
  item_index++;
  return true;
}
//--------------------------------------------------------
bool jfFFN_CatP_SelCat::BackIndex() {
  if (item_index==-1) return false;
  int csize = cs_name.count();
  if ((csize == 0) || (item_index==0)) {
    item_index = -1;
    return false;
  }
  // the normal case
  item_index--;
  return true;
}
//--------------------------------------------------------
bool jfFFN_CatP_SelCat::ToIndex(size_t toindex) {
  if (toindex<-2) return false;
  int csize = cs_name.count();
  if (toindex>=csize) return false;
  item_index = toindex;
  return true;
}
//--------------------------------------------------------
int jfFFN_CatP_SelCat::WhichIndex() const {
  return item_index;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info at the current index
//--------------------------------------------------------
bool jfFFN_CatP_SelCat::IsCrossover() const {
  if (item_index<0) return false;
  return (cs_name[item_index]!="");
}
//--------------------------------------------------------
QString jfFFN_CatP_SelCat::GetCrossSection() const {
  if (item_index<0) return "";
  return cs_name[item_index];
}
//--------------------------------------------------------
QString jfFFN_CatP_SelCat::GetGroupSection() const {
  if (item_index<0) return "";
  return gs_name[item_index];
}
//--------------------------------------------------------
QString jfFFN_CatP_SelCat::GetCatName() const {
  if (item_index<0) return "";
  return cat_name[item_index];
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
QStringList* jfFFN_CatP_SelCat::GetListing() const {
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
bool jfFFN_CatP_SelCat::SetAndFilter(jfSimpleExpr* in_value) {
  if (in_value==NULL) return false;
  if (!(in_value->IsValid())) return false;
  and_filter = in_value;
  return true;
}
//-----------------------------------------------------------------------------
bool jfFFN_CatP_SelCat::SetOrFilter(jfSimpleExpr* in_value) {
  if (in_value==NULL) return false;
  if (!(in_value->IsValid())) return false;
  or_filter = in_value;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFN_CatP_SelCat::FiltersNotEmpty() const {
  if ((and_filter!=NULL) && (!and_filter->isEmpty())) return true;
  return ((or_filter!=NULL) && (!or_filter->isEmpty()));
}
//---------------------------------------------------
QString jfFFN_CatP_SelCat::GetAnd() const {
  if (and_filter==NULL) return "";
  else return and_filter->GetSrc();
}
//-----------------------------------
const jfSimpleExpr* jfFFN_CatP_SelCat::GetAndFilter() const {
  return and_filter;
}
//-----------------------------------
QString jfFFN_CatP_SelCat::GetOr() const {
  if (or_filter==NULL) return "";
  else return or_filter->GetSrc();
}
//-----------------------------------
const jfSimpleExpr* jfFFN_CatP_SelCat::GetOrFilter() const {
  return or_filter;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// evaluating the filters
//-------------------------------------------------------------------------
bool jfFFN_CatP_SelCat::NotEmptyFil() const {
  if ((and_filter!=NULL) && (!and_filter->isEmpty())) return true;
  if ((or_filter!=NULL) && (!or_filter->isEmpty())) return true;
  return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// adding data
//--------------------------------------------------------
void jfFFN_CatP_SelCat::AddSection(const QString& secname, bool duplicheck) {
  size_t notused;
  if (!(duplicheck && FindItem(notused,"",secname,"*"))) {
    RemoveSection(secname);
    Append3("",secname,"*");
  }
}
//--------------------------------------------------------
void jfFFN_CatP_SelCat::AddCategory(const QString& secname, const QString& catname, bool duplicheck) {
  size_t notused;
  if (!(duplicheck && FindItem(notused,"",secname,catname))) {
    Append3("",secname,catname);
  }
}
//--------------------------------------------------------
void jfFFN_CatP_SelCat::AddCrossoverSection(const QString& secname, bool duplicheck) {
  size_t notused;
  if (!(duplicheck && FindItem(notused,secname,"*","*"))) {
    RemoveCrossoverSection(secname);
    Append3(secname,"*","*");
  }
}
//--------------------------------------------------------
void jfFFN_CatP_SelCat::AddCrossoverGroup(const QString& secname, const QString& groupname, bool duplicheck) {
  size_t notused;
  if (!(duplicheck && FindItem(notused,secname,groupname,"*"))) {
    RemoveCrossoverGroup(secname,groupname);
    Append3(secname,groupname,"*");
  }
}
//--------------------------------------------------------
void jfFFN_CatP_SelCat::AddCrossoverCategory(const QString& secname, const QString& groupname, const QString& catname, bool duplicheck) {
  size_t notused;
  if (!(duplicheck && FindItem(notused,secname,groupname,catname))) {
    Append3(secname,groupname,catname);
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// removing data
//--------------------------------------------------------
// resmoves a section (including any individual categories from that section) from the search list
bool jfFFN_CatP_SelCat::RemoveSection(const QString& secname) {
  const QString fname="jfFFN_CatP_SelCat::RemoveSection";
  bool result = false;
  ResetIndex();
  // we iterate over the stored data. removing anything which belongs to the section
  /**/JDEBUGLOG(fname,1);
  while (NextIndex()) {
    /**/JDEBUGLOG(fname,2);
    if (!IsCrossover()) {
      /**/JDEBUGLOG(fname,3);
      if (GetGroupSection()==secname) {
        /**/JDEBUGLOGI(fname,4,item_index);
        RemoveAtIndex(item_index);
        item_index--;
        result = true;
      }
    }
  }
  return result;
}
//--------------------------------------------------------
// removes a specific category
bool jfFFN_CatP_SelCat::RemoveCategory(const QString& secname, const QString& catname) {
  size_t locx;
  bool rval = FindItem(locx,"",secname,catname);
  if (rval) {
    RemoveAtIndex(locx);
    ResetIndex();
  }
  return rval;
}
//--------------------------------------------------------
bool jfFFN_CatP_SelCat::RemoveCrossoverSection(const QString& secname) {
  bool result = false;
  ResetIndex();
  // we iterate over the stored data. removing anything which belongs to the section
  while (NextIndex()) {
    if (IsCrossover()) {
      if (GetCrossSection()==secname) {
        RemoveAtIndex(item_index);
        item_index--;
        result = true;
      }
    }
  }
  return result;
}
//--------------------------------------------------------
bool jfFFN_CatP_SelCat::RemoveCrossoverGroup(const QString& secname, const QString& groupname) {
  bool result = false;
  ResetIndex();
  // we iterate over the stored data. removing anything which belongs to the section
  while (NextIndex()) {
    if (IsCrossover()) {
      if ((GetCrossSection()==secname) && (GetGroupSection()==groupname)) {
        RemoveAtIndex(item_index);
        item_index--;
        result = true;
      }
    }
  }
  return result;
}
//--------------------------------------------------------
bool jfFFN_CatP_SelCat::RemoveCrossoverCategory(const QString& secname, const QString& groupname, const QString& catname) {
  size_t locx;
  bool rval = FindItem(locx,secname,groupname,catname);
  if (rval) {
    RemoveAtIndex(locx);
    ResetIndex();
  }
  return rval;
}
//--------------------------------------------------------------------------
// removes a search, but one size fits all here... except we only remove specific searches, not whole sections or such
bool jfFFN_CatP_SelCat::RemoveByString(const QString& inval) {
  // constants
  const QString fname = "jfFFN_CatP_SelCat::RemoveByString";
  // local variables
  jfLineParse* parser;
  size_t floc;
  bool fval;
  // parsing the input
  /**/JDEBUGLOGS(fname,1,inval)
  parser = new jfLineParse(inval);
  // special case faliures: the input is invalid
  if ((parser->Num())<2) {
    delete parser;
    /**/JDEBUGLOG(fname,2);
    return false;
  }
  if ((parser->Num())>3) {
    /**/JDEBUGLOG(fname,3);
    delete parser;
    return false;
  }
  /**/JDEBUGLOGST(fname,4,parser->Num())
    // removing
  if (parser->Num()==2) fval=FindItem(floc,"",parser->GetT(0),parser->GetT(1));
    else fval = FindItem(floc,parser->GetT(0),parser->GetT(1),parser->GetT(2));
    /**/JDEBUGLOGST(fname,5,floc)
    // next up..
    delete parser;
    if (!fval) return false;
    return RemoveAtIndex(floc);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Looks for categories within the specified group/section. Retuns NULL if nothing is
found, an empty result if we're supposed to search all of them, or a list of the
specific categories to search */
QStringList* jfFFN_CatP_SelCat::FindSection(const QString& inname) const {
  // local variables
  QStringList* result;
  size_t loopc;
  // we start
  result = NULL;
  for (loopc=0; loopc<item_count; loopc++) {
    if ((cs_name[loopc]=="") && (gs_name[loopc]==inname)) {
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
//--------------------------------------------------------------------------
jfStrBoolList* jfFFN_CatP_SelCat::FindSectionCross(const QString& sec1, const QString& cat1) const {
  const QString fname = "jfFFN_CatP_SelCat::FindSectionCross";
  // local variables
  QString thirdlook, thirdcombo;
  jfStrBoolList* result;
  size_t loopc, lxc;
  // checks
  if ((sec1.isEmpty()) || (cat1.isEmpty())) return NULL;
  // starting
  result = new jfStrBoolList();
  thirdlook = cat1 + " (" + sec1 + ")";
  // the check loop
  for (loopc=0; loopc<item_count; loopc++) {
    thirdcombo = gs_name[loopc] + " (" + cs_name[loopc] + ")";
    if ((cs_name[loopc]==sec1)&&(gs_name[loopc]==cat1)) {
      if (cat_name[loopc]=="*") {
        result->clear();
        return result;
      }
      result->AddItem(cat_name[loopc],false);
    }
    else if (cat_name[loopc]==thirdlook) {
      result->AddItem(thirdcombo,false);
    }
    else if (cat_name[loopc]=="*") {
      if (gs_name[loopc]=="*") {
        lxc = catdatlink->AppendSection(sec1,cat1,cs_name[loopc],result);
      }
      else {
        // need to stick in an external link check
        if (catdatlink->ContainsCat(sec1,cat1,cs_name[loopc],gs_name[loopc])) {
          result->AddItem(thirdcombo,true);
        }
      }
    }
  }
  if (result->size()==0) {
    delete result;
    result = NULL;
  }
  return result;
}
//---------------------------------------------------------------------------
/* like find section, except the input is a crossover section and the output
names are for crossover groups */
QStringList* jfFFN_CatP_SelCat::FindCrossSection(const QString& inname, bool incl_part) const {
  // local variables
  QStringList* result;
  size_t loopc;
  // we start
  result = NULL;
  for (loopc=0; loopc<item_count; loopc++) {
    if (cs_name[loopc]==inname) {
      if (result==NULL) result = new QStringList;
      if (gs_name[loopc]=="*") {
        assert(result->count()==0);
        return result;
      }
      if (incl_part) result->append(gs_name[loopc]);
      else if (cat_name[loopc]=="*") result->append(gs_name[loopc]);
    }
  }
  if ((result!=NULL) && (result->count()==0)) {
    delete result;
    return NULL;
  }
  return result;
}
//-----------------------------------------------------------------
// returns a list of sections where all crossovers are checked
QStringList* jfFFN_CatP_SelCat::FindCheckedCrossoverSections() const {
  // local variables
  QStringList* result;
  size_t loopc;
  QString sname;
  // starting
  result = new QStringList();
  for (loopc = 0 ; loopc<item_count; loopc++) {
    if (gs_name[loopc]=="*") result->append(cs_name[loopc]);
  }
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
QString jfFFN_CatP_SelCat::GetTypeID() const {
  return "CategoryPickerResults";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFN_CatP_SelCat::~jfFFN_CatP_SelCat() {
  if (and_filter!=NULL) delete and_filter;
  if (or_filter!=NULL) delete or_filter;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// private helper methods
//--------------------------------------------------------
// removes the search at the specified index
bool jfFFN_CatP_SelCat::RemoveAtIndex(size_t inindex) {
  if ((inindex<0) || (inindex>=item_count)) return false;
  cs_name.removeAt(inindex);
  gs_name.removeAt(inindex);
  cat_name.removeAt(inindex);
  item_count--;
  return true;
}
//--------------------------------------------------------
// adds a search
void jfFFN_CatP_SelCat::Append3(QString first, QString second, QString third) {
  cs_name.append(first);
  gs_name.append(second);
  cat_name.append(third);
  item_count++;
}
//---------------------------------------------------------------
bool jfFFN_CatP_SelCat::ThreeEq(int index, QString first, QString second, QString third) {
  const QString fname = "jfFFN_CatP_SelCat::ThreeEq";
  if (cs_name[index]!=first) return false;
  if (gs_name[index]!=second) return false;
  if (cat_name[index]!=third) return false;
  return true;
}

//---------------------------------------------------------------
// returns true if the search at the index is equal to the parameters
bool jfFFN_CatP_SelCat::Equal3(int index, QString first, QString second, QString third) {
  const QString fname = "jfFFN_CatP_SelCat::Equal3";
  if ((index<0) || (index>=item_count)) return false;
  // non crossover mode
  if (first.isEmpty()) return ThreeEq(index,first,second,third);
  // crossover mode
  else {
    // there are at least 2 ways to represent a crossover
    QString alt_sec1, alt_cat1, alt_sec2, alt_cat2, build2;
    alt_sec2 = first;
    alt_cat2 = second;
    bool xres = SplitCross(third,alt_sec1,alt_cat1);
    assert(xres);
    build2 = alt_cat2 + " (" + alt_sec2 + ")";
    // looking for option1
    if (ThreeEq(index,first,second,third)) return true;
    else return ThreeEq(index,alt_sec1,alt_cat1,build2);
  }
}
//-------------------------------------------------------------------
// looks for a search
bool jfFFN_CatP_SelCat::FindItem(size_t& outloc, QString first, QString second, QString third) {
  const QString fname="jfFFN_CatP_SelCat::FindItem";
  /**/JDEBUGLOGST(fname,1,item_count);
  /**/JDEBUGLOGS(fname,2, first);
  /**/JDEBUGLOGS(fname,3,second);
  /**/JDEBUGLOGS(fname,4, third);
  for (size_t loopc=0; loopc<item_count; loopc++) {
    /**/JDEBUGLOG(fname,5);
    if (Equal3(loopc,first,second,third)) {
      outloc = loopc;
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
QString jfFFN_CatP_SelCat::Item2Str(size_t which) const {
  QString result = "";
  if (which>=item_count) return "";
  if (cs_name[which]!="") result = cs_name[which] + " ; ";
  result += gs_name[which] + " ; ";
  result += cat_name[which];
  return result;
}
//-------------------------------------------------------------------------
// Takes a string with format "category name (section name)" and splits it
bool jfFFN_CatP_SelCat::SplitCross(QString input, QString& sec_out, QString& cat_out) const {
  // variables
  int lpos1, lpos2;
  // loking for )
  lpos1 = input.lastIndexOf(')');
  if (lpos1<0) return false;
  // looking for (
  lpos2 = input.lastIndexOf('(',lpos1);
  if (lpos2<0) return false;
  // extracting
  sec_out = input.mid(lpos2+1,lpos1-lpos2-1);
  cat_out = input.left(lpos2);
  // post processing
  sec_out = sec_out.trimmed();
  if (sec_out.isEmpty()) return false;
  cat_out = cat_out.trimmed();
  return (!cat_out.isEmpty());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// private implemented virtual methods
//-------------------------------------------------------------------
bool jfFFN_CatP_SelCat::AddMiddleToFile(QTextStream* outfile) const {
  QString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding the number of items
  result = QString::number(item_count);
  (*outfile) << result << "\n";
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
bool jfFFN_CatP_SelCat::AddRestToFile(QTextStream* outfile) const {
  QString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // the main loop
  for (size_t loopc=0; loopc<item_count; loopc++) {
    result = Item2Str(loopc);
    (*outfile) << result << "\n";
  }
  return true;
}
//-------------------------------------------------------------------
bool jfFFN_CatP_SelCat::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_CatP_SelCat::ReadMiddleFromFile";
  // local variables
  QString cline;
  unsigned long oval;
  // starting checks (and reading the line)
  if (!infile->ReadULong(oval,funcname)) return false;
  item_count = oval;
  // getting the and_filter
  if (!infile->ReadUnEsc(cline,funcname)) return false;
  if (and_filter!=NULL) delete and_filter;
  if (cline.isEmpty()) and_filter=NULL;
  else and_filter = new jfSimpleExpr(cline);
  if ((and_filter!=NULL) && !(and_filter->IsValid())) {
    validdata = false;
    return infile->BuildError("The AND Category expression does not parse properly!");
  }
  // getting the or_filter
  if (!infile->ReadUnEsc(cline,funcname)) return false;
  if (or_filter!=NULL) delete or_filter;
  if (cline.isEmpty()) or_filter=NULL;
  else or_filter = new jfSimpleExpr(cline);
  if ((or_filter!=NULL) && (!(or_filter->IsValid()))) {
    validdata = false;
    return infile->BuildError("The OR Category expression does not parse properly!");
  }
  // done
  return true;
}
//-------------------------------------------------------------------
bool jfFFN_CatP_SelCat::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_CatP_SelCat::ReadRestFromFile";
  // input data
  QString cline, buffer;
  size_t iloop;
  size_t target_count;
  // starting
  target_count = item_count;
  item_count = 0;
  /**/JDEBUGLOGST(funcname,1,item_count);
  // we read in data via a loop
  for (iloop = 0; iloop < target_count ; iloop++) {
    // getting the line
    /**/JDEBUGLOGST(funcname,3,iloop);
    // parsing it
    if (!infile->ReadParseLine(2,3,funcname)) return false;
    // 2 entries
    if ((infile->lp.Num())==2) {
      Append3("",infile->lp.GetT(0),infile->lp.GetT(1));
    }
    // 3 entries
    else {
      Append3(infile->lp.GetT(0),infile->lp.GetT(1),infile->lp.GetT(2));
    }
  }
  // done
  item_count = iloop;
  ResetIndex();
  validdata = true;
  return true;
}
//****************************************************************************
