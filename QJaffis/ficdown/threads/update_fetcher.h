/*****************************************************************************
Name    : update_fetcher.h
Basic   : jfPageFetchInterface for updating fanfics
Author  : John Q Metro
Started : July 2, 2022
Updated : July 3, 2022

******************************************************************************/
#ifndef UPDATE_FETCHER_H
    #define UPDATE_FETCHER_H
#endif // UPDATE_FETCHER_H
// -----------------------------
#ifndef FICEXTRACT_H_INCLUDED
  #include "../data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef FETCHBASE_H
    #include "../../fetching/core/fetchbase.h"
#endif // FETCHBASE_H
#ifndef FETCHWRAPBASE_H
    #include "../../fetching/core/fetchwrapbase.h"
#endif // FETCHWRAPBASE_H
#ifndef FETCHQT_H
    #include "../../fetching/core/fetchqt.h"
#endif // FETCHQT_H
// -----------------------------
#include <QMap>
#include <QStringList>
/*****************************************************************************/
// parameter that separates creating the sub-fetchers from the fetcher
// note that deleting the added fetch interfaces will be done inside this class
class jfFanficFetcherPack {
  public:
    QMap<jf_FICTYPE,jfPageFetchInterface*> fetcher_map;
    jfFanficFetcherPack(const QStringList* user_agents);
    // fetcher
    bool SetFetcherFor(jf_FICTYPE fictype, jfPageFetchInterface* fetcher);
    bool HasFetcherFor(jf_FICTYPE fictype) const;
    jfPageFetchInterface* GetFetcher(jf_FICTYPE fictype) const;
    jfPageFetchInterface* GetDefaultFetcher() const;

    virtual ~jfFanficFetcherPack();
  protected:
    jfPageFetchWithQt* default_fetcher;

};
// ============================================================
// url to fic type!
bool FindFicTypeFor(const QString& url, jf_FICTYPE& found_type);

// ============================================================
// makes jfFanficFetcherPack
jfFanficFetcherPack* MakeFetcherPack(const QStringList* user_agents);

// ============================================================

class jfFanficUpdateFetcher : public virtual jfFetchWrapBase {
  public:
    jfFanficUpdateFetcher(jfFanficFetcherPack* fetcher_pack_in);
    virtual ~jfFanficUpdateFetcher();
  protected:
    virtual jfPageFetchInterface* FetcherForURL(const QString& inurl) const;
    jfFanficFetcherPack* fetcher_pack;

};

/*****************************************************************************/
