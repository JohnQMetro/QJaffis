/******************************************************************************
Name    :   fim_search.cpp
Author  :   John Q Metro
Purpose :   Search Object for fimfiction.net
Created :   November 25, 2012
Conversion to Qt Started Nov 7, 2013
Updated :   December 30, 2017 (major changes to tags)
******************************************************************************/
#ifndef FIM_SEARCH_H_INCLUDED
  #include "fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED
#ifndef FIM_CONSTANTS_H
  #include "fim_constants.h"
#endif // FIM_CONSTANTS_H
//------------------------------------------------
#ifndef UTILS3_H_INCLUDED
  #include "../../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef HTML_DISPLAY_H_INCLUDED
  #include "../../../interface/display/html_display.h"
#endif // HTML_DISPLAY_H_INCLUDED
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../../../core/filters/fim/fimfilters1.h"
#endif
#ifndef FIM_CONSTANTS_H_INCLUDED
  #include "fim_constants.h"
#endif // FIM_CONSTANTS_H_INCLUDED
//-------------------------------------------------------
#include <assert.h>

//=========================================================================
// constructors
jfFIMSearch::jfFIMSearch():jfSearchCore() {
  pagelimit = 400000;
  genres_filter = NULL;
  characters_filter = NULL;
  warnings_filter = NULL;
  contentt_filter = NULL;
  min_wc = 0;
  max_wc = 0;
  completed = false;
  rating = 'A';
  order = 0;
  viewmature = true;
  eg_status = jfts_NONE;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
//----------------------------
bool jfFIMSearch::SetWordcount(size_t wcmin_in, size_t wcmax_in) {
  if (wcmax_in!=0) {
    if (wcmax_in<=wcmin_in) return false;
  }
  min_wc = wcmin_in;
  max_wc = wcmax_in;
  return true;
}
//----------------------------
void jfFIMSearch::SetSString(QString inval) {
  ssearch = inval;
  esc_search = QueryEncode(inval);
}
//----------------------------
bool jfFIMSearch::SetMinor(size_t inorder, QChar inrating) {
  // cases in which we return false
  if (inorder>5) return false;
  if (!fimcon::ratchars.contains(inrating)) return false;
  // setting the values
  order = inorder;
  rating = inrating;
  return true;
}
//----------------------------
void jfFIMSearch::SetCompleted(bool invalue) {
  completed = invalue;
}
//------------------------------
void jfFIMSearch::SetMature(bool inval) {
  viewmature = inval;
}
//------------------------------
// for including, excluding, or ignoring Equestria Girls
bool jfFIMSearch::SetEGStatus(jfTAG_STATUS new_status) {
    if (new_status == jfts_ALTERNATE) return false; // only used for multi-possibility filtering
    eg_status = new_status;
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented method for sending category data
void jfFIMSearch::DispatchCategory() {
  // variables
  struct jfCategoryInfoMsg oval;
  // building the event to send
  oval.catheader = GetCatHeader();
  oval.catname = GetCatName();
  emit SendCatInfo(oval);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemeted virtual methods
//----------------------------
QString jfFIMSearch::GetTypeID() const {
  return "FIMSearch";
}
//----------------------------
QString jfFIMSearch::GetCatName() const {
  return "FIM";
}
//----------------------------
QString jfFIMSearch::GetCatHeader() const {
  return "";
}
//----------------------------
QString jfFIMSearch::GetIndexUrl(size_t inp_index) const {
  return GetUrl(inp_index,false);
}
//----------------------------
QString jfFIMSearch::GetHTMLHeader(size_t result_category) const  {
  QString result;
  result = "<h1 class=\"maintittle\">FIMFiction.Net Search Results</h1>\n";
  result += "<h2 class=\"res_categ_header\">Result Category : ";
  if (result_category == 32) result += categories->GetName();
  else result += categories->NameAtIndex(result_category);
  result += "</h2>";
  return result;
}
//----------------------------
void jfFIMSearch::MakeLocalSkeleton(jfSkeletonBase* src) {
  jfSkeletonSingle* temp, *newval;
  // asserts and checks
  assert(src!=NULL);
  assert(src->GetSkelType()==jfs_SINGLE);
  // we start
  temp = dynamic_cast<jfSkeletonSingle*>(src);
  newval = new jfSkeletonSingle(temp);
  // clearing
  if (local_skel) ClearLocalSkeleton();
  skel_link = newval;
  local_skel = true;
}
//----------------------------
void jfFIMSearch::LoadValues(jfSkeletonParser* inparser, size_t result_categ) {
  LoadCoreValues(inparser,result_categ);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returning data data
//-------------------------------------------
QString jfFIMSearch::GetSString() const {
  return ssearch;
}
//-------------------------------------------
size_t jfFIMSearch::GetMinWC() const {
  return min_wc;
}
//-------------------------------------------
size_t jfFIMSearch::GetMaxWC() const {
  return max_wc;
}
//-------------------------------------------
IntPair* jfFIMSearch::GetWC() const {
  IntPair* wcres;
  wcres = new IntPair;
  wcres->A = min_wc;
  wcres->B = max_wc;
  return wcres;
}
//-------------------------------------------
bool jfFIMSearch::GetCompleted() const {
  return completed;
}
//-------------------------------------------
QChar jfFIMSearch::GetRating() const {
  return rating;
}
//-------------------------------------------
size_t jfFIMSearch::GetOrder() const {
  return order;
}
//------------------------------------------
bool jfFIMSearch::GetMature() const {
  return viewmature;
}
//------------------------------------------
jfTAG_STATUS jfFIMSearch::GetEGStatus() const {
    return eg_status;
}
//-------------------------------------------
jfTagListing* jfFIMSearch::GetGenres() {
  if (genres_filter==NULL) MakeEmptyTags(FIMT_genre);
  return genres_filter;
}
//-------------------------------------------
jfTagListing* jfFIMSearch::GetCharacters() {
  if (characters_filter==NULL) MakeEmptyTags(FIMT_character);
  return characters_filter;
}
//---------------------------------------------------
jfTagListing* jfFIMSearch::GetWarnings() {
    if (warnings_filter == NULL) MakeEmptyTags(FIMT_warning);
    return warnings_filter;
}
//---------------------------------------------------
jfTagListing* jfFIMSearch::GetContentType() {
    if (contentt_filter == NULL) MakeEmptyTags(FIMT_content);
    return contentt_filter;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// more methods
QString jfFIMSearch::GetCompactUrl(size_t inp_index) const {
  return GetUrl(inp_index,true);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIMSearch::MakeTagQuery(FIM_TagType this_type, jfTagListing* this_filter) const {
    if (this_filter == NULL) return "";
    QString result = "";
    jfTAG_STATUS tvalue;
    QString curr_slug;
    QStringList* names_slugs = fimcon::tags->namesAndSlugsForType(this_type);
    // looping thru the genres
    for (size_t idxl = 0; idxl <(names_slugs->count()) ; idxl += 2) {
        tvalue = (*this_filter)[(names_slugs->at(idxl))];
        if (tvalue == jfts_NONE) continue;
        if (tvalue == jfts_ALTERNATE) assert(false);

        curr_slug = names_slugs->at(idxl+1);
        if (tvalue == jfts_INCLUDE) result += "+%23" + curr_slug;
        else result += "+-%23" + curr_slug;
    }
    delete names_slugs;
    return result;
}
//---------------------------------------------------------
// url methods
QString jfFIMSearch::GetUrl(size_t index, bool is_iscompact) const {
  // constants
  QString Result = "https://www.fimfiction.net/stories?";
  // variables
  QString buffer;
  // starting
  assert(index!=0);
  assert(fimcon::tags != NULL);

    // we add the search term
    Result += "&q=";
    if (!esc_search.isEmpty()) Result += esc_search;

    // genres next
    Result += MakeTagQuery(FIMT_genre,genres_filter);

    // next up : characters
    Result += MakeTagQuery(FIMT_character,characters_filter);

    // warnings
    Result += MakeTagQuery(FIMT_warning,warnings_filter);

    // content type
    Result += MakeTagQuery(FIMT_content,contentt_filter);

    // series: for now, Equestria Girls only
    if (eg_status != jfts_NONE) {
        if (eg_status == jfts_INCLUDE) Result += "+%23";
        else Result += "+-%23";
        Result += "equestria-girls";
    }

    // the rating is also a tag, but for now, I'll treat it the same way I used to
    if (rating != 'A') {
        Result += "+%23" + fimcon::RatSearchLookup(rating);
    }

  // compact
  Result += "&view_mode=";
  Result += QString::number((is_iscompact)?(2):(0));

  // order
  Result += "&order=" + QString(fimcon::ordername[order]);
  // completion
  if (completed) Result += "&status=completed";
  // minimum words and max words
  if (min_wc!=0) {
      Result += "&minimum_words=";
      Result += QString::number(min_wc);
  }
  if (max_wc!=0) {
      Result += "&maximum_words=";
      Result += QString::number(max_wc);
  }
  // index
  if (index!=1) {
    Result += "&page=";
    Result += QString::number(index);
  }
  // done
  return Result;
}
//--------------------------------------------------------
void jfFIMSearch::MakeEmptyTags(FIM_TagType type) {
    assert(fimcon::tags != NULL);
  // starting
  jfTagListing* temp;
  temp = new jfTagListing();
  // producing the new tag listing
  fimcon::tags->SetTagsToEmpty(*temp,type);
  // sticking it where it should go...
  if (type == FIMT_genre) {
    if (genres_filter!=NULL) delete genres_filter;
    genres_filter = temp;
  }
  else if (type == FIMT_character) {
    if (characters_filter!=NULL) delete characters_filter;
    characters_filter = temp;
  }
  else if (type == FIMT_warning) {
      if (warnings_filter != NULL) delete warnings_filter;
      warnings_filter = temp;
  }
  else if (type == FIMT_content) {
      if (contentt_filter != NULL) delete contentt_filter;
      contentt_filter = temp;
  }
  else {
      // To do...
      assert(false);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o methods for fim search specific data
//-----------------------------------------------
void jfFIMSearch::AddTagListingToFile(jfTagListing* listing, QTextStream* outfile) const {
    assert(outfile != NULL);
    assert(listing != NULL);
    jfOutString resline;
    // outputting
    if (listing == NULL) resline << "NULL";
    else resline << listing->ToString();
    (*outfile) << resline << "\n";
}
//-----------------------------------------------
bool jfFIMSearch::AddMiddleToFile(QTextStream* outfile) const {
  // local variables
  jfOutString resline;
  // checking and special conditions
  if (outfile==NULL) return false;
  // building the first output line
  resline << completed << QString(rating) << order << ssearch;
  (*outfile) << resline << "\n";
  resline.clear();
  // second output line
  resline << min_wc << max_wc << viewmature << ((size_t)eg_status);
  (*outfile) << resline << "\n";
  resline.clear();
  // third output line
  AddTagListingToFile(genres_filter,outfile);
  // fourth output line
  AddTagListingToFile(characters_filter,outfile);
  // fifth output line
  AddTagListingToFile(warnings_filter,outfile);
  // sixth output line
  AddTagListingToFile(contentt_filter,outfile);
  // done
  return true;
}
//-------------------------------------------------------
bool jfFIMSearch::ReadTagListing(jfTagListing *&target, const QString& tlname,jfFileReader* infile) {
    const QString fname = "jfFIMSearch::ReadTagListing";
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
//------------------------------------------------------------
bool jfFIMSearch::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFIMSearch::ReadMiddleFromFile";
  // local variables
  QString buffer;
  // starting with line 1
  assert(infile!=NULL);
  if (!infile->ReadParseLine(4,funcname)) return false;
  // getting the values
  if (!infile->lp.BoolVal(0,completed)) return infile->BuildError("The Completed Flag is malformed!");
  buffer = infile->lp.UnEscStr(1);
  if (buffer.isEmpty()) return infile->BuildError("The Rating Field is blank!");
  rating = buffer[0];
  if (!fimcon::ratchars.contains(rating)) return infile->BuildError("The Rating Field is invalid!");
  if (!infile->lp.SIntVal(2,order)) return infile->BuildError("The Order Field is malformed!");
  ssearch = infile->lp.UnEscStr(3);
  SetSString(ssearch);

  // line 2, word counts, view mature
  if (!infile->ReadParseLine(4,funcname)) return false;
  // getting the values
  if (!infile->lp.SIntVal(0,min_wc)) return infile->BuildError("The minimum word count is not a number!");
  if (!infile->lp.SIntVal(1,max_wc)) return infile->BuildError("The maximum word count is not a number!");
  if (max_wc!=0) {
    if (max_wc<min_wc) return infile->BuildError("The max wordcount is less than the min wordcount!");
  }
  // mature settings
  if (!infile->lp.BoolVal(2,viewmature)) return infile->BuildError("The third (viewmature) item is not boolean!");
  // Equestria Girls status
  size_t temp_egint;  QString egoerr;
  if (!infile->lp.SBoundVal(3,4,temp_egint,egoerr)) {
      return infile->BuildError("The Equestria Girls item is invalid: " + egoerr);
  }
  jfTAG_STATUS temp_status = (jfTAG_STATUS)temp_egint;
  if (temp_status == jfts_ALTERNATE) return infile->BuildError("Equestia Girls tag status cannot be alternate!");
  eg_status = temp_status;
  // genres line
  if (!ReadTagListing(genres_filter,"Genres",infile)) return false;
  // characters line
  if (!ReadTagListing(characters_filter,"Characters",infile)) return false;
    /*
    // working our way around a tag change
    jfFIMCharacterFilter* tempee = new jfFIMCharacterFilter();
    tempee->SetTags(characters_filter);
    delete characters_filter;
    characters_filter = tempee->GetTagsCopy();
    delete tempee;
    */
  if (!ReadTagListing(warnings_filter,"Warnings",infile)) return false;
  // content type
  if (!ReadTagListing(contentt_filter,"Content Type",infile)) return false;
  // done
  return true;
}
//****************************************************************************
