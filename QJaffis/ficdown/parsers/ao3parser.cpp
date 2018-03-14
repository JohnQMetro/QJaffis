/***************************************************************
 * Name:      ao3parser.cpp
 * Purpose:   Fic parser: Archiveofourown.org
 * Author:    John Q Metro
 * Created:   July 6, 2016
 * Updated:   July 19, 2017
 *
 **************************************************************/
#ifndef AO3PARSER_H
  #include"ao3parser.h"
#endif // AO3PARSER_H
//-------------------------------
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//-------------------------------
#include <assert.h>
#include <QStringList>
/**************************************************************/
const QString cstart1 = "<h3 class=\"landmark heading\" id=\"work\">Chapter Text</h3>";
const QString cstart2 = "<h3 class=\"landmark heading\" id=\"work\">Work Text:</h3>";
//===================================================================================
// constructor
jfAO3_FicPartParser::jfAO3_FicPartParser():jfStoryPartParseBase() {}
//++++++++++++++++++++++++++++++++++++++++
// virual methods that are implemented
//------------------------------------
QString* jfAO3_FicPartParser::makeRedirectedURL(const QString& inPart) {
  if (inPart=="https://archiveofourown.org/") return NULL;
  // AO3 redirects are pretty simple
  QString* rresult = new QString(inPart);
  (*rresult) += "?view_adult=true";
  return rresult;
}
//------------------------------------
bool jfAO3_FicPartParser::testMissing(const QString *page) const {
  // constants and variables
  const QString mstring1 = "<h3 class=\"heading\">The page you were looking for doesn't exist.</h3>";
  // checking
  assert(page!=NULL);
  bool rval = page->contains(mstring1);
  return !rval;
}
//------------------------------------
bool jfAO3_FicPartParser::testIncomplete(const QString *page) const {
  // constants and variables
  const QString trunc1 = "<!-- BEGIN comment section -->";
  // checking
  assert(page!=NULL);
  return (page->contains(trunc1));
}
//------------------------------------
QString jfAO3_FicPartParser::getCookie() const {
  return "";
}
//++++++++++++++++++++++++++++++++++++++++
// custom virtual methods that are implemented
//------------------------------------
bool jfAO3_FicPartParser::ParseFirstPage(const QString& indata) {
  // constants
  const QString fname = "jfAO3_FicPartParser::ParseFirstPage";

  // variables
  QString partname,xbuffer;
  jfFicPart tempfic;
  // initializing
  /**/lpt->tLog(fname,1);
  // skipping past the header gunk
  if (!xparser.MovePast("<ul class=\"work navigation actions\" role=\"menu\">")) {
    return parsErr("Header info error!");
  }
  // initial chapter info (plus the possible name of the first part!)
  jfFicExtract_AO3* ffresult = new jfFicExtract_AO3();
  if (!ExtractChapterFicIDs(ffresult,partname)) {
    delete ffresult;
    /**/lpt->tLog(fname,2);
    return false;
  }
  // loading up on some of the fic part data
  tempfic.part_name = partname;
  tempfic.partindex = 1;
  tempfic.partcount = ffresult->pcount;
  tempfic.partid = ffresult->chapterids[0];

  // skiping past more stuff to skip
  if (!xparser.MovePast("<dt class=\"stats\">Stats:</dt>")) {
    delete ffresult;
    /**/lpt->tLog(fname,3);
    return parsErr("Cannot find stats line!");
  }

  if (!DatesAndCompletion(ffresult)) {
    delete ffresult;
    /**/lpt->tLog(fname,4);
    return false;
  }
  /**/lpt->tLog(fname,5);
  // title
  QString buffer;
  if (!xparser.GetDelimited("<h2 class=\"title heading\">","</h2>",buffer)) {
    delete ffresult;
    return parsErr("Cannot find title!");
  }
  ffresult->fic_title = buffer.trimmed();
  /**/lpt->tLog(fname,6,buffer);
  // author name
  if (!xparser.MovePast("<a rel=\"author\"")) {
      delete ffresult;
      return parsErr("AO3 Cannot find author name!");
  }
  if (!xparser.GetDelimited("\">","</a>",buffer)) {
    delete ffresult;
    return parsErr("AO3 Cannot extract author name!");
  }
  ffresult->author_name = buffer.trimmed();
  // no author id
  ffresult->auth_id = 1;
  /**/lpt->tLog(fname,7);
  // getting special notes
  tempfic.startnotes = GetStartNotes();
  /**/lpt->tLog(fname,8,tempfic.startnotes);
  // getting part contents
  // single part
  if (partcount==1) {
    /**/lpt->tLog(fname,9);
    if (!xparser.GetDelimited(cstart2,"<!-- end cache -->",xbuffer)) {
      delete ffresult;
      return parsErr("AO3 P1s Cannot find contents!");
    }
  }
  else {
    /**/lpt->tLog(fname,10);
    if (!xparser.GetDelimited(cstart1,"<!--/main-->",xbuffer)) {
      delete ffresult;
      return parsErr("AO3 P1m Cannot find contents!");
    }
  }
  /**/lpt->tLog(fname,11);
  // here, there is no chance of faliure, so we set some results
  extract = ffresult;
  pagecount = partcount;
  // there might be endnotes
  if (xparser.MovePast("<h3 class=\"heading\">Notes:</h3>")){
    if (!xparser.GetDelimitedFromEnd("<blockquote class=\"userstuff\">","</blockquote>",buffer,"</div>")) {
      if (!xparser.GetMovePast("</div>",buffer)) buffer="";
    }
    tempfic.endnotes = buffer.trimmed();
  }
  fpart = new jfFicPart(tempfic);
  fpart->part_contents = PartProcessing(xbuffer);
  /**/lpt->tLog(fname,12);
  return true;
}

