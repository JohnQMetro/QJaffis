/******************************************************************************
Name    :   ficbase.cpp
Author  :   John Q Metro
Purpose :   Defines base fanfic objects that hold fanfic info
Created :   April 2, 2012
Conversion to QT started : April 20, 2013
Updated :   December 28, 2014
Renamed from ficobj2.cpp
******************************************************************************/
#ifndef FICBASE_H
  #include "ficbase.h"
#endif // FICBASE_H

#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <assert.h>
//****************************************************************************
jfGenericFanfic::jfGenericFanfic():jfBasePD() {
  ustatus = jud_NONE;
  validdata = false;
  part_count = 1;
}
//--------------------------------------------------------------------------
jfGenericFanfic::jfGenericFanfic(const jfGenericFanfic& src) {
  // stuff from jfBaseObj
  src.CopyOver(this);
  // new stuff for FFN Fic objects
  part_count = src.part_count;
  updated_date = src.updated_date;
  author_name = src.author_name;
  author_url = src.author_url;
  // update status
  ustatus = src.ustatus;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting values
//-----------------------------------------------------------------------
QString jfGenericFanfic::GetAuthor() const {
  return author_name;
}
//-----------------------------------------------------------------------
QString jfGenericFanfic::GetAuthorUrl() const {
  return author_url;
}
//-----------------------------------------------------------------------
size_t jfGenericFanfic::GetChapterCount() const {
  return part_count;
}
//-----------------------------------------------------------------------
QDate jfGenericFanfic::GetUpdates() const {
  return updated_date;
}
//-----------------------------------------------------------------------
QString jfGenericFanfic::GetPDesc(size_t elen) const {
  QString slice;
  if (description.length()<elen) return description;
  else {
    slice = description.left(elen).trimmed();
    slice += " ...";
    return slice;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting the extract
jfFicExtract* jfGenericFanfic::GetExtract() const {
  jfFicExtract* result_val = MakeExtract();
  result_val->author_name = author_name;
  result_val->fic_id = num_id;
  result_val->pcount = part_count;
  result_val->fic_title = name;
  result_val->updated_date = updated_date;
  LoadIntoExtract(result_val);
  return result_val;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGenericFanfic::SetMissing() {
  ustatus = jud_MISSING;
}
//-------------------------------------------------------------------------
void jfGenericFanfic::SetNew(bool ival) {
  if (ival) ustatus = jud_NEW;
  else ustatus = jud_NONE;
}
//-------------------------------------------------------------------
bool jfGenericFanfic::ChangeAuthorname(QString newname) {
  if (newname!=author_name) {
    author_name = newname;
    ustatus = jud_AUTHORNC;
    author_url = MakeAuthorUrl();
    return true;
  }
  else return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGenericFanfic::LoadMoreValues1(jfSkeletonParser* inparser) const {
  // checking
  assert(inparser!=NULL);
  // values
  inparser->AddText("ITEMF_AUTHORNAME",author_name);
  inparser->AddText("ITEMF_AUTHORURL",author_url);
  inparser->AddUInt("ITEMF_CHAPTERCOUNT",part_count);
  inparser->AddText("ITEMF_UPDDATE",updated_date.toString("MM-dd-yyyy"));
  // status
  inparser->AddText("ITEMF_USTATUS",MakeUStatus());
}
//--------------------------------------------------------------
void jfGenericFanfic::StoreToCopy(jfGenericFanfic* destination) const {
  assert(destination!=NULL);
  CopyOver(destination);
  destination->author_name = author_name;
  destination->part_count = part_count;
  destination->updated_date = updated_date;
  destination->ustatus = ustatus;
  destination->author_url = author_url;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfGenericFanfic::MakeUStatus() const {
  QString result;
  if (ustatus==jud_NONE) return "";
  else {
    // the span indicating the text color
    result = "<span style=\"color:";
    if (ustatus==jud_UPDATED) result += "lime";
    else if (ustatus==jud_MISSING) result += "red";
    else if (ustatus==jud_NEW) result += "aqua";
    else if (ustatus==jud_AUTHORNC) result += "yellow";
    else assert(false);
    // middle part
    result += "\">[";
    // the label
    result += UpdateStatus2String(ustatus);
    // finishing
    result += "]</span>";
  }
  // done
  return result;
}
//--------------------------------------------------------------
QString jfGenericFanfic::DisplayHTMLHeader(size_t mindex) const {
  const QString fname = "jfGenericFanfic::DisplayHTMLHeader";
  // asserts
  // assert(mindex<fms_count);
  assert(mindex<3);
  assert(validdata);
  // varibales
  QString result,texmpx;
  // building the result
  // we start with the table
  result = "<table width=99%";
  // background color based on fic marks...
  texmpx = GetMarkColor(mindex);
  if (!texmpx.isEmpty()) result += " bgcolor=\"#" + texmpx + "\"";
  result += "><tr><td>";
  // building the title line
  result += "<font size=+3 color=#010101><a href=\"";
  result += primarylink + "\">";
  result += name + "</a>";
  // adding the author stuff
  result += " by " + author_name;
  result += "</font> ";
  // status
  if (ustatus!=jud_NONE) {
    result += "<font size=+2 color=";
    if (ustatus==jud_UPDATED) result += "Lime><b>[Updated";
    else if (ustatus==jud_MISSING) result += "Red><b>[Missing";
    else if (ustatus==jud_NEW) result += "Aqua><b>[New";
    else if (ustatus==jud_AUTHORNC) result += "Yellow><b>[Author Name Change";
    result += "!]</b></font>";
  }
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// core extract methods
void jfGenericFanfic::LoadIntoExtractCore(jfFicExtract* into) const {
  // checks
  assert(into!=NULL);
  into->author_name = author_name;
  into->fic_id = num_id;
  into->pcount = part_count;
  into->fic_title = name;
  into->updated_date = updated_date;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// fic mark methods
//------------------------------------------
bool jfGenericFanfic::IsMarked(size_t mindex) const {
  /*
  assert(mindex<fms_count);
  return (ficm::data->fd)[mindex]->ContainsID(num_id);
  */
  return false;
}
//------------------------------------------
QString jfGenericFanfic::GetMarkColor(size_t mindex) const {
    /*jf_FicMark rval;
    assert(mindex<fms_count);
    rval = (ficm::data->fd)[mindex]->GetValue(num_id);
    return fmcolors[(size_t)rval];
    */
  return "";
}
//===========================================================================
jfGenericFanfic2::jfGenericFanfic2():jfGenericFanfic() {
  word_count = 0;
  completed = false;
}
//--------------------------------------------------------
jfGenericFanfic2::jfGenericFanfic2(const jfGenericFanfic2& src):jfGenericFanfic(src) {
  word_count = src.word_count;
  completed = src.completed;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfGenericFanfic2::GetWordcount() const {
  return word_count;
}
//--------------------------------------------------------
bool jfGenericFanfic2::IsComplete() const {
  return completed;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGenericFanfic2::LoadMoreValues2(jfSkeletonParser* inparser) const {
  assert(inparser!=NULL);
  inparser->AddUInt("ITEMF_WORDCOUNT",word_count);
  inparser->AddBool("ITEMF_COMPLETED",completed);
}
//===========================================================================
// default constructors
//----------------------------------------------------------
jfGenericFanfic3::jfGenericFanfic3():jfGenericFanfic2() {
  author_id = 0;
}
//----------------------------------------------------------
jfGenericFanfic3::jfGenericFanfic3(const jfGenericFanfic3& src):jfGenericFanfic2(src) {
  author_id = src.author_id;
  genres = src.genres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data getting methods
QString jfGenericFanfic3::GetGenres() const {
  return genres;
}
//----------------------------------------------------------------------------
size_t jfGenericFanfic3::GetAuthorID() const {
  return author_id;
}
//-------------------------------------------------------
void jfGenericFanfic3::ChangeAuthorID(size_t newid) {
  author_id = newid;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // file i/o output
    //----------------------------------------------------------
bool jfGenericFanfic3::AddRestToFile(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // preparing line 4
  xresult << author_id << author_name << author_url;
  (*outfile) << xresult << "\n";
  xresult.clear();
  // doing line 5
  xresult << part_count << genres << updated_date;
  (*outfile) << xresult << "\n";
  xresult.clear();
  // doing line 6
  xresult << word_count << completed;
  (*outfile) << xresult << "\n";
  // done
  return AddExtraStuff(outfile);
}
//----------------------------------------------------------
bool jfGenericFanfic3::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfGenericFanfic3::ReadRestFromFile";
  const QString inan = " is not a number!";
  // local variables
  QString buffer;
  // starting
  assert(infile!=NULL);
  // next line
  if (!infile->ReadParseLine(3,funcname)) return false;
  if (!infile->lp.SIntVal(0,author_id)) return infile->BuildError("The author id"+inan);
  author_name = infile->lp.UnEscStr(1);
  author_url = infile->lp.UnEscStr(2);
  // line after that
  if (!infile->ReadParseLine(3,funcname)) return false;
  if (!infile->lp.SIntVal(0,part_count)) return infile->BuildError("The part count"+inan);
  genres = infile->lp.UnEscStr(1);
  // the next value of line 5 is a bit more complicated because it is a date
  buffer = infile->lp.UnEscStr(2);
  updated_date = QDate::fromString(buffer,"M'-'d'-'yy");
  if (!updated_date.isValid()) {
    return infile->BuildError("Unable to convert the Updated Date field properly");
  }
  // next line
  if (!infile->ReadParseLine(2,funcname)) return false;
  if (!infile->lp.SIntVal(0,word_count)) return infile->BuildError("The word_count is not a number!");
  if (!infile->lp.BoolVal(1,completed)) return infile->BuildError("The completed value is not boolean!");
  // done with the lines
  return ReadExtraStuff(infile);
}
//****************************************************************************

