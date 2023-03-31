/******************************************************************************
Name    :   daterange.cpp
Author  :   John Q Metro
Purpose :   Filters relating to dates
Created :   November 29, 2011
Conversion to QT Started October 1, 2013
Updated :   December 3, 2021 (fixed bug in ToString)
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
jfDateRangeFilter::jfDateRangeFilter():jfBaseFilter() {
  validdata = true;
}
//----------------
jfDateRangeFilter::jfDateRangeFilter(QDate instart,QDate inend) {
  validdata = SetDates(instart,inend);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match against the filter
bool jfDateRangeFilter::isEmpty() const {
  return (!newdate.isValid()) && (!olddate.isValid());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
bool jfDateRangeFilter::FromString(const QString& sourcedata) {
  // constants
  const QString fname = "jfDateRangeFilter::FromString";
  // variables
  const QString npointer;
  QDate testdate;
  int divider;
  QString p1, p2;
  // special case to start
  /**/JDEBUGLOGS(fname,1,sourcedata)
  if (sourcedata=="") {
    newdate = QDate();
    olddate = QDate();
    validdata = true;
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
  if (p1.isEmpty()) newdate = QDate();
  else {
    testdate = QDate::fromString(p1,"MM/dd/yyyy");
    /**/JDEBUGLOGS(fname,5,p1)
    if (!testdate.isValid()) return false;
    newdate = DayPlus(testdate,false);
  }
  if (p2.isEmpty()) olddate = QDate();
  else {
    testdate = QDate::fromString(p2,"MM/dd/yyyy");
    /**/JDEBUGLOGS(fname,6,p2)
    if (!testdate.isValid()) return false;
    olddate = testdate;
  }
  /**/JDEBUGLOG(fname,7)
  validdata = true;
  return true;
}
//-------------------------------------------------------------
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
  validdata = true;
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
  if (isEmpty()) return true;
  // testing
  if (newdate.isValid()) {
    if (intest>=newdate) return false;
  }
  if (olddate.isValid()) {
    if (intest<olddate) return false;
  }
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//-------------------------------------------------------------
bool jfDateRangeFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  buffer = ToString();
  (*outfile) << buffer << "\n";
  return true;
}
//-------------------------------------------------------------
bool jfDateRangeFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfDateRangeFilter::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadLine(cline,funcname)) return false;
  // there is only one line, and one filed, so this is pretty simple
  resx = FromString(cline);
  if (!resx) return infile->BuildError("The date range string is invalid!");
  else return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* since different types are stored together, the text file reprentation may have objects of varying length */
size_t jfDateRangeFilter::ExtraLines() const { return 1;}
//***************************************************************************
// constructors
//----------------------
jfUpdatedDateFilter::jfUpdatedDateFilter():jfDateRangeFilter() {}
//----------------------
jfUpdatedDateFilter::jfUpdatedDateFilter(QDate instart,QDate inend):jfDateRangeFilter(instart,inend) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets a description
QString jfUpdatedDateFilter::GetTypeDescription() const {
    return "Filters items depending on their updated date (If the item has \
never been updated, the published date is used instead). The default is to \
include everything.";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfBaseFilter* jfUpdatedDateFilter::GenCopy() const {
  jfUpdatedDateFilter* result;
  result = new jfUpdatedDateFilter();
  CopyOver(result);
  result->SetDates(newdate,olddate);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfUpdatedDateFilter::GetTypeID() const { return "UpdatedDateFilter";}
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
// constructors
//----------------------
jfPublishedDateFilter::jfPublishedDateFilter():jfDateRangeFilter() {}
//----------------------
jfPublishedDateFilter::jfPublishedDateFilter(QDate instart,QDate inend):jfDateRangeFilter(instart,inend) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets a description
QString jfPublishedDateFilter::GetTypeDescription() const {
    return "Filters items depending on their Published date. The default is to \
include everything.";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy
jfBaseFilter* jfPublishedDateFilter::GenCopy() const {
  jfPublishedDateFilter* result;
  result = new jfPublishedDateFilter();
  CopyOver(result);
  result->SetDates(newdate,olddate);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special meta-information
QString jfPublishedDateFilter::GetTypeID() const { return "PublishedDateFilter";}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfPublishedDateFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  const jfFFNItem* rvalue;
  QDate gdate;
  // asserts
  assert(testelem!=NULL);
  // doing
  rvalue = dynamic_cast<const jfFFNItem*>(testelem);
  gdate = rvalue->GetPublished();
  return RealMatchCore(gdate);
}
/*****************************************************************************/

