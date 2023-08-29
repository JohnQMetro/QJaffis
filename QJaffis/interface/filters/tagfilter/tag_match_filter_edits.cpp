/******************************************************************************
Name    :   tag_match_filter_edits.cpp
Author  :   John Q Metro
Purpose :   Filter edit widgets for tag matchers
Created :   August 27, 2022
Updated :   April 16, 2023
******************************************************************************/
#include "tag_match_filter_edits.h"

#include <QMessageBox>
/*****************************************************************************/
// core wrapper for jfSimpleExprEdit
// the default constructor
jfPercentTagMatchFilterEditor::jfPercentTagMatchFilterEditor(
        const jfMatchPercentTagFilterBase* infilt, QWidget* parent):
        jfBaseFilterEditor(infilt, parent) {

    // most things are handled by the parent constructor
    assert(infilt!=NULL);
    // we create the insert.. the *actual* expression editor
    typed_fpointer = infilt;
    insert_panel = new jfPercentExprEdit(true);
    source_list = infilt->GetSourceList();
    insert_panel->SetNamesToPickFrom(source_list->ExcludeListNames());

    // loading the filter data
    QString omsg;
    insert_panel->SetData(infilt->MainExpression(), omsg);
    insert_panel->SetPickedNames(infilt->MainListNames());
    insert_panel->SetPercent(infilt->GetPercentMatch());
    insert_panel->SetMatchEmpty(infilt->GetMatchEmpty());
    // finalizing things
    insert = new QBoxLayout(QBoxLayout::TopToBottom);
    insert->addWidget(insert_panel,1);
    // calling the sizers
    ArrangeSizers();
    NameLoad();
}
// --------------------------------------------------
// the constructor to use if we have no filter to start with
jfPercentTagMatchFilterEditor::jfPercentTagMatchFilterEditor(
        const jfGeneralTagListsGroup* in_source_list, QWidget* parent):
        jfBaseFilterEditor(NULL, parent) {
    // most things are handled by the parent constructor
    assert(in_source_list!=NULL);
    // we create the insert.. the *actual* expression editor
    typed_fpointer = NULL;
    insert_panel = new jfPercentExprEdit(true);
    source_list = in_source_list;
    insert_panel->SetNamesToPickFrom(source_list->ExcludeListNames());

    // finalizing things
    insert = new QBoxLayout(QBoxLayout::TopToBottom);
    insert->addWidget(insert_panel,1);
    // calling the sizers
    ArrangeSizers();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
void jfPercentTagMatchFilterEditor::LoadFilter(const jfFilterBase* infilter) {
    assert(infilter!=NULL);

    // loading the filter object
    const jfMatchPercentTagFilterBase* temp_check = dynamic_cast<const jfMatchPercentTagFilterBase*>(infilter);
    assert(temp_check != NULL);
    filt_pointer = infilter;
    typed_fpointer = temp_check;

    // checking the source list
    const jfGeneralTagListsGroup* filter_source_list = temp_check->GetSourceList();
    if (source_list != filter_source_list) {
        source_list = filter_source_list;
        insert_panel->SetNamesToPickFrom(source_list->ExcludeListNames());
    }

    // loading filter data
    QString omsg;
    insert_panel->SetData(typed_fpointer->MainExpression(), omsg);
    insert_panel->SetPickedNames(typed_fpointer->MainListNames());
    insert_panel->SetPercent(typed_fpointer->GetPercentMatch());
    insert_panel->SetMatchEmpty(typed_fpointer->GetMatchEmpty());
    NameLoad();
}
// ----------------------------------------------
jfFilterBase* jfPercentTagMatchFilterEditor::GetFilter() {

    jfMatchPercentTagFilterBase* result = NULL;
    jfMultiMatchSource* match_source = NULL;

    match_source = insert_panel->getMatchData();
    if (match_source == NULL) return NULL;
    result = MakeTypedFilter(source_list);
    if (result == NULL) return NULL;

    result->SetFromSource(match_source);
    result->SetPercent(insert_panel->GetPercent(), insert_panel->IsMatchEmpty());

    namedesc_edit->ChangeFilter(result);
    // done
    return result;

}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// checking
bool jfPercentTagMatchFilterEditor::GeneralCheck(const jfFilterMap* infmap) const {
    // local variables
    jfSimpleExpr* toutput;
    QString omsg;
    QMessageBox emsg;
    // first we see if the name is correct
    if (NameNUniq(infmap)) return false;
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
      emsg.setWindowTitle("Problem with filter contents!");
      emsg.exec();
      if (toutput!=NULL) delete toutput;
      return false;
    }
}


