/******************************************************************************
Name    :   ao3_special2.h
Author  :   John Q Metro
Purpose :   Declares some interface panels for some AO3 Filters
Created :   October 6, 2012
Conversion to Qt Started October 19, 2013
Updated :   February 25, 2023
******************************************************************************/
#ifndef AO3_SPECIAL2_H_INCLUDED
#define AO3_SPECIAL2_H_INCLUDED
#endif // AO3_SPECIAL2_H_INCLUDED
//---------------------------------
#ifndef AO3_SPECIALS2_H_INCLUDED
  #include "../../../core/filters/ao3/ao3_specials2.h"
#endif // AO3_SPECIALS2_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../../controls/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef CHARO_EDITOR_H_INCLUDED
  #include "../../controls/charo_editor.h"
#endif // CHARO_EDITOR_H_INCLUDED
/*****************************************************************************/
// ao3 ratings filter editor
class jfAO3_WarnFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfAO3_WarnFilterEditor(const jfAO3WarnFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    virtual bool GeneralCheck(const jfFilterMap* infmap) const override;
  protected:
    jfCharCheckBoxGroup* insert_panel;
};
/*****************************************************************************/
