/******************************************************************************
Name    :   fimfilters1.h
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 4, 2012
Conversion to QT Started September 30, 2013
Updated :   May 28, 2023 (Rebasing)
******************************************************************************/
#ifndef FIMFILTERS1_H_INCLUDED
#define FIMFILTERS1_H_INCLUDED
#endif // FIMFILTERS1_H_INCLUDED
//-----------------------------
#include "../base/basefilter.h"

#ifndef MOREFILTERS1_H_INCLUDED
  #include "../extended/morefilters1.h"
#endif
/*****************************************************************************/
extern const jfFilterTypeMeta FIM_THUMBS_FILTER_INFO;

class jfFimThumbsFilter : public jfFilterBase {
  public:
    // constructors
    jfFimThumbsFilter(const QString& filter_name);
    jfFimThumbsFilter(QString&& filter_name);
    jfFimThumbsFilter(int inmin, int inmax);
    // setting values
    bool SetValues(int inmin, int inmax, bool inc_disabled = false);
    // getting values
    int GetMin() const;
    int GetMax() const;
    bool GetIncludeDisabled() const;
    // redefined virtual methods
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;

    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    // internal values
    int min, max;
    bool include_disabled;
};
//===========================================================================
extern const jfFilterTypeMeta FIM_GENRE_FILTER_INFO;

class jfFIMGenreFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMGenreFilter(const QString& filter_name);
    jfFIMGenreFilter(QString&& filter_name);
    jfFIMGenreFilter(const jfFIMGenreFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty() override;
    // redefined virtual methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    virtual bool ModifyList(QStringList* templist) const override;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify() override;
    virtual bool DoVerifyCheck(jfTagListing* to_check) const override final;
};
//===========================================================================
// getting replacements
QStringList* MakeFIMCharRep();
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
extern const jfFilterTypeMeta FIM_CHARACTER_FILTER_INFO;

class jfFIMCharacterFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMCharacterFilter(const QString& filter_name);
    jfFIMCharacterFilter(QString&& filter_name);
    jfFIMCharacterFilter(const jfFIMCharacterFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty() override;
    // redefined virtual methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    virtual bool ModifyList(QStringList* templist) const override;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify() override;
    virtual bool DoVerifyCheck(jfTagListing* to_check) const override final;
};
//================================================================
extern const jfFilterTypeMeta FIM_RATING_FILTER_INFO;

class jfFimRatingFilter : public jfFilterBase {
  public:
    // constructors
    jfFimRatingFilter(const QString& filter_name);
    jfFimRatingFilter(QString&& filter_name);
    jfFimRatingFilter(const QString& filter_name, QString value_in);
    // match against the filter
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;
    // gets a description
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // copy
    virtual jfFilterBase* GenCopy() const override;
    // custom methods
    bool Includes(QString test) const;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    // internal data
    QString value;
};
//================================================================
extern const jfFilterTypeMeta FIM_PERCENT_RATING_FILTER_INFO;

class jfFimThumbPercentFilter : public jfFilterBase {
  public:
    // constructors
    jfFimThumbPercentFilter(const QString& filter_name);
    jfFimThumbPercentFilter(QString&& filter_name);
    jfFimThumbPercentFilter(const QString& filter_name, QString value_in);
    // match against the filter
    virtual bool IsEmpty() const;
    // getting a string representation
    virtual QString ToString() const override;
    // set and get some parts
    bool SetPercent(size_t invalue);
    void SetIncludeDisabled(bool yes);
    size_t GetPercent() const;
    bool GetIncludeDisabled() const;
    // gets a meta info object
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // copy
    virtual jfFilterBase* GenCopy() const override;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    // internal data
    size_t value;
    bool include_disabled;
};

/*****************************************************************************/
