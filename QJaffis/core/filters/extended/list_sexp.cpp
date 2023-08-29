//***************************************************************************
// Name:        list_sexp.cpp
// Author :     John Q Metro
// Purpose :    String expression based filters for lists of strings
// Created:     February 26, 2023
// Updated:     April 9, 2023
//***************************************************************************
#include "list_sexp.h"

#ifndef FICBASE_H
    #include "../../../fanfic/data/ficbase.h"
#endif // FICBASE_H
//***************************************************************************
// the constructors
//----------------------------------------------------------------------
jfListExprFilterBase::jfListExprFilterBase(const QString& filter_name):jfFilterBase(filter_name) {
    parsed_expression = NULL;
    match_mode = jfListMatchMode::MATCH_MERGED;
}
//-----------------------------
jfListExprFilterBase::jfListExprFilterBase(QString&& filter_name):jfFilterBase(filter_name) {
    parsed_expression = NULL;
    match_mode = jfListMatchMode::MATCH_MERGED;
}
//-----------------------------
jfListExprFilterBase::jfListExprFilterBase(jfListMatchMode in_match_mode, jfSimpleExpr* in_source):jfFilterBase("(List Filter)") {
    parsed_expression = NULL;
    bool validdata = FromExpr(in_source);
    assert(validdata);
    match_mode = in_match_mode;
}
//-----------------------------
jfListExprFilterBase::jfListExprFilterBase(const QString& filter_name,
        jfListMatchMode in_match_mode, jfSimpleExpr* in_source):jfFilterBase(filter_name) {
    parsed_expression = NULL;
    bool validdata = FromExpr(in_source);
    assert(validdata);
    match_mode = in_match_mode;
}
//-----------------------------
jfListExprFilterBase::jfListExprFilterBase(const jfListExprFilterBase& source):jfFilterBase(source.name) {
    description = source.description;
    match_mode = source.match_mode;
    if (source.parsed_expression == NULL) {
        parsed_expression = NULL;
    }
    else {
        parsed_expression = source.parsed_expression->Copy();
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfListExprFilterBase::IsEmpty() const {
    return (parsed_expression == NULL) || (parsed_expression->isEmpty());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual functions
QString jfListExprFilterBase::ToString() const {
    if (parsed_expression == NULL) return "";
    else {
        jfOutString resb;
        resb << ModeToString(match_mode) << parsed_expression->GetSrc();
        return resb;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// modifying and checking the contents
//----------------------------------------------------------------------
jfListMatchMode jfListExprFilterBase::GetMatchMode() const {
    return match_mode;
}
//----------------------------------------------------------------------
void jfListExprFilterBase::SetMatchMode(jfListMatchMode in_match_mode) {
    match_mode = in_match_mode;
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::FromExpr(jfSimpleExpr* in_source) {
  if (in_source==NULL) return false;
  else if ((in_source->IsValid()) == false) return false;
  else {
      if (parsed_expression != NULL) {
          delete parsed_expression;
      }
      parsed_expression = in_source->Copy();
      return true;
  }
}
//----------------------------------------------------------------------
// returning a string representation of the parsed contents
/*
QString jfListExprFilterBase::MakePList() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->MakePList();
}
*/
//-----------------------------------------------------------------------
// sort of like load string, except the string is empty
void jfListExprFilterBase::EmptyFilter() {
    if (parsed_expression != NULL) {
        delete parsed_expression;
        parsed_expression = NULL;
    }
    match_mode = jfListMatchMode::MATCH_MERGED;
}
//-----------------------------------------------------------------------
QString jfListExprFilterBase::GetExpression() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->GetSrc();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfListExprFilterBase::~jfListExprFilterBase() {
    EmptyFilter();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfListExprFilterBase::FromStringInner(const QString& sourcedata, QString& error_out) {
    const QString fname = "jfListExprFilterBase::FromStringInner";

    if (sourcedata.isEmpty()) {
        EmptyFilter();
        return true;
    }
    else {
       jfLineParse* lparser = new jfLineParse(sourcedata);
       if (lparser->NNotX(2)) {
           delete lparser;
           error_out = "The input string was not 2 fields!";
           return false;
       }
       // the first field should be the match mode
       jfListMatchMode in_match_mode = StringToMode(lparser->Get(0));
       jfSimpleExpr* new_expr = new jfSimpleExpr(lparser->UnEscStr(1));
       delete lparser;
       bool okay_expr = (new_expr->IsValid()) || (new_expr->isEmpty());
       if (okay_expr) {
           EmptyFilter();
           match_mode = in_match_mode;
           if (new_expr->isEmpty()) parsed_expression = NULL;
           else parsed_expression = new_expr;
       }
       else {
           error_out = new_expr->parse_error;
           delete new_expr;
       }
       return okay_expr;
    }
}
// -----------------------------------------------------------------------------
QString jfListExprFilterBase::ModeToString(jfListMatchMode in_match_mode) const {
    if (in_match_mode == jfListMatchMode::MATCH_ALL) return "ALL";
    else if (in_match_mode == jfListMatchMode::MATCH_ONE) return "ONE";
    else if (in_match_mode == jfListMatchMode::MATCH_75P) return "75P";
    else return "MERGED";
}
//----------------------------------------------------------------------
jfListMatchMode jfListExprFilterBase::StringToMode(const QString& mode_source) const {
    if (mode_source.compare("ALL", Qt::CaseInsensitive) == 0) return jfListMatchMode::MATCH_ALL;
    else if (mode_source.compare("ONE", Qt::CaseInsensitive) == 0) return jfListMatchMode::MATCH_ONE;
    else if (mode_source.compare("75P", Qt::CaseInsensitive) == 0) return jfListMatchMode::MATCH_75P;
    else return jfListMatchMode::MATCH_MERGED;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match methods
//----------------------------------------------------------------------
bool jfListExprFilterBase::MergeMatch(const QStringList& to_match) const {
    QString joined_list = to_match.join(QChar(','));
    return parsed_expression->InternalMatch(joined_list);
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::ItemwiseMatchAll(const QStringList& to_match) const {
    for (QString item : to_match) {
        bool did_match = parsed_expression->InternalMatch(item);
        if (! did_match) return false;
    }
    return true;
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::ItemwiseMatchAny(const QStringList& to_match) const {
    for (QString item : to_match) {
        bool did_match = parsed_expression->InternalMatch(item);
        if (did_match) return true;
    }
    return false;
}
//----------------------------------------------------------------------
int jfListExprFilterBase::ItemwiseMatchCount(const QStringList& to_match) const {
    int match_count = 0;
    for (QString item : to_match) {
        bool did_match = parsed_expression->InternalMatch(item);
        if (did_match) match_count++;
    }
    return match_count;
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::InternalMatch(const QStringList& to_match) const {
    if ((parsed_expression == NULL) || (parsed_expression->isEmpty())) return true;
    else if (to_match.isEmpty()) return false;
    else {
        if (match_mode == jfListMatchMode::MATCH_ALL) {
            return ItemwiseMatchAll(to_match);
        }
        else if (match_mode == jfListMatchMode::MATCH_ONE) {
            return ItemwiseMatchAny(to_match);
        }
        else if (match_mode == jfListMatchMode::MATCH_75P) {
            int mcount = ItemwiseMatchCount(to_match);
            if (mcount == to_match.size()) return true;
            else {
                double frac = ((double)mcount) / (double)(to_match.size());
                return frac >= 0.75;
            }
        }
        else {
            return MergeMatch(to_match);
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void jfListExprFilterBase::CoreCopy(const jfListExprFilterBase& source) {
    name = source.name;
    description = source.description;
    EmptyFilter();
    if (source.parsed_expression == NULL) {
        parsed_expression = NULL;
    }
    else {
        parsed_expression = source.parsed_expression->Copy();
    }
}

// ==========================================================================================
const jfFilterTypeMeta CHARACTER_LIST_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::CHARACTERS, "CharacterListFilter",
                     "Character List Filter", QString("The Character List ") +
                     "Expression Filter matches the list of characters against" +
                     " a boolean expression, the elements of which are themselves" +
                     " to be matched. These elements are either strings or " +
                     "substrings. The user can choose to match a merged list, all" +
                     " characters, or at least one character.",
          IdForGenericFanfic2(), createFilter<jfCharListExprFilter> );
// =======================================================================
// constructors
jfCharListExprFilter::jfCharListExprFilter(const QString& filter_name):jfListExprFilterBase(filter_name) {

}
//----------------------------------------
jfCharListExprFilter::jfCharListExprFilter(QString&& filter_name):jfListExprFilterBase(filter_name) {

}
//----------------------------------------
jfCharListExprFilter::jfCharListExprFilter(const jfCharListExprFilter& source):jfListExprFilterBase(source) {

}
//----------------------------------------
jfCharListExprFilter::jfCharListExprFilter(const QString& filter_name,
        jfListMatchMode in_match_mode, jfSimpleExpr* in_source):jfListExprFilterBase(in_match_mode, in_source) {

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------
jfCharListExprFilter* jfCharListExprFilter::Copy() const {
  return new jfCharListExprFilter(*this);
}
//------------------------------------------------------------
jfFilterBase* jfCharListExprFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
const jfFilterTypeMeta& jfCharListExprFilter::GetTypeMetaInfo() const {
    return CHARACTER_LIST_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfCharListExprFilter::CoreMatch(const jfSearchResultItem* testelem) const {
    const jfGenericFanfic2* fanfic_item = NULL;
    // checks and starts
    assert(testelem!=NULL);
    // type checking
    fanfic_item = dynamic_cast<const jfGenericFanfic2*>(testelem);
    return InternalMatch(fanfic_item->GetCharacterList());
}

//*******************************************************************************************
