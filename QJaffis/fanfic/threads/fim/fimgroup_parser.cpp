/******************************************************************************
Name    :   fimgroup_parser.cpp
Author  :   John Q Metro
Purpose :   Group object for fimfiction.net
Created :   June 21, 2016
Updated :   March 20, 2023
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
  item_outputter = new jfFIMGroupOutputter();
  item_parser = new jfFIMGroupItemParser();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse method
void jfFIMGroupParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
    // constants
    const QString func = "jfFIMGroupParser::ParseDownloadedPage";
    // variables
    jfFIMGroupItemParser* parser = dynamic_cast<jfFIMGroupItemParser*>(item_parser);

    // starting...
    page_parsed = false;
    NewData(inPage);
    // only 1 group per page...
    /**/lpt->tLog(func,1);
    page_results = new std::vector<jfItemFlagGroup>();
    size_t pit_index = (pageindex-1);
    /**/lpt->tLogS(func,2,pit_index);

    jfItemFlagGroup temp;

    // next up is parsing the items
    QString buffer;
    if (xparser.GetDelimited("<!--Content-->","<!--Footer-->",buffer)) {
        jfItemParseResultState result = parser->ParseFromSource(buffer);
        // the item is okay
        if (result == jfItemParseResultState::SUCCEESS) {
            /**/lpt->tLog(func,4);
            temp.item = parser->GetGroupItem();
            temp.flags = new jfItemMetaFlags();
            page_results->push_back(temp);
            page_parsed = true;
        }
        // ignore (unlikely)
        else if (result == jfItemParseResultState::DEFECTIVE) {

        }
        // parse error
        else {
            /**/lpt->tLog(func,5);
            parseErrorMessage = "ParseError: " + parser->LastError() + " in :\n";
            parseErrorMessage += buffer;
            /**/lpt->tParseError(func,parseErrorMessage);
            delete page_results;
            page_results = NULL;
        }
        parser->Clear();
    }
    else {
        parseErrorMessage = "ParseError: Missing contents!";
        delete page_results;
        page_results = NULL;
        /**/lpt->tParseError(func,parseErrorMessage);
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
QString* jfFIMGroupParser::makeRedirectedURL(const QString& inPart) const {
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

