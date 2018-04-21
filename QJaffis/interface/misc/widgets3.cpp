/******************************************************************************
Name    :   widgets3.cpp
Author  :   John Q Metro
Purpose :   Some misc user interface stuff
Created :   December 5, 2010
Conversion to Qt Started May 22, 2013
Updated :   August 5, 2015
******************************************************************************/
#ifndef WIDGETS3_H_INCLUDED
  #include "widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
#include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
#include <QFileDialog>
#include <QMessageBox>

//****************************************************************************
// just to reduce layout, a bundled line editor and label
//++++++++++++++++++
// constructor
jfLabeledEdit::jfLabeledEdit(QWidget* parent, QString inlabel, bool horiz, QString intext):QWidget(parent) {
  // creating the sizer
  layout = new QBoxLayout((horiz)?(QBoxLayout::LeftToRight):(QBoxLayout::TopToBottom),this);
  // creating the items
  clabel = inlabel;
  thelabel = new QLabel(inlabel+ " : ");
  thetext = new QLineEdit(intext);
  // arranging
  layout->addWidget(thelabel,0,(horiz)?(Qt::AlignVCenter):(Qt::AlignAbsolute));
  layout->addWidget(thetext,1,(horiz)?(Qt::AlignVCenter):(Qt::AlignAbsolute));
  setLayout(layout);
}
//+++++++++++++++++
// getting and setting
//------------------
void jfLabeledEdit::SetLabel(QString inlabel) {
  clabel = inlabel;
  thelabel->setText(inlabel + " : ");
}
//------------------
QString jfLabeledEdit::GetLabel() const {
  return clabel;
}
//------------------
void jfLabeledEdit::SetText(QString intext) {
  thetext->setText(intext);
}
//------------------
void jfLabeledEdit::SetFiletag(QString inval) {
  inval = UBReplace(inval,UFNAME_DELIMS);
  thetext->setText(inval);
}
//------------------
QString jfLabeledEdit::GetText() const {
  return thetext->text();
}
//=====================================================================
// just to reduce layout, a bundled integer entry and label
//+++++++++++++++++++++++++++++++++++++++
// constructor
jfLabeledIntEdit::jfLabeledIntEdit(QWidget* parent, QString inlabel, bool horiz, int min, int max):QWidget(parent) {
  // checking the min max
  assert(max>min);
  // creating the sizer
  layout = new QBoxLayout((horiz)?(QBoxLayout::LeftToRight):(QBoxLayout::TopToBottom),this);
  // creating the items
  clabel = inlabel;
  thelabel = new QLabel(inlabel+ " : ");
  thenumber = new QSpinBox();
  thenumber->setMinimum(min);
  thenumber->setMaximum(max);
  thenumber->setValue(min);
  // arranging
  layout->addWidget(thelabel,0,(horiz)?(Qt::AlignVCenter):(Qt::AlignCenter));
  if (horiz) layout->addWidget(thenumber,1,Qt::AlignVCenter);
  else layout->addWidget(thenumber,0,Qt::AlignAbsolute);
  setLayout(layout);
}
//+++++++++++++++++++++++++++++++++++++++
// getting and setting
//------------------
void jfLabeledIntEdit::SetLabel(QString inlabel) {
  clabel = inlabel;
  thelabel->setText(inlabel + " : ");
}
//------------------
QString jfLabeledIntEdit::GetLabel() const {
  return clabel;
}
//------------------
bool jfLabeledIntEdit::SetMinMax(int min, int max) {
  if (max<=min) return false;
  int cval = thenumber->value();
  if (cval<min) cval = min;
  else if (cval>max) cval = max;
  thenumber->setValue(cval);
  thenumber->setMinimum(min);
  thenumber->setMaximum(max);
  return true;
}
//------------------
bool jfLabeledIntEdit::SetValue(int invalue) {
  if (invalue<(thenumber->minimum())) return false;
  if (invalue>(thenumber->maximum())) return false;
  thenumber->setValue(invalue);
  return true;
}
//------------------
int jfLabeledIntEdit::GetValue() const {
  return thenumber->value();
}
//==========================================================================
// a directory picker, which display and button which launched a Dialog
//+++++++++++++++++++++++++++++++++++++++++++
// constructor
jfDirPicker::jfDirPicker(QWidget* parent,bool vertical, bool left, QString indirlabel, QString indir):QWidget(parent) {
  // setting the directory
  QDir tpath = QDir(indir);
  if (tpath.exists()) thedir = indir;
  else thedir = QDir::homePath();
  dirlabel = indirlabel;
  // creating the widgets
  launch_dialog = new QPushButton("Select a &Directory");
  dlabel = new QLabel(indirlabel + " :");
  dir_display = new QLabel(thedir);
  // arranging
  ArrangeWidgets(vertical,left);
  // finishing
  connect(launch_dialog, SIGNAL(clicked(bool)),this, SLOT(HandleClick(bool)));
}
//+++++++++++++++++++++++++++++++++++++++++++
// i/o
//------------------------------
QString jfDirPicker::GetPath() const {
  /**/JDEBUGLOGS("jfDirPicker::GetPath()",1,thedir)
  return thedir;
}
//------------------------------
bool jfDirPicker::SetPath(QString inval) {
  /**/JDEBUGLOGS("jfDirPicker::SetPath()",1,thedir)
  QDir tpath = QDir(inval);
  if (tpath.exists()) thedir = inval;
  else return false;
  dir_display->setText(inval);
  return true;
}
//-------------------------------
void jfDirPicker::SetPathUnchecked(const QString& inval) {
    /**/JDEBUGLOGS("jfDirPicker::SetPathUnchecked",1,inval)
  thedir = inval.trimmed();
  dir_display->setText(thedir);
}
//-------------------------------
void jfDirPicker::SetLEnabled(bool enable) {
  launch_dialog->setEnabled(enable);
}