//------------------------------------
bool jfAO3_FicPartParser::ParseOtherPage() {
  // constants
  const QString fname = "jfAO3_FicPartParser::ParseOtherPage";
  const QString navstart = "<ul class=\"work navigation actions\" role=\"menu\">";
  const QString navend = "<li class=\"comments\" id=\"show_comments_link_top\">";

  // variables
  QString buffer,notes,xbuffer;
  jfFicPart thispart;
  // getting the chapter info
  /**/lpt->tLog(fname,1);
  if (!xparser.GetDelimited(navstart,navend,buffer)) return parsErr("Missing Chapter Data!");
  // processing chapter info
  buffer = buffer.trimmed();
  /**/lpt->tLog(fname,2);
  if (!HandleChapters(buffer,thispart)) {
    return parsErr("Could not process chapter info! : " + parseErrorMessage);
  }
  thispart.startnotes = GetStartNotes();
  /**/lpt->tLog(fname,3);
  // getting part contents
  // single part
  if (partcount==1) {
    if (!xparser.GetDelimited(cstart2,"<!-- end cache -->",xbuffer)) {
      return parsErr("AO3 POs Cannot find contents!");
    }
  }
  else {
    if (!xparser.GetDelimited(cstart1,"<!--/main-->",xbuffer)) {
      return parsErr("AO3 POm Cannot find contents!");
    }
  }
  /**/lpt->tLog(fname,4,xbuffer);
  // getting special notes
  if (xparser.MovePast("<!--work endnotes-->")) {
    if (xparser.MovePast("<h3 class=\"heading\">Notes:</h3>")){
      if (!xparser.GetDelimitedFromEnd("<blockquote class=\"userstuff\">","</blockquote>",buffer,"</div>")) {
        if (!xparser.GetMovePast("</div>",buffer)) buffer="";
      }
      thispart.endnotes = buffer.trimmed();
    }
  }
  // after
  fpart = new jfFicPart(thispart);
  fpart->part_contents = PartProcessing(xbuffer);
  /**/lpt->tLog(fname,5);
  return true;
}

