//***************************************************************************
// Name:        pairfilter.h
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters are complicated.
// Created:     January 1, 2011
// Conversion to QT Started April 11, 2013
// Updated:     August 22, 2012 (adding jfFileReader)
//***************************************************************************
#ifndef PAIRFILTER_H_INCLUDED
#define PAIRFILTER_H_INCLUDED
#endif // PAIRFILTER_H_INCLUDED
//--------------------------------------------
#ifndef PAIRFILTER_TOOLS_H_INCLUDED
  #include "pairfilter_tools.h"
#endif // PAIRFILTER_TOOLS_H_INCLUDED
#ifndef JFBASEFILTER
  #include "filterbase.h"
#endif
//***************************************************************************
class jfPairFilterCore : public jfBaseFilter {
  public:
    // core constructor
    jfPairFilterCore();
    jfPairFilterCore(const jfPairFilterCore& insrc);
    // setting data
    bool SetSeparators(QString inval);
    jfPF_flaghold& GetFlagsRef() const;
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
    // returns a general filter type
    virtual size_t GetFilType() const;
    // overloaded filter/baseobj methods
    virtual bool isEmpty() const;
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    virtual QString GetTypeID() const = 0;
    virtual QString GetTypeDescription() const = 0;
    // virtual copt methods
    virtual jfBaseFilter* GenCopy() const = 0;
    virtual jfPairFilterCore* PairFilterCopy() const = 0;
    // destructor
    ~jfPairFilterCore();
  protected:
    // virtual method
    virtual bool CoreMatch(const jfBasePD* testelem) const = 0;
    // internal helper matching methods
    bool DoMatchString(QString inbase, size_t ninindex1, size_t ninindex2) const;
    bool DoMatchToken(const QStringList* inbase, size_t ninindex1, size_t ninindex2) const;
    bool MatchSpecialsString(QString inbase) const;
    bool MatchSpecialsToken(const QStringList* inbase) const;
    bool MatchMultipleString(QString inbase, size_t startat) const;
    bool MatchMultipleToken(const QStringList* inbase, size_t startat) const;
    bool MainMatch(const jfBasePD* intest) const;
    // file i/o
    virtual size_t ExtraLines() const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
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
// pairing filter: two namesets max
class jfPairFilterSingle : public jfPairFilterCore {
  public:
    // core constructor
    jfPairFilterSingle();
    jfPairFilterSingle(const jfPairFilterSingle& insrc);
    // overloaded filter/baseobj methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    // returns the list of element names campatible with this filter
    virtual jfBaseFilter* GenCopy() const;
    virtual jfPairFilterCore* PairFilterCopy() const;
    jfPairFilterSingle* Copy() const;
  protected:
    // virtual method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};
//===========================================================================
// pairing filter: one pairset paired with a list of others
class jfPairFilterList : public jfPairFilterCore {
  public:
    // core constructor
    jfPairFilterList();
    jfPairFilterList(const jfPairFilterList& insrc);
    // overloaded filter/baseobj methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    // returns the list of element names campatible with this filter
    virtual jfBaseFilter* GenCopy() const;
    virtual jfPairFilterCore* PairFilterCopy() const;
    jfPairFilterList* Copy() const;
  protected:
    // virtual method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};
//===========================================================================
// pairing filter: every nameset matched against the other
class jfPairFilterMultiple : public jfPairFilterCore {
  public:
    // core constructor
    jfPairFilterMultiple();
    jfPairFilterMultiple(const jfPairFilterMultiple& insrc);
    // overloaded filter/baseobj methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    // returns the list of element names campatible with this filter
    virtual jfBaseFilter* GenCopy() const;
    virtual jfPairFilterCore* PairFilterCopy() const;
    jfPairFilterMultiple* Copy() const;
  protected:
    // virtual method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};

//***************************************************************************
