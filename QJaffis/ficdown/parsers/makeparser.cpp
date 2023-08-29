/***************************************************************
 * Name:      makeparser.cpp
 * Purpose:   a single function to make a fic parser
 * Author:    John Q Metro
 * Created:   July 10, 2016
 * Updated:   August 27, 2023
 *
 **************************************************************/
#ifndef MAKEPARSER_H
  #include "makeparser.h"
#endif // MAKEPARSER_H
//-------------------------
#ifndef AO3PARSER_H
  #include"ao3parser.h"
#endif // AO3PARSER_H
#ifndef FFNPARSER_H
  #include "ffnparser.h"
#endif // FFNPARSER_H
#ifndef MMOPARSER_H
  #include "mmoparser.h"
#endif // MMOPARSER_H
/**************************************************************/
jfStoryPartParseBase* MakeTypedFicParser(jf_FICTYPE in_ftype) {
  if (in_ftype == jfft_FIM) {
    return new jfFIM_FicPartParser();
  }
  else if (in_ftype == jfft_FFN) {
    return new jfFFN_FicPartParser();
  }
  else if (in_ftype == jfft_AO3) {
    return new jfAO3_FicPartParser();
  }
  else if (in_ftype == jfft_MMO) {
    return new jfMMO_FicPartParser();
  }
  else return NULL;
}

// ==============================================================

jfFanficUpdateWrapParser::jfFanficUpdateWrapParser():jfPageParserBase() {
    has_current_type = false;
    current_type = jfft_FFN; // default dummy
    page_index = -1;
    ffn_parser = NULL;
    ao3_parser = NULL;
    fim_parser = NULL;
    mmo_parser = NULL;
}
//--------------------------------------------------
jfFanficUpdateWrapParser::~jfFanficUpdateWrapParser() {
    if (ffn_parser != NULL) delete ffn_parser;
    if (ao3_parser != NULL) delete ao3_parser;
    if (fim_parser != NULL) delete fim_parser;
    if (mmo_parser != NULL) delete mmo_parser;
    has_current_type = false;
}
//--------------------------------------------------
void jfFanficUpdateWrapParser::SetLogPointer(jfLoggBase* in_ptr) {
    lpt = in_ptr;
    // sub parsers will have the log pointer set when we get them
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom parse method
void jfFanficUpdateWrapParser::ParseDownloadedPage(const QString& inPage, size_t pageindex) {
    const QString fname = "jfFanficUpdateWrapParser::ParseDownloadedPage";
    // some logging to check things
    /**/lpt->tLogB(fname,1,has_current_type);
    /**/lpt->tLog(fname,2,Fictype2String(current_type));

    jfStoryPartParseBase* part_parser = fetchParserFor(current_type);
    page_index = pageindex;
    /**/lpt->tLog(fname,4);
    part_parser->ParseDownloadedPage(inPage, page_index);
    /**/lpt->tLog(fname,5);
    page_parsed = part_parser->isPageParsed();
    /**/lpt->tLogB(fname, 6, page_parsed);
    parseErrorMessage = part_parser->getParseErrorMessage();
    /**/lpt->tLog(fname,7);
    if (page_parsed) {
        pagecount = part_parser->getPageCount();
        /**/lpt->tLog(fname,8);
    }
}
//----------------------------------------------------------------
void* jfFanficUpdateWrapParser::getResults() {
    if (has_current_type) {
        jfStoryPartParseBase* part_parser = fetchParserFor(current_type);
        return part_parser->getResults();
    }
    else return NULL;
}
//------------------------------------------------------------
// handling redirection, NULL result if faliure
QString* jfFanficUpdateWrapParser::makeRedirectedURL(const QString& inPart) const {
    if (has_current_type) {
        const jfStoryPartParseBase* part_parser = fetchParserFor(current_type);
        return part_parser->makeRedirectedURL(inPart);
    }
    else return NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// test methods
//--------------------------------------
bool jfFanficUpdateWrapParser::testMissing(const QString *page) const {
    if (has_current_type) {
        return fetchParserFor(current_type)->testMissing(page);
    }
    return false;
}
//--------------------------------------
bool jfFanficUpdateWrapParser::testIncomplete(const QString *page) const {
    if (has_current_type) {
        return fetchParserFor(current_type)->testIncomplete(page);
    }
    return false;
}
// ----------------------------------------
bool jfFanficUpdateWrapParser::testRatelimit(const QString *page) const {
    if (has_current_type) {
        return fetchParserFor(current_type)->testRatelimit(page);
    }
    return false;
}
//---------------------------------------
bool jfFanficUpdateWrapParser::setPageIndex(const size_t& page_index) {
    if (has_current_type) {
        this->page_index = page_index;
        return fetchParserFor(current_type)->setPageIndex(page_index);
    }
    return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFanficUpdateWrapParser::getCookie() const {
    const QString fname = "jfFanficUpdateWrapParser::getCookie";
    /**/lpt->tLogS(fname,1,page_index);
    if (has_current_type) {
        return fetchParserFor(current_type)->getCookie();
    }
    return "";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFanficUpdateWrapParser::SetFicType(jf_FICTYPE fictype) {
    if (IsValidFictype(fictype)) {
        if ((has_current_type == false) || (fictype != current_type)) {
            current_type = fictype;
            setupParserFor(fictype);
            has_current_type = true;
            return true;
        }
        else return false;
    }
    else return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFanficUpdateWrapParser::setupParserFor(jf_FICTYPE type) {
    if (type == jfft_FFN) {
        if (ffn_parser == NULL) {
            ffn_parser = MakeTypedFicParser(type);
            ffn_parser->SetLogPointer(lpt);
        }
        return true;
    }
    else if (type == jfft_AO3) {
        if (ao3_parser == NULL) {
            ao3_parser = MakeTypedFicParser(type);
            ao3_parser->SetLogPointer(lpt);
        }
        return true;
    }
    else if (type == jfft_FIM) {
        if (fim_parser == NULL) {
            fim_parser = MakeTypedFicParser(type);
            fim_parser->SetLogPointer(lpt);
        }
        return true;
    }
    else if (type == jfft_MMO) {
        if (mmo_parser == NULL) {
            mmo_parser = MakeTypedFicParser(type);
            mmo_parser->SetLogPointer(lpt);
        }
        return true;
    }
    else return false;
}
// ----------------------------
jfStoryPartParseBase* jfFanficUpdateWrapParser::fetchParserFor(jf_FICTYPE type) const {
    if (type == jfft_FFN) return ffn_parser;
    else if (type == jfft_AO3) return ao3_parser;
    else if (type == jfft_FIM) return fim_parser;
    else if (type == jfft_MMO) return mmo_parser;
    else return NULL;
}
/*****************************************************************************/
