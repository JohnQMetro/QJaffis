/*****************************************************************************
Name    : fimtag_parse.h
Basic   : Parser for getting tag data out of a downloaded Fimfiction.net page
Author  : John Q Metro
Started : December 29, 2017
Updated : December 29, 2017

******************************************************************************/
#ifndef FIMTAG_PARSE_H
    #define FIMTAG_PARSE_H
#endif // FIMTAG_PARSE_H
//-----------------------------------
#ifndef BASEPARSE_H
    #include "../../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef FIM_TAGS_H
    #include "../../data/fim/fim_tags.h"
#endif // FIM_TAGS_H
/*****************************************************************************/
class jfFIMTag_Extractor : public jfPageParserBase {
  public:
    // constructor
    jfFIMTag_Extractor();
    // custom parse method
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // handling redirection, NULL result if faliure
    virtual QString* makeRedirectedURL(const QString& inPart);
    // test methods
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    // also
    virtual QString getCookie() const;
  protected:
    jfFIMTag_Collection* tag_data;

};
/*****************************************************************************/
