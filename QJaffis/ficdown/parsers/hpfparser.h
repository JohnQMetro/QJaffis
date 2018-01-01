/***************************************************************
 * Name:      hpfparser.h
 * Purpose:   Fic parser: Harry Potter fanfic
 * Author:    John Q Metro
 * Created:   July 4, 2016
 * Updated:   July 5, 2016
 *
 **************************************************************/
#ifndef HPFPARSER_H
#define HPFPARSER_H
#endif // HPFPARSER_H
//--------------------------
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H
/**************************************************************/
class jfHPF_FicPartParser : public jfStoryPartParseBase {
  public:
    jfHPF_FicPartParser();
    // virual methods that are implemented
    virtual QString* makeRedirectedURL(const QString& inPart);
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
/**************************************************************/



