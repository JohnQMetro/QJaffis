/******************************************************************************
Name    :   sfilt_edit.h
Author  :   John Q Metro
Purpose :   Filter editors (wraping the core). Simple expression & more
Created :   December 5, 2010
Conversion to Qt Started October 8, 2013
Updated :   April 9, 2023 (rebasing)
******************************************************************************/
#ifndef SFILT_EDIT_H_INCLUDED
#define SFILT_EDIT_H_INCLUDED
#endif // SFILT_EDIT_H_INCLUDED
//+++++++++++++++++++++++++++++++++++
#ifndef BFILT_EDIT_H_INCLUDED
  #include "bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef SEXPR_EDIT_H_INCLUDED
  #include "sexpr_edit.h"
#endif // SEXPR_EDIT_H_INCLUDED
//****************************************************************************
// core wrapper for jfSimpleExprEdit
class jfSimpleSFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfSimpleSFilterEditor(const jfSimpleExpFilterCore* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter);
    virtual jfFilterBase* GetFilter();
    // checking
    bool GeneralCheck(const jfFilterMap* filter_group) const;
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval) = 0;
    // internal data
    jfSimpleExprEdit* insert_panel;
    const jfSimpleExpFilterCore* typed_fpointer;
};
//****************************************************************************
