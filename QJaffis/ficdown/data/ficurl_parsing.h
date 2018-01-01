/****************************************************************************
Name:        ficurl_parsing.h
Author  :    John Q Metro
Purpose :    When given a fic url to download, it has to be checked
Created :    July 8, 2016
Updated :    July 12, 2016 (conversion to Qt)

****************************************************************************/
#ifndef FICURL_PARSING_H_INCLUDED
  #define FICURL_PARSING_H_INCLUDED
#endif // FICURL_PARSING_H_INCLUDED
//----------------------------------
#ifndef FICEXTRACT_H_INCLUDED
  #include "ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
/***************************************************************************/
class jfFicURLParser {
  public:
    jfFicURLParser();
    // data in
    bool ParseURL(const QString& input_url);
    // data out
    bool isValid() const;
    jf_FICTYPE getType() const;
    QString firstURL();
    QString FilenameBase();
  protected:
    bool ProcessStart(QStringList* urlarr, bool& writehttp);
    bool HandleFFN(QStringList* urlarr);
    bool HandleFIM(QStringList* urlarr);
    bool HandleAO3(QStringList* urlarr);
    bool HandleHPF(QStringList* urlarr);
    bool HandleMMO(QStringList* urlarr);

    // internal data
    bool validURL;
    bool writehttp;
    QString inputURL;
    QString outputURL;
    QString fname_base;
    jf_FICTYPE type;
};
/***************************************************************************/
