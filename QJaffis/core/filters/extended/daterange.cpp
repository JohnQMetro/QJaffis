/******************************************************************************
Name    :   daterange.cpp
Author  :   John Q Metro
Purpose :   Filters relating to dates
Created :   November 29, 2011
Conversion to QT Started October 1, 2013
Updated :   July 1, 2023 (rebasing)
******************************************************************************/
#ifndef DATERANGE_FILTER_H
  #include "daterange.h"
#endif // DATERANGE_FILTER_H
//----------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FICBASE_H
  #include "../../../fanfic/data/ficbase.h"
#endif // FICBASE_H
#ifndef JFFNFICOBJ
  #include "../../../fanficnet/data/ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED
//----------------------------------------
#include <assert.h>
//=============================================================================
// constructors
//-------------------
jfDateRangeFilter::jfDateRangeFilter(const QString& filter_name):jfFilterBase(filter_name) {
    // the dates are invalid, thus empty by default
}
//-----------------
jfDateRangeFilter::jfDateRangeFilter(QString&& filter_name):jfFilterBase(filter_name) {
    // the dates are invalid, thus empty by default
}
//----------------
jfDateRangeFilter::jfDateRangeFilter(const QString& filter_name, QDate instart,QDate inend):jfFilterBase(filter_name) {
  bool setOkay = SetDates(instart,inend);
  jerror::AssertLog(setOkay, "jfDateRangeFilter", "The dates in the constructor are invalid!");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter
bool jfDateRangeFilter::IsEmpty() const {
  return (!newdate.isValid()) && (!olddate.isValid());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfDateRangeFilter::ToString() const {
  // variables
  QDate xdate;
  QString result;
  // new date
  if (newdate.isValid()) {
    xdate = DayPlus(newdate,true);
    result = xdate.toString("MM/dd/yyyy");
  }
  result += " to ";
  // old date
  if (olddate.isValid()) {
    result += olddate.toString("MM/dd/yyyy");
  }
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
bool jfDateRangeFilter::SetDates(QDate instart,QDate inend) {
  // tester
  if (instart.isValid() && inend.isValid()) {
    if (instart<inend) return false;
  }
  // setting the dates
  newdate = instart;
  olddate = inend;
  // removing times for comparison. also, date range will be inclusive
  if (newdate.isValid()) newdate = DayPlus(newdate,false);
  // finishing
  return true;
}
//----------------------------------------------------------------------------
QDate jfDateRangeFilter::GetValue(bool first) const {
  if (first) {
    QDate zvalue = DayPlus(newdate,true);
    return zvalue;
  }
  else return olddate;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfDateRangeFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    // constants
    const QString fname = "jfDateRangeFilter::FromStringInner";
    // variables
    const QString npointer;
    int divider;
    QString p1, p2;
    // special case to start
    /**/JDEBUGLOGS(fname,1,sourcedata)
    if (sourcedata=="") {
      newdate = QDate();
      olddate = QDate();
      return true;
    }
    /**/JDEBUGLOG(fname,2)
    // splitting as per normal as per normal
    divider = sourcedata.indexOf("to");
    if (divider<0) return false;
    /**/JDEBUGLOG(fname,3)
    p1 = sourcedata.mid(0,divider);
    p2 = sourcedata.mid(divider+2);
    p1 = p1.trimmed();
    p2 = p2.trimmed();
    /**/JDEBUGLOG(fname,4)
    // assigning based on values
    QDate x_newdate, x_oldate;
    if (p1.isEmpty()) x_newdate = QDate();
    else {
      x_newdate = QDate::fromString(p1,"MM/dd/yyyy");
      /**/JDEBUGLOGS(fname,5,p1)
      if (!x_newdate.isValid()) {
          error_out = "Failed to parse the to date.";
          return false;
      }
      x_newdate = DayPlus(x_newdate,false);
    }
    if (p2.isEmpty()) x_oldate = QDate();
    else {
      x_oldate = QDate::fromString(p2,"MM/dd/yyyy");
      if (!x_oldate.isValid()) {
          error_out = "Gailed to parse the from date.";
          return false;
      }
    }
    /**/JDEBUGLOG(fname,7)
    newdate = x_newdate;
    olddate = x_oldate;
    return true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
QDate jfDateRangeFilter::DayPlus(QDate inval, bool decrement) const {
  // special case
  if (!inval.isValid()) return inval;
  // other cases
  if (decrement) inval = inval.addDays(-1);
  else inval = inval.addDays(1);
  return inval;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfDateRangeFilter::RealMatchCore(const QDate intest) const {
  // special cases, will ignore
  if (!intest.isValid()) return true;
  if (IsEmpty()) return true;
  // testing
  if (newdate.isValid()) {
    if (intest>=newdate) return false;
  }
  if (olddate.isValid()) {
    if (intest<olddate) return false;
  }
  return true;
}

//***************************************************************************
const jfFilterTypeMeta UPDATED_DATE_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::DATE, "UpdatedDateFilter", "Updated Date Filter",
          QString("Filters items depending on their updated date (If the item has") +
                  " never been updated, the published date is used instead). The " +
                  "default is to include everything.",
          IdForGenericFanfic(), createFilter<jfUpdatedDateFilter> );
// =======================================================================
// constructors
//----------------------
jfUpdatedDateFilter::jfUpdatedDateFilter(const QString& filter_name):jfDateRangeFilter(filter_name) {

}
//-----------------------------------------------
jfUpdatedDateFilter::jfUpdatedDateFilter(QString&& filter_name):jfDateRangeFilter(filter_name) {

}
//----------------------
jfUpdatedDateFilter::jfUpdatedDateFilter(const QString& filter_name, QDate instart,QDate inend):
    jfDateRangeFilter(filter_name, instart,inend) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfUpdatedDateFilter::GetTypeMetaInfo() const  {
    return UPDATED_DATE_FILTER_INFO;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfFilterBase* jfUpdatedDateFilter::GenCopy() const {
  jfUpdatedDateFilter* result = new jfUpdatedDateFilter(name, newdate, olddate);
  result->description = description;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfUpdatedDateFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  const jfGenericFanfic* rvalue;
  QDate gdate;
  // asserts
  assert(testelem!=NULL);
  // doing
  rvalue = dynamic_cast<const jfGenericFanfic*>(testelem);
  gdate = rvalue->GetUpdates();
  return RealMatchCore(gdate);
}
//***************************************************************************
const jfFilterTypeMeta PUBLISHED_DATE_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::DATE, "PublishedDateFilter", "Published Date Filter",
          QString("Filters items depending on their Published date. The default is") +
                  " to include everything.",
          IdForFFNItemCore(), createFilter<jfPublishedDateFilter> );
// =======================================================================

// constructors
//----------------------
jfPublishedDateFilter::jfPublishedDateFilter(const QString& filter_name):jfDateRangeFilter(filter_name) {

}
jfPublishedDateFilter::jfPublishedDateFilter(QString&& filter_name):jfDateRangeFilter(filter_name) {

}
//----------------------
jfPublishedDateFilter::jfPublishedDateFilter(const QString& filter_name, QDate instart,QDate inend):
            jfDateRangeFilter(filter_name, instart,inend) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets type info
const jfFilterTypeMeta& jfPublishedDateFilter::GetTypeMetaInfo() const {
    return PUBLISHED_DATE_FILTER_INFO;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfFilterBase* jfPublishedDateFilter::GenCopy() const {
  jfPublishedDateFilter* result = new jfPublishedDateFilter(name, newdate, olddate);
  result->description = description;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfPublishedDateFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  const jfFFNItemCore* rvalue;
  QDate gdate;
  // asserts
  assert(testelem!=NULL);
  // doing
  rvalue = dynamic_cast<const jfFFNItemCore*>(testelem);
  gdate = rvalue->GetPublished();
  return RealMatchCore(gdate);
}
/*****************************************************************************/

