/******************************************************************************
Name    :   ao3_specials2.h
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   October 10, 2012
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
/*****************************************************************************/
extern const QString warn3_ac;
extern const QString warnlist[];
//-----------------------------
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
    size_t GetPairs() const;
    void SetAlternates(bool inval);
    bool GetAlternate() const;
    // destructor
    ~jfAO3PairFilter();
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    QStringList* ParsePairs(const QString& inval) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal data
    QStringList* pnames;
    bool alternate;
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
