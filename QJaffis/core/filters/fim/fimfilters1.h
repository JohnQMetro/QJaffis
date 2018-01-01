/******************************************************************************
Name    :   fimfilters1.h
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 4, 2012
Conversion to QT Started September 30, 2013
Updated :   August 18, 2014
******************************************************************************/
#ifndef FIMFILTERS1_H_INCLUDED
#define FIMFILTERS1_H_INCLUDED
#endif // FIMFILTERS1_H_INCLUDED
//-----------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../extended/morefilters1.h"
#endif
/*****************************************************************************/
class jfFimThumbsFilter : public jfBaseFilter {
  public:
    // constructors
    jfFimThumbsFilter();
    jfFimThumbsFilter(int inmin, int inmax);
    // setting values
    bool SetValues(int inmin, int inmax);
    // getting values
    int GetMin() const;
    int GetMax() const;
    // redefined virtual methods
    virtual bool isEmpty() const;
    virtual QString GetTypeID() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // returns the list of element names campatible with this filter
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal values
    int min, max;
};
//===========================================================================
class jfFIMGenreFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMGenreFilter();
    jfFIMGenreFilter(const jfFIMGenreFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty();
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    virtual bool ModifyList(QStringList* templist) const;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify();
};
//===========================================================================
// getting replacements
QStringList* MakeFIMCharRep();
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
class jfFIMCharacterFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMCharacterFilter();
    jfFIMCharacterFilter(const jfFIMCharacterFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty();
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    virtual bool ModifyList(QStringList* templist) const;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify();
};
//================================================================
class jfFimRatingFilter : public jfBaseFilter {
  public:
    // constructors
    jfFimRatingFilter();
    jfFimRatingFilter(QString value_in);
    // match against the filter
    virtual bool isEmpty() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
    // custom methods
    bool Includes(QString test) const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal data
    QString value;
};
//================================================================
class jfFimThumbPercentFilter : public jfBaseFilter {
  public:
    // constructors
    jfFimThumbPercentFilter();
    jfFimThumbPercentFilter(QString value_in);
    // match against the filter
    virtual bool isEmpty() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    bool SetPercent(size_t invalue);
    size_t GetPercent() const;
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal data
    size_t value;
};

/*****************************************************************************/