/*****************************************************************************/
jfFilteredPercentTagMatchFilterEditor::jfFilteredPercentTagMatchFilterEditor(
        const jfMatchFilteredPercentTagFilterBase* infilt, QWidget* parent):
        jfBaseFilterEditor(infilt, parent) {

    // many things are handled by the parent constructor
    assert(infilt!=NULL);
    // we create the insert.. the *actual* editor
    typed_fpointer = infilt;
    insert_panel = new jfFilteredTagMatcherEdit(true);
    source_list = infilt->GetSourceList();
    insert_panel->SetNamesToPickFrom(source_list->ExcludeListNames());

    // loading the filter data
    QString omsg;
    // prefilter
    insert_panel->SetPrefilterExpression(infilt->PreFilterExpression(), omsg);
    insert_panel->SetPrefilterNames(infilt->PreFilterListNames());

    // main filter
    insert_panel->SetMatcherExpression(infilt->MainExpression(),omsg);
    insert_panel->SetMatcherNames(infilt->MainListNames());
    insert_panel->SetPercent(infilt->GetPercentMatch());
    insert_panel->SetMatchEmpty(infilt->GetMatchEmpty());

    // finalizing things
    insert = new QBoxLayout(QBoxLayout::TopToBottom);
    insert->addWidget(insert_panel,1);
    // calling the sizers
    ArrangeSizers();
    NameLoad();
}
// ---------------------------------------
jfFilteredPercentTagMatchFilterEditor::jfFilteredPercentTagMatchFilterEditor(
        const jfGeneralTagListsGroup* in_source_list, QWidget* parent):
        jfBaseFilterEditor(NULL, parent) {

    // many things are handled by the parent constructor
    assert(in_source_list!=NULL);
    // we create the insert.. the *actual* editor
    typed_fpointer = NULL;
    insert_panel = new jfFilteredTagMatcherEdit(true);
    source_list = in_source_list;
    insert_panel->SetNamesToPickFrom(source_list->ExcludeListNames());

    // finalizing things
    insert = new QBoxLayout(QBoxLayout::TopToBottom);
    insert->addWidget(insert_panel,1);
    // calling the sizers
    ArrangeSizers();
    NameLoad();
}
// ++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
// ---------------------------------------
void jfFilteredPercentTagMatchFilterEditor::LoadFilter(const jfFilterBase* infilter) {
    assert(infilter!=NULL);

    // loading the filter object
    const jfMatchFilteredPercentTagFilterBase* temp_check = dynamic_cast<const jfMatchFilteredPercentTagFilterBase*>(infilter);
    assert(temp_check != NULL);
    filt_pointer = infilter;
    typed_fpointer = temp_check;

    // checking the source list
    const jfGeneralTagListsGroup* filter_source_list = temp_check->GetSourceList();
    if (source_list != filter_source_list) {
        source_list = filter_source_list;
        insert_panel->SetNamesToPickFrom(source_list->ExcludeListNames());
    }

    QString omsg;
    // prefilter
    insert_panel->SetPrefilterExpression(typed_fpointer->PreFilterExpression(), omsg);
    insert_panel->SetPrefilterNames(typed_fpointer->PreFilterListNames());

    // main filter
    insert_panel->SetMatcherExpression(typed_fpointer->MainExpression(),omsg);
    insert_panel->SetMatcherNames(typed_fpointer->MainListNames());
    insert_panel->SetPercent(typed_fpointer->GetPercentMatch());
    insert_panel->SetMatchEmpty(typed_fpointer->GetMatchEmpty());

    NameLoad();
}
// ---------------------------------------
jfFilterBase* jfFilteredPercentTagMatchFilterEditor::GetFilter() {

    jfMatchFilteredPercentTagFilterBase* result = NULL;
    jfMultiMatchSource* prefilter_source = NULL;
    jfMultiMatchSource* match_source = NULL;

    match_source = insert_panel->GetMatchData();
    if (match_source == NULL) return NULL;
    prefilter_source = insert_panel->GetPrefilterData();
    if (prefilter_source == NULL) return NULL;

    result = MakeTypedFilter(source_list);
    if (result == NULL) return NULL;

    result->SetFromSource(match_source);
    result->SetPreFilterFromSource(prefilter_source);
    result->SetPercent(insert_panel->GetPercent(), insert_panel->IsMatchEmpty());

    namedesc_edit->ChangeFilter(result);
    // done
    return result;

}
// ++++++++++++++++++++++++++++++++++++++++++++++
// checking
bool jfFilteredPercentTagMatchFilterEditor::GeneralCheck(const jfFilterMap* infmap) const {
    // local variables
    QString omsg;
    QMessageBox emsg;
    // first we see if the name is correct
    if (NameNUniq(infmap)) return false;

    // next, we check the filters
    bool in_prefilter;
    if (!(insert_panel->AreExpressionsValid(in_prefilter, omsg))) {
        // things are not okay
        emsg.setIcon(QMessageBox::Critical);
        emsg.setText( ((in_prefilter) ? "Prefilter Expression: " : "Main Expression: ") + omsg);
        emsg.setWindowTitle("Problem with filter contents!");
        emsg.exec();
        return false;
    }
    else return true;
}

/*****************************************************************************/
