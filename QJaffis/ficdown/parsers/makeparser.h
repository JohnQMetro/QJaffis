/***************************************************************
 * Name:      makeparser.h
 * Purpose:   a single function to make a fic parser
 *            Also, a fic part parser that wraps other fic part parsers
 * Author:    John Q Metro
 * Created:   July 10, 2016
 * Updated:   July 4, 2022
 *
 **************************************************************/
#ifndef MAKEPARSER_H
  #define MAKEPARSER_H
#endif // MAKEPARSER_H
//-------------------------
#ifndef FICEXTRACT_H_INCLUDED
  #include "../data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H

#ifndef BASEPARSE_H
    #include "../../fetching/baseparse.h"
#endif // BASEPARSE_H
/**************************************************************/
jfStoryPartParseBase* MakeTypedFicParser(jf_FICTYPE in_ftype);

// ==================================================================
class jfFanficUpdateWrapParser : virtual public jfPageParserBase {
  public:
    // constructor
    jfFanficUpdateWrapParser();
    virtual ~jfFanficUpdateWrapParser();
    virtual void SetLogPointer(jfLoggBase* in_ptr);
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual bool testRatelimit(const QString *page) const;
    virtual bool setPageIndex(const size_t& page_index);
    // getting results
    size_t getPageCount() const;
    // also
    virtual QString getCookie() const;
    bool SetFicType(jf_FICTYPE fictype);
  protected:
    bool setupParserFor(jf_FICTYPE type);
    jfStoryPartParseBase* fetchParserFor(jf_FICTYPE type) const;
    // data
    size_t page_index;
    jf_FICTYPE current_type;
    bool has_current_type;

    jfStoryPartParseBase* ffn_parser;
    jfStoryPartParseBase* ao3_parser;
    jfStoryPartParseBase* fim_parser;
    jfStoryPartParseBase* mmo_parser;



};

/**************************************************************/
