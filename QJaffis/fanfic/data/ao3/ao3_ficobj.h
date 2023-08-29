/******************************************************************************
Name    :   ao3_ficobj.h
Author  :   John Q Metro
Purpose :   Declares fanfic object data of archoveofourown.org
Created :   August 26, 2012
Conversion to Qt Started September 28, 2013
Updated :  March 18, 2023
******************************************************************************/
#ifndef AO3_FICOBJ_H_INCLUDED
#define AO3_FICOBJ_H_INCLUDED
#endif // AO3_FICOBJ_H_INCLUDED
//---------------------------------------
#ifndef AO3_CATS_H_INCLUDED
  #include "ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
#ifndef STRINGPARSER_H_INCLUDED
  #include "../../../core/utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
#ifndef FICBASE_H
  #include "../ficbase.h"
#endif // FICBASE_H

#include <vector>
#include <QLocale>
//**************************************************************************
const QString& IdForAO3Fanfic();

class jfAO3Fanfic : public jfGenericFanfic2, public jfFanficPairsMixin {
    friend class jfAO3FanficItemParser;
  public:
    // default constructors
    jfAO3Fanfic();
    jfAO3Fanfic(const jfSearchResultItemData& init_data);
    jfAO3Fanfic(const jfAO3Fanfic& src);
    // getting info
    QChar GetRating() const;
    bool MultipleOrientations() const;
    const QString& GetOrientations() const;
    QChar GetWarnflag() const;
    const QString& GetWarntag() const;
    bool TestWarntag(QChar inval) const;
    int GetEstPartCount() const;  // part counts are often given as <current>/<estimated total>
    size_t GetKudos() const;
    const QStringList& GetExtraTags() const;
    QString GetJoinedExtraTags() const;
    // series info
    bool InSeries() const;
    size_t SeriesIndex() const;
    QString SeriesName() const;
    QString SeriesUrl() const;
    // fandom info
    QString GetFandoms() const;
    bool MultipleFandoms() const;
    QString GetFandom(size_t inindex) const;
    size_t FandomCount() const;
    // special url construction
    virtual QString MakeAuthorUrl() const override;
    // to string helper methods
    QString RatingToString() const;
    QString WarnToString() const;
    // added stuff
    virtual ~jfAO3Fanfic();
  protected:
    virtual bool LoadMoreValues2(jfSkeletonParser* inparser) const override;

    // special methods
    virtual jfFicExtract* MakeExtract() const override;
    virtual void LoadIntoExtract(jfFicExtract* into) const override;
    // file i/o output
    virtual bool AddExtraStuff(QTextStream* outfile) const override;
    virtual bool ReadExtraStuff(jfFileReader* infile) override;
    bool AddMoreExtraStuff(QTextStream* outfile) const;
    bool ReadMoreExtraStuff(jfFileReader* infile);

    // more data
    QChar rating, warn;
    QString warntags;
    QString orientations;
    int eccount;
    size_t kudcount;
    QStringList extratags;
    // categories
    std::vector<const jfAO3_Category*> cats;
    // series
    size_t series_index;
    QString series_name, series_url;
};
//**************************************************************************
