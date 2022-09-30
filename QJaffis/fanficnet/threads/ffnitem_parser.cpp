/******************************************************************************
Name    :   ffnitem_parser.cpp
Author  :   John Q Metro
Purpose :   Parser for Fanfiction.Net Category pages, to get lists of fanfics
Created :   July 22, 2016
Updated :   Sreptember 28, 2022
******************************************************************************/
#ifndef FFNITEM_PARSER_H
  #include "ffnitem_parser.h"
#endif // FFNITEM_PARSER_H
//-----------------------------------
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED

#include <assert.h>
#include <math.h>
/*****************************************************************************/
// --- [ METHODS for jfFFNItemParser ] ---------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfFFNItemParser::jfFFNItemParser():jfItemsPageParserBase() {
  this_category = NULL;
  newfic = false;
  limit = NULL;
  limitreached = false;

  page_header = QRegularExpression("\\s+class=['\"]?xcontrast_outer['\"]?\\s+id=['\"]?content_parent['\"]?>");
  cross_link = QRegularExpression("<a\\s+href=['\"]/crossovers/");
  pagehead = QRegularExpression("<center\\s+style=['\"]?margin-top:5px;margin-bottom:5px;?['\"]?>",QRegularExpression::CaseInsensitiveOption);
  zlist_start = QRegularExpression("<div\\s+class=['\"]?z-list\\s",QRegularExpression::CaseInsensitiveOption);
  item_ico = QRegularExpression("/static/fi?cons/");
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
  // skipping past header gunk
  if (!xparser.MovePastAlt("</form>","</FORM>")) {
      /**/lpt->tLog(fname,2,xparser.GetBlock(2000));
      parseErrorMessage = "Could not find end of form";
      return;
  }
  // we check names first
  if (!CheckNames()) {
      /**/lpt->tParseError(fname,"Could not get names!");
    parseErrorMessage = "Could not get names!";
    return;
  }
  /**/lpt->tLog(fname,3);
  // next, we check the item count and thus get the page count
  if (!PageAndItemCount(newcount)) {
    size_t pindex = xparser.GetIndex();
    /**/lpt->tParseError(fname,"Failed to get page and item count: " + QString::number(pindex));
    return;
  }
  /**/lpt->tLog(fname,5);
  // initializing the list of results
  page_results = new jfPDVector();

  /**/lpt->tLog(fname,6);
  // next up is parsing the items
  //

  while (xparser.MovePast(zlist_start)) {
      if (!xparser.GetMovePast("</div></div></div>", buffer)) {
          parseErrorMessage = "Failed to find end of fic info";
          lpt->tLog(fname,7,parseErrorMessage);
          delete page_results;
          page_results =NULL;
          return;
      }
    temp = new jfFFNItem();
    temp->SetFromString(buffer,this_category,outerr);
    /**/lpt->tLog(fname,8);
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
      /**/lpt->tParseError(fname,parseErrorMessage);
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
      /**/lpt->tLog(fname,9);
    firstid = (*page_results)[0]->GetID();
    // post page processing and getting the result
    /**/lpt->tLog(fname,10);
    PostPageProcessing();
    /**/lpt->tLog(fname,11);
  }
  else {
      firstid = 0;
  }
  page_parsed = true;
  /**/lpt->tLog(fname,12);
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
QString* jfFFNItemParser::makeRedirectedURL(const QString& inPart) const {
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
    const QString fname = "jfFFNItemParser::testIncomplete";
  assert(page!=NULL);
  const QString footer1 = "<div id=p_footer class=maxwidth";
  const QString footer2 = "<div id=\"p_footer\" class=\"maxwidth\"";
  if (page->contains(footer1)) return true;
  else return page->contains(footer2);
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

  const QString chev_sym1 = " icon-chevron-right xicon-section-arrow\"></span>";
  // variables
  QString buffer1,buffer2;
  // checks and asserts
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(page_header)) {
      /**/lpt->tLog(fname,2,xparser.GetBlock(2000));
      return false;
  }
  if (!xparser.MovePast(item_ico)) {
      /**/lpt->tLog(fname,3,xparser.GetBlock(2000));
      return false;
  }
  // the crossover version
  /**/lpt->tLog(fname,4);
  if ((this_category->GetCatType())==jfx_CROSS) {
    if (!CheckCrossoverName()) {
        /**/lpt->tParseError(fname,"Crossover Name Check Failed!");
      return false;
    }
  }
  // the non crossover version
  else {
    /**/lpt->tLog(fname,5);
    if (!xparser.GetDelimited(chev_sym1,"<div ",buffer1)) return false;
    buffer1 = buffer1.trimmed();
    buffer2 = this_category->GetName();
    buffer2 = buffer2.trimmed();
    /**/lpt->tLog(fname,6);
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
  if(!xparser.MovePast(cross_link)) return false;
  if(!xparser.GetDelimited(">","</a>",buffer1)) return false;
  buffer1 = htmlparse::HTML2Text(buffer1).replace("shoppingmode","");;
  if(!xparser.MovePast(cross_link)) return false;
  if(!xparser.GetDelimited(">","</a>",buffer2)) return false;
  buffer2 = htmlparse::HTML2Text(buffer2).replace("shoppingmode","");;
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
  uint llen;
  // starting up things
  /**/lpt->tLog(fname,1);
  // next, numeric data
  if (xparser.MovePast(pagehead)) {
    /**/lpt->tLog(fname,2);
    if (!xparser.GetMovePast(" | ",buffer)) {
        /**/lpt->tParseError(fname,"Could not get itemcount! \n" + xparser.GetBlock(200));
      return parsErr("Could not get itemcount! \n" + xparser.GetBlock(200));
    }
    buffer = buffer.trimmed();
    /**/lpt->tLog(fname,3,buffer);
    llen = buffer.length();
    isk = (buffer[llen-1]=='K');
    if (!Count2ULong(buffer,outval)) {
        /**/lpt->tParseError(fname,"Could not convert itemcount! \n" + xparser.GetBlock(200));
      return parsErr("Could not convert itemcount! \n" + xparser.GetBlock(200));
    }
    itemcount = outval;
    /**/lpt->tLogS(fname,4,itemcount);
    // getting pagecount is a chore
    if (isk) {
        // last works for large categories unless we are near the end
        if (!xparser.GetDelimitedFromEndULong("&amp;p=","\">Last",outval,outerr,"</center>")) {
            if (!xparser.GetDelimitedFromEndULong("&p=","'>Last",outval,outerr,"</center>")) {
                // next works near the end
                if (!xparser.GetDelimitedFromEndULong("&amp;p=","\">Next",outval,outerr,"</center>")) {
        if (!xparser.GetDelimitedFromEndULong("&p=","'>Next",outval,outerr,"</center>")) {
                        // for the very last page
          if (!xparser.GetDelimitedFromEndULong("<b>","</b>",outval,outerr,"</center>")) {
            return parsErr("Could not determine pagecount! \n" + xparser.GetBlock(400));
                        }
                    }
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
