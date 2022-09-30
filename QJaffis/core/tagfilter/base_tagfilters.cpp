//***************************************************************************
// Name:        base_tagfilters.cpp
// Author :     John Q Metro
// Purpose :    The base tag filters
// Created:     August 20, 2022
// Updated:     August 26, 2022
//***************************************************************************

#include "base_tagfilters.h"

#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
// ----------------------------------------------
#include <QStringList>
#include <assert.h>
//***************************************************************************

// base filter where a single match in a list counts
jfMatchOneTagFilterBase::jfMatchOneTagFilterBase(const jfGeneralTagListsGroup* in_source_list):jfMultiMatchBaseFilter() {
    assert(in_source_list != NULL);
    source_list = in_source_list;
}
// -----------------------------------
bool jfMatchOneTagFilterBase::ChangeSourceList(const jfGeneralTagListsGroup* in_source_list) {
    if (in_source_list == NULL) return false;
    source_list = in_source_list;
    return true;
}
// ----------------------------------------
bool jfMatchOneTagFilterBase::SetFromSource(jfMultiMatchSource* in_source) {
    if (main == NULL) main = new jfMatchFilterCore();
    else main->Clear();
    main->SetFromSource(in_source, (*source_list));
    validdata = !(main->BadExpression());
    return validdata;
}
// ++++++++++++++++++++++++++++++++++
// implemented methods
bool jfMatchOneTagFilterBase::isEmpty() const {
    return (main == NULL) || (main->IsEmpty());
}
// --------------------------------------
bool jfMatchOneTagFilterBase::FromString(const QString& sourcedata) {
    // splitting
    jfLineParse* lparser = new jfLineParse(sourcedata);
    if (lparser->NNotX(2)) {
        delete lparser;
        return false;
    }
    QString csv = lparser->UnEscStrE(0);
    QString exp = lparser->UnEscStrE(1);
    delete lparser;
    // creating main
    if (main == NULL) main = new jfMatchFilterCore();
    main->SetFromRawParts(csv, exp, (*source_list));
    validdata = !(main->BadExpression());
    return validdata;
}
// --------------------------------------
QString jfMatchOneTagFilterBase::ToString() const {
    jfOutString sform;
    sform << ((main == NULL)?"": main->GetListsCSV()) << ((main == NULL)?"": main->GetExpressionSource());
    return std::move(sform);
}
// ---------------------------------------
bool jfMatchOneTagFilterBase::IsUsable() const {
    if (main == NULL) return false;
    else return main->IsUsable();
}
// ++++++++++++++++++++++++++++++++++
jfMatchOneTagFilterBase::~jfMatchOneTagFilterBase() {
    source_list = NULL;
}
// ++++++++++++++++++++++++++++++++++
bool jfMatchOneTagFilterBase::InternalMatch(const QStringList& incheck) const {
    if ((main == NULL) || main->IsEmpty()) return true;
    else if (main->BadExpression()) return false;
    else return main->matcher->MatchesAny(incheck);
}

//================================================================================
// base percentage tag match filter

