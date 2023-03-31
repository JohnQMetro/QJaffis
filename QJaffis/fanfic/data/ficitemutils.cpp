/*****************************************************************************
Name    : ficitemutils.cpp
Basic   : Classes and stuff that help with Fanfic Item manipulation
Author  : John Q Metro
Started : March 11, 2023
Updated : March 16, 2023
Notes   : Replaces some parsing and output stuff
******************************************************************************/
#include "ficitemutils.h"
// ---------------------------------------------------

/*****************************************************************************/
jfFanficItemOutputterBase::jfFanficItemOutputterBase(const jfDisplayHTMLHelper* in_html_helper):jfResultItemOutputter() {
    fanfic_source = NULL;
    html_helper = in_html_helper;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFanficItemOutputterBase::~jfFanficItemOutputterBase() {
    fanfic_source = NULL;
    html_helper = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFanficItemOutputterBase::DisplayHTMLHeader() const {
    const QString fname = "jfFanficItemOutputterBase::DisplayHTMLHeader";

    // we start with the table
    QString result = "<table width=99%";
    // background color based on fic marks...
    // if (!texmpx.isEmpty()) result += " bgcolor=\"#" + texmpx + "\"";
    result += "><tr><td>";
    // building the title line
    result += "<font size=+3 color=#010101><a href=\"";
    result += fanfic_source->GetURL() + "\">";
    result += fanfic_source->GetTitle() + "</a>";
    // adding the author stuff
    result += " by " + fanfic_source->GetAuthor();
    result += "</font> ";
    // status (if there)
    if (flags != NULL) {
        result += MakeUpdateStatusBlock(flags->ustatus);
    }
    return result;
}
// ----------------------------------------------------------------------
QString jfFanficItemOutputterBase::DisplayHTMLHeader(const jfDisplayHTMLHelper* helper ) const {

    // we start with the table
    QString result = "<table width=99%";
    // background color based on fic marks...
    // if (!texmpx.isEmpty()) result += " bgcolor=\"#" + texmpx + "\"";
    result += "><tr><td>";
    // building the title line
    result += helper->WrapHyperlink(fanfic_source->GetURL(),"fictitle",  fanfic_source->GetTitle());
    // adding the author stuff
    result += helper->WrapText("fictitle", " by " + fanfic_source->GetAuthor());

    // status (if there)
    if (flags != NULL) {
        result += helper->UpdateStatus(flags->ustatus);
    }
    return result;
}

// =============================================================================
// =============================================================================

jfFicExtractParseBase::jfFicExtractParseBase(bool in_case_sensetive) {
    has_result = false;
    result_state = jfItemParseResultState::DEFECTIVE;
    untyped_item = NULL;
    case_sensetive = in_case_sensetive;
}
//-----------------------
jfItemParseResultState jfFicExtractParseBase::ParseFromSource(const QString& source) {
    ClearCore(false);
    xparser.ChangeData(source, case_sensetive);
    result_state = InnerParse();
    has_result = true;
    return result_state;
}

// +++++++++++++++++++++++++++++++++++++++
// result getting
bool jfFicExtractParseBase::HasResult() const {
    return has_result;
}
//-----------------------
jfItemParseResultState jfFicExtractParseBase::LastResultState() const {
    return result_state;
}
//-----------------------
const QString& jfFicExtractParseBase::LastError() const {
    return error_message;
}
// +++++++++++++++++++++++++++++++++++++++
jfFicExtractParseBase::~jfFicExtractParseBase() {
    ClearCore(true);
}
//+++++++++++++++++++++++++++++++++++++++++
jfItemParseResultState jfFicExtractParseBase::SetError(const QString err_msg) {
    error_message = err_msg;
    ClearItem();
    return jfItemParseResultState::FAILURE;
}
//++++++++++++++++++++++++++++++++++++++++++
void jfFicExtractParseBase::ClearCore(bool clear_source) {
    if (untyped_item != NULL) {
        delete untyped_item;
        untyped_item = NULL;
    }
    has_result = false;
    result_state = jfItemParseResultState::DEFECTIVE;
    error_message.clear();
    if (clear_source) xparser.Clear();
}
/*****************************************************************************/
