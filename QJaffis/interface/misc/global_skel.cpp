/******************************************************************************
Name    :   global_skel.cpp
Author  :   John Q Metro
Purpose :   A global skeleton editor
Created :   December 2, 2009
Conversion to Qt started : September 15, 2013
Updated :   September 04, 2014
******************************************************************************/
#ifndef GLOBAL_SKEL_H_INCLUDED
  #include "global_skel.h"
#endif // GLOBAL_SKEL_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------------------------------------
#include <assert.h>
/*****************************************************************************/
// constructor
jfGlobalSkeletonEdit::jfGlobalSkeletonEdit(jfSkelCollection* icoll_link,QWidget* parent):QWidget(parent) {
  const QString fname = "jfGlobalSkeletonEdit::jfGlobalSkeletonEdit";
  // initializing things
  /**/JDEBUGLOG(fname,1);
  assert(icoll_link!=NULL);
  coll_link = icoll_link;
  single = NULL;
  multiple = NULL;
  ficpart = NULL;
  stype = jfs_SINGLE;
  i_index = 0;
  /**/JDEBUGLOG(fname,2);
  // we create and initialize gui elements
  // the list box
  selector = new QListWidget();
  selector->setSelectionMode(QAbstractItemView::SingleSelection);
  /**/JDEBUGLOG(fname,3);
  QStringList* items = coll_link->GetNameList();
  selector->addItems(*items);
  /**/JDEBUGLOG(fname,4);
  delete items;
  // we start with skeleton 1, which is single
  single = new jfSkelEditSingle(coll_link->AtIndexS(0));
  /**/JDEBUGLOG(fname,5);
  // creating the sizer and arranging
  topsizer = new QHBoxLayout();
  topsizer->addWidget(selector,1);
  topsizer->addWidget(single,4);
  /**/JDEBUGLOG(fname,6);
  // finishing off
  selector->item(0)->setSelected(true);
  /**/JDEBUGLOG(fname,7);
  // connecting
  connect(selector, SIGNAL(itemSelectionChanged()), this, SLOT(ListSelect()));
  setLayout(topsizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// evaluation
void jfGlobalSkeletonEdit::ListSelect() {
  size_t selidx = selector->currentRow();
  if (selidx!=i_index) {
    if ((coll_link->GetSkelType(selidx))==jfs_SINGLE) {
      ToSingle(coll_link->AtIndexS(selidx));
    }
    else if ((coll_link->GetSkelType(selidx))==jfs_MULTIPLE) {
      ToMultiple(coll_link->AtIndexM(selidx));
    }
    else if ((coll_link->GetSkelType(selidx))==jfs_FICPART) {
      ToFicPart(coll_link->AtIndexF(selidx));
    }
    else assert(false);
    i_index = selidx;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfGlobalSkeletonEdit::ToSingle(jfSkeletonSingle* inval) {
  // constants and variables
  const QString fname = "jfGlobalSkeletonEdit::ToSingle";
  bool atest;
  // starting
  /**/JDEBUGLOG(fname,1);
  assert(inval!=NULL);
  /**/JDEBUGLOG(fname,2);
  // single to single
  if (stype==jfs_SINGLE) {
    /**/JDEBUGLOG(fname,3);
    if (single==NULL) single = new jfSkelEditSingle(inval);
    else if ((single->GetSkeleton())==inval) return false;
    else single->ChangeSkeleton(inval);
    /**/JDEBUGLOG(fname,4);
  }
  // multiple to single
  else {
    /**/JDEBUGLOG(fname,5);
    if (single==NULL) single = new jfSkelEditSingle(inval);
    else single->ChangeSkeleton(inval);
    /**/JDEBUGLOG(fname,6);
    atest = ChangePanel(single);
    assert(atest);
  }
  stype = jfs_SINGLE;
  /**/JDEBUGLOG(fname,7);
  // done
  return true;
}
//-------------------------------------------------------------------
bool jfGlobalSkeletonEdit::ToMultiple(jfSkeletonMultiple* inval) {
  // constants and variables
  const QString fname = "jfGlobalSkeletonEdit::ToMultiple";
  bool atest;
  // starting
  /**/JDEBUGLOG(fname,1);
  assert(inval!=NULL);
  // multiple to multiple
  if (stype==jfs_MULTIPLE) {
    /**/JDEBUGLOG(fname,2);
    if (multiple==NULL) multiple = new jfSkelEditMultiple(inval);
    else if ((multiple->GetSkeleton())==inval) return false;
    else multiple->ChangeSkeleton(inval);
    /**/JDEBUGLOG(fname,3);
  }
  // single to multiple
  else {
    /**/JDEBUGLOG(fname,4);
    if (multiple==NULL) multiple = new jfSkelEditMultiple(inval);
    else multiple->ChangeSkeleton(inval);
    /**/JDEBUGLOG(fname,5);
    // we now change the panels
    atest = ChangePanel(multiple);
    assert(atest);
  }
  stype = jfs_MULTIPLE;
  /**/JDEBUGLOG(fname,6);
  // done
  return true;
}
//------------------------------------------------------------------
bool jfGlobalSkeletonEdit::ToFicPart(jfSkeletonFicPart* inval) {
  // constants and variables
  const QString fname = "jfGlobalSkeletonEdit::ToFicPart";
  bool atest;
  // starting
  /**/JDEBUGLOG(fname,1);
  assert(inval!=NULL);
  // ficpart to ficpart
  if (stype==jfs_FICPART) {
    /**/JDEBUGLOG(fname,2);
    if (ficpart==NULL) ficpart = new jfSkelEditFicPart(inval);
    else if ((ficpart->GetSkeleton())==inval) return false;
    else ficpart->ChangeSkeleton(inval);
    /**/JDEBUGLOG(fname,3);
  }
  // single to multiple
  else {
    /**/JDEBUGLOG(fname,4);
    if (ficpart==NULL) ficpart = new jfSkelEditFicPart(inval);
    else ficpart->ChangeSkeleton(inval);
    /**/JDEBUGLOG(fname,5);
    // we now change the panels
    atest = ChangePanel(ficpart);
    assert(atest);
  }
  stype = jfs_FICPART;
  /**/JDEBUGLOG(fname,6);
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// more helper methods
bool jfGlobalSkeletonEdit::ChangePanel(jfSkelEditCore* inskel) {
  // special cases
  if (inskel==NULL) return false;
  if (stype==(inskel->GetSkelType())) return false;
  // general
  inskel->show();
  if (stype==jfs_SINGLE) {
    topsizer->removeWidget(single);
    topsizer->addWidget(inskel,4);
    single->hide();
  }
  else if (stype==jfs_MULTIPLE) {
    topsizer->removeWidget(multiple);
    topsizer->addWidget(inskel,4);
    multiple->hide();
  }
  else if (stype==jfs_FICPART) {
    topsizer->removeWidget(ficpart);
    topsizer->addWidget(inskel,4);
    ficpart->hide();
  }
  else return false;
  topsizer->update();
  return true;
}
/*****************************************************************************/
