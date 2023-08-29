//***************************************************************************
// Name:        ao3_stringfilters.h
// Author :     John Q Metro
// Purpose :    Simple expression filters specific to archiveofourown.org
// Created:     September 7, 2012
// Conversion to Qt Started Oct 3, 2013
// Updated:     April 15, 2023 (rebasing)
//***************************************************************************
#ifndef AO3_STRINGFILTERS_H_INCLUDED
#define AO3_STRINGFILTERS_H_INCLUDED
#endif // AO3_STRINGFILTERS_H_INCLUDED
//--------------------------------------

#include "../base/sexp_filters.h"
#include "../base/basefilter.h"

//***************************************************************************
extern const jfFilterTypeMeta EXTRATAG_EXPR_FILTER_INFO;
// -----------------------------------------------------
// Filtering the extra tags
class jfExtraTagFilter : public jfSimpleExpFilterCore {
  public:

    jfExtraTagFilter(const QString& filter_name);
    jfExtraTagFilter(QString&& filter_name);
    jfExtraTagFilter(const jfExtraTagFilter& source);
    jfExtraTagFilter(const QString& filter_name, jfSimpleExpr* in_source);

    virtual jfExtraTagFilter* Copy() const;
    virtual jfFilterBase* GenCopy() const override;
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;

  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};

//***************************************************************************
