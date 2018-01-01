/*****************************************************************************
Name:        skelstore.cpp
Author :     John Q Metro
Purpose :    holding html skeletons
Created:     November 18, 2009
// Conversion to Qt started March 12, 2013
Updated:     November 9, 2013
******************************************************************************/
#ifndef SKELSTORE_H_INCLUDED
  #include "skelstore.h"
#endif // SKELSTORE_H_INCLUDED

#include <assert.h>
/*****************************************************************************/
jfSkeletonCore::jfSkeletonCore():jfBaseObj() {
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfSkeletonCore::GetGuide() const {
  return skelguide;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfSkeletonCore::SetGuide(const QString& inguide) {
  skelguide = inguide;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper methods
// serves as a base for a copy constructor
void jfSkeletonCore::CopyOver(jfSkeletonCore& src) {
  // base data
  name = src.name;
  description = src.description;
  num_id = src.num_id;
  validdata = src.validdata;
  // skeleton stuff
  header_skel    = src.header_skel;
  item_skel      = src.item_skel;
  item_separator = src.item_separator;
  footer_skel    = src.footer_skel;
  skelguide      = src.skelguide;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSkeletonCore::AddMiddleToFile(QTextStream* outfile) const {
  // local variable
  QString buffer;
  // checks
  if (outfile==NULL) return false;
  // 3 extra lines to add
  buffer = EscapeNL(skelguide,IO_DELIMS);
  (*outfile) << buffer << '\n';
  buffer = EscapeNL(header_skel,IO_DELIMS);
  (*outfile) << buffer << '\n';
  buffer = EscapeNL(item_skel,IO_DELIMS);
  (*outfile) << buffer << '\n';
  buffer = EscapeNL(item_separator,IO_DELIMS);
  (*outfile) << buffer << '\n';
  buffer = EscapeNL(footer_skel,IO_DELIMS);
  (*outfile) << buffer << '\n';
  // done
  return AddMoreToFile(outfile);
}
//-----------------------------------------------------------------------------
bool jfSkeletonCore::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfSkeletonBase::ReadMiddleFromFile";
  assert(infile!=NULL);
  // we read the 5 lines
  if (!infile->ReadUnEsc(skelguide,funcname)) return false;
  if (!infile->ReadUnEsc(header_skel,funcname)) return false;
  if (!infile->ReadUnEsc(item_skel,funcname)) return false;
  if (!infile->ReadUnEsc(item_separator,funcname)) return false;
  if (!infile->ReadUnEsc(footer_skel,funcname)) return false;
  // readinf more
  return ReadMoreFromFile(infile);
}
/*****************************************************************************/
jfSkeletonBase::jfSkeletonBase():jfSkeletonCore() {
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper methods
// serves as a base for a copy constructor
void jfSkeletonBase::CopyOver(jfSkeletonBase& src) {
  jfSkeletonCore::CopyOver(src);
  // skeleton stuff
  cat_header     = src.cat_header;
  cat_footer     = src.cat_footer;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSkeletonBase::AddMoreToFile(QTextStream* outfile) const {
  // local variable
  QString buffer;
  // checks
  if (outfile==NULL) return false;
  // 2 extra lines to add
  buffer = EscapeNL(cat_header,IO_DELIMS);
  (*outfile) << buffer << '\n';
  buffer = EscapeNL(cat_footer,IO_DELIMS);
  (*outfile) << buffer << '\n';
  // done
  return true;
}
//-----------------------------------------------------------------------------
bool jfSkeletonBase::ReadMoreFromFile(jfFileReader* infile) {
  const QString funcname = "jfSkeletonBase::ReadMiddleFromFile";
  QString cline;
  // we read the 2 lines
  assert(infile!=NULL);
  if (!infile->ReadUnEsc(cat_header,funcname)) return false;
  return infile->ReadUnEsc(cat_footer,funcname);
}
/*****************************************************************************/
jfSkeletonSingle::jfSkeletonSingle():jfSkeletonBase() {
}
//---------------------------------
// the copy constructor
jfSkeletonSingle::jfSkeletonSingle(jfSkeletonSingle* src) {
  assert(src!=NULL);
  CopyOver(*src);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfSkeletonSingle::GetTypeID() const {
  return "SkeletonSingle";
}
//-------------------------------------------------------------------------------
jfs_Skeltype jfSkeletonSingle::GetSkelType() const {
  return jfs_SINGLE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSkeletonSingle::AddRestToFile(QTextStream* outfile) const {
  return true;
}
//-------------------------------------------------------------------------------
bool jfSkeletonSingle::ReadRestFromFile(jfFileReader* infile) {
  return true;
}
/*****************************************************************************/
jfSkeletonMultiple::jfSkeletonMultiple():jfSkeletonBase() {
}
//------------------------------------------------------------
// the copy constructor
jfSkeletonMultiple::jfSkeletonMultiple(jfSkeletonMultiple* src) {
  assert(src!=NULL);
  CopyOver(*src);
  catlink           = src->catlink;
  catlink_separator = src->catlink_separator;
  top_footer        = src->top_footer;
  cat_separator     = src->cat_separator;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfSkeletonMultiple::GetTypeID() const {
  return "SkeletonMultiple";
}
//--------------------------------------------------------------------------
jfs_Skeltype jfSkeletonMultiple::GetSkelType() const {
  return jfs_MULTIPLE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSkeletonMultiple::AddRestToFile(QTextStream* outfile) const {
  // local variable
  QString linkline,buffer;
  // checks
  if (outfile==NULL) return false;
  // adding the link lines
  buffer = EscapeNL(catlink,IO_DELIMS);
  buffer += ';' + EscapeNL(catlink_separator,IO_DELIMS);
  (*outfile) << buffer << '\n';
  // adding the next 4 lines
  buffer = EscapeNL(top_footer,IO_DELIMS);
  (*outfile) << buffer << '\n';
  buffer = EscapeNL(cat_separator,IO_DELIMS);
  (*outfile) << buffer << '\n';
  // done
  return true;
}
//-------------------------------------------------------------------------------
bool jfSkeletonMultiple::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfSkeletonMultiple::ReadRestFromFile";
  QString cline, buffer;
  // we read the first line
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,funcname)) return false;
  catlink = infile->lp.UnEscStr(0);
  catlink_separator = infile->lp.UnEscStr(1);
  // we read the next 2 lines
  if (!infile->ReadUnEsc(top_footer,funcname)) return false;
  return infile->ReadUnEsc(cat_separator,funcname);
}
//===========================================================================
// constructors
//--------------------
jfSkeletonFicPart::jfSkeletonFicPart():jfSkeletonCore() {}
//--------------------
jfSkeletonFicPart::jfSkeletonFicPart(jfSkeletonFicPart* src) {
  assert(src!=NULL);
  CopyOver(*src);
  grouplink = src->grouplink;
  grouplink_separator = src->grouplink_separator;
  partlink = src->partlink;
  partlink_separator = src->partlink_separator;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// methods
//--------------------
QString jfSkeletonFicPart::GetTypeID() const {
  return "SkeletonFicPart";
}
//--------------------
jfs_Skeltype jfSkeletonFicPart::GetSkelType() const {
  return jfs_FICPART;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSkeletonFicPart::AddRestToFile(QTextStream* outfile) const {
  // local variable
  jfOutString o_str;
  // checks
  if (outfile==NULL) return false;
  // adding the group line
  o_str << grouplink << grouplink_separator;
  (*outfile) << o_str << '\n';
  // adding the part line
  o_str.clear();
  o_str << partlink << partlink_separator;
  (*outfile) << o_str << '\n';
  // done
  return true;
}
//--------------------
bool jfSkeletonFicPart::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfSkeletonFicPart::ReadRestFromFile";
  QString cline, buffer;
  // we read the first line
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,funcname)) return false;
  grouplink = infile->lp.UnEscStr(0);
  grouplink_separator = infile->lp.UnEscStr(1);
  // we read the second line
  if (!infile->ReadParseLine(2,funcname)) return false;
  partlink = infile->lp.UnEscStr(0);
  partlink_separator = infile->lp.UnEscStr(1);
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
    // some extra stuff
bool jfSkeletonFicPart::AddMoreToFile(QTextStream* outfile) const {
  return true;
}
bool jfSkeletonFicPart::ReadMoreFromFile(jfFileReader* infile) {
  return true;
}
/*****************************************************************************/
jfHtmlParams::jfHtmlParams() {
  outfile = NULL;
  parse = NULL;
  base = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info
bool jfHtmlParams::CheckOk() const {
  if (base==NULL) {
    errmsg = "Base is NULL.";
    return false;
  }
  if (parse==NULL) {
    errmsg = "Parse is NULL.";
    return false;
  }
  if (outfile==NULL) {
    errmsg = "Outfile is NULL.";
    return false;
  }
  if (rescindex>33) {
    errmsg = "Result index is too high.";
    return false;
  }
  return true;
}
//--------------------------------------------------------------------------
QString jfHtmlParams::MakeResult(QString inskel) {
  assert(CheckOk());
  // doing the parsing
  parse->SetSkeleton(inskel);
  parse->ProduceIntermediate();
  parse->MakeResult();
  return parse->GetResult();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSkeletonSingle* jfHtmlParams::Single() {
  assert(base!=NULL);
  assert((base->GetSkelType())==jfs_SINGLE);
  return dynamic_cast<jfSkeletonSingle*>(base);
}
//-----------------------------------------------------------------
jfSkeletonMultiple* jfHtmlParams::Multiple() {
  assert(base!=NULL);
  assert((base->GetSkelType())==jfs_MULTIPLE);
  return dynamic_cast<jfSkeletonMultiple*>(base);
}
//-----------------------------------------------------------------
size_t jfHtmlParams::GetResCat() const {
  return rescindex;
}
//-----------------------------------------------------------------
bool jfHtmlParams::SetResCat(size_t inval) {
  if (inval>32) return false;
  rescindex = inval;
  return true;
}
//----------------------------------------------------------------
bool jfHtmlParams::AppendLine(const QString oline) {
  if (outfile==NULL) return false;
  (*outfile) << oline << "\n";
  return true;
}

/*****************************************************************************/
