/******************************************************************************
Name    :   ao3_specials2.cpp
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   May 6, 2023
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
const jfFilterTypeMeta AO3_WARNINGS_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::MISC, "AO3WarnFilter", "AO3 Warning Filter",
          QString("Excludes fics based on whether they have the specified archive warnings."),
          IdForAO3Fanfic(), createFilter<jfAO3WarnFilter> );

// -----------------------------------------------
const QRegularExpression NOT_A_WARNING = QRegularExpression("[^"+warn3_ac+"]");

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

jfAO3WarnFilter::jfAO3WarnFilter(const QString& filter_name):jfSpecialsFilter(filter_name) {}
// --------------------------------------
jfAO3WarnFilter::jfAO3WarnFilter(QString&& filter_name):jfSpecialsFilter(filter_name) {}
//---------------------------
jfAO3WarnFilter::jfAO3WarnFilter(const QString& filter_name, const QString& insrc):jfSpecialsFilter(filter_name) {
    QString error;
    bool okay = FromStringInner(insrc, error);
    jerror::AssertLog(okay, "jfAO3WarnFilter::jfAO3WarnFilter", error);
}
//---------------------------
const jfFilterTypeMeta& jfAO3WarnFilter::GetTypeMetaInfo() const {
    return AO3_WARNINGS_FILTER_INFO;
}
//---------------------------
// copy
jfFilterBase* jfAO3WarnFilter::GenCopy() const {
  jfAO3WarnFilter* qval = new jfAO3WarnFilter(name, value);
  qval->description = description;
  return qval;
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
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfAO3WarnFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    if (sourcedata.contains(NOT_A_WARNING)) {
        error_out = "Input contains non-warning characters!";
        return false;
    }
    value = sourcedata;
    return true;
}
// ---------------------------------------------
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
