/***************************************************************
 * Name:      hpfparser.h
 * Purpose:   Fic parser: Harry Potter fanfic
 * Author:    John Q Metro
 * Created:   July 4, 2016
 * Updated:   October 12, 2019
 *
 **************************************************************/
#ifndef HPFPARSER_H
#define HPFPARSER_H
#endif // HPFPARSER_H
//--------------------------
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H

#include <QRegExp>
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
    bool IDEPHelper(const QString& label, const QString& start,ulong& outval);
    // custom virtual methods that are implemented
    virtual bool ParseFirstPage(const QString& indata);
    virtual bool ParseOtherPage();
    //helper methods
    QString PartProcessing(QString inbuffer);

    QRegExp spec;
    QRegExp wdiv;
};
/**************************************************************/



