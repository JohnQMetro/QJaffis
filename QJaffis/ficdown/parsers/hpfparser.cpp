/***************************************************************
 * Name:      hpfparser.cpp
 * Purpose:   Fic parser: Harry Potter fanfic
 * Author:    John Q Metro
 * Created:   July 4, 2016
 * Updated:   October 12, 2019
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
jfHPF_FicPartParser::jfHPF_FicPartParser():jfStoryPartParseBase(){
    spec = QRegExp("[&'']");
    wdiv = QRegExp("<div\\s+style=\"[^\"]+\">&nbsp;</div><br\\s*/>",Qt::CaseInsensitive);
}
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
bool jfHPF_FicPartParser::IDEPHelper(const QString& label, const QString& start,ulong& outval) {
    QString oerr;
    size_t odex = xparser.GetIndex();
    if (!xparser.MovePast(start)) return parsErr("Cannot find start of " + label);
    if (!xparser.MovePast(spec)) return parsErr("Cannot find end of " + label);
    if (!Str2ULong(xparser.GetSkipped(),outval)) return parsErr("Cannot convert string to number for " + label);
    if (!xparser.MovePast(">")) return parsErr("Cannot find end of containing block for " + label);
    return true;
}

// custom virtual methods that are implemented
//--------------------------------------
bool jfHPF_FicPartParser::ParseFirstPage(const QString& indata) {
  // constants
  const QString fname = "jfHPF_FicPartParser::ParseFirstPage";
  // variables
  jfFicExtract_HPF* result;
  QString buffer,buffer2,errval;
  unsigned long qval;
  size_t ccount;
  // starting
  /**/lpt->tLog(fname,1);
  // skippting past the header
  if (!xparser.MovePast("</header>")) {
    return parsErr("Could not find end of Header!");
  }
  // title
  if (!xparser.GetDelimited("<h2>","<i>",buffer)) {
      return parsErr("Could not extract title!");
  }
  // author id
  if (!IDEPHelper("author id","<a href=\'viewuser.php?uid=",qval)) return false;

  // author name
  if (!xparser.GetMovePast("</a>",buffer2)) {
      return parsErr("Cannot extract author pen-name!");
  }
  // now that we have three good pieces of data, build the result...
  result = new jfFicExtract_HPF();
  result->fic_title = buffer.trimmed();
  result->auth_id = qval;
  result->author_name = buffer2.trimmed();
  /**/lpt->tLog(fname,2,buffer.trimmed());

  // fic id
  if (!IDEPHelper("fic id","<a href=\'/reviews.php?storyid=",qval)) return false;
  result->fic_id = qval;

  // part count
  if (!xparser.MovePast("<div class=\'entry__key\'>Chapters</div>")) {
    /**/lpt->tLog(fname,5,buffer);
    delete result;
    return parsErr("Cannot find start of chapter count!");
  }
  if (!xparser.GetDelimitedULong("<div class=\'entry__value\'>","</div>",qval,errval)) {
      delete result;
      return parsErr("failed to extract chapter count!");
  }
  result->pcount = qval;

  // status
  if (!xparser.MovePast("<div class='entry__key'>Status</div>")) {
    /**/lpt->tLog(fname,6,buffer);
    delete result;
    return parsErr("Cannot find start of status!");
  }
  if (!xparser.GetDelimited("<div class=\'entry__value\'>","</div>",buffer)) {
      delete result;
      return parsErr("failed to extract status!");
  }

  /**/lpt->tLog(fname,7,buffer);
  result->complete = (buffer=="Completed");

  // next, all we need is the updated date
  if (!xparser.MovePast("<div class='entry__key'>Last Updated</div>")) {
    /**/lpt->tLog(fname,8,buffer);
    delete result;
    return parsErr("Cannot find start of updated!");
  }
  if (!xparser.GetDelimited("<div class=\'entry__value\'>","</div>",buffer)) {
      delete result;
      return parsErr("failed to extract status!");
  }

  /**/lpt->tLog(fname,9,buffer);
  QDateTime hpf_ud = QDateTime::fromString(buffer,"yyyy-MM-dd h:mma");
  if (!hpf_ud.isValid()) {
      delete result;
      return parsErr("Could not parse updated date time!");
  }
  result->updated_date = hpf_ud.date();

  if (!xparser.MovePastTwo("</section>","<tbody>")) {
    delete result;
    return parsErr("Could not find end of section / start of chapter list!");
  }
  // handling the chapters
  ccount = 0;
  /**/lpt->tLog(fname,10);
  // getting chapters ids
  const QString cstart = "<a class='h4' href='/viewstory.php?chapterid=";

  while (IDEPHelper("chapter id",cstart,qval)) {
    result->chapterids.push_back(qval);
    if (!xparser.GetMovePast("</a>",buffer)) {
      delete result;
      return parsErr("Problems extracting part name!");
    }
    (result->partnames).append(htmlparse::ConvertEntities(buffer.trimmed(),false));
    ccount++;
  }
  /**/lpt->tLogS(fname,11,ccount);

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
  const QString fname = "jfHPF_FicPartParser::ParseOtherPage";
  const QString hval = "<div class='navbar__r'>";
  // varibales
  QString buffer,oerr;
  jfFicPart result;
  ulong qval;
  // starting...
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(hval)) return parsErr("Cannot findend of header!");

  // getting partindex
  if (!xparser.GetDelimitedULong("<i class=\"fa fa-book\"></i>","/",qval,oerr)) {
      /**/lpt->tLog(fname,2,xparser.GetBlock(1000));
      return parsErr("Cannot extract Part Index! " + oerr);
  }
  result.partindex = qval;
  if (!xparser.GetMovePastULong("</a>",qval,oerr)) {
      /**/lpt->tLog(fname,3,xparser.GetBlock(1000));
      return parsErr("Cannot extract Part Count!");
  }
  result.partcount = qval;

  // moving to the part id and part name
  if (!xparser.MovePast("<section class='section section--2'>")) {
      /**/lpt->tLog(fname,4);
      return parsErr("Cannot find start of section 2");
  }
  const QString cid_st = "<a href='/viewstory.php?chapterid=";
  if (!IDEPHelper("chapter id",cid_st,qval)) return false;
  result.partid = qval;

  // next, we get the part title
  if (!xparser.GetMovePast("</a>",buffer)) {
      /**/lpt->tLog(fname,6);
      return parsErr("Unable to find part title!");
  }
  result.part_name = buffer.trimmed();

  // main contents
  if (!xparser.MovePast("<div class='storytext-container'")) {
      /**/lpt->tLog(fname,7);
      return parsErr("Cannot find start of main contents!");
  }
  if (!xparser.GetDelimited(">","<div class='chapter-nav chapter-nav--footer'>",buffer)) {
      /**/lpt->tLog(fname,8);
      return parsErr("Cannot extract main fic part contents!");
  }
  // finishing off for the results
  fpart = new jfFicPart(result);
  partcount = result.partcount;
  pagecount = partcount+1;
  /**/lpt->tLog(fname,9);
  // processing contents
  fpart->part_contents = PartProcessing(buffer);
  /**/lpt->tLog(fname,10);
  // done
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
QString jfHPF_FicPartParser::PartProcessing(QString inbuffer) {
    const QString fname = "jfHPF_FicPartParser::PartProcessing";
  /**/JDEBUGLOG(fname,1);
  inbuffer = inbuffer.trimmed();


  // trailing div
  if (inbuffer.endsWith("</div>",Qt::CaseInsensitive)) {
      inbuffer.chop(6);
      inbuffer = inbuffer.trimmed();
  }
  // misc non-breaking spaces messes
  inbuffer.replace("&nbsp;<br />","<br />");
  inbuffer.replace("<div>&nbsp;</div><br />","<br />");
  inbuffer.replace(wdiv,"<br />");
  inbuffer.replace("</div><br />","</div>");
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
