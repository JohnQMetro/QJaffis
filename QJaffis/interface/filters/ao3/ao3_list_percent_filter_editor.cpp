//***************************************************************************
// Name:        ao3_list_percent_filter_editor.cpp
// Author :     John Q Metro
// Purpose :    Editor for a filter which matches an expression against the extra
// tags, and passes if the percetage of matches is greater than the specified amount.
// Created:     July 24, 2022
// Updated:     April 16, 2023
//***************************************************************************
#ifndef AO3_LIST_PERCENT_FILTER_EDITOR_H
    #include "ao3_list_percent_filter_editor.h"
#endif // AO3_LIST_PERCENT_FILTER_EDITOR_H
//-----------------------------------------------

//***************************************************************************
/* a wrapper for jfSimpleExprEdit that is for an character filter */

// the default constructor
jfAO3PercentExpressionTagFilterEditor::jfAO3PercentExpressionTagFilterEditor(QWidget* parent):
    jfFilteredPercentTagMatchFilterEditor(taglistings::typeListsGroup,parent) {

}

jfAO3PercentExpressionTagFilterEditor::jfAO3PercentExpressionTagFilterEditor(
        const jfAO3ExtraTagsPercentFilter* infilt, QWidget* parent):
        jfFilteredPercentTagMatchFilterEditor(infilt,parent) {

}
// --------------------------------------------------------------------
// internal methods
jfMatchFilteredPercentTagFilterBase* jfAO3PercentExpressionTagFilterEditor::MakeTypedFilter(const jfGeneralTagListsGroup* in_source_list) {
    return new jfAO3ExtraTagsPercentFilter("(dummy name)");
}

//***************************************************************************

