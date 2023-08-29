//***************************************************************************
// Name:        multimatchmake.h
// Author :     John Q Metro
// Purpose :    Creating matcher objects
// Created:     August 15, 2022
// Updated:     April 8, 2023 (Rebasing, creating jfMatchTagFilterBase)
//***************************************************************************
#pragma once
#include "taglist.h"
#include "tagmatch.h"
// ----------------------------------------------
#include <QStringList>

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED

#include "../filters/base/basefilter.h"

//***************************************************************************
// the sort of raw output that might be produced by a multi-match editor
struct jfMultiMatchSource {
    QStringList listNamesToUse;
    jfSimpleExpr* extraExpression;
    jfStringCounter* miss_counter;
};
// ===============================================================================

jfTagMultiMatcher* MakeMultiMatcher(jfMultiMatchSource& source, const jfGeneralTagListsGroup& lists);

// ===============================================================================
/* Made to simplify filter construction */
class jfMatchFilterCore {
  public:
    jfMultiMatchSource* source;
    jfTagMultiMatcher* matcher;

    jfMatchFilterCore();
    jfMatchFilterCore(jfMultiMatchSource* in_source, jfTagMultiMatcher* in_matcher);
    // info
    bool IsEmpty() const;
    bool BadExpression() const;
    bool IsUsable() const;
    QStringList GetLists() const;
    QString GetListsCSV() const;
    QString GetExpressionSource() const;
    jfMatchFilterCore* Duplicate(bool attach_miss_counter) const;
    bool HasMissCounter() const;
    // altering
    void Clear();
    bool SetFromSource(jfMultiMatchSource* in_source, const jfGeneralTagListsGroup& lists);
    bool SetFromRawParts(const QString& listnames, const QString& expression, const jfGeneralTagListsGroup& lists);
    // destructor
    virtual ~jfMatchFilterCore();

};
// =====================================================
/* Base filter that wraps a tag multimatcher */
class jfMultiMatchBaseFilter : public jfFilterBase {
  public:
    // the constructors
    jfMultiMatchBaseFilter(const QString& filter_name);
    jfMultiMatchBaseFilter(QString&& filter_name);

    // external information
    QString MainExpression() const;
    QStringList MainListNames() const;
    bool HasMissCounter() const;
    virtual bool IsUsable() const = 0;
    // the destructor
    virtual ~jfMultiMatchBaseFilter();
  protected:
    bool SetFromSourceInternal(jfMultiMatchSource* in_source, const jfGeneralTagListsGroup& lists);
    jfMultiMatchSource* SourceCopyHelper(const jfMultiMatchSource& original_source, const jfTagMultiMatcher* in_matcher) const;
    void CoreCopy(const jfMultiMatchBaseFilter& mm_source, bool attach_miss_counter);

    // private data
    jfMatchFilterCore* main;
};
// ========================================================================
// base filter for multimatching that uses a jfGeneralTagListsGroup;
class jfMatchTagFilterBase : public jfMultiMatchBaseFilter {
  public:
    // the constructors
    jfMatchTagFilterBase(const QString& filter_name);
    jfMatchTagFilterBase(const QString& filter_name, const jfGeneralTagListsGroup* in_source_list);
    jfMatchTagFilterBase(QString&& filter_name, const jfGeneralTagListsGroup* in_source_list);

    bool ChangeSourceList(const jfGeneralTagListsGroup* in_source_list);
    const jfGeneralTagListsGroup* GetSourceList() const;

    bool SetFromSource(jfMultiMatchSource* in_source);

    virtual ~jfMatchTagFilterBase();

  protected:
    jfMatchFilterCore* FromStringsHelper(const QString& listname, const QString& expression, QString& out_error);
    virtual bool InternalMatch(const QStringList& incheck) const = 0;

    const jfGeneralTagListsGroup* source_list;

};

//***************************************************************************
