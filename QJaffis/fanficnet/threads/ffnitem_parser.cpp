/******************************************************************************
Name    :   ffnitem_parser.cpp
Author  :   John Q Metro
Purpose :   Parser for Fanfiction.Net Category pages, to get lists of fanfics
Created :   July 22, 2016
Updated :   July 25, 2016
******************************************************************************/
#ifndef FFNITEM_PARSER_H
  #include "ffnitem_parser.h"
#endif // FFNITEM_PARSER_H
//-----------------------------------

#include <assert.h>
/*****************************************************************************/
// --- [ METHODS for jfFFNItemParser ] ---------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfFFNItemParser::jfFFNItemParser():jfItemsPageParserBase() {
  this_category = NULL;
  newfic = false;
  limit = NULL;
  limitreached = false;
}
//------------------------------------------------------
bool jfFFNItemParser::SetCategory(const jfFFN_CategoryCore* in_category){
  if (in_category==NULL) return false;
  this_category = in_category;
  return true;
}
//-----------------------------------------
bool jfFFNItemParser::setLimit(jfFicLimit* in_limit) {
  if (in_limit == NULL) return false;
  limit = in_limit;
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse methods
//----------------------------------
void jfFFNItemParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  const QString fname = "jfFFNItemParser::ParseDownloadedPage";
  // variables
  size_t newcount;
  jfFFNItem* temp;
  QString buffer,outerr;
  // starting...
  /**/lpt->tLog(fname,1);
  NewData(inPage);
  limitreached = false;
  // we check names first
  if (!CheckNames()) {
    /**/lpt->tLog(fname,2);
    parseErrorMessage = "Could not get names!";
    return;
  }
  /**/lpt->tLog(fname,3);
  // next, we check the item count and thus get the page count
  if (!PageAndItemCount(newcount)) {
    size_t pindex = xparser.GetIndex();
    /**/lpt->tLogS(fname,4,pindex);
    return;
  }
  /**/lpt->tLog(fname,5);
  // initializing the list of results
  page_results = new jfPDVector();

  /**/lpt->tLog(fname,6);
  // next up is parsing the items
  while (xparser.GetDelimited("<div class=\'z-list ","</div></div></div>",buffer)) {
    temp = new jfFFNItem();
    temp->SetFromString(buffer,this_category,outerr);
    /**/lpt->tLog(fname,7);
    // the item is okay
    if (temp->IsValid()) {
      // checking the date limit
      if (ExitCheck(temp)) {
        limitreached = true;
        break;
      }
      // adding
      page_results->push_back(temp);
      if (newfic) temp->SetNew(true);
    }
    // the item is not okay
    else {
      parseErrorMessage = "Parsing Error : " + outerr + " : IN\n" + buffer;
      /**/lpt->tLog(fname,7,parseErrorMessage);
      if (!(temp->included)) delete temp;
      else {
        delete temp;
        delete page_results;
        page_results =NULL;
        return;
      }
    }
  }
  /**/lpt->tLog(fname,8);
  // done with the items
  if (!(page_results->empty())) {
    firstid = (*page_results)[0]->GetID();
    // post page processing and getting the result
    PostPageProcessing();
  }
  else {
      firstid = 0;
  }
  page_parsed = true;
  /**/lpt->tLog(fname,9);
}

