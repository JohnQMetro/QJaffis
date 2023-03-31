//***************************************************************************
// Name:        multimatchmake.h
// Author :     John Q Metro
// Purpose :    Creating matcher objects
// Created:     August 16, 2022
// Updated:     August 28, 2022
//***************************************************************************

#include "taglist.h"
#include "tagmatch.h"
#include "multimatchmake.h"

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED

#include <QSet>
#include <QString>
#include <QList>
#include <QStringList>

//***************************************************************************

jfTagMultiMatcher* MakeMultiMatcher(jfMultiMatchSource& source, const jfGeneralTagListsGroup& lists) {
    QSet<QString>* match_set = NULL;
    if (source.listNamesToUse.count() > 0) {
        match_set = lists.MergedSetForNames(source.listNamesToUse);
    }
    jfSimpleExpr* expr_copy = NULL;
    if (source.extraExpression != NULL) {
        expr_copy = source.extraExpression->Copy();
    }
    jfTagMultiMatcher* result = new jfTagMultiMatcher(match_set, expr_copy);
    if (source.miss_counter != NULL) {
        result->AttachMissCounter(source.miss_counter);
    }
    return result;
}
// ========================================================================
/* Made to simplify filter construction */

jfMatchFilterCore::jfMatchFilterCore() {
    source = NULL;
    matcher = NULL;
}
// -----------------------------
jfMatchFilterCore::jfMatchFilterCore(jfMultiMatchSource* in_source, jfTagMultiMatcher* in_matcher) {
    source = in_source;
    matcher = in_matcher;
}
//+++++++++++++++++++++++++++++++
// info
bool jfMatchFilterCore::IsEmpty() const {
    return (matcher == NULL) || (matcher->IsEmpty());
}
// ----------------------------
bool jfMatchFilterCore::BadExpression() const {
    if (matcher == NULL) return false;
    else return matcher->BadExpression();
}
// ----------------------------
bool jfMatchFilterCore::IsUsable() const {
    if (matcher == NULL) return false;
    else return matcher->IsUsable();
}
// -----------------------------
QStringList jfMatchFilterCore::GetLists() const {
    if (source == NULL) return QStringList();
    else return source->listNamesToUse;
}
// -----------------------------
QString jfMatchFilterCore::GetListsCSV() const {
    if (source == NULL) return "";
    else return (source->listNamesToUse).join(',');
}
// -----------------------------
QString jfMatchFilterCore::GetExpressionSource() const {
    if ((source == NULL) || (source->extraExpression == NULL)) return "";
    else return (source->extraExpression)->GetSrc();
}
// -----------------------------
jfMatchFilterCore* jfMatchFilterCore::Duplicate(bool attach_miss_counter) const {
    jfMultiMatchSource* source_new = NULL;
    if (source != NULL) {
        source_new = new jfMultiMatchSource();
        (*source_new) = (*source);
        if (source->extraExpression != NULL) {
            source_new->extraExpression = (source->extraExpression)->Copy();
        }
    }
    jfTagMultiMatcher* matcher_new =  NULL;
    if (matcher != NULL) matcher_new = matcher->Duplicate(attach_miss_counter);
    return new jfMatchFilterCore(source_new, matcher_new);
}
// -----------------------------
bool jfMatchFilterCore::HasMissCounter() const {
    if (source == NULL) return false;
    else return (source->miss_counter != NULL);
}
//+++++++++++++++++++++++++++++++
// altering
// -----------------------------
void jfMatchFilterCore::Clear() {
    if (matcher != NULL) {
        delete matcher;
        matcher = NULL;
    }
    if (source != NULL) {
        if (source->miss_counter != NULL) {
            source->miss_counter = NULL;
        }
        if (source->extraExpression != NULL) {
            delete source->extraExpression;
            source->extraExpression = NULL;
        }
    }
}
// -----------------------------
bool jfMatchFilterCore::SetFromSource(jfMultiMatchSource* in_source, const jfGeneralTagListsGroup& lists) {
    if (in_source == NULL) return false;
    else {
        jfTagMultiMatcher* in_matcher = MakeMultiMatcher((*in_source), lists);
        if (in_matcher == NULL) return false;
        matcher = in_matcher;
        source = in_source;
        return true;
    }
}
// -----------------------------
bool jfMatchFilterCore::SetFromRawParts(const QString& listnames, const QString& expression, const jfGeneralTagListsGroup& lists) {
    jfSimpleExpr* new_exp = new jfSimpleExpr(expression);
    jfMultiMatchSource* new_source = new jfMultiMatchSource();
    new_source->extraExpression = new_exp;
    new_source->listNamesToUse = listnames.split(',');
    new_source->miss_counter = NULL;
    return SetFromSource(new_source, lists);
}
//+++++++++++++++++++++++++++++++
// destructor
jfMatchFilterCore::~jfMatchFilterCore() {
    Clear();
}
// ========================================================================

