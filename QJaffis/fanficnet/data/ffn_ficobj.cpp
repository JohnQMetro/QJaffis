/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_ficobj.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     May 25, 2010
// Conversion to Qt Started September 25, 2013
// Updated:     March 24, 2023
/////////////////////////////////////////////////////////////////////////////
#ifndef JFFNFICOBJ
  #include "ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../../core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef HTMLSKELTON_H_INCLUDED
  #include "../../core/utils/htmlskelton.h"
#endif // HTMLSKELTON_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

/*
#ifndef UPD_TYPES_H_INCLUDED
  #include "../../ffnupdate/data/upd_types.h"
#endif // UPD_TYPES_H_INCLUDED
*/
//----------------------------------------------
#include <assert.h>
//*************************************************************************
const QString& IdForFFNItemCore() {
    static QString FFN_CORE_TYPE_ID = QString("FFN_Core_Fanfic");
    return FFN_CORE_TYPE_ID;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//---------------------------------------
jfFFNItemCore::jfFFNItemCore():jfGenericFanfic3() {
    type_labels.append(IdForFanficPairs());
    type_labels.append(IdForFFNItemCore());
    favs = 0;
    isupdated = false;
}
// -----------------------------
jfFFNItemCore::jfFFNItemCore(const jfSearchResultItemData& init_data):jfGenericFanfic3(init_data) {
    type_labels.append(IdForFanficPairs());
    type_labels.append(IdForFFNItemCore());
    favs = 0;
    isupdated = false;
}
// ----------------------------
jfFFNItemCore::jfFFNItemCore(const jfFFNItemCore& src):jfGenericFanfic3(src) {
    type_labels.append(IdForFanficPairs());
    type_labels.append(IdForFFNItemCore());
    language = src.language;
    isupdated = src.isupdated;
    published = src.published;
    rating = src.rating;
    favs = src.favs;
    RelationshipCopy(src);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//---------------------------------------
QString jfFFNItemCore::MakeAuthorUrl() const {
  QString result = ("https://www.fanfiction.net/u/");
  result += QString::number(author_id) + ("/");
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data getting methods
//-------------------------------------------------
QDate jfFFNItemCore::GetPublished() const {
  return published;
}
//-------------------------------------------------
bool jfFFNItemCore::IsUpdated() const {
  return isupdated;
}
//--------------------------------------------
QChar jfFFNItemCore::GetRating() const {
  return rating;
}
//----------------------------------------------
const QString& jfFFNItemCore::GetLanguage() const {
  return language;
}
//----------------------------------------------
size_t jfFFNItemCore::GetFavs() const {
  return favs;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNItemCore::~jfFFNItemCore() {
    ClearPairingVector(char_pairs);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFNItemCore::StoreToCopyFFNCore(jfFFNItemCore* destination) const {
    assert(destination != NULL);
    StoreToCopyGenericFanfic3(destination);
    destination->RelationshipCopy(*this);
    destination->language = language;
    destination->isupdated = isupdated;
    destination->published = published;
    destination->rating = rating;
    destination->favs = favs;
}
// -----------------------------------------------
bool jfFFNItemCore::UpdateFromSource(const jfFFNItemCore* source) {
    if (source == NULL) return false;
    source->StoreToCopyGenericFanfic3(this);
    RelationshipCopy(*source);
    language = source->GetLanguage();
    isupdated = source->IsUpdated();
    published = source->GetPublished();
    rating = source->GetRating();
    favs = source->GetFavs();
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNItemCore::LoadValuesContinued(jfSkeletonParser* inparser) const {
    LoadRelationships(inparser, true);

    inparser->AddText("ITEMF_LANGUAGE",language);
    QString rbuffer;
    if (rating==('+')) rbuffer = ("K+");
    else rbuffer = rating;
    inparser->AddText("ITEMF_RATING",rbuffer);
    // we add the dates last
    inparser->AddText("ITEMF_PUBDATE",published.toString("MM-dd-yyyy"));
    inparser->AddBool("ITEMF_ISUPDATED",isupdated);
    inparser->AddUInt("ITEMF_FAVS",favs);
    return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//---------------------------------------
jfFicExtract* jfFFNItemCore::MakeExtract() const {
  jfFicExtract_FFN* result;
  result = new jfFicExtract_FFN();
  LoadIntoExtract(result);
  return result;
}
//---------------------------------------
void jfFFNItemCore::LoadIntoExtract(jfFicExtract* into) const {
  // loading basic stuff
  LoadIntoExtractCore(into);
  into->auth_id = author_id;
  into->complete = completed;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//---------------------------------------
bool jfFFNItemCore::AddMoreExtraStuff(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
    // relationships (line 8)
    (*outfile) << RelationshipsAsStorageString() << "\n";
    // line 9, more info
    xresult << language << QString(rating) << favs << isupdated << published;
    (*outfile) << xresult << "\n";
  // done
  return AddCodaStuff(outfile);
}
//---------------------------------------
bool jfFFNItemCore::ReadMoreExtraStuff(jfFileReader* infile) {
  const QString funcname = ("jfFFNItemCore::ReadMoreExtraStuff");
  // local variables
  QString buffer;
  assert(infile!=NULL);
  /**/JDEBUGLOG(funcname,1);
  // line 8 is relationships
  if (!infile->ReadLine(buffer,funcname)) return false;
  if (!SetRelationshipsFromString(buffer, false)) return infile->BuildError("Could not parse relationships!");

  // line 9, more stuff
  if (!infile->ReadParseLine(5, funcname)) return false;
  language = (infile->lp).UnEscStr(0);
  buffer = (infile->lp).UnEscStr(1);
  rating = buffer[0];
  if (!infile->lp.SIntVal(2,favs)) return infile->BuildError("The favorites count is not a number!");
  if (!infile->lp.BoolVal(3,isupdated)) return infile->BuildError("The updated flag is not valid!");
  if (!infile->lp.DateVal(4,published)) return infile->BuildError("Unable to convert the Published field properly");

  // done with the lines
  /**/JDEBUGLOG(funcname,3);
  return ReadCodaStuff(infile);
}
// ===============================================================================
// ================================================================================
const QString& IdForFFNItem() {
    static QString FFN_ITEM_TYPE_ID = QString("FFNItem");
    return FFN_ITEM_TYPE_ID;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNItem::jfFFNItem():jfFFNItemCore() {
    type_labels.append(IdForFFNItem());
    cat_link = NULL;
}
// ---------------------------------
jfFFNItem::jfFFNItem(const jfSearchResultItemData& init_data):jfFFNItemCore(init_data) {
    type_labels.append(IdForFFNItem());
    cat_link = NULL;
}
//-------------------------------------------------------------------
// copy constructor
jfFFNItem::jfFFNItem(const jfFFNItem& src):jfFFNItemCore(src) {
    type_labels.append(IdForFFNItem());
    cat_link = src.cat_link;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data setting methods
//---------------------------------------
bool jfFFNItem::SetCatLink(const jfFFN_CategoryCore* cat_linkin) {
    if (cat_linkin==NULL) return false;
    else {
        cat_link = cat_linkin;
        return true;
    }
}
//--------------------------------------------------------------------
// updates the item from src (if valid)
bool jfFFNItem::UpdateFromObj(const jfFFNItemCore* src) {
    // invalid cases
    if (src==NULL) return false;
    if (numeric_id != src->GetId()) return false;

    // copying over the fields
    UpdateFromSource(src);;
    isupdated = true;
    return true;
}
//-----------------------------------------------------------------
bool jfFFNItem::UpdateWordcount(const jfFFNItemCore* src) {
  // invalid cases
  if (src==NULL) return false;
  if (numeric_id != src->GetId()) return false;
  // changing the word count
  rating = src->GetRating();
  word_count = src->GetWordcount();
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNItem::GetCatString() const {
  return cat_link->GetName();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNItem::AddCodaStuff(QTextStream* outfile) const  {
  return true;
}
//-------------------------------------------------------------
bool jfFFNItem::ReadCodaStuff(jfFileReader* infile) {
  return true;
}
//*********************************************************************************



