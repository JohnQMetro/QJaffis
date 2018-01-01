/***************************************************************
 * Name:      mmoparser.cpp
 * Purpose:   Fic parser: MediaMiner.org
 * Author:    John Q Metro
 * Created:   July 6, 2016
 * Updated:   August 4, 2016
 *
 **************************************************************/
#ifndef MMOPARSER_H
  #include "mmoparser.h"
#endif // MMOPARSER_H
//------------------------------------------
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//----------------------------------------
#include <assert.h>
/**************************************************************/
// constructor
jfMMO_FicPartParser::jfMMO_FicPartParser():jfStoryPartParseBase() {}
//+++++++++++++++++++++++++++++++++++++++++
// virual methods that are implemented
//------------------------------------
// mediaminer.org no longer does redirections
QString* jfMMO_FicPartParser::makeRedirectedURL(const QString& inPart) {
  return NULL;
}
//------------------------------------
bool jfMMO_FicPartParser::testMissing(const QString *page) const {
  return true;
}
//------------------------------------
bool jfMMO_FicPartParser::testIncomplete(const QString *page) const{
  assert(page!=NULL);
  return page->contains("<footer>");
}
//------------------------------------
QString jfMMO_FicPartParser::getCookie() const {
  return "";
}
//+++++++++++++++++++++++++++++++++++++++++
// custom virtual methods that are implemented
//------------------------------------
bool jfMMO_FicPartParser::ParseFirstPage(const QString& indata) {
  // constants
  const QString fname = "jfMMO_FicPartParser::ParseFirstPage";
  // variables
  jfFicExtract_MMO* result;
  QString buffer, outerr;
  ulong tval;

  // the url fragment, and fic id, can best be found in the <head>
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast("<link rel=\"canonical\" href=\"http://www.mediaminer.org/fanfic/s/")) {
    return parsErr("Cannot find story link in head!");
  }
  if (!xparser.MovePastTwice("/")) return parsErr("Problems in parsing link url!");
  buffer = xparser.GetSkipped();
  if (!xparser.GetMovePastULong("\" /> ",tval,outerr)) {
    return parsErr("Problems with getting fic ID : " + outerr);
  }
  // Moving past the header (HTML 5!)
  if (!xparser.MovePast("</header>")) return parsErr("Cannot find end of header!");
  if (!xparser.MovePast("<h1 id=\"post-title\">")) return parsErr("Cannot find start of title!");

  // time to start making the extract result!
  result = new jfFicExtract_MMO();
  result->url_cname_part = buffer;
  result->fic_id = tval;

  // extracting the title
  if (!xparser.GetDelimited("&#10095;","</h1>",buffer)) {
    delete result;
    return parsErr("Cannot extract title!");
  }
  result->fic_title = buffer.trimmed();
  /**/lpt->tLog(fname,2,buffer);
  // the author id
  if (!xparser.GetDelimitedULong("<b>By: </b><a href=\"/user_info.php/","\">",tval,outerr)) {
    delete result;
    return parsErr("Cannot extract author id! : " + outerr);
  }
  result->auth_id = tval;
  // the author name
  if (!xparser.GetMovePast("</a>",buffer)) {
    delete result;
    return parsErr("Cannot find end of author name!");
  }
  result->author_name = buffer.trimmed();
  // net up, the ‘Uploaded Date’ — which is the only date provided, seems to be updated
  if (!xparser.GetDelimited("<b>Uploaded On: </b>","|",buffer)) {
    delete result;
    return parsErr("Cannot get uploaded date!");
  }
  buffer = buffer.trimmed();
  result->updated_date = QDate::fromString(buffer,"MM.dd.yyyy");
  if (!(result->updated_date).isValid()) {
    delete result;
    return parsErr("Could not convert the date!");
  }
  /**/lpt->tLog(fname,3,buffer);
  // annotingly, there is nothing to indicate completion status
  result->complete = false;

  // next up, the parts/chapters!
  size_t pcount = 0;
  while (xparser.MovePast("<a href=\"/fanfic/c/")) {
    pcount++;
    /**/lpt->tLogS(fname,4,pcount);
    if (!xparser.MovePastTwice("/")) {
      delete result;
      return parsErr("Problems with parsing chapter link!");
    }
    // getting the chaper id
    if (!xparser.GetDelimitedULong("/","\">",tval,outerr)) {
      delete result;
      return parsErr("Problem getting chapter id : " + outerr);
    }
    // part name
    if (!xparser.GetMovePastPair("(","</a>",buffer)) {
      delete result;
      return parsErr("Problem getting chapter nme!");
    }
    (result->chapterids).push_back(tval);
    buffer = htmlparse::ConvertEntities(buffer.trimmed(),false);
    (result->partnames).append(buffer);
  }
  result->pcount = pcount;

  // all done, except for some finishing touches
  partcount = pcount;
  pagecount = partcount + 1;
  extract = result;
  return true;
}

