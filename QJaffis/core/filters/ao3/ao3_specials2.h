/******************************************************************************
Name    :   ao3_specials2.h
Author  :   John Q Metro
Purpose :   AO3 warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   May 6, 2023
******************************************************************************/
#ifndef AO3_SPECIALS2_H_INCLUDED
#define AO3_SPECIALS2_H_INCLUDED
#endif // AO3_SPECIALS2_H_INCLUDED
//--------------------------------

#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED

#include <QVector>
/*****************************************************************************/
extern const QString warn3_ac;
extern const QString warnlist[];
//-----------------------------

//========================================================================
extern const jfFilterTypeMeta AO3_WARNINGS_FILTER_INFO;

// excludes fics based on warnings
class jfAO3WarnFilter : public jfSpecialsFilter {
  public:
    // constructors
    jfAO3WarnFilter(const QString& filter_name);
    jfAO3WarnFilter(QString&& filter_name);
    jfAO3WarnFilter(const QString& filter_name, const QString& insrc);
    // gets a description
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    // copy
    virtual jfFilterBase* GenCopy() const override;
    // custom methods
    bool Includes(QChar test) const;
    virtual void SetToFull() override;
    virtual bool IsFull() const override;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override final;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
/*****************************************************************************/
