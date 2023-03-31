/******************************************************************************
Name    :   expr_editor.cpp
Author  :   John Q Metro
Purpose :   Editors for jfExpressionFiler, and also categories
Created :   March 26, 2009
Conversion to Qt Started Oct 3, 2013
Updated :   February 12, 2018 (Filter Check fixes)
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
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//--------------------------------------
#include <assert.h>
#include <QTextCursor>
#include <QMessageBox>
#include <QPalette>
#include <QApplication>
//****************************************************************************
jfExpressionEditor::jfExpressionEditor(jfFilterMap* inmap, bool notinmap, const bool& multiline, bool in_global, QWidget* parent):jfExprEditBase(multiline,parent) {
  // we start with checking the inmap
  assert(inmap!=NULL);
  local_fmap = inmap;
  map_nomap = notinmap;
  isglobal = in_global;
  // the additional filter picker button
  finsert_btn = new QPushButton("Pick Filter");

  // what we do next depends on whether the editor is multiline or not
  if (multiline) {
    // we start arranging
    rightsizer = new QVBoxLayout();
    rightsizer->addWidget(finsert_btn,0);
    rightsizer->addWidget(fcheck_btn,0);
    rightsizer->addStretch(1);
    topsizer->addLayout(rightsizer,0);
  }
  else {
    rightsizer = NULL;
    // we start arranging
    topsizer->addWidget(finsert_btn,0);
    topsizer->addWidget(fcheck_btn,0);
  }
  connect(finsert_btn, SIGNAL(clicked(bool)), this, SLOT(PressPickFilter(bool)));
  // recalc layout
  topsizer->update();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfExpressionFilter* jfExpressionEditor::CheckFilter(QString& outmessage) {
    const QString fname = "jfExpressionEditor::CheckFilter";
  // local variables
  QString dstring;
  jfNameVerif* verifier;
  jfExpressionFilter *result_expression;
  // starting...
  dstring = getText();
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
bool jfExpressionEditor::CheckExpr(const QString& inexpr,bool& outempty,QString& outmessage) const {
  // local variables
  jfNameVerif* verifier;
  jfExpressionFilter *result_expression;
  // starting...
  QString tinexpr = inexpr.trimmed();
  // empty expresssions
  if (tinexpr.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    outempty = true;
    return true;
  }
  // non-empty expressions
  else {
    outempty = false;
    result_expression = new jfExpressionFilter();
    result_expression->SetFiltermapLink(local_fmap);
    if (!result_expression->FromString(tinexpr)) {
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfExpressionEditor::PressPickFilter(bool checked) {
    // local variables
    jfFilterPickerDialog* pickerdialog;
    jfGlobalFilterPickerDialog* gpicker_dialog;
    QString* insert_result;
    QDialog::DialogCode dialog_result;
    QTextCursor tval;

    // we record the insertion point (not sure if this is necessary)
    long iindex;
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
//*****************************************************************************
