//***************************************************************************
// Name:        ao3_stringfilters.h
// Author :     John Q Metro
// Purpose :    Simple expression filters specific to archiveofourown.org
// Created:     September 7, 2012
// Conversion to Qt Started Oct 3, 2013
// Updated:     May 15, 2014
//***************************************************************************
#ifndef AO3_STRINGFILTERS_H_INCLUDED
#define AO3_STRINGFILTERS_H_INCLUDED
#endif // AO3_STRINGFILTERS_H_INCLUDED
//--------------------------------------
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
//***************************************************************************
// Filtering the extra tags
class jfExtraTagFilter : public jfSimpleExpFilterCore {
  public:
    jfExtraTagFilter();
    jfExtraTagFilter(const jfExtraTagFilter& source);
    jfExtraTagFilter(jfSimpleExpr* in_source);
    virtual QString GetTypeDescription() const;
    virtual jfExtraTagFilter* Copy() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const;
};

//***************************************************************************