//------------------------------------
bool jfMMO_FicPartParser::ParseOtherPage() {
  // func constant
  const QString fname = "jfMMO_FicPartParser::ParseOtherPage";
  // variables
  jfFicPart tempfic;
  QString buffer,outerr;
  ulong tval;

  // the part id can best be found in the <head>
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast("<link rel=\"canonical\" href=\"http://www.mediaminer.org/fanfic/c/")) {
    return parsErr("Cannot chapter link in head!");
  }
  if (!xparser.MovePastTwice("/")) return parsErr("Problems in parsing link url!");
  if (!xparser.GetDelimitedULong("/","\" />",tval,outerr)) {
    return parsErr("Problems with getting chapter ID : " + outerr);
  }
  tempfic.partid = tval;

  // Moving past the header (HTML 5!)
  /**/lpt->tLogS(fname,2,tempfic.partid);
  if (!xparser.MovePast("</header>")) return parsErr("Cannot find end of header!");
  if (!xparser.MovePastTwo("<h1 id=\"post-title\">","&#10095;")) return parsErr("Cannot find start of complete title!");

  // getting the part name
  if (!xparser.GetDelimited("&#10095;","</b> <font size=\"-1\">",buffer)) {
    return parsErr("Cannot extract chapter name!");
  }
  tempfic.part_name = buffer.trimmed();
  /**/lpt->tLog(fname,3,buffer);

  // extracting the part contents
  if (!xparser.GetDelimited("<div id=\"fanfic-text\">","<div class=\"adWrap\">",buffer)) {
    return parsErr("Could not extract the part contents!");
  }
  // parsing the chapter list to get partcount
  size_t pxcount = 0;
  if (!xparser.MovePast("<form method=\"get\" action=\"/fanfic/view_ch.php\"")) {
    tempfic.partindex = 1;
    pxcount = 1;
  }
  else {
    /**/lpt->tLog(fname,4);
    while (xparser.MovePast("<option value=\"")) {
      pxcount++;
      if (!xparser.GetMovePastULong("\"",tval,outerr)) {
        return parsErr("Could not get chapter id in chapter list :" + outerr);
      }
      if (tval == tempfic.partid) tempfic.partindex = pxcount;
    }
    if (pxcount == 0) return parsErr("No chapters found while parsing chapter!?");
    if (tempfic.partindex == 0) return parsErr("The chapter index is not in the chapter list!");
  }

  // final touches
  partcount = tempfic.partcount = pxcount;
  pagecount = pxcount+1;
  fpart = new jfFicPart(tempfic);
  fpart->part_contents = PartProcessing(buffer);
  /**/lpt->tLog(fname,5);
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++
//helper methods
//------------------------------------
QString jfMMO_FicPartParser::PartProcessing(QString inbuffer) {
  inbuffer = inbuffer.trimmed();
  // chopping div
  inbuffer.chop(6);
  return inbuffer.trimmed();
}

/**************************************************************/

