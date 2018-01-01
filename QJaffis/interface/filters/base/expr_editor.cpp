/******************************************************************************
Name    :   expr_editor.cpp
Author  :   John Q Metro
Purpose :   Editors for jfExpressionFiler, and also categories
Created :   March 26, 2009
Conversion to Qt Started Oct 3, 2013
Updated :   July 8, 2016 (removed jfUrlFilEdit)
******************************************************************************/
#ifndef EXPR_EDITOR_H_INCLUDED
  #include "expr_editor.h"
#endif // EXPR_EDITOR_H_INCLUDED
#ifndef FILTER_WIDGETS_H_INCLUDED
  #include "../filter_picker.h"
#endif // FILTER_WIDGETS_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../../../core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
//--------------------------------------
#include <assert.h>
#include <QTextCursor>
#include <QMessageBox>
#include <QPalette>
//****************************************************************************
jfExpressionEditor::jfExpressionEditor(jfFilterMap* inmap, bool notinmap, const bool& multiline, bool in_global, QWidget* parent):QWidget(parent) {
  // we start with checking the inmap
  assert(inmap!=NULL);
  local_fmap = inmap;
  error_code = 1;
  map_nomap = notinmap;
  isglobal = in_global;
  mline = multiline;
  // sw start with button creation
  finsert_btn = new QPushButton("Pick Filter");
  fcheck_btn = new QPushButton("Check");
  // creating the top sizer, which is always horizontal
  topsizer = new QHBoxLayout();
  // what we do next depends on whether the editor is multiline or not
  if (multiline) {
    editcore1 = NULL;
    editcore2 = new QPlainTextEdit();
    // we start arranging
    rightsizer = new QVBoxLayout();
    rightsizer->addWidget(finsert_btn,0);
    rightsizer->addWidget(fcheck_btn,0);
    rightsizer->addStretch(1);
    topsizer->addWidget(editcore2,1);
    topsizer->addLayout(rightsizer,0);
  }
  else {
    editcore1 = new QLineEdit();
    editcore2 = NULL;
    rightsizer = NULL;
    // we start arranging
    topsizer->addWidget(editcore1,1,Qt::AlignVCenter);
    topsizer->addWidget(finsert_btn,0);
    topsizer->addWidget(fcheck_btn,0);
  }
  // done with creation and sizing
  setLayout(topsizer);
  // connections
  if (mline) connect(editcore2, SIGNAL(textChanged()), this, SLOT(TextChanged()));
  else connect(editcore1, SIGNAL(textChanged()), this, SLOT(TextChanged()));
  connect(finsert_btn, SIGNAL(clicked(bool)), this, SLOT(PressPickFilter(bool)));
  connect(fcheck_btn, SIGNAL(clicked(bool)), this, SLOT(PressCheckFilter(bool)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfExpressionFilter* jfExpressionEditor::CheckFilter(QString& outmessage) {
  // local variables
  QString dstring;
  jfNameVerif* verifier;
  jfExpressionFilter *result_expression;
  // starting...
  if (mline) dstring = editcore2->toPlainText();
  else editcore1->text();
  // empty expresssions
  if (dstring.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    result_expression = new jfExpressionFilter();
    result_expression->EmptyFilter();
    result_expression->SetFiltermapLink(local_fmap);
    SetStatus(0);
    return result_expression;
  }
  // non-empty expressions
  else {
    result_expression = new jfExpressionFilter();
    result_expression->SetFiltermapLink(local_fmap);
    if (!result_expression->FromString(dstring) ) {
      outmessage = result_expression->parse_error;
      delete result_expression;
      SetStatus(2);
      return NULL;
    }
    // we check for validity
    verifier = new jfNameVerif();
    if (!(result_expression->VerifyNames(verifier,map_nomap))) {
      outmessage = verifier->GetError();
      delete result_expression;
      SetStatus(2);
      return NULL;
    }
    else {
      outmessage="No errors detected.";
      SetStatus(0);
      return result_expression;
    }
  }
}
//-----------------------------------------------------------------------------
/* This method treats in input string as an expression and reports on it's
validity */
bool jfExpressionEditor::CheckExpr(const QString& inexpr,QString& outmessage) const {
  // local variables
  jfNameVerif* verifier;
  jfExpressionFilter *result_expression;
  // starting...
  // empty expresssions
  if (inexpr.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    return true;
  }
  // non-empty expressions
  else {
    result_expression = new jfExpressionFilter();
    result_expression->SetFiltermapLink(local_fmap);
    if (!result_expression->FromString(inexpr)) {
      outmessage = result_expression->parse_error;
      delete result_expression;
      return false;
    }
    // we check for validity
    verifier = new jfNameVerif();
    if (!(result_expression->VerifyNames(verifier,map_nomap))) {
      outmessage = verifier->GetError();
      delete result_expression;
      return false;
    }
    else {
      outmessage="No errors detected.";
      delete result_expression;
      return true;
    }
  }
}
//-----------------------------------------------------------------------------
// setting data
bool jfExpressionEditor::SetData(const QString& inexpr,QString& outmessage) {
  // getting the check result
  bool chkres = CheckExpr(inexpr,outmessage);
  if (mline) editcore2->setPlainText(inexpr);
  else editcore1->setText(inexpr);
  if (chkres) SetStatus(0);
  else SetStatus(2);
  return chkres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfExpressionEditor::PressPickFilter(bool checked) {
  // local variables
  jfFilterPickerDialog* pickerdialog;
  jfGlobalFilterPickerDialog* gpicker_dialog;
  long iindex;
  QString* insert_result;
  QDialog::DialogCode dialog_result;
  QTextCursor tval;
  // we record the insertion point (not sure if this is necessary)
  if (mline) {
    tval = editcore2->textCursor();
    iindex = tval.position();
  }
  else iindex = editcore1->cursorPosition();
  // start preparations to show the dialog
  insert_result = new QString();
  // the global only version
  if (isglobal) {
    gpicker_dialog = new jfGlobalFilterPickerDialog("Pick a Global filter",insert_result,this);
    dialog_result = (QDialog::DialogCode)gpicker_dialog->exec();
    delete gpicker_dialog;
  }
  else { // or the one for local as well
    pickerdialog = new jfFilterPickerDialog("Pick a Local or Global filter",local_fmap,insert_result,this);
    dialog_result = (QDialog::DialogCode)pickerdialog->exec();
    delete pickerdialog;
  }
  // showing
  if (dialog_result==QDialog::Accepted) {
    if (!(*insert_result).isEmpty()) {
      // we insert the result in the editor
      if (mline) editcore2->insertPlainText(*insert_result);
      else editcore1->insert(*insert_result);
    }
  }
  // cleaning up the results
  delete insert_result;
}
//--------------------------------------------------------------------------
void jfExpressionEditor::PressCheckFilter(bool checked) {
  // local variables
  const QString rvalid = "Expression Valid";
  const QString nvalid = "Expression Not Valid";
  QString dstring, errstring;
  jfExpressionFilter *result_expression;
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
void jfExpressionEditor::TextChanged() {
  SetStatus(1);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper
void jfExpressionEditor::SetStatus(size_t instatus) {
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
//*****************************************************************************
