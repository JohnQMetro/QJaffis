/***************************************************************
 * Name:      basefim.cpp
 * Purpose:   Fic parsers: base abstract class and fim
 * Author:    John Q Metro
 * Created:   July 3, 2016
 * Updated:   June 7, 2017 (Fimfiction 4 parsing fixes)
 *
 **************************************************************/
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H
//----------------------------------
#ifndef FIM_FICOBJ_H_INCLUDED
  #include "../../fanfic/data/fim/fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED
//----------------------------------
#include <assert.h>

/*************************************************************/
// the constructor
jfStoryPartParseBase::jfStoryPartParseBase():jfPageParserBase() {
  extract = NULL;
  fpart = NULL;
  partcount = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
void jfStoryPartParseBase::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  const QString fname = "jfStoryPartParseBase::ParseDownloadedPage";
  // clearing out old data and setting the parser
  /**/lpt->tLog(fname,1);
  NewData(inPage);
  extract = NULL;
  fpart = NULL;
  partcount = 0;
  // calling the sub parsers depending on first page
  bool result;
  if (pageindex == 1) result = ParseFirstPage(inPage);
  else if (pageindex > 1) result = ParseOtherPage();
  else assert(false);
  // done
  page_parsed = result;
}

//---------------------------------------------
void* jfStoryPartParseBase::getResults() {
  if ((extract != NULL) && (fpart != NULL)){
    jfFicPair* reval = new jfFicPair();
    reval->extract = extract;
    reval->part_one = fpart;
    return reval;
  }
  else if (extract !=NULL) return extract;
  else return fpart;
}
//==========================================================
// +++ [ METHODS for jfFIM_FicPartParser ]+++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the constructor
jfFIM_FicPartParser::jfFIM_FicPartParser():jfStoryPartParseBase() {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virual methods that are implemented
//------------------------------------
QString* jfFIM_FicPartParser::makeRedirectedURL(const QString& inPart){
  return NULL;
}
//------------------------------------
bool jfFIM_FicPartParser::testMissing(const QString *page) const {
  // constants and variables
  const QString mstring1 = "<img src='//static.fimfiction.net/images/missing_story.png' />";
  // checking
  assert(page!=NULL);
  bool rval = page->contains(mstring1);
  return !rval;
}
//------------------------------------
bool jfFIM_FicPartParser::testIncomplete(const QString *page) const {
  // constants and variables
  const QString trunc1 = "<div id=\"story_comments\" class=\"comment_content_box\"";
  // checking
  assert(page!=NULL);
  bool rval = page->contains(trunc1);
  return rval;
}
//------------------------------------
QString jfFIM_FicPartParser::getCookie() const {
  return "view_mature=true";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom virtual methods that are implemented
//------------------------------------
// the first page in a FIM fic download is the index
bool jfFIM_FicPartParser::ParseFirstPage(const QString& indata) {
  const QString fname= "jfFIM_FicPartParser::ParseFirstPage";
  // variables
  jfFicExtract_FIM* result;
  jfFIM_Fanfic* fres;
  QString outerr;
  /* The stuff in the index page is almost identical to what gets fetched
   * in FIM Search results, so I've overloaded the class for parsing and
   * and holding those, to also get the fic extract for the FIM fic index
   * page.*/
  /**/lpt->tLog(fname,1);
  fres = new jfFIM_Fanfic();
  result = fres->ExtractFromString(indata,outerr);
  /**/lpt->tLog(fname,2,outerr);
  delete fres;
  // checking the result
  if (result == NULL) {
    parseErrorMessage = outerr;
    return false;
  }
  else {
    partcount = result->pcount;
    pagecount = partcount + 1;
    /**/lpt->tLog(fname,3);
    extract = result;
    return true;
  }
}
//------------------------------------
// getting the pic parts
bool jfFIM_FicPartParser::ParseOtherPage() {
  // consstants
  const QString fname = "jfFIM_FicPartParser::ParseOtherPage";
  // varibles
  QString buffer;
  jfFicPart temp_part;
  // starting...
  /**/lpt->tLog(fname,1);
  // chapter proccessing
  if (!xparser.MovePast("<h1 class=\"chapter-title\">")) return false;
  // chapter processing next
  if (xparser.MovePast("<div class=\"button-group ")) {
      /**/lpt->tLog(fname,3);
    // we get chapter info (used to determine partcount and partindex)
    if (!xparser.GetDelimited("<ul","</ul>",buffer)) return false;
    if (!HandleChapters(buffer,temp_part)) {
      parseErrorMessage = "Chapter Processing failed\n" + buffer;
      return false;
    }
  }
  else {
    /**/lpt->tLog(fname,6);
    partcount = temp_part.partcount = 1;
    temp_part.partindex = 1;
    pagecount = 2;
  }
  /**/lpt->tLog(fname,7);
  // looking for the chapter title
  if (!xparser.MovePast("<a id=\"chapter_title\"")) return false;
  /**/lpt->tLog(fname,8);
  if (!xparser.GetDelimited(">","</a>",buffer)) return false;
  temp_part.part_name = buffer.trimmed();
  // next, the contents
  if (!xparser.MovePast("<div id=\"chapter-body\"")) return false;
  /**/lpt->tLog(fname,10);
  const QString startm = "<div class=\"bbcode\">";
  const QString endm1 = "<div class=\"chapter_footer\">";
  const QString endm2 = "<ins ";
  if (!xparser.GetDelimitedEndPair(startm,endm1,endm2,buffer)) return false;
  /**/lpt->tLog(fname,11);
  // processing after to set the part contents
  fpart = new jfFicPart(temp_part);
  fpart->part_contents = PartProcessing(buffer);
  /**/lpt->tLog(fname,12);
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// ParsePtherPage() helper methods
//----------------------------
bool jfFIM_FicPartParser::HandleChapters(QString inbuffer, jfFicPart& fpoint){
  // constants
  const QString fname = "jfFIM_FicPartParser::HandleChapters";
  // variables
  jfStringParser* qparser;
  unsigned long qval;
  QString buffer, oxerr;
  bool fail = false;
  bool cselected;
  size_t partindex;
  // we start

  qparser = new jfStringParser(inbuffer);
  // the main parsing loop
  while (qparser->MovePast("<li")) {
    // getting everything up to the >
    if (!qparser->GetMovePast(">",buffer)) fail = true;
    else {
      buffer = buffer.trimmed();
      cselected = (buffer == "class=\"selected\"");
      // extracting the chapter index
      if (!qparser->MovePast("<a href=\"/story/")) fail = true;
      else if (!qparser->GetDelimitedULong("/","/",qval,oxerr)) fail = true;
      else if (cselected) partindex = qval;
    }
  }
  // done with the loop
  delete qparser;
  if (fail) return false;
  if (qval==0) return false;
  // setting the values we just got
  fpoint.partindex = partindex;
  fpoint.partcount = partcount = qval;
  pagecount = partcount +1;
  return true;
}
//------------------------------
QString jfFIM_FicPartParser::PartProcessing(QString inbuffer){
  // constants
  const QString fname = "jfFicPart_FIM::PartProcessing";
  const QString wdiv1 = "<br /><br /><div style='float:left; height:1.0em; width:3.0em;'></div>";
  const QString wdiv2 = "<br /><div style='float:left; height:1.0em; width:3.0em;'></div>";
  const QString wdiv3 = "<div style='float:left; height:1.0em; width:3.0em;'>";
  const QString ctag = "<br /><center>o.0.o</center></p>";
  const QString ctag2 = "<p><center>o.0.o</center></p>";
  const QString bbcent = "<div class=\"bbcode-center\" style=\"text-align:center\">";
  const QString newcent = "<div style=\"text-align:center\">";
  // variables
  QString obuf;
  int lpos;
  // we backtrack to just after the last </div>
  lpos = inbuffer.lastIndexOf("</div>",-1,Qt::CaseInsensitive);
  if (lpos != -1) inbuffer.truncate(lpos+6);
  inbuffer = inbuffer.trimmed();
  // we have to remove floating divs (they break certain browsers)
  inbuffer.replace(wdiv1,"</p>\n<p>");
  inbuffer.replace(wdiv2,"</p>\n<p>");
  inbuffer.replace(wdiv3,"</p>\n<p>");
  // bbcode class names are not wanted
  inbuffer.replace(bbcent,newcent);
  // also, why use breaks when we can use paragraphs
  inbuffer.replace("<br /><br />","</p>\n<p>");
  // centre tags should not be embedded in paragraphs!
  inbuffer.replace(ctag,"</p>\n<center>o.0.o</center>");
  inbuffer.replace(ctag2,"<center>o.0.o</center>");
  // removing more empty things
  inbuffer.replace("<p></div></p>\n","\n");
  inbuffer.replace("<p></p>\n","\n");
  obuf = inbuffer;
  while (true) {
    inbuffer.replace("\n\n\n","\n");
    if (inbuffer==obuf) break;
    obuf = inbuffer;
  }
  inbuffer.replace("<p></div>","<p>");
  // more processing
  inbuffer.replace("</p><p class=\"indented double\">","</p>\n<p class=\"indented double\">");
  obuf = inbuffer;
  while (true) {
    inbuffer.replace("&nbsp; </p>"," </p>");\
    if (inbuffer==obuf) break;
    obuf = inbuffer;
  }
  // done
  if (!inbuffer.startsWith("<p")) inbuffer = "<p>" + inbuffer;
  return inbuffer + "</p>";
}

/*************************************************************/
