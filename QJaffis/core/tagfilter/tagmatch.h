//***************************************************************************
// Name:        tagmatch.h
// Author :     John Q Metro
// Purpose :    Global lists and matcher classes for tags
// Created:     August 13, 2022
// Updated:     August 28, 2022
//***************************************************************************
#pragma once
#include "taglist.h"
//-------------------------------------------------------------------------
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED

#include "stringcounter.h"

#include <QString>

//***************************************************************************
namespace taglistings {
    extern const QString tagfilename;
    // lists of tags for one-hit excludes or similar stuff
    extern jfGeneralTagListsGroup* excludeListsGroup;
    // (bigger) lists of tags for percent filtering assigning
    extern jfGeneralTagListsGroup* typeListsGroup;

    bool InitializeListsFromFile();
    bool SaveAndClearLists();
}
//***************************************************************************
// a multi-part class used to match tags
class jfTagMultiMatcher {
  friend class jfMultiMatchBaseFilter;
  public:
    jfTagMultiMatcher(QSet<QString>* in_first_match_set, jfSimpleExpr* in_second_match_expression);
    bool AttachMissCounter(jfStringCounter* in_miss_counter);
    // info flags
    bool IsEmpty() const;
    bool BadExpression() const;
    bool IsUsable() const;
    bool HasSet() const;
    bool HasExpression() const;
    // matching methods
    bool Matches(const QString& tag) const;
    bool MatchesAny(const QStringList& tags) const;
    size_t CountMatches(const QStringList& tags) const;
    bool FilterList(QStringList& tags, bool filter_matches) const;

    // special
    jfTagMultiMatcher* Duplicate(bool attach_miss_counter) const;
    virtual ~jfTagMultiMatcher();
  protected:
    bool recordMiss(const QString& tag) const;

    QSet<QString>* first_match_set;
    jfSimpleExpr* second_match_expression;

    mutable jfStringCounter* miss_counter;
};


//***************************************************************************
