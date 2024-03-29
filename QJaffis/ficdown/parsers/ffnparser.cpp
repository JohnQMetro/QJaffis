/***************************************************************
 * Name:      ffnparser.cpp
 * Purpose:   Fic parser: Fanfiction.net
 * Author:    John Q Metro
 * Created:   July 4, 2016
 * Updated:   November 4, 2022
 *
 **************************************************************/
#ifndef FFNPARSER_H
  #include "ffnparser.h"
#endif // FFNPARSER_H
//--------------------------
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//-------------------------
#include <assert.h>

/**************************************************************/
// +++ [ METHODS for jfFFN_FicPartParser ] +++
//+++++++++++++++++++++++++++++++++++++++++++++++
jfFFN_FicPartParser::jfFFN_FicPartParser():jfStoryPartParseBase(){

    titlehead = QRegularExpression("id=['\"]?pre_story_links['\"]?",QRegularExpression::CaseInsensitiveOption);
    arrow_switch_icon = QRegularExpression("<img\\s+src=['\"]?(//ff7\\d\\.b-cdn\\.net)?/static/fcons/arrow-switch.png['\"]?",QRegularExpression::CaseInsensitiveOption);
    chevron_icon = QRegularExpression("<span\\s+class=['\"]?xcontrast_txt icon-chevron-right xicon-section-arrow['\"]?>",QRegularExpression::CaseInsensitiveOption);

}
//+++++++++++++++++++++++++++++++++++++++++++++++
// virual methods that are implemented
//--------------------------------------
QString* jfFFN_FicPartParser::makeRedirectedURL(const QString& inPart) const {
  return NULL;
}
//--------------------------------------
bool jfFFN_FicPartParser::testMissing(const QString *page) const {
  // constants and variables
  const QString mstring_pfx = "Story Not Found";
  const QString mstring1 = "Story Not Found<hr size=1 noshade>Unable to locate story. Code 1.";
  const QString mstring2 = "Story Not Found<hr size=\"1\" noshade=\"\">Unable to locate story. Code 1.";
  const QString mstring3 = "Story Not Found<hr size=\"1\" noshade=\"\">Story is unavailable for reading.";
  const QString mstring4 = "Story Not Found<hr size=1 noshade>Story is unavailable for reading.";
  // checking
  assert(page!=NULL);
  bool rval = page->contains(mstring_pfx);
  if (!rval) {
      return true;
  }
  if (page->contains(mstring1)) {
      return false;
  }
  if (page->contains(mstring2)){
      return false;
  }
  if (page->contains(mstring3)) {
      return false;
  }
  if (page->contains(mstring4)) {
      return false;
  }
  return true;
}
//--------------------------------------
bool jfFFN_FicPartParser::testIncomplete(const QString *page) const {
  // checking
  assert(page!=NULL);
  const QString footer1 = "<div id=p_footer class=maxwidth";
  const QString footer2 = "<div id=\"p_footer\" class=\"maxwidth\"";
  if (page->contains(footer1)) return true;
  else return page->contains(footer2);
}
//------------------------------------
bool jfFFN_FicPartParser::testRatelimit(const QString *page) const {
    return true;
}
//--------------------------------------
QString jfFFN_FicPartParser::getCookie() const{
  return "";
}
//+++++++++++++++++++++++++++++++++++++++++++++++
// custom virtual methods that are implemented
//--------------------------------------
bool jfFFN_FicPartParser::ParseFirstPage(const QString& indata) {
  const QString fname = "jfFFN_FicPartParser::ParseFirstPage";
  // variables
  jfFicExtract_FFN* result = NULL;
  QString buffer, errout;
  unsigned long qval;
  // we start
  getpartnames = true;
  /**/lpt->tLog(fname,1);
  if (!FirstProcessStart(buffer)) return false;
  // going ahead
  result = new jfFicExtract_FFN();
  result->fic_title = buffer;
  /**/lpt->tLog(fname,2,buffer);
  // looking for the author id
  const QString aidpfx1 = "<a class=\"xcontrast_txt\" href=\"/u/";
  const QString aidpfx2 = "<a class='xcontrast_txt' href='/u/";

  if (!xparser.MovePastAlt(aidpfx1,aidpfx2)) {
    delete result;
      /**/lpt->tLog(fname,3);
      return parsErr("Cannot find start of author id!");
  }
  if (!xparser.GetMovePastULong("/",qval,errout)) {
      delete result;
      /**/lpt->tLog(fname,4);
    return parsErr("Problems getting author id! " + errout);
  }

  result->auth_id = qval;
  // getting the author name
  if (!xparser.GetDelimited(">","</a>",buffer)) {
    delete result;
      /**/lpt->tParseError(fname,"Problems getting author name!");
    return parsErr("Problems getting author name!");
  }
  result->author_name = buffer.trimmed();
  if (!DateAndCompletion(result)) {
    delete result;
    /**/lpt->tLog(fname,6);
    return false;
  }
  // we go ahead with the normal things one would expect from set from string
  if (!ParseOtherPage()) {
    delete result;
    /**/lpt->tLog(fname,7,parseErrorMessage);
    return false;
  }
  result->fic_id = story_id;
  /**/lpt->tLogS(fname,8,partcount);
  result->pcount = partcount;
  result->partnames = extracted_partnames;
  extract = result;
  getpartnames = false;
  extracted_partnames.clear();
  return true;
}

