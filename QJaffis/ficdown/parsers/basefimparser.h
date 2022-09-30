/***************************************************************
 * Name:      basefim.h
 * Purpose:   Fic parsers: base abstract class and fim
 * Author:    John Q Metro
 * Created:   July 3, 2016
 * Updated:   July 4, 2016
 *
 **************************************************************/
#ifndef BASEFIM_H
#define BASEFIM_H
#endif // BASEFIM_H
//----------------------------------
#ifndef BASEPARSE_H
  #include "../../fetching/baseparse.h"
#endif // BASEPARSE_H
#ifndef FICPART_H
  #include "../data/ficpart.h"
#endif // FICPART_H

/*************************************************************/
class jfStoryPartParseBase : public jfPageParserBase {
  public:
    jfStoryPartParseBase();
    // implemented virtual methods
    virtual void ParseDownloadedPage(const QString& inPage, size_t pageindex);
    virtual void* getResults();
    // virual methods that still haveto be implemented
    virtual QString* makeRedirectedURL(const QString& inPart) const = 0;
    virtual bool testMissing(const QString *page) const = 0;
    virtual bool testIncomplete(const QString *page) const = 0;
    virtual QString getCookie() const = 0;

  protected:
    // custom virtual methods to implement
    virtual bool ParseFirstPage(const QString& indata) = 0;
    virtual bool ParseOtherPage() = 0;

  // result info
    jfFicExtract* extract;
    jfFicPart* fpart;
    size_t partcount;

};
//==========================================================
class jfFIM_FicPartParser : public jfStoryPartParseBase {
  public:
    jfFIM_FicPartParser();
    // virual methods that are implemented
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual QString getCookie() const;

  protected:
    // custom virtual methods that are implemented
    virtual bool ParseFirstPage(const QString& indata);
    virtual bool ParseOtherPage();
    //helper methods
    bool HandleChapters(QString inbuffer,jfFicPart& fpoint);
    QString PartProcessing(QString inbuffer);
};

/*************************************************************/
