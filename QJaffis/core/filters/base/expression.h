/////////////////////////////////////////////////////////////////////////////
// Name:        expression.h
// Author :     John Q Metro
// Purpose :    Declares the summary and title general expression filter
// Created:     31.12.06
// Conversion to QT Started April 8, 2013
// Updated:     July 8, 2016 (rename file and modify header comments)
/////////////////////////////////////////////////////////////////////////////
// headers
// ----------------------------------------------------------------------------
#ifndef EXPRESSION_H
  #define EXPRESSION_H
#endif // EXPRESSION_H
//--------------------------------------------
#ifndef JFBASEFILTER
  #include "filterbase.h"
#endif
#ifndef JFEXPRSTRUCTS
  #include "../../express/expr_structs.h"
#endif

//****************************************************************************
extern size_t efcounter;
extern const QString EXP_FIL_TAG;
//-----------------------------------------------------------------------------
// the expression filter: a complicated filter indeed, which holds a boolean expression
// the operands of the expression, which can evaluate to true or false, are
// either string matching/searching, or other filters, embedded or linked.
class jfExpressionFilter : public jfBaseFilter {
  public:
    QString parse_error;
    // the constructors
    jfExpressionFilter();
    jfExpressionFilter(const jfExpressionFilter& source,jfFilterMap* infmap_link);
    // redefined virtual functions
    virtual bool isEmpty() const;
    bool SetFiltermapLink(jfFilterMap* infmap_link);
    bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    virtual jfExpressionFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    // basic constants
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    // modifying and checking the contents
    virtual bool VerifyNames(jfNameVerif* nameobj, bool notinmap);
    virtual bool VerifyNames(jfNameVerif* nameobj, const QString& inmapname);
    bool CopyLoad();
    QString MakePList() const;
    // special functions
    void EmptyFilter();
    // the destructor
    ~jfExpressionFilter();
  protected:
    // private methods
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    virtual void SetValid();
    virtual size_t ExtraLines() const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // private data
    QString srcstring;
    jfElemArray* parsedinfo;
    bool loaded;
    // filter map link
    jfFilterMap* fmap_link;
};

///////////////////////////////////////////////////////////////////////////////

