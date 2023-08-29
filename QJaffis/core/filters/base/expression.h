/////////////////////////////////////////////////////////////////////////////
// Name:        expression.h
// Author :     John Q Metro
// Purpose :    Declares the summary and title general expression filter
// Created:     31.12.06
// Conversion to QT Started April 8, 2013
// Updated:     April 3, 2023 (Rebasing)
/////////////////////////////////////////////////////////////////////////////
// headers
// ----------------------------------------------------------------------------
#ifndef EXPRESSION_H
  #define EXPRESSION_H
#endif // EXPRESSION_H
//--------------------------------------------
#ifndef JFEXPRSTRUCTS
  #include "../../express/expr_structs.h"
#endif

#include "basefilter.h"
#include "../filtermap.h"

//****************************************************************************
extern size_t efcounter;
extern const jfFilterTypeMeta EXPRESSION_FILTER_INFO;
//----------------------------------------------------------------------------
enum class jfExpVerifiedState {
    UNCHECKED, VERIFIED, FAILED
};

//-----------------------------------------------------------------------------
// the expression filter: a complicated filter indeed, which holds a boolean expression
// the operands of the expression, which can evaluate to true or false, are
// either string matching/searching, or other filters, embedded or linked.
class jfExpressionFilter : public jfFilterBase {
  public:
    // QString parse_error;
    // the constructors
    jfExpressionFilter(const QString& filter_name);
    jfExpressionFilter(QString&& filter_name);
    jfExpressionFilter(const jfExpressionFilter& source, const jfFilterMap* infmap_link);
    // redefined virtual functions
    virtual jfFilterBase* GenCopy() const override;
    virtual bool IsEmpty() const override;
    bool SetFiltermapLink(const jfFilterMap* infmap_link);
    virtual QString ToString() const override;
    virtual jfExpressionFilter* Copy() const;
    // basic constants
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;

    // modifying and checking the contents
    virtual bool VerifyNames(jfNameVerif* nameobj, bool notinmap);
    virtual bool VerifyNames(jfNameVerif* nameobj, const QString& inmapname);
    const jfExpVerifiedState& VerifiedState() const;
    bool CopyLoad();
    QString MakePList() const;
    // special functions
    bool FromString(const QString& sourcedata, QString& error_out);
    void EmptyFilter();
    // the destructor
    ~jfExpressionFilter();
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // private methods
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    // private data
    QString srcstring;
    jfElemArray* parsedinfo;
    bool loaded;
    jfExpVerifiedState verify_state;
    // filter map link
    const jfFilterMap* fmap_link;
};

///////////////////////////////////////////////////////////////////////////////

