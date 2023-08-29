//***************************************************************************
// Name:        pairfilter.h
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters are complicated.
// Created:     January 1, 2011
// Conversion to QT Started April 11, 2013
// Updated:     August 24, 2023
//***************************************************************************
#ifndef PAIRFILTER_H_INCLUDED
#define PAIRFILTER_H_INCLUDED
#endif // PAIRFILTER_H_INCLUDED
//--------------------------------------------
#ifndef PAIRFILTER_TOOLS_H_INCLUDED
  #include "pairfilter_tools.h"
#endif // PAIRFILTER_TOOLS_H_INCLUDED

#include "../base/basefilter.h"
//***************************************************************************
class jfPairFilterCore : public jfFilterBase {
  public:
    // core constructor
    jfPairFilterCore(const QString& filter_name);
    jfPairFilterCore(QString&& filter_name);
    jfPairFilterCore(const jfPairFilterCore& insrc);
    // setting data
    bool SetSeparators(QString inval);
    const jfPF_flaghold& GetFlagsRef() const;
    jfPF_flaghold& GetMutFlagsRef();
    bool AddNameset(QString inval);
    bool AddSpecials(QString inval);
    // getting data
    QString GetSeparators() const;
    QString GetSpecials() const;
    size_t GetNamesetCount() const;
    QString GetNameset(size_t nindex) const;
    // other methods
    bool ChangeName(QString inname);
    bool StartLogging();
    bool WriteLog();
    // overloaded filter/baseobj methods
    virtual bool IsEmpty() const override;
    virtual QString ToString() const override;
    // virtual copt methods
    virtual jfPairFilterCore* PairFilterCopy() const = 0;
    // destructor
    ~jfPairFilterCore();
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;

    // internal helper matching methods
    bool DoMatchString(QString inbase, size_t ninindex1, size_t ninindex2) const;
    bool DoMatchToken(const QStringList* inbase, size_t ninindex1, size_t ninindex2) const;
    bool MatchSpecialsString(QString inbase) const;
    bool MatchSpecialsToken(const QStringList* inbase) const;
    bool MatchMultipleString(QString inbase, size_t startat) const;
    bool MatchMultipleToken(const QStringList* inbase, size_t startat) const;
    bool MainMatch(const jfSearchResultItem* intest) const;
    // file i/o
    virtual bool CompatReadRestFromFile(jfFileReader* infile) override;
    // internal data
    mutable jfPF_flaghold flags;
    QString sepchars;
    QStringList* specials;
    std::vector<QStringList*> namedata;
    // logging info
    bool logstats;
    jfPairStats* logdata;
};
//==========================================================================
extern const jfFilterTypeMeta SINGLE_PAIR_FILTER_INFO;
// pairing filter: two namesets max
class jfPairFilterSingle : public jfPairFilterCore {
  public:
    // core constructor
    jfPairFilterSingle(const QString& filter_name);
    jfPairFilterSingle(QString&& filter_name);
    jfPairFilterSingle(const jfPairFilterSingle& insrc);
    // returns the list of element names campatible with this filter
    virtual jfFilterBase* GenCopy() const override;
    virtual jfPairFilterCore* PairFilterCopy() const override;
    jfPairFilterSingle* Copy() const;
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
  protected:
    // virtual method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//===========================================================================
extern const jfFilterTypeMeta LIST_PAIR_FILTER_INFO;

// pairing filter: one pairset paired with a list of others
class jfPairFilterList : public jfPairFilterCore {
  public:
    // core constructor
    jfPairFilterList(const QString& filter_name);
    jfPairFilterList(QString&& filter_name);
    jfPairFilterList(const jfPairFilterList& insrc);
    // overloaded filter/baseobj methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // returns the list of element names campatible with this filter
    virtual jfFilterBase* GenCopy() const override;
    virtual jfPairFilterCore* PairFilterCopy() const override;
    jfPairFilterList* Copy() const;
  protected:
    // virtual method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//===========================================================================
extern const jfFilterTypeMeta MULTIPLE_PAIR_FILTER_INFO;

// pairing filter: every nameset matched against the other
class jfPairFilterMultiple : public jfPairFilterCore {
  public:
    // core constructor
    jfPairFilterMultiple(const QString& filter_name);
    jfPairFilterMultiple(QString&& filter_name);
    jfPairFilterMultiple(const jfPairFilterMultiple& insrc);
    // overloaded filter/baseobj methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // returns the list of element names campatible with this filter
    virtual jfFilterBase* GenCopy() const override;
    virtual jfPairFilterCore* PairFilterCopy() const override;
    jfPairFilterMultiple* Copy() const;
  protected:
    // virtual method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};

//***************************************************************************
bool IsOldPairFilter(jfFilterBase* filter);
