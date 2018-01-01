/******************************************************************************
Name    :   sexpr_edit.h
Author  :   John Q Metro
Purpose :   Simple Expression editors
Created :   December 4, 2010
Conversion to Qt Started October 7, 2013
Updated :   Febrary 13, 2014
******************************************************************************/
#ifndef SEXPR_EDIT_H_INCLUDED
#define SEXPR_EDIT_H_INCLUDED
#endif // SEXPR_EDIT_H_INCLUDED
//-------------------------------------------------------------------------------------
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
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
class jfSimpleExprEdit : public QWidget {
    Q_OBJECT
  public:
    jfSimpleExprEdit(bool mline_in, QWidget* parent = NULL);
    // checking method
    jfSimpleExpr* CheckFilter(QString& outmessage);
    bool CheckExpr(const QString& inexpr,QString& outmessage) const;
    // setting data
    bool SetData(const QString& inexpr,QString& outmessage);
  public slots:
    void PressCheckFilter(bool checked);
    void TextChanged();
  protected:
    // internal helper
    void SetStatus(size_t instatus);
    // the internal controls
    QLineEdit* editcore1;
    QPlainTextEdit* editcore2;
    QPushButton* fcheck_btn;
    // sizers (horizontal, buttons on the right)
    QHBoxLayout* topsizer;
    // error info
    size_t error_code;
    bool mline;
};
//****************************************************************************
