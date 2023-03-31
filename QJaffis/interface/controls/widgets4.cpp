/******************************************************************************
Name    :   widgets4.cpp
Author  :   John Q Metro
Purpose :   Some misc user interface stuff
Created :   February 12, 2018
Updated :   March 27, 2023
******************************************************************************/
#ifndef WIDGETS4_H
    #include "widgets4.h"
#endif // WIDGETS4_H
//------------------------------
#include <QPalette>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextCharFormat>
#include <QMessageBox>
#include <assert.h>
/*****************************************************************************/
// annoyingly, QPlainTextEdit is still too unplain for some things I want
// so a custom class to provide simple extra functions for expression editors
//----------------------------------------------------------------
jfExprTextEdit::jfExprTextEdit(QWidget* parent):QPlainTextEdit(parent) {
    current_colour = palette().color(QPalette::Text);
    default_colour = current_colour;
}
//-------------------------------
jfExprTextEdit::jfExprTextEdit(const QString& text, QWidget* parent):QPlainTextEdit(text,parent) {
    current_colour = palette().color(QPalette::Text);
    default_colour = current_colour;
}
//+++++++++++++++++++++++++++++++++++++++++++
// color get and Set
//-------------------------------
QColor jfExprTextEdit::currentColor() const {
    return current_colour;
}
//-------------------------------
void jfExprTextEdit::changeColor(const QColor& newcolor) {
    if (!newcolor.isValid()) return;
    blockSignals(true);
    // saving the document color to the palette
    current_colour = newcolor;
    QPalette cpal = palette();
    cpal.setColor(QPalette::Text,newcolor);
    cpal.setColor(QPalette::WindowText,newcolor);
    setPalette(cpal);
    // we still need to change the color of the existing text...
    // this is ridiculously complicated
    QTextCursor backup_cursor = textCursor();
    QTextCharFormat temp_format = currentCharFormat();
    selectAll();
    QTextCursor all_cursor = textCursor();
    temp_format.setForeground(QBrush(newcolor));
    all_cursor.setCharFormat(temp_format);
    setTextCursor(backup_cursor);
    blockSignals(false);
    repaint();
}
//------------------------------
void jfExprTextEdit::setColorToDefault() {
    changeColor(default_colour);
}
//++++++++++++++++++++++++++++++++++++++++++++
void jfExprTextEdit::setModified(bool isModified) {
    document()->setModified(isModified);
}
//===================================================================
jfExprEditBase::jfExprEditBase(bool mline_in, QWidget* parent):QWidget(parent) {
  // we start with checking the inmap
  error_code = 1;
  // sw start with button creation
  fcheck_btn = new QPushButton("Check");
  // creating the top sizer, which is always horizontal
  topsizer = new QHBoxLayout();
  topsizer->setContentsMargins(5,5,5,5);
  // we create and arrange
  mline = mline_in;
  if (mline) {
    editcore1 = NULL;
    editcore2 = new jfExprTextEdit();
    topsizer->addWidget(editcore2,1);
  }
  else {
    editcore1 = new QLineEdit();
    editcore2 = NULL;
    topsizer->addWidget(editcore1,1);
  }
  // done with creation and sizing
  setLayout(topsizer);
  if (mline) connect(editcore2, SIGNAL(textChanged()), this, SLOT(TextChanged()));
  else connect(editcore1, SIGNAL(textEdited(QString)), this, SLOT(TextChanged2(QString)));
  connect(fcheck_btn, SIGNAL(clicked(bool)), this, SLOT(PressCheckFilter(bool)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfExprEditBase::getText() const {
    QString dstring;
    if (mline) dstring = editcore2->toPlainText();
    else dstring = editcore1->text();
    return dstring.trimmed();
}
//-----------------------------------------------------------------------------
// setting data
bool jfExprEditBase::SetData(const QString& inexpr,QString& outmessage) {
  // getting the check result
    bool isempty;
  bool chkres = CheckExpr(inexpr,isempty,outmessage);
  setText(inexpr);
  if (chkres) SetStatus(0);
  else SetStatus(2);
  return chkres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfExprEditBase::PressCheckFilter(bool value) {
  // local variables
  const QString rvalid = "Expression Valid";
  const QString nvalid = "Expression Not Valid";
  QString dstring, errstring;
  QMessageBox emsg;
  // starting
  dstring = getText();
  bool isempty;
  bool checkval = CheckExpr(dstring,isempty,errstring);
  if (!checkval) {
    // we are in error here
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(errstring);
    emsg.setWindowTitle(nvalid);
    emsg.exec();
    SetStatus(2);
  }
  else {
    // no error
    if (isempty) dstring = "Empty expressions match everything.";
    else dstring = "Everthing okay.";
    emsg.setIcon(QMessageBox::Information);
    emsg.setText(dstring);
    emsg.setWindowTitle(rvalid);
    emsg.exec();
    SetStatus(0);
  }
  // done
}
//--------------------------------------------------------------------------
void jfExprEditBase::TextChanged() {
    SetStatus(1);
}
//--------------------------------------
void jfExprEditBase::TextChanged2(QString value) {
    SetStatus(1);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfExprEditBase::setText(const QString intext) {
    if (mline) editcore2->setPlainText(intext);
    else editcore1->setText(intext);
}
//---------------------------------------------------------------
// internal helper
void jfExprEditBase::SetStatus(size_t instatus) {
  // asserts and variables
  assert(instatus<3);
  QPalette tcolour;
  QString xtext;
  // starting...
  if (instatus==0) {
    tcolour.setColor(QPalette::Text,Qt::darkGreen);
    if (mline) {
      editcore2->changeColor(Qt::darkGreen);
      editcore2->setModified(false);
    }
    else {
      editcore1->setPalette(tcolour);
      editcore1->setModified(false);
    }
  }
  else if (instatus==1) {
    if (mline) {
      editcore2->setColorToDefault();
      editcore2->setModified(true);
    }
    else {
      editcore1->setPalette(tcolour);
      editcore1->setModified(true);
    }
  }
  else {
    tcolour.setColor(QPalette::Text,Qt::darkRed);
    if (mline) {
      editcore2->changeColor(Qt::darkRed);
      editcore2->setModified(false);
    }
    else {
      editcore1->setPalette(tcolour);
      editcore1->setModified(false);
    }
  }
  error_code = instatus;
}

/*****************************************************************************/
