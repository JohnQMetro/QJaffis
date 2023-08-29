/******************************************************************************
Name    :   morefilters1.h
Author  :   John Q Metro
Purpose :   More Filters
Created :   December 6, 2010
Conversion to QT Started September 30, 2013
Updated :   April 14, 2023 (rebasing, added verify check)
******************************************************************************/
#ifndef MOREFILTERS1_H_INCLUDED
#define MOREFILTERS1_H_INCLUDED
#endif
//---------------------------------------

#include "../base/basefilter.h"

#ifndef JFMISCTYPES1
	#include "../../objs/misc_types1.h"
#endif
/******************************************************************************/
enum class jfTagFilterEmpty {
    UNKNOWN, IS_EMPTY, NOT_EMPTY
};
//----------------------------------------

class jfTagFilterCore : public jfFilterBase {
  public:
    // constructors
    jfTagFilterCore(const QString& filter_name);
    jfTagFilterCore(QString&& filter_name);
    jfTagFilterCore(const jfTagFilterCore& insrc);

    // getting and setting values
    bool SetTags(const jfTagListing* insource);
    jfTagListing* GetTagsCopy() const;
    virtual bool SetToEmpty() = 0;
    // redefined virtual methods
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;
  protected:
    void DetermineEmptyState();
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    bool MatchVsData(const QString& incheck) const;
    void MakeNullResult();
    virtual bool ModifyList(QStringList* templist) const = 0;
    // check the tags against a list of pre-approved tags
    bool VerifyTags(size_t NUMTAGIN, const QString intags[], QStringList* replist);
    bool VerifyTags(const QStringList* sourcelist, QStringList* replist);
    bool VerifyCheck(jfTagListing* to_check, const QStringList* sourcelist, const QStringList* replist) const;

    virtual bool DoVerify() = 0;
    virtual bool DoVerifyCheck(jfTagListing* to_check) const = 0;
    void SetEmpty(size_t NUMTAGIN, const QString intags[]);
    void SetEmpty(const QStringList* sourcelist);

    // internal values
    jfTagListing* thedata;
    QString cm;
    bool nullresult;
    jfTagFilterEmpty empty_state;
};
//==========================================================================
enum class jfCompletedState {
    INCOMPLETE, COMPLETE, EITHER
};
// --------------------------------------------------------
extern const jfFilterTypeMeta COMPLETED_FILTER_INFO;
// ----------------------------------------------------------

class jfCompletedFilter : public jfFilterBase {
  public:
    // constructors
    jfCompletedFilter(const QString& filter_name);
    jfCompletedFilter(QString&& filter_name);
    jfCompletedFilter(const QString& filter_name, jfCompletedState value);
    jfCompletedFilter(QString&& filter_name, jfCompletedState value);
    // match against the filter
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;
    // data setting
    void SetValue(jfCompletedState value_in);
    void SetCompleted(bool value_in);
    jfCompletedState GetValue() const;
    void SetEmpty();
    // copy
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;

    // internal data
    jfCompletedState state_wanted;
};
/******************************************************************************/
