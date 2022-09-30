/******************************************************************************
Name    :   tag_match_filter_edits.h
Author  :   John Q Metro
Purpose :   Filter edit widgets for tag matchers
Created :   August 27, 2022
Updated :   August 27, 2022
******************************************************************************/
#pragma once

#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED

#include "multimatch.h"
#include "filter_match.h"
#include "../../../core/tagfilter/base_tagfilters.h"

/*****************************************************************************/
// core wrapper for jfPercentExprEdit
class jfPercentTagMatchFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfPercentTagMatchFilterEditor(const jfFilterMap* infmap, const jfMatchPercentTagFilterBase* infilt, QWidget* parent = NULL);
    jfPercentTagMatchFilterEditor(const jfFilterMap* infmap, const jfGeneralTagListsGroup* in_source_list, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  protected:
    // internal methods
    virtual jfMatchPercentTagFilterBase* MakeTypedFilter(const jfGeneralTagListsGroup* in_source_list) = 0;
    // internal data
    jfPercentExprEdit* insert_panel;
    const jfMatchPercentTagFilterBase* typed_fpointer;

    const jfGeneralTagListsGroup* source_list;
};
/*****************************************************************************/
// core wrapper for jfFilteredTagMatcherEdit
class jfFilteredPercentTagMatchFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfFilteredPercentTagMatchFilterEditor(const jfFilterMap* infmap, const jfMatchFilteredPercentTagFilterBase* infilt, QWidget* parent = NULL);
    jfFilteredPercentTagMatchFilterEditor(const jfFilterMap* infmap, const jfGeneralTagListsGroup* in_source_list, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  protected:
    // internal methods
    virtual jfMatchFilteredPercentTagFilterBase* MakeTypedFilter(const jfGeneralTagListsGroup* in_source_list) = 0;
    // internal data
    jfFilteredTagMatcherEdit* insert_panel;
    const jfMatchFilteredPercentTagFilterBase* typed_fpointer;

    const jfGeneralTagListsGroup* source_list;
};
/*****************************************************************************/
