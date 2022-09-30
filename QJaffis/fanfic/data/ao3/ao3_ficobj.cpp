/******************************************************************************
Name    :   ao3_ficobj.cpp
Author  :   John Q Metro
Purpose :   Defines fanfic object data of archoveofourown.org
Created :   August 26, 2012
Conversion to Qt Started September 28, 2013
Updated :   August 31, 2022
******************************************************************************/
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
//---------------------------------------
#ifndef FICBASE_H
  #include "../ficbase.h"
#endif // FICBASE_H
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef AO3_CONSTS_H_INCLUDED
  #include "ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED

#ifndef DISPLAYHTMLSPEC_H
    #include "../displayhtmlspec.h"
#endif // DISPLAYHTMLSPEC_
//-------------------------------------
#include <assert.h>
#include <QRegExp>
#include <QRegularExpression>
//**************************************************************************
const QRegularExpression breaks = QRegularExpression("<br/?>[\\s\\-*]*<br/?>([\\s\\-*]*<br/?>)*",QRegularExpression::CaseInsensitiveOption);

// default constructors
//----------------------------
jfAO3Fanfic::jfAO3Fanfic():jfGenericFanfic2() {
  rating = warn = ao3con::unchar;
  eccount = -1;
  kudcount = 0;
  series_index = 0;
  x_parser = NULL;
  english_locale = QLocale(QLocale::English, QLocale::UnitedStates);
}
//----------------------------
jfAO3Fanfic::jfAO3Fanfic(const jfAO3Fanfic& src):jfGenericFanfic2(src) {
  rating = src.rating;
  orientations = src.orientations;
  warn = src.warn;
  eccount = src.eccount;
  kudcount = src.kudcount;
  warntags = src.warntags;
  relationships = src.relationships;
  characters = src.characters;
  extratags = src.extratags;
  // categories and series
  series_index = src.series_index;
  series_name = src.series_name;
  series_url = src.series_url;
  cats.assign(src.cats.begin(),src.cats.end());
  x_parser = NULL;
  english_locale = QLocale(QLocale::English, QLocale::UnitedStates);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
bool jfAO3Fanfic::SetFromString(QString inval, const jfAO3_Category* incat, QString& parse_err) {
  const QString fxname = "jfAO3Fanfic::SetFromString";
  // preparing
  bool success = false;
  validdata = false;
  // category stuff
  assert(incat!=NULL);
  cats.clear();
  cats.push_back(incat);
  // setting up the parser
  x_parser = new jfStringParser(inval);
  // calling
  if (ParseStart()) {
    if (ParseMiddle()) {
      if (ParseTags()) {
        if (ParseEnd()) success = true;
      }
    }
  }
  if (!success) {
    parse_err = parser_error;
    JDEBUGLOGS(fxname,1, parse_err)
  }
  else delete x_parser;
  validdata = success;
  return success;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfAO3Fanfic::GetTypeID() const {
  return "AO3Fanfic";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting info
//----------------------------
QChar jfAO3Fanfic::GetRating() const { return rating; }
//----------------------------
QString jfAO3Fanfic::GetOrientations() const { return orientations; }
//----------------------------
QChar jfAO3Fanfic::GetWarnflag() const { return warn; }
//----------------------------
QString jfAO3Fanfic::GetWarntag() const { return warntags;}
//----------------------------
bool jfAO3Fanfic::TestWarntag(QChar inval) const {
  return warntags.contains(inval);
}
//----------------------------
int jfAO3Fanfic::GetEstPCount() const { return eccount; }
//----------------------------
size_t jfAO3Fanfic::GetKudos() const { return kudcount; }
//----------------------------
QString jfAO3Fanfic::GetRelationships() const { return relationships; }
//----------------------------
QString jfAO3Fanfic::GetCharacters() const { return characters; }
//----------------------------
const QStringList& jfAO3Fanfic::GetExtraTags() const { return extratags; }
//----------------------------
QString jfAO3Fanfic::GetJoinedExtraTags() const {
    return extratags.join(',');
}
//----------------------------
bool jfAO3Fanfic::InSeries() const { return (series_index!=0); }
//----------------------------
size_t jfAO3Fanfic::SeriesIndex() const { return series_index; }
//----------------------------
QString jfAO3Fanfic::SeriesName() const { return series_name; }
//----------------------------
QString jfAO3Fanfic::SeriesUrl() const { return series_url; }
//---------------------------------
QString jfAO3Fanfic::GetFandoms() const {
  QString resval;
  for (size_t xl = 0;xl<cats.size();xl++) {
    if (!resval.isEmpty()) resval += ", ";
    resval += cats[xl]->GetName();
  }
  return resval;
}
//---------------------------------
bool jfAO3Fanfic::MultipleFandoms() const {
  return (cats.size()>1);
}
//---------------------------------
QString jfAO3Fanfic::GetFandom(size_t inindex) const {
  if (inindex>=cats.size()) return "";
  else return cats[inindex]->GetName();
}
//---------------------------------
size_t jfAO3Fanfic::FandomCount() const {
  return (cats.size());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special url construction
QString jfAO3Fanfic::MakeAuthorUrl() const { return  author_url; }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// to string functions
//----------------------------
QString jfAO3Fanfic::ToText() const {
  QString result,buffer;
  // building the title and author line
  result += name + " by " + author_name + "\n";
  // next line: link display
  result += primarylink + "\n";
  // adding the main description
  result += description + "\n";
  if (InSeries()) {
    result += "Part " + QString::number(series_index) + " of the " + series_name;
    result += " Series.\n";
  }
  // next up.. two extra lines
  result += "Updated: " + updated_date.toString("MM-dd-yyyy");

  // the rating... and orientation
  result += " - Rating: " + RatingToString();
  result += " - Orientation";
  result += (orientations.contains(","))?"s: ":": ";
  result += orientations;

  // part count and word count
  result += " - Parts: " + QString::number(part_count);
  result += " - Words: " + QString::number(word_count);
  if (kudcount!=0) result += " - Kudos: " + QString::number(kudcount);
  result+= "\n";
  // final info,
  result += "Warnings: " + WarnToString();
  if (!characters.isEmpty()) {
    result += " - Characters: " + characters + "\n";
  }
  // character data
  if (!relationships.isEmpty()) {
    result += "Pairings: " + relationships;
  }
  // tags
  if (!extratags.isEmpty()) {
    result += " - Tags: " + GetJoinedExtraTags();
  }
  if (completed) {
    result += " - Complete";
  }
  // finishing off
  result += "\n";
  // done
  return result;
}
//----------------------------
QString jfAO3Fanfic::ToDisplayHTML() const {
  const QString fname = "jfAO3Fanfic::ToDisplayHTML";
  QString result,buffer,optline;

  const jfDisplayHTMLHelper* helper = HTMLHelpers::ao3_item_helper;
  // we start with the table
  result = DisplayHTMLHeader(1, helper);
  // fandom display
  result += "<br>\n" + helper->WrapText("fandoms", GetFandoms());
  result += "</td></tr>\n";
  // next line: link display
  result += "<tr><td>";
  result += helper->WrapText("ficlink", primarylink, false);

  // adding the main description
  result += "<div style=\"margin-left:10px;\">";
  QString rdesc = (description.length() > 1000) ? (description.left(1000) + "...") : description;
  result += helper->WrapTextNoEsc("description",rdesc,false);
  result += "</div>";

  // series
  if (InSeries()) {
      QString series = "Part " + QString::number(series_index) + " of the " + series_name + " Series";
      result += helper->WrapText("series", series, true);
  }
  // standard metadata
  QString mdata = "Updated: " + updated_date.toString("MM-dd-yyyy") + " - Rating: " + RatingToString();
  mdata += " - Orientation";
  mdata += ((orientations.contains(","))?"s: ":": ") + orientations;
  // part count and word count
  mdata += " - Parts: " + QString::number(part_count);
  mdata += " - Words: " + QString::number(word_count);
  // kudos
  mdata += " - Kudos: " + QString::number(kudcount);
  if (completed) {
    mdata += " - Complete";
  }
  result += helper->WrapText("basicinfo", mdata, true);

  // final info
  optline = helper->ConditionalWrapText("warnings",false,"Warnings: ", true, WarnToString());
  optline += helper->ConditionalWrapText("characters", !optline.isEmpty(), "Characters: ", true, characters);
  optline += helper->ConditionalWrapText("pairings", !optline.isEmpty(), "Pairings: ", true, relationships);
  if (!optline.isEmpty()) result += optline + "<br>\n";

  // extra tags
  result += helper->ConditionalWrapText("tags", false, "", false, GetJoinedExtraTags());

  // finishing off
  result += "</td></tr>\n</table>";
  // done
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// added stuff
bool jfAO3Fanfic::LoadValues(jfSkeletonParser* inparser) const {
  // checking
  assert(inparser!=NULL);
  QString rtval;
  // starting
  LoadCoreValues(inparser);
  LoadMoreValues1(inparser);
  LoadMoreValues2(inparser);
  // rating
  inparser->AddText("ITEMF_RATING",RatingToString());
  // orientation
  inparser->AddText("ITEMF_ORIENT",GetOrientations());
  // warnings
  inparser->AddText("ITEMF_WARNING",WarnToString());
  // kudos
  inparser->AddUInt("ITEMF_KUDOS",kudcount);
  // the strings
  inparser->AddText("ITEMF_CHARACTERS",characters);
  inparser->AddText("ITEMF_RELATIONSHIPS",relationships);
  inparser->AddText("ITEMF_EXTRATAGS",GetJoinedExtraTags());
  inparser->AddText("ITEMF_FANDOMS",GetFandoms());
  // series stuff
  inparser->AddBool("ITEMF_INSERIES",InSeries());
  inparser->AddUInt("ITEMF_SERIESINDEX",series_index);
  inparser->AddText("ITEMF_SERIESNAME",series_name);
  inparser->AddText("ITEMF_SERIESURL",series_url);
  // done
  return true;
}
//------------------------------------------------------------------
void jfAO3Fanfic::ProcessDescription() {
  if (!description.isEmpty()) {
    description = description.trimmed();
    description = description.mid(3,description.length()-7);
    // post processing
    description.replace("</p><p><hr></p>","<br>\n",Qt::CaseInsensitive);
    description.replace("<hr />","<br>\n",Qt::CaseInsensitive);
    description.replace("</p><p>","<br>\n",Qt::CaseInsensitive);
    description.replace("</p>","<br>\n",Qt::CaseInsensitive);
    description.replace("<p>"," ",Qt::CaseInsensitive);
    description.replace(breaks,"<br>\n");

    description = description.simplified();
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal to string helper methods
//----------------------------
QString jfAO3Fanfic::RatingToString() const {
  if (rating=='G') return "General";
  else if (rating=='T') return "Teen";
  else if (rating=='M') return "Mature";
  else if (rating=='E') return "Explicit";
  else if (rating=='_') return "Unspecified";
  else assert(false);
  return "";
}
//----------------------------
QString jfAO3Fanfic::WarnToString() const {
  // variables
  QString resval;
  // testing
  if (warn=='N') return "Creator Chose Not To Use Archive Warnings";
  else if (warn=='_') return "No Archive Warnings Apply";
  else if (warn=='E') return "External Site";
  else if (warn=='W') {
    if (TestWarntag('V')) resval = "Violence";
    if (TestWarntag('D')) {
      if (!resval.isEmpty()) resval+= ", ";
      resval += "Character Death";
    }
    if (TestWarntag('R')) {
      if (!resval.isEmpty()) resval+= ", ";
      resval += "Rape";
    }
    if (TestWarntag('P')) {
      if (!resval.isEmpty()) resval+= ", ";
      resval += "Underage Sex";
    }
    // done
    return resval;
  }
  else assert(false);
  return "";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal parsing methods
//-------------------------------------------
// handles fic name, id, author name and id, and category info
bool jfAO3Fanfic::ParseStart() {
  // constants
  // variables
  unsigned long tval;
  QString errpass,buffer;
  jfStringParser* tparser;
  QString curl;
  const jfGeneralCategory* temp;
  size_t aloc, bloc;
  // starting...
  assert(x_parser!=NULL);
  // the fic id first
  if (!x_parser->GetDelimitedULong("<a href=\"/works/","\">",tval,errpass)) {
    return parseError("Error in getting fic id : " + errpass);
  }
  num_id = tval;
  primarylink = "https://archiveofourown.org/works/" + QString::number(num_id);
  primarylink += "?view_adult=true";
  // title
  if (!x_parser->GetMovePast("</a>",name)) return parseError("Could not extract title!");
  name = htmlparse::ConvertEntities(name, false);
  // can be rather complicated because we can have anonymous stories with no link
  if (!x_parser->Find1stPair("<a rel=\"author\"","Anonymous",aloc,bloc)) {
    return parseError("Could not get author!");
  }
  if (aloc==1) {
    // author link
    if (!x_parser->MovePast("<a rel=\"author\"")) {
      return parseError("Error in getting start of author link!");
    }
    if (!x_parser->GetDelimited("href=\"","\"",buffer)) {
      return parseError("Error in getting author link!");
    }
    author_url = "https://archiveofourown.org" + buffer;
    // author name
    if (!x_parser->GetDelimited(">","</a>",author_name)) {
      return parseError("Error in getting author name!");
    }
    author_name = htmlparse::ConvertEntities(author_name, false);
  }
  else {
    author_name = "Anonymous";
  }
  // fandoms
  if (!x_parser->GetDelimited("<h5 class=\"fandoms","</h5>",buffer)) {
    return parseError("Cannot find fandoms!");
  }
  tparser = new jfStringParser(buffer);
  while (tparser->MovePast("<a class=\"tag\"")) {
    if (!tparser->GetDelimited("href=\"","\"",curl)) break;
    curl = "https://archiveofourown.org" + curl;
    if (curl==cats[0]->GetUrl()) continue;
    temp = ao3_catdata::ao3_catmanager->GetData()->FindByUrl(curl);
    /* note that I've decided to ignore unfound fandoms after my first first
    test revealed multiple names/urls for the same fandon. Too difficult to track
    all of them. */
    if (temp!=NULL) cats.push_back(dynamic_cast<const jfAO3_Category*>(temp));
  }
  delete tparser;
  if (cats.size()==0) return parseError("No fandoms!");
  else return true;
}
//-------------------------------------------
// special flags and date
bool jfAO3Fanfic::ParseMiddle() {
  // constants

  // variables
  QString buffer;
  QString whatwarn = "";
  // starting...
  assert(x_parser!=NULL);
  // rating
  if (!x_parser->GetDelimited("<span class=\"rating-","\"",buffer)) {
    return parseError("Cannot find rating!");
  }
  if (buffer=="teen rating") rating = 'T';
  else if (buffer=="explicit rating") rating = 'E';
  else if (buffer=="general-audience rating") rating = 'G';
  else if (buffer=="mature rating") rating = 'M';
  else if (buffer=="notrated rating") rating = '_';
  else return parseError("Rating unrecognized!");

  // warning flag
  if (!x_parser->GetDelimited("<span class=\"warning-","\"",buffer)) {
    return parseError("Cannot find warning flag!");
  }
  if (buffer=="yes warnings") warn = 'W';
  else if (buffer=="no warnings") warn = '_';
  else if (buffer=="choosenotto warnings") warn = 'N';
  else if (buffer=="mature rating") warn = 'E';
  else return parseError("Warning Flags unrecognized!");
  // we try to get the actual warning
  if (warn == 'W') {
      if (x_parser->GetDelimited("title=\"","\">",buffer)) {
          whatwarn = buffer.trimmed().replace("\\'","'");;
      }
  }

  // orientation
  if (!x_parser->MovePastTwo("<span class=\"category-","\"")) {
      return parseError("Cannot start of find orientation!");
  }
  if (!x_parser->GetDelimited("title=\"","\"",buffer)) {
      return parseError("Cannot find orientation!");
  }
  if (buffer == "No category") orientations = "";
  else {
      orientations = buffer.trimmed();
  }

  // completed flag
  if (!x_parser->GetDelimited("<span class=\"complete-","\"",buffer)) {
    return parseError("Cannot find completion status!");
  }
  completed = (buffer=="yes iswip");
  // getting the date
  if (!x_parser->GetDelimited("<p class=\"datetime\">","</p>",buffer)) {
    return parseError("Cannot find date!");
  }
  // converting the date
  buffer = buffer.trimmed();
  updated_date = english_locale.toDate(buffer,"d MMM yyyy");
  // updated_date = QDate::fromString(buffer,"d MMM yyyy");
  if (!updated_date.isValid()) return parseError("Date Conversion failed! " + buffer);
  return true;
}
//-------------------------------------------
bool jfAO3Fanfic::ParseTags() {
  // constants
  // variables
  QString buffer1,buffer2;
  QString extrawarn;
  // the tag loop
  while(x_parser->GetDelimited("<li class='","'>",buffer1)) {
    if (!x_parser->MovePast("<a class=\"tag\"")) {
        return parseError("Cannot find middle part of tag!");
    }
    if (!x_parser->GetDelimited("\">","</a>",buffer2)) {
      return parseError("Cannot find final part of tag!");
    }
    buffer2 = htmlparse::ConvertEntities(buffer2, false);
    // here, we've got the tag strings, just sort them!
    if (buffer1=="warnings") {
      if (buffer2=="Underage") warntags += 'P';
      else if (buffer2=="Graphic Depictions Of Violence") warntags += 'V';
      else if (buffer2=="Major Character Death") warntags += 'D';
      else if (buffer2=="Rape/Non-Con") warntags += 'R';
      else if (buffer2=="Creator Chose Not To Use Archive Warnings") ;
      else if (buffer2=="No Archive Warnings Apply") ;
      /* annoyingly, AO3 has just come up with a bunch of additional warnings which
       * seem to be not standardized or filterable. so, I'll stick them in extratags */
      else extrawarn = buffer2.trimmed();
    }
    else if (buffer1=="characters") {
      if (!characters.isEmpty()) characters += ", ";
      characters += buffer2;
    }
    else if (buffer1=="relationships") {
      if (!relationships.isEmpty()) relationships += ", ";
      relationships += buffer2;

    }
    else if (buffer1=="freeforms") {
        extratags.append(buffer2.trimmed());
    }
    else return parseError("Unrecognized Tag Type! (" + buffer1 + ")");
  }
  characters = characters.trimmed();
  relationships = relationships.trimmed();
  if (!extrawarn.isEmpty()) {
    if (!extratags.isEmpty()) extratags += ", ";
    extratags += extrawarn;
  }
  // done
  return true;
}
//-------------------------------------------
bool jfAO3Fanfic::ParseEnd() {
  // constants
  const QString sum_string = "<blockquote class=\"userstuff summary\">";
  // variables
  QString buffer1,buffer2;
  QString errout;
  unsigned long tval;
  // getting the summary
  if (!x_parser->GetDelimited(sum_string,"</blockquote>",buffer1)) {
    // sometimes there is no summary
    description = "";
  }
  else description = buffer1.trimmed();

  // series (optional)
  if (x_parser->MovePast("<h6 class=\"landmark heading\">Series</h6>")) {
    if (!x_parser->GetDelimitedULong("Part <strong>","</strong>",tval,errout)) {
      return parseError("Series index is wrong! " + errout);
    }
    series_index = tval;
    if (!x_parser->GetDelimited("<a href=\"","\">",buffer2)) {
      return parseError("Problems getting series url!");
    }
    series_url = "https://archiveofourown.org" + buffer2;
    if (!x_parser->GetMovePast("</a>",buffer1)) {
      return parseError("Problems getting series name!");
    }
    series_name = htmlparse::ConvertEntities(buffer1, false);
  }
  else series_index = 0;
  // words
  if (!x_parser->MovePast("<dt class=\"words\">Words:</dt>")) return parseError("Cannot find wordcount!");
  if (!x_parser->GetDelimitedULong("<dd class=\"words\">","</dd>",tval,errout)) {
    included = false;
    return parseError("Cannot extract wordcount! " + errout);
  }
  word_count = tval;
  // part count
  if (!x_parser->MovePast("<dt class=\"chapters\">Chapters:</dt>"))return parseError("Cannot find part count!");
  if (!x_parser->MovePast("<dd class=\"chapters\">")) {
      return parseError("Cannot find start of chapter counts!");
  }
  // try the 'not embedded in a link' first
  if (!x_parser->GetMovePastULong("/",tval,errout)) {
      // if that fails, assume we are embedded in a link
      if (!x_parser->GetDelimitedULong("\">","</a>",tval,errout)) {
          return parseError("Cannot extract part count! " + errout);
      }
      x_parser->MovePast("/");
  }
  part_count = tval;
  // estimated part count
  if (!x_parser->GetMovePast("</dd>",buffer1)) {
    return parseError("Cannot find estimated part count!");
  }
  // checking..
  if (buffer1=="?") eccount = -1;
  else if (Str2ULong(buffer1,tval,false)) eccount = tval;
  else return parseError("Estimated part count is not a number!");
  // kudos
  if (x_parser->MovePast("<dt class=\"kudos\">Kudos:</dt>")) {
    if (!x_parser->MovePast("<dd class=\"kudos\">")) return parseError("Missing Kudos!");
    if (!x_parser->GetDelimitedULong("\">","</a>",tval,errout)) {
      return parseError("Kudos is not a number!");
    }
    kudcount = tval;
  }
  else kudcount = 0;
  // done
  return true;
}
//----------------------------
bool jfAO3Fanfic::parseError(const QString in_msg) {
  if (x_parser != NULL) delete x_parser;
  x_parser =NULL;
  parser_error = "PARSE ERROR: "+ in_msg;
  return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
//----------------------------
jfFicExtract* jfAO3Fanfic::MakeExtract() const {
  jfFicExtract_AO3* result;
  result = new jfFicExtract_AO3();
  LoadIntoExtract(result);
  return result;
}
//----------------------------
void jfAO3Fanfic::LoadIntoExtract(jfFicExtract* into) const {
  // loading basic stuff
  LoadIntoExtractCore(into);
  into->complete = completed;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//----------------------------
bool jfAO3Fanfic::AddRestToFile(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  QStringList* cfinders;
  size_t chloop;
  // checking and special conditions
  if (outfile==NULL) return false;
  // preparing line 4
  xresult << author_name << author_url;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // doing line 5
  xresult << part_count << updated_date << word_count << completed;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // long line 6
  xresult << series_index << series_name << series_url;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // chapter finders (line 7)
  cfinders = new QStringList();
  for (chloop=0;chloop<cats.size();chloop++) {
    cfinders->append(cats[chloop]->GetCombo());
  }
  xresult << StrArrToList(cfinders,',');
  (*outfile) << xresult << "\n";
  // done
  return AddExtraStuff(outfile);
}
//----------------------------
bool jfAO3Fanfic::ReadRestFromFile(jfFileReader* infile) {
  // constants and variables
  const QString funcname = "jfAO3Fanfic::ReadRestFromFile";
  QString buffer;
  QStringList* clist;
  size_t cloop;
  const jfGeneralCategory* temp;
  // the line with characters
  assert(infile!=NULL);
  // reading line 4
  if (!infile->ReadParseLine(2,funcname)) return false;
  author_name = infile->lp.UnEscStr(0);
  author_url = infile->lp.UnEscStr(1);
  if (author_url.isEmpty()) infile->BuildError("The author link is empty!");
  // reading line 5
  if (!infile->ReadParseLine(4,funcname)) return false;
  if (!infile->lp.SIntVal(0,part_count)) return infile->BuildError("The part count is invalid!");
  if (!infile->lp.DateVal(1,updated_date)) return infile->BuildError("Unable to convert the Updated Date field properly!");
  if (!infile->lp.SIntVal(2,word_count)) return infile->BuildError("The word count is not valid!");
  if (!infile->lp.BoolVal(3,completed)) return infile->BuildError("The completed flag is not a boolean!");
  // reading line 6
  if (!infile->ReadParseLine(3,funcname)) return false;
  if (!infile->lp.SIntVal(0,series_index)) return infile->BuildError("The series index is wrong!");
  series_name = infile->lp.UnEscStr(1);
  series_url = infile->lp.UnEscStr(2);
  // getting chapters
  if (!infile->ReadUnEsc(buffer,funcname)) return infile->BuildError("Category list not found!");
  clist = ListToStrArr(buffer,',');
  // first error handling
  if (clist==NULL) return infile->BuildError("The categories are empty!");
  if (clist->isEmpty()) {
    delete clist;
    return infile->BuildError("The categories are empty!");
  }
  // chapter finders
  for (cloop=0;cloop<(clist->count());cloop++) {
    temp = ao3_catdata::ao3_catmanager->GetData()->FindCatFromFinder(clist->at(cloop));
    if (temp==NULL) break;
    cats.push_back(dynamic_cast<const jfAO3_Category*>(temp));
  }
  // cheking the results
  delete clist;
  if (temp==NULL) return infile->BuildError("Unknown category!");
  // finally
  return ReadExtraStuff(infile);
}
//----------------------------
bool jfAO3Fanfic::AddExtraStuff(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // preparing line 8
  xresult << rating << orientations << warn << warntags << eccount << kudcount;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // doing line 9
  xresult << characters;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // doing line 10
  xresult << relationships;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // doing line 11
  xresult << extratags;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // done
  return true;
}
//----------------------------
bool jfAO3Fanfic::ReadExtraStuff(jfFileReader* infile) {
    // constants and variables
  const QString funcname = "jfAO3Fanfic::ReadExtraStuff";
  QString buffer;
  QRegExp qcheck = QRegExp("[^" + ao3con::warn2_ac + "]");
  // the line with characters
  assert(infile!=NULL);
  // reading line 8
  if (!infile->ReadParseLine(6,funcname)) return false;
  // testing special chars
  if (!infile->lp.CharVal(0,rating,ao3con::rating_ac)) return infile->BuildError("Rating is invalid!");
  orientations = infile->lp.UnEscStr(1);
  if (!infile->lp.CharVal(2,warn,ao3con::warn1_ac)) return infile->BuildError("Warn Flag is invalid!");
  buffer = infile->lp.UnEscStr(3);
  if (buffer.contains(qcheck)) {
    return infile->BuildError("Warning tags not correct!");
  }
  if (!infile->lp.IIntVal(4,eccount)) return infile->BuildError("Estimated chaper count is not a number!");
  if (!infile->lp.SIntVal(5,kudcount)) return infile->BuildError("The amount of kudos is invalid!");
  // lines 9, 10, 11
  if (!infile->ReadUnEsc(characters)) return infile->BuildError("Could not get characters!");
  if (!infile->ReadUnEsc(relationships)) return infile->BuildError("Could not get pairings!");
  QString joined_extra_tags;
  if (!infile->ReadUnEsc(joined_extra_tags)) return infile->BuildError("Could not get tags!");
  extratags = joined_extra_tags.split(',');
  // finally
  return true;
}
//*******************************************************************************
