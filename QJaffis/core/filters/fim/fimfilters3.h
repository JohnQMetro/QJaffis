/******************************************************************************
Name    :   fimfilters3.h
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net, new in 2018
Created :   January 21, 2018
Updated :   August 23, 2023
******************************************************************************/
#ifndef FIMFILTERS3_H
    #define FIMFILTERS3_H
#endif // FIMFILTERS3_H
//-----------------------------
#ifndef JFBASEFILTER
  #include "../base/basefilter.h"
#endif
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../extended/morefilters1.h"
#endif
/*****************************************************************************/
extern const jfFilterTypeMeta CONTENT_TYPE_FILTER_INFO;

// filters using content type
class jfFIMContentTypeFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMContentTypeFilter(const QString& filter_name);
    jfFIMContentTypeFilter(QString&& filter_name);
    jfFIMContentTypeFilter(const jfFIMContentTypeFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty() override;
    // redefined virtual methods
    virtual jfFilterBase* GenCopy() const override;
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    virtual bool ModifyList(QStringList* templist) const override;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify() override;
    virtual bool DoVerifyCheck(jfTagListing* to_check) const override;
};
//------------------------------------------------------
extern const jfFilterTypeMeta FIM_WARNING_FILTER_INFO;

// filters using warnings
class jfFIMWarningsFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMWarningsFilter(const QString& filter_name);
    jfFIMWarningsFilter(QString&& filter_name);
    jfFIMWarningsFilter(const jfFIMWarningsFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty() override;
    // redefined virtual methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    virtual bool ModifyList(QStringList* templist) const override;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify() override;
    virtual bool DoVerifyCheck(jfTagListing* to_check) const override;
};

/*****************************************************************************/
