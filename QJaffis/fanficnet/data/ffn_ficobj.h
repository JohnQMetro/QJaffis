/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_ficobj.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     May 25, 2010
// Conversion to Qt Started September 25, 2013
// Updated:     July 8, 2016
/////////////////////////////////////////////////////////////////////////////
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef JFFFNSCATCORE
  #include "../categories/ffn_catcore.h"
#endif // JFFFNSCATCORE

#ifndef FICBASE_H
  #include "../../fanfic/data/ficbase.h"
#endif // FICBASE_H

#define JFFNFICOBJ
//*************************************************************************
/* */
//------------------------------------------------------------------------
class jfFFNItemCore : public jfGenericFanfic3 {
  public:
    // constructors
    jfFFNItemCore();
    // implemented virtual methods
    virtual QString ToText() const;
    virtual QString ToDisplayHTML() const;
    // virtual QString GetURLforPart(size_t partindex) const;
    virtual QString MakeAuthorUrl() const;
    // data getting methods
    QString GetCharacter() const;
    bool IsUpdated() const;
    QDate GetPublished() const;
    QChar GetRating() const;
    QString GetLanguage() const;
    size_t GetFavs() const;
    // special
    virtual void ProcessDescription();
  protected:
    // parsing helper methods
    virtual bool GetLinkTitle(jfStringParser& zparser, QString& parseerr) = 0;
    bool ExtractDescription(jfStringParser &zparser, QString &parserr);
    bool Rating2Words(jfStringParser &zparser, QString &parserr);
    bool Dates_and_Completion(jfStringParser &zparser, QString &parserr);
    bool failMsg(QString &pto, const QString message)const;
    // implemented virtual methods
    virtual jfFicExtract* MakeExtract() const;
    virtual void LoadIntoExtract(jfFicExtract* into) const;
    // file i/o output
    virtual bool AddExtraStuff(QTextStream* outfile) const;
    virtual bool ReadExtraStuff(jfFileReader* infile);
    virtual bool AddMoreExtraStuff(QTextStream* outfile) const = 0;
    virtual bool ReadMoreExtraStuff(jfFileReader* infile) = 0;
    // new internal data
    QString language;
    bool isupdated;
    QDate published;
    QChar rating;
    QString char_data;
    size_t favs;
};
//========================================================================
// a forward declaration
class jfAuthorInfo;
//------------------------------
class jfFFNItem : public jfFFNItemCore {
  public:
    // constructors
    jfFFNItem();
    jfFFNItem(const jfFFNItem& src);
    jfFFNItem(const QString& instr, const jfFFN_CategoryCore* cat_linkin);
    // data setting methods
    bool SetCatLink(const jfFFN_CategoryCore* cat_linkin);
    bool SetFromString(const QString& instr, const jfFFN_CategoryCore* cat_linkin, QString& parserr);
    bool UpdateFromObj(const jfFFNItemCore* src);
    bool UpdateWordcount(const jfFFNItemCore* src);
    // implemented virtual methods
    virtual QString GetTypeID() const;
    virtual bool LoadValues(jfSkeletonParser* inparser) const;
    // getting info
    QString GetCatString() const;
    // jfAuthorInfo* GetAuthorInfo() const;
  protected:
    virtual bool GetLinkTitle(jfStringParser& zparser, QString& parseerr);
    // io methods
    virtual bool AddMoreExtraStuff(QTextStream* outfile) const ;
    virtual bool ReadMoreExtraStuff(jfFileReader* infile);
    // external data
    const jfFFN_CategoryCore* cat_link;
};

//*************************************************************************
