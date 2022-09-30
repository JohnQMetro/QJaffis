/******************************************************************************
Name    :   ao3_search.cpp
Author  :   John Q Metro
Purpose :   Defines search object of AO3
Created :   August 28, 2012
Conversion to Qt Started March 29, 2014
Updated :   July 19, 2022
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
#ifndef AO3_LOOKVALS_H
    #include "ao3_lookvals.h"
#endif // AO3_LOOKVALS_H
//-----------------------------------
#include <assert.h>
/*****************************************************************************/
// constructors
//-----------------------------------------------------
// default constructor, please call SetData before using
jfAO3Search::jfAO3Search():jfSearchCore() {
    // category sstuff, and some others
  cat_data = NULL;
  curr_cat = NULL;
  selector = new jfGenCat_PickData();
  validdata = false;
  pagelimit = 100000;
  numscats = 0;
  icatindex = -1;

  // search parameters and objects
  url_maker = NULL;
  rindex = 1000; // invalid rsting value will translate to no restriction
  wx_violence = wx_death = wx_underage = false;
  wx_rape = true;
  orientation_parameters = NULL; // created as needed
  excludes = includes = template_replacer = "";
  conly = false;
  eng_only = true;
  ord_index = 1; // updated date new to old
  words_from = words_to = -1;
  cross_only = false;

}
//-----------------------------------------------------
jfAO3Search::jfAO3Search(jfGen_CatHolder* cat_datain, size_t search_indexin):jfSearchCore() {
  pagelimit = 100000;
  selector = NULL;
  SetData(cat_datain,search_indexin);

  // search parameters and objects
  url_maker = NULL;
  rindex = 1000; // invalid rsting value will translate to no restriction
  wx_violence = wx_death = wx_underage = false;
  wx_rape = true;
  orientation_parameters = NULL; // created as needed
  excludes = includes = template_replacer = "";
  conly = false;
  eng_only = true;
  ord_index = 1; // updated date new to old
  words_from = words_to = -1;
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
//---------------------------------------------------
size_t jfAO3Search::GetSSIndex() const {
  return search_index;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting search options
void jfAO3Search::SetOrder(size_t in_oindex) {
    ord_index = in_oindex;
}
void jfAO3Search::SetRatingIndex(size_t in_rindex) {
    rindex = in_rindex;
}

void jfAO3Search::SetWarningExcludes(bool in_violence, bool in_death, bool in_rape, bool in_underage) {
    wx_violence = in_violence;
    wx_death = in_death;
    wx_rape = in_rape;
    wx_underage = in_underage;
}
void jfAO3Search::SetTagExcludes(const QString& in_excludes, const QString& in_replacer) {
    excludes = in_excludes;
    template_replacer = in_replacer;
}

void jfAO3Search::SetPredefinedExcludes(const QStringList& predefined_excludes) {
    predef_excludes.clear();
    if (predefined_excludes.size() > 0) predef_excludes.append(predefined_excludes);
}

void jfAO3Search::SetIncludes(const QString& in_includes) {
    includes = in_includes;
}
void jfAO3Search::SetWordLimits(int in_words_from, int in_words_to) {
    words_from = in_words_from;
    words_to = in_words_to;
}
void jfAO3Search::SetExtras(bool in_completed_only, bool in_english_only) {
    conly = in_completed_only;
    eng_only = in_english_only;
}
void jfAO3Search::SetCrossoverOnly(bool only_crossovers) {
    cross_only = only_crossovers;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
jfGenCat_PickData* jfAO3Search::GetSelector() {
  return selector;
}
//----------------------------------------------------
jfGen_CatHolder* jfAO3Search::GetHolder() {
  return cat_data;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// [ getting search options ]

jfTagListing* jfAO3Search::GetOrientation() {
    if (orientation_parameters == NULL) {
        orientation_parameters = ao3values::orientMaker.MakeOrientationListing(1);
    }
    return orientation_parameters;
}
size_t jfAO3Search::GetOrderIndex() const {
    return ord_index;
}
size_t jfAO3Search::GetRatingIndex() const {
    return rindex;
}
bool jfAO3Search::GetExcludedViolence() const {
    return wx_violence;
}
bool jfAO3Search::GetExcludedDeath() const {
    return wx_death;
}
bool jfAO3Search::GetExcludedRape() const {
    return wx_rape;
}
bool jfAO3Search::GetExcludedUnderageSex() const {
    return wx_underage;
}
QString jfAO3Search::GetExcludedTagsList() const {
    return excludes;
}
QString jfAO3Search::GetExcludeReplacer() const {
    return template_replacer;
}
QStringList jfAO3Search::GetPredefinedExcludes() const {
    return predef_excludes;
}
QString jfAO3Search::GetIncludedTagsList() const {
    return includes;
}
int jfAO3Search::GetMinWordCount() const {
    return words_from;
}
int jfAO3Search::GetMaxWordCount() const {
    return words_to;
}
bool jfAO3Search::IsCompletedOnly() const {
    return conly;
}
bool jfAO3Search::IsEnglishOnly() const {
    return eng_only;
}
bool jfAO3Search::IsCrossoverOnly() const {
    return cross_only;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented method for sending category data
void jfAO3Search::DispatchCategory() {
  // variables and constants
  const QString fname="jfAO3Search::DispatchCategory";
  struct jfCategoryInfoMsg ccatdata;
  // doing
  if (url_maker != NULL) delete url_maker;
  /**/JDEBUGLOG(fname,1);
  MakeURLMaker();
  /**/JDEBUGLOG(fname,2);
  url_maker->setCategoryName(curr_cat->GetName());
  /**/JDEBUGLOG(fname,3);
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
  QString first_url = GetIndexUrl(1);
  // building the title line
  result += "<font size=+2><b>Category : <a href=\"";
  result += first_url + "\">";
  result += GetCatName() + "</a></b></font><br>\n";
  // next line: link display
  result += "<font color=\"green\">";
  result += first_url + "</font><br>\n";
  // we should perhaps add info on the number of results...
  // done
  return result;
}
//-----------------------------------------------------
// pages start at 1
QString jfAO3Search::GetIndexUrl(size_t inp_index) const {
  const QString fname = "jfAO3Search::GetIndexUrl";
  assert(inp_index>=1);
  // return an empty string if there is no valid category
  if (curr_cat==NULL) return "";
  else {
      QString url = url_maker->getUrlForPage(inp_index);
      /**/JDEBUGLOGS(fname,1,url)
      return url;
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
  if (url_maker != NULL) delete url_maker;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfAO3Search::MakeURLMaker() {
    const QString fname = "jfAO3Search::MakeURLMaker";
    url_maker = new AO3UrlMaker();
    /**/JDEBUGLOG(fname,1)
    url_maker->setOrientationListing(orientation_parameters);
    /**/JDEBUGLOG(fname,2)
    url_maker->setRatingChoice(rindex);
    /**/JDEBUGLOG(fname,3)
    url_maker->setOrderingChoice(ord_index);
    /**/JDEBUGLOG(fname,4)
    url_maker->setFullExcludeQuery(predef_excludes, excludes, template_replacer);
    /**/JDEBUGLOG(fname,6)
    url_maker->setWarningExcludes(wx_violence, wx_death, wx_rape, wx_underage);
    /**/JDEBUGLOG(fname,7)
    url_maker->setTagIncludes(includes);
    /**/JDEBUGLOGS(fname,8,includes)
    url_maker->setExtraLimits(conly, cross_only,((eng_only)?"en":""));
    /**/JDEBUGLOG(fname,9)
    url_maker->setWordLimit(words_from, words_to);
    /**/JDEBUGLOG(fname,10)

    url_maker->commitParts();
    /**/JDEBUGLOG(fname,11)
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o methods for fanfiction.net specific data
//-----------------------------------------------------
void jfAO3Search::AddTagListingToFile(jfTagListing* listing, QTextStream* outfile) const {
    assert(outfile != NULL);
    assert(listing != NULL);
    jfOutString resline;
    // outputting
    if (listing == NULL) resline << "NULL";
    else resline << listing->ToString();
    (*outfile) << resline << "\n";
}
//-----------------------------------------------------
bool jfAO3Search::AddMiddleToFile(QTextStream* outfile) const {
  // local variables
  jfOutString xline;
  // checking and special conditions
  if (outfile==NULL) return false;

  // ordering, rating combo index, exclude violence/death/underage/rape,
  // completed only, english only, crossover only
  xline << "V3" << ord_index << rindex << wx_violence << wx_death << wx_underage << wx_rape;
  xline << conly << eng_only << cross_only;
  (*outfile) << xline << "\n";
  xline.FullClear();
  // predefined excludes
  if (!predef_excludes.isEmpty()) xline << predef_excludes;
  (*outfile) << xline << "\n";
  xline.FullClear();
  // custom excludes
  xline << 0 << excludes << template_replacer;
  (*outfile) << xline << "\n";
  xline.FullClear();
  // includes, words from and words to
  xline << words_from << words_to << includes;
  (*outfile) << xline << "\n";
  xline.FullClear();
  // orientation
  AddTagListingToFile(orientation_parameters, outfile);
  // finally, the selector
  assert(selector!=NULL);
  return selector->AddToFile(outfile);
}
//-----------------------------------------------------
bool jfAO3Search::ReadMiddleFromFile(jfFileReader* infile) {
  const QString fname = "jfAO3Search::ReadMiddleFromFile";
  QString xoerr;

  // first, the many options of the first line
  if (!infile->ReadParseLine(10,fname)) return false;
  // order index
  if (!infile->lp.SIntVal(1, ord_index)) {
      return infile->BuildError("Order index invalid.");
  }
  // rating combo index
  if (!infile->lp.SIntVal(2, rindex)) {
      return infile->BuildError("Rating combo index invalid.");
  }
  // exclude flags
  if (!infile->lp.BoolVal(3, wx_violence)) {
      return infile->BuildError("Excl Violence flag invalid.");
  }
  if (!infile->lp.BoolVal(4, wx_death)) {
      return infile->BuildError("Excl Major Death flag invalid.");
  }
  if (!infile->lp.BoolVal(5, wx_underage)) {
      return infile->BuildError("Excl Underage Sex flag invalid.");
  }
  if (!infile->lp.BoolVal(6, wx_rape)) {
      return infile->BuildError("Excl Rape flag invalid.");
  }
  // completed only flag
  if (!infile->lp.BoolVal(7, conly)) {
      return infile->BuildError("Completed only flag invalid.");
  }
  // english only flag
  if (!infile->lp.BoolVal(8, eng_only)) {
      return infile->BuildError("English only flag invalid.");
  }
  // crossover only flag
  if (!infile->lp.BoolVal(9, cross_only)) {
      return infile->BuildError("Crossover only flag invalid.");
  }

  // excludes
  // predefined excludes
  if (!infile->ReadParseLine(0,fname)) return false;
  if ((infile->lp.Num()) == 0) predef_excludes.clear();
  else {
      QStringList* values = (infile->lp).GetRest(0);
      predef_excludes = (*values);
      delete values;
  }
  // other excludes
  if (!infile->ReadParseLine(3,fname)) return false;

  // string exclude info
  excludes = infile->lp.UnEscStr(1);
  template_replacer = infile->lp.UnEscStr(2);

  // includes and word limits
  if (!infile->ReadParseLine(3,fname)) return false;
  if (!infile->lp.IIntVal(0, words_from)) {
      return infile->BuildError("Words from value invalid.");
  }
  if (!infile->lp.IIntVal(1, words_to)) {
      return infile->BuildError("Words to value invalid.");
  }
  includes = infile->lp.UnEscStr(2);
  // orientation
  if (!ReadTagListing(orientation_parameters,"Orientation",infile)) return false;

  // next, the fandom selector
  if (selector==NULL) selector = new jfGenCat_PickData();
  assert(infile!=NULL);
  return selector->GetFromFile(infile);
}
//-----------------------------------------------------
bool jfAO3Search::ReadTagListing(jfTagListing *&target, const QString& tlname,jfFileReader* infile) {
    const QString fname = "jfAO3Search::ReadTagListing";
    QString buffer;
    if (!infile->ReadUnEsc(buffer,fname)) return false;
    if (buffer=="NULL") target = NULL;
    else {
      target = new jfTagListing();
      if (!target->FromString(buffer)) {
        delete target;
        target = NULL;
        return infile->BuildError("The " + tlname + " listing is wrong!");
      }
    }
    return true;
}
/*****************************************************************************/
