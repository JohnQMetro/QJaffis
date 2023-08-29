/******************************************************************************
Name    :   newfimgroup_parser.cpp
Author  :   John Q Metro
Purpose :   Now that there is a group search interface at Fimfiction.Net, the
old direct parser can be mostly replaced by a more typical item parser.
Created :   January 6, 2018
Updated :   March 26, 2023
******************************************************************************/
#ifndef NEWFIMGROUP_PARSER_H
    #include "newfimgroup_parser.h"
#endif // NEWFIMGROUP_PARSER_H
//----------------------------------
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "../../data/fim/fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED

/*****************************************************************************/
// constructor
jfNewFIMGroupParser::jfNewFIMGroupParser():jfItemsPageParserBase() {
    pagecount = 0;
    item_outputter = new jfFIMGroupOutputter();
    item_parser = new jfFIMGroupItemParser();
}
//------------------------------------------------------------
// custom parse method
void jfNewFIMGroupParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
    // constants
    const QString func = "jfNewFIMGroupParser::ParseDownloadedPage";
    // variables

    // starting...
    page_parsed = false;
    NewData(inPage);
    // maximum 36 groups per page
    /**/lpt->tLog(func,1);
    page_results = new std::vector<jfItemFlagGroup>();
    size_t pit_index = (pageindex-1)*36;
    /**/lpt->tLogS(func,2,pit_index);

    if (!xparser.MovePast("<ul class=\"group-card-list\">")) {
        parseErrorMessage = "ParseError: Missing 'card-list' contents!";
        delete page_results;
        page_results = NULL;
        /**/lpt->tParseError(func,parseErrorMessage);
    }
    else {
        // preparing for the loop
        jfFIMGroupItemParser* parser = dynamic_cast<jfFIMGroupItemParser*>(item_parser);
        jfItemFlagGroup temp;
        QString buffer;

        while (xparser.GetDelimited("<li>","</div> </div>",buffer)) {
            pit_index++;
            /**/lpt->tLog(func,4);
            jfItemParseResultState result = parser->ParseFromCardSource(buffer);

            // the item is okay
            if (result == jfItemParseResultState::SUCCEESS) {
                /**/lpt->tLog(func,5);
                temp.item = parser->GetGroupItem();
                temp.flags = new jfItemMetaFlags();
                page_results->push_back(temp);
                parser->Clear();

            }
            else if (result == jfItemParseResultState::DEFECTIVE) {
                /**/lpt->tLog(func,6);
                parser->Clear();
            }
            else {
                /**/lpt->tLog(func,7);
                parseErrorMessage = "ParseError: " + parser->LastError() + " in :\n";
                parseErrorMessage += buffer;
                /**/lpt->tParseError(func,parseErrorMessage);
                parser->Clear();
                delete page_results;
                page_results = NULL;
                break;
            }

        }
        if (page_results != NULL) {
            // here, we have the cards, but we still need the page count
            if (xparser.MovePast("<div class=\"page_list\">")) {
                xparser.ChopAfter("</ul>",true);
                // the last link label that can be converted to an integer is the page count!
                ulong tval = 1; QString perror_out;
                while(xparser.MovePast("<a href=\"")) {
                    xparser.GetDelimitedULong("\">","</a>",tval,perror_out);
                }
                pagecount = tval;
                page_parsed = true;
            }
        }
    }
     // done with the items
    /**/lpt->tLog(func,8);
    // post page processing and getting the result
    PostPageProcessing();
    /**/lpt->tLog(func,9);
}

//-----------------------
void* jfNewFIMGroupParser::getResults() {
      return MakeResults();
}
//------------------------------------------------------------
// handling redirection, NULL result if faliure
QString* jfNewFIMGroupParser::makeRedirectedURL(const QString& inPart) const{
    return NULL; // fimfiction.net does not use redirection (normally)
}
//------------------------------------------------------------
// test methods
bool jfNewFIMGroupParser::testMissing(const QString *page) const {
    // no special test
    return true;
}
//------------------------------------------
bool jfNewFIMGroupParser::testIncomplete(const QString *page) const {
    assert(page!=NULL);
    return page->contains("<!--Footer-->");
}
// --------------------------------------------
bool jfNewFIMGroupParser::testRatelimit(const QString *page) const {
    return true;
}
//------------------------------------------------------------
// another implemented method
QString jfNewFIMGroupParser::getCookie() const {
    return "view_mature=true";
}

/*****************************************************************************/

