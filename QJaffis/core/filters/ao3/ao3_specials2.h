/******************************************************************************
Name    :   ao3_specials2.h
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   February 19, 2023
******************************************************************************/
#ifndef AO3_SPECIALS2_H_INCLUDED
#define AO3_SPECIALS2_H_INCLUDED
#endif // AO3_SPECIALS2_H_INCLUDED
//--------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED

#include <QVector>
/*****************************************************************************/
extern const QString warn3_ac;
extern const QString warnlist[];
//-----------------------------

//========================================================================
// excludes fics based on warnings
class jfAO3WarnFilter : public jfSpecialsFilter {
  public:
    // constrcutors
    jfAO3WarnFilter();
    jfAO3WarnFilter(const QString& insrc);
    // string conversion
    virtual bool FromString(const QString& sourcedata);
    // gets a description
    virtual QString GetTypeDescription() const;
    // copy
    virtual jfBaseFilter* GenCopy() const;
    // special meta-information
    virtual QString GetTypeID() const;
    // custom methods
    bool Includes(QChar test) const;
    virtual void SetToFull();
    virtual bool IsFull() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
};
/*****************************************************************************/
