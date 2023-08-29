//***************************************************************************
// Name:        pairf_editor.cpp
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters editing, various panels.
// Created:     January 1, 2011
// Conversion to Qt Started October 16, 2013
// Updated:     August 19, 2023
//***************************************************************************
#ifndef PAIRF_EDITOR_H_INCLUDED
  #include "pairf_editor.h"
#endif // PAIRF_EDITOR_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//---------------------------------------
#include <assert.h>
#include <QMessageBox>
//***************************************************************************
// the constructor
jfPairFilEditPanelCore::jfPairFilEditPanelCore(QWidget* parent):QWidget(parent) {
  // we start with the special phrases editor
  spec_label = new QLabel("Special pairing keywords (separated by semicolon) :");
  spec_edit = new QLineEdit();
  // next, the characters used the separate names in a a pairing
  delim_label = new QLabel("Pairing separator characters :");
  delim_edit = new QLineEdit();
  delim_sizer = new QHBoxLayout();
  delim_sizer->addWidget(delim_label,0,Qt::AlignVCenter);
  delim_sizer->addWidget(delim_edit,1,Qt::AlignVCenter);
  // the various checkboxes and thier sizer
  usetokens = new QCheckBox("Use tokens, not substrings");
  sidespaces = new QCheckBox("Look for pairs with spaces around the separator as well.");
  nofriend = new QCheckBox("Make certain the word 'friendship' does not follow");
  nospace = new QCheckBox("Also look for name pairs with no separation");
  cb_sizer = new QGridLayout();
  cb_sizer->addWidget(usetokens,0,0);
  cb_sizer->addWidget(sidespaces,0,1);
  cb_sizer->addWidget(nofriend,1,0);
  cb_sizer->addWidget(nospace,1,1);
  // the top level sizer
  top_sizer = new QVBoxLayout();
  top_sizer->addWidget(spec_label,0, Qt::AlignVCenter);
  top_sizer->addWidget(spec_edit,0);
  top_sizer->addSpacing(3);
  top_sizer->addLayout(delim_sizer,0);
  top_sizer->addSpacing(3);
  top_sizer->addLayout(cb_sizer,0);
  top_sizer->addSpacing(3);

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading data
bool jfPairFilEditPanelCore::LoadData(const jfPairFilterCore* inval) {
  if (inval==NULL) return false;
  // separators
  QString buffer = inval->GetSeparators();
  delim_edit->setText(buffer);
  // specials
  buffer = inval->GetSpecials();
  spec_edit->setText(buffer);
  // checkboxes
  usetokens->setChecked((inval->GetFlagsRef()).usetokens );
  sidespaces->setChecked((inval->GetFlagsRef()).sidespaces );
  nofriend->setChecked((inval->GetFlagsRef()).nofriend );
  nospace->setChecked((inval->GetFlagsRef()).nospace );
  // done
  return true;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data. more complicated because we have to diplay errors for bad data
bool jfPairFilEditPanelCore::SaveData(jfPairFilterCore* outval) const {
  // constants
  const QString invalid = "There is a problem with this Pairing Filter.";
  const QString err1 = "Is is not valid for the separators to be empty.";
  // local variables
  QMessageBox message;
  QString bufval;
  bool atest;
  // checks
  if (outval==NULL) return false;
  // we start with the separators
  bufval = delim_edit->text();
  bufval = bufval.trimmed();
  if (bufval.isEmpty()) {
    // we are in error here
    message.setIcon(QMessageBox::Critical);
    message.setText(err1);
    message.setWindowTitle(invalid);
    message.exec();
    return false;
  }
  // setting the separator string
  bufval = delim_edit->text();
  atest = outval->SetSeparators(bufval);
  assert(atest);
  // the specials
  bufval = spec_edit->text();
  outval->AddSpecials(bufval);
  // doing the booleans
  (outval->GetMutFlagsRef()).usetokens = usetokens->isChecked();
  (outval->GetMutFlagsRef()).sidespaces = sidespaces->isChecked();
  (outval->GetMutFlagsRef()).nofriend = nofriend->isChecked();
  (outval->GetMutFlagsRef()).nospace= nospace->isChecked();
  // done
  return true;
}
//-----------------------------------------------------------------------
bool jfPairFilEditPanelCore::CheckSeps() const {
  // constants
  const QString invalid = "There is a problem with this Pairing Filter.";
  const QString err1 = "Is is not valid for the separators to be empty.";
  // local variables
  QString bufval;
  QMessageBox message;
  // getting separators
  bufval = delim_edit->text();
  bufval = bufval.trimmed();
  // are the separators empty or occupied?
  if (bufval.isEmpty()) {
    // we are in error here
    message.setIcon(QMessageBox::Critical);
    message.setText(err1);
    message.setWindowTitle(invalid);
    message.exec();
    return false;
  }
  else return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking function
bool jfPairFilEditPanelCore::CheckValid() const {
  if (!CheckSeps()) return false;
  return CheckNames();
}
//==================================================================================
// constructor
jfPairFilEditPanelSingle::jfPairFilEditPanelSingle(const jfPairFilterSingle* infil, QWidget* parent):jfPairFilEditPanelCore(parent) {
  // the gui elements we do here
  name1_label = new QLabel("Names (semicolon separated) of the first person in the pairing.");
  name1_edit = new QLineEdit();
  name2_label = new QLabel("Names (semicolon separated) of the second person in the pairing.");
  name2_edit = new QLineEdit();
  // adding to the sizer
  top_sizer->addWidget(name1_label,0,Qt::AlignLeft);
  top_sizer->addWidget(name1_edit,0);
  top_sizer->addSpacing(3);
  top_sizer->addWidget(name2_label,0,Qt::AlignLeft);
  top_sizer->addWidget(name2_edit,0);
  // next..
  setLayout(top_sizer);
  if (infil==NULL) {
    jfPairFilterSingle* tval = new jfPairFilterSingle("no name");
    LoadFilterData(tval);
    delete tval;
  }
  else LoadFilterData(infil);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual load and store stuff
bool jfPairFilEditPanelSingle::LoadFilterData(const jfPairFilterCore* inval) {
  size_t ncount;
  bool atest;
  // various checks
  assert(inval!=NULL);
  ncount = inval->GetNamesetCount();
  assert(ncount<3);
  // handling the common bits
  atest = LoadData(inval);
  assert(atest);
  // setting the namesets
  if (ncount>0) name1_edit->setText(inval->GetNameset(0));
  if (ncount>1) name2_edit->setText(inval->GetNameset(1));
  // done
  return false;
}
//-----------------------------
bool jfPairFilEditPanelSingle::SaveFilterData(jfPairFilterCore* outval) const {
  // local variables
  QString bufval1,bufval2;
  // checks
  if (outval==NULL) return false;
  if (!CheckNames()) return false;
  // getting the name sets
  bufval1 = name1_edit->text();
  bufval1 = bufval1.trimmed();
  bufval2 = name2_edit->text();
  bufval2 = bufval2.trimmed();
  // doing the core stuff
  if (!SaveData(outval)) return false;
  outval->AddNameset(bufval1);
  outval->AddNameset(bufval2);
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking function
bool jfPairFilEditPanelSingle::CheckNames() const {
  // constants
  const QString invalid = "There is a problem with this Pairing Filter.";
  const QString err1 = "Neither of the names (or names list) should be empty.";
  // local variables
  QString bufval1,bufval2;
  QMessageBox message;
  // getting the name sets
  bufval1 = name1_edit->text();
  bufval1 = bufval1.trimmed();
  bufval2 = name2_edit->text();
  bufval2 = bufval2.trimmed();
  // testing
  if (bufval1.isEmpty() || bufval2.isEmpty()) {
    message.setIcon(QMessageBox::Critical);
    message.setText(err1);
    message.setWindowTitle(invalid);
    message.exec();
    return false;
  }
  return true;
}
//==================================================================================
// constructor
jfPairFilEditPanelList::jfPairFilEditPanelList(const jfPairFilterList* infil, QWidget* parent):jfPairFilEditPanelCore(parent) {
  // the gui elements we do here
  name1_label = new QLabel("Names (semicolon separated) of the first person in the pairing.");
  name1_edit = new QLineEdit();
  namelist_label = new QLabel("Names (semicolon separated) of the other people (separate lines) in the pairing ");
  namelist_edit = new QPlainTextEdit();
  namelist_edit->setWordWrapMode(QTextOption::NoWrap);
  // adding to the sizer
  top_sizer->addWidget(name1_label,0,Qt::AlignLeft);
  top_sizer->addWidget(name1_edit,0);
  top_sizer->addSpacing(3);
  top_sizer->addWidget(namelist_label,0,Qt::AlignLeft);
  top_sizer->addWidget(namelist_edit,1);
  // next..
  setLayout(top_sizer);
  if (infil==NULL) {
    jfPairFilterList* tval = new jfPairFilterList("unnamed");
    LoadFilterData(tval);
    delete tval;
  }
  else LoadFilterData(infil);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual load and store stuff
bool jfPairFilEditPanelList::LoadFilterData(const jfPairFilterCore* inval) {
  size_t ncount,nloop;
  bool atest;
  // various checks
  assert(inval!=NULL);
  ncount = inval->GetNamesetCount();
  // handling the common bits
  atest = LoadData(inval);
  assert(atest);
  // setting the namesets
  if (ncount>0) name1_edit->setText(inval->GetNameset(0));
  namelist_edit->clear();
  if (ncount>=2) {
    for (nloop=1;nloop<ncount;nloop++) {
      namelist_edit->appendPlainText(inval->GetNameset(nloop));
      if (nloop!=(ncount-1)) {
        namelist_edit->appendPlainText("\n");
      }
    }
  }
  // done
  return false;
}
//-----------------------------
bool jfPairFilEditPanelList::SaveFilterData(jfPairFilterCore* outval) const {
  // local variables
  QString bufval1,bufval2;
  size_t lloop,linenum;
  QStringList slines;
  // checks
  if (outval==NULL) return false;
  if (!CheckNames()) return false;
  // doing the core stuff
  if (!SaveData(outval)) return false;
  // getting the first name
  bufval1 = name1_edit->text();
  bufval1 = bufval1.trimmed();
  outval->AddNameset(bufval1);
  // getting the othernames
  bufval2 = (namelist_edit->toPlainText()).trimmed();
  slines = bufval2.split("\n",QString::SkipEmptyParts);
  linenum = slines.count();
  // the writing loop
  for (lloop=0;lloop<linenum;lloop++) {
    bufval1 = slines[lloop].trimmed();
    if (!bufval1.isEmpty()) outval->AddNameset(bufval1);
  }
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking function
bool jfPairFilEditPanelList::CheckNames() const {
  // constants
  const QString invalid = "There is a problem with this Pairing Filter.";
  const QString err1 = "Neither of the names (or names list) should be empty.";
  // local variables
  QString bufval1,bufval2;
  QMessageBox message;
  // getting the name sets
  bufval1 = name1_edit->text();
  bufval1 = bufval1.trimmed();
  bufval2 = namelist_edit->toPlainText();
  bufval2 = bufval2.trimmed();
  // testing
  if (bufval1.isEmpty() || bufval2.isEmpty()) {
    message.setIcon(QMessageBox::Critical);
    message.setText(err1);
    message.setWindowTitle(invalid);
    message.exec();
    return false;
  }
  return true;
}
//==================================================================================
// constructor
jfPairFilEditPanelMulti::jfPairFilEditPanelMulti(const jfPairFilterMultiple* infil, QWidget* parent):jfPairFilEditPanelCore(parent) {
  // the gui elements we do here
  name_label = new QLabel("Names (semicolon separated) of the people to be paired (one person per line)");
  names_edit = new QPlainTextEdit();
  names_edit->setWordWrapMode(QTextOption::NoWrap);
  // adding to the sizer
  top_sizer->addWidget(name_label,0,Qt::AlignLeft);
  top_sizer->addWidget(names_edit,1);
  // next..
  setLayout(top_sizer);
  if (infil==NULL) {
    jfPairFilterMultiple* tval = new jfPairFilterMultiple("nothing");
    LoadFilterData(tval);
    delete tval;
  }
  else LoadFilterData(infil);

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual load and store stuff
bool jfPairFilEditPanelMulti::LoadFilterData(const jfPairFilterCore* inval) {
  const QString fname = "jfPairFilEditPanelMulti::LoadFilterData";
  // variables
  QString nameset;
  size_t ncount,nloop;
  bool atest;
  // various checks
  assert(inval!=NULL);
  ncount = inval->GetNamesetCount();
  // handling the common bits
  atest = LoadData(inval);
  assert(atest);
  // setting the namesets
  names_edit->clear();
  if (ncount==0) return false;
  for (nloop=0;nloop<ncount;nloop++) {
    nameset = inval->GetNameset(nloop);
    names_edit->appendPlainText(nameset);
    if (nloop!=(ncount-1)) {
      names_edit->appendPlainText("\n");
    }
  }
  // done
  return false;
}
//-----------------------------
bool jfPairFilEditPanelMulti::SaveFilterData(jfPairFilterCore* outval) const {
  const QString fname = "jfPairFilEditPanelMulti::SaveFilterData";
  // local variables
  QString bufval1,bufval2;
  size_t linenum, lloop;
  QStringList slines;
  // checks
  if (outval==NULL) return false;
  if (!CheckNames()) return false;
  // doing the core stuff
  if (!SaveData(outval)) return false;
  // getting the names
  bufval2 = (names_edit->toPlainText()).trimmed();
  slines = bufval2.split("\n",QString::SkipEmptyParts);
  linenum = slines.count();
  // the writing loop
  for (lloop=0;lloop<linenum;lloop++) {
    bufval1 = slines[lloop].trimmed();
    if (!bufval1.isEmpty()) outval->AddNameset(bufval1);
  }
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking function
bool jfPairFilEditPanelMulti::CheckNames() const {
  // constants
  const QString fname = "jfPairFilEditPanelMulti::CheckNames";
  const QString invalid = "There is a problem with this Pairing Filter.";
  const QString err1 = "There must be at least two people (lines) in the names box.";
  // local variables
  size_t lloop, lsize;
  QString line, bufval2;
  size_t lcount;
  QStringList slines;
  QMessageBox message;
  // initializing values
  bufval2 = (names_edit->toPlainText()).trimmed();
  slines = bufval2.split("\n",QString::SkipEmptyParts);
  // looping thru the lines in the listbox
  lsize = 0;
  lcount = slines.size();
  for (lloop=0;lloop<lcount;lloop++) {
    line = slines[lloop].trimmed();
    if (line.length()>0) lsize++;
  }
  // testing
  if (lsize<2) {
    message.setIcon(QMessageBox::Critical);
    message.setText(err1);
    message.setWindowTitle(invalid);
    message.exec();
    return false;
  }

  return true;
}

//***************************************************************************
