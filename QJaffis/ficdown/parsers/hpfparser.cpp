/***************************************************************
 * Name:      hpfparser.cpp
 * Purpose:   Fic parser: Harry Potter fanfic
 * Author:    John Q Metro
 * Created:   July 4, 2016
 * Updated:   April 18, 2018
 *
 **************************************************************/
#ifndef HPFPARSER_H
  #include "hpfparser.h"
#endif // HPFPARSER_H
//--------------------------
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//--------------------------
#include <assert.h>
/**************************************************************/
// +++ [ METHODS for jfHPF_FicPartParser ] +++
//+++++++++++++++++++++++++++++++++++++++++++++++
jfHPF_FicPartParser::jfHPF_FicPartParser():jfStoryPartParseBase(){}
//+++++++++++++++++++++++++++++++++++++++++++++++
// virual methods that are implemented
//--------------------------------------
QString* jfHPF_FicPartParser::makeRedirectedURL(const QString& inPart) {
  return NULL;
}
//--------------------------------------
bool jfHPF_FicPartParser::testMissing(const QString *page) const {
  // constants and variables
  const QString mstring1 = "ERROR locating story meta for psid";
  // checking
  assert(page!=NULL);
  bool rval = page->contains(mstring1);
  return !rval;
}
//--------------------------------------
bool jfHPF_FicPartParser::testIncomplete(const QString *page) const {
  return true;
}
//--------------------------------------
QString jfHPF_FicPartParser::getCookie() const{
  return "";
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// custom virtual methods that are implemented
//--------------------------------------
bool jfHPF_FicPartParser::ParseFirstPage(const QString& indata) {
  // constants
  const QString fname = "ExtractFromIndex_HPF";
  const QString hpciex = "<a href=\"/viewstory2.php?chapterid=";
  const QString abhead1 = "&gt;&gt;</a> </form></span><br clear=all>";
  // variables
  jfFicExtract_HPF* result;
  QString buffer,errval;
  unsigned long qval;
  size_t ccount;
  // starting
  /**/lpt->tLog(fname,1);
  // skippting past the header
  if (!xparser.MovePastAlt(abhead1,">Report Abuse</a>&nbsp;]")) {
    return parsErr("Could not find initial tags!");
  }
  // fic id
  if (!xparser.GetDelimitedULong("<center><a href=\"?psid=","\">",qval,errval)) {
    if (!xparser.GetDelimitedULong(" location = '?psid=","'\">",qval,errval)) {
      return parsErr("Could not get fic id!");
    }
  }
  /**/lpt->tLog(fname,2);
  // title
  if (!xparser.GetMovePast("</a>",buffer)){
    return parsErr("Could not find end of title!");
  }
  // now that we have two good pieces of data, build the result...
  result = new jfFicExtract_HPF();
  result->fic_id = qval;
  result->fic_title = buffer.trimmed();
  /**/lpt->tLog(fname,3,buffer);

  // author id
  if (!xparser.GetDelimitedULong("<a href=\"viewuser.php?showuid=","\">",qval,errval)) {
    delete result;
    return parsErr("Cannot extract author id");
  }
  // author name
  if (!xparser.GetMovePast("</a>",buffer)) {
    delete result;
    return parsErr("Cannot extract author pen-name!");
  }
  result->auth_id = qval;
  result->author_name = buffer.trimmed();
  /**/lpt->tLog(fname,4,buffer);
  // part count
  if (!xparser.GetDelimitedULong("<b>Chapters:</b>","<br>",qval,errval)) {
       /**/lpt->tLog(fname,5,buffer);
    delete result;
      /**/lpt->tLog(fname,6,buffer);
    return parsErr("Cannot extract chapter count!");
  }
  /**/lpt->tLog(fname,6,buffer);
  // status
  if (!xparser.GetDelimited("<b>Status:</b>","<br>",buffer)) {
    delete result;
    return parsErr("Cannot extract status");
  }
  /**/lpt->tLog(fname,7,buffer);
  result->pcount = qval;
  buffer = buffer.trimmed();
  /**/lpt->tLog(fname,8,buffer);
  result->complete = (buffer=="Completed");

  // next, all we need is the updated date, much easier than fimfiction
  if (!xparser.GetDelimited("<b>Last Updated:</b> ","<br />",buffer)) {
    delete result;
    return parsErr("Cannot find updated date!");
  }
  /**/lpt->tLog(fname,9,buffer);
  (result->updated_date) = QDate::fromString(buffer,"yyyy.MM.dd");
  if (!(result->updated_date).isValid()){
    delete result;
    return parsErr("Could not parse updated date!");
  }
  if (!xparser.MovePastTwice("</table>")) {
    delete result;
    return parsErr("Could not find table ends!");
  }
  // handling the chapters
  ccount = 0;
  /**/lpt->tLog(fname,10);
  // getting chapters ids
  while (xparser.GetDelimitedULong("<a href=\"?chapterid=","\">",qval,errval)) {
    result->chapterids.push_back(qval);
    if (!xparser.GetMovePast("</a>",buffer)) {
      delete result;
      return parsErr("Problems extracting part name!");
    }
    (result->partnames).append(htmlparse::ConvertEntities(buffer.trimmed(),false));
    ccount++;
  }
  /**/lpt->tLogS(fname,11,ccount);
  // alternate version that works for 'mature' fics
  if (ccount==0) {
    while (xparser.GetDelimitedULong(hpciex,"&i=",qval,errval)) {
      result->chapterids.push_back(qval);
      if (!xparser.GetDelimited("\">","</a>",buffer)) {
        delete result;
        return parsErr("Problems extracting part name!");
      }
      (result->partnames).append(htmlparse::ConvertEntities(buffer.trimmed(),false));
      ccount++;
    }
  }
  /**/lpt->tLogS(fname,12,ccount);
  // checking the result
  if ((result->pcount)!=ccount) {
    delete result;
    return parsErr("Stated partcount does not match list of parts!");
  }
  // done
  /**/lpt->tLog(fname,13);
  partcount = result->pcount;
  pagecount = partcount + 1;
  extract = result;
  return true;
}

//--------------------------------------
bool jfHPF_FicPartParser::ParseOtherPage() {
  // consstants
  const QString fname = "jfFicPart_HPF::SetFromStringCore";
  const QString hval = "Report Abuse</a>&nbsp;]";
  const QString cboxs = "<form style=\"margin:0\" enctype=\"multipart/form-data\" method=\"post\" action=\"\">";
  // varibales
  QString buffer;
  jfFicPart result;
  // starting...
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(hval)) return parsErr("Cannot find header tag!");
  // we get chapter info (used to determine partcount and partindex)
  if (xparser.GetDelimited(cboxs,"</form>",buffer)) {
    if (!HandleChapters(buffer,result)) {
      return parsErr("Chapter Processing failed");
    }
  }
  else {
    // we assume 1 part
    result.partcount = partcount = 1;
    result.partindex = 1;
  }
  /**/lpt->tLog(fname,2);
  // moving to the part name
  if (!xparser.MovePastTwo("<center>","<br>")) return false;
  if (!xparser.MovePast(" : ")) return false;
  // getting the part name
  if (!xparser.GetMovePast("</center>",buffer)) return false;
  result.part_name = buffer.trimmed();
  /**/lpt->tLog(fname,3);
  // next, the contents
  if (!xparser.GetDelimited("<div id='fluidtext'>","</div><br><hr>",buffer)) return false;
  // finishing off for the results
  fpart = new jfFicPart(result);
  partcount = result.partcount;
  pagecount = partcount+1;
  /**/lpt->tLog(fname,4);
  // processing contents
  fpart->part_contents = PartProcessing(buffer);
  /**/lpt->tLog(fname,5);
  // done
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//helper methods
//--------------------------------------
bool jfHPF_FicPartParser::HandleChapters(QString inbuffer,jfFicPart& fpoint) {
  // variables
  jfStringParser* qparser;
  QString buffer;
  size_t ccount;
  bool fail = false;
  unsigned long tval;
  QString oerr;
  // we start
  qparser = new jfStringParser(inbuffer);
  ccount = 0;
  // the main parsing loop
  while (qparser->MovePast("<option value=\"?chapterid=")) {
    ccount++;
    // looking for the part id
    if (!qparser->GetMovePastULong("\"",tval,oerr)) fail = true;
    else if (!qparser->GetMovePast(">",buffer)) fail = true;
    else {
      buffer =buffer.trimmed();
      if (buffer=="selected") {
        fpoint.partindex = ccount;
        fpoint.partid = tval;
      }
      if (!qparser->MovePast(".")) fail = true;
    }
    if (fail) break;
  }
  // done with the loop
  delete qparser;
  if (fail) return false;
  if (ccount==0) return false;
  fpoint.partcount = ccount;
  return true;
}

//--------------------------------------
QString jfHPF_FicPartParser::PartProcessing(QString inbuffer) {
    const QString fname = "jfHPF_FicPartParser::PartProcessing";
  /**/JDEBUGLOG(fname,1);
  inbuffer = inbuffer.trimmed();
  // misc non-breaking spaces
  inbuffer.replace("&nbsp;<br />","<br />");
  inbuffer.replace("<div>&nbsp;</div><br />","<br />");
  inbuffer.replace("<div><br />\n&nbsp;</div><br />","<br />\n<br />");
  /**/JDEBUGLOG(fname,2)
  // getting rid of too may linebreaks... we do need the loop
  int oldlen,newlen;
  do {
      oldlen = inbuffer.length();
      inbuffer.replace("<br />\n<br />\n<br />","<br />\n<br />");
      newlen = inbuffer.length();
  } while (oldlen != newlen);
  return inbuffer;
}
/**************************************************************/
