/***************************************************************
 * Name:      ffnparser.h
 * Purpose:   Fic parser: Fanfiction.net
 * Author:    John Q Metro
 * Created:   July 4, 2016
 * Updated:   June 25, 2022
 *
 **************************************************************/
#ifndef FFNPARSER_H
#define FFNPARSER_H
#endif // FFNPARSER_H
//--------------------------
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H
/**************************************************************/
class jfFFN_FicPartParser : public jfStoryPartParseBase {
  public:
    jfFFN_FicPartParser();
    // virual methods that are implemented
    virtual QString* makeRedirectedURL(const QString& inPart) const;
    virtual bool testMissing(const QString *page) const;
    virtual bool testIncomplete(const QString *page) const;
    virtual QString getCookie() const;

  protected:
    // custom virtual methods that are implemented
    virtual bool ParseFirstPage(const QString& indata);
    virtual bool ParseOtherPage();
    virtual bool ExtractPartContents(QString& partcontents);
    //helper methods
    bool FirstProcessStart(QString& out_title);
    bool DateAndCompletion(jfFicExtract_FFN* in_result);
    bool SectionProcessing(jfFicPart& fpoint);
    QString PartProcessing(QString inbuffer);

    // also
    size_t story_id;
    bool getpartnames;
    QStringList extracted_partnames;
};
/**************************************************************/


