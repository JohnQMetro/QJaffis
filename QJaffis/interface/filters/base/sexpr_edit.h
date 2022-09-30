/******************************************************************************
Name    :   sexpr_edit.h
Author  :   John Q Metro
Purpose :   Simple Expression editors
Created :   December 4, 2010
Conversion to Qt Started October 7, 2013
Updated :   Febrary 12, 2018 (fixes for checking)
******************************************************************************/
#ifndef SEXPR_EDIT_H_INCLUDED
#define SEXPR_EDIT_H_INCLUDED
#endif // SEXPR_EDIT_H_INCLUDED
//-------------------------------------------------------------------------------------
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
#ifndef WIDGETS4_H
    #include "../../controls/widgets4.h"
#endif // WIDGETS4_H
//----------------------------------------------
#include <QWidget>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
//****************************************************************************
/* The simple expression editor has an edit box and one button, for checking
the validity of an expression

Color coding will be used to determine errors:
0: Green, okay
1: Black, unchecked
2: Red, in error
*/
class jfSimpleExprEdit : public jfExprEditBase {
    Q_OBJECT
  public:
    jfSimpleExprEdit(bool mline_in, QWidget* parent = NULL);
    // checking method
    jfSimpleExpr* CheckFilter(QString& outmessage);
    virtual bool CheckExpr(const QString& inexpr,bool& outempty,QString& outmessage) const;
  protected:
};
//****************************************************************************
