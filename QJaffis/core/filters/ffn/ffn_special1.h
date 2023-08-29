/******************************************************************************
Name    :   ffn_special1.h
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016
Updated :   May 5, 2023
******************************************************************************/
#ifndef FFN_SPECIAL1_H
  #define FFN_SPECIAL1_H
#endif // FFN_SPECIAL1_H
//--------------------------------
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../extended/morefilters1.h"
#endif
#ifndef MINMAX_FILTER_H
  #include "../extended/minmax.h"
#endif // MINMAX_FILTER_H

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
/*****************************************************************************/
extern const jfFilterTypeMeta FFN_GENRES_FILTER_INFO;

class jfFFNGenresFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFFNGenresFilter(const QString& filter_name);
    jfFFNGenresFilter(QString&& filter_name);
    jfFFNGenresFilter(const jfFFNGenresFilter& insrc);
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
//===============================================================
extern const jfFilterTypeMeta FFN_RATINGS_FILTER_INFO;

extern const QString FFN_RATINGS;
//--------------------------------------------------------------------
class jfFFNRatingFilter : public jfFilterBase {
  public:
    // constructors
    jfFFNRatingFilter(const QString& filter_name);
    jfFFNRatingFilter(QString&& filter_name);
    jfFFNRatingFilter(const QString& filter_name, QString value_in);
    // match against the filter
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;
    // gets a description
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // copy
    virtual jfFilterBase* GenCopy() const override;
    // custom methods
    bool Includes(QChar test) const;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    // internal data
    QString value;
};
//===========================================================================
extern const jfFilterTypeMeta FFN_FANDOM_FILTER_INFO;

// a simple expression filter that matches against the category name
class jfFFN_CategoryExprFilter : public jfSimpleExpFilterCore {
  public:
    jfFFN_CategoryExprFilter(const QString& filter_name);
    jfFFN_CategoryExprFilter(QString&& filter_name);
    jfFFN_CategoryExprFilter(const jfFFN_CategoryExprFilter& source);
    jfFFN_CategoryExprFilter(const QString& filter_name, jfSimpleExpr* in_source);
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFFN_CategoryExprFilter* Copy() const;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
/*****************************************************************************/
