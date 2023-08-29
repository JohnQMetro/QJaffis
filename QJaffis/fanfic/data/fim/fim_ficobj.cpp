/******************************************************************************
Name    :   fim_ficobj.cpp
Author  :   John Q Metro
Purpose :   Fanfic object for fimfiction.net
Created :   May 8, 2012
Conversion to QT started : April 20, 2013
Updated :   March 18, 2023
******************************************************************************/
#ifndef FIM_FICOBJ_H_INCLUDED
  #include "fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED


#ifndef STRINGPARSER_H_INCLUDED
  #include "../../../core/utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
#include <QDateTime>

/*****************************************************************************/
const QString& IdForFIMFanfics() {
    static QString FIM_FANFIC_TYPE_ID = QString("FIM_Fanfic");
    return FIM_FANFIC_TYPE_ID;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFIM_Fanfic::jfFIM_Fanfic():jfGenericFanfic3() {
    type_labels.append(IdForFIMFanfics());
    thumbsup = 0;
    thumbsdown = 0;
}
// -------------------------------------
jfFIM_Fanfic::jfFIM_Fanfic(const jfSearchResultItemData& init_data):jfGenericFanfic3(init_data) {
    type_labels.append(IdForFIMFanfics());
    thumbsup = 0;
    thumbsdown = 0;
}
//----------------------------------------------
jfFIM_Fanfic::jfFIM_Fanfic(const jfFIM_Fanfic& src):jfGenericFanfic3(src) {
    type_labels.append(IdForFIMFanfics());
    thumbsup = src.thumbsup;
    thumbsdown = src.thumbsdown;
    rating = src.rating;
    pubdate = src.pubdate;
    compact_summary = src.compact_summary;
    warnings = src.warnings;
    content_types = src.content_types;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIM_Fanfic::SetCompactSummary(const QString& insum) {
  compact_summary = insum;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
//-----------------------------------------------------------------------
const QString& jfFIM_Fanfic::GetContentTypes() const {
    return content_types;
}
//-----------------------------------------------------------------------
const QString& jfFIM_Fanfic::GetWarnings() const {
    return warnings;
}
//-----------------------------------------------------------------------
int jfFIM_Fanfic::GetThumbs(bool down) const {
  if (down) return thumbsdown;
  else return thumbsup;
}
//-----------------------------------------------------------------------
bool jfFIM_Fanfic::AreRatingsDisabled() const {
    return (thumbsup < 0);
}
//--------------------------------------------------------------------------
QString jfFIM_Fanfic::GetRating() const {
  return rating;
}
//------------------------------------------------------------------------
const QString& jfFIM_Fanfic::GetCompactSummary() const {
  return compact_summary;
}
// ------------------------------------------------------
QDate jfFIM_Fanfic::GetPublishedDate() const {
    return pubdate;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special url construction
//----------------------------------------------
QString jfFIM_Fanfic::MakeAuthorUrl() const {
  QString result = "https://www.fimfiction.net/user/";
  result += author_name;
  return result;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFIM_Fanfic::LoadValuesContinued(jfSkeletonParser* inparser) const {
    inparser->AddText("ITEMF_CONTENTTYPES",content_types);
    inparser->AddText("ITEMF_WARNINGS",warnings);
    inparser->AddText("ITEMF_RATING",rating);
    inparser->AddBool("THUMBS_DISABLED",(thumbsup < 0));
    inparser->AddUInt("THUMBSUP",(thumbsup< 0)?0:thumbsup);
    inparser->AddUInt("THUMBSDOWN",thumbsdown);
    inparser->AddUInt("THUMBSDOWN",thumbsdown);
    inparser->AddText("ITEMF_PUBDATE",pubdate.toString("MM-dd-yyyy"));
    return true;
}
//--------------------------------------------------
void jfFIM_Fanfic::StoreToCopyFIMFanfic(jfFIM_Fanfic* destination) const {
    assert(destination!=NULL);
    StoreToCopyGenericFanfic3(destination);
    destination->content_types = content_types;
    destination->warnings = warnings;
    destination->thumbsup = thumbsup;
    destination->thumbsdown = thumbsdown;
    destination->rating = rating;
    destination->pubdate = pubdate;
    destination->compact_summary = compact_summary;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// special methods
//----------------------------------------------

jfFicExtract* jfFIM_Fanfic::MakeExtract() const {
  jfFicExtract_FIM* result;
  result = new jfFicExtract_FIM();
  LoadIntoExtract(result);
  return result;
}
//----------------------------------------------
void jfFIM_Fanfic::LoadIntoExtract(jfFicExtract* into) const {
  // the basics
  assert(into!=NULL);
  LoadIntoExtractCore(into);
  // more stuff
  into->auth_id = author_id;
  into->complete = completed;
  // fim specific stuff
  jfFicExtract_FIM* typed_ext = dynamic_cast<jfFicExtract_FIM*>(into);
  typed_ext->wordcount = word_count;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//--------------------------------
bool jfFIM_Fanfic::AddMoreExtraStuff(QTextStream* outfile) const {
  // local variables
  QString result;
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;

  // next line (line 8)
  // content type and warnings
  xresult << content_types << warnings;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // line after that is more complicated
  xresult << rating << thumbsup << thumbsdown << pubdate;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // compact summary line
  xresult << compact_summary;
  (*outfile) << xresult << "\n";
  // done
  return true;
}
//--------------------------------
bool jfFIM_Fanfic::ReadMoreExtraStuff(jfFileReader* infile) {
  const QString funcname = "jfFIM_Fanfic::ReadExtraStuff";
  // content type and warnings (line 8)
  if (!infile->ReadParseLine(2,funcname)) return false;
  content_types = (infile->lp).UnEscStr(0);
  warnings = (infile->lp).UnEscStr(1);
  // the next line
  if (!infile->ReadParseLine(3,4,funcname)) return false;
  rating = infile->lp.UnEscStr(0);
  if (!infile->lp.IIntVal(1,thumbsup)) return infile->BuildError("The thumbsup is not valid!");
  if (!infile->lp.IIntVal(2,thumbsdown)) return infile->BuildError("The thumbsdown is not valid!");
  // the optional published date value
  if ((infile->lp.Num())==4) {
    if (!infile->lp.DateVal(2,pubdate)) {
      return infile->BuildError("Unable to convert the Published field properly");
    }
  }
  else {
    pubdate = QDate::currentDate();
    pubdate.setDate(1984,pubdate.month(),pubdate.day());
  }
  // the next line
  if (!infile->ReadUnEsc(compact_summary,funcname)) return false;
  return true;
}
/*****************************************************************************/
