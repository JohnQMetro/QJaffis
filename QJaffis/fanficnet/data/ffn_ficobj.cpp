/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_ficobj.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     May 25, 2010
// Conversion to Qt Started September 25, 2013
// Updated:     July 8, 2016 (change base to jfGenericFanfic3)
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
// constructors
//---------------------------------------
jfFFNItemCore::jfFFNItemCore():jfGenericFanfic3() {
  favs = 0;
  isupdated = false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//---------------------------------------
QString jfFFNItemCore::ToText() const {
  QString result,buffer;
  // building the title and author line
  result = name + " by " + author_name + "\n";
  // next line: link display
  result += primarylink + "\n";
  // adding the main description
  result += description + "\n";
  // next up.. two extra lines
  result += "Published: ";
  result += published.toString("MM-dd-yyyy");
  if (isupdated) {
    result += " - Updated: " + updated_date.toString("MM-dd-yyyy");
  }
  // the rating...
  if (rating=='+') buffer = "K+";
  else buffer = rating;
  result += " - Rating: " + buffer;
  // part count and word count
  result += " - Parts: " + QString::number(part_count);
  result += " - Words: " + QString::number(word_count);
  result += " - Favs: " + QString::number(favs) + "\n";
  // final info,
  result += language;
  if (!genres.isEmpty()) {
    result += " - Genres: " + genres;
  }
  // character data
  if (!genres.isEmpty()) {
    result += " - Characters Specified: " + char_data;
  }
  if (completed) {
    result += " - Complete";
  }
  // finishing off
  result += "\n";
  // done
  return result;
}
//---------------------------------------
QString jfFFNItemCore::ToDisplayHTML() const {
  const QString fname = ("jfFFNItem::ToDisplayHTML");
  QString result,buffer;
  // we start with the table
  result = ("<table width=99%><tr><td>");
  // building the title line
  result += ("<font size=+2 color=#010101><a href=\"");
  result += primarylink + ("\">");
  result += name + ("</a>");
  // adding the author stuff
  result += (" by ") + author_name;
  result += ("</font> ");
  // status
  if (ustatus!=jud_NONE) {
    result += ("<font size=+1 color=");
    if (ustatus==jud_UPDATED) result += ("Lime><b>[Updated");
    else if (ustatus==jud_MISSING) result += ("Red><b>[Missing");
    else if (ustatus==jud_NEW) result += ("Aqua><b>[New");
    else if (ustatus==jud_AUTHORNC) result += ("Yellow><b>[Author Name Change");
    result += ("!]</b></font>");
  }
  result += ("</td></tr>\n");
  // next line: link display
  result += ("<tr><td>");
  result += ("<font color=\"green\">");
  result += primarylink + ("</font><br>\n");
  // adding the main description
  result += "<font color=#010101>" + description + ("<br>\n");
  // next up.. two extra lines
  result += ("<font color=gray>Published: ");
  result += published.toString("MM-dd-yyyy");
  if (isupdated) {
    result += " - Updated: " + updated_date.toString("MM-dd-yyyy");
  }
  // the rating...
  if (rating==('+')) buffer = ("K+");
  else buffer = rating;
  result += (" - Rating: ") + buffer;
  // part count and word count
  result += " - Parts: " + QString::number(part_count);
  result += " - Words: " + QString::number(word_count);
  result += " - Favs: " + QString::number(favs) + "</font><br>\n";
  // final info,
  result += ("<font color=gray>");
  result += language;
  if (!genres.isEmpty()) {
    result += (" - Genres: ") + genres;
  }
  // character data
  if (!genres.isEmpty()) {
    result += " - Characters Specified: " + char_data;
  }
  if (completed) {
    result += " - Complete";
  }
  // finishing off
  result += ("</font></td></tr>\n</table>");
  // done
  return result;
}
//---------------------------------------
QString jfFFNItemCore::MakeAuthorUrl() const {
  QString result = ("https://www.fanfiction.net/u/");
  result += QString::number(author_id) + ("/");
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data getting methods
//---------------------------------------
QString jfFFNItemCore::GetCharacter() const {
  return char_data;
}
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
QString jfFFNItemCore::GetLanguage() const {
  return language;
}
//----------------------------------------------
size_t jfFFNItemCore::GetFavs() const {
  return favs;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFNItemCore::ProcessDescription() {
  // fanfiction.net does not put anything wierd in the descriptions
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNItemCore::ExtractDescription(jfStringParser &zparser, QString &parserr) {
  // going ahed
  if (!(zparser.GetDelimited("<div class='z-indent z-padtop'>","<div ",description))) {
    return failMsg(parserr,"Description not found!");
  }
  return true;
}
//-------------------------------------------------
bool jfFFNItemCore::Rating2Words(jfStringParser &zparser, QString &parserr) {
  // varibales
  QString buffer,buffer2;
  QString xerr;
  unsigned long qval;
  // starting
  // extrating the rating
  if (!(zparser.GetDelimited("Rated:","-",buffer))) return failMsg(parserr,"Rating not found!");
  buffer = buffer.trimmed();
  if (buffer=="K+") rating = '+';
  else {
    if (buffer.length()!=1) return failMsg(parserr,"Rating is malformed!");
    rating = buffer[0];
  }
  // next is the language
  if (!zparser.GetMovePast("- ",buffer)) return failMsg(parserr,"Language section not found!");
  language = buffer.trimmed();
  // next is the genres, or possibly the chapters
  if (!zparser.StartsWithAtIndex("Chapters:")) {
    if (zparser.GetMovePast("Sci-",buffer2)) buffer2+="Sci-";
    else buffer2 = "";
    if (!zparser.GetMovePast("-",buffer)) return failMsg(parserr,"Genres section not found!");
    genres = buffer2 + buffer;
  }
  // Chapter count
  if (!zparser.GetDelimitedULong("Chapters:","-", qval,xerr)) {
    return failMsg(parserr,xerr + " When getting chapter count.");
  }
  part_count = qval;
  // word count
  if (!zparser.GetDelimitedULong("Words:","-", qval,xerr)) {
    return failMsg(parserr,xerr + " When getting word count.");
  }
  word_count = qval;
  // number of favs
  if (!zparser.GetDelimitedULong("Favs:","-", qval,xerr)) {
    favs = 0;
  }
  else favs = qval;
  return true;
}
//----------------------------------------------------------------------------
bool jfFFNItemCore::Dates_and_Completion(jfStringParser &zparser, QString &parserr) {
  // varibales
  QString buffer,xoerr,buffer2;
  bool uset, moref;
  unsigned long xoutval;
  QDateTime xdatetime;
  // starting
  moref = false;
  uset = false;
  // next is the optional Updated Section
  if (zparser.MovePast("Updated:")) {
    if (!zparser.GetDelimitedULong("<span data-xutime='","'>",xoutval,xoerr)) {
      return failMsg(parserr,"Problem with Updated Date :" + xoerr);
    }
    xdatetime.setTime_t((uint)(xoutval));
    updated_date = xdatetime.date();
    uset = true;
  }
  // the published section is next
  if (!zparser.MovePast("Published:")) {
    return failMsg(parserr,"Published section is not found!");
  }
  if (!zparser.GetDelimitedULong("<span data-xutime='","'>",xoutval,xoerr)) {
    return failMsg(parserr,"Problem with Published Date :" + xoerr);
  }
  xdatetime.setTime_t((uint)(xoutval));
  published = xdatetime.date();
  if (!uset) updated_date = published;
  isupdated = uset;
  /* The next 2 fields are for character data, and completion status. Both of
  them are optional, so parsing is tricky */
  // first, there is always a dash if at least 1 field is there
  // x_parser->GetToEnd(buffer);
  if (zparser.MovePast("-")) {
    zparser.GetToEnd(buffer);
    // If we do have fields, the split is always at the last dash, using a custom method...
    if (zparser.GetAfterLast("-",buffer)) {
      // buffer 2 is character data
      buffer2 = zparser.GetSkipped();
      buffer2 = buffer2.trimmed();
      // buffer is probably completion
      buffer = buffer.trimmed();
      if (buffer=="Complete") completed = true;
      // but maybe not, if the name has a dash...
      else buffer2 = buffer2 + "-" + buffer;
      char_data = buffer2;
    }
    // below, there must be only 1 field
    else {
      moref = zparser.GetToEnd(buffer);
      if (!moref) {
        return failMsg(parserr,"Strange error after dates!");
      }
      buffer = buffer.trimmed();
      // What is the field? I will assume there is no character known as 'Complete'
      if (buffer=="Complete") completed = true;
      else char_data = buffer;
    }
  }
  // done
  return true;
}
//-----------------------------
bool jfFFNItemCore::failMsg(QString &pto, const QString message) const {
  pto = "PARSE ERROR: " + message;
  return false;
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
bool jfFFNItemCore::AddExtraStuff(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // preparing line 4, lots of data
  xresult << language << QString(rating) << char_data;
  (*outfile) << xresult << "\n";
  xresult.clear();
  // line 5 is more stuff
  xresult << favs << isupdated << published;
  (*outfile) << xresult << "\n";
  // done
  return AddMoreExtraStuff(outfile);
}
//---------------------------------------
bool jfFFNItemCore::ReadExtraStuff(jfFileReader* infile) {
  const QString funcname = ("jfFFNItemCore::ReadRestFromFile");
  // local variables
  QString buffer;
  // starting with line 4
  assert(infile!=NULL);
  /**/JDEBUGLOG(funcname,1);
  if (!infile->ReadParseLine(3,funcname)) return false;
  language = (infile->lp).UnEscStr(0);
  buffer = (infile->lp).UnEscStr(1);
  rating = buffer[0];
  char_data = (infile->lp).UnEscStr(2);

  // line 5, reading more data
  /**/JDEBUGLOG(funcname,2);
  if (!infile->ReadParseLine(3,funcname)) return false;
  if (!infile->lp.SIntVal(0,favs)) return infile->BuildError("The favorites count is not a number!");
  if (!infile->lp.BoolVal(1,isupdated)) return infile->BuildError("The updated flag is not valid!");
  // the next two values of line 5 are a bit more complicated because they are dates
  if (!infile->lp.DateVal(2,published)) return infile->BuildError("Unable to convert the Published field properly");

  // done with the lines
  /**/JDEBUGLOG(funcname,3);
  return ReadMoreExtraStuff(infile);
}
//================================================================================
jfFFNItem::jfFFNItem():jfFFNItemCore() {
  ustatus = jud_NONE;
  cat_link = NULL;
}
//-------------------------------------------------------------------
// copy constructor
jfFFNItem::jfFFNItem(const jfFFNItem& src) {
  // stuff from jfGenericFanfic
  src.StoreToCopy(this);
  // new stuff for FFN Fic objects
  language = src.language;
  genres = src.genres;
  word_count = src.word_count;
  completed = src.completed;
  isupdated = src.isupdated;
  published = src.published;
  rating = src.rating;
  char_data = src.char_data;
  author_id = src.author_id;
}
//------------------------------------------------------------------------
jfFFNItem::jfFFNItem(const QString& instr, const jfFFN_CategoryCore* cat_linkin):jfFFNItemCore() {
  cat_link = NULL;
  QString parse_err;
  if (!SetFromString(instr, cat_linkin,parse_err)) {
    /**/jfXLogString(parse_err);
    validdata = false;
  }
  ustatus = jud_NONE;
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
//---------------------------------------
/*
 style='min-height:63px;border-bottom:1px #cdcdcd solid;'>
 <img class='cimage' style='clear:left;float:left;margin-right:3px;padding:2px;border:1px solid #ccc;-moz-border-radius:2px;-webkit-border-radius:2px;' src='http://b.fanfiction.net/imageu/2384942/40/' width=40 height=59>
  <a href="/s/8189704/1/Harry_Potter_and_the_Ninja_Village">Harry Potter and the Ninja Village</a>
   by <a href="/u/2384942/Animewar">Animewar</a>
	<div class='z-indent z-padtop'>what if Harry grows up in the Shinobi world with Naruto, and what will happen to Hogwarts when he gets their?<div class='z-padtop2' style='color:gray;'>Rated: T - English - Chapters: 1 - Words: 598 - Published: 6-6-12 - Harry P. & Naruto U.
*/
bool jfFFNItem::SetFromString(const QString& instr, const jfFFN_CategoryCore* cat_linkin, QString& parserr) {
  // local variables
  QString buffer, oerr;
  unsigned long qval;
  jfStringParser xparser;
  // we start
  validdata = false;
  completed = false;
  // we check the inlink first
  if (cat_linkin==NULL) return failMsg(parserr,"Category Link is NULL");
  else cat_link = cat_linkin;
  // start parsing
  xparser.ChangeData(instr);
  // processing fic id, title, and making the link
  if (!GetLinkTitle(xparser,parserr)) return false;
  // the author id
  if (!xparser.GetDelimitedULong("<a href=\"/u/","/",qval,oerr)) {
    /* in fanfiction.net, you can sometimes comes across unreachable stories with
    no author (because the actual author id has been deleted somehow). these should
    be discarded, they are not a sign that parsing is wrong */
    included = false;
    return failMsg(parserr,oerr + " When getting author id.");
  }
  author_id = qval;
  // the author
  if (!(xparser.GetDelimited("\">","</a>",buffer)))return failMsg(parserr,"Author not found!");
  author_name = buffer;
  // next, the description!
  if (!ExtractDescription(xparser,parserr)) return false;
  // next, the rating, language, genres, chapter count, and word count
  if (!Rating2Words(xparser,parserr)) return false;

  // we skip past the reviews
  if (!xparser.MovePast("Reviews:")) {
    // this is okay, sometimes there are no reviews
  }
  if (!Dates_and_Completion(xparser,parserr)) return false;
  // done with all the parsing, just some finishing up...
  author_url = MakeAuthorUrl();
  validdata = true;
  return true;
}
//--------------------------------------------------------------------
// updates the item from src (if valid)
bool jfFFNItem::UpdateFromObj(const jfFFNItemCore* src) {
  // invalid cases
  if (src==NULL) return false;
  if (num_id!=(src->GetID())) return false;
  // we set the new update status
  ustatus = jud_UPDATED;
  // copying over stuff
  name = src->GetName();
  description = src->GetDescription();
  primarylink = src->GetUrl();
  rank = src->GetRank();
  genres = src->GetGenres();
  word_count = src->GetWordcount();
  part_count = src->GetChapterCount();
  completed = src->IsComplete();
  isupdated = true;
  updated_date = src->GetUpdates();
  char_data = src->GetCharacter();
  rating = src->GetRating();
  author_name = src->GetAuthor();
  // I will assume, for now, that author and language cannot change
  return true;
}
//-----------------------------------------------------------------
bool jfFFNItem::UpdateWordcount(const jfFFNItemCore* src) {
  // invalid cases
  if (src==NULL) return false;
  if (num_id!=(src->GetID())) return false;
  // changing the word count
  rating = src->GetRating();
  word_count = src->GetWordcount();
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNItem::GetTypeID() const {
  return ("FFNItem");
}
//---------------------------------------
// loads various internal data into the skeleton parser
bool jfFFNItem::LoadValues(jfSkeletonParser* inparser) const {
  QString rbuffer;
  LoadCoreValues(inparser);
  LoadMoreValues1(inparser);
  LoadMoreValues2(inparser);
  inparser->AddText("ITEMF_LANGUAGE",language);
  inparser->AddText("ITEMF_GENRES",genres);
  inparser->AddText("ITEMF_CHARDATA",char_data);
  inparser->AddUInt("ITEMF_AUTHORID",author_id);
  if (rating==('+')) rbuffer = ("K+");
  else rbuffer = rating;
  inparser->AddText("ITEMF_RATING",rbuffer);
  // we add the dates last
  inparser->AddText("ITEMF_PUBDATE",published.toString("MM-dd-yyyy"));
  inparser->AddBool("ITEMF_ISUPDATED",isupdated);
  inparser->AddUInt("ITEMF_FAVS",favs);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNItem::GetCatString() const {
  return cat_link->GetName();
}
//----------------------------------------------------
/*
jfAuthorInfo* jfFFNItem::GetAuthorInfo() const {
  jfAuthorInfo* result;
  result = new jfAuthorInfo(author_id,author_name);
  return result;
}
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parsing helper methods
bool jfFFNItem::GetLinkTitle(jfStringParser& zparser, QString& parseerr) {
  // variables
  unsigned long qval;
  QString buffer, oerr;
  bool tval;
  // starting
  rank = 0;
  tval = (cat_link->GetName()).contains("SEGA");
  // next up is the story id
  if (!zparser.MovePastAlt("<a  class=stitle href=\"/s/", "<a class=stitle href=\"/s/")) {
    return failMsg(parseerr,"Start of Fic ID not found!");
  }
  if (!zparser.GetMovePastULong("/",qval,oerr)) {
    return failMsg(parseerr,oerr + " When getting fic id.");
  }
  num_id = qval;
  // we get the rest of the url
  if (!zparser.GetMovePast("\">",buffer)) return failMsg(parseerr,"End of fic link not found!");
  // we then build the url...
  primarylink = "https://www.fanfiction.net/s/" + QString::number(num_id);
  primarylink += (QString("/") + buffer);
  // moving past the image
  if (!zparser.MovePastTwo("<img ",">")) return failMsg(parseerr,"Fic image not found!");
  // next up is the title
  if (!zparser.GetMovePast("</a>",buffer)) {
    // some sort of parsing bug for a particular sega story, I cannot seem to fix, so workaround
    if (tval) included = false;
    return failMsg(parseerr,"End of title field not found!");
  }
  name = buffer;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNItem::AddMoreExtraStuff(QTextStream* outfile) const  {
  QString xline;
  size_t xval;
  xval = (size_t)(jud_NONE);
  xline = QString::number(xval);
  (*outfile) << xline << "\n";
  return true;
}
//-------------------------------------------------------------
bool jfFFNItem::ReadMoreExtraStuff(jfFileReader* infile) {
  const QString funcname = ("jfFFNItem::ReadExtraStuff");
  // local variables
  unsigned long oval;
  // starting with line 4
  /**/JDEBUGLOG(funcname,1);
  assert(infile!=NULL);
  /**/JDEBUGLOG(funcname,2);
  if (!infile->ReadULong(oval,funcname)) return false;
  /**/JDEBUGLOGU(funcname,3,oval);
  ustatus = (jf_FUpdateStatus)oval;
  return true;
}
//*********************************************************************************



