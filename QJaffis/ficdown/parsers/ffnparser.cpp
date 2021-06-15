/***************************************************************
 * Name:      ffnparser.cpp
 * Purpose:   Fic parser: Fanfiction.net
 * Author:    John Q Metro
 * Created:   July 4, 2016
 * Updated:   January 2, 2021
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
jfFFN_FicPartParser::jfFFN_FicPartParser():jfStoryPartParseBase(){}
//+++++++++++++++++++++++++++++++++++++++++++++++
// virual methods that are implemented
//--------------------------------------
QString* jfFFN_FicPartParser::makeRedirectedURL(const QString& inPart) {
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
  if (!rval) return true;
  if (page->contains(mstring1)) return false;
  if (page->contains(mstring2)) return false;
  if (page->contains(mstring3)) return false;
  if (page->contains(mstring4)) return false;
  return true;
}
//--------------------------------------
bool jfFFN_FicPartParser::testIncomplete(const QString *page) const {
  // constants and variables
  // checking
  assert(page!=NULL);
  const QString footer1 = "<div id=p_footer class=maxwidth";
  const QString footer2 = "<div id=\"p_footer\" class=\"maxwidth\"";
  if (page->contains(footer1)) return true;
  else return page->contains(footer2);
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
  if (!xparser.GetDelimitedULong("<a class='xcontrast_txt' href='/u/","/",qval,errout)) {
    delete result;
    /**/lpt->tLog(fname,3);
    return parsErr("Problems getting author id! " + errout);
  }
  result->auth_id = qval;
  // getting the author name
  if (!xparser.GetDelimited("'>","</a>",buffer)) {
    delete result;
    /**/lpt->tLog(fname,4);
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
  const QString ficstart_tag = "<div role='main' aria-label='story content' class='storytextp' id='storytextp'";
  const QString patscript_tag = "<FORM name=myselect";

  // variables
  jfFicPart intfic;
  QString partcontents;
  QString errout;
  ulong oval;
  // starting
  /**/lpt->tLog(fname,1);
  if (!SectionProcessing(intfic)) return false;
  // doing things
  if (!xparser.MovePast(ficstart_tag)) {
    return parsErr("Missing Start Tag");
  }
  /**/lpt->tLog(fname,2);
  // we here make the possibly dubious assumption that there are no <div> tags in the actual story
  if (!xparser.GetDelimited(" id='storytext'>","</div>",partcontents)) {
    return parsErr("Cannot find contents");
  }
  /**/lpt->tLog(fname,3);
  // just to check...
  if (!xparser.MovePast(patscript_tag)) {
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

//+++++++++++++++++++++++++++++++++++++++++++++++
//helper methods
//--------------------------------------
bool jfFFN_FicPartParser::FirstProcessStart(QString& out_title) {
  // constants
  const QString fname = "jfFicPart::FirstProcessStart";
  const QString titlehead_tag = " id=pre_story_links>";
  const QString icon_tag1 = "<img src='//ff74.b-cdn.net/static/fcons/arrow-switch.png'";
  const QString icon_tag2 = "<img src='/static/fcons/arrow-switch.png'";
  const QString chev_tag = "<span class='xcontrast_txt icon-chevron-right xicon-section-arrow'>";
  // variables
  QString buffer;
  bool crossover;
  // we start
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(titlehead_tag)) {
    return parsErr("Missing Special DIV tag!");
  }
  crossover = xparser.MovePastAlt(icon_tag1,icon_tag2);
  if (!crossover) {
    if (!xparser.MovePast(chev_tag)) {
      /**/lpt->tLog(fname,2);
      return parsErr("Missing Icon or Chevron!");
    }
  }
  /**/lpt->tLogB(fname,3,crossover);
  // extracting the title
  if (!xparser.GetDelimited("<b class='xcontrast_txt'>","</b>",buffer)) {
    /**/lpt->tLog(fname,4);
    return parsErr("Cannot find the title!");
  }
  buffer = buffer.trimmed();
  /**/lpt->tLog(fname,5,buffer);
  if (buffer.isEmpty()) {
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
  if (!xparser.GetDelimitedULong("Updated: <span data-xutime='","'>",xoval,xoerr)) {
    if (!xparser.GetDelimitedULong("Published: <span data-xutime='","'>",xoval,xoerr)) {
      /**/lpt->tLog(fname,1);
      return parsErr("Problems getting date! " + xoerr);
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
  const QString chapt_start = "<SELECT id=chap_select title=\"Chapter Navigation\"";
  jfStringParser* opt_parser;
  QString extopt, partname, partname_x;
  size_t part_counter = 0;
  size_t selcount = 0;
  bool exopcs = false;

  // checking first...
  if (!xparser.MovePast(chapt_start)) {
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