//++++++++++++++++++++++++++++++++++++++++
//helper methods
//------------------------------------
bool jfAO3_FicPartParser::ExtractChapterFicIDs(jfFicExtract_AO3* extract_ptr, QString& out_partname) {
  // constants
  const QString fname = "jfAO3_FicPartParser::ExtractChapterFicIDs";
  const QString toomuch = "<textarea cols=\"50\" rows=\"4\" id=\"embed_code\">&lt;a href=&quot;https://archiveofourown.org/works/";
  const QString embed_code = "<textarea cols=\"50\" rows=\"4\" id=\"embed_code\">";
  const QString select1 = "<select name=\"selected_id\" id=\"selected_id\">";
  const QString select2 = "<select id=\"selected_id\" name=\"selected_id\">";
  // variables
  unsigned long qval;
  QString buffer,errres;
  QString tempname;
  bool ficidf = false;
  // initializing
  assert(extract_ptr!=NULL);
  /**/lpt->tLog(fname,1);
  /* chapter navigation is next, we use it to get partcount, chapter ids, and the partname */
  if (!xparser.MovePastAlt(select1,select2)) {
    // single part stories
    partcount = extract_ptr->pcount = 1;
    /**/lpt->tLogS(fname,2,partcount);
    if (!xparser.MovePast(embed_code)) return parsErr("Cannot find link TEXTAREA!");
    /**/lpt->tLog(fname,3);
    extract_ptr->chapterids.push_back(1);
    extract_ptr->partnames.append("");
    out_partname = "";
  }
  else {
    // multipart stories
    size_t ccount = 0;
    /**/lpt->tLog(fname,4);
    while (xparser.MovePastLimit("<option ","</select>")) {
      if (!xparser.GetDelimitedULong("value=\"","\"",qval,errres)) {
          xparser.GoBack();
          break;
      }
      ccount++;
      extract_ptr->chapterids.push_back(qval);
      if (!xparser.MovePast(".")) return parsErr("Problems with start of part name!");
      if (!xparser.GetMovePast("</option>",buffer)) {
        /**/lpt->tLog(fname,5);
        return parsErr("Problems with end of part name!");
      }
      if (ccount == 1) out_partname = buffer.trimmed();
      tempname = htmlparse::ConvertEntities(buffer.trimmed(),false);
      (extract_ptr->partnames).append(tempname);
    }
    /**/lpt->tLogS(fname,6,ccount);
    if (ccount==0) return parsErr("Unable to find any parts!");
    partcount = extract_ptr->pcount = ccount;
    /**/lpt->tLog(fname,7);
    if (xparser.GetDelimitedULong("<li><a href=\"/works/","/navigate",qval,errres)) {
        extract_ptr->fic_id = qval;
        ficidf = true;
    }
  }
   /**/lpt->tLog(fname,8);
  // getting the fic id from twitter buttons...
  if (!ficidf) {
      const QString twit1 = "<a href=\"http://twitter.com/share\"";
      const QString twit2 = "<a href=\"http://www.twitter.com/share\"";
      if (!xparser.MovePastAlt(twit1,twit2)) {
          return parsErr("Problems with getting fic id! : Twitter");
      }
      /**/lpt->tLog(fname,9);
      if (!xparser.GetDelimitedULong("data-url=\"https://archiveofourown.org/works/","\"",qval,errres)) {
          return parsErr("Problems with getting fic id! : " + errres);
      }
      /**/lpt->tLog(fname,10);
      extract_ptr->fic_id = qval;
  }

  return true;
}
//------------------------------------
bool jfAO3_FicPartParser::DatesAndCompletion(jfFicExtract_AO3* extract_ptr) {
  assert(extract_ptr != NULL);
  // constants
  const QString fname = "jfAO3_FicPartParser::DatesAndCompletion";
  // variables
  QString buffer;
  // completion
  extract_ptr->complete = true;
  /**/lpt->tLog(fname,1);
  // the date is somewhat complicated, as we have 3 successive options
  if (xparser.GetDelimited("Completed:</dt><dd class=\"status\">","</dd>",buffer)) buffer = buffer.trimmed();
  else if (xparser.GetDelimited("Updated:</dt><ddclass=\"status\">","</dd>",buffer)) {
    buffer = buffer.trimmed();
    /**/lpt->tLog(fname,2,buffer);
    extract_ptr->complete = false;
  }
  else if (xparser.GetDelimited("Published:</dt><dd class=\"published\">","</dd>",buffer)) {
    buffer = buffer.trimmed();
  }
  else {
    return parsErr("Cannot find date!");
  }
  /**/lpt->tLog(fname,3);
  // parsing the date
  extract_ptr->updated_date = QDate::fromString(buffer,"yyyy-MM-dd");
  if (!(extract_ptr->updated_date).isValid()) {
    return parsErr("Parsing date failed!");
  }
  /**/lpt->tLog(fname,4);
  return true;
}

