//***************************************************************************
// Name:        ao3_list_percent_filter_editor.h
// Author :     John Q Metro
// Purpose :    Editor for a filter which matches an expression against the extra
// tags, and passes if the percetage of matches is greater than the specified amount.
// Created:     July 24, 2022
// Updated:     April 16, 2023
//***************************************************************************
#ifndef AO3_LIST_PERCENT_FILTER_EDITOR_H
    #define AO3_LIST_PERCENT_FILTER_EDITOR_H
#endif // AO3_LIST_PERCENT_FILTER_EDITOR_H
//-----------------------------------------------
#ifndef AO3_PERCENT_TAGS_FILTER_H
    #include "../../../core/filters/ao3/ao3_percent_tags_filter.h"
#endif // AO3_PERCENT_TAGS_FILTER_H

#include "../tagfilter/tag_match_filter_edits.h"


//***************************************************************************
/* a wrapper for jfSimpleExprEdit that is for an character filter */
class jfAO3PercentExpressionTagFilterEditor : public jfFilteredPercentTagMatchFilterEditor {
  public:
    // the default constructor
    jfAO3PercentExpressionTagFilterEditor(QWidget* parent);
    jfAO3PercentExpressionTagFilterEditor(const jfAO3ExtraTagsPercentFilter* infilt, QWidget* parent);
  protected:
    // internal methods
    virtual jfMatchFilteredPercentTagFilterBase* MakeTypedFilter(const jfGeneralTagListsGroup* in_source_list);
};
//***************************************************************************

