/******************************************************************************
Name    :   ao3_specials2.cpp
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   February 19, 2023
******************************************************************************/
#ifndef AO3_SPECIALS2_H_INCLUDED
  #include "ao3_specials2.h"
#endif // AO3_SPECIALS2_H_INCLUDED
//--------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//-----------------------------------------------
#include <assert.h>
#include <QRegExp>
//---------------------------------
const QString warn3_ac = "VDRPN";
const QString warnlist[5] = {"Violence","Character Death","Rape","Underage Sex","Warnings Avoided"};

/*****************************************************************************/

jfAO3WarnFilter::jfAO3WarnFilter():jfSpecialsFilter()  {}
//---------------------------
jfAO3WarnFilter::jfAO3WarnFilter(const QString& insrc):jfSpecialsFilter() {
  FromString(insrc);
}
//---------------------------
// string conversion
bool jfAO3WarnFilter::FromString(const QString& sourcedata) {
  QRegExp xrpos = QRegExp("[^"+warn3_ac+"]");
  if (!sourcedata.contains(xrpos)) {
    value = sourcedata;
    validdata = true;
  }
  else validdata = false;
  return validdata;
}

//---------------------------
// gets a description
QString jfAO3WarnFilter::GetTypeDescription() const {
  return "Excludes fics based on whether they have the \
specified archive warnings.";
}
//---------------------------
// copy
jfBaseFilter* jfAO3WarnFilter::GenCopy() const {
  jfAO3WarnFilter* qval = new jfAO3WarnFilter(value);
  return qval;
}
//---------------------------
// special meta-information
QString jfAO3WarnFilter::GetTypeID() const {
  return "AO3WarnFilter";
}
//---------------------------
// custom methods
void jfAO3WarnFilter::SetToFull() {
 value = warn3_ac;
}
//-----------------------------
bool jfAO3WarnFilter::IsFull() const {
  return value==warn3_ac;
}
//---------------------------
// the core matching method
bool jfAO3WarnFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const QString fname = "jfAO3WarnFilter::CoreMatch";
  QRegExp qpval = QRegExp("["+value+"]");
  // starting
  const jfAO3Fanfic* temp = dynamic_cast<const jfAO3Fanfic*>(testelem);
  QChar warn1 = temp->GetWarnflag();
  QString warn2 = temp->GetWarntag();
  // contains warings
  if (warn1=='W') {
    if (warn2.contains(qpval)) return false;
    else return true;
  }
  else if (warn1=='N') {
    if (!value.contains(warn1)) return false;
    else return true;
  }
  else return true;
}
/*****************************************************************************/
