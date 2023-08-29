/******************************************************************************
Name    :   minmax.cpp
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016, extracting from older morefilters1.h
Updated :  April 8, 2023
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
jfMinMaxUFilter::jfMinMaxUFilter(const QString& name):jfFilterBase(name) {
    min = max = 0;
}
// ------------------------
jfMinMaxUFilter::jfMinMaxUFilter(const QString& name, size_t inmin, size_t inmax):jfFilterBase(name) {
    assert((inmax == 0) || (inmax >= inmin));
    min = inmin;
    max = inmax;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting values
bool jfMinMaxUFilter::SetValues(size_t inmin, size_t inmax) {
    if ((inmax>=inmin) || (inmax==0)) {
        min = inmin;
        max = inmax;
        return true;
    }
    else return false;
}
// -------------------------------------------
bool jfMinMaxUFilter::SetIntValues(int inmin, int inmax) {
    if ((inmin < 0) || (inmax < 0)) return false;
    return SetValues(inmin, inmax);
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
bool jfMinMaxUFilter::IsEmpty() const {
    return (min == 0) && (max == 0);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfMinMaxUFilter::ToString() const {
  QString result;
  result = QString::number(min) + ";";
  result += QString::number(max);
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfMinMaxUFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    jfLineParse* lparser;
    lparser = new jfLineParse(sourcedata);
    bool isvalid = false;

    if (lparser->NNotX(2)) {
        error_out = "Input does not have 2 fields!";
    }
    else {
        size_t raw_min; size_t raw_max;
        if ((lparser->SIntVal(0,raw_min)) && (lparser->SIntVal(1,raw_max))) {
            if ((raw_max == 0) || (raw_max >= raw_min)) {
                isvalid = true;
                min = raw_min;
                max = raw_max;
            }
            else {
                error_out = "Input Max and Max are not ordered correctly";
            }
        }
        else error_out = "One or both of the fields is not a valid unsigned number!";
    }
    delete lparser;
    return isvalid;
}
// -------------------------------------------------------------
bool jfMinMaxUFilter::TestMatch(size_t invalue) const {
    if (min == 0) {
        return (max == 0) || (invalue < max);
    }
    else if (invalue >= min) {
        return (max == 0) || (invalue < max);
    }
    else return false;
}

//***************************************************************************
const jfFilterTypeMeta WORDCOUNT_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::COUNTS, "WordCountFilter",
                     "Word Count Filter",
                     QString("For items that have word counts, this filter passes if") +
                     "the word count is between min and max, inclusive." +
                     " A special rule is that if max is 0, there is not upper word limit.",
                     IdForGenericFanfic2(),
                     createFilter<jfWordCountFilter> );

// =========================================================================
// constructors
//---------------------------------------------------
jfWordCountFilter::jfWordCountFilter(const QString& name):jfMinMaxUFilter(name) {}
//---------------------------------------------------
jfWordCountFilter::jfWordCountFilter(const QString& name, size_t inmin, size_t inmax):jfMinMaxUFilter(name,inmin,inmax) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfWordCountFilter::GetTypeMetaInfo() const {
    return WORDCOUNT_FILTER_INFO;
}
//---------------------------------------------------
jfFilterBase* jfWordCountFilter::GenCopy() const {
    jfWordCountFilter* result;
    result = new jfWordCountFilter(name,min,max);
    result->description = description;
    return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfWordCountFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  const jfGenericFanfic2* rvalue;
  size_t cvalue;
  // checks
  // determining the type
  rvalue = dynamic_cast<const jfGenericFanfic2*>(testelem);
  cvalue = rvalue->GetWordcount();
  // checking the wordcount
  return TestMatch(cvalue);
}
/*****************************************************************************/
