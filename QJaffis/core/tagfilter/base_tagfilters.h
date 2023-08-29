//***************************************************************************
// Name:        base_tagfilters.h
// Author :     John Q Metro
// Purpose :    The base tag filters
// Created:     August 20, 2022
// Updated:     April 8, 2023 (Rebasing)
//***************************************************************************
#pragma once

#include "multimatchmake.h"
// ----------------------------------------------
#include <QStringList>
//***************************************************************************

// base filter where a single match in a list counts
class jfMatchOneTagFilterBase : public jfMatchTagFilterBase {
  public:
    // the constructors
    jfMatchOneTagFilterBase(const QString& filter_name);
    jfMatchOneTagFilterBase(const QString& filter_name, const jfGeneralTagListsGroup* in_source_list);
    jfMatchOneTagFilterBase(QString&& filter_name, const jfGeneralTagListsGroup* in_source_list);

    // implemented methods
    virtual bool IsEmpty() const override;
    virtual QString ToString() const override;
    virtual bool IsUsable() const override;

    virtual ~jfMatchOneTagFilterBase();

  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    virtual bool InternalMatch(const QStringList& incheck) const override;

    const jfGeneralTagListsGroup* source_list;

};
//============================================================
// base percentage tag match filter
class jfMatchPercentTagFilterBase : public jfMatchTagFilterBase {
  public:
    // the constructors
    jfMatchPercentTagFilterBase(const QString& filter_name);
    jfMatchPercentTagFilterBase(const QString& filter_name, const jfGeneralTagListsGroup* in_source_list);
    jfMatchPercentTagFilterBase(QString&& filter_name, const jfGeneralTagListsGroup* in_source_list);

    bool SetPercent(size_t in_min_percentage, bool in_match_empty);

    // info methods
    size_t GetPercentMatch() const;
    bool GetMatchEmpty() const;

    // implemented methods
    virtual bool IsEmpty() const override;
    virtual QString ToString() const override;
    virtual bool IsUsable() const override;

    virtual ~jfMatchPercentTagFilterBase();

  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    virtual bool InternalMatch(const QStringList& incheck) const override;
    virtual void CoreCopy(const jfMatchPercentTagFilterBase& mm_source, bool setup_miss_counter);

    size_t min_percentage;
    bool match_empty;
    const jfGeneralTagListsGroup* source_list;

};
//============================================================
// base percentage tag match filter, but with an extra 'pre filter'
class jfMatchFilteredPercentTagFilterBase : public jfMatchTagFilterBase {
  public:
    // the constructors
    jfMatchFilteredPercentTagFilterBase(const QString& filter_name);
    jfMatchFilteredPercentTagFilterBase(const QString& filter_name, const jfGeneralTagListsGroup* in_source_list);
    jfMatchFilteredPercentTagFilterBase(QString&& filter_name, const jfGeneralTagListsGroup* in_source_list);

    // bool SetFromSource(jfMultiMatchSource* in_source);
    bool SetPercent(size_t in_min_percentage, bool in_match_empty);

    // info methods
    size_t GetPercentMatch() const;
    bool GetMatchEmpty() const;

    // implemented methods
    virtual bool IsEmpty() const override;
    virtual QString ToString() const override;
    virtual bool IsUsable() const override;

    // the pre-filter
    bool SetPreFilterFromSource(jfMultiMatchSource* in_source);
    void ClearPreFilter();
    QString PreFilterExpression() const;
    QStringList PreFilterListNames() const;

    virtual ~jfMatchFilteredPercentTagFilterBase();

  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    virtual bool InternalMatch(const QStringList& incheck) const override;
    virtual void CoreCopy(const jfMatchFilteredPercentTagFilterBase& mm_source, bool setup_miss_counter);

    size_t min_percentage;
    bool match_empty;

    // private data
    jfMatchFilterCore* prefilter;

    const jfGeneralTagListsGroup* source_list;

};

//***************************************************************************
