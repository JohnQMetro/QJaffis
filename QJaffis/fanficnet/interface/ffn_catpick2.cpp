/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_catpick2.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net catgeory picker
// Created:     August 22, 2010
// Conversion to Qt started July 26, 2014
// Updated:     July 26, 2014
/////////////////////////////////////////////////////////////////////////////
#ifndef FFNCATPICK2
  #include "ffn_catpick2.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//--------------------------------------
#include <QMessageBox>
#include <assert.h>
//************************************************************************


jfCPickerDispl::jfCPickerDispl(jfFFN_CatP_SelCat* inselection, QWidget* parent):QWidget(parent) {
  // starting off
  assert(inselection!=NULL);
  selection_data = inselection;
  // setting up the GUI
  display = new QListWidget();
  del_btn = new QPushButton("Delete");
  top_sizer = new QVBoxLayout();
  top_sizer->addWidget(display,1);
  top_sizer->addWidget(del_btn,0,Qt::AlignRight);
  setLayout(top_sizer);
  // loading the values
  UpdateDisplay();
  // connecting
  connect(del_btn,SIGNAL(clicked(bool)),this,SLOT(HandleDelete(bool)));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
//------------------------------------
void jfCPickerDispl::UpdateDisplay() {
  // variables
  size_t itmcount, itmindex;
  QStringList* newitems;
  QString sel_string, curr_string;
  int storedindex;
  // gathering selection info
  if (display->currentRow()>=0) {
    sel_string = display->currentItem()->text();
  }
  // clearing the old items
  itmcount = display->count();
  if (itmcount>0) display->clear();
  // adding the new items
  newitems = selection_data->GetListing();
  if (newitems!=NULL) {
    storedindex = -1;
    itmcount = newitems->size();
    for (itmindex = 0 ; itmindex<itmcount ; itmindex++) {
      curr_string = newitems->at(itmindex);
      display->addItem(curr_string);
      if (curr_string==sel_string) storedindex = itmindex;
    }
    // reselecting
    if (storedindex>=0) display->setCurrentRow(storedindex);
  }
}

//------------------------------------
void jfCPickerDispl::Disable(bool yes) {
  display->setEnabled(!yes);
  del_btn->setEnabled(!yes);
}
//------------------------------------
bool jfCPickerDispl::ChangeSelector(jfFFN_CatP_SelCat* newselection) {
  if (newselection==NULL) return false;
  selection_data = newselection;
  UpdateDisplay();
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfCPickerDispl::HandleDelete(bool cclick) {
  const QString fname = "jfCPickerDispl::HandleDelete";
  QListWidgetItem* titem;
  QString dstring;
  // starting
  int sloc = display->currentRow();
  if (sloc>=0) {
    // deleting
    titem = display->takeItem(sloc);
    dstring = titem->text();
    delete titem;
    // new selection
    int itcount = display->count();
    if (sloc<itcount) display->setCurrentRow(sloc);
    else if (itcount>0) display->setCurrentRow(itcount-1);
    // done
    emit ItemDeleted(dstring);
  }
}

//*********************************************************************
    // the constructor
jf_FFN_CatPicker::jf_FFN_CatPicker(jfFFN_Categories* inmain, jfFFN_CatP_SelCat* inselection, QWidget* parent):QWidget(parent) {
// constants
  const QString fname="jf_FFN_CatPicker::jf_FFN_CatPicker";
  // variables
  QString omsg;
  bool atest;
  // checks
  assert(inmain!=NULL);
  assert(inselection!=NULL);
  /**/JDEBUGLOG(fname,1);
  // assigning some values
  main_catdata = inmain;
  selection_data = inselection;
  // we start creating gui elements
  panel_holder = new QTabWidget();
  section_panel = new jfFFN_SectionPicker(main_catdata,selection_data);
  crossover_panel = new jfFFN_CrossoverPicker(main_catdata,selection_data);
  picked_display = new jfCPickerDispl(selection_data);
  /**/JDEBUGLOG(fname,2);
  // assigning the notebook
  panel_holder->addTab(section_panel,"Regular Sections");
  panel_holder->addTab(crossover_panel,"Crossover Sections");
  // next up is the expression filters
  and_label = new QLabel("Catgeory must match :");
  and_edit = new jfSimpleExprEdit(false);
  atest = and_edit->SetData(inselection->GetAnd(),omsg);
  /**/JDEBUGLOGB(fname,3,atest);
  assert(atest);
  or_label = new QLabel("Include if Category matches :");
  or_edit = new jfSimpleExprEdit(false);
  atest = or_edit->SetData(inselection->GetOr(),omsg);
  /**/JDEBUGLOGB(fname,4,atest);
  assert(atest);
  // the filter sizer
  expredit_sizer = new QGridLayout();
  expredit_sizer->addWidget(and_label,0,0,Qt::AlignRight);
  expredit_sizer->addWidget(and_edit,0,1);
  expredit_sizer->addWidget(or_label,1,0,Qt::AlignRight);
  expredit_sizer->addWidget(or_edit,1,1);
  expredit_sizer->setColumnStretch(1,1);
  // arranging...
  /**/JDEBUGLOG(fname,5)
  middle_sizer = new QHBoxLayout();
  middle_sizer->addWidget(panel_holder,3);
  middle_sizer->addWidget(picked_display,1);
  // the top level sizer
  top_sizer = new QVBoxLayout();
  top_sizer->addLayout(middle_sizer,1);
  top_sizer->addLayout(expredit_sizer,0);
  // done
  setLayout(top_sizer);
  // connecting
  /**/JDEBUGLOG(fname,6)
  connect(section_panel,SIGNAL(sendCheckSig(bool)),this,SLOT(HandleCheck(bool)));
  connect(crossover_panel,SIGNAL(CheckSignal(bool)),this,SLOT(HandleCheck(bool)));
  connect(picked_display,SIGNAL(ItemDeleted(QString)),this,SLOT(HandleListDelete(QString)));
  /**/JDEBUGLOG(fname,7)

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jf_FFN_CatPicker::ChangeSelector(jfFFN_CatP_SelCat* newselection) {
  const QString fname = "jf_FFN_CatPicker::ChangeSelector";
  // assertion...
  if (newselection==NULL) return false;
  selection_data = newselection;
  section_panel->ChangeSelector(newselection);
  crossover_panel->ChangeSelector(newselection);
  picked_display->ChangeSelector(newselection);
  QString oomsg;
  and_edit->SetData(newselection->GetAnd(),oomsg);
  or_edit->SetData(newselection->GetOr(),oomsg);
  return true;
}
//--------------------------------------------------------------------
// saving the expression filters
bool jf_FFN_CatPicker::SaveFilters() {
  // variables
  const QString nvalid = "Expression Not Valid";
  QString omsg;
  jfSimpleExpr *result_expression;
  QMessageBox emsg;
  // we start, and filter first
  result_expression = and_edit->CheckFilter(omsg);
  if (result_expression==NULL) {
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(omsg);
    emsg.setWindowTitle(nvalid);
    emsg.exec();
    return false;
  }
  else {
    selection_data->SetAndFilter(result_expression);
  }
  // or filter next
  result_expression = or_edit->CheckFilter(omsg);
  if (result_expression==NULL) {
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(omsg);
    emsg.setWindowTitle(nvalid);
    emsg.exec();
    return false;
  }
  else {
    selection_data->SetOrFilter(result_expression);
  }
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handlers
//---------------------------------------------------------------
void jf_FFN_CatPicker::HandleCheck(bool xcheck) {
  picked_display->UpdateDisplay();
}
//-----------------------------------------------------------------------
void jf_FFN_CatPicker::HandleListDelete(QString in_dstring) {
  // constants and variables
  const QChar pstr = ';';
  QStringList* pvalue;
  // we parse
  pvalue = ListToStrArr(in_dstring, pstr );
  assert(pvalue!=NULL);
  if (pvalue->count()==2) {
    section_panel->UncheckAndRemove(pvalue->at(0),pvalue->at(1));
  }
  else if (pvalue->count()==3) {
    crossover_panel->UncheckAndRemove(pvalue->at(0),pvalue->at(1),pvalue->at(2));
  }
  delete pvalue;
}
//******************************************************************************
