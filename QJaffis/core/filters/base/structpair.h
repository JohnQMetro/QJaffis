/******************************************************************************
Name    :   structpair.h
Author  :   John Q Metro
Purpose :   'Structured Pair' filters
Created :   February 16, 2023
Updated :   March 10, 2023
******************************************************************************/
#pragma once

#ifndef STRUCTPAIR_H
    #define STRUCTPAIR_H
#endif // STRUCTPAIR_H
//--------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif

#include "../../objs/baseitem.h"

#include <QVector>
/*****************************************************************************/
enum class PairType { Unknown, Platonic, SlashPair, xPair };
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class jfPairingStruct;
class jfPairFilterPair;

/* static methods for parsing and fiddling with pairs. */
class jfPairFilterUtils {
  public:
    static bool isReader(const QString& name);
    static bool isOriginal(const QString& name);
    static int posMin(int a, int b);
    static PairType isStoragePlatonic(const QString& source);
    static PairType isAO3Platonic(const QString& source);
    static PairType isFilterPlatonic(const QString& source);
    static jfPairingStruct* ParsePairingDisplay(const QString& source);
    static jfPairingStruct* ParsePairingStorage(const QString& source);
    static jfPairFilterPair* ParseFilterPair(const QString source);
};
// ============================================================================
/* Sometimes, relationship/romance pairs are in fanfic data in a structured
 * way, so we create a class to hold such data.
 * NOTE: despite the name, this class will also handly polys, and cases where
 * no pair is expressed (often found in AO3 due to freeform).
 */
class jfPairingStruct {
  public:
    QStringList participants;
    bool is_platonic;
    bool reader;
    bool original;

    jfPairingStruct();
    jfPairingStruct(const jfPairingStruct& source);
    jfPairingStruct(const QString& A, const QString& B, const bool& in_platonic );
    jfPairingStruct(const QStringList& people, const bool& in_platonic );
    jfPairingStruct(const QString& source_string);

    QString toDisplayString(bool rom_pipe) const;
    QString toStorageString() const;
    bool NotAPair() const;
};
//===================================================================
/* A 'pair' in a pair filter. This will not exactly be the same as a parsed pair
 * from a fic.
 * For not_pair == true, a special case where in_participants must be length 1
*/
class jfPairFilterPair {
  public:
    jfPairFilterPair();
    jfPairFilterPair(const jfPairFilterPair& source);
    jfPairFilterPair(bool in_platonic, const QStringList& people);
    // the constructor for not_pair
    jfPairFilterPair(const QString& source_string);
    QString toString() const;
    bool NotForPairs() const;
    /* checks if the partipants are in in_participants (lowercase). */
    bool IsAMatchFor(bool list_platonic, const QStringList& in_participants) const;
  protected:
    bool not_pair;
    bool is_platonic;
    QStringList participants; // the should be lowercase only
};
//------------------------------------------------------
class jfFanficPairsMixin;

class jfStructuredPairFilter : public jfBaseFilter {
  public:
    // constructors
    jfStructuredPairFilter();
    jfStructuredPairFilter(const jfStructuredPairFilter& source);
    // match against the filter
    virtual bool isEmpty() const override;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata) override;
    virtual QString ToString() const override;
    bool SetNamesData(const QString& sourcedata, bool nl_split);
    QString GetNamesData(bool nl_split) const;
    // gets a description
    virtual QString GetTypeDescription() const override;
    // copy
    virtual jfBaseFilter* GenCopy() const override;
    // special meta-information
    virtual QString GetTypeID() const override;
    // other methods
    void SetMatchAll(bool inval);
    bool GetMatchAll() const;
    void SetMatchReader(bool inval);
    bool GetMatchReader() const;
    void SetMatchOriginal(bool inval);
    bool GetMatchOriginal() const;
    // destructor
    virtual ~jfStructuredPairFilter();
  protected:
    static void EmptyPairList(QVector<const jfPairFilterPair*>& plist);
    bool DoesPairingMatch(const jfPairingStruct& target) const;
    const jfFanficPairsMixin* CheckCastElement(const jfSearchResultItem* testelem) const;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    bool ParsePairs(const QString& inval, bool nl, QVector<const jfPairFilterPair*>& target) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const override;
    virtual bool ReadRestFromFile(jfFileReader* infile) override;

    /* since different types are stored together, the text file reprentation
    may have objects of varying length */

    virtual size_t ExtraLines() const override;
    // internal data
    QVector<const jfPairFilterPair*> filter_pairs;
    bool match_all;
    bool match_reader;
    bool match_original;
};
