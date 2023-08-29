/*****************************************************************************
Name    : testswrapper.cpp
Basic   : An implementation of jfTestsDelegate that wraps a pointer to jfPageParserBase
Author  : John Q Metro
Updated : June 29, 2022

******************************************************************************/
#ifndef TESTSWRAPPER_H
    #include "testswrapper.h"
#endif // TESTSWRAPPER_H
//------------------------
#ifndef LOGGING_H_INCLUDED
    #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
/*****************************************************************************/
jfTestsParserWrapper::jfTestsParserWrapper(jfPageParserBase* parser_old) {
    const QString fname = "jfTestsParserWrapper::jfTestsParserWrapper";
    jerror::AssertLog(parser_old != NULL, fname, "Parser passed in constructor is NULL!");
    parser_link = parser_old;
    badpagecount = 0;
}
//++++++++++++++++++++++++++++++++++++
bool jfTestsParserWrapper::setPageIndex(const size_t& page_index) {
    return parser_link->setPageIndex(page_index);
}
QString jfTestsParserWrapper::GetCookie() const {
    return parser_link->getCookie();
}
bool jfTestsParserWrapper::TestIsBlocked(const QString* downpage) const {
    if (downpage == NULL) return true;
    return downpage->contains("<title>Attention Required! | Cloudflare</title>");
}
bool jfTestsParserWrapper::TestNotMissing(const QString* downpage) const {
    return (parser_link->testMissing(downpage));
}
bool jfTestsParserWrapper::TestNotTruncated(const QString* downpage) const {
    if (parser_link->testIncomplete(downpage)) {
        return true;
    }
    else if (downpage == NULL) return false;
    else {
        badpagecount++;
        QString fname = "truncated_page" + QString::number(badpagecount) + ".txt";
        ToFile(*downpage, fname);
        return false;
    }
}
// -------------------------------------------------------
bool jfTestsParserWrapper::TestNotRateLimited(const QString* downpage) const {
    return (parser_link->testRatelimit(downpage));
}
// +++++++++++++++++++++++++++++++++++++++++++++++++
bool jfTestsParserWrapper::ResolveRedirect(const QString& raw_redirect, QString& full_redirect) const {
    QString* red = parser_link->makeRedirectedURL(raw_redirect);
    if (red == NULL) return false;
    full_redirect = (*red);
    delete red;
    return true;
}
//-------------------------
jfTestsParserWrapper::~jfTestsParserWrapper() {
    parser_link = NULL;
}

/******************************************************************************/
