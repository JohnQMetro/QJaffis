//***************************************************************************
// Name:        tagmatch.cpp
// Author :     John Q Metro
// Purpose :    Global lists and matcher classes for tags
// Created:     August 14, 2022
// Updated:     August 28, 2022
//***************************************************************************

#include "tagmatch.h"
//-------------------------------------------------------------------------
#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED

#include <QString>
#include <QTextStream>
//***************************************************************************
const QString taglistings::tagfilename = "config/taglist.dat";
// lists of tags for one-hit excludes or similar stuff
jfGeneralTagListsGroup* taglistings::excludeListsGroup = NULL;
// (bigger) lists of tags for percent filtering assigning
jfGeneralTagListsGroup* taglistings::typeListsGroup = NULL;
// +++++++++++++++++++++++++++++++++++++++
bool taglistings::InitializeListsFromFile() {
    jfFileReader* source_in = new jfFileReader(tagfilename);
    bool open_okay = source_in->OpenFile();
    if (!open_okay) {
        delete source_in;
        return false;
    }
    // here, we start reading the lists
    excludeListsGroup = new jfGeneralTagListsGroup("Exclude Lists");
    int amount_loaded = excludeListsGroup->LoadFromFile(source_in);
    if (amount_loaded < 0) {
        delete excludeListsGroup;
        excludeListsGroup = NULL;
        source_in->CloseFile();
        delete source_in;
        return false;
    }
    typeListsGroup = new jfGeneralTagListsGroup("Type Lists");
    amount_loaded = typeListsGroup->LoadFromFile(source_in);
    if (amount_loaded < 0) {
        delete typeListsGroup;
        typeListsGroup = NULL;
    }
    source_in->CloseFile();
    delete source_in;
    return (amount_loaded >= 0);
}
// ----------------------
bool taglistings::SaveAndClearLists() {
    if ((excludeListsGroup == NULL) && (typeListsGroup == NULL)) return false;
    else {
        QTextStream* target_file = GetOpenFile(tagfilename, true);
        if (target_file == NULL) return false;
        bool wrote_okay;
        // the exclude lists
        if (excludeListsGroup != NULL) {
            wrote_okay = excludeListsGroup->AddToFileStream(target_file);
            delete excludeListsGroup;
            excludeListsGroup = NULL;
        }
        else {
            (*target_file) << "0\n";
            wrote_okay = (target_file->status() == QTextStream::Ok);
        }
        if (!wrote_okay) {
            CloseFileStream(target_file, true);
            return false;
        }
        // the type lists
        if (typeListsGroup != NULL) {
            wrote_okay = typeListsGroup->AddToFileStream(target_file);
            delete typeListsGroup;
            typeListsGroup = NULL;
        }
        else {
            (*target_file) << "0\n";
            wrote_okay = (target_file->status() == QTextStream::Ok);
        }
        CloseFileStream(target_file, true);
        return wrote_okay;
    }
}

//***************************************************************************
// a multi-part class used to match tags

jfTagMultiMatcher::jfTagMultiMatcher(QSet<QString>* in_first_match_set, jfSimpleExpr* in_second_match_expression) {
    first_match_set = in_first_match_set;
    second_match_expression = in_second_match_expression;

    miss_counter = NULL;
}
// ----------------------------------
bool jfTagMultiMatcher::AttachMissCounter(jfStringCounter* in_miss_counter) {
    miss_counter = in_miss_counter;
    return (miss_counter != NULL);
}
// ++++++++++++++++++++++++++++++++++
// info flags
bool jfTagMultiMatcher::IsEmpty() const {
    if (first_match_set != NULL) {
        if (!first_match_set->isEmpty()) return false;
    }
    if (second_match_expression != NULL) {
        return second_match_expression->isEmpty();
    }
    else return true;
}
// ---------------------------
bool jfTagMultiMatcher::BadExpression() const {
    if (second_match_expression == NULL) return false;
    else if (second_match_expression->IsValid()) return false;
    else return true;
}
// ----------------------------
bool jfTagMultiMatcher::IsUsable() const {
    return HasSet() || HasExpression();
}
// ----------------------------
bool jfTagMultiMatcher::HasSet() const {
    return (first_match_set != NULL) && (!(first_match_set->isEmpty()));
}
// ----------------------------
bool jfTagMultiMatcher::HasExpression() const {
    if (second_match_expression != NULL) {
        return (second_match_expression->IsValid()) && (!(second_match_expression->isEmpty()));
    }
    else return false;
}
// ++++++++++++++++++++++++++++++++++
// matching methods
bool jfTagMultiMatcher::Matches(const QString& tag) const {
    if (tag.isEmpty()) return false;
    if (HasSet()) {
        if (first_match_set->contains(tag)) return true;
    }
    if (HasExpression()) {
        if (second_match_expression->InternalMatch(tag)) return true;
    }
    recordMiss(tag);
    return false;
}
// ----------------------------
bool jfTagMultiMatcher::MatchesAny(const QStringList& tags) const {
    if (tags.isEmpty() || BadExpression()) return false;
    if (HasSet()) {
        for (const QString& tag : tags) {
            if (first_match_set->contains(tag)) return true;
        }
    }
    if (HasExpression()) {
        for (const QString& tag : tags) {
            if (second_match_expression->InternalMatch(tag)) return true;
        }
    }
    return false;
}
// ----------------------------
size_t jfTagMultiMatcher::CountMatches(const QStringList& tags) const {
    if (tags.isEmpty() || BadExpression()) return 0;
    size_t run_count = 0;
    for (const QString& tag : tags) {
        bool match = false;
        if (HasSet()) {
            match = first_match_set->contains(tag);
        }
        if (!match & HasExpression()) {
            match = second_match_expression->InternalMatch(tag);
        }
        if (match) run_count++;
        else recordMiss(tag);
    }
    return run_count;
}
// ----------------------------
bool jfTagMultiMatcher::FilterList(QStringList& tags, bool filter_matches) const {
    if (tags.isEmpty() || BadExpression()) return false;
    for (int i = tags.size() -1 ; i >= 0; i--) {
        bool match = Matches(tags[i]);
        if (match && filter_matches) tags.removeAt(i);
        else if (!match && (!filter_matches)) tags.removeAt(i);
    }
    return true;
}
// +++++++++++++++++++++++++++++++++

jfTagMultiMatcher* jfTagMultiMatcher::Duplicate(bool attach_miss_counter) const {
    QSet<QString>* new_first_match_set = NULL;
    jfSimpleExpr* new_second_match_expression = NULL;

    if (first_match_set != NULL) {
        new_first_match_set = new QSet<QString>();
        (*new_first_match_set) = (*first_match_set);
        new_first_match_set->detach();
    }
    if (second_match_expression != NULL) {
        new_second_match_expression = second_match_expression->Copy();
    }
    jfTagMultiMatcher* result = new jfTagMultiMatcher(new_first_match_set, new_second_match_expression);

    // the miss counter, if there, is not duplicated
    if (attach_miss_counter && (miss_counter != NULL)) {
       result->miss_counter = miss_counter;
    }

    return result;
}


// ++++++++++++++++++++++++++++++++++
jfTagMultiMatcher::~jfTagMultiMatcher() {
    if (first_match_set != NULL) delete first_match_set;
    if (second_match_expression != NULL) delete second_match_expression;
    miss_counter = NULL;
}
// ++++++++++++++++++++++++++++++++++++++
bool jfTagMultiMatcher::recordMiss(const QString& tag) const {
    if (miss_counter != NULL) {
        miss_counter->RecordItem(tag);
        return true;
    }
    else return false;
}


//***************************************************************************
