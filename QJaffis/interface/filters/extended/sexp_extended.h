/******************************************************************************
Name    :   sexp_extended.h
Author  :   John Q Metro
Purpose :   Extended simple expression filter editors
Created :   July 8, 2016 (split from sfilt_edit.h)
Updated :   July 8, 2016
******************************************************************************/
#ifndef SEXP_EXTENDED_H_INCLUDED
#define SEXP_EXTENDED_H_INCLUDED
#endif // SEXP_EXTENDED_H_INCLUDED
//-------------------------------------------------
#ifndef SFILT_EDIT_H_INCLUDED
  #include "../base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
#ifndef EXTENDED_SEXP_FILTER_H
  #include "../../../core/filters/extended/extended_sexp.h"
#endif // EXTENDED_SEXP_FILTER_H
//****************************************************************************
/* a wrapper for jfSimpleExprEdit that is for an author filter */
class jfAuthorFilterEditor : public jfSimpleSFilterEditor {
  public:
    // the default constructor
    jfAuthorFilterEditor(const jfFilterMap* infmap, const jfAuthExprFilter* infilt, QWidget* parent = NULL);
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval);
};
//================================================================================
/* a wrapper for jfSimpleExprEdit that is for an character filter */
class jfCharFilterEditor : public jfSimpleSFilterEditor {
  public:
    // the default constructor
    jfCharFilterEditor(const jfFilterMap* infmap, const jfCharExprFilter* infilt, QWidget* parent = NULL);
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval);
};
//================================================================================
/* a wrapper for jfSimpleExprEdit that is for a language filter */
class jfLanguageFilterEditor : public jfSimpleSFilterEditor {
  public:
    // the default constructor
    jfLanguageFilterEditor(const jfFilterMap* infmap, const jfLanguageExprFilter* infilt, QWidget* parent = NULL);
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval);
};


//****************************************************************************
