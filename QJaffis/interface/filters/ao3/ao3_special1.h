/******************************************************************************
Name    :   ao3_special1.h
Author  :   John Q Metro
Purpose :   Declares some base class for filter diting
Created :   September 10, 2012
Conversion to Qt Started October 18, 2013
Updated :   August 12, 2023
******************************************************************************/
#ifndef AO3_SPECIAL1_H_INCLUDED
#define AO3_SPECIAL1_H_INCLUDED
#endif // AO3_SPECIAL1_H_INCLUDED
//------------------------------------
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef CHARO_EDITOR_H_INCLUDED
  #include "../../controls/charo_editor.h"
#endif // CHARO_EDITOR_H_INCLUDED
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "../../../core/filters/ao3/ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
/*****************************************************************************/
extern const QString ratinglist[];
extern const QString orientlist[];
//=====================================================================
// ao3 ratings filter editor
class jfAO3_RatingFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfAO3_RatingFilterEditor(const jfAO3RatingFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    virtual bool GeneralCheck(const jfFilterMap* infmap) const override;
  protected:
    jfCharCheckBoxGroup* insert_panel;
};
//======================================================================

// tag filter using 'orientations' ('F/F', 'Multi', etc)
class jfAO3_OrientFilterEditor : public jfTagFilterEditor {
  public:
    // the default constructor
    jfAO3_OrientFilterEditor(const jfAO3OrientationFilter* infilt, QWidget* parent = NULL);
    // extra methods
    virtual jfTagFilterCore* GetTagFilter();
    virtual bool isListLong() const;
    virtual void LoadBlank();
};
/*****************************************************************************/
