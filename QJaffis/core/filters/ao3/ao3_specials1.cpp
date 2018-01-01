/******************************************************************************
Name    :   ao3_specials1.cpp
Author  :   John Q Metro
Purpose :   AO3 rating and orientation filters
Created :   September 9, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   September 11, 2012
******************************************************************************/
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
//-------------------------------------------
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#ifndef AO3_CONSTS_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED
//----------------------------------------
#include <assert.h>
#include <QRegExp>
/*****************************************************************************/
// constructors
//---------------------------
jfSpecialsFilter::jfSpecialsFilter():jfBaseFilter() {}
//+++++++++++++++++++++++++++++++++++++++++++
// public methods implemented here
//---------------------------
bool jfSpecialsFilter::isEmpty() const {
  return (value.isEmpty() || IsFull());
}
//---------------------------
QString jfSpecialsFilter::ToString() const {
  return value;
}
//---------------------------
bool jfSpecialsFilter::Includes(QChar test) const {
  return value.contains(test);
}
//+++++++++++++++++++++++++++++++++++++++++++
// file i/o
//---------------------------
bool jfSpecialsFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding
  (*outfile) << value << "\n";
  return true;
}
//---------------------------
bool jfSpecialsFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfSpecialsFilter::ReadRestFromFile";
  // input data
  QString cline;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  // parsing the line
  if (!infile->ReadLine(cline,funcname)) return false;
  if (!FromString(cline)) return infile->BuildError("The characters do not match the filter!");
  return true;
}
//---------------------------
size_t jfSpecialsFilter::ExtraLines() const { return 1; }
//========================================================================
// constructors
//------------------------------------
jfAO3RatingFilter::jfAO3RatingFilter():jfSpecialsFilter() {}
//------------------------------------
jfAO3RatingFilter::jfAO3RatingFilter(const QString& sourcedata):jfSpecialsFilter() {
  FromString(sourcedata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//------------------------------------
bool jfAO3RatingFilter::FromString(const QString& sourcedata) {
  QRegExp xrpos = QRegExp("[^"+ao3con::rating_ac+"]");
  if (!sourcedata.contains(xrpos)) {
    value = sourcedata;
    validdata = true;
  }
  else validdata = false;
  return validdata;
}
//------------------------------------
QString jfAO3RatingFilter::GetTypeDescription() const {
  return "Checks if the rating for the tested fanfic is one of the \
selected ratings.";
}
//------------------------------------
jfBaseFilter* jfAO3RatingFilter::GenCopy() const {
  jfAO3RatingFilter* qval = new jfAO3RatingFilter(value);
  return qval;
}
//------------------------------------
QString jfAO3RatingFilter::GetTypeID() const {
  return "AO3RatingFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented custom methods
//------------------------------------
void jfAO3RatingFilter::SetToFull() {
  value = ao3con::rating_ac;
  validdata = true;
}
//------------------------------------
bool jfAO3RatingFilter::IsFull() const {
  return value==ao3con::rating_ac;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAO3RatingFilter::CoreMatch(const jfBasePD* testelem) const {
  const jfAO3Fanfic* testval = dynamic_cast<const jfAO3Fanfic*>(testelem);
  return value.contains(testval->GetRating());
}
//========================================================================
// constructors
//------------------------------------
jfAO3OrientationFilter::jfAO3OrientationFilter():jfSpecialsFilter() {}
//------------------------------------
jfAO3OrientationFilter::jfAO3OrientationFilter(const QString& sourcedata):jfSpecialsFilter() {
  FromString(sourcedata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//------------------------------------
bool jfAO3OrientationFilter::FromString(const QString& sourcedata) {
  QRegExp xrpos = QRegExp("[^"+ao3con::orient_ac+"]");
  if (!sourcedata.contains(xrpos)) {
    value = sourcedata;
    validdata = true;
  }
  else validdata = false;
  return validdata;
}
//------------------------------------
QString jfAO3OrientationFilter::GetTypeDescription() const {
  return "Checks if the Sexual Orientation for the tested fanfic is one \
of the selected Orientations.";
}
//------------------------------------
jfBaseFilter* jfAO3OrientationFilter::GenCopy() const {
  jfAO3OrientationFilter* qval = new jfAO3OrientationFilter(value);
  return qval;
}
//------------------------------------
QString jfAO3OrientationFilter::GetTypeID() const {
  return "AO3OrientationFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented custom methods
//------------------------------------
void jfAO3OrientationFilter::SetToFull() {
  value = ao3con::orient_ac;
  validdata = true;
}
//------------------------------------
bool jfAO3OrientationFilter::IsFull() const {
  return value==ao3con::orient_ac;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAO3OrientationFilter::CoreMatch(const jfBasePD* testelem) const {
  const jfAO3Fanfic* testval = dynamic_cast<const jfAO3Fanfic*>(testelem);
  return value.contains(testval->GetOrientation());
}
/*****************************************************************************/
