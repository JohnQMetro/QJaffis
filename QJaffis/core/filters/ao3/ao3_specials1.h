/******************************************************************************
Name    :   ao3_specials.h
Author  :   John Q Metro
Purpose :   AO3 rating and orientation filters
Created :   September 8, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   December 24, 2021 (changing orientation filter to a tagfilter)
******************************************************************************/
#ifndef AO3_SPECIALS1_H_INCLUDED
#define AO3_SPECIALS1_H_INCLUDED
#endif // AO3_SPECIALS1_H_INCLUDED
//-------------------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../extended/morefilters1.h"
#endif
/*****************************************************************************/
class jfSpecialsFilter : public jfBaseFilter {
  public:
    // constructors
    jfSpecialsFilter();
    // match against the filter
    virtual bool isEmpty() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata) = 0;
    virtual QString ToString() const;
    // gets a description
    virtual QString GetTypeDescription() const = 0;
    // copy
    virtual jfBaseFilter* GenCopy() const = 0;
    // special meta-information
    virtual QString GetTypeID() const = 0;
    // custom methods
    bool Includes(QChar test) const;
    virtual void SetToFull() = 0;
    virtual bool IsFull() const = 0;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const = 0;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal data
    QString value;
};
//========================================================================
class jfAO3RatingFilter : public jfSpecialsFilter {
  public:
    // constructors
    jfAO3RatingFilter();
    jfAO3RatingFilter(const QString& sourcedata);
    // implemented virtual methods
    virtual bool FromString(const QString& sourcedata);
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
    // implemented custom methods
    virtual void SetToFull();
    virtual bool IsFull() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};
//========================================================================

class jfAO3OrientationFilter : public jfTagFilterCore {
  public:
    // constructors
    jfAO3OrientationFilter();
    jfAO3OrientationFilter(const jfAO3OrientationFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty();
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
    virtual bool ModifyList(QStringList* templist) const;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify();
};
/*****************************************************************************/
