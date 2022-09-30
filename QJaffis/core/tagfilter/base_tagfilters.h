//***************************************************************************
// Name:        base_tagfilters.h
// Author :     John Q Metro
// Purpose :    The base tag filters
// Created:     August 20, 2022
// Updated:     August 28, 2022
//***************************************************************************
#pragma once

#include "multimatchmake.h"
// ----------------------------------------------
#include <QStringList>
//***************************************************************************

// base filter where a single match in a list counts
class jfMatchOneTagFilterBase : public jfMultiMatchBaseFilter {
  public:
    // the constructors
    jfMatchOneTagFilterBase(const jfGeneralTagListsGroup* in_source_list);
    bool ChangeSourceList(const jfGeneralTagListsGroup* in_source_list);
    bool SetFromSource(jfMultiMatchSource* in_source);

    // implemented methods
    virtual bool isEmpty() const;
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    virtual bool IsUsable() const;

    // to implement
    virtual QString GetTypeID() const = 0;
    virtual QString GetTypeDescription() const = 0;
    virtual jfBaseFilter* GenCopy() const = 0;

    virtual ~jfMatchOneTagFilterBase();

  protected:
    bool InternalMatch(const QStringList& incheck) const;

    virtual bool CoreMatch(const jfBasePD* testelem) const = 0;

    const jfGeneralTagListsGroup* source_list;

};
//============================================================
// base percentage tag match filter
class jfMatchPercentTagFilterBase : public jfMultiMatchBaseFilter {
  public:
    // the constructors
    jfMatchPercentTagFilterBase(const jfGeneralTagListsGroup* in_source_list);
    bool ChangeSourceList(const jfGeneralTagListsGroup* in_source_list);
    const jfGeneralTagListsGroup* GetSourceList() const;
    bool SetFromSource(jfMultiMatchSource* in_source);
    bool SetPercent(size_t in_min_percentage, bool in_match_empty);

    // info methods
    size_t GetPercentMatch() const;
    bool GetMatchEmpty() const;

    // implemented methods
    virtual bool isEmpty() const;
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    virtual bool IsUsable() const;

    // to implement
    virtual QString GetTypeID() const = 0;
    virtual QString GetTypeDescription() const = 0;
    virtual jfBaseFilter* GenCopy() const = 0;

    virtual ~jfMatchPercentTagFilterBase();

  protected:
    bool InternalMatch(const QStringList& incheck) const;
    virtual void CoreCopy(const jfMatchPercentTagFilterBase& mm_source, bool setup_miss_counter);

    virtual bool CoreMatch(const jfBasePD* testelem) const = 0;

    size_t min_percentage;
    bool match_empty;
    const jfGeneralTagListsGroup* source_list;

};
//============================================================
// base percentage tag match filter
class jfMatchFilteredPercentTagFilterBase : public jfMultiMatchBaseFilter {
  public:
    // the constructors
    jfMatchFilteredPercentTagFilterBase(const jfGeneralTagListsGroup* in_source_list);
    bool ChangeSourceList(const jfGeneralTagListsGroup* in_source_list);
    const jfGeneralTagListsGroup* GetSourceList() const;
    bool SetMainFromSource(jfMultiMatchSource* in_source);
    bool SetPercent(size_t in_min_percentage, bool in_match_empty);

    // info methods
    size_t GetPercentMatch() const;
    bool GetMatchEmpty() const;

    // implemented methods
    virtual bool isEmpty() const;
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    virtual bool IsUsable() const;

    // the pre-filter
    bool SetPreFilterFromSource(jfMultiMatchSource* in_source);
    QString PreFilterExpression() const;
    QStringList PreFilterListNames() const;

    // to implement
    virtual QString GetTypeID() const = 0;
    virtual QString GetTypeDescription() const = 0;
    virtual jfBaseFilter* GenCopy() const = 0;

    virtual ~jfMatchFilteredPercentTagFilterBase();

  protected:
    bool InternalMatch(const QStringList& incheck) const;
    virtual void CoreCopy(const jfMatchFilteredPercentTagFilterBase& mm_source, bool setup_miss_counter);

    virtual bool CoreMatch(const jfBasePD* testelem) const = 0;

    size_t min_percentage;
    bool match_empty;

    // private data
    jfMatchFilterCore* prefilter;

    const jfGeneralTagListsGroup* source_list;

};

//***************************************************************************
