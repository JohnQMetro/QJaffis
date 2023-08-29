/******************************************************************************
Name    :   minmax.h
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016, extracting from older morefilters1.h
Updated :  April 8, 2023 (Rebasing)
******************************************************************************/
#ifndef MINMAX_FILTER_H
#define MINMAX_FILTER_H
#endif // MINMAX_FILTER_H
//---------------------------------------

#include "../base/basefilter.h"

#ifndef JFMISCTYPES1
  #include "../../objs/misc_types1.h"
#endif
/******************************************************************************/
class jfMinMaxUFilter : public jfFilterBase {
  public:
    // constructors
    jfMinMaxUFilter(const QString& name);
    jfMinMaxUFilter(const QString& name, size_t inmin, size_t inmax);
    // setting values
    bool SetValues(size_t inmin, size_t inmax);
    bool SetIntValues(int inmin, int inmax);
    // getting values
    size_t GetMin() const;
    size_t GetMax() const;
    IntPair* GetMinMax() const;
    // redefined virtual methods
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    bool TestMatch(size_t invalue) const;

    // internal values
    size_t min, max;
};

// ==================================================================
extern const jfFilterTypeMeta WORDCOUNT_FILTER_INFO;
// ==================================================================

class jfWordCountFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfWordCountFilter(const QString& name);
    jfWordCountFilter(const QString& name, size_t inmin, size_t inmax);
    // redefined virtual methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};

/******************************************************************************/