//----------------------------------
void* jfFFNItemParser::getResults() {
  jfResultUnitVector* results = MakeResults();
  results->firstid = firstid;
  results->limitreached = limitreached;
  return MakeResults();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods for download
//----------------------------------
QString* jfFFNItemParser::makeRedirectedURL(const QString& inPart) {
  // fanfiction.net does no redirects
  return NULL;
}

//----------------------------------
bool jfFFNItemParser::testMissing(const QString *page) const{
    assert(page != NULL);
    const QString errmsg = "FanFiction.Net Error Type 1<hr size=1 noshade>An error has occurred while processing your request.";
    if (page->contains(errmsg)) return false;
    return true;
}
//----------------------------------
bool jfFFNItemParser::testIncomplete(const QString *page) const {
  assert(page!=NULL);
  const QString footer1= "<a href='/tos/'>Terms of Service</a>";
  const QString footer2 = "<!-- BEGIN footer -->";
  return page->contains(footer1);
}
//----------------------------------
QString jfFFNItemParser::getCookie() const {
  return "";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods for parsing
//-----------------------------------------------------------------------------
bool jfFFNItemParser::CheckNames() {
  // constants
  const QString fname = "jfFFNItemParser::CheckNames";
  const QString header1 = "<div style='width:100%;' class=xcontrast_outer id=content_parent>";
  const QString itmico1 = "/static/ficons/";
  const QString itmico2 = "/static/fcons/";

  const QString chev_sym = " icon-chevron-right xicon-section-arrow'></span>";
  // variables
  QString buffer1,buffer2;
  // checks and asserts
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(header1)) return false;
  if (!xparser.MovePastAlt(itmico1,itmico2)) return false;
  // the crossover version
  /**/lpt->tLog(fname,2);
  if ((this_category->GetCatType())==jfx_CROSS) {
    if (!CheckCrossoverName()) {
      /**/lpt->tLog(fname,3);
      return false;
    }
  }
  // the non crossover version
  else {
    /**/lpt->tLog(fname,4);
    if (!xparser.GetDelimited(chev_sym,"<div ",buffer1)) return false;
    buffer1 = buffer1.trimmed();
    buffer2 = this_category->GetName();
    buffer2 = buffer2.trimmed();
    /**/lpt->tLog(fname,5);
    if (buffer1!=buffer2) return false;
  }
  return true;
}
//-----------------------------------------------------
// checking if a crossover name is correct has complications, so I separated it from the resi
bool jfFFNItemParser::CheckCrossoverName() {
  // local variables
  jfFFN_CategoryCore* zccat;
  const jfFFN_CrossoverCategory* ccat;
  QString buffer1,buffer2;
  QString name1, name2;
  bool option1, option2;
  // getting the names
  zccat = const_cast<jfFFN_CategoryCore*>(this_category);
  ccat = dynamic_cast<jfFFN_CrossoverCategory*>(zccat);
  name1 = ccat->GetCatPart(true);
  name2 = ccat->GetCatPart(false);
  // extracting the names from the parser
  if(!xparser.MovePast("<a href=\'/crossovers/")) return false;
  if(!xparser.GetDelimited("/\'>","</a>",buffer1)) return false;
  if(!xparser.MovePast("<a href=\'/crossovers/")) return false;
  if(!xparser.GetDelimited("/\'>","</a>",buffer2)) return false;
  // checking the match
  option1 = (name1==buffer1) && (name2==buffer2);
  option2 = (name1==buffer2) && (name2==buffer1);
  // the result
  return (option1 || option2);
}
//----------------------------------------------------------------
bool jfFFNItemParser::PageAndItemCount(size_t& itemcount) {
  // constants
  const QString fname = "jfFFNItemCollectionCore::PageAndItemCount";
  // local variables
  QString cname;
  QString buffer;
  unsigned long outval;
  QString outerr;
  float tpagecount;
  bool isk;
  size_t llen;
  // starting up things
  /**/lpt->tLog(fname,1);
  // next, numeric data
  if (xparser.MovePast("<center style='margin-top:5px;margin-bottom:5px;'>")) {
    /**/lpt->tLog(fname,2);
    if (!xparser.GetMovePast(" | ",buffer)) {
      return parsErr("Could not get itemcount! \n" + xparser.GetBlock(200));
    }
    buffer = buffer.trimmed();
    /**/lpt->tLog(fname,3,buffer);
    llen = buffer.length();
    isk = (buffer[llen-1]=='K');
    if (!Count2ULong(buffer,outval)) {
      return parsErr("Could not convert itemcount! \n" + xparser.GetBlock(200));
    }
    itemcount = outval;
    /**/lpt->tLogS(fname,4,itemcount);
    // the latest version of fanfiction.net makes getting the pagecount a chore ...
    if (isk) {
      if (!xparser.GetDelimitedFromEndULong("&p=","'>Last",outval,outerr,"</center>")) {
        if (!xparser.GetDelimitedFromEndULong("&p=","'>Next",outval,outerr,"</center>")) {
          if (!xparser.GetDelimitedFromEndULong("<b>","</b>",outval,outerr,"</center>")) {
            return parsErr("Could not determine pagecount! \n" + xparser.GetBlock(400));
          }
        }
      }
      /**/lpt->tLogS(fname,6,outval);
    }
    else {
      /**/lpt->tLogI(fname,7,outval);
      // we've got the item count
      tpagecount = float(itemcount) / float(25);
      pagecount = ceil(tpagecount);
    }
  }
  // annoyingly, if there are 25 or less items in the category, no itemcount info is displayed
  else {
    /**/lpt->tLog(fname,8);
    /* there is a chance that the page is actually defective, so we store it for examination
    if the fetch fails */
    if (pagecount>2) {
      return parsErr("No itemcount header on subsequent page!");
    }
    pagecount=1;
    // since I won't go the trouble of counting them before processsng the page...
    itemcount = this_category->GetEstStoryCount(); // the count can be inaccurate!
  }
  /**/lpt->tLog(fname,4);
  // done
  return true;
}
//---------------------------------------------
bool jfFFNItemParser::ExitCheck(jfFFNItem* tocheck) const {
  if (limit==NULL) return false;
  assert(tocheck!=NULL);
  return !(limit->IsLater(tocheck));
}
/*****************************************************************************/
