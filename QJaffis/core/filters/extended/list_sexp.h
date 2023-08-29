//***************************************************************************
// Name:        list_sexp.h
// Author :     John Q Metro
// Purpose :    String expression based filters for lists of strings
// Created:     February 26, 2023
// Updated:     April 9, 2023 (Rebasing)
//***************************************************************************
#ifndef LIST_SEXP_FILTER_H
  #define LIST_SEXP_FILTER_H
#endif // LIST_SEXP_FILTER_H

#pragma once
//------------------------------------------
#include "../base/basefilter.h"

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif
//***************************************************************************
// the matching mode of the list expression matcher
enum class jfListMatchMode {
    MATCH_MERGED, MATCH_ONE, MATCH_ALL, MATCH_75P
};
// ---------------------------------------------------------------

class jfListExprFilterBase : public jfFilterBase {
  public:
    // the constructors
    jfListExprFilterBase(const QString& filter_name);
    jfListExprFilterBase(QString&& filter_name);
    jfListExprFilterBase(jfListMatchMode in_match_mode, jfSimpleExpr* in_source);
    jfListExprFilterBase(const QString& filter_name, jfListMatchMode in_match_mode, jfSimpleExpr* in_source);
    jfListExprFilterBase(const jfListExprFilterBase& source);

    // redefined virtual functions
    virtual bool IsEmpty() const override;
    virtual QString ToString() const override;

    // extra methods
    jfListMatchMode GetMatchMode() const;
    void SetMatchMode(jfListMatchMode in_match_mode);
    bool FromExpr(jfSimpleExpr* in_source);
    // QString MakePList() const;
    void EmptyFilter();
    QString GetExpression() const;
    // the destructor
    ~jfListExprFilterBase();
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    QString ModeToString(jfListMatchMode in_match_mode) const;
    jfListMatchMode StringToMode(const QString& mode_source) const;
    // match methods
    bool MergeMatch(const QStringList& to_match) const;
    bool ItemwiseMatchAll(const QStringList& to_match) const;
    bool ItemwiseMatchAny(const QStringList& to_match) const;
    int ItemwiseMatchCount(const QStringList& to_match) const;
    bool InternalMatch(const QStringList& to_match) const;
    // i/o methods
    void CoreCopy(const jfListExprFilterBase& source);
    // private data
    jfSimpleExpr* parsed_expression;
    jfListMatchMode match_mode;
};
// ==========================================================================
extern const jfFilterTypeMeta CHARACTER_LIST_FILTER_INFO;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// an expression list filter for characters
class jfCharListExprFilter : public jfListExprFilterBase {
  public:
    jfCharListExprFilter(const QString& filter_name);
    jfCharListExprFilter(QString&& filter_name);
    jfCharListExprFilter(const jfCharListExprFilter& source);
    jfCharListExprFilter(const QString& filter_name, jfListMatchMode in_match_mode, jfSimpleExpr* in_source);
    virtual jfCharListExprFilter* Copy() const;
    virtual jfFilterBase* GenCopy() const override;
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};

//***************************************************************************
