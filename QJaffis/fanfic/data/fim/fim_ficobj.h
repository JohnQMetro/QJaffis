/******************************************************************************
Name    :   fim_ficobj.h
Author  :   John Q Metro
Purpose :   Fanfic object for fimfiction.net
Created :   May 7, 2012
// Conversion to QT started : April 20, 2013
Updated :   June 29, 2016
******************************************************************************/
#ifndef FIM_FICOBJ_H_INCLUDED
#define FIM_FICOBJ_H_INCLUDED
#endif // FIM_FICOBJ_H_INCLUDED
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef FICBASE_H
  #include "../ficbase.h"
#endif // FICBASE_H


#ifndef FICEXTRACT_H_INCLUDED
  #include "../../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED

/*****************************************************************************/

class jfStringParser;
//==========================================================================
class jfFIM_Fanfic : public jfGenericFanfic3 {
  public:
    // default constructors
    jfFIM_Fanfic();
    jfFIM_Fanfic(const jfFIM_Fanfic& src);
    // special meta-information
    virtual QString GetTypeID() const;
    // other methods
    bool SetFromString(const QString& inval, QString& parse_err, bool checkavatar);
    jfFicExtract_FIM* ExtractFromString(QString inval, QString& parse_err);
    void SetCompactSummary(const QString& insum);
    // getting data
    QString GetCharacters() const;
    size_t GetThumbs(bool down) const;
    QString GetRating() const;
    QString GetCompactSummary() const;
    // special url construction
    virtual QString MakeAuthorUrl() const;
    // to string functions
    virtual QString ToText() const;
    virtual QString ToDisplayHTML() const;
    // added stuff
    virtual bool LoadValues(jfSkeletonParser* inparser) const;
    virtual void ProcessDescription();
  protected:
    // special methods
    bool ParseFIMDate(QString insrc, QDate& out);
    virtual jfFicExtract* MakeExtract() const;
    virtual void LoadIntoExtract(jfFicExtract* into) const;
    bool ParseError(QString& perror,const QString& message);
    jfFicExtract_FIM* ExtractError(QString& perror,const QString& message);
    // file i/o output
    virtual bool AddExtraStuff(QTextStream* outfile) const;
    virtual bool ReadExtraStuff(jfFileReader* infile);
    // data
    QString characters;
    size_t thumbsup, thumbsdown;
    QString rating;
    QDate pubdate;
    QString compact_summary;
    // parsing data
    jfStringParser* xparser;
};
/*****************************************************************************/
