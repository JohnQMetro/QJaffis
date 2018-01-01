/******************************************************************************
Name    :   sfilt_edit.h
Author  :   John Q Metro
Purpose :   Filter editors (wraping the core). Simple expression & more
Created :   December 5, 2010
Conversion to Qt Started October 8, 2013
Updated :   July 8, 2016 (removing the more specific filters to a new file).
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
    jfSimpleSFilterEditor(const jfFilterMap* infmap, const jfSimpleExpFilterCore* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval) = 0;
    // internal data
    jfSimpleExprEdit* insert_panel;
    const jfSimpleExpFilterCore* typed_fpointer;
};
//****************************************************************************
