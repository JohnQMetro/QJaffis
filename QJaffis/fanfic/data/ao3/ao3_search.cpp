/******************************************************************************
Name    :   ao3_search.cpp
Author  :   John Q Metro
Purpose :   Defines search object of AO3
Created :   August 28, 2012
Conversion to Qt Started March 29, 2014
Updated :   June 20, 2017 (added English-only option)
******************************************************************************/
#ifndef AO3_SEARCH_H_INCLUDED
  #include "ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED
//-----------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef AO3_CONSTS_H_INCLUDED
  #include "ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED
//-----------------------------------
#include <assert.h>
/*****************************************************************************/
// constructors
//-----------------------------------------------------
// default constructor, please call SetData before using
jfAO3Search::jfAO3Search():jfSearchCore() {
  cat_data = NULL;
  curr_cat = NULL;
  selector = new jfGenCat_PickData();
  validdata = false;
  pagelimit = 100000;
  numscats = 0;
  icatindex = -1;
  rindex = sindex = windex = 0;
  conly = false;
  eng_only = false;
  ord_index = 0;
}
//-----------------------------------------------------
jfAO3Search::jfAO3Search(jfGen_CatHolder* cat_datain, size_t search_indexin):jfSearchCore() {
  pagelimit = 100000;
  selector = NULL;
  SetData(cat_datain,search_indexin);
  rindex = sindex = windex = 0;
  conly = false;
  ord_index = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
bool jfAO3Search::SetData(jfGen_CatHolder* cat_datain, size_t search_indexin) {
  const QString fname = "jfAO3Search::SetData";
  // we do some checking...
  assert(search_indexin<32);
  assert(cat_datain!=NULL);
  // assigning some values
  cat_data = cat_datain;
  search_index = search_indexin;
  if (selector==NULL) selector = new jfGenCat_PickData();
  numscats = 0;
  icatindex = -1;
  // indexing..
  curr_cat = NULL;
  // done
  validdata = true;
  return true;
}
//--------------------------------------------------
// called when launching a search, we apply the selction to the category data and get an index
void jfAO3Search::ApplySelection() {
  // the function name
  const QString fname = "jfAO3Search::ApplySelection";
  // we start checking
  assert(validdata);
  assert(cat_data!=NULL);
  // applying the item
  numscats = cat_data->ApplyCatPicker(selector,search_index);
  cat_data->ResetIndex(search_index);
  curr_cat = NULL;
}
//----------------------------------------------------
void jfAO3Search::SetSearchOptions(size_t irindex, size_t isindex,
          size_t iwindex, QString iintag, bool iconly) {
  // assigning the values
  rindex = irindex;
  sindex = isindex;
  windex = iwindex;
  intag = iintag;
  conly = iconly;
}
//-----------------------------------------------------
void jfAO3Search::SetOrder(size_t in_oindex) {
  assert(in_oindex<ao3con::order_count);
  ord_index = in_oindex;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
jfGenCat_PickData* jfAO3Search::GetSelector() {
  return selector;
}
//----------------------------------------------------
jfGen_CatHolder* jfAO3Search::GetHolder() {
  return cat_data;
}
//---------------------------------------------------
size_t jfAO3Search::GetSSIndex() const {
  return search_index;
}
//---------------------------------------------------
size_t jfAO3Search::GetSearchValue(size_t qindex) const {
  assert(qindex<4);
  if (qindex==0) return rindex;
  if (qindex==1) return sindex;
  if (qindex==2) return windex;
  return ord_index;
}
//---------------------------------------------------
QString jfAO3Search::GetExtraSTag() const {
  return intag;
}
//---------------------------------------------------
bool jfAO3Search::GetCompletedValue() const {
  return conly;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented method for sending category data
void jfAO3Search::DispatchCategory() {
  // variables and constants
  const QString fname="jfAO3Search::DispatchCategory";
  struct jfCategoryInfoMsg ccatdata;
  // doing
  ccatdata.catheader = GetCatHeader();
  ccatdata.catname = curr_cat->GetName();
  emit SendCatInfo(ccatdata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemeted virtual methods
//-----------------------------------------------------
QString jfAO3Search::GetTypeID() const {
  return "AO3Search";
}
//-----------------------------------------------------
// the name of the current category
QString jfAO3Search::GetCatName() const {
  // no valid category to get a name from
  if (curr_cat==NULL) return "";
  else return curr_cat->GetName();
}
//-----------------------------------------------------
/* HTML header for display in the result list */
QString jfAO3Search::GetCatHeader() const {
  QString result;
  // building the title line
  result += "<font size=+2><b>Category : <a href=\"";
  result += GetIndexUrl(1) + "\">";
  result += GetCatName() + "</a></b></font><br>\n";
  // next line: link display
  result += "<font color=\"green\">";
  result += GetIndexUrl(1) + "</font><br>\n";
  // we should perhaps add info on the number of results...
  // done
  return result;
}
//-----------------------------------------------------
// pages start at 1
QString jfAO3Search::GetIndexUrl(size_t inp_index) const {
  const QString fname = "jfAO3Search::GetIndexUrl";
  QString result;
  assert(inp_index>=1);
  // return an empty string if there is no valid category
  if (curr_cat==NULL) return "";
  else {
    result = ao3con::url_start;
    if (inp_index>1) result += "&page=" + QString::number(inp_index);
    result += "&work_search[sort_column]=";
    result += ao3con::order_ids[ord_index];
    result += ao3con::MakeUrlPart(rindex, sindex, windex, intag, conly, eng_only);
    result += "&tag_id=";
    result += ao3con::AO3Encode(curr_cat->GetName());
    return result;
  }
}
//-----------------------------------------------------
QString jfAO3Search::GetHTMLHeader(size_t result_category) const {
  QString result;
  // the top line, with category name and link
  result = "<h1 class=\"maintittle\">Category: ";
  result += "<a href=\"";
  result += GetIndexUrl(1) + "\">";
  result += curr_cat->GetName();
  result += "</a></h1>\n";
  // the next line gives the result category specifier
  result += "<h2 class=\"res_categ_header\">Result Category : ";
  if (result_category == 32) result += categories->GetName();
  else result += categories->NameAtIndex(result_category);
  result += "</h2>";
  return result;
}
//-----------------------------------------------------
void jfAO3Search::MakeLocalSkeleton(jfSkeletonBase* src) {
  jfSkeletonMultiple* temp, *newval;
  // asserts and checks
  assert(src!=NULL);
  assert((src->GetSkelType())==jfs_MULTIPLE);
  // we start
  temp = dynamic_cast<jfSkeletonMultiple*>(src);
  newval = new jfSkeletonMultiple(temp);
  // clearing
  if (local_skel) ClearLocalSkeleton();
  skel_link = newval;
  local_skel = true;
}
//-----------------------------------------------------
void jfAO3Search::LoadValues(jfSkeletonParser* inparser, size_t result_categ) {
  LoadCoreValues(inparser,result_categ);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// source category indexing
//-----------------------------------------------------
void jfAO3Search::ResetIndex() {
  cat_data->ResetIndex(search_index);
  curr_cat = NULL;
  icatindex = -1;
}
//-----------------------------------------------------
bool jfAO3Search::NextIndex() {
  // the function name
  const QString fname = "jfAO3Search::NextIndex";
  bool rvalue;
  assert(cat_data!=NULL);
  rvalue = cat_data->NextIndex(search_index);
  if (!rvalue) {
    curr_cat = NULL;
    icatindex = -2;
  }
  else {
    curr_cat = dynamic_cast<const jfAO3_Category*>(cat_data->CatAtIndex(search_index));
    icatindex++;
  }
  return rvalue;
}
//------------------------------------------------------
bool jfAO3Search::LastCat() const {
  return (icatindex==(numscats-1));
}
//------------------------------------------------------
size_t jfAO3Search::GetNumCats() const {
  return numscats;
}
//-----------------------------------------------------
void jfAO3Search::SetCurrCat(const jfAO3_Category* inval) {
  assert(inval!=NULL);
  curr_cat = inval;
}
//-------------------------------------------------------
const jfAO3_Category* jfAO3Search::GetCurrCat() const {
  return curr_cat;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfAO3Search::~jfAO3Search() {
  if (selector!=NULL) delete selector;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o methods for fanfiction.net specific data
//-----------------------------------------------------
bool jfAO3Search::AddMiddleToFile(QTextStream* outfile) const {
  // local variables
  jfOutString xline;
  // checking and special conditions
  if (outfile==NULL) return false;
  // search options
  xline << rindex << sindex << windex << intag << conly << ord_index << eng_only;
  (*outfile) << xline << "\n";
  // the only thing we really need to add is the selector
  assert(selector!=NULL);
  return selector->AddToFile(outfile);
}
//-----------------------------------------------------
bool jfAO3Search::ReadMiddleFromFile(jfFileReader* infile) {
  const QString fname = "jfAO3Search::ReadMiddleFromFile";
  QString xoerr;
  // reading the search options
  if (!infile->ReadParseLine(6,7,fname)) return false;
  if (!infile->lp.SBoundVal(0,ao3con::rat_count,rindex,xoerr)) {
          return infile->BuildError("The rating value "+xoerr); }
  if (!infile->lp.SBoundVal(1,ao3con::soc_count,sindex,xoerr)) {
          return infile->BuildError("The sexual orientation value "+xoerr); }
  if (!infile->lp.SBoundVal(2,ao3con::warn_count,windex,xoerr)) {
          return infile->BuildError("The warning value "+xoerr); }
  intag = (infile->lp).UnEscStr(3);
  if (!infile->lp.BoolVal(4,conly)) return infile->BuildError("The complete flag is not boolean!");
  if (!infile->lp.SBoundVal(5,ao3con::order_count,ord_index,xoerr)) {
          return infile->BuildError("The ordervalue "+xoerr); }
  // english only
  if ((infile->lp.Num()) == 7) {
      if (!infile->lp.BoolVal(6,eng_only)) return infile->BuildError("The english only flag is not boolean!");
  }
  else eng_only = false;
  // next, the selector
  if (selector==NULL) selector = new jfGenCat_PickData();
  assert(infile!=NULL);
  return selector->GetFromFile(infile);
}
/*****************************************************************************/
