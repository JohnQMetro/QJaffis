/******************************************************************************
Name    :   widgets2.cpp
Author  :   John Q Metro
Purpose :   Some more miscuser interface stuff
Created :   November 24, 2009
Conversion to Qt started : September 15, 2013
Updated :   April 4, 2023 (fixed bug in GetBoth)
******************************************************************************/
#ifndef WIDGETS2_H_INCLUDED
  #include "widgets2.h"
#endif // WIDGETS2_H_INCLUDED
//-----------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//--------------------------------------------------------------------------
#include <assert.h>
#include <QSize>
#include <QMessageBox>
/*****************************************************************************/
// the constructor
jfTwoTexts::jfTwoTexts(QString label1, QString label2, size_t prp, bool multiline, QWidget* parent):QWidget(parent) {
  // starting
  assert(prp>0);
  assert(prp<10);
  prop = prp;
  ismulti = multiline;
  // we create the gui elements
  slabel1 = new QLabel(label1);
  slabel2 = new QLabel(label2);
  // we create the editors
  if (!multiline) {
    lebox1 = new QLineEdit();
    lebox2 = new QLineEdit();
    tebox1 = NULL;
    tebox2 = NULL;
  }
  else {
    tebox1 = new QPlainTextEdit();
    lebox1 = NULL;
    tebox2 = new QPlainTextEdit();
    lebox2 = NULL;
  }
  // we create the sizer and arrange
  topsizer = new QVBoxLayout();
  topsizer->addWidget(slabel1,0,Qt::AlignLeft);
  if (multiline) topsizer->addWidget(tebox1,1);
  else topsizer->addWidget(lebox1,0);
  topsizer->addSpacing(8);
  topsizer->addWidget(slabel2,0,Qt::AlignLeft);
  if (multiline) topsizer->addWidget(tebox2,1);
  else topsizer->addWidget(lebox2,0);
  // done
  setLayout(topsizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting strings
//------------------------------------------
void jfTwoTexts::Set1(QString inval) {
  if (ismulti) tebox1->setPlainText(inval);
  else lebox1->setText(inval);
}
//------------------------------------------
void jfTwoTexts::Set2(QString inval) {
  if (ismulti) tebox2->setPlainText(inval);
  else lebox2->setText(inval);
}
//------------------------------------------
void jfTwoTexts::SetBoth(QString inval1, QString inval2) {
  if (ismulti) tebox1->setPlainText(inval1);
  else lebox1->setText(inval1);
  if (ismulti) tebox2->setPlainText(inval2);
  else lebox2->setText(inval2);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting strings
//------------------------------------------
QString jfTwoTexts::Get1() const {
  if (ismulti) return tebox1->toPlainText();
  else return lebox1->text();
}
//------------------------------------------
QString jfTwoTexts::Get2() const {
  if (ismulti) return tebox2->toPlainText();
  else return lebox2->text();
}
//------------------------------------------
void jfTwoTexts::GetBoth(QString& out1, QString& out2) const {
  if (ismulti) out1 = tebox1->toPlainText();
  else out1 = lebox1->text();
  if (ismulti) out2 = tebox2->toPlainText();
  else out2 = lebox2->text();
}
//=======================================================================
// --- [ METHODS for jfNameEdit ]------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfNameEdit::jfNameEdit(const QString& ltext, const QString& starttext, int margin, QWidget* parent):QWidget(parent) {
  assert(!starttext.isEmpty());
  name_edit = new QLineEdit(starttext);
  name_label = new QLabel(ltext + " : ");
  layout = new QHBoxLayout();
  if (margin>=0) layout->setMargin(margin);
  layout->addWidget(name_label,0,Qt::AlignVCenter);
  layout->addWidget(name_edit,1);
  setLayout(layout);
  connect(name_edit, SIGNAL(textEdited(const QString&)),this, SLOT(HandleNameChange(const QString&)));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting names
//-----------------------------
bool jfNameEdit::ChangeText(const QString& starttext) {
  QString xext = starttext.trimmed();
  if (xext.isEmpty()) return false;
  backup_data = xext;
  name_edit->setText(xext);
  return true;
}
//-----------------------------
bool jfNameEdit::ChangeTextNS(const QString& starttext) {
  QString xext = starttext.trimmed();
  if (xext.isEmpty()) return false;
  backup_data = xext;
  name_edit->blockSignals(true);
  name_edit->setText(xext);
  name_edit->blockSignals(false);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting names
//-----------------------------
bool jfNameEdit::IsTextEmpty() const {
  QString xext = name_edit->text();
  xext = xext.trimmed();
  return xext.isEmpty();
}
//-----------------------------
QString jfNameEdit::TryGetName() const {
  QString xext = name_edit->text();
  xext = xext.trimmed();
  if (xext.isEmpty()) {
    // the copying has failed, we show the error
    ShowEmptyMessage();
    name_edit->setText(backup_data);
    name_edit->setModified(false);
    return "";
  }
  return xext;
}
//-----------------------------
QString jfNameEdit::GetName() const {
  QString xext = name_edit->text();
  return xext.trimmed();
}
//-----------------------------
// an override (because Qt can be stupid)
QSize jfNameEdit::sizeHint() const {
  QSize defs = QWidget::sizeHint();
  int widthx = defs.width();
  int heightx = defs.height();
  if (widthx < minimumWidth()) widthx = minimumWidth();
  if (heightx < minimumHeight()) heightx = minimumHeight();
  return QSize(widthx, heightx);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfNameEdit::HandleNameChange(const QString& newname) {
  QString localcopy = newname.trimmed();
  if (!localcopy.isEmpty()) emit SendNewName(localcopy);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// an internal helper method
void jfNameEdit::ShowEmptyMessage() const {
  // constants and local varibales
  QMessageBox emsg;
  const QString ermsg="Blank names are not allowed! Reverting to the previous name.";
  // we create the dialog
  emsg.setIcon(QMessageBox::Warning);
  emsg.setText(ermsg);
  emsg.exec();
}
//*****************************************************************************
