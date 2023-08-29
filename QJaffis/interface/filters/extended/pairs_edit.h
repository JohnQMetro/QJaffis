//***************************************************************************
// Name:        pairs_edit.h
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters editing, various panels.
// Created:     January 2, 2011
// Conversion to Qt Started October 18, 2013
// Updated:     August 19, 2023
//***************************************************************************
#ifndef PAIRS_EDIT_H_INCLUDED
#define PAIRS_EDIT_H_INCLUDED
#endif // PAIRS_EDIT_H_INCLUDED
//--------------------------------------------------------------------------
#ifndef PAIRF_EDITOR_H_INCLUDED
  #include "pairf_editor.h"
#endif // PAIRF_EDITOR_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
//-----------------------------------------------------
//***************************************************************************
/* a wrapper for jfPairFilEditPanelSingle that is an editor for an single pair filter */
class jfPairFS_Editor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfPairFS_Editor(const jfPairFilterSingle* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    // checking
    virtual bool GeneralCheck(const jfFilterMap* filter_group) const override;
  private:
    jfPairFilEditPanelSingle* insert_panel;
};
//=============================================================================
/* a wrapper for jfPairFilEditPanelList that is an editor for a name paired against a list */
class jfPairFL_Editor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfPairFL_Editor(const jfPairFilterList* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    // checking
    virtual bool GeneralCheck(const jfFilterMap* filter_group) const override;
  private:
    jfPairFilEditPanelList* insert_panel;
};
//=============================================================================
/* a wrapper for jfPairFilEditPanelMulti that is an editor for an all combination list of names */
class jfPairFM_Editor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfPairFM_Editor(const jfPairFilterMultiple* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    // checking
    virtual bool GeneralCheck(const jfFilterMap* filter_group) const override;
  private:
    jfPairFilEditPanelMulti* insert_panel;
};
//***************************************************************************
