/*****************************************************************************
Name    : testswrapper.h
Basic   : An implementation of jfTestsDelegate that wraps a pointer to jfPageParserBase
Author  : John Q Metro
Started : May 21, 2022
Updated : August 27, 2023

******************************************************************************/
#ifndef TESTSWRAPPER_H
    #define TESTSWRAPPER_H
#endif // TESTSWRAPPER_H
//------------------------
#ifndef PAGEFETCH_H
    #include "pagefetch.h"
#endif // PAGEFETCH_H
#ifndef BASEPARSE_H
  #include "../baseparse.h"
#endif // BASEPARSE_H
//---------------------------
#include <QString>
/*****************************************************************************/
class jfTestsParserWrapper : public virtual jfTestsDelegate {
  public:
    jfTestsParserWrapper(jfPageParserBase* parser_old);

    virtual bool setPageIndex(const size_t& page_index) override;

    virtual QString GetCookie() const override;
    virtual bool TestIsBlocked(const QString* downpage) const override;
    virtual bool TestNotMissing(const QString* downpage) const override;
    virtual bool TestNotTruncated(const QString* downpage) const override;
    virtual bool TestNotRateLimited(const QString* downpage) const override;
    virtual bool ResolveRedirect(const QString& raw_redirect, QString& full_redirect) const override;
    virtual ~jfTestsParserWrapper();
  protected:
    jfPageParserBase* parser_link;
    mutable int badpagecount;
};

/******************************************************************************/