// the constructors
// -------------------------
jfMatchPercentTagFilterBase::jfMatchPercentTagFilterBase(const jfGeneralTagListsGroup* in_source_list):jfMultiMatchBaseFilter() {
    assert(in_source_list != NULL);
    source_list = in_source_list;
    min_percentage = 0;
    match_empty = true;
}
// -------------------------
bool jfMatchPercentTagFilterBase::ChangeSourceList(const jfGeneralTagListsGroup* in_source_list) {
    if (in_source_list == NULL) return false;
    source_list = in_source_list;
    return true;
}
const jfGeneralTagListsGroup* jfMatchPercentTagFilterBase::GetSourceList() const {
    return source_list;
}
// -------------------------
bool jfMatchPercentTagFilterBase::SetFromSource(jfMultiMatchSource* in_source) {
    if (main == NULL) main = new jfMatchFilterCore();
    else main->Clear();
    main->SetFromSource(in_source, (*source_list));
    validdata = !(main->BadExpression());
    return validdata;
}
// -------------------------
bool jfMatchPercentTagFilterBase::SetPercent(size_t in_min_percentage, bool in_match_empty) {
    if (in_min_percentage > 100) return false;
    min_percentage = in_min_percentage;
    match_empty = in_match_empty;
    return true;
}
// ++++++++++++++++++++++++++++++++++++
size_t jfMatchPercentTagFilterBase::GetPercentMatch() const {
    return min_percentage;
}
// -------------------------
bool jfMatchPercentTagFilterBase::GetMatchEmpty() const {
    return match_empty;
}
// ++++++++++++++++++++++++++++++++++++
// implemented methods
bool jfMatchPercentTagFilterBase::isEmpty() const {
    return match_empty && ((main == NULL) || main->IsEmpty());
}
// -------------------------
bool jfMatchPercentTagFilterBase::FromString(const QString& sourcedata) {
    jfLineParse* lparser = new jfLineParse(sourcedata);
    if (lparser->NNotX(4)) {
        delete lparser;
        return false;
    }
    // parsing the percentage and match empty
    QString oerr;
    size_t pct;
    if (!lparser->SBoundVal(0,100,pct,oerr)) {
        delete lparser;
        return false;
    }
    bool me;
    if (!lparser->BoolVal(1,me)) {
        delete lparser;
        return false;
    }
    // the list names and expression
    QString csv = lparser->UnEscStrE(2);
    QString exp = lparser->UnEscStrE(3);
    delete lparser;

    // creating main
    if (main == NULL) main = new jfMatchFilterCore();
    main->SetFromRawParts(csv, exp, (*source_list));
    validdata = !(main->BadExpression());
    return validdata;
}
// -------------------------
QString jfMatchPercentTagFilterBase::ToString() const {
    jfOutString packer;
    packer << min_percentage << match_empty << ((main == NULL)?"": main->GetListsCSV()) << ((main == NULL)?"": main->GetExpressionSource());
    return std::move(packer);
}
// ---------------------------------------
bool jfMatchPercentTagFilterBase::IsUsable() const {
    if (min_percentage == 0) return false;
    else {
        if (main == NULL) return false;
        else return main->IsUsable();
    }
}
// ++++++++++++++++++++++++++++++++++++
jfMatchPercentTagFilterBase::~jfMatchPercentTagFilterBase() {
    source_list = NULL;
}
// ++++++++++++++++++++++++++++++++++++
bool jfMatchPercentTagFilterBase::InternalMatch(const QStringList& incheck) const {
    if (validdata == false) return false; // no matter what
    else if (incheck.isEmpty()) {
        return match_empty;
    }
    else if (main != NULL) {
        if (main->BadExpression()) return false;
        else if (main->IsEmpty()) return true;
        else {
            // finally, a real case
            size_t match_count = main->matcher->CountMatches(incheck);
            double pct = 100*((double)match_count)/((double)incheck.size());
            return pct >= min_percentage;
        }
    }
    else return true;
}
// ++++++++++++++++++++++++++++++++++++
void jfMatchPercentTagFilterBase::CoreCopy(const jfMatchPercentTagFilterBase& mm_source, bool setup_miss_counter) {
    jfMultiMatchBaseFilter::CoreCopy(mm_source, setup_miss_counter);
    min_percentage = mm_source.min_percentage;
    match_empty = mm_source.match_empty;
}
// ==========================================================================================
// base percentage tag match filter
// the constructors
jfMatchFilteredPercentTagFilterBase::jfMatchFilteredPercentTagFilterBase(const jfGeneralTagListsGroup* in_source_list):jfMultiMatchBaseFilter() {
    assert(in_source_list != NULL);
    source_list = in_source_list;
    prefilter = NULL;
    min_percentage = 0;
    match_empty = true;
}
// -----------------------------------
bool jfMatchFilteredPercentTagFilterBase::ChangeSourceList(const jfGeneralTagListsGroup* in_source_list) {
    if (in_source_list == NULL) return false;
    source_list = in_source_list;
    return true;
}
const jfGeneralTagListsGroup* jfMatchFilteredPercentTagFilterBase::GetSourceList() const {
    return source_list;
}
// -----------------------------------
bool jfMatchFilteredPercentTagFilterBase::SetMainFromSource(jfMultiMatchSource* in_source) {
    if (main == NULL) main = new jfMatchFilterCore();
    else main->Clear();
    main->SetFromSource(in_source, (*source_list));
    bool main_valid = !main->BadExpression();
    bool pre_valid_or_empty = (prefilter == NULL) || prefilter->IsEmpty() || !(prefilter->BadExpression());
    validdata = main_valid && pre_valid_or_empty;
    return main_valid;
}
// -----------------------------
bool jfMatchFilteredPercentTagFilterBase::SetPercent(size_t in_min_percentage, bool in_match_empty) {
    if (in_min_percentage > 100) return false;
    min_percentage = in_min_percentage;
    match_empty = in_match_empty;
    return true;
}
// +++++++++++++++++++++++++++++++++++++++++
// info methods
size_t jfMatchFilteredPercentTagFilterBase::GetPercentMatch() const {
    return min_percentage;
}
// -----------------------------
bool jfMatchFilteredPercentTagFilterBase::GetMatchEmpty() const {
    return match_empty;
}
// +++++++++++++++++++++++++++++++++++++++++
// implemented methods
bool jfMatchFilteredPercentTagFilterBase::isEmpty() const {
    return (main == NULL) || main->IsEmpty();
}
//--------------------------------------------
bool jfMatchFilteredPercentTagFilterBase::FromString(const QString& sourcedata) {
    jfLineParse* lparser = new jfLineParse(sourcedata);
    if (lparser->NNotX(6)) {
        delete lparser;
        return false;
    }
    // parsing the percentage and match empty
    QString oerr;
    size_t pct;
    if (!lparser->SBoundVal(0,100,pct,oerr)) {
        delete lparser;
        return false;
    }
    bool me;
    if (!lparser->BoolVal(1,me)) {
        delete lparser;
        return false;
    }
    min_percentage = pct;
    match_empty = me;
    // the list names and expression
    QString main_csv = lparser->UnEscStrE(2);
    QString main_exp = lparser->UnEscStrE(3);
    QString pre_csv = lparser->UnEscStrE(4);
    QString pre_exp = lparser->UnEscStrE(5);
    delete lparser;
    // main convert
    if (main == NULL) main = new jfMatchFilterCore();
    else main->Clear();
    main->SetFromRawParts(main_csv, main_exp, (*source_list));

    // pre-filter convert
    if (prefilter == NULL) prefilter = new jfMatchFilterCore();
    else prefilter->Clear();
    prefilter->SetFromRawParts(pre_csv, pre_exp, (*source_list));

    // determining validity
    bool any_bad = (main->BadExpression()) || (prefilter->BadExpression());
    validdata = !any_bad;
    return validdata;
}
//--------------------------------------------
QString jfMatchFilteredPercentTagFilterBase::ToString() const {
    jfOutString packer;
    QString csv = (main == NULL) ? "" : main->GetListsCSV();
    QString exp = (main == NULL) ? "" : main->GetExpressionSource();
    packer << min_percentage << match_empty << csv << exp;
    csv = (prefilter == NULL) ? "" : prefilter->GetListsCSV();
    exp = (prefilter == NULL) ? "" : prefilter->GetExpressionSource();
    packer << csv << exp;
    return std::move(packer);
}
//--------------------------------------------
bool jfMatchFilteredPercentTagFilterBase::IsUsable() const {
    if (min_percentage == 0) return false;
    else {
        if (main == NULL) return false;
        else if (main->IsUsable()) {
            if (prefilter == NULL) return true;
            else return !prefilter->BadExpression();
        }
        else return false;
    }
}
// +++++++++++++++++++++++++++++++++++++++++
// the pre-filter
bool jfMatchFilteredPercentTagFilterBase::SetPreFilterFromSource(jfMultiMatchSource* in_source) {
    if (prefilter == NULL) prefilter = new jfMatchFilterCore();
    else prefilter->Clear();
    prefilter->SetFromSource(in_source, (*source_list));
    bool pre_valid = !prefilter->BadExpression();
    validdata = validdata && pre_valid;
    return pre_valid;
}
// -----------------------------
QString jfMatchFilteredPercentTagFilterBase::PreFilterExpression() const {
    if (prefilter == NULL) return "";
    else return prefilter->GetExpressionSource();
}
// --------------------------------
QStringList jfMatchFilteredPercentTagFilterBase::PreFilterListNames() const {
    if (prefilter == NULL) return QStringList();
    else return prefilter->GetLists();
}
// +++++++++++++++++++++++++++++++++++++++++
jfMatchFilteredPercentTagFilterBase::~jfMatchFilteredPercentTagFilterBase() {
    if (prefilter == NULL) {
        delete prefilter;
        prefilter = NULL;
    }
    source_list = NULL;
}
// +++++++++++++++++++++++++++++++++++++++++
bool jfMatchFilteredPercentTagFilterBase::InternalMatch(const QStringList& incheck) const {
    if (validdata == false) return false; // no matter what
    else if (incheck.isEmpty()) {
        return match_empty;
    }
    else if (main != NULL) {
        if (main->BadExpression()) return false;
        else if (main->IsEmpty()) return true;
        else {
            QStringList ncheck = incheck;
            // double checking the pre-filter
            if (prefilter != NULL) {
                if (prefilter->BadExpression()) return false;
                if (!prefilter->IsEmpty()) {
                    prefilter->matcher->FilterList(ncheck, true);
                    if (ncheck.isEmpty()) return match_empty;
                }
            }

            // finally, a real case
            size_t match_count = main->matcher->CountMatches(ncheck);
            double pct = 100*((double)match_count)/((double)ncheck.size());
            return pct >= min_percentage;
        }
    }
    else return true;
}
// -----------------------------
void jfMatchFilteredPercentTagFilterBase::CoreCopy(const jfMatchFilteredPercentTagFilterBase& mm_source, bool setup_miss_counter) {
    jfMultiMatchBaseFilter::CoreCopy(mm_source, setup_miss_counter);
    min_percentage = mm_source.min_percentage;
    match_empty = mm_source.match_empty;
    if (prefilter != NULL) {
        delete prefilter;
        prefilter = NULL;
    }
    if (mm_source.prefilter != NULL) {
        prefilter = mm_source.prefilter->Duplicate(true);
    }

}


//***************************************************************************
