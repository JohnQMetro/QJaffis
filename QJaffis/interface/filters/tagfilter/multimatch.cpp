/******************************************************************************
Name    :   multimatch.cpp
Author  :   John Q Metro
Purpose :   Widget for creating multi-tag-matchers
Created :   August 26, 2022
Updated :   August 28, 2022
******************************************************************************/
#include "multimatch.h"

/*****************************************************************************/
jfMultiMatchEditor::jfMultiMatchEditor(bool include_miss_name_input,QWidget* parent):jfExprEditBase(true,parent) {
    tagset_picker = new jfCheckList(false);
    topsizer->insertWidget(0,tagset_picker,0);
    right_column = new QVBoxLayout();
    right_column->addWidget(fcheck_btn,0,Qt::AlignTop);
    right_column->addStretch(1);
    right_column->addStretch(1);
    SetupMissInput(include_miss_name_input);
    topsizer->addLayout(right_column);
}
// --------------------------------------
void jfMultiMatchEditor::SetupMissInput(bool yes) {
    miss_report_allowed = yes;
    if (yes) {
        miss_counter = NULL; // to start with
        miss_name_input = new QLineEdit();
        miss_report = new QPushButton("r");
        miss_row = new QHBoxLayout();
        miss_row->addWidget(miss_name_input,4);
        miss_row->addWidget(miss_report,1);
        right_column->addLayout(miss_row);
        connect(miss_report,SIGNAL(clicked(bool)),this,SLOT(HandleMissReport(bool)));
    }
    else {
        miss_counter = NULL;
        miss_row = NULL;
        miss_name_input = NULL;
        miss_report = NULL;
    }
}
// ++++++++++++++++++++++++++++++++++++++++++++
void jfMultiMatchEditor::SetNamesToPickFrom(const QStringList& names) {
    tagset_list = names;
    tagset_picker->replaceItems(names);
}
// --------
QStringList jfMultiMatchEditor::GetPickedNames() const {
    QStringList result;
    for (int i = 0; i < tagset_list.size(); i++) {
        if (tagset_picker->GetBoolState(i)) {
            result.append(tagset_list.at(i));
        }
    }
    return result;
}
// --------
size_t jfMultiMatchEditor::SetPickedNames(const QStringList& names) {
    tagset_picker->SetCheckAll(Qt::Unchecked);
    return tagset_picker->CheckNames((&names));
}
// -------------------------------------------
jfSimpleExpr* jfMultiMatchEditor::CheckFilter(QString& outmessage) {
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
// -------------------------------------------
jfMultiMatchSource* jfMultiMatchEditor::getMatchData() const {

    jfSimpleExpr* result_expression = new jfSimpleExpr();
    if (!result_expression->FromString(getText())) {
        delete result_expression;
        return NULL;
    }
    jfMultiMatchSource* result = new jfMultiMatchSource;
    result->extraExpression = result_expression;
    result->listNamesToUse = GetPickedNames();
    result->miss_counter = NULL;

    if (miss_report_allowed) {
        QString filename = miss_name_input->text().trimmed();
        if (!filename.isEmpty()) {
            miss_counter = new jfStringCounter(true, false);
            result->miss_counter = miss_counter;
        }
    }

    return result;
}
// ------------------------------------------
bool jfMultiMatchEditor::CheckExpr(const QString& inexpr,bool& outempty,QString& outmessage) const {
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
// +++++++++++++++++++++++++++++++++++++++++
void jfMultiMatchEditor::HandleMissReport(bool value) {
    if (miss_report_allowed && (miss_counter != NULL)) {
        QString filename = miss_name_input->text().trimmed();
        if (!filename.isEmpty()) miss_counter->SaveRecordedCounts(filename,4);
    }
}

// ======================================================================================
jfPercentExprEdit::jfPercentExprEdit(bool include_miss_name_input, QWidget* parent):jfMultiMatchEditor(include_miss_name_input, parent) {
    percent_editor = new jfLabeledIntEdit(NULL, "Min % Matched", true ,0 ,100);
    match_empty = new QCheckBox("Match No Items");
    right_column->insertWidget(2, percent_editor);
    right_column->insertWidget(3, match_empty);

}
// ++++++++++++++++++++++++++++++++++++++++
// checking method
size_t jfPercentExprEdit::GetPercent() const {
    return (size_t)(percent_editor->GetValue());
}
// -------------------------------
bool jfPercentExprEdit::SetPercent(size_t percent_value) const {
    return percent_editor->SetValue((int)percent_value);
}
// -------------------------------
bool jfPercentExprEdit::IsMatchEmpty() const {
    return match_empty->isChecked();
}
// -------------------------------
void jfPercentExprEdit::SetMatchEmpty(bool value) {
    match_empty->setChecked(value);
}

/*****************************************************************************/
