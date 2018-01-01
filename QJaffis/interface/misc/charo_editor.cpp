/******************************************************************************
Name    :   charo_editor.cpp
Author  :   John Q Metro
Purpose :   Editor with a collection of checkboxes that have single-char i/o
Created :   September 8, 2012
Conversion to Qt Started October 18, 2013
Updated :   April 29, 2014
******************************************************************************/
#ifndef CHARO_EDITOR_H_INCLUDED
  #include "charo_editor.h"
#endif // CHARO_EDITOR_H_INCLUDED
//---------------------------------------------
#include <math.h>
#include <assert.h>
/*****************************************************************************/
jfCharCheckBoxGroup::jfCharCheckBoxGroup(QString inname, size_t icount, const QString inlabels[],
      const QString inchars, size_t incols,QWidget* parent):QWidget(parent) {
  // after the whopping paramater list...
  // we check...
  assert(icount>0);
  assert(incols<=icount);
  assert(inchars.length()==icount);
  // we don't check this
  cbcount = icount;
  mainlabel = inname;
  charvals = inchars;
  // starting the setup...
  SetupSizers(icount,incols);
  QCheckBox* temp;
  for (size_t cloop=0;cloop<icount;cloop++) {
    temp = new QCheckBox(inlabels[cloop]);
    cboxes.push_back(temp);
  }
  PopulateBoxes(cbcount,incols);
  // done
  setLayout(wsizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfCharCheckBoxGroup::SetFromString(const QString& inval) {
  // variables
  size_t rescount = 0, loopval;
  QChar tcahr;
  for (loopval=0;loopval<cbcount;loopval++) {
    tcahr = charvals[loopval];
    if (inval.contains(tcahr)) {
      cboxes[loopval]->setChecked(true);
      rescount++;
    }
    else cboxes[loopval]->setChecked(false);
  }
  return (rescount==inval.length());
}
//----------------------------------------------------
QString jfCharCheckBoxGroup::GetStringValue() const {
  QString result;
  for (size_t cloop=0;cloop<cbcount;cloop++) {
    if (cboxes[cloop]->isChecked()) result += charvals[cloop];
  }
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special
void jfCharCheckBoxGroup::CheckAll(bool doit) {
  for (size_t cloop=0;cloop<cbcount;cloop++) {
    cboxes[cloop]->setChecked(doit);
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfCharCheckBoxGroup::SetupSizers(size_t tcount, size_t colcount) {
  // starting
  mbox = new QGroupBox(mainlabel);
  // one column
  if (colcount==1) {
    mode = 1;
    tsizer = new QBoxLayout(QBoxLayout::TopToBottom);
    fsizer = NULL;
    usegrid = false;
  }
  // one row
  else if ((colcount==0) || (colcount==tcount)) {
    mode = 2;
    tsizer = new QBoxLayout(QBoxLayout::LeftToRight);
    fsizer = NULL;
    usegrid = false;
  }
  // grid
  else {
    mode = 3;
    fsizer = new QGridLayout();
    tsizer= NULL;
    usegrid = true;
  }
  // finishing
  if (usegrid) mbox->setLayout(fsizer);
  else mbox->setLayout(tsizer);
  wsizer = new QBoxLayout(QBoxLayout::TopToBottom);
  wsizer->addWidget(mbox,1);
}
//----------------------------------------------------------------------------
void jfCharCheckBoxGroup::PopulateBoxes(size_t tcount, size_t colcount) {
  // setup variables
  int rowin; float rowest;
  size_t cloop, rloop, tloop;
  // adding for non grid
  if (!usegrid) {
    for (cloop = 0; cloop<tcount; cloop++) {
      tsizer->addWidget(cboxes[cloop],0);
     }
  }
  // adding for grid
  else {
    tloop = 0;
    rowest = (float)(tcount) / (float)(colcount);
    rowin = ceil(rowest);
    for (rloop = 0; rloop<rowin; rloop++) {
      for (cloop = 0; cloop<colcount; cloop++) {
        fsizer->addWidget(cboxes[tloop],rloop,cloop);
        tloop++;
        if (tloop==tcount) break;
      }
    }
  }
}

/*****************************************************************************/
