/******************************************************************************
Name    :   listexpr_edit.cpp
Author  :   John Q Metro
Purpose :   An expression editor for list expression filters
Created :   March 2, 2023
Updated :   April 16, 2023
******************************************************************************/
#include "listexpr_edit.h"

#include <QMessageBox>
/*****************************************************************************/
jfListExprEdit::jfListExprEdit(bool mline_in, QWidget* parent):jfExprEditBase(mline_in,parent) {
    // match type picker
    match_type_picker = new QComboBox();
    match_type_picker->addItem("Merge & Match");
    match_type_picker->addItem("Match All");
    match_type_picker->addItem("Match One+");
    match_type_picker->addItem("Match 75%+");
    match_type_picker->setCurrentIndex(0);

    // finishing the layout
    // for multiline, the picker goes under the check button
    if (mline_in) {
        right_column = new QVBoxLayout();
        right_column->addWidget(fcheck_btn, 0, Qt::AlignHCenter|Qt::AlignTop);
        right_column->addWidget(match_type_picker, 0 , Qt::AlignHCenter|Qt::AlignTop);
        right_column->addStretch(1);
        topsizer->addLayout(right_column,0);
    }
    // otherwise, it goes to the right of the check button
    else {
        right_column = NULL;
        topsizer->addWidget(fcheck_btn,0,Qt::AlignTop);
        topsizer->addWidget(match_type_picker,0,Qt::AlignTop);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match mode
// -----------------------------
jfListMatchMode jfListExprEdit::GetMatchMode() const {
    int mtp_index = match_type_picker->currentIndex();
    if (mtp_index <= 0) return jfListMatchMode::MATCH_MERGED;
    else if (mtp_index == 1) return jfListMatchMode::MATCH_ALL;
    else if (mtp_index == 2) return jfListMatchMode::MATCH_ONE;
    else return jfListMatchMode::MATCH_75P;
}
// -----------------------------
void jfListExprEdit::SetMatchMode(jfListMatchMode match_mode_in) {
    if (match_mode_in == jfListMatchMode::MATCH_MERGED) match_type_picker->setCurrentIndex(0);
    else if (match_mode_in == jfListMatchMode::MATCH_ALL) match_type_picker->setCurrentIndex(1);
    else if (match_mode_in == jfListMatchMode::MATCH_ONE) match_type_picker->setCurrentIndex(2);
    else match_type_picker->setCurrentIndex(3);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSimpleExpr* jfListExprEdit::CheckFilter(QString& outmessage) {
  // local variables
  QString dstring;
  jfSimpleExpr *result_expression;
  // starting...
  dstring = getText();
  // empty expresssions
  if (dstring.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    result_expression = new jfSimpleExpr(dstring);
    SetStatus(0);
    return result_expression;
  }
  // non-empty expressions
  else {
    result_expression = new jfSimpleExpr();
    if (!result_expression->FromString(dstring)) {
      outmessage = result_expression->parse_error;
      delete result_expression;
      SetStatus(2);
      return NULL;
    }
    outmessage="No errors detected.";
    SetStatus(0);
    return result_expression;
  }
}
//-----------------------------------------------------------------------------
/* This method treats in input string as an expression and reports on it's
validity */
bool jfListExprEdit::CheckExpr(const QString& inexpr,bool& outempty, QString& outmessage) const {
  // local variables
  jfSimpleExpr *result_expression;
  QString inval = inexpr.trimmed();
  // starting...
  // empty expresssions
  if (inval.isEmpty()) {
    outmessage = "Empty expressions match everything.";
    outempty = true;
    return true;
  }
  // non-empty expressions
  else {
    outempty = false;
    result_expression = new jfSimpleExpr();
    if (!result_expression->FromString(inval)) {
      outmessage = result_expression->parse_error;
      delete result_expression;
      return false;
    }
    // we check for validity
    outmessage="No errors detected.";
    delete result_expression;
    return true;
  }
}
// ============================================================================
// messaages
const QString pfmsg = "Problem with filter contents!";
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the default constructor
jfBaseListExprFilterEditor::jfBaseListExprFilterEditor(const jfListExprFilterBase* infilt,
                                     QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
  typed_fpointer = infilt;
  // we create the insert.. the *actual* expression editor
  insert_panel = new jfListExprEdit(true);
  insert_panel->SetData(typed_fpointer->GetExpression(),omsg);
  insert_panel->SetMatchMode(typed_fpointer->GetMatchMode());
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
  if (infilt!=NULL) NameLoad();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfBaseListExprFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  assert(infilter!=NULL);
  QString omsg;
  // loading the data
  const jfListExprFilterBase* list_filter = dynamic_cast<const jfListExprFilterBase*>(infilter);
  filt_pointer = infilter;
  insert_panel->SetData(list_filter->GetExpression(),omsg);
  insert_panel->SetMatchMode(list_filter->GetMatchMode());
  NameLoad();
}
//------------------------------------------------------------------------------
jfFilterBase* jfBaseListExprFilterEditor::GetFilter() {
  jfSimpleExpr* toutput;
  jfListExprFilterBase* result;
  QString omsg;
  // getting the output
  toutput = insert_panel->CheckFilter(omsg);
  result = MakeTypedFilter(insert_panel->GetMatchMode(),toutput);
  namedesc_edit->ChangeFilter(result);
  // done
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfBaseListExprFilterEditor::GeneralCheck(const jfFilterMap* filter_group) const {
  // local variables
  jfSimpleExpr* toutput;
  QString omsg;
  QMessageBox emsg;
  // first we see if the name is correct
  if (NameNUniq(filter_group)) return false;
  // next, we check the filter
  toutput = insert_panel->CheckFilter(omsg);
  if ((toutput!=NULL) && (toutput->IsValid())) {
    // things are ok
    delete toutput;
    return true;
  }
  else {
    // things are not okay
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(omsg);
    emsg.setWindowTitle(pfmsg);
    emsg.exec();
    if (toutput!=NULL) delete toutput;
    return false;
  }
}
// ============================================================================

// the default constructor
jfCharacterListExprFilterEditor::jfCharacterListExprFilterEditor(
        const jfCharListExprFilter* infilt, QWidget* parent):
        jfBaseListExprFilterEditor(infilt, parent) {

}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfListExprFilterBase* jfCharacterListExprFilterEditor::MakeTypedFilter(
        jfListMatchMode match_mode, jfSimpleExpr* inpval) {
    return new jfCharListExprFilter("(dummy naame)",match_mode, inpval);
}
/*****************************************************************************/
