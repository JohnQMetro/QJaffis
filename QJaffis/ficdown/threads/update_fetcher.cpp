/*****************************************************************************
Name    : update_fetcher.cpp
Basic   : jfPageFetchInterface for updating fanfics
Author  : John Q Metro
Started : July 2, 2022
Updated : July 3, 2022

******************************************************************************/
#ifndef UPDATE_FETCHER_H
    #include "update_fetcher.h"
#endif // UPDATE_FETCHER_H
// -----------------------------
#ifndef GLOBALSETTINGS_H
    #include "../../globalsettings.h"
#endif // GLOBALSETTINGS_H

#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED

#ifndef DEFAULTPATHS_H
    #include "../../defaultpaths.h"
#endif // DEFAULTPATHS_H

#ifndef FETCHQT_H
    #include "../../fetching/core/fetchqt.h"
#endif // FETCHQT_H
#ifndef FETCHPYTHON_H
    #include "../../fetching/core/fetchpython.h"
#endif // FETCHPYTHON_H
/*****************************************************************************/
// parameter that separates creating the sub-fetchers from the fetcher

jfFanficFetcherPack::jfFanficFetcherPack(const QStringList* user_agents) {
    fetcher_map = QMap<jf_FICTYPE,jfPageFetchInterface*>();
    default_fetcher = new jfPageFetchWithQt(user_agents);
}
//++++++++++++++++++++++++++++++++++++++++++
// fetcher
bool jfFanficFetcherPack::SetFetcherFor(jf_FICTYPE fictype, jfPageFetchInterface* fetcher) {
    if (fetcher == NULL) return false;
    else if (!IsValidFictype(fictype)) return false;
    else {
        jfPageFetchInterface* old_fetcher = NULL;
        if (fetcher_map.contains(fictype)) {
            old_fetcher = fetcher_map.value(fictype,NULL);
        }
        fetcher_map.insert(fictype, fetcher);
        if (old_fetcher != NULL) delete old_fetcher;
        return true;
    }
}
// ----
bool jfFanficFetcherPack::HasFetcherFor(jf_FICTYPE fictype) const {
    return fetcher_map.contains(fictype);
}
// ----
jfPageFetchInterface* jfFanficFetcherPack::GetFetcher(jf_FICTYPE fictype) const {
    jfPageFetchInterface* map_fetcher = fetcher_map.value(fictype,NULL);
    if (map_fetcher != NULL) return map_fetcher;
    else return default_fetcher;
}
// ----
jfPageFetchInterface* jfFanficFetcherPack::GetDefaultFetcher() const {
    return default_fetcher;
}
//++++++++++++++++++++++++++++++++++++++++++
jfFanficFetcherPack::~jfFanficFetcherPack() {
    jfPageFetchInterface* fetcher = NULL;
    // delete mapped fetchers
    QList<jf_FICTYPE> fetcher_keys = fetcher_map.keys();
    for (int i = 0; i < fetcher_keys.size(); i++) {
        fetcher = fetcher_map.take(fetcher_keys.at(i));
        if (fetcher != NULL) delete fetcher;
    }
    fetcher_map.clear();
    delete default_fetcher;
}
// =================================================================
// estimating fictype
bool FindFicTypeFor(const QString& url, jf_FICTYPE& found_type) {
    QUrl fetchUrl = QUrl(url);
    if (fetchUrl.isValid() && (fetchUrl.isRelative() == false)) {
        QString host = fetchUrl.host().toLower();
        if ((host == "fanfiction.net") || host.endsWith(".fanfiction.net")) {
            found_type = jfft_FFN;
            return true;
        }
        else if ((host == "archiveofourown.org") || (host == "archiveofourown.org") || (host == "archiveofourown.org")) {
            found_type = jfft_AO3;
            return true;
        }
        else if (host == "www.fimfiction.net") {
            found_type = jfft_FIM;
            return true;
        }
        else if (host == "www.mediaminer.org") {
            found_type = jfft_MMO;
            return true;
        }
        else return false;
    }
    else return false;
}
// ========================================================================
jfPageFetchInterface* MakeFetcherInterface(jfParseFetchPackageMaker* parse_fetch_maker, jf_FICTYPE siteType) {
    jglobal::jfFetchBasics fetch_type = jglobal::settings.FindFetchTypeFor(siteType, jglobal::FPT_FICPART_PAGE);
    if (fetch_type.IsValid()) {
        return parse_fetch_maker->makeFetcherInterface(fetch_type);
    }
    else return NULL;
}
// ---------------------------------
jfFanficFetcherPack* MakeFetcherPack(const QStringList* user_agents) {
    jfFanficFetcherPack* result = new jfFanficFetcherPack(user_agents);
    jfParseFetchPackageMaker* parse_fetch_maker = jglobal::settings.getFetchPackageMaker();
    if (parse_fetch_maker == NULL) return result;
    else {
        jfPageFetchInterface* interface = NULL;
        interface = MakeFetcherInterface(parse_fetch_maker, jfft_FFN);
        if (interface != NULL) result->SetFetcherFor(jfft_FFN, interface);
        interface = MakeFetcherInterface(parse_fetch_maker, jfft_FIM);
        if (interface != NULL) result->SetFetcherFor(jfft_FIM, interface);
        interface = MakeFetcherInterface(parse_fetch_maker, jfft_AO3);
        if (interface != NULL) result->SetFetcherFor(jfft_AO3, interface);
        interface = MakeFetcherInterface(parse_fetch_maker, jfft_MMO);
        if (interface != NULL) result->SetFetcherFor(jfft_MMO, interface);

        return result;
    }

}
// =========================================================================

jfFanficUpdateFetcher::jfFanficUpdateFetcher(jfFanficFetcherPack* fetcher_pack_in):jfFetchWrapBase() {
    fetcher_pack = fetcher_pack_in;
}
//------------------
jfFanficUpdateFetcher::~jfFanficUpdateFetcher() {
    if (fetcher_pack != NULL) delete fetcher_pack;
}
//------------------
jfPageFetchInterface* jfFanficUpdateFetcher::FetcherForURL(const QString& inurl) const {
    jf_FICTYPE found_type;
    if (FindFicTypeFor(inurl, found_type)) {
        return fetcher_pack->GetFetcher(found_type);
    }
    else return fetcher_pack->GetDefaultFetcher();
}


/*****************************************************************************/
