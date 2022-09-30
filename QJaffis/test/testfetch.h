/*****************************************************************************
Name    : testfetch.h
Basic   : An implementation of jfPageFetchInterface that wraps others, for testing
Author  : John Q Metro
Started : June 15, 2022
Updated : June 18, 2022

******************************************************************************/
#ifndef TESTFETCH_H
    #define TESTFETCH_H
#endif // TESTFETCH_H

#ifndef FETCHWRAPBASE_H
    #include "../fetching/core/fetchwrapbase.h"
#endif // FETCHWRAPBASE_H
//----------------------
#ifndef FETCHQT_H
    #include "../fetching/core/fetchqt.h"
#endif // FETCHQT_H
#ifndef FETCHPYTHON_H
    #include "../fetching/core/fetchpython.h"
#endif // FETCHPYTHON_H

/***************************************************************************/
class jfTestsFetcherWrapper : public virtual jfFetchWrapBase {
  public:

    jfTestsFetcherWrapper(const QStringList* user_agents, jfPythonPaths* python_paths);

    virtual ~jfTestsFetcherWrapper();
  protected:
    virtual jfPageFetchInterface* FetcherForURL(const QString& inurl) const;


    jfPageFetchWithQt* default_fetcher;
    jfPageFetchWithPython* python_fetcher;

};

/***************************************************************************/
