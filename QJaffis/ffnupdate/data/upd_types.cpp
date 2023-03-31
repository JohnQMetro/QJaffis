/////////////////////////////////////////////////////////////////////////////
// Name:        upd_types.cpp
// Author :     John Q Metro
// Purpose :    Misc types and untils helpful in fanfiction.net updating
// Created:     January 18, 2011
// Conversion to Qt Started September 25, 2013
// Updated:     March 26, 2023
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
const QString jfFFNItemAuthor::FFN_AUTHORITEM_TYPE_ID = QString("FFNItemAuthor");
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//-------------------------------------------
jfFFNItemAuthor::jfFFNItemAuthor():jfFFNItemCore() {
    type_labels.append(FFN_AUTHORITEM_TYPE_ID);
}
//-------------------------------------------
jfFFNItemAuthor::jfFFNItemAuthor(const jfSearchResultItemData& init_data):jfFFNItemCore(init_data) {
    type_labels.append(FFN_AUTHORITEM_TYPE_ID);
}
//-------------------------------------------
jfFFNItemAuthor::jfFFNItemAuthor(const jfFFNItemAuthor& src):jfFFNItemCore(src) {
    type_labels.append(FFN_AUTHORITEM_TYPE_ID);
    catname = src.catname;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
const QString& jfFFNItemAuthor::GetCatname() const {
  return catname;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// io methods. compatability only
//-------------------------------------------
bool jfFFNItemAuthor::AddCodaStuff(QTextStream* outfile) const {
  assert(false);
  return false;
}
//-------------------------------------------
bool jfFFNItemAuthor::ReadCodaStuff(jfFileReader* infile) {
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
  if ((test->GetId())<id_limit) return false;
  if (nodate) return true;
  if ((test->GetPublished())<fpublished) return false;
  return true;
}

//****************************************************************************
