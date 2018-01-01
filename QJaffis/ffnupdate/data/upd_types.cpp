/////////////////////////////////////////////////////////////////////////////
// Name:        upd_types.cpp
// Author :     John Q Metro
// Purpose :    Misc types and untils helpful in fanfiction.net updating
// Created:     January 18, 2011
// Conversion to Qt Started September 25, 2013
// Updated:     August 8, 2016
/////////////////////////////////////////////////////////////////////////////
#ifndef UPD_TYPES_H_INCLUDED
  #include "upd_types.h"
#endif // UPD_TYPES_H_INCLUDED
//---------------------------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
//--------------------------------------------------------
#include <assert.h>
//****************************************************************************
jfAuthorInfo::jfAuthorInfo(size_t in_authorid, QString in_authname) {
  author_id = in_authorid;
  author_name = in_authname;
}
//-----------------------------------------------------------------
jfAuthorInfo::jfAuthorInfo(const jfAuthorInfo& inval) {
  author_id = inval.author_id;
  author_name = inval.author_name;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting info
//------------------------------------------------
size_t jfAuthorInfo::GetAuthorID() const { return author_id;}
//------------------------------------------------
QString jfAuthorInfo::GetAuthorName() const { return author_name; }
//------------------------------------------------
QString jfAuthorInfo::GetUrl() const {
  // constants and values
  const QString sstart = "https://www.fanfiction.net/u/";
  QString result = sstart;
  QString nbuffer = author_name;
  // we build from there
  result += QString::number(author_id);
  result += "/";
  nbuffer.replace("\'","");
  nbuffer.replace(" ","_");
  nbuffer.replace("-","_");
  nbuffer.replace("__","_");
  nbuffer.replace("__","_");
  result += nbuffer;
  return result;
}
//****************************************************************************
// constructors
//-------------------------------------------
jfFFNItemAuthor::jfFFNItemAuthor():jfFFNItemCore() {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
bool jfFFNItemAuthor::SetFromString(const QString& indata, QString& outerr) {
  // local variables
  QString buffer;
 jfStringParser xparser;
  // we start
  xparser.ChangeData(indata);
  // processing fic id, title, and making the link
  if (!GetLinkTitle(xparser,outerr)) return false;
  // get we the descrtption
  if (!ExtractDescription(xparser,outerr)) return false;
  // getting the category name
  if (!xparser.MovePastAlt("class='gray z-padtop2'>","class='z-padtop2 xgray'>")) {
    outerr = "Category Name not found! A";
    return false;
  }
  if (!xparser.GetUpto("- Rated:",buffer)) {
    outerr = "Category Name not found! B";
    return false;
  }
  catname = buffer.trimmed();
  // the rest is just like jfFFNItem
  // next, the rating, language, genres, chapter count, and word count
   if (!Rating2Words(xparser, outerr)) return false;
  // we skip past the reviews
  if (!xparser.MovePast("Reviews:")) {
    // this is okay, sometimes there are no reviews
  }
  if (!Dates_and_Completion(xparser,outerr)) return false;
  // done with all the parsing, just some finishing up...
  validdata = true;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
QString jfFFNItemAuthor::GetCatname() const {
  return catname;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------------
QString jfFFNItemAuthor::GetTypeID() const {
  return "FFNItemAuthor";
}
//-------------------------------------------
// since we do not actually display the author derived items, this is here for compatabilty
bool jfFFNItemAuthor::LoadValues(jfSkeletonParser* inparser) const {
  assert(false);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parsing helper methods
bool jfFFNItemAuthor::GetLinkTitle(jfStringParser& xparser, QString& outerr) {
  // variables
  unsigned long qval;
  QString buffer, oerr;
  // starting
  rank = 0;
  // next up is the story id
  if (!xparser.MovePastAlt("<a  class=stitle href=\"/s/", "<a class=stitle href=\"/s/")) {
    outerr = "Start of Fic ID not found!";
    return false;
  }
  if (!xparser.GetMovePastULong("/",qval,oerr)) {
    outerr = oerr + " When getting fic id.";
    return false;
  }
  num_id = qval;
  // we get the rest of the url
  if (!xparser.GetMovePast("\">",buffer)) {
    outerr = "End of fic link not found!";
    return false;
  }
  // we then build the url...
  primarylink = "https://www.fanfiction.net/s/" + QString::number(num_id);
  primarylink += (QString("/") + buffer);
  // next up is the title
  if (!xparser.MovePast("<img class='lazy cimage")) {
    outerr = "Problems finding image in author link!";
    return false;
  }
  if (!xparser.GetDelimited(">","</a>",buffer)) {
    outerr = "End of title field not found!";
    return false;
  }
  name = buffer.trimmed();
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// io methods. compatability only
//-------------------------------------------
bool jfFFNItemAuthor::AddMoreExtraStuff(QTextStream* outfile) const {
  assert(false);
  return false;
}
//-------------------------------------------
bool jfFFNItemAuthor::ReadMoreExtraStuff(jfFileReader* infile) {
  assert(false);
  return false;
}
//=========================================================================
// bundles fic id and published date for a limit: do not get fics with lower id or earlier date
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors and data methods
//---------------------------------------------------
jfFicLimit::jfFicLimit(QDate dlimit, size_t idlimit) {
  fpublished = dlimit;
  id_limit = idlimit;
  nodate = false;
}
//---------------------------------------------------
void jfFicLimit::ChangeLimit(size_t inval, bool innodate) {
  id_limit = inval;
  nodate = innodate;
}
//---------------------------------------------------
size_t jfFicLimit::IdLimit() const {
  return id_limit;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// testing...
bool jfFicLimit::IsLater(const jfFFNItemCore* test) const {
  assert(test!=NULL);
  if ((test->GetID())<id_limit) return false;
  if (nodate) return true;
  if ((test->GetPublished())<fpublished) return false;
  return true;
}

//****************************************************************************