//+++++++++++++++++++++++++++++++++++++++++++
void jfDirPicker::HandleClick(bool checked) {
  QString dst = QFileDialog::getExistingDirectory(this,dirlabel,thedir,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  thedir = dst;
  dir_display->setText(thedir);
  emit SendPathChanged();
}
//+++++++++++++++++++++++++++++++++++++++++++
void jfDirPicker::ArrangeWidgets(bool vertical, bool left) {
  // top level sizer
  mlay = new QBoxLayout((left)?(QBoxLayout::LeftToRight):(QBoxLayout::RightToLeft),this);
  // adding the button
  mlay->addWidget(launch_dialog,0/*,Qt::AlignVCenter*/);
  // label layout
  if (vertical) {
    slay = new QBoxLayout(QBoxLayout::TopToBottom,this);
    slay->addWidget(dlabel,0,Qt::AlignLeft);
    slay->addWidget(dir_display,0,Qt::AlignLeft);
    mlay->addSpacing(2);
    mlay->addLayout(slay,1);
  }
  else {
    slay = NULL;
    mlay->addSpacing(2);
    if (left) {
      mlay->addWidget(dlabel,0,Qt::AlignVCenter);
      mlay->addWidget(dir_display,1,Qt::AlignVCenter);
    }
    else {
      mlay->addWidget(dir_display,1,Qt::AlignVCenter);
      mlay->addWidget(dlabel,0,Qt::AlignVCenter);
    }
  }
  // setting the layout
  setLayout(mlay);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// sometimes, I need to initialize a QComboBox from an array of strings, not a QStringList
bool LoadCBoxFromArray(QComboBox* destination, const QString source[], size_t lcount) {
  if (destination==NULL) return false;
  if (destination->count()>0) destination->clear();
  if (lcount==0) return true;
  for (size_t lidx=0;lidx<lcount;lidx++) {
    destination->addItem(source[lidx]);
  }
  return true;
}
//============================================================================
// constructor
jfMinMaxEditor::jfMinMaxEditor(QString inname, bool wrapped, bool horizontal, bool shortnames, QWidget* parent):QWidget(parent) {
  // setting internal values
  stored_name = inname;
  // setting default values
  lbound = 0;  ubound = 99;
  zeromaxokay = true;
  // creating the widgets
  min_label = new QLabel((shortnames)?("min:"):("minimum:"));
  max_label = new QLabel((shortnames)?("max:"):("maximum:"));
  min_entry = new QSpinBox();
  max_entry = new QSpinBox();
  // calling the layout Setup
  SetupLayout(wrapped,horizontal);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking
//-----------------------------
bool jfMinMaxEditor::Check() const {
  int maxval = max_entry->value();
  if ((maxval==0) && zeromaxokay) return true;
  return maxval>=(min_entry->value());
}
//-----------------------------
bool jfMinMaxEditor::CheckMessage() const {
  // varibales
  const QString emsgmsg1 = "The maximum count is smaller than the minimum.";
  QString emsgtitle = "A problem with the ";
  QMessageBox emsg;
  // starting
  bool cres = Check();
  if (!cres) {
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(emsgmsg1);
    emsg.setWindowTitle(emsgtitle+stored_name);
    emsg.exec();
  }
  return cres;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting things
//-----------------------------
bool jfMinMaxEditor::SetMin(int inval) {
  if (inval<lbound) return false;
  if (inval>ubound) return false;
  min_entry->setValue(inval);
  return true;
}
//-----------------------------
bool jfMinMaxEditor::SetMax(int inval) {
  if (inval<lbound) return false;
  if (inval>ubound) return false;
  max_entry->setValue(inval);
  return true;
}
//-----------------------------
bool jfMinMaxEditor::SetMinMax(int in_min, int in_max) {
  if (in_min<lbound) return false;
  if (in_max>ubound) return false;
  if (in_max<in_min) {
    if (zeromaxokay && (in_max!=0)) return false;
    else if (!zeromaxokay) return false;
  }
  // setting values
  min_entry->setValue(in_min);
  max_entry->setValue(in_max);
  return true;
}
//-----------------------------
bool jfMinMaxEditor::SetMinMax(IntPair* invalue) {
  if (invalue==NULL) return false;
  return SetMinMax(invalue->A,invalue->B);
}
//-----------------------------
void jfMinMaxEditor::ResetValues() {
  min_entry->setValue(lbound);
  max_entry->setValue(ubound);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting things
//-----------------------------
int jfMinMaxEditor::GetMin() const {
  return min_entry->value();
}
//-----------------------------
int jfMinMaxEditor::GetMax() const {
  return max_entry->value();
}
//-----------------------------
IntPair* jfMinMaxEditor::GetMixMax() const {
  IntPair* result;
  result = new IntPair;
  result->A = min_entry->value();
  result->B = max_entry->value();
  return result;
}
//-----------------------------
bool jfMinMaxEditor::LoadIntoPair(IntPair* destination) const {
  if (destination==NULL) return false;
  destination->A = min_entry->value();
  destination->B = max_entry->value();
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//-----------------------------
void jfMinMaxEditor::SetupLayout(bool wrapped, bool horizontal) {
  // optional creation of the wrapper
  if (wrapped) {
    wrapper = new QGroupBox(stored_name);
    wrap_holder = new QHBoxLayout();
    wrap_holder->addWidget(wrapper,1);
  }
  else {
    wrapper = NULL;
    wrap_holder = NULL;
  }
  // making the main layout
  if (horizontal) {
    // horizontal row
    hlayout = new QHBoxLayout();
    hlayout->addWidget(min_label,0);
    hlayout->addWidget(min_entry,1);
    hlayout->addWidget(max_label,0);
    hlayout->addWidget(max_entry,1);
    // finishing
    if (wrapped) {
      wrapper->setLayout(hlayout);
      setLayout(wrap_holder);
    }
    else setLayout(hlayout);
  }
  else {
    // creating and adding the widgets
    vlayout = new QGridLayout();
    vlayout->addWidget(min_label,0,0,1,1,Qt::AlignLeft);
    vlayout->addWidget(max_label,0,1,1,1,Qt::AlignLeft);
    vlayout->addWidget(min_entry,1,0);
    vlayout->addWidget(max_entry,1,1);
    vlayout->setColumnStretch(0,1);
    vlayout->setColumnStretch(1,1);
    // finishing
    if (wrapped) {
      wrapper->setLayout(vlayout);
      setLayout(wrap_holder);
    }
    else setLayout(vlayout);
  }
  // done
}
//-----------------------------
bool jfMinMaxEditor::SetRange(int in_lbound, int in_ubound) {
  // ubound must be greater than lbound
  if (in_ubound<=in_lbound) return false;
  // setting
  zeromaxokay = false;
  lbound = in_lbound;
  ubound = in_ubound;
  min_entry->setMinimum(lbound);
  max_entry->setMinimum(lbound);
  min_entry->setMaximum(ubound);
  max_entry->setMaximum(ubound);
  // done
  return true;
}
//============================================================================
jfZeroToMaxEditor::jfZeroToMaxEditor(QString inname, bool wrapped, bool horizontal,
    bool shortnames, QWidget* parent):jfMinMaxEditor(inname,wrapped,horizontal,shortnames,parent) {
  // setting some things
  SetRange(0,999999999);
  zeromaxokay = true;
}

//----------------------------------------------------------------------------
bool jfZeroToMaxEditor::SetMaxVal(size_t inmaxval) {
  if (inmaxval==0) return false;
  SetRange(0,inmaxval);
  zeromaxokay = true;
  return true;
}
//==========================================================================
jfGenMinMaxEditor::jfGenMinMaxEditor(QString inname, bool wrapped, bool horizontal,
     bool shortnames, QWidget* parent):jfMinMaxEditor(inname, wrapped, horizontal,
          shortnames,parent) {

}

//--------------------------------------------------------------------------
bool jfGenMinMaxEditor::SetValueRange(int in_lbound, int in_ubound) {
  return SetRange(in_lbound,in_ubound);
}

//****************************************************************************