//------------------------------------
bool jfAO3_FicPartParser::HandleChapters(QString inbuffer,jfFicPart& fpoint){
  // constants
  const QString fname = "jfAO3_FicPartParser::HandleChapters";
  const QString select1 = "<select name=\"selected_id\" id=\"selected_id\">";
  const QString select2 = "<select id=\"selected_id\" name=\"selected_id\">";
  // variables
  jfStringParser* qparser;
  size_t ccount;
  ulong oval;
  QString buffer, outerr;
  bool current;
  // starting
  /**/lpt->tLog(fname,1);
  qparser = new jfStringParser(inbuffer);
  /* chapter navigation is next, we use it to get partcount, chapter ids, and the partname */
  if (!qparser->MovePastAlt(select1,select2)) {
    // single part stories
    pagecount = partcount = fpoint.partcount = 1;
    fpoint.partindex = 1;
    partcount = 1;
    /**/lpt->tLog(fname,2);
  }
  else {
    // multipart stories
    ccount = 0;
    /**/lpt->tLog(fname,3);
    while (qparser->MovePastLimit("<option","</select>")) {
      ccount++;
      // checking for selected...
      if (!qparser->GetMovePast("value=\"",buffer)) {
          delete qparser;
          return parsErr("Cannot find start of chapter id!");
      }
      buffer = buffer.trimmed();
      current = (buffer=="selected=\"selected\"");
      // getting the part id
      if (!qparser->GetMovePastULong("\"",oval,outerr)) {
        delete qparser;
        return parsErr("Error while getting chapter id: " +outerr);
      }
      fpoint.partid = oval;
      // processing the current part to get the name
      if (current) {
        fpoint.partindex = ccount;
        if (!qparser->GetDelimited(".","</option>",buffer)) {
          delete qparser;
          return parsErr("Cannot find end of option!");
        }
        fpoint.part_name = buffer.trimmed();
      }
      else continue;
    }
    // post loop
    /**/lpt->tLogS(fname,4,ccount);
    pagecount = partcount = fpoint.partcount = ccount;
  }
  // finishing touches
  delete qparser;
  return (partcount!=0);
}

//------------------------------------
QString jfAO3_FicPartParser::PartProcessing(QString inbuffer) {
  inbuffer = inbuffer.trimmed();
  // getting rid of trailing div
  inbuffer.chop(6);
  inbuffer = inbuffer.trimmed();
  return inbuffer;
}
//------------------------------------
QString jfAO3_FicPartParser::GetStartNotes() {
  const QString fname = "jfAO3_FicPartParser::GetStartNotes";
  const QString notemark = "<h3 class=\"heading\">Notes:</h3>";
  const QString blockstart = "<blockquote class=\"userstuff\">";
  // building the list to check
  QStringList* idlist = new QStringList();
  idlist->append(notemark);
  idlist->append(cstart1);
  idlist->append(cstart2);
  // variables for finding
  size_t which, outloc;
  QString buffer;
  // looking for the strings
  if (xparser.Find1stSet(idlist,which,outloc)) {
    // startnotes have been found!
    if (which == 0) {
      xparser.MovePast(notemark);
      // annoyingly, these notes are sometimes in a blockquotes, sometimes not
      if (!xparser.GetDelimitedFromEnd(blockstart,"</blockquote>",buffer,"</div>")) {
        if (!xparser.GetMovePast("</div>",buffer)) return "";
      }
      return  buffer.trimmed();
    }
    return "";
  }
  return "";
}

/**************************************************************/

