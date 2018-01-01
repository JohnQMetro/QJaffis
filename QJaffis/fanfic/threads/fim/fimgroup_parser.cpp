/******************************************************************************
Name    :   fimgroup_parser.cpp
Author  :   John Q Metro
Purpose :   Group object for fimfiction.net
Created :   June 21, 2016
Updated :   June 29, 2016
******************************************************************************/
#ifndef FIMGROUP_PARSER_H
  #include "fimgroup_parser.h"
#endif // FIMGROUP_PARSER_H
//------------------------------------
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "../../data/fim/fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
/*****************************************************************************/
// constructor
jfFIMGroupParser::jfFIMGroupParser():jfItemsPageParserBase() {
  pagecount = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse method
void jfFIMGroupParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
  // constants
  const QString func = "jfFIMGroupCollection::ProcessPage";
  // variables
  jfFIMGroup* temp;
  size_t pit_index;
  QString buffer;
  QString perror_out;
  // starting...
  page_parsed = false;
  NewData(inPage);
  // only 1 group per page...
  /**/lpt->tLog(func,1);
  page_results = new jfPDVector();
  pit_index = (pageindex-1);
  /**/lpt->tLogS(func,2,pit_index);
  // next up is parsing the items
 if (xparser.GetDelimited("<!--Content-->","<!--Footer-->",buffer)) {
    pit_index++;
    temp = new jfFIMGroup();
    /**/lpt->tLog(func,3);
    temp->SetFromString(buffer,perror_out);
    // the item is okay
    if (temp->IsValid()) {
      /**/lpt->tLog(func,4);
      page_results->push_back(temp);
      page_parsed = true;
    }
    // the item is not okay
    else {
      /**/lpt->tLog(func,5);
      parseErrorMessage = "ParseError: " + perror_out + " in :\n";
      parseErrorMessage += buffer;
      /**/lpt->tLog(func,6,parseErrorMessage);
      delete page_results;
      page_results = NULL;
    }
  }
  else {
    parseErrorMessage = "ParseError: Missing contents!";
    delete page_results;
    page_results = NULL;
    /**/lpt->tLog(func,7,parseErrorMessage);
  }
   // done with the items
  /**/lpt->tLog(func,8);
  // post page processing and getting the result
  PostPageProcessing();
  /**/lpt->tLog(func,9);
}

//-------------------------------------
void* jfFIMGroupParser::getResults() {
  return MakeResults();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// handling redirection, NULL result if faliure
QString* jfFIMGroupParser::makeRedirectedURL(const QString& inPart) {
  // because fim groups does not use redirecton
  return NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// test methods
//-------------------------------------
bool jfFIMGroupParser::testMissing(const QString *page) const {
  // no special test
  return true;
}
//-------------------------------------
bool jfFIMGroupParser::testIncomplete(const QString *page) const {
  assert(page!=NULL);
  return page->contains("<!--Footer-->");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIMGroupParser::getCookie() const {
  return "view_mature=true";
}


/*****************************************************************************/

