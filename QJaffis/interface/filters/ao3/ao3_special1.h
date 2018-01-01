/******************************************************************************
Name    :   ao3_special1.h
Author  :   John Q Metro
Purpose :   Declares some base class for filter diting
Created :   September 10, 2012
Conversion to Qt Started October 18, 2013
Updated :   September 10, 2012
******************************************************************************/
#ifndef AO3_SPECIAL1_H_INCLUDED
#define AO3_SPECIAL1_H_INCLUDED
#endif // AO3_SPECIAL1_H_INCLUDED
//------------------------------------
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef CHARO_EDITOR_H_INCLUDED
  #include "../../misc/charo_editor.h"
#endif // CHARO_EDITOR_H_INCLUDED
/*****************************************************************************/
extern const QString ratinglist[];
extern const QString orientlist[];
//=====================================================================
// ao3 ratings filter editor
class jfAO3_RatingFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfAO3_RatingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfCharCheckBoxGroup* insert_panel;
};
//======================================================================
// ao3 ratings filter editor
class jfAO3_OrientFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfAO3_OrientFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfCharCheckBoxGroup* insert_panel;
};
/*****************************************************************************/
