/******************************************************************************
Name    :   minmax_editor.h
Author  :   John Q Metro
Purpose :   Declares some editors for numeric range filters
Created :   June 8, 2016 (Taken from other sources)
Updated :   April 15, 2023
******************************************************************************/
#ifndef MINMAX_EDITOR_H
  #define MINMAX_EDITOR_H
#endif // MINMAX_EDITOR_H
//-----------------------------------
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
/*****************************************************************************/
// A Base editor for most jfMinMaxUFilter type filters
class jfZeroToMaxFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfZeroToMaxFilterEditor(QString typname, size_t xmax, const jfMinMaxUFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override final;
    virtual jfFilterBase* GetFilter() override;
    // checking
    bool GeneralCheck(const jfFilterMap* filter_group) const override;
  protected:
    virtual jfMinMaxUFilter* MakeTypedMinMax() const = 0;
    jfZeroToMaxEditor* insert_panel;
};
//===============================================================
// the actual word count filter editor, a specialization of jfZeroToMaxFilterEditor
class jfWCFilterEditor : public jfZeroToMaxFilterEditor {
  public:
    // the default constructor
    jfWCFilterEditor(const jfWordCountFilter* infilt, QWidget* parent = NULL);
  private:
    virtual jfMinMaxUFilter* MakeTypedMinMax() const;
};
/*****************************************************************************/
