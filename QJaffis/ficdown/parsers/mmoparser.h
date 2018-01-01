/***************************************************************
 * Name:      mmoparser.h
 * Purpose:   Fic parser: MediaMiner.org
 * Author:    John Q Metro
 * Created:   July 6, 2016
 * Updated:   July 6, 2016
 *
 **************************************************************/
#ifndef MMOPARSER_H
#define MMOPARSER_H
#endif // MMOPARSER_H
//----------------------------------------
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H
/**************************************************************/
class jfMMO_FicPartParser : public jfStoryPartParseBase {
  public:
    jfMMO_FicPartParser();
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
    QString PartProcessing(QString inbuffer);
};
/**************************************************************/
