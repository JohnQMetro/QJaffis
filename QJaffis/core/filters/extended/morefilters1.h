/******************************************************************************
Name    :   morefilters1.h
Author  :   John Q Metro
Purpose :   More Filters
Created :   December 6, 2010
Conversion to QT Started September 30, 2013
Updated :   July 29,2016
******************************************************************************/
#ifndef MOREFILTERS1_H_INCLUDED
#define MOREFILTERS1_H_INCLUDED
#endif
//---------------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef JFMISCTYPES1
	#include "../../objs/misc_types1.h"
#endif
/******************************************************************************/
class jfTagFilterCore : public jfBaseFilter {
  public:
    // constructors
    jfTagFilterCore();
    jfTagFilterCore(const jfTagFilterCore& insrc);
    // getting and setting values
    bool SetTags(const jfTagListing* insource);
    jfTagListing* GetTagsCopy() const;
    virtual bool SetToEmpty() = 0;
    // redefined virtual methods
    virtual bool isEmpty() const;
    virtual QString GetTypeID() const = 0;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // returns the list of element names campatible with this filter
    virtual QString GetTypeDescription() const = 0;
    virtual jfBaseFilter* GenCopy() const = 0;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const = 0;
    bool MatchVsData(const QString& incheck) const;
    void MakeNullResult();
    virtual bool ModifyList(QStringList* templist) const = 0;
    // check the tags against a list of pre-approved tags
    bool VerifyTags(size_t NUMTAGIN, const QString intags[], QStringList* replist);
    bool VerifyTags(const QStringList* sourcelist, QStringList* replist);
    virtual bool DoVerify() = 0;
    void SetEmpty(size_t NUMTAGIN, const QString intags[]);
    void SetEmpty(const QStringList* sourcelist);
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal values
    jfTagListing* thedata;
    QString cm;
    bool nullresult;
    mutable size_t emptyc;
};
//==========================================================================
class jfCompletedFilter : public jfBaseFilter {
  public:
    // constructors
    jfCompletedFilter();
    jfCompletedFilter(bool incexl_in);
    // match against the filter
    virtual bool isEmpty() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // data setting
    void SetValue(bool incexl_in);
    bool GetValue() const;
    void SetEmpty();
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal data
    bool valset, incexl;
};
/******************************************************************************/