/* Base filter that wraps a tag multimatcher */
// the constructors
jfMultiMatchBaseFilter::jfMultiMatchBaseFilter():jfBaseFilter() {
    main = NULL;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++
// external information
QString jfMultiMatchBaseFilter::MainExpression() const {
    return (main == NULL) ? "" : main->GetExpressionSource();
}
// ----------------------------------
QStringList jfMultiMatchBaseFilter::MainListNames() const {
    if (main == NULL) return QStringList();
    else return main->GetLists();
}
// -------------------------
bool jfMultiMatchBaseFilter::HasMissCounter() const {
    if (main == NULL) return false;
    else return main->HasMissCounter();
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++
// returns a general filter type
size_t jfMultiMatchBaseFilter::GetFilType() const {
    return 1;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfMultiMatchBaseFilter::~jfMultiMatchBaseFilter() {
    delete main;
    main = NULL;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// external information
bool jfMultiMatchBaseFilter::SetFromSourceInternal(jfMultiMatchSource* in_source, const jfGeneralTagListsGroup& lists) {
    if (main == NULL) main = new jfMatchFilterCore();
    else main->Clear();
    return main->SetFromSource(in_source, lists);
}
// -------------------------
// private methods
size_t jfMultiMatchBaseFilter::ExtraLines() const {
    return 1;
}
// ----------------------------------------------
bool jfMultiMatchBaseFilter::AddRestToFile(QTextStream* outfile) const {
    QString buffer;
    // checking and special conditions
    if (outfile==NULL) return false;
    // composing line 4
    QString src = ToString();
    buffer = EscapeNL(src,IO_DELIMS);
    (*outfile) << buffer << "\n";
    return true;
}
// ----------------------------------------------
bool jfMultiMatchBaseFilter::ReadRestFromFile(jfFileReader* infile) {
    const QString funcname = "jfMultiMatchBaseFilter::ReadRestFromFile";
    // input data
    QString cline;
    bool resx;
    // starting checks (and reading the line)
    assert(infile!=NULL);
    if (!infile->ReadUnEsc(cline,funcname)) return false;
    // there is only one line, and one field, so this is pretty simple
    resx = FromString(cline);
    if (!resx) return infile->BuildError("The tag match string is invalid!");
    else return true;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfMultiMatchSource* jfMultiMatchBaseFilter::SourceCopyHelper(const jfMultiMatchSource& original_source, const jfTagMultiMatcher* in_matcher) const {
    jfMultiMatchSource* new_source = new jfMultiMatchSource();
    (*new_source) = original_source;
    // the extra expression is supposed to be the same as the matcher
    if (new_source->extraExpression != NULL) {
        if (in_matcher->second_match_expression != NULL) {
            new_source->extraExpression = in_matcher->second_match_expression;
        }
        else {
            new_source->extraExpression = original_source.extraExpression->Copy();
        }
    }
    return new_source;
}
// ----------------------------------------------
// copies stuff 'into' this,
void jfMultiMatchBaseFilter::CoreCopy(const jfMultiMatchBaseFilter& mm_source, bool attach_miss_counter) {
    // copying the most basic items
    name = mm_source.name;
    validdata = mm_source.validdata;

    // copying the contents
    if (main != NULL) delete main;
    if ((mm_source.main) == NULL) main = NULL;
    else main = (mm_source.main)->Duplicate(attach_miss_counter);
}

//***************************************************************************
