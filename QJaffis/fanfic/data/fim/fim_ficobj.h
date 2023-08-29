/******************************************************************************
Name    :   fim_ficobj.h
Author  :   John Q Metro
Purpose :   Fanfic object for fimfiction.net
Created :   May 7, 2012
// Conversion to QT started : April 20, 2013
Updated :   March 18, 2023
******************************************************************************/
#ifndef FIM_FICOBJ_H_INCLUDED
#define FIM_FICOBJ_H_INCLUDED
#endif // FIM_FICOBJ_H_INCLUDED
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef FICBASE_H
  #include "../ficbase.h"
#endif // FICBASE_H


#include <QLocale>
/*****************************************************************************/
const QString& IdForFIMFanfics();

//==========================================================================
class jfFIM_Fanfic : public jfGenericFanfic3 {
    friend class jfFIMFanficItemParser;
  public:
    // default constructors
    jfFIM_Fanfic();
    jfFIM_Fanfic(const jfSearchResultItemData& init_data);
    jfFIM_Fanfic(const jfFIM_Fanfic& src);
    // other methods
    void SetCompactSummary(const QString& insum);
    // getting data
    const QString& GetContentTypes() const;
    const QString& GetWarnings() const;
    int GetThumbs(bool down) const;
    bool AreRatingsDisabled() const;
    QString GetRating() const;
    const QString& GetCompactSummary() const;
    QDate GetPublishedDate() const;
    // special url construction
    virtual QString MakeAuthorUrl() const override;
  protected:
    virtual bool LoadValuesContinued(jfSkeletonParser* inparser) const override;
    void StoreToCopyFIMFanfic(jfFIM_Fanfic* destination) const;
    // special methods

    virtual jfFicExtract* MakeExtract() const override;
    virtual void LoadIntoExtract(jfFicExtract* into) const override;
    // file i/o output
    virtual bool AddMoreExtraStuff(QTextStream* outfile) const override;
    virtual bool ReadMoreExtraStuff(jfFileReader* infile) override;

    // data
    QString content_types,warnings;
    int thumbsup, thumbsdown;
    QString rating;
    QDate pubdate;
    QString compact_summary;
};
/*****************************************************************************/
