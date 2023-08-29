/******************************************************************************
Name    :   sexp_extended.cpp
Author  :   John Q Metro
Purpose :   Extended simple expression filter editors
Created :   July 8, 2016 (split from sfilt_edit.h)
Updated :   August 12, 2023
******************************************************************************/
#ifndef SEXP_EXTENDED_H_INCLUDED
  #include "sexp_extended.h"
#endif // SEXP_EXTENDED_H_INCLUDED
//-------------------------------------------------
/******************************************************************************/
// the default constructor
jfAuthorFilterEditor::jfAuthorFilterEditor(const jfAuthExprFilter* infilt, QWidget* parent):
                                            jfSimpleSFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfAuthorFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfAuthExprFilter* result = new jfAuthExprFilter(namedesc_edit->TryGetName(), inpval);
  return result;
}
//================================================================================
// the default constructor
jfLanguageFilterEditor::jfLanguageFilterEditor(const jfLanguageExprFilter* infilt,
                                         QWidget* parent):jfSimpleSFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfLanguageFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfLanguageExprFilter* result = new jfLanguageExprFilter(namedesc_edit->TryGetName(), inpval);
  return result;
}
/*****************************************************************************/
