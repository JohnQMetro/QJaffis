/******************************************************************************
Name    :   fimfilters2.h
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 12, 2015
Conversion to Qt Started Aug 4, 2015
Updated :   July 12, 2015
******************************************************************************/
#ifndef FIMFILTERS2_H_INCLUDED
#define FIMFILTERS2_H_INCLUDED
#endif // FIMFILTERS2_H_INCLUDED
//----------------------------------------
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
#ifndef MINMAX_FILTER_H
  #include "../extended/minmax.h"
#endif // MINMAX_FILTER_H
/*****************************************************************************/

/* a simple expression filter that matches against the shorter, separate summary that FIM
shows for compact lists, and also at the top of chapter pages (but not the fic index page). */

class jfFIM_ShortDesc_ExprFilter : public jfSimpleExpFilterCore {
  public:
    jfFIM_ShortDesc_ExprFilter();
    jfFIM_ShortDesc_ExprFilter(const jfFIM_ShortDesc_ExprFilter& source);
    jfFIM_ShortDesc_ExprFilter(jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfFIM_ShortDesc_ExprFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};

//=============================================================================
/* Similar to the previous filter, but matches against either the short description
*or* the usual description */

class jfFIM_DualDesc_ExprFilter : public jfSimpleExpFilterCore {
  public:
    jfFIM_DualDesc_ExprFilter();
    jfFIM_DualDesc_ExprFilter(const jfFIM_DualDesc_ExprFilter& source);
    jfFIM_DualDesc_ExprFilter(jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfFIM_DualDesc_ExprFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};
//=============================================================================
// story count filter for FIM Group Search
class jfFIMGroupSCFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfFIMGroupSCFilter();
    jfFIMGroupSCFilter(size_t inmin, size_t inmax);
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};
//-----------------------------------------------------------------------
// member count filter for FIM Group Search
class jfFIMGroupMCFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfFIMGroupMCFilter();
    jfFIMGroupMCFilter(size_t inmin, size_t inmax);
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};
/*****************************************************************************/
