/******************************************************************************
Name    :   minmax.h
Author  :   John Q Metro
Purpose :   More Filters
Created :   July 8, 2016, extracting from older morefilters1.h
Updated :
******************************************************************************/
#ifndef MINMAX_FILTER_H
#define MINMAX_FILTER_H
#endif // MINMAX_FILTER_H
//---------------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef JFMISCTYPES1
  #include "../../objs/misc_types1.h"
#endif
/******************************************************************************/
class jfMinMaxUFilter : public jfBaseFilter {
  public:
    // constructors
    jfMinMaxUFilter();
    jfMinMaxUFilter(size_t inmin, size_t inmax);
    // setting values
    bool SetValues(size_t inmin, size_t inmax);
    // getting values
    size_t GetMin() const;
    size_t GetMax() const;
    IntPair* GetMinMax() const;
    // redefined virtual methods
    virtual bool isEmpty() const;
    //virtual wxString GetTypeID() const = 0;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // additional methods
    // virtual wxString GetTypeDescription() const = 0;
    // virtual jfBaseFilter* GenCopy() const = 0;
  protected:
    bool TestMatch(size_t invalue) const;
    // the core matching method
    // virtual bool CoreMatch(const jfBasePD* testelem) const = 0;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // internal values
    size_t min, max;
};
//-------------------------------------------------------------------------
class jfWordCountFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfWordCountFilter();
    jfWordCountFilter(size_t inmin, size_t inmax);
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};

/******************************************************************************/



