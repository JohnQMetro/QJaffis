/******************************************************************************
Name    :   filter_match.cpp
Author  :   John Q Metro
Purpose :   Widget for creating filtered percentage multi-tag-matchers
Created :   August 26, 2022
Updated :   August 27, 2022
******************************************************************************/

#include "filter_match.h"

/*****************************************************************************/

jfFilteredTagMatcherEdit::jfFilteredTagMatcherEdit(bool include_miss_name_input, QWidget* parent):QWidget(parent) {
    main_layout = new QVBoxLayout();
    prefilter_label = new QLabel("The edit below is used to identify Tags to ignore.");
    prefilter_edit = new jfMultiMatchEditor(false);
    match_label = new QLabel("The edit below is for picking tags that count.");
    match_edit = new jfPercentExprEdit(include_miss_name_input);
    main_layout->addWidget(prefilter_label);
    main_layout->addWidget(prefilter_edit,2);
    main_layout->addWidget(match_label);
    main_layout->addWidget(match_edit,3);
    setLayout(main_layout);
}
// ++++++++++++++++++++++++++++++++++
void jfFilteredTagMatcherEdit::SetNamesToPickFrom(const QStringList& names) {
    prefilter_edit->SetNamesToPickFrom(names);
    match_edit->SetNamesToPickFrom(names);
}
// ++++++++++++++++++++++++++++++++++
// getting data
// -----------------------
jfMultiMatchSource* jfFilteredTagMatcherEdit::GetPrefilterData() const {
    return prefilter_edit->getMatchData();
}
// -----------------------
jfSimpleExpr* jfFilteredTagMatcherEdit::CheckPrefilterExpression(QString& outmessage) {
    return prefilter_edit->CheckFilter(outmessage);
}
// -----------------------
jfMultiMatchSource* jfFilteredTagMatcherEdit::GetMatchData() const {
    return match_edit->getMatchData();
}
// -----------------------
jfSimpleExpr* jfFilteredTagMatcherEdit::CheckMatchExpression(QString& outmessage) {
    return match_edit->CheckFilter(outmessage);
}
// -----------------------
size_t jfFilteredTagMatcherEdit::GetPercent() const {
    return match_edit->GetPercent();
}
// -----------------------
bool jfFilteredTagMatcherEdit::IsMatchEmpty() const {
    return match_edit->IsMatchEmpty();
}
// -----------------------
bool jfFilteredTagMatcherEdit::AreExpressionsValid(bool& in_prefilter, QString& outmessage) {
    // local variables
    jfSimpleExpr* toutput;

    // checking the prefilter
    in_prefilter = true;
    toutput = prefilter_edit->CheckFilter(outmessage);
    bool bad = (toutput == NULL) || (!toutput->IsValid());
    if (toutput != NULL) delete toutput;
    if (bad) return false;

    toutput = NULL;

    // checking the main matcher
    in_prefilter = false;
    toutput = match_edit->CheckFilter(outmessage);
    bad = (toutput == NULL) || (!toutput->IsValid());
    if (toutput != NULL) delete toutput;

    return !bad;
}
// ++++++++++++++++++++++++++++++++++
// setting prefilter data
// -----------------------
void jfFilteredTagMatcherEdit::SetPrefilterNames(const QStringList& names) {
    prefilter_edit->SetPickedNames(names);
}
// -----------------------
bool jfFilteredTagMatcherEdit::SetPrefilterExpression(const QString& inexpr,QString& outmessage) {
    return prefilter_edit->SetData(inexpr, outmessage);
}
// ++++++++++++++++++++++++++++++++++
// setting main matcher data
// -----------------------
void jfFilteredTagMatcherEdit::SetMatcherNames(const QStringList& names) {
    match_edit->SetPickedNames(names);
}
// -----------------------
bool jfFilteredTagMatcherEdit::SetMatcherExpression(const QString& inexpr,QString& outmessage) {
    return match_edit->SetData(inexpr, outmessage);
}
// -----------------------
bool jfFilteredTagMatcherEdit::SetPercent(size_t percent_value) {
    return match_edit->SetPercent(percent_value);
}
// -----------------------
void jfFilteredTagMatcherEdit::SetMatchEmpty(bool value) {
    match_edit->SetMatchEmpty(value);
}

/*
    QVBoxLayout* main_layout;
    jfMultiMatchEditor* prefilter_edit;
    jfPercentExprEdit* match_edit;
*/

/*****************************************************************************/
