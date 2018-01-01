/******************************************************************************
Name    :   sexpr_edit.cpp
Author  :   John Q Metro
Purpose :   Simple Expression editors
Created :   December 4, 2010
Conversion to Qt Started October 7, 2013
Updated :   February 13, 2014
******************************************************************************/
#ifndef SEXPR_EDIT_H_INCLUDED
  #include "sexpr_edit.h"
#endif // SEXPR_EDIT_H_INCLUDED
//--------------------------------------------
#include <QMessageBox>
#include <assert.h>
//****************************************************************************
jfSimpleExprEdit::jfSimpleExprEdit(bool mline_in, QWidget* parent):QWidget(parent) {
  // we start with checking the inmap
  error_code = 1;
  // sw start with button creation
  fcheck_btn = new QPushButton("Check");
  // creating the top sizer, which is always horizontal
  topsizer = new QHBoxLayout();
  // we create and arrange
  mline = mline_in;
  if (mline) {
    editcore1 = NULL;
    editcore2 = new QPlainTextEdit();
    topsizer->addWidget(editcore2,1);
  }
  else {
    editcore1 = new QLineEdit();
    editcore2 = NULL;
    topsizer->addWidget(editcore1,1);
  }
  topsizer->addWidget(fcheck_btn,0,Qt::AlignTop);
  // done with creation and sizing
  setLayout(topsizer);
  if (mline) connect(editcore2, SIGNAL(textChanged()), this, SLOT(TextChanged()));
  else connect(editcore1, SIGNAL(textChanged()), this, SLOT(TextChanged()));
  connect(fcheck_btn, SIGNAL(clicked(bool)), this, SLOT(PressCheckFilter(bool)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSimpleExpr* jfSimpleExprEdit::CheckFilter(QString& outmessage) {
  // local variables
  QString dstring;
  jfSimpleExpr *result_expression;
  // starting...
  if (mline) dstring = editcore2->toPlainText();
  else dstring = editcore1->text();
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
bool jfSimpleExprEdit::CheckExpr(const QString& inexpr,QString& outmessage) const {
  // local variables
  jfSimpleExpr *result_expression;
  // starting...
  // empty expresssions
  if (inexpr.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    return true;
  }
  // non-empty expressions
  else {
    result_expression = new jfSimpleExpr();
    if (!result_expression->FromString(inexpr)) {
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
//-----------------------------------------------------------------------------
// setting data
bool jfSimpleExprEdit::SetData(const QString& inexpr,QString& outmessage) {
  // getting the check result
  bool chkres = CheckExpr(inexpr,outmessage);
  if (mline) editcore2->setPlainText(inexpr);
  else editcore1->setText(inexpr);
  if (chkres) SetStatus(0);
  else SetStatus(2);
  return chkres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfSimpleExprEdit::PressCheckFilter(bool checked) {
  // local variables
  const QString rvalid = "Expression Valid";
  const QString nvalid = "Expression Not Valid";
  QString dstring, errstring;
  jfSimpleExpr *result_expression;
  QMessageBox emsg;
  // starting
  result_expression = CheckFilter(errstring);
  if (result_expression==NULL) {
    // we are in error here
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(errstring);
    emsg.setWindowTitle(nvalid);
    emsg.exec();
  }
  else {
    // no error
    if (result_expression->isEmpty()) dstring = "Empty expressions match everything.";
    else dstring = "Everthing okay.";
    emsg.setIcon(QMessageBox::Information);
    emsg.setText(dstring);
    emsg.setWindowTitle(rvalid);
    emsg.exec();
  }
  // done
  delete result_expression;
}
//--------------------------------------------------------------------------
void jfSimpleExprEdit::TextChanged() {
  SetStatus(1);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper
void jfSimpleExprEdit::SetStatus(size_t instatus) {
  // asserts and variables
  assert(instatus<3);
  QPalette tcolour;
  // starting...
  if (instatus==0) {
    tcolour.setColor(QPalette::Text,Qt::green);
    if (mline) {
      editcore2->setPalette(tcolour);
      editcore2->document()->setModified(false);
    }
    else {
      editcore1->setPalette(tcolour);
      editcore1->setModified(false);
    }
  }
  else if (instatus==1) {
    if (mline) {
      editcore2->setPalette(tcolour);
      editcore2->document()->setModified(true);
    }
    else {
      editcore1->setPalette(tcolour);
      editcore1->setModified(true);
    }
  }
  else {
    tcolour.setColor(QPalette::Text,Qt::red);
    if (mline) {
      editcore2->setPalette(tcolour);
      editcore2->document()->setModified(false);
    }
    else {
      editcore1->setPalette(tcolour);
      editcore1->setModified(false);
    }
  }
  error_code = instatus;
}
//****************************************************************************
