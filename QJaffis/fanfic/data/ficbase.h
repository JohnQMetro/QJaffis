/******************************************************************************
Name    :   ficbase.h
Author  :   John Q Metro
Purpose :   Declares fanfic objects that hold fanfic info
Created :   April 2, 2012
// Conversion to QT started : April 20, 2013
Updated :   August 25, 2023
// Replacing static ids with functions to avoid order problems
******************************************************************************/
#ifndef FICBASE_H
  #define FICBASE_H
#endif // FICBASE_H
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "../../core/objs/baseitem.h"

#ifndef JFMISCTYPES1
  #include "../../core/objs/misc_types1.h"
#endif
#ifndef FICEXTRACT_H_INCLUDED
  #include "../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED

#include "../../core/filters/base/structpair.h"
#include <QVector>

//****************************************************************************

const QString& IdForGenericFanfic();

/* A common base class for Fanfic Items (description) */
class jfGenericFanfic : public jfSearchResultItem {
  public:
    // default constructor
    jfGenericFanfic();
    jfGenericFanfic(const jfSearchResultItemData& init_data);
    jfGenericFanfic(const jfGenericFanfic& src);
    // getting values
    const QString& GetAuthor() const;
    const QString& GetAuthorUrl() const;
    size_t GetChapterCount() const;
    QDate GetUpdates() const;
    // special
    void ChangeAuthorName(const QString& new_author_name);
    // getting the extract
    jfFicExtract* GetExtract() const;
    // special url construction
    virtual QString MakeAuthorUrl() const = 0;

  protected:
    // extra methods
    virtual bool LoadMoreValues(jfSkeletonParser* inparser) const override;
    virtual bool LoadExtraValues(jfSkeletonParser* inparser) const = 0;
    void StoreToCopyGenericFanfic(jfGenericFanfic* destination) const;

    // special methods
    virtual jfFicExtract* MakeExtract() const = 0;
    void LoadIntoExtractCore(jfFicExtract* into) const;
    virtual void LoadIntoExtract(jfFicExtract* into) const = 0;

    // internal data
    QString author_name;
    size_t part_count;
    QString author_url;
    QDate updated_date;
};
//==================================================================
const QString& IdForGenericFanfic2();

/* A base class for fanfic items that have some extras like characters, word counts, and completed state. */
class jfGenericFanfic2 : public jfGenericFanfic {
  public:
    // default constructor
    jfGenericFanfic2();
    jfGenericFanfic2(const jfSearchResultItemData& init_data);
    jfGenericFanfic2(const jfGenericFanfic2& src);
    // getting values
    size_t GetWordcount() const;
    bool IsComplete() const;
    const QStringList& GetCharacterList() const;
  protected:
    // extra methods
    virtual bool LoadExtraValues(jfSkeletonParser* inparser) const override;
    virtual bool LoadMoreValues2(jfSkeletonParser* inparser) const = 0;
    void StoreToCopyGenericFanfic2(jfGenericFanfic2* destination) const;
    // file i/o output
    virtual bool AddRestToFile(QTextStream* outfile) const override;
    virtual bool ReadRestFromFile(jfFileReader* infile) override;
    virtual bool AddExtraStuff(QTextStream* outfile) const = 0;
    virtual bool ReadExtraStuff(jfFileReader* infile) = 0;
    // internal data
    size_t word_count;
    bool completed;
    QStringList characters;
};
// ================================================================
const QString& IdForFanficPairs();

// a mixin class for fanfic items that have pairing info
class jfFanficPairsMixin {
  public:
    int RelationshipCount() const;
    const QVector<const jfPairingStruct*>& GetRelationships() const;
    QString RelationshipsAsDisplayString(bool rom_pipe) const;
  protected:
    QString RelationshipsAsStorageString() const;
    void ClearPairingVector(QVector<const jfPairingStruct*>& target) const;
    void RelationshipCopy(const jfFanficPairsMixin& source);
    void LoadRelationships(jfSkeletonParser* inparser, bool rom_pipe) const;
    bool SetRelationshipsFromString(const QString& source, bool display);
    bool ParseAndAddPair(const QString& source, bool display);

    QVector<const jfPairingStruct*> char_pairs;
};
//==================================================================
const QString& IdForGenericFanfic3();

class jfGenericFanfic3 : public jfGenericFanfic2 {
  public:
    // default constructors
    jfGenericFanfic3();
    jfGenericFanfic3(const jfSearchResultItemData& init_data);
    jfGenericFanfic3(const jfGenericFanfic3& src);
    // data getting methods
    const QString& GetGenres() const;
    size_t GetAuthorID() const;
    void ChangeAuthorID(size_t newid);
  protected:
    // extra methods
    virtual bool LoadMoreValues2(jfSkeletonParser* inparser) const override;
    virtual bool LoadValuesContinued(jfSkeletonParser* inparser) const = 0;
    void StoreToCopyGenericFanfic3(jfGenericFanfic3* destination) const;
    // file i/o output
    virtual bool AddExtraStuff(QTextStream* outfile) const override;
    virtual bool ReadExtraStuff(jfFileReader* infile) override;
    virtual bool AddMoreExtraStuff(QTextStream* outfile) const = 0;
    virtual bool ReadMoreExtraStuff(jfFileReader* infile) = 0;
    // more data
    size_t author_id;
    QString genres;
};
//****************************************************************************
