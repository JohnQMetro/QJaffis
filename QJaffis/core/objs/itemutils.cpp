/*****************************************************************************
Name    : itemutils.cpp
Basic   : Classes and stuff that help with Search Result Item manipulation
Author  : John Q Metro
Started : March 10, 2023
Updated : March 12, 2023
Notes   : Replaces stuff that was previous entirely in jfBasePD items.
******************************************************************************/

#include "itemutils.h"

#ifndef HTMLPARSE_H_INCLUDED
  #include "../utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED

/*****************************************************************************/

/* This is for converting jfSearchResultItem items to Text or HTML */
jfResultItemOutputter::jfResultItemOutputter() {
    source = NULL;
    flags = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// output
QString jfResultItemOutputter::GetSummaryExtract(size_t lines, size_t maxlen) const {
    if (source == NULL) return "";
    else return htmlparse::MakeShorterText(source->GetSummary(), lines, maxlen);
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfResultItemOutputter::~jfResultItemOutputter() {
    source = NULL;
    flags = NULL;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfResultItemOutputter::UpdateStatus2String(const jfUpdateStatus& inval) const {
    QString result;
    if (inval==jfUpdateStatus::NONE) result.clear();
    else if (inval==jfUpdateStatus::NEW) result = "New!";
    else if (inval==jfUpdateStatus::UPDATED) result = "Updated!";
    else if (inval==jfUpdateStatus::MISSING) result = "Missing!";
    else if (inval==jfUpdateStatus::AUTHORNC) result = "Author Name Changed!";
    else assert(false);
    // done
    return result;
}
// -----------------------------------------------------------------
QString jfResultItemOutputter::MakeUpdateStatusSpan(const jfUpdateStatus& ustatus) const {
    QString result;
    if (ustatus==jfUpdateStatus::NONE) return "";
    else {
        // the span indicating the text color
        result = "<span style=\"color:";
        if (ustatus == jfUpdateStatus::UPDATED) result += "lime";
        else if (ustatus == jfUpdateStatus::MISSING) result += "red";
        else if (ustatus == jfUpdateStatus::NEW) result += "aqua";
        else if (ustatus == jfUpdateStatus::AUTHORNC) result += "yellow";
        else assert(false);
        // middle part
        result += "\">[";
        // the label
        result += UpdateStatus2String(ustatus);
        // finishing
        result += "]</span>";
    }
    // done
    return result;
}
// ---------------------------------------------------------
QString jfResultItemOutputter::MakeUpdateStatusBlock(const jfUpdateStatus& ustatus) const {
    QString result = "";
    // status
    if (ustatus != jfUpdateStatus::NONE) {
      result += "<font size=+2 color=";
      if (ustatus == jfUpdateStatus::UPDATED) result += "Lime><b>[Updated";
      else if (ustatus == jfUpdateStatus::MISSING) result += "Red><b>[Missing";
      else if (ustatus == jfUpdateStatus::NEW) result += "Aqua><b>[New";
      else if (ustatus == jfUpdateStatus::AUTHORNC) result += "Yellow><b>[Author Name Change";
      result += "!]</b></font>";
    }
    return result;
}
// ==========================================================================
jfItemParseBase::jfItemParseBase(bool in_case_sensetive) {
    has_result = false;
    result_state = jfItemParseResultState::DEFECTIVE;
    untyped_item = NULL;
    case_sensetive = in_case_sensetive;
}
//-----------------------
jfItemParseResultState jfItemParseBase::ParseFromSource(const QString& source) {
    ClearCore(false);
    xparser.ChangeData(source, case_sensetive);
    result_state = InnerParse();
    has_result = true;
    return result_state;
}

// +++++++++++++++++++++++++++++++++++++++
// result getting
bool jfItemParseBase::HasResult() const {
    return has_result;
}
//-----------------------
jfItemParseResultState jfItemParseBase::LastResultState() const {
    return result_state;
}
//-----------------------
const jfSearchResultItem* jfItemParseBase::GetUntypedItem() {
    jfSearchResultItem* untyped_item_copy = untyped_item;
    if (untyped_item != NULL) untyped_item = NULL;
    return untyped_item_copy;
}
//-----------------------
const QString& jfItemParseBase::LastError() const {
    return error_message;
}
// +++++++++++++++++++++++++++++++++++++++
jfItemParseBase::~jfItemParseBase() {
    ClearCore(true);
}
//+++++++++++++++++++++++++++++++++++++++++
jfItemParseResultState jfItemParseBase::SetError(const QString err_msg) {
    error_message = err_msg;
    ClearItem();
    return jfItemParseResultState::FAILURE;
}
//++++++++++++++++++++++++++++++++++++++++++
void jfItemParseBase::ClearCore(bool clear_source) {
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
