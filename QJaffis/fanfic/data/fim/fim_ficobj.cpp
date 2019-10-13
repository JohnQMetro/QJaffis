/******************************************************************************
Name    :   fim_ficobj.cpp
Author  :   John Q Metro
Purpose :   Fanfic object for fimfiction.net
Created :   May 8, 2012
Conversion to QT started : April 20, 2013
Updated :   October 11, 2019
******************************************************************************/
#ifndef FIM_FICOBJ_H_INCLUDED
  #include "fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED
#ifndef FIM_CONSTANTS_H
  #include "fim_constants.h"
#endif // FIM_CONSTANTS_H
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
#ifndef STRINGPARSER_H_INCLUDED
  #include "../../../core/utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
#include <QDateTime>

/*****************************************************************************/

//=============================================================================
jfFIM_Fanfic::jfFIM_Fanfic():jfGenericFanfic3() {
  xparser = NULL;
}
//----------------------------------------------
jfFIM_Fanfic::jfFIM_Fanfic(const jfFIM_Fanfic& src):jfGenericFanfic3(src) {
  characters = src.characters;
  thumbsup = src.thumbsup;
  thumbsdown = src.thumbsdown;
  rating = src.rating;
  pubdate = src.pubdate;
  compact_summary = src.compact_summary;
  warnings = src.warnings;
  content_types = src.content_types;

  xparser = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfFIM_Fanfic::GetTypeID() const {
  return "FIM_Fanfic";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
bool jfFIM_Fanfic::SetFromString(const QString& inval, QString& parse_err, bool checkavatar) {
  // constants
  const QString fname = "jfFIM_Fanfic::SetFromString";
  const QString avatartag = "<img src=\"//www.fimfiction-static.net/images/avatars/";
  const QString startdesc = "<span class=\"description-text bbcode\">";
  const QString enddesc = "</section>";
  const QString datestart = "<span class=\"date\"><b>&#xb7;</b>";
  // variables
  unsigned long qval;
  QString tfrag, oerr;
  QString buffer,buffer2;
  size_t findwhich;
  bool haspubdate;
  QString ratstring;
  QDate latedate,currdate;
  int lpos;
  // starting
  validdata = false;
  xparser = new jfStringParser(inval);
  // avatar author id checking...
  if (checkavatar) {
    if (!xparser->GetDelimitedULong(avatartag,"_64.jpg\"",qval,buffer2)) author_id = 0;
    else author_id = qval;
  }
  // rating
  if (!xparser->MovePast(" data-controller=\"rate-story\">")) {
      return ParseError(parse_err,"Cannot find likes! A");
  }
  // looking for likes
  if (xparser->GetDelimited("<span class=\"likes\">","</span>",buffer)) {
      if (buffer=="") thumbsup = 0;
      else if (Str2ULongC(buffer,qval)) thumbsup = qval;
      else return ParseError(parse_err,"Likes value is not a number!");

      if (!xparser->GetDelimited("<span class=\"dislikes\">","</span>",buffer)) {
        return ParseError(parse_err,"Cannot find dislikes!");
      }
      if (buffer=="") thumbsdown = 0;
      else if (Str2ULongC(buffer,qval)) thumbsdown = qval;
      else return ParseError(parse_err,"Dislikes value is not a number!");
  }
  // failing that, we look for 'Ratings Disabled'
  else if (xparser->MovePastLimit("Ratings Disabled","<div class=\"divider\">")) {
      thumbsup = -1;
      thumbsdown = 0;
  }
  else return ParseError(parse_err,"Cannot find likes! B");

  // getting the fic id
  if (!xparser->GetDelimitedULong("id=\"story_","\"",qval,oerr)) {
    return ParseError(parse_err,oerr +=" While getting story id.");
  }
  num_id = qval;
  // next, the rating
  if (!xparser->MovePast("<a href=\"/stories?content_rating=")) {
    return ParseError(parse_err,"Could not find rating!");
  }
  if (!xparser->GetDelimited(">","</a>",ratstring)) return ParseError(parse_err,"Could not extract rating!");
  rating = fimcon::RatLookup((ratstring.trimmed())[0]);
  // building the primary link
  tfrag = "<a class=\"story_name\" href=\"/story/";
  if (!xparser->MovePast(tfrag)) return ParseError(parse_err,"Cannot find primary link info!");
  if (!xparser->GetDelimited("/","\"",buffer)) {
    return ParseError(parse_err,"Cannot find end of primary link!");
  }
  primarylink = "https://www.fimfiction.net/story/" + QString::number(num_id) + "/" + buffer;
  // the title
  if (!xparser->GetDelimited(">","</a>",buffer)) return ParseError(parse_err,"Cannot find title!");
  name = htmlparse::HTML2Text(buffer);
  // the author name
  // if checkavatar is false, the name is not here (the page is for the individual story)
  if (checkavatar) {
      if (!xparser->MovePast("<a href=\"/user/")) {
          return ParseError(parse_err,"Cannot find start of author id name!");
      }
      // getting the author ID and link-name
      if (!xparser->GetMovePastULong("/",qval,buffer2)) {
          return ParseError(parse_err,"Could not get Author ID : " + buffer2);
      }
      if (author_id = 0) author_id = qval;
      if (!xparser->GetMovePast("\">",buffer)) {
          return ParseError(parse_err,"Could find end of Author link!");
      }
      // setting the author link
      author_url = "https://www.fimfiction.net/user/" + QString::number(author_id);
      author_url += "/" + buffer;
      // getting the author name
      if (!xparser->GetMovePast("</a>",buffer)) {
          return ParseError(parse_err,"Cannot find author name!");
      }
      author_name = buffer;
  }
  // moving to the 'categories' (aka genres)
  if (!xparser->MovePast("<ul class=\"story-tags\">")) return ParseError(parse_err,"Story-Tags box not found!");
  // Tag checking loop
  while (xparser->MovePast("class=\"tag-")) {
      // getting the tag
      if (!xparser->GetMovePast("\"",buffer)) return ParseError(parse_err,"Could not get tag type!");
      if (!xparser->GetDelimited(">","</a>",buffer2)) return ParseError(parse_err,"missing genre link");
      // converting it to the internal format...
      if (buffer=="character") {
          if (!characters.isEmpty()) characters += ", ";
          characters += buffer2.trimmed();
      }
      else if (buffer=="content") {
          if (!content_types.isEmpty()) content_types += ", ";
          content_types += buffer2.trimmed();
      }
      else if (buffer=="warning") {
          if (!warnings.isEmpty()) warnings += ", ";
          warnings += buffer2.trimmed();
      }
      // series will be treated as a genre for now
      else {
        if (!genres.isEmpty()) genres += ", ";
        genres += buffer2.trimmed();
      }

  }
  // getting the description
  if (!xparser->GetDelimited(startdesc,enddesc,buffer)) {
    return ParseError(parse_err,"Description missing!");
  }
  lpos = buffer.lastIndexOf("</span>",-1,Qt::CaseInsensitive);
  if (lpos != -1) buffer.truncate(lpos+7);
  description = buffer.trimmed();
  /* Chapter links. Due to a lack of updated dates in fimfiction 4, we collect the dates for
   * each chapter to determine the updated date. */
  findwhich = 0;
  latedate = QDate(1900,1,1);
  while (xparser->MovePast("<li data-published class=\"")) {
    findwhich++;
    if (!xparser->GetDelimitedEndPair(datestart,"<span ","</span>",buffer)) {
        return ParseError(parse_err,"Unable to extract updated date for chapter!");
    }
    buffer = buffer.trimmed();
    if (!ParseFIMDate(buffer,currdate)) {
        return ParseError(parse_err,"Unable to parse chapter date!");
    }
    if (currdate > latedate) latedate = currdate;
  }
  if (findwhich==0) {
      included = false; // this actually happens, so not really a parsing error
      return ParseError(parse_err,"No chapters found!");
  }
  else part_count = findwhich;
  /**/JDEBUGLOG(fname,1)
  // status
  if (!xparser->MovePast("<span class=\"completed-status")) {
    return ParseError(parse_err,"No status span found!");
  }
  if (!xparser->GetDelimited(">","</span>",buffer)) return ParseError(parse_err,"Status not found!");
  buffer = buffer.trimmed();
  completed = (buffer=="Complete");
  /**/JDEBUGLOG(fname,2)
  // published date
  tfrag = "<span class=\"desktop\"><b>Published:</b> </span><";
  // this is more complicated than it used to be
  if (!xparser->MovePast(tfrag)) {
      /**/JDEBUGLOG(fname,3)
      /* since I've come cross a case of a story with no pubdate, I have to deal with
      it, instead of assuming a parsing error */
      haspubdate = false;
  }
  else {
      /**/JDEBUGLOG(fname,4)
      if (!xparser->GetDelimitedULong("data-time=\"","\"",qval,buffer)) {
          /**/JDEBUGLOG(fname,5)
        haspubdate = false;
      }
      else haspubdate = true;
  }
  /**/JDEBUGLOG(fname,6)
  if (haspubdate) {
    // the extracted date is in Unix TimeStamp format
    QDateTime temppubdate;
    temppubdate.setTime_t(qval);
    if (!temppubdate.isValid()) return ParseError(parse_err,"Could not parse published date!");
    pubdate = temppubdate.date();
    /**/JDEBUGLOG(fname,7)
    // annoyingly, with fimfiction 4 there is no modified/updated date anymore!
    // so I use a date built while scanning the list of chapters
    if (latedate > pubdate) updated_date = latedate;
    else updated_date = pubdate;
  }
  else updated_date = latedate;
  // word count
  if (!xparser->MovePast("<div class=\"word_count\">")) return ParseError(parse_err,"Missing wordcount");
  if (!xparser->GetDelimitedULong("<b>","</b>",qval,oerr)) {
    return ParseError(parse_err,"Could not parse wordcount! : " + oerr);
  }
  word_count = qval;

  // finishing touches
  validdata = true;
  ustatus = jud_NONE;
  delete xparser;
  xparser = NULL;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFicExtract_FIM* jfFIM_Fanfic::ExtractFromString(QString inval, QString& parse_err) {
  // constants
  const QString fname = "jfFIM_Fanfic::ExtractFromString";
  const QString dstr1 = "<a href=\"/user/";
  const QString partstart = "<article class=\"story_container\"";
  const QString partend = "</article>";
  // variables
  jfFicExtract_FIM* result;
  QString buffer, errval;
  QString rec_authname, rec_authlink;
  size_t rec_authid;
  unsigned long qval;
  // starting
  /**/JDEBUGLOG(fname,1);
  xparser = new jfStringParser(inval);
  if (!xparser->MovePast("<div class=\"user-page-header\"")) {
    return ExtractError(parse_err,"Missing User Page Header");
  }
  // getting author id and name
  if (!xparser->GetDelimitedULong(dstr1,"/",qval,errval)) {
    return ExtractError(parse_err,errval);
  }
  rec_authid = qval;
  if (!xparser->GetMovePast("\"",buffer)) {
      return ExtractError(parse_err,"Error while getting Author Link!");
  }
  if (!xparser->GetDelimited(">","</a>",rec_authname)) {
     return ExtractError(parse_err,"Error while getting Author Name!");
  }
  rec_authname = rec_authname.trimmed();
  // building the author link
  rec_authlink = "https://www.fimfiction.net/user/" + QString::number(rec_authid) + "/";
  rec_authlink += buffer.trimmed();

  // skipping past more crud and unecessary stuff
  /**/JDEBUGLOG(fname,3);
  /* Unfortunatly, this does not always work...
  if (!xparser->MovePast("<hr class=\"sidebar-hr\" />")) {
    return ExtractError(parse_err,"Missing Header Tabs");
  }
  */
  /**/JDEBUGLOG(fname,4);
  // extracting the main contents
  if (!xparser->GetDelimited(partstart,partend,buffer)) {
    buffer = xparser->GetBlock(2000);
    return ExtractError(parse_err,"could not get main contents\n"+buffer);
  }
  delete xparser;
  xparser = NULL;
  // calling set from string
  if (!SetFromString(buffer,errval,false)) {
    return ExtractError(parse_err,"Parsing details failed:\n"+errval);
  }
  // additional parsing to get part names
  QStringList extracted_names;
  QString namebuf;
  /**/JDEBUGLOG(fname,7);
  xparser = new jfStringParser(buffer);
  if (xparser->MovePast("<!--Chapters-->")) {
    xparser->ChopAfter("<div class=\"chapters-footer\">",true);
    while(xparser->MovePast("<a class=\"chapter-title\" href=\"/story/")) {
      if (!xparser->GetDelimited("\">","</a>",namebuf)) {
        return ExtractError(parse_err,"Problems in extracting part names");
      }
      extracted_names.append(htmlparse::ConvertEntities(namebuf.trimmed(),false));
    }
  }
  /**/JDEBUGLOG(fname,11);
  if (part_count != extracted_names.size()) {
    return ExtractError(parse_err,"Number of part names does not match part count!");
  }
  // finishing
  result = new jfFicExtract_FIM();
  author_id = rec_authid;
  author_name = rec_authname;
  author_url = rec_authlink;
  /**/JDEBUGLOG(fname,12);
  LoadIntoExtract(result);
  result->auth_id = rec_authid;
  result->partnames = extracted_names;
  delete xparser;
  xparser = NULL;
  /**/JDEBUGLOG(fname,14);
  // done
  return result;
}
//-----------------------------------------------------------
void jfFIM_Fanfic::SetCompactSummary(const QString& insum) {
  compact_summary = insum;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
QString jfFIM_Fanfic::GetCharacters() const {
  return characters;
}
//-----------------------------------------------------------------------
QString jfFIM_Fanfic::GetContentTypes() const {
    return content_types;
}
//-----------------------------------------------------------------------
QString jfFIM_Fanfic::GetWarnings() const {
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
QString jfFIM_Fanfic::GetCompactSummary() const {
  return compact_summary;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special url construction
//----------------------------------------------
QString jfFIM_Fanfic::MakeAuthorUrl() const {
  QString result = "https://www.fimfiction.net/user/";
  result += author_name;
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// to string functions
//----------------------------------------------
QString jfFIM_Fanfic::ToText() const {
  QString result,buffer;
  // building the title and author line
  result += name + " by " + author_name + "\n";
  // next line: link display
  result += primarylink + "\n";
  // adding the main description
  result += GetDescExtract(6,750) + "\n";;
  // next up.. two extra lines
  result += "Published: " + pubdate.toString("M-d-yyyy");
  result += " - Updated: " + updated_date.toString("M-d-yyyy");
  // the rating...
  result += " - Rating: " + rating;
  // part count and word count
  result += " - Parts: " + QString::number(part_count);
  result += " - Words: " + QString::number(word_count) + "\n";
  // first of 2 possible lines
  bool hasGenre = !genres.isEmpty();
  bool hasCT = !content_types.isEmpty();
  bool hasWarn = !warnings.isEmpty();
  if (hasGenre) {
    result += "Genres: " + genres;
  }
  if (hasCT) {
      if (hasGenre) result += " - ";
      result += "Types: " + content_types;
  }
  if (hasWarn) {
      if (hasGenre || hasCT) result += " - ";
      result += "Warnings: " + warnings;
  }
  // second of 2 possible lines
  bool hasChar = !characters.isEmpty();
  if (hasChar || completed) result += "\n";
  // character data
  if (hasChar) {
    result += "Characters Specified: " + characters;
  }
  if (completed) {
      if (hasChar) result += " - ";
      result += "Complete";
  }
  // finishing off
  result += "\n";
  // done
  return result;
}
//----------------------------------------------
QString jfFIM_Fanfic::ToDisplayHTML() const {
  const QString fname = "jfFFNItem::ToDisplayHTML";
  QString result,buffer;
  // we start with the table
  result = "<table width=99%><tr><td>";
  // building the title line
  result += "<font size=+2><a href=\"";
  result += primarylink + "\">";
  result += name + "</a>";
  // adding the author stuff
  result += " by " + author_name;
  result += "</font> ";
  // status
  if (ustatus!=jud_NONE) {
    result += "<font size=+1 color=";
    if (ustatus==jud_UPDATED) result += "lime><b>[Updated";
    else if (ustatus==jud_MISSING) result += "red><b>[Missing";
    else if (ustatus==jud_NEW) result += "aqua><b>[New";
    else if (ustatus==jud_AUTHORNC) result += "yellow><b>[Author Name Change";
    result += "!]</b></font>";
  }
  result += "</td></tr>\n";
  // next line: link display
  result += "<tr><td>";
  result += "<font color=green>";
  result += primarylink + "</font><br>\n";
  // adding the main description
  result +=  "<font color=#010101>" + GetDescExtract(6,750) + "<br>\n";
  // next up.. two extra lines
  result += "<font color=gray>Published: " + pubdate.toString("MM-dd-yyyy");
  result += " - Updated: " + updated_date.toString("MM-dd-yyyy");
  result += " - Rating: " + rating;
  // part count and word count
  result += " - Parts: " + QString::number(part_count);
  result += " - Words: " + QString::number(word_count);
  // thumbs up and down
  if (thumbsup < 0) result += " - Rating Disabled";
  else {
    result += " - Thumbs Up: " + QString::number(thumbsup);
    result += " - Thumbs Down: " + QString::number(thumbsdown);
  }
  result += "</font><br>\n";
  // genres and content type...
  bool hasGenre = !genres.isEmpty();
  bool hasCT = !content_types.isEmpty();
  bool hasWarn = !warnings.isEmpty();
  // final info,
  if (hasGenre) {
      result += "<font color=green><b>Genres:</b> ";
      result += genres + "</font>";
  }
  if (hasCT) {
      if (hasGenre) result += " &middot; ";
      result += "<font color=gray><b>Types:</b> ";
      result += content_types + "</font>";
  }
  // warnings
  if (hasWarn) {
      if (hasGenre || hasCT) result += " &middot; ";
      result += "<font color=red><b>Warnings:</b> ";
      result += warnings + "</font>";
  }
  if (hasGenre || hasCT || hasWarn) result += "<br>\n";
  // characters and completion status
  bool hasChar = !characters.isEmpty();
  if (hasChar || completed) {
      result += "<font color=blue>";
      if (hasChar) {
          result += "<b>Characters:</b> " + characters;
      }
      if (completed) {
          if (hasChar) result += " &middot; ";
          result += "<b>Complete</b>";
      }
      result += "</font>";
  }
  // finishing off
  result += "</font></td></tr>\n</table>";
  // done
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// added stuff
bool jfFIM_Fanfic::LoadValues(jfSkeletonParser* inparser) const {
  QString rbuffer;
  LoadCoreValues(inparser);
  LoadMoreValues1(inparser);
  LoadMoreValues2(inparser);
  inparser->AddText("ITEMF_GENRES",genres);
  inparser->AddText("ITEMF_CHARDATA",characters);
  inparser->AddText("ITEMF_CONTENTTYPES",content_types);
  inparser->AddText("ITEMF_WARNINGS",warnings);
  inparser->AddUInt("ITEMF_AUTHORID",author_id);
  inparser->AddText("ITEMF_RATING",rating);
  inparser->AddBool("THUMBS_DISABLED",(thumbsup < 0));
  inparser->AddUInt("THUMBSUP",(thumbsup< 0)?0:thumbsup);
  inparser->AddUInt("THUMBSDOWN",thumbsdown);
  inparser->AddUInt("THUMBSDOWN",thumbsdown);
  inparser->AddText("ITEMF_PUBDATE",pubdate.toString("MM-dd-yyyy"));
  return true;
}
//----------------------------------------------------------------------------
void jfFIM_Fanfic::ProcessDescription() {
  jfStringParser* vparse;
  QString bufx, nrerep;
  bool imgrep, qres;
  // skipping the images
  imgrep = false;
  vparse = new jfStringParser(description);
  while(vparse->GetMovePast("<img",bufx)) {
    imgrep = true;
    nrerep += bufx;
    qres = vparse->MovePast(">");
    if (!qres) break;
  }
  if (imgrep) {
    if (qres) {
      vparse->GetToEnd(bufx);
      nrerep += bufx;
    }
    delete vparse;
    description = nrerep;
  }
  // post processing
  description.replace("</p><br /><p><p class=\"double\"><hr>","<br><br>\n",Qt::CaseInsensitive);
  description.replace("</p><p class=\"double\"><hr></p>","<br><br>\n",Qt::CaseInsensitive);
  description.replace("</p><p><hr></p>","<br><br>\n",Qt::CaseInsensitive);
  description.replace("<hr />","<br><br>\n",Qt::CaseInsensitive);
  description.replace("</p><p class=\"double\">","<br><br>\n",Qt::CaseInsensitive);
  description.replace("</p>","<br>\n",Qt::CaseInsensitive);
  description.replace("<br><br>\n<br>\n","<br><br>\n",Qt::CaseInsensitive);
  description.replace("<p>"," ",Qt::CaseInsensitive);
  description = description.simplified();
  description += "</center>";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
//----------------------------------------------
// fimfiction.net now has dates... which cannot be converted by standard functions
bool jfFIM_Fanfic::ParseFIMDate(QString insrc, QDate& out) {
    const QString fname = "jfFIM_Fanfic::ParseFIMDate";
  // variables
  QString split1,split2;
  int spos;
  // splitting the string
  insrc = insrc.trimmed();
  spos = insrc.indexOf(' ');
  if (spos<0) return false;
  split1 = insrc.left(spos);
  split2 = insrc.mid(spos+1).trimmed();
  // checking and truncating the first part (to get rid of the 'st', 'nd', 'rd', 'th' )
  if (split1.length()<3) return false;
  split1.chop(2);
  // rebuilding, after which we can use QDate::ParseFormat
  insrc = split1 + " " + split2;
  out = QDate::fromString(insrc,"d MMM yyyy");
  // returning the success
  return (out.isValid());
}
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
//----------------------------------------------
bool jfFIM_Fanfic::ParseError(QString& perror,const QString& message) {
  if (xparser != NULL) delete xparser;
  xparser = NULL;
  perror = "PARSE ERROR: " + message;
  return false;
}
//----------------------------------------------
jfFicExtract_FIM* jfFIM_Fanfic::ExtractError(QString& perror,const QString& message) {
  if (xparser != NULL) delete xparser;
  xparser = NULL;
  perror = "PARSE ERROR: " + message;
  return NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//--------------------------------
bool jfFIM_Fanfic::AddExtraStuff(QTextStream* outfile) const {
  // local variables
  QString result;
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // preparing next line, characters
  xresult << characters;
  (*outfile) << xresult << "\n";
  xresult.clear();
  // content type and warnings
  xresult << content_types << warnings;
  (*outfile) << xresult << "\n";
  xresult.clear();
  // line after that is more complicated
  xresult << rating << thumbsup << thumbsdown << pubdate;
  (*outfile) << xresult << "\n";
  xresult.clear();
  // compact summary line
  xresult << compact_summary;
  (*outfile) << xresult << "\n";
  // done
  return true;
}
//--------------------------------
bool jfFIM_Fanfic::ReadExtraStuff(jfFileReader* infile) {
  const QString funcname = "jfFIM_Fanfic::ReadExtraStuff";
  // the line with characters
  assert(infile!=NULL);
  if (!infile->ReadUnEsc(characters,funcname)) return false;
  // content type and warnings
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
