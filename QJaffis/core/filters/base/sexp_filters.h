//***************************************************************************
// Name:        sexp_filters.h
// Author :     John Q Metro
// Purpose :    Simple expression filters that work on specific string data
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     April 2, 2023 (Rebasing)
//***************************************************************************
#pragma once

#include "basefilter.h"

#ifndef JFEXPRSTRUCTS
  #include "../../express/expr_structs.h"
#endif
//***************************************************************************
/* the result of a simple expression editor, the filter class base is abstract,
this is to avoid having a separate editor for each simple expression filter subtype,
since the only real difference is what they match. */
class jfSimpleExpr {
    friend class jfAO3FandomFilter;
  public:
    QString parse_error;
    // constructors
    jfSimpleExpr();
    jfSimpleExpr(const QString& sourcedata);
    // data setting methods
    bool FromString(const QString& sourcedata);
    // other methods
    bool InternalMatch(const QString& incheck) const;
    jfSimpleExpr* Copy() const;
    QString MakePList() const;
    // info
    bool IsValid() const;
    bool isEmpty() const;
    QString GetSrc() const;
    // destructors
    ~jfSimpleExpr();
  protected:
    QString srcstring;
    jfElemArray* parsedinfo;
    bool valid;
    bool isempty;
};
//==========================================================================
/* The simple expression filter: Holds a boolean expression.
 the operands of the expression, which can evaluate to true or false, are
 for either string matching/searching. */
class jfSimpleExpFilterCore : public jfFilterBase {
  public:
    // the constructors
    jfSimpleExpFilterCore(const QString& filter_name);
    jfSimpleExpFilterCore(QString&& filter_name);
    jfSimpleExpFilterCore(const QString& filter_name, const QString& sourcedata);
    jfSimpleExpFilterCore(const QString& filter_name, jfSimpleExpr* in_source);
    // redefined virtual functions
    virtual bool IsEmpty() const override;
    virtual QString ToString() const override;
    // extra methods
    bool FromExpr(jfSimpleExpr* in_source);
    // QString MakePList() const;
    void EmptyFilter();
    // the destructor
    ~jfSimpleExpFilterCore();
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override final;
    // private methods
    bool InternalMatch(const QString& incheck) const;
    void CoreCopy(const jfSimpleExpFilterCore& source);
    // private data
    jfSimpleExpr* parsed_expression;
};
//***************************************************************************
