//***************************************************************************
// Name:        multimatchmake.h
// Author :     John Q Metro
// Purpose :    Creating matcher objects
// Created:     August 15, 2022
// Updated:     August 28, 2022
//***************************************************************************
#pragma once
#include "taglist.h"
#include "tagmatch.h"
// ----------------------------------------------
#include <QStringList>

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED

#ifndef JFBASEFILTER
  #include "filterbase.h"
#endif
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
class jfMultiMatchBaseFilter : public jfBaseFilter {
  public:
    // the constructors
    jfMultiMatchBaseFilter();
    // virtual methods
    virtual bool isEmpty() const = 0;
    virtual bool FromString(const QString& sourcedata) = 0;
    virtual QString ToString() const = 0;

    // external information
    QString MainExpression() const;
    QStringList MainListNames() const;
    bool HasMissCounter() const;
    virtual bool IsUsable() const = 0;

    // returns a general filter type
    virtual size_t GetFilType() const;
    // the destructor
    virtual ~jfMultiMatchBaseFilter();
  protected:
    bool SetFromSourceInternal(jfMultiMatchSource* in_source, const jfGeneralTagListsGroup& lists);
    // private methods
    virtual size_t ExtraLines() const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    jfMultiMatchSource* SourceCopyHelper(const jfMultiMatchSource& original_source, const jfTagMultiMatcher* in_matcher) const;
    void CoreCopy(const jfMultiMatchBaseFilter& mm_source, bool attach_miss_counter);

    // private data
    jfMatchFilterCore* main;
};

//***************************************************************************
