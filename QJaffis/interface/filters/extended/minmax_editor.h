/******************************************************************************
Name    :   minmax_editor.h
Author  :   John Q Metro
Purpose :   Declares some editors for numeric range filters
Created :   June 8, 2016 (Taken from other sources)
Updated :   June 8, 2016
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
    jfZeroToMaxFilterEditor(QString typname, size_t xmax, const jfFilterMap* infmap, const jfMinMaxUFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  protected:
    virtual jfMinMaxUFilter* MakeTypedMinMax() const = 0;
    jfZeroToMaxEditor* insert_panel;
};
//===============================================================
// the actual word count filter editor, a specialization of jfZeroToMaxFilterEditor
class jfWCFilterEditor : public jfZeroToMaxFilterEditor {
  public:
    // the default constructor
    jfWCFilterEditor(const jfFilterMap* infmap, const jfWordCountFilter* infilt, QWidget* parent = NULL);
  private:
    virtual jfMinMaxUFilter* MakeTypedMinMax() const;
};
/*****************************************************************************/
