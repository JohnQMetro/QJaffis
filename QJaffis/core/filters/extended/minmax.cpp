/******************************************************************************
Name    :   minmax.cpp
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016, extracting from older morefilters1.h
Updated :
******************************************************************************/
#ifndef MINMAX_FILTER_H
  #include "minmax.h"
#endif // MINMAX_FILTER_H
//---------------------------------------
#ifndef FICBASE_H
  #include "../../../fanfic/data/ficbase.h"
#endif // FICBASE_H
#include <assert.h>
/*****************************************************************************/
// constructors
//---------------------------------------------------
jfMinMaxUFilter::jfMinMaxUFilter():jfBaseFilter() {
  validdata = false;
  min = 0;
  max = 0;
}
//---------------------------------------------------
jfMinMaxUFilter::jfMinMaxUFilter(size_t inmin, size_t inmax):jfBaseFilter() {
  validdata = SetValues(inmin,inmax);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting values
bool jfMinMaxUFilter::SetValues(size_t inmin, size_t inmax) {
  if ((inmax>=inmin) || (inmax==0)) {
    min = inmin;
    max = inmax;
    validdata = true;
  }
  else validdata = false;
  return validdata;
}
//-----------------------------------------------------------------------
// getting values
//-------------------------------------------
size_t jfMinMaxUFilter::GetMin() const { return min; }
//-------------------------------------------
size_t jfMinMaxUFilter::GetMax() const { return max; }
//-------------------------------------------
IntPair* jfMinMaxUFilter::GetMinMax() const {
  IntPair* result;
  result = new IntPair;
  result->A = min;
  result->B = max;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual methods
bool jfMinMaxUFilter::isEmpty() const { return false; }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading from a string representation
//---------------------------------------------------
bool jfMinMaxUFilter::FromString(const QString& sourcedata) {
  jfLineParse* lparser;
  lparser = new jfLineParse(sourcedata);
  validdata = false;
  if (lparser->NNotX(2)) delete lparser;
  else {
    if ((lparser->SIntVal(0,min)) && (lparser->SIntVal(1,max))) validdata = true;
    delete lparser;
  }
  return validdata;
}
//---------------------------------------------------
QString jfMinMaxUFilter::ToString() const {
  QString result;
  result = QString::number(min) + ";";
  result += QString::number(max);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfMinMaxUFilter::TestMatch(size_t invalue) const {
  assert(invalue<=999999999);
  if (invalue<min) return false;
  else if (max==0) return true;
  else return (invalue<=max);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//-----------------------------------
bool jfMinMaxUFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  buffer = ToString();
  (*outfile) << buffer << "\n";
  return true;
}
//-----------------------------------
bool jfMinMaxUFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfMinMaxUFilter::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if(!infile->ReadLine(cline,funcname)) return false;
  // there is only one line, and one filed, so this is pretty simple
  resx = FromString(cline);
  if (!resx) return infile->BuildError("The min-max string is invalid!");
  else return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* since different types are stored together, the text file reprentation
may have objects of varying length */
size_t jfMinMaxUFilter::ExtraLines() const { return 1;}
//***************************************************************************
// constructors
//---------------------------------------------------
jfWordCountFilter::jfWordCountFilter():jfMinMaxUFilter() {}
//---------------------------------------------------
jfWordCountFilter::jfWordCountFilter(size_t inmin, size_t inmax):jfMinMaxUFilter(inmin,inmax) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfWordCountFilter::GetTypeID() const {
  return "WordCountFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------------
QString jfWordCountFilter::GetTypeDescription() const {
    return "For items that have word counts, this filter passes if \
the word count is between min and max, inclusive.";
}
//---------------------------------------------------
jfBaseFilter* jfWordCountFilter::GenCopy() const {
  jfWordCountFilter* result;
  result = new jfWordCountFilter(min,max);
  CopyOver(result);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfWordCountFilter::CoreMatch(const jfBasePD* testelem) const {
  // variables
  const jfGenericFanfic2* rvalue;
  size_t cvalue;
  // checks
  assert(testelem!=NULL);
  // determining the type
  rvalue = dynamic_cast<const jfGenericFanfic2*>(testelem);
  cvalue = rvalue->GetWordcount();
  // checking the wordcount
  return TestMatch(cvalue);
}
/*****************************************************************************/
