//***************************************************************************
// Name:        base_tagfilters.cpp
// Author :     John Q Metro
// Purpose :    The base tag filters
// Created:     August 20, 2022
// Updated:     April 8, 2023 (Rebasing)
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

jfMatchOneTagFilterBase::jfMatchOneTagFilterBase(const QString& filter_name):jfMatchTagFilterBase(filter_name) {
}
// ------------------------------------------------
jfMatchOneTagFilterBase::jfMatchOneTagFilterBase(const QString& filter_name,
            const jfGeneralTagListsGroup* in_source_list):jfMatchTagFilterBase(filter_name, in_source_list) {

}
// ------------------------------------------------
jfMatchOneTagFilterBase::jfMatchOneTagFilterBase(QString&& filter_name,
            const jfGeneralTagListsGroup* in_source_list):jfMatchTagFilterBase(filter_name, in_source_list) {

}
// ++++++++++++++++++++++++++++++++++
// implemented methods
bool jfMatchOneTagFilterBase::IsEmpty() const {
    return (main == NULL) || (main->IsEmpty());
}
// --------------------------------------
QString jfMatchOneTagFilterBase::ToString() const {
    jfOutString sform;
    sform << ((main == NULL)?"": main->GetListsCSV()) << ((main == NULL)?"": main->GetExpressionSource());
    return std::move(sform);
}
// ---------------------------------------
bool jfMatchOneTagFilterBase::IsUsable() const {
    if ((main == NULL) || (source_list == NULL)) return false;
    else return main->IsUsable();
}
// ++++++++++++++++++++++++++++++++++
jfMatchOneTagFilterBase::~jfMatchOneTagFilterBase() {
}
// +++++++++++++++++++++++++++++++++++
bool jfMatchOneTagFilterBase::FromStringInner(const QString& sourcedata, QString& error_out) {
    assert(source_list != NULL);
    // splitting
    jfLineParse* lparser = new jfLineParse(sourcedata);
    if (lparser->NNotX(2)) {
        delete lparser;
        error_out = "Source does not have 2 fields!";
        return false;
    }
    QString csv = lparser->UnEscStrE(0);
    QString exp = lparser->UnEscStrE(1);
    delete lparser;

    // creating main
    jfMatchFilterCore* new_main = FromStringsHelper(csv, exp, error_out);
    if (new_main == NULL) return false;

    // replacing
    if (main != NULL) delete main;
    main = new_main;
    return true;
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
// ---------------------------------
jfMatchPercentTagFilterBase::jfMatchPercentTagFilterBase(const QString& filter_name):jfMatchTagFilterBase(filter_name) {
    min_percentage = 0;
    match_empty = true;
}
// ---------------------------------
jfMatchPercentTagFilterBase::jfMatchPercentTagFilterBase(const QString& filter_name,
        const jfGeneralTagListsGroup* in_source_list):jfMatchTagFilterBase(filter_name, in_source_list) {
    min_percentage = 0;
    match_empty = true;
}
// ---------------------------------
jfMatchPercentTagFilterBase::jfMatchPercentTagFilterBase(QString&& filter_name,
        const jfGeneralTagListsGroup* in_source_list):jfMatchTagFilterBase(filter_name, in_source_list) {
    min_percentage = 0;
    match_empty = true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
bool jfMatchPercentTagFilterBase::IsEmpty() const {
    return match_empty && ((main == NULL) || main->IsEmpty());
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

}
// ++++++++++++++++++++++++++++++++++++
bool jfMatchPercentTagFilterBase::FromStringInner(const QString& sourcedata, QString& error_out) {
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
        error_out = "% field was not a valid 0 to 100 integer!";
        return false;
    }
    bool me;
    if (!lparser->BoolVal(1,me)) {
        delete lparser;
        error_out = "The match empty field was neither true nor false!";
        return false;
    }
    // the list names and expression
    QString csv = lparser->UnEscStrE(2);
    QString exp = lparser->UnEscStrE(3);
    delete lparser;

    // creating main
    jfMatchFilterCore* new_main = FromStringsHelper(csv, exp, error_out);
    if (new_main == NULL) return false;

    // replacing the contents
    if (main != NULL) delete main;
    main = new_main;
    min_percentage = pct;
    match_empty = me;
    return true;
}
// ---------------------------------------------
bool jfMatchPercentTagFilterBase::InternalMatch(const QStringList& incheck) const {
    if (incheck.isEmpty()) {
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
jfMatchFilteredPercentTagFilterBase::jfMatchFilteredPercentTagFilterBase(const QString& filter_name):jfMatchTagFilterBase(filter_name) {
    prefilter = NULL;
    min_percentage = 0;
    match_empty = true;
}
//-----------------------------------------------------
jfMatchFilteredPercentTagFilterBase::jfMatchFilteredPercentTagFilterBase(const QString& filter_name,
        const jfGeneralTagListsGroup* in_source_list):jfMatchTagFilterBase(filter_name, in_source_list) {
    prefilter = NULL;
    min_percentage = 0;
    match_empty = true;
}
//-----------------------------------------------------
jfMatchFilteredPercentTagFilterBase::jfMatchFilteredPercentTagFilterBase(QString&& filter_name,
        const jfGeneralTagListsGroup* in_source_list):jfMatchTagFilterBase(filter_name, in_source_list) {
    prefilter = NULL;
    min_percentage = 0;
    match_empty = true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
bool jfMatchFilteredPercentTagFilterBase::IsEmpty() const {
    return (main == NULL) || main->IsEmpty();
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
    if (in_source == NULL) {
        ClearPreFilter();
        return true;
    }


    jfMatchFilterCore* new_prefilter = new jfMatchFilterCore();
    new_prefilter->SetFromSource(in_source, (*source_list));
    if (new_prefilter->BadExpression()) {
        delete new_prefilter;
        return false;
    }
    if (prefilter != NULL) delete prefilter;
    prefilter = new_prefilter;
    return true;
}
// -----------------------------
void jfMatchFilteredPercentTagFilterBase::ClearPreFilter() {
    if (prefilter != NULL) {
        delete prefilter;
        prefilter = NULL;
    }
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
    ClearPreFilter();
}
// +++++++++++++++++++++++++++++++++++++++++
bool jfMatchFilteredPercentTagFilterBase::FromStringInner(const QString& sourcedata, QString& error_out) {

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

    // the list names and expression
    QString main_csv = lparser->UnEscStrE(2);
    QString main_exp = lparser->UnEscStrE(3);
    QString pre_csv = lparser->UnEscStrE(4);
    QString pre_exp = lparser->UnEscStrE(5);
    delete lparser;

    // main convert
    jfMatchFilterCore* main_core = FromStringsHelper(main_csv, main_exp, error_out);
    if (main_core == NULL) return false;

    // pre-filter convert
    jfMatchFilterCore* prefilter_core = FromStringsHelper(pre_csv, pre_exp, error_out);
    if (prefilter_core == NULL) {
        delete main_core;
        return false;
    }

    // finishing off
    min_percentage = pct;
    match_empty = me;
    if (main != NULL) delete main;
    main = main_core;
    if (prefilter != NULL) delete prefilter;
    prefilter = (prefilter_core->IsEmpty()) ? NULL : prefilter_core;

    return true;
}
// +++++++++++++++++++++++++++++++++++++++++
bool jfMatchFilteredPercentTagFilterBase::InternalMatch(const QStringList& incheck) const {
    if (incheck.isEmpty()) {
        return match_empty;
    }
    else if (main != NULL) {
        if (main->IsEmpty()) return true;
        else {
            QStringList ncheck = incheck;
            // double checking the pre-filter
            if (prefilter != NULL) {
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
