/******************************************************************************
Name    :   daterange.h
Author  :   John Q Metro
Purpose :   Filters relating to dates
Created :   November 29, 2011
Conversion to QT Started October 1, 2013
Updated :   July 8, 2016 (removed FFN and renamed file)
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
class jfDateRangeFilter : public jfBaseFilter {
  public:
    // constructors
    jfDateRangeFilter();
    jfDateRangeFilter(QDate instart,QDate inend);
    // match against the filter
    virtual bool isEmpty() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // setting and getting data
    bool SetDates(QDate instart,QDate inend);
    QDate GetValue(bool first) const;
    // gets a description
    virtual QString GetTypeDescription() const = 0;
    // copy
    virtual jfBaseFilter* GenCopy() const = 0;
    // special meta-information
    virtual QString GetTypeID() const = 0;
  protected:
    // special methdos
    QDate DayPlus(QDate inval, bool decrement) const;
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const = 0;
    bool RealMatchCore(const QDate intest) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal data
    QDate newdate, olddate;
};
//============================================================================
class jfUpdatedDateFilter : public jfDateRangeFilter {
  public:
    // constructors
    jfUpdatedDateFilter();
    jfUpdatedDateFilter(QDate instart,QDate inend);
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};
//============================================================================
class jfPublishedDateFilter : public jfDateRangeFilter {
  public:
    // constructors
    jfPublishedDateFilter();
    jfPublishedDateFilter(QDate instart,QDate inend);
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};

/*****************************************************************************/
