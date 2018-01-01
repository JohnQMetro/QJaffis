/******************************************************************************
Name    :   sexp_extended.cpp
Author  :   John Q Metro
Purpose :   Extended simple expression filter editors
Created :   July 8, 2016 (split from sfilt_edit.h)
Updated :   July 8, 2016
******************************************************************************/
#ifndef SEXP_EXTENDED_H_INCLUDED
  #include "sexp_extended.h"
#endif // SEXP_EXTENDED_H_INCLUDED
//-------------------------------------------------
/******************************************************************************/
// the default constructor
jfAuthorFilterEditor::jfAuthorFilterEditor(const jfFilterMap* infmap, const jfAuthExprFilter* infilt,
      QWidget* parent):jfSimpleSFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfAuthorFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfAuthExprFilter* result = new jfAuthExprFilter(inpval);
  return result;
}
//================================================================================
// the default constructor
jfCharFilterEditor::jfCharFilterEditor(const jfFilterMap* infmap, const jfCharExprFilter* infilt,
      QWidget* parent):jfSimpleSFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfCharFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfCharExprFilter* result = new jfCharExprFilter(inpval);
  return result;
}
//================================================================================
// the default constructor
jfLanguageFilterEditor::jfLanguageFilterEditor(const jfFilterMap* infmap,
    const jfLanguageExprFilter* infilt, QWidget* parent):jfSimpleSFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfLanguageFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfLanguageExprFilter* result = new jfLanguageExprFilter(inpval);
  return result;
}
/*****************************************************************************/
