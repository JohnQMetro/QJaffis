//***************************************************************************
// Name:        sexp_filters.h
// Author :     John Q Metro
// Purpose :    Simple expression filters that work on specific string data
// Created:     November 30, 2010
// Conversion to Qt Started September 26, 2013
// Updated:     August 22, 2012
//***************************************************************************
#ifndef SEXP_FILTERS_H_INCLUDED
#define SEXP_FILTERS_H_INCLUDED
#endif // SEXP_FILTERS_H_INCLUDED
//-------------------------------------------------------------------------
#ifndef JFBASEFILTER
  #include "filterbase.h"
#endif
#ifndef JFEXPRSTRUCTS
  #include "../../express/expr_structs.h"
#endif
//***************************************************************************
/* the result of a simple expression editor, the filter class base is abstract,
this is to avoid having a separate editor for each simple expression filter subtype,
since the only real difference is what they match. */
class jfSimpleExpr {
    friend class jfSimpleExpFilterCore;
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
class jfSimpleExpFilterCore : public jfBaseFilter {
  public:
    QString parse_error;
    // the constructors
    jfSimpleExpFilterCore();
    jfSimpleExpFilterCore(const QString& sourcedata);
    jfSimpleExpFilterCore(jfSimpleExpr* in_source);
    // redefined virtual functions
    virtual bool isEmpty() const;
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // extra methods
    bool FromExpr(jfSimpleExpr* in_source);
    QString MakePList() const;
    void EmptyFilter();
    // returns a general filter type
    virtual size_t GetFilType() const;
    // the destructor
    ~jfSimpleExpFilterCore();
  protected:
    // private methods
    bool InternalMatch(const QString& incheck) const;
    virtual void SetValid();
    virtual size_t ExtraLines() const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    void CoreCopy(const jfSimpleExpFilterCore& source);
    // private data
    QString srcstring;
    jfElemArray* parsedinfo;
    bool loaded;
};
//***************************************************************************
