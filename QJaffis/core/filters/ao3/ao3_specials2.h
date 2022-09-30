/******************************************************************************
Name    :   ao3_specials2.h
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   June 25, 2022 (Revise pair filter)
******************************************************************************/
#ifndef AO3_SPECIALS2_H_INCLUDED
#define AO3_SPECIALS2_H_INCLUDED
#endif // AO3_SPECIALS2_H_INCLUDED
//--------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED

#include <QVector>
/*****************************************************************************/
extern const QString warn3_ac;
extern const QString warnlist[];
//-----------------------------
// AO3 now distinguishes between romantic and platonic pairings
class jfAO3Pairing {
  public:
    QString first;
    QString second;
    bool is_platonic;
    bool reader;
    bool original;

    jfAO3Pairing();
    jfAO3Pairing(const jfAO3Pairing& source);
    jfAO3Pairing(const QString& A, const QString& B, const bool in_platonic );

    QString toString() const;

    static int posMin(int a, int b);
    static jfAO3Pairing* ParsePairing(const QString& source);
    static bool ComparePairing(const jfAO3Pairing& pattern,const jfAO3Pairing& target, bool notype, bool either);
};
//------------------------------------------------------

class jfAO3PairFilter : public jfBaseFilter {
  public:
    // constructors
    jfAO3PairFilter();
    jfAO3PairFilter(const jfAO3PairFilter& source);
    // match against the filter
    virtual bool isEmpty() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    bool SetNamesData(const QString& sourcedata);
    QString GetNamesData() const;
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
    // other methods
    void SetAlternates(bool inval);
    bool GetAlternate() const;
    void SetMatchReader(bool inval);
    bool GetMatchReader() const;
    void SetMatchOriginal(bool inval);
    bool GetMatchOriginal() const;
    // destructor
    ~jfAO3PairFilter();
  protected:
    static void EmptyPairList(QVector<jfAO3Pairing*>& plist);
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    QVector<jfAO3Pairing*>* ParsePairs(const QString& inval) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal data
    QVector<jfAO3Pairing*>* pnames;
    bool alternate;
    bool match_reader;
    bool match_original;
};
//========================================================================
// excludes fics based on warnings
class jfAO3WarnFilter : public jfSpecialsFilter {
  public:
    // constrcutors
    jfAO3WarnFilter();
    jfAO3WarnFilter(const QString& insrc);
    // string conversion
    virtual bool FromString(const QString& sourcedata);
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
    // custom methods
    bool Includes(QChar test) const;
    virtual void SetToFull();
    virtual bool IsFull() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};
/*****************************************************************************/