//--------------------------------------
bool jfFFN_FicPartParser::ParseOtherPage() {
  // constants
  const QString fname = "jfFFN_FicPartParser::ParseOtherPage";
  const QString ficstart_tag1 = "<div role=\"main\" aria-label=\"story content\" class=\"storytextp\" id=\"storytextp\"";
  const QString ficstart_tag2 = "<div role='main' aria-label='story content' class='storytextp' id='storytextp'";
  const QString patscript_tag1 = "<form name=\"myselect\"";
  const QString patscript_tag2 = "<FORM name=myselect";

  // variables
  jfFicPart intfic;
  QString partcontents;
  QString errout;
  ulong oval;
  // starting
  /**/lpt->tLog(fname,1);
  if (!SectionProcessing(intfic)) return false;
  // doing things
  if (!xparser.MovePastAlt(ficstart_tag1, ficstart_tag2)) {
    return parsErr("Missing Start Tag");
  }
  /**/lpt->tLog(fname,2);

  /* extracting parts. I'll try the old way first, and if that fails, switch
   * to the more elaborate new way. */
  if (!xparser.GetDelimited(" id='storytext'>","</div>",partcontents)) {
      /**/lpt->tLog(fname,3);
      if (!ExtractPartContents(partcontents)) {
    return parsErr("Cannot find contents");
  }
  }
  /**/lpt->tLog(fname,4);
  // just to check...
  if (!xparser.MovePastAlt(patscript_tag1,patscript_tag2)) {
    return parsErr("Cannot find separator after style and social script parts");
  }
  /**/lpt->tLog(fname,4);
  // next, we look for more info...
  if (!xparser.MovePast("<script>")) {
    return parsErr("Cannot find fic info script start");
  }
  // looking for the fic id
  if (!xparser.GetDelimitedULong("var storyid = ",";",oval,errout)) {
    /**/lpt->tLog(fname,5);
    return parsErr("Problems getting fic id! " + errout);
  }
  story_id = oval;
  /**/lpt->tLogS(fname,6,story_id);
  if (!xparser.GetDelimitedULong("var chapter =",";",oval,errout)) {
      /**/lpt->tParseError(fname,"Problems with chaper index : " + errout);
    return parsErr("Problems with chaper index : " + errout);
  }

  // setting up result info
  intfic.partindex = oval;
  /**/lpt->tLogS(fname,7,intfic.partindex);
  fpart = new jfFicPart(intfic);
  fpart->part_contents = PartProcessing(partcontents);
  /**/lpt->tLog(fname,8);
  return true;
}
//---------------------------------------------------------------------------
/* A new extract contents. The pages are formatted differently when fetched
 * via a web-browser (using Selenium and Python). */
bool jfFFN_FicPartParser::ExtractPartContents(QString& partcontents) {
    const QString fname = "jfFFN_FicPartParser::ExtractPartContents";
    QString result = "";
    QString buffer = "";
    int scount = 0;
    result.reserve(60000);
    buffer.reserve(30000);
    /**/lpt->tLog(fname,1);
    // moving part the intro part
    if (!xparser.MovePastTwo("<div class=\"storytext",">")) {
        /**/lpt->tLog(fname,2,xparser.GetBlock(3000));
        return parsErr("Cannot find start of contents!");
    }
    scount = 0;
    /**/lpt->tLog(fname,3);
    /* trying to skip over the Google advertising blocks inserted by
     * Javascript. */
    while(xparser.GetMovePast("<div ", buffer)) {
        /**/lpt->tLog(fname,4);
        result += buffer;
        scount += 1;
        if (!xparser.MovePast("class=\"google-auto-placed ap_container\"><ins")) break;
        if (!xparser.MovePast("</ins></div>")) {
            /**/lpt->tLog(fname,5,xparser.GetBlock(3000));
            return parsErr("Cannot find end of google advert block!");
        }
    }
    /**/lpt->tLogI(fname,6,scount);
    // trimming /div from the end
    if (result.endsWith("</div>")) {
        result.chop(6);
        result = result.trimmed();
        if (result.endsWith("</div>")) {
            result.chop(6);
            result = result.trimmed();
        }
    }
    // done
    partcontents = result;
    return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//helper methods
//--------------------------------------
bool jfFFN_FicPartParser::FirstProcessStart(QString& out_title) {
  // constants
  const QString fname = "jfFicPart::FirstProcessStart";
  // variables
  QString buffer;
  bool crossover;
  // we start
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(titlehead)) {
    return parsErr("Missing Special DIV tag!");
  }
  crossover = xparser.MovePast(arrow_switch_icon);
  if (!crossover) {
    if (!xparser.MovePast(chevron_icon)) {
        /**/lpt->tLog(fname,2,xparser.GetBlock(3000));
      return parsErr("Missing Icon or Chevron!");
    }
  }
  /**/lpt->tLogB(fname,3,crossover);
  // extracting the title
  if (!xparser.MovePastAlt("<b class=\"xcontrast_txt\">","<b class='xcontrast_txt'>")) {
    /**/lpt->tLog(fname,4);
    return parsErr("Cannot find the title! A");
  }
  if (!xparser.GetMovePast("</b>",buffer)) {
    /**/lpt->tLog(fname,5);
    return parsErr("Cannot find the title! B");
  }
  buffer = buffer.trimmed();
  /**/lpt->tLog(fname,5,buffer);
  if (buffer.isEmpty()) {
      /**/lpt->tParseError(fname,"Title is empty");
    return parsErr("Title is Empty!");
  }
  // returning
  out_title = buffer;
  return true;
}

