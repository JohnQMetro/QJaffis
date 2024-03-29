/***************************************************************
 * Name:      ao3parser.h
 * Purpose:   Fic parser: Archiveofourown.org
 * Author:    John Q Metro
 * Created:   July 6, 2016
 * Updated:   January 20, 2020
 *
 **************************************************************/
#ifndef AO3PARSER_H
#define AO3PARSER_H
#endif // AO3PARSER_H
//-------------------------------
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H

#ifndef FICEXTRACT_H_INCLUDED
  #include "../data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED

#include <QRegExp>
/**************************************************************/
class jfAO3_FicPartParser : public jfStoryPartParseBase {
  public:
    jfAO3_FicPartParser();
    // virual methods that are implemented
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual bool testRatelimit(const QString *page) const;
    virtual QString getCookie() const;

  protected:
    // custom virtual methods that are implemented
    virtual bool ParseFirstPage(const QString& indata);
    virtual bool ParseOtherPage();
    //helper methods
    bool ExtractChapterFicIDs(jfFicExtract_AO3* extract_ptr, QString& out_partname);
    bool DatesAndCompletion(jfFicExtract_AO3* extract_ptr);
    bool HandleChapters(QString inbuffer,jfFicPart& fpoint);
    QString PartProcessing(QString inbuffer);
    QString GetStartNotes();
    // some regexs, because http vs https is annoying
    static QRegExp* twit_exp;
    static QRegExp* ao3_exp;
    void setupRegExp();
};
/**************************************************************/
