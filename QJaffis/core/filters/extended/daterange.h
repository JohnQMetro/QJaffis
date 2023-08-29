/******************************************************************************
Name    :   daterange.h
Author  :   John Q Metro
Purpose :   Filters relating to dates
Created :   November 29, 2011
Conversion to QT Started October 1, 2013
Updated :   July 1, 2023 (rebasing)
******************************************************************************/
#ifndef DATERANGE_FILTER_H
  #define DATERANGE_FILTER_H
#endif // DATERANGE_FILTER_H
//----------------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef JFILTERGLOBALS
  #include "../filter_globals.h"
#endif
#ifndef MINMAX_FILTER_H
  #include "minmax.h"
#endif // MINMAX_FILTER_H
//----------------------------------------------------
#include <QDate>
/*****************************************************************************/
class jfDateRangeFilter : public jfFilterBase {
  public:
    // constructors
    jfDateRangeFilter(const QString& filter_name);
    jfDateRangeFilter(QString&& filter_name);
    jfDateRangeFilter(const QString& filter_name, QDate instart,QDate inend);
    // match against the filter
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const;
    // setting and getting data
    bool SetDates(QDate instart,QDate inend);
    QDate GetValue(bool first) const;
    // not overriden yet...
    // virtual const jfFilterTypeMeta& GetTypeMetaInfo() const = 0;
    // virtual jfFilterBase* GenCopy() const = 0;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // special methdos
    QDate DayPlus(QDate inval, bool decrement) const;
    // the core matching method
    // virtual bool CoreMatch(const jfSearchResultItem* testelem) const = 0;
    bool RealMatchCore(const QDate intest) const;

    // internal data
    QDate newdate, olddate;
};
//============================================================================
extern const jfFilterTypeMeta UPDATED_DATE_FILTER_INFO;

class jfUpdatedDateFilter : public jfDateRangeFilter {
  public:
    // constructors
    jfUpdatedDateFilter(const QString& filter_name);
    jfUpdatedDateFilter(QString&& filter_name);
    jfUpdatedDateFilter(const QString& filter_name, QDate instart,QDate inend);
    // gets type info
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // copy
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
//============================================================================
extern const jfFilterTypeMeta PUBLISHED_DATE_FILTER_INFO;

class jfPublishedDateFilter : public jfDateRangeFilter {
  public:
    // constructors
    jfPublishedDateFilter(const QString& filter_name);
    jfPublishedDateFilter(QString&& filter_name);
    jfPublishedDateFilter(const QString& filter_name, QDate instart,QDate inend);
    // gets type info
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // copy
    virtual jfFilterBase* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};

/*****************************************************************************/