//--------------------------------------
bool jfFFN_FicPartParser::DateAndCompletion(jfFicExtract_FFN* in_result) {
  // constants
  const QString fname = "jfFicPart_FFN::DateAndCompletion";
  // variables
  QDateTime temptime;
  // asserts and checks
  assert(in_result!=NULL);
  // variables
  unsigned long xoval;
  QString buffer1,xoerr;
  // looking for the updated date (or published date)
  if (!xparser.GetDelimitedULong("Updated: <span data-xutime=\"","\">",xoval,xoerr)) {
  if (!xparser.GetDelimitedULong("Updated: <span data-xutime='","'>",xoval,xoerr)) {
          if (!xparser.GetDelimitedULong("Published: <span data-xutime=\"","\">",xoval,xoerr)) {
    if (!xparser.GetDelimitedULong("Published: <span data-xutime='","'>",xoval,xoerr)) {
      /**/lpt->tLog(fname,1);
      return parsErr("Problems getting date! " + xoerr);
    }
  }
      }
  }
  // the time in xoval is a unix time stamp
  temptime = QDateTime::fromTime_t((uint)(xoval));
  in_result->updated_date = temptime.date();

  // getting the place where we look for completion status
  if (!xparser.GetDelimited("</span>"," - id:",buffer1)) {
    /**/lpt->tLog(fname,2);
    return parsErr("Cannot find the section after the time!");
  }
  // looking for completion
  in_result->complete = buffer1.contains("Status: Complete");
  // done
  return true;
}
//--------------------------------------
bool jfFFN_FicPartParser::SectionProcessing(jfFicPart& fpoint) {
  // constants and variables
  const QString chapt_start1 = "<SELECT id=chap_select title=\"Chapter Navigation\"";
  const QString chapt_start2 = "<select id=\"chap_select\" title=\"Chapter Navigation\"";

  jfStringParser* opt_parser;
  QString extopt, partname, partname_x;
  size_t part_counter = 0;
  size_t selcount = 0;
  bool exopcs = false;

  // checking first...
  if (!xparser.MovePastAlt(chapt_start2, chapt_start1)) {
    // only one part
    pagecount = partcount = fpoint.partcount = 1;
    fpoint.partindex = 1;
    return true;
  }
  // here, we have multiple parts
  if (!xparser.GetDelimited(">","/select>",extopt)) {
    return parsErr("Could not extract Chapter Navigation!");
  }
  opt_parser = new jfStringParser(extopt);
  // looping for each chapter
  while (opt_parser->MovePastTwo("option","value=")) {
    part_counter++;
    // looking for the selected item
    if (!opt_parser->GetMovePast(">",extopt)) {
      delete opt_parser;
      return parsErr("End of Option (chapter selector) not found!");
    }
    exopcs = extopt.contains("selected");
    if (exopcs || getpartnames) {
      if (!opt_parser->GetDelimited(".","<",partname_x)) {
        delete opt_parser;
        return parsErr("Failed at getting chapter label");
      }
      partname_x = htmlparse::ConvertEntities(partname_x.trimmed(),false);
      if (getpartnames) extracted_partnames.append(partname_x);
      if (exopcs) {
        partname = partname_x;
        selcount++;
      }
    }
  }
  // post loop checking
  delete opt_parser;
  if (part_counter==0) return parsErr("Chapter selector ugly.");
  if (selcount!=1) return parsErr("Selected Chapter wrong.");
  // no errors
  pagecount = partcount = fpoint.partcount = part_counter;
  fpoint.part_name = partname.trimmed();
  return true;
}

//--------------------------------------
QString jfFFN_FicPartParser::PartProcessing(QString inbuffer) {
  // just handling a small problem here...
  inbuffer.replace("</I><I>"," ",Qt::CaseInsensitive);
  inbuffer.replace("</em><em>"," ",Qt::CaseInsensitive);
  // just in case
  inbuffer.replace("</b><b>"," ",Qt::CaseInsensitive);
  return inbuffer;
}


/**************************************************************/
