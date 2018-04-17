/*****************************************************************************
Name    : testurl.h
Basic   : URLs and any misc stuff for the test routines
Author  : John Q Metro
Started : March 18, 2018
Updated : March 25, 2018
******************************************************************************/
#ifndef TESTURL_H
    #define TESTURL_H
#endif // TESTURL_H
//-----------------------------
#ifndef BASEPARSE_H
    #include "../fetching/baseparse.h"
#endif // BASEPARSE_H
//-----------------------------
#include <QString>
#include <vector>
/*****************************************************************************/
typedef std::vector<jfPageParserBase*> ParserList;

namespace test {
    extern const size_t count;
    extern const QString labels[];
    extern const QString urls[];
    extern const size_t pageindexes[];

    ParserList* makeParsers();
    bool FreeParsers(ParserList* plist);
    bool DeleteResults(size_t index, void* resultdata);

}
/*****************************************************************************/
