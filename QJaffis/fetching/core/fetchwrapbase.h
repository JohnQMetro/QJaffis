/*****************************************************************************
Name    : fetchwrapbase.h
Basic   : A base implementation of jfPageFetchInterface that wraps others.
Author  : John Q Metro
Started : June 18, 2022
Updated : June 18, 2022

******************************************************************************/
#ifndef FETCHWRAPBASE_H
    #define FETCHWRAPBASE_H
#endif // FETCHWRAPBASE_H
//----------------------
#ifndef FETCHBASE_H
    #include "fetchbase.h"
#endif // FETCHBASE_H

/*****************************************************************************/
// if a single set of downloads involves hitting multiple sites, this can be useful
class jfFetchWrapBase : public virtual jfPageFetchInterface {
  public:

    jfFetchWrapBase();
    // implemented public methods
    virtual jfFETCH_ERROR Download(const QString& inurl, const QString& cookie = "");
    virtual bool Abort();
    virtual void Shutdown();

    virtual ~jfFetchWrapBase();
  protected:
    virtual jfPageFetchInterface* FetcherForURL(const QString& inurl) const = 0;
    jfPageFetchInterface* current_fetcher;

};
/*****************************************************************************/
