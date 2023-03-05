/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_ficobj.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     May 25, 2010
// Conversion to Qt Started September 25, 2013
// Updated:     February 25, 2023
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
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
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
// ----------------------------
jfFFNItemCore::jfFFNItemCore(const jfFFNItemCore& src):jfGenericFanfic3(src) {
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
    result += " - Characters Specified: " + characters.join(", ");
  }
  // pairings
  if (RelationshipCount() > 0) {
      result += " - Pairings: " + RelationshipsAsDisplayString(true);
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

  const jfDisplayHTMLHelper* helper = HTMLHelpers::ffn_item_helper;
  // we start with the table
  result = DisplayHTMLHeader(1, helper);
  result += ("</td></tr>\n");
  // next line: link display
  result += ("<tr><td>");
  result += helper->WrapText("ficlink", primarylink, true);

  // adding the main description
  result += "<div style=\"margin-left:10px;\">";
  QString rdesc = (description.length() > 1000) ? (description.left(1000) + "...") : description;
  result += helper->WrapTextNoEsc("description",rdesc,false);
  result += "</div>";

  // next up.. two extra lines
  QString mdata = "Published: " + published.toString("MM-dd-yyyy");
  if (isupdated) {
    mdata += " - Updated: " + updated_date.toString("MM-dd-yyyy");
  }
  // the rating...
  if (rating==('+')) buffer = ("K+");
  else buffer = rating;
  mdata += (" - Rating: ") + buffer;
  // part count and word count
  mdata += " - Parts: " + QString::number(part_count);
  mdata += " - Words: " + QString::number(word_count);
  mdata += " - Favs: " + QString::number(favs);
  if (completed) {
    mdata += " - Complete";
  }
  result += helper->WrapText("basicinfo", mdata, true);

  // characters
  result += helper->ConditionalWrapText("characters", false,"Characters: ", true, characters.join(","), false);
  // relationships
  result += helper->ConditionalWrapText("pairings", true,"Pairings: ", true, RelationshipsAsDisplayString(true), false);
  // genres
  result += helper->ConditionalWrapText("genres", true, "Genres: ", true, genres, true);

  // finishing off
  result += ("</td></tr>\n</table>");
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
jfFFNItemCore::~jfFFNItemCore() {
    ClearPairingVector(char_pairs);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNItemCore::UpdateFromSource(const jfFFNItemCore* source) {
    if (source == NULL) return false;
    source->StoreToCopy3(this);
    RelationshipCopy(*source);
    language = source->GetLanguage();
    isupdated = source->IsUpdated();
    published = source->GetPublished();
    rating = source->GetRating();
    favs = source->GetFavs();
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNItemCore::ExtractDescription(jfStringParser &zparser, QString &parserr) {
  // going ahed
    if (!zparser.MovePastAlt("<div class=\"z-indent z-padtop\">","<div class='z-indent z-padtop'>")) {
        return failMsg(parserr,"Could not find start of description!");
    }
    if (!zparser.GetMovePast("<div ",description)) {
        return failMsg(parserr,"Could not find end of description!");
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
// -------------------------------------------------------------------------
// I find that to properly handle that character list, I need to split and keep the delimiters
QStringList jfFFNItemCore::SplitCharacters(const QString& source) const {
    const QChar comm = QChar(',');
    const QChar opbr = QChar('[');
    const QChar clbr = QChar(']');
    QStringList result;
    result.reserve(10);
    QString charbuf;
    charbuf.reserve(25);

    // we now go over the string character by character
    for (int ind = 0; ind < source.size(); ind++) {
        QChar curr = source.at(ind);
        if ((curr == comm) || (curr == opbr) || (curr == clbr)) {
            charbuf = charbuf.trimmed();
            if (charbuf.size() > 0) result.append(charbuf);
            charbuf = "";
            result.append(QString(curr));
        }
        else charbuf += curr;
    }
    if (charbuf.size() > 0) {
        charbuf = charbuf.trimmed();
        if (charbuf.size() > 0) result.append(charbuf);
    }
    return result;
}
// --------------------------------------------------------------------------
bool jfFFNItemCore::Characters_Pairs_and_Completion(jfStringParser &zparser, QString &parserr) {
    QString buffer, buffer2;
    QString raw_characters;

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
        raw_characters = buffer2;
    }
    // below, there must be only 1 field
    else {
      bool moref = zparser.GetToEnd(buffer);
      if (! moref) {
        return failMsg(parserr,"Strange error after dates!");
      }
      buffer = buffer.trimmed();
      // What is the field? I will assume there is no character known as 'Complete'
      if (buffer=="Complete") completed = true;
      else raw_characters = buffer;
    }

    // processing characters
    if (! raw_characters.isEmpty()) {
        QStringList char_parts = SplitCharacters(raw_characters);
        bool inpair = false;
        QStringList pair_members;

        // the list contains both characters and separators
        for (QString char_part : char_parts) {
            if (char_part == "[") {
                // pairing start
                inpair = true;
            }
            else if (char_part == ",") {
                // merely a divider
            }
            else if (char_part == "]") {
                if (inpair) {
                    // the pairing has come to an end, save the data
                    inpair = false;
                    if (pair_members.size() > 0) {
                        char_pairs.append(new jfPairingStruct(pair_members, false));
                        pair_members.clear();
                    }
                }
            }
            else {
                // character processing
                QString parsed_character = htmlparse::ConvertEntities(char_part,false);
                characters.append(parsed_character);
                if (inpair) pair_members.append(parsed_character);

            }
        }
        // to avoid discarding data if we end somehow still inside a pair...
        if (inpair) {
            if (pair_members.size() > 0) {
                char_pairs.append(new jfPairingStruct(pair_members, false));
                pair_members.clear();
            }
        }
    }
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
      if (!zparser.MovePastAlt("<span data-xutime=\"","<span data-xutime='")) {
          return failMsg(parserr,"Cannot find start of Updated Date");
      }
      if (!zparser.GetMovePastAltULong("\">","'>",xoutval,xoerr)) {
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
  if (!zparser.MovePastAlt("<span data-xutime=\"","<span data-xutime='")) {
      return failMsg(parserr,"Cannot find start of Published Date");
  }
  if (!zparser.GetMovePastAltULong("\">","'>",xoutval,xoerr)) {
    return failMsg(parserr,"Problem with Published Date :" + xoerr);
  }

  xdatetime.setTime_t((uint)(xoutval));
  published = xdatetime.date();
  if (!uset) updated_date = published;
  isupdated = uset;
  /* The next 2 fields are for character data, and completion status. Both of
  them are optional, so parsing is tricky */
  // first, there is always a dash if at least 1 field is there
  if (zparser.MovePast("-")) {
    return Characters_Pairs_and_Completion(zparser, parserr);
  }
  else return true;
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
//================================================================================
jfFFNItem::jfFFNItem():jfFFNItemCore() {
  ustatus = jud_NONE;
  cat_link = NULL;
}
//-------------------------------------------------------------------
// copy constructor
jfFFNItem::jfFFNItem(const jfFFNItem& src):jfFFNItemCore(src) {
    ustatus = jud_NONE;
    cat_link = src.cat_link;
}
//------------------------------------------------------------------------
jfFFNItem::jfFFNItem(const QString& instr, const jfFFN_CategoryCore* cat_linkin):jfFFNItemCore() {
    const QString fname = "jfFFNItem::jfFFNItem";
    cat_link = NULL;
    QString parse_err;
    if (!SetFromString(instr, cat_linkin,parse_err)) {
        jerror::ParseLog(fname,parse_err);
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

  // copying over the fields
  UpdateFromSource(src);

  // we set the new update status
  ustatus = jud_UPDATED;
  isupdated = true;

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
  LoadMoreValues3(inparser);
  LoadRelationships(inparser, true);
  inparser->AddText("ITEMF_LANGUAGE",language);
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parsing helper methods
bool jfFFNItem::GetLinkTitle(jfStringParser& zparser, QString& parseerr) {
    const QString fname = "jfFFNItem::GetLinkTitle";
    /**/JDEBUGLOG(fname,1);
  // variables
  unsigned long qval;
  QString buffer, oerr;
  bool tval;
  // starting
  rank = 0;
  /**/JDEBUGLOGB(fname,2, cat_link == NULL);
  tval = (cat_link->GetName()).contains("SEGA");
  // next up is the story id
/**/JDEBUGLOG(fname,3);
  if (!zparser.MovePast("<a class=\"stitle\" href=\"/s/")) {
  if (!zparser.MovePastAlt("<a  class=stitle href=\"/s/", "<a class=stitle href=\"/s/")) {
    return failMsg(parseerr,"Start of Fic ID not found!");
  }
  }

  /**/JDEBUGLOG(fname,4);
  if (!zparser.GetMovePastULong("/",qval,oerr)) {
    return failMsg(parseerr,oerr + " When getting fic id.");
  }
  /**/JDEBUGLOG(fname,3);
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
bool jfFFNItem::AddCodaStuff(QTextStream* outfile) const  {
  QString xline;
  size_t xval;
  xval = (size_t)(jud_NONE);
  xline = QString::number(xval);
  (*outfile) << xline << "\n";
  return true;
}
//-------------------------------------------------------------
bool jfFFNItem::ReadCodaStuff(jfFileReader* infile) {
  const QString funcname = ("jfFFNItem::ReadCodaStuff");
  // local variables
  unsigned long oval;
  // starting with line 4
  /**/JDEBUGLOG(funcname,1);
  jerror::AssertLog(infile!=NULL,funcname,"infile reader is NULL");
  if (!infile->ReadULong(oval,funcname)) return false;
  /**/JDEBUGLOGU(funcname,3,oval);
  ustatus = (jf_FUpdateStatus)oval;
  return true;
}
//*********************************************************************************



