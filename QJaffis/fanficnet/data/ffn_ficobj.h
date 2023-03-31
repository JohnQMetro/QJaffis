/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_ficobj.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     May 25, 2010
// Conversion to Qt Started September 25, 2013
// Updated:     March 24, 2023
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
class jfFFNItemCore : public jfGenericFanfic3, public jfFanficPairsMixin {
    friend class jfFFNFanficParseBase;
  public:
    static const QString FFN_CORE_TYPE_ID;

    // constructors
    jfFFNItemCore();
    jfFFNItemCore(const jfSearchResultItemData& init_data);
    jfFFNItemCore(const jfFFNItemCore& src);
    // virtual QString GetURLforPart(size_t partindex) const;
    virtual QString MakeAuthorUrl() const override;
    // data getting methods
    bool IsUpdated() const;
    QDate GetPublished() const;
    QChar GetRating() const;
    const QString& GetLanguage() const;
    size_t GetFavs() const;
    // special
    virtual ~jfFFNItemCore();
  protected:
    void StoreToCopyFFNCore(jfFFNItemCore* destination) const;
    bool UpdateFromSource(const jfFFNItemCore* source);

    // implemented virtual methods
    virtual bool LoadValuesContinued(jfSkeletonParser* inparser) const override;
    virtual jfFicExtract* MakeExtract() const override;
    virtual void LoadIntoExtract(jfFicExtract* into) const override;
    // file i/o output
    virtual bool AddMoreExtraStuff(QTextStream* outfile) const override;
    virtual bool ReadMoreExtraStuff(jfFileReader* infile) override;
    virtual bool AddCodaStuff(QTextStream* outfile) const = 0;
    virtual bool ReadCodaStuff(jfFileReader* infile) = 0;

    // new internal data
    QString language;
    bool isupdated;
    QDate published;
    QChar rating;
    size_t favs;
};
//========================================================================

class jfFFNItem : public jfFFNItemCore {
    friend class jfFFNFanficParse;
  public:
    static const QString FFN_ITEM_TYPE_ID;

    // constructors
    jfFFNItem();
    jfFFNItem(const jfSearchResultItemData& init_data);
    jfFFNItem(const jfFFNItem& src);

    // data setting methods
    bool SetCatLink(const jfFFN_CategoryCore* cat_linkin);
    bool UpdateFromObj(const jfFFNItemCore* src);
    bool UpdateWordcount(const jfFFNItemCore* src);
    // getting info
    QString GetCatString() const;
  protected:

    // io methods
    virtual bool AddCodaStuff(QTextStream* outfile) const ;
    virtual bool ReadCodaStuff(jfFileReader* infile);
    // external data
    const jfFFN_CategoryCore* cat_link;
};

//*************************************************************************
