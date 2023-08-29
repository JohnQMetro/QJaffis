/******************************************************************************
Name    :   ao3_special3.cpp
Author  :   John Q Metro
Purpose :   Defines some interface panels for some AO3 Filters
Created :   May 14, 2014
Updated :   April 16, 2023
******************************************************************************/
#ifndef AO3_SPECIAL3_H
  #include "ao3_special3.h"
#endif // AO3_SPECIAL3_H
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------
#include <assert.h>
#include <QMessageBox>
/*****************************************************************************/
// constructor
jfAO3FandomFilterEditor::jfAO3FandomFilterEditor(const jfAO3FandomFilter* infilt,
                             QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
  typed_fpointer = infilt;
  // calling the custom methods
  MakeWidgets();
  ArrangeSizers2();
  // finalizing things
  insert = msizer;
  // calling the sizers
  ArrangeSizers();
  connect(crossover_box,SIGNAL(stateChanged(int)),this,SLOT(HandleCrossoverCheck(int)));
  // setting empty values
  /*
  if (infilt==NULL) {
    allmatch_box->setEnabled(false);
    perf_edit->setEnabled(false);
  }
  */
  // loafing the filter
  if (infilt != NULL) LoadFilter(typed_fpointer);
}
//+++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-----------------------------------
void jfAO3FandomFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  // variables
  const jfAO3FandomFilter* thefilt;
  QString omsg;
  bool xvalue;
  // basic preps
  assert(infilter!=NULL);
  thefilt = dynamic_cast<const jfAO3FandomFilter*>(infilter);
  filt_pointer = typed_fpointer = thefilt;
  // loading the name info
  NameLoad();
  // loading the main data
  main_edit->SetData(typed_fpointer->GetMainSrc(),omsg);
  // loading the crossover data
  xvalue = typed_fpointer->GetCrossover();
  crossover_box->setChecked(xvalue);
  allmatch_box->setChecked(typed_fpointer->GetAllMatch());
  perf_edit->SetData(typed_fpointer->GetPerFandomSrc(),omsg);
  // enabling or disabling basic on crossover
  /*
  allmatch_box->setEnabled(xvalue);
  perf_edit->setEnabled(xvalue);
  */
  // done
}
//-----------------------------------
jfFilterBase* jfAO3FandomFilterEditor::GetFilter() {
  // variables
  jfSimpleExpr* toutput;
  jfAO3FandomFilter* result;
  QString omsg;
  bool cvalue;
  // building the result
  result = new jfAO3FandomFilter("(dummy name");
  // adding main expression
  toutput = main_edit->CheckFilter(omsg);
  result->GeneralFromExpr(toutput);
  delete toutput;
  // adding crossover stuff
  cvalue = crossover_box->isChecked();
  result->SetCrossover(cvalue);
  result->SetAllMatch(allmatch_box->isChecked());
  toutput = perf_edit->CheckFilter(omsg);
  result->PerFandomFromExpr(toutput);
  delete toutput;
  // final details
  namedesc_edit->ChangeFilter(result);
  // done
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++
// checking
bool jfAO3FandomFilterEditor::GeneralCheck(const jfFilterMap* infmap) const {
    // local variables
    const QString pfmsg = "Problem with filter contents!";
    jfSimpleExpr* toutput;
    QString omsg;
    QMessageBox emsg;
    // first we see if the name is correct
    if (NameNUniq(infmap)) return false;
    // next, we check the main filter
    toutput = main_edit->CheckFilter(omsg);
    if ((toutput!=NULL) && (toutput->IsValid())) {
        // things are ok
        delete toutput;
        if (crossover_box->isChecked()) {
            toutput = main_edit->CheckFilter(omsg);
            if ((toutput!=NULL) && (toutput->IsValid())) {
                delete toutput;
                return true;
            }
        }
        else return true;
    }
    // things are not ok    // we are in error here
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(omsg);
    emsg.setWindowTitle(pfmsg);
    emsg.exec();
    if (toutput!=NULL) delete toutput;
    return false;
}
//+++++++++++++++++++++++++++++++++++++++++++
// event handlers
void jfAO3FandomFilterEditor::HandleCrossoverCheck(int state) {
  bool ccheck = crossover_box->isChecked();
  /*
  allmatch_box->setEnabled(ccheck);
  perf_edit->setEnabled(ccheck);
  */
  emit SendCrossCheck(ccheck);
}
//+++++++++++++++++++++++++++++++++++++++++++
// internal setup methods
//-----------------------------------
void jfAO3FandomFilterEditor::MakeWidgets() {
  // making the checkboxes
  crossover_box = new QCheckBox("Must have Multiple Fandoms");
  allmatch_box = new QCheckBox("All Fandoms instead");
  // making the other stuff
  perf_label = new QLabel("At least one fandom must match:");
  perf_edit = new jfSimpleExprEdit(true);
  main_label = new QLabel("The list of all the fandoms should match :");
  main_edit = new jfSimpleExprEdit(true);
}
//-----------------------------------
void jfAO3FandomFilterEditor::ArrangeSizers2() {
  csizer = new QHBoxLayout();
  msizer = new QVBoxLayout();
  csizer->addWidget(perf_label,1);
  csizer->addWidget(allmatch_box,0);
  msizer->addWidget(crossover_box,0);
  msizer->addSpacing(3);
  msizer->addLayout(csizer,0);
  msizer->addWidget(perf_edit,0);
  msizer->addSpacing(6);
  msizer->addWidget(main_label,0);
  msizer->addWidget(main_edit,1);
}

//====================================================================

jfAO3KudoFilterEditor::jfAO3KudoFilterEditor(const jfFavKudoFilter* infilt, QWidget* parent):
            jfZeroToMaxFilterEditor("Kudo Count Range",25000,infilt,parent) {
  // not much
  insert_panel->SetMinMax(0,0);
}

//--------------------------------------------------------------------
jfMinMaxUFilter* jfAO3KudoFilterEditor::MakeTypedMinMax() const {
  return new jfFavKudoFilter("(dummy name)");
}
/*****************************************************************************/
