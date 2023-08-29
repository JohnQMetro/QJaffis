/******************************************************************************
Name    :   ao3_specials.h
Author  :   John Q Metro
Purpose :   AO3 rating and orientation filters
Created :   September 8, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   April 19, 2023
******************************************************************************/
#ifndef AO3_SPECIALS1_H_INCLUDED
#define AO3_SPECIALS1_H_INCLUDED
#endif // AO3_SPECIALS1_H_INCLUDED
//-------------------------------------------
#include "../base/basefilter.h"

#ifndef MOREFILTERS1_H_INCLUDED
  #include "../extended/morefilters1.h"
#endif
/*****************************************************************************/
class jfSpecialsFilter : public jfFilterBase {
  public:
    // constructors
    jfSpecialsFilter(const QString& filter_name);
    jfSpecialsFilter(QString&& filter_name);
    // match against the filter
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;
    // custom methods
    bool Includes(QChar test) const;
    virtual void SetToFull() = 0;
    virtual bool IsFull() const = 0;
  protected:
    // internal data
    QString value;
};
//========================================================================
extern const jfFilterTypeMeta AO3_RATING_FILTER_INFO;
// --------------------------------------------------
class jfAO3RatingFilter : public jfSpecialsFilter {
  public:
    // constructors
    jfAO3RatingFilter(const QString& filter_name);
    jfAO3RatingFilter(QString&& filter_name);
    jfAO3RatingFilter(const QString& filter_name, const QString& sourcedata);
    // implemented virtual methods
    virtual jfFilterBase* GenCopy() const override;
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // implemented custom methods
    virtual void SetToFull() override;
    virtual bool IsFull() const override;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override final;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//========================================================================
extern const jfFilterTypeMeta AO3_ORIENTATION_FILTER_INFO;
// --------------------------------------
class jfAO3OrientationFilter : public jfTagFilterCore {
  public:
    // constructors
    jfAO3OrientationFilter(const QString& filter_name);
    jfAO3OrientationFilter(QString&& filter_name);
    jfAO3OrientationFilter(const jfAO3OrientationFilter& insrc);
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
    virtual bool DoVerifyCheck(jfTagListing* to_check) const override;
};
/*****************************************************************************/
