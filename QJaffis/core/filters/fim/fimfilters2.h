/******************************************************************************
Name    :   fimfilters2.h
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net
Created :   July 12, 2015
Conversion to Qt Started Aug 4, 2015
Updated :   July 27, 2023 (rebasing)
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

extern const jfFilterTypeMeta FIM_SHORT_DESC_FILTER_INFO;

class jfFIM_ShortDesc_ExprFilter : public jfSimpleExpFilterCore {
  public:
    jfFIM_ShortDesc_ExprFilter(const QString& filter_name);
    jfFIM_ShortDesc_ExprFilter(QString&& filter_name);
    jfFIM_ShortDesc_ExprFilter(const jfFIM_ShortDesc_ExprFilter& source);
    jfFIM_ShortDesc_ExprFilter(const QString& filter_name, jfSimpleExpr* in_source);

    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFIM_ShortDesc_ExprFilter* Copy() const;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};

//=============================================================================
/* Similar to the previous filter, but matches against either the short description
*or* the usual description */
extern const jfFilterTypeMeta FIM_DUAL_DESC_FILTER_INFO;

class jfFIM_DualDesc_ExprFilter : public jfSimpleExpFilterCore {
  public:
    jfFIM_DualDesc_ExprFilter(const QString& filter_name);
    jfFIM_DualDesc_ExprFilter(QString&& filter_name);
    jfFIM_DualDesc_ExprFilter(const jfFIM_DualDesc_ExprFilter& source);
    jfFIM_DualDesc_ExprFilter(const QString& filter_name, jfSimpleExpr* in_source);

    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFIM_DualDesc_ExprFilter* Copy() const;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//=============================================================================
extern const jfFilterTypeMeta FIM_GROUP_STORY_COUNT_FILTER_INFO;

// story count filter for FIM Group Search
class jfFIMGroupSCFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfFIMGroupSCFilter(const QString& name);
    jfFIMGroupSCFilter(const QString& name, size_t inmin, size_t inmax);
    // redefined virtual methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//-----------------------------------------------------------------------
extern const jfFilterTypeMeta FIM_GROUP_MEMBER_COUNT_FILTER_INFO;

// member count filter for FIM Group Search
class jfFIMGroupMCFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfFIMGroupMCFilter(const QString& name);
    jfFIMGroupMCFilter(const QString& name, size_t inmin, size_t inmax);
    // redefined virtual methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
/*****************************************************************************/
