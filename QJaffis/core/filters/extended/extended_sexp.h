//***************************************************************************
// Name:        extended_sexp.h
// Author :     John Q Metro
// Purpose :    Specializations of string expressions for particular strings
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     May 2, 2023
//***************************************************************************
#ifndef EXTENDED_SEXP_FILTER_H
  #define EXTENDED_SEXP_FILTER_H
#endif // EXTENDED_SEXP_FILTER_H
//------------------------------------------
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
//***************************************************************************

extern const jfFilterTypeMeta AUTHOR_FILTER_INFO;

// a simple expression filter that matches against author data
class jfAuthExprFilter : public jfSimpleExpFilterCore {
  public:
    jfAuthExprFilter(const QString& filter_name);
    jfAuthExprFilter(QString&& filter_name);
    jfAuthExprFilter(const jfAuthExprFilter& source);
    jfAuthExprFilter(const QString& filter_name, jfSimpleExpr* in_source);
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfAuthExprFilter* Copy() const;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//===========================================================================
extern const jfFilterTypeMeta LANGUAGE_FILTER_INFO;

// a simple expression filter that matches against the language name
class jfLanguageExprFilter : public jfSimpleExpFilterCore {
  public:
    jfLanguageExprFilter(const QString& filter_name);
    jfLanguageExprFilter(QString&& filter_name);
    jfLanguageExprFilter(const jfLanguageExprFilter& source);
    jfLanguageExprFilter(const QString& filter_name, jfSimpleExpr* in_source);
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfLanguageExprFilter* Copy() const;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//***************************************************************************
