/******************************************************************************
Name    :   urlfilter_editor.cpp
Author  :   John Q Metro
Purpose :   Editors for jfExpressionFiler, and also jfUrlFilter
Created :   July 8, 2016 (split from expr_editor.h)
Conversion to Qt Started Oct 3, 2013
Updated :   July 8, 2016
******************************************************************************/
#ifndef URLFILTER_EDITOR_H
  #include "urlfilter_editor.h"
#endif // URLFILTER_EDITOR_H
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
#include <assert.h>

//****************************************************************************
/* The url filter has two multiline Text control boxes, one to list alternate
includes, the other for excludes, and a checkbox to disbale the first. Another
checkbox controls whether https protocol is counted when applying the filter
*/
jfUrlFilEdit::jfUrlFilEdit(bool side_arrangement, QWidget* parent):QWidget(parent) {

  // we prepare the lists and thier labels
  // alternates
  alt_labl = new QLabel("The url must match ONE of the patterns below:");
  alt_incls = new QPlainTextEdit();
  alt_incls->setLineWrapMode(QPlainTextEdit::NoWrap);
  // excludes
  excl_labl = new QLabel("The url must NOT match ANY of the patterns below:");
  excludes = new QPlainTextEdit();
  excludes->setLineWrapMode(QPlainTextEdit::NoWrap);
  // next up: the checkboxes
  case_insensetive = new QCheckBox("Case insensetive matching");
  excludes_only = new QCheckBox("This filter acts to exclude urls only");
  https_counted = new QCheckBox("HTTPS as well as HTTP urls will be matched");
  https_counted->setChecked(true);
  // saving the arrangement
  sidearr = side_arrangement;
  // start with sizers
  if (sidearr) {
    // the left column has everything except the exludes
    left_sizer = new QVBoxLayout();
    left_sizer->addWidget(case_insensetive,0);
    left_sizer->addWidget(https_counted,0);
    left_sizer->addWidget(excludes_only,0);
    left_sizer->addSpacing(3);
    left_sizer->addWidget(alt_labl,0);
    left_sizer->addWidget(alt_incls,1);
    // the right column has the excludes
    right_sizer = new QVBoxLayout();
    right_sizer->addWidget(excl_labl,0);
    right_sizer->addWidget(excludes,1);
    // putting the two sizers together
    totalsizer = new QBoxLayout(QBoxLayout::LeftToRight);
    totalsizer->addLayout(left_sizer,1);
    totalsizer->addLayout(right_sizer,1);
  }
  else {
    // one column for everything
    left_sizer = NULL;
    right_sizer = NULL;
    totalsizer = new QBoxLayout(QBoxLayout::TopToBottom);
    // we stack things in
    totalsizer->addWidget(case_insensetive,0);
    totalsizer->addWidget(https_counted,0);
    totalsizer->addWidget(excludes_only,0);
    totalsizer->addSpacing(3);
    totalsizer->addWidget(alt_labl,0);
    totalsizer->addWidget(alt_incls,1);
    totalsizer->addWidget(excl_labl,0);
    totalsizer->addWidget(excludes,1);
  }
  connect(excludes_only, SIGNAL(stateChanged(int)), this, SLOT(ExcludesOnlyChecked(int)));
  setLayout(totalsizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// return a filter with no name and description set, using the contents of the GUI
jfUrlFilter* jfUrlFilEdit::GetData() const {
  // local variables
  jfUrlFilter* result;
  QStringList* x1;
  // starting
  result = new jfUrlFilter();
  if (isEmpty()) return result;
  // we set the boolean values first
  result->cimatch = (case_insensetive->isChecked());
  result->https_incl = (https_counted->isChecked());
  // setting the alternates
  x1 = TextCtrl2List(alt_incls);
  result->SetAlternates(x1);
  // seeting the excludes
  x1 = TextCtrl2List(excludes);
  result->SetExcludes(x1);
  // done
  return result;
}
//-----------------------------------------------------------------------------
// thw filter panel is empty is there is nothing other than whitespace in the wxTextCtrls
bool jfUrlFilEdit::isEmpty() const {
  QString buffer;
  bool t1,t2;
  buffer = alt_incls->toPlainText();
  buffer = buffer.trimmed();
  t1 = buffer.isEmpty();
  buffer = excludes->toPlainText();
  buffer = buffer.trimmed();
  t2 = buffer.isEmpty();
  return (t1 && t2);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
bool jfUrlFilEdit::SetData(const jfUrlFilter* indata) {
  // local variables
  QStringList* tval;
  bool tvaln;
  size_t loopc;
  // basic checks
  assert(indata!=NULL);
  // setting the checked items first
  case_insensetive->setChecked(indata->cimatch);
  https_counted->setChecked(indata->https_incl);
  // setting the alternates
  tval = indata->GetAlternates();
  tvaln = (tval!=NULL);
  if (tvaln) {
    excludes_only->setChecked(false);
    alt_incls->setEnabled(true);
    alt_incls->clear();
    for (loopc=0;loopc<(tval->count());loopc++) {
      alt_incls->appendPlainText((*tval)[loopc]);
      alt_incls->appendPlainText(QString('\n'));
    }
    delete tval;
  }
  // setting the excludes
  tval = indata->GetExcludes();
  excludes->clear();
  if (tval!=NULL) {
    for (loopc=0;loopc<(tval->count());loopc++) {
      excludes->appendPlainText((*tval)[loopc]);
      excludes->appendPlainText(QString('\n'));
    }
    delete tval;
  }
  if (!tvaln) {
    excludes_only->setChecked(true);
    alt_incls->setEnabled(false);
  }
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// events
void jfUrlFilEdit::ExcludesOnlyChecked(int state) {
  if (excludes_only->isChecked()) {
    alt_incls->clear();
    alt_incls->setEnabled(false);
  }
  else {
    alt_incls->setEnabled(true);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QStringList* jfUrlFilEdit::TextCtrl2List(QPlainTextEdit* inedit) const {
  // checks and variables
  assert(inedit!=NULL);
  QStringList* result;
  QString rawdata;
  // we start
  result = new QStringList();
  rawdata = (inedit->toPlainText()).trimmed();
  (*result) = rawdata.split("\n",QString::SkipEmptyParts);
  return result;
}

//*****************************************************************************
