//***************************************************************************
// Name:        ao3_percent_tags_filter.h
// Author :     John Q Metro
// Purpose :    Matches individual extra tags against a tag set and an expression,
//              passing if the percentags of tags that match exceed the defined
//              percentage.
// Created:     July 23, 2022
// Updated:     April 8, 2023 (Rebasing)
//***************************************************************************
#ifndef AO3_PERCENT_TAGS_FILTER_H
#define AO3_PERCENT_TAGS_FILTER_H
#endif // AO3_PERCENT_TAGS_FILTER_H
//-------------------------------------------------------------------------
#include "../../tagfilter/base_tagfilters.h"
//***************************************************************************
extern const jfFilterTypeMeta AO3_EXTRATAGS_PERCENT_FILTER_INFO;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/* We apply a boolean expression list items, counting matches.  */
class jfAO3ExtraTagsPercentFilter : public jfMatchFilteredPercentTagFilterBase {
  public:
    // the constructors
    jfAO3ExtraTagsPercentFilter(const QString& filter_name);

    // implemted virtual methods
    virtual jfFilterBase* GenCopy() const override;
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;

    // extra method
    virtual jfAO3ExtraTagsPercentFilter* Copy() const;

  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};

//***************************************************************************


