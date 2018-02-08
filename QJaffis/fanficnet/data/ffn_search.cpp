//***************************************************************************
// Name:        ffn_search.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net search
// Created:     June 9, 2010
// Conversion to Qt Started July 10, 2014
// Updated:     February 8, 2018 (exclude genres)
//***************************************************************************
#ifndef JF_FFNSEARCH
  #include "ffn_search.h"
#endif
//------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FFN_LINKDATA_H_INCLUDED
  #include "ffn_linkdata.h"
#endif // FFN_LINKDATA_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//------------------------------------
#include <assert.h>

//***************************************************************************
// constructors
//-----------------------------------------------------
// default constructor, please call SetData before using
jfFFNSearch::jfFFNSearch():jfSearchCore() {
  cat_data = NULL;
  curr_cat = NULL;
  selector = NULL;
  validdata = false;
  list_to_search = NULL;
  pagelimit = 100000;
  sv[0] = 10; sv[1] = 0; sv[2] = 0; sv[3] = 0; sv[4] = 0;
  sv[5] = 0;
}
//-----------------------------------------------------
jfFFNSearch::jfFFNSearch(jfFFN_Categories* cat_datain, size_t search_indexin):jfSearchCore() {
  list_to_search = NULL;
  pagelimit = 100000;
  selector = NULL;
  SetData(cat_datain,search_indexin);
  sv[0] = 10; sv[1] = 0; sv[2] = 0; sv[3] = 0; sv[4] = 0;
  sv[5] = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
bool jfFFNSearch::SetData(jfFFN_Categories* cat_datain, size_t search_indexin) {
  // we do some checking...
  assert(search_indexin<32);
  assert(cat_datain!=NULL);
  // assigning some values
  cat_data = cat_datain;
  search_index = search_indexin;
  if (selector==NULL) selector = new jfFFN_CatP_SelCat(cat_data);
  // indexing..
  if (list_to_search!=NULL) delete list_to_search;
  list_to_search = NULL;
  curr_cat = NULL;
  // done
  validdata = true;
  return true;

}
//--------------------------------------------------
// called when launching a search, we apply the selction to the category data and get an index
void jfFFNSearch::ApplySelection() {
  // the function name
  const QString fname = "jfFFNSearch::ApplySelection";
  // variables
  size_t ccountres;
  // bool xres;
  // we start checking
  /**/JDEBUGLOG(fname,1);
  assert(validdata);
  assert(cat_data!=NULL);
  /**/JDEBUGLOGST(fname,2,search_index);
  // applying the item
  if (list_to_search!=NULL) {
    list_to_search->ClearMarks(search_index);
    delete list_to_search;
    list_to_search = NULL;
  }
  list_to_search = cat_data->MakeSearchList(selector,search_index);
  assert(list_to_search!=NULL);
  catdex = -1;
  /**/JDEBUGLOG(fname,3);
  ccountres = list_to_search->size();
  /**/JDEBUGLOGST(fname,4,ccountres);
  // xres = WriteListOfCats("catlist.txt");
  curr_cat = NULL;
}
//-----------------------------------------------------------
/* sets the search values. The inputs are indexes into the values list.
We use that to look up the id values that are stored */
bool jfFFNSearch::SetSearchValues(size_t frin, size_t genin, size_t wcin, size_t lanin, size_t exlgenin, size_t compin) {
  // out of bounds checks
  if (frin>=ffn_consts::frcount) return false;
  if (genin>=ffn_consts::gencount) return false;
  if (exlgenin>=ffn_consts::gencount) return false;
  if (wcin>=ffn_consts::wccount) return false;
  if (lanin>=ffn_consts::lancount) return false;
  if (compin>=3) return false;
  // within bounds, we lookup
  sv[0] = ffn_consts::fr_idlist[frin];
  sv[1] = ffn_consts::gen_idlist[genin];
  sv[2] = ffn_consts::wc_idlist[wcin];
  sv[3] = ffn_consts::lan_idlist[lanin];
  sv[4] = compin;
  sv[5] = ffn_consts::gen_idlist[exlgenin];
  // done
  return true;
}
//-------------------------------------------------------
// translates id values to list indexes when getting the result
int jfFFNSearch::GetSearchValue(size_t pindex) const {
  if (pindex> 5) return -1;
  size_t revval;
  if (ffn_consts::ReverseLookup(pindex,sv[pindex],revval)) return revval;
  else return -1;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
jfFFN_CatP_SelCat* jfFFNSearch::GetSelector() {
  return selector;
}
//----------------------------------------------------
jfFFN_Categories* jfFFNSearch::GetHolder() {
  return cat_data;
}
//---------------------------------------------------
size_t jfFFNSearch::GetSSIndex() const {
  return search_index;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented method for sending category data
void jfFFNSearch::DispatchCategory() {
  // variables and constants
  const QString fname="jfFFNSearch::DispatchCategory";
  struct jfCategoryInfoMsg ccatdata;
  // doing
  ccatdata.catheader = GetCatHeader();
  ccatdata.catname = curr_cat->GetName();
  emit SendCatInfo(ccatdata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemeted virtual methods
//-----------------------------------------------------
QString jfFFNSearch::GetTypeID() const {
  return "FFNSearch";
}
//-----------------------------------------------------
// the name of the current category
QString jfFFNSearch::GetCatName() const {
  // no valid category to get a name from
  if (curr_cat==NULL) return "";
  else return curr_cat->GetName();
}
//-----------------------------------------------------
/* HTML header for display in the result list */
QString jfFFNSearch::GetCatHeader() const {
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
QString jfFFNSearch::GetIndexUrl(size_t inp_index) const {
  // constants and variables
  const QString fname = "jfFFNSearch::GetIndexUrl";
  QString result;
  // checks
  assert(inp_index>=1);
  // return an empty string if there is no valid category
  if (curr_cat==NULL) return "";
  else {
    result = curr_cat->GetUrl();
    result += "?&srt=2";
    result += ffn_consts::MakeURLPart(sv);
    result += "&p=" + QString::number(inp_index);
    /**/JDEBUGLOGS(fname,1,result)
    return result;
  }
}
//-----------------------------------------------------
QString jfFFNSearch::GetHTMLHeader(size_t result_category) const {
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
void jfFFNSearch::MakeLocalSkeleton(jfSkeletonBase* src) {
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
void jfFFNSearch::LoadValues(jfSkeletonParser* inparser, size_t result_categ) {
  LoadCoreValues(inparser,result_categ);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// source category indexing
//-----------------------------------------------------
void jfFFNSearch::ResetIndex() {
  // the function name
  catdex = -1;
  curr_cat = NULL;
}
//-----------------------------------------------------
bool jfFFNSearch::NextIndex() {
  // bad cases
  if (list_to_search==NULL) return false;
  if (catdex==(list_to_search->size())) return false;
  // other cases
  catdex++;
  if (catdex==(list_to_search->size())) {
    curr_cat = NULL;
    return false;
  }
  else {
    curr_cat = (*list_to_search)[catdex];
    return true;
  }
}
//------------------------------------------------------
bool jfFFNSearch::LastCat() const {
  assert(list_to_search!=NULL);
  return (catdex==((list_to_search->size())-1));
}
//------------------------------------------------------
size_t jfFFNSearch::GetNumCats() const {
  assert(list_to_search!=NULL);
  return (list_to_search->size());
}
//-----------------------------------------------------
void jfFFNSearch::SetCurrCat(const jfFFN_CategoryCore* inval) {
  assert(inval!=NULL);
  curr_cat = inval;
}
//-------------------------------------------------------
const jfFFN_CategoryCore* jfFFNSearch::GetCurrCat() const {
  return curr_cat;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// new fic processing
//-------------------------------------
/*
bool jfFFNSearch::IsCurrent(const jfFFNSCategory* incat) const {
  return (curr_cat==incat);
} */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfFFNSearch::~jfFFNSearch() {
  if (list_to_search!=NULL) delete list_to_search;
  if (selector!=NULL) delete selector;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNSearch::WriteListOfCats(QString fname) const {
  // variables
  size_t loc_count, loc_index;
  QTextStream* outfile;
  jfFFN_CategoryCore* ccat;
  // initial checks
  if (list_to_search==NULL) return false;
  loc_count = list_to_search->size();
  if (loc_count==0) return false;
  // setting up the file
  outfile = GetOpenFile(fname,true);
  if (outfile==NULL) return false;
  // looping and writing
  for (loc_index=0 ; loc_index<loc_count ; loc_index++ ) {
    ccat = (*list_to_search)[loc_index];
    assert(ccat!=NULL);
    (*outfile) << ccat->MakeFinder() << "\n";
  }
  // done
  CloseFileStream(outfile,true);
  delete outfile;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o methods for fanfiction.net specific data
//-----------------------------------------------------
bool jfFFNSearch::AddMiddleToFile(QTextStream* outfile) const {
  // local variables
  jfOutString ostring;
  // checking and special conditions
  if (outfile==NULL) return false;
  assert(selector!=NULL);
  // adding the search values...
  ostring << sv[0] << sv[1] << sv[2] << sv[3] << sv[4] << sv[5];
  (*outfile) << QString(ostring) << "\n";
  // the only other thing we really need to add is the selector
  return selector->AddToFile(outfile);
}
//-----------------------------------------------------
bool jfFFNSearch::ReadMiddleFromFile(jfFileReader* infile) {
  // constants, checks, and variables
  const QString fname = "jfFFNSearch::ReadMiddleFromFile";
  QString nline;
  /**/JDEBUGLOG(fname,1)
  assert(infile!=NULL);
  /**/JDEBUGLOG(fname,2)
  /* the first part to check is the search values*/
  if (!infile->ReadParseLine(5,6,fname)) return infile->BuildError("The FFN Search Parameters line is bad!");
  if (!(infile->lp).SIntVal(0,sv[0])) return infile->BuildError("FFN Search Param 0 failed!");
  if (!(infile->lp).SIntVal(1,sv[1])) return infile->BuildError("FFN Search Param 1 failed!");
  if (!(infile->lp).SIntVal(2,sv[2])) return infile->BuildError("FFN Search Param 2 failed!");
  if (!(infile->lp).SIntVal(3,sv[3])) return infile->BuildError("FFN Search Param 3 failed!");
  if (!(infile->lp).SIntVal(4,sv[4])) return infile->BuildError("FFN Search Param 4 failed!");
  if ((infile->lp).Num() == 6) {
      if (!(infile->lp).SIntVal(5,sv[5])) return infile->BuildError("FFN Search Param 5 failed!");
  }
  /**/JDEBUGLOG(fname,3)
  // reading the selector
  if (selector==NULL) selector = new jfFFN_CatP_SelCat();
  return selector->GetFromFile(infile);
}

//***************************************************************************

