/******************************************************************************
Name    :   sexpr_edit.cpp
Author  :   John Q Metro
Purpose :   Simple Expression editors
Created :   December 4, 2010
Conversion to Qt Started October 7, 2013
Updated :   February 12, 2018
******************************************************************************/
#ifndef SEXPR_EDIT_H_INCLUDED
  #include "sexpr_edit.h"
#endif // SEXPR_EDIT_H_INCLUDED
//--------------------------------------------
#include <QMessageBox>
#include <assert.h>
//****************************************************************************
jfSimpleExprEdit::jfSimpleExprEdit(bool mline_in, QWidget* parent):jfExprEditBase(mline_in,parent) {
    // the check button goes directly to the right in this version...
    topsizer->addWidget(fcheck_btn,0,Qt::AlignTop);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSimpleExpr* jfSimpleExprEdit::CheckFilter(QString& outmessage) {
  // local variables
  QString dstring;
  jfSimpleExpr *result_expression;
  // starting...
  dstring = getText();
  // empty expresssions
  if (dstring.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    result_expression = new jfSimpleExpr(dstring);
    SetStatus(0);
    return result_expression;
  }
  // non-empty expressions
  else {
    result_expression = new jfSimpleExpr();
    if (!result_expression->FromString(dstring)) {
      outmessage = result_expression->parse_error;
      delete result_expression;
      SetStatus(2);
      return NULL;
    }
    outmessage="No errors detected.";
    SetStatus(0);
    return result_expression;
  }
}
//-----------------------------------------------------------------------------
/* This method treats in input string as an expression and reports on it's
validity */
bool jfSimpleExprEdit::CheckExpr(const QString& inexpr,bool& outempty, QString& outmessage) const {
  // local variables
  jfSimpleExpr *result_expression;
  QString inval = inexpr.trimmed();
  // starting...
  // empty expresssions
  if (inval.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    outempty = true;
    return true;
  }
  // non-empty expressions
  else {
    outempty = false;
    result_expression = new jfSimpleExpr();
    if (!result_expression->FromString(inval)) {
      outmessage = result_expression->parse_error;
      delete result_expression;
      return false;
    }
    // we check for validity
    outmessage="No errors detected.";
    delete result_expression;
    return true;
  }
}
//****************************************************************************
