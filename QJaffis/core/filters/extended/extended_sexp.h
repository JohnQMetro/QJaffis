//***************************************************************************
// Name:        extended_sexp.h
// Author :     John Q Metro
// Purpose :    Specializations of string expressions for particular strings
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     July 8, 2016 (taken from sexp_filters.h)
//***************************************************************************
#ifndef EXTENDED_SEXP_FILTER_H
  #define EXTENDED_SEXP_FILTER_H
#endif // EXTENDED_SEXP_FILTER_H
//------------------------------------------
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
//***************************************************************************

// a simple expression filter that matches against author data
class jfAuthExprFilter : public jfSimpleExpFilterCore {
  public:
    jfAuthExprFilter();
    jfAuthExprFilter(const jfAuthExprFilter& source);
    jfAuthExprFilter(jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfAuthExprFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};

//===========================================================================
// a simple expression filter that matches against author data
class jfCharExprFilter : public jfSimpleExpFilterCore {
  public:
    jfCharExprFilter();
    jfCharExprFilter(const jfCharExprFilter& source);
    jfCharExprFilter(jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfCharExprFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};
//===========================================================================
// a simple expression filter that matches against the language name
class jfLanguageExprFilter : public jfSimpleExpFilterCore {
  public:
    jfLanguageExprFilter();
    jfLanguageExprFilter(const jfLanguageExprFilter& source);
    jfLanguageExprFilter(jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfLanguageExprFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};
//***************************************************************************
