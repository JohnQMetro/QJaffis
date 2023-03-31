/******************************************************************************
Name    :   ffn_special1.h
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016
Updated :   July 8, 2016 (created using filters taken from other files)
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
class jfFFNGenresFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFFNGenresFilter();
    jfFFNGenresFilter(const jfFFNGenresFilter& insrc);
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
//===============================================================
extern const QString FFN_RATINGS;
//--------------------------------------------------------------------
class jfFFNRatingFilter : public jfBaseFilter {
  public:
    // constructors
    jfFFNRatingFilter();
    jfFFNRatingFilter(QString value_in);
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
    bool Includes(QChar test) const;
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
    QString value;
};
//======================================================================
class jfFFNFavsFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfFFNFavsFilter();
    jfFFNFavsFilter(size_t inmin, size_t inmax);
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};
//===========================================================================
// a simple expression filter that matches against the category name
class jfFFN_CategoryExprFilter : public jfSimpleExpFilterCore {
  public:
    jfFFN_CategoryExprFilter();
    jfFFN_CategoryExprFilter(const jfFFN_CategoryExprFilter& source);
    jfFFN_CategoryExprFilter(jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfFFN_CategoryExprFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};
/*****************************************************************************/
