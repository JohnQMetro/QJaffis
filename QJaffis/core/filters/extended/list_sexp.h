//***************************************************************************
// Name:        list_sexp.h
// Author :     John Q Metro
// Purpose :    String expression based filters for lists of strings
// Created:     February 26, 2023
// Updated:     March 4, 2023
//***************************************************************************
#ifndef LIST_SEXP_FILTER_H
  #define LIST_SEXP_FILTER_H
#endif // LIST_SEXP_FILTER_H

#pragma once
//------------------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif
//***************************************************************************
// the matching mode of the list expression matcher
enum class jfListMatchMode {
    MATCH_MERGED, MATCH_ONE, MATCH_ALL, MATCH_75P
};
// ---------------------------------------------------------------

class jfListExprFilterBase : public jfBaseFilter {
  public:
    // the constructors
    jfListExprFilterBase();
    jfListExprFilterBase(const QString& sourcedata);
    jfListExprFilterBase(jfListMatchMode in_match_mode, jfSimpleExpr* in_source);
    jfListExprFilterBase(const jfListExprFilterBase& source);

    // redefined virtual functions
    virtual bool isEmpty() const;
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;

    // extra methods
    jfListMatchMode GetMatchMode() const;
    void SetMatchMode(jfListMatchMode in_match_mode);
    bool FromExpr(jfSimpleExpr* in_source);
    QString MakePList() const;
    void EmptyFilter();
    QString GetExpression() const;
    // returns a general filter type
    virtual size_t GetFilType() const;
    // the destructor
    ~jfListExprFilterBase();
  protected:
    QString ModeToString(jfListMatchMode in_match_mode) const;
    jfListMatchMode StringToMode(const QString& mode_source) const;
    // match methods
    bool MergeMatch(const QStringList& to_match) const;
    bool ItemwiseMatchAll(const QStringList& to_match) const;
    bool ItemwiseMatchAny(const QStringList& to_match) const;
    int ItemwiseMatchCount(const QStringList& to_match) const;
    bool InternalMatch(const QStringList& to_match) const;
    // i/o methods
    virtual size_t ExtraLines() const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    void CoreCopy(const jfListExprFilterBase& source);
    // private data
    jfSimpleExpr* parsed_expression;
    jfListMatchMode match_mode;
};
// ==========================================================================
// an expression list filter for characters
class jfCharListExprFilter : public jfListExprFilterBase {
  public:
    jfCharListExprFilter();
    jfCharListExprFilter(const jfCharListExprFilter& source);
    jfCharListExprFilter(jfListMatchMode in_match_mode, jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfCharListExprFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};

//***************************************************************************
