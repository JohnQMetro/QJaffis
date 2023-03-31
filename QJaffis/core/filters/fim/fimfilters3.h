/******************************************************************************
Name    :   fimfilters3.h
Author  :   John Q Metro
Purpose :   Filters for fimfiction.net, new in 2018
Created :   January 21, 2018
Updated :   January 21, 2018
******************************************************************************/
#ifndef FIMFILTERS3_H
    #define FIMFILTERS3_H
#endif // FIMFILTERS3_H
//-----------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../extended/morefilters1.h"
#endif
/*****************************************************************************/
// filters using content type
class jfFIMContentTypeFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMContentTypeFilter();
    jfFIMContentTypeFilter(const jfFIMContentTypeFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty();
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
    virtual bool ModifyList(QStringList* templist) const;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify();
};
//------------------------------------------------------
// filters using warnings
class jfFIMWarningsFilter : public jfTagFilterCore {
  public:
    // constructors
    jfFIMWarningsFilter();
    jfFIMWarningsFilter(const jfFIMWarningsFilter& insrc);
    // getting and setting values
    virtual bool SetToEmpty();
    // redefined virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
    virtual bool ModifyList(QStringList* templist) const;
    // check the tags against a list of pre-approved tags
    virtual bool DoVerify();
};

/*****************************************************************************/
