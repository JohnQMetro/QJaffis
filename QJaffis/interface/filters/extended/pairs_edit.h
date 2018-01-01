//***************************************************************************
// Name:        pairs_edit.h
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters editing, various panels.
// Created:     January 2, 2011
// Conversion to Qt Started October 18, 2013
// Updated:     January 2, 2011
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
    jfPairFS_Editor(const jfFilterMap* infmap,const jfPairFilterSingle* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    virtual bool GeneralCheck() const;
  private:
    jfPairFilEditPanelSingle* insert_panel;
};
//=============================================================================
/* a wrapper for jfPairFilEditPanelList that is an editor for a name paired against a list */
class jfPairFL_Editor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfPairFL_Editor(const jfFilterMap* infmap,const jfPairFilterList* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    virtual bool GeneralCheck() const;
  private:
    jfPairFilEditPanelList* insert_panel;
};
//=============================================================================
/* a wrapper for jfPairFilEditPanelMulti that is an editor for an all combination list of names */
class jfPairFM_Editor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfPairFM_Editor(const jfFilterMap* infmap,const jfPairFilterMultiple* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    virtual bool GeneralCheck() const;
  private:
    jfPairFilEditPanelMulti* insert_panel;
};
//***************************************************************************
