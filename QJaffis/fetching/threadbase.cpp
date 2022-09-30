/*****************************************************************************
Name    : threadbase.cpp
Basic   : Core thread class, with per thread logging and signals
Author  : John Q Metro
Started : October 17, 2015
Updated : May 22, 2021 (moved jfLoggBase to logbase.cpp)

******************************************************************************/
#ifndef THREADBASE_H
  #include "threadbase.h"
#endif // THREADBASE_H
//-----------------------------
#include <assert.h>
/*****************************************************************************/

//============================================================================
// [[ Methods for jfDownloadResults ]]
//+++++++++++++++++++++++++++++++++++++++++++
jfDownloadResults::jfDownloadResults() {
  resultData = NULL;
  wasRedirected = false;
  fetchResults.halt = false;
  fetchResults.why = jfa_DONE;
  pagecount = 0;
}

//-------------------------------
jfDownloadResults::jfDownloadResults(void* newResultData) {
  assert(newResultData != NULL);
  resultData = newResultData;
  wasRedirected = false;
  fetchResults.halt = false;
  fetchResults.why = jfa_DONE;
  pagecount = 0;
}

//-------------------------------
jfDownloadResults::jfDownloadResults(const QString& failRedirection) {
  resultData = NULL;
  wasRedirected = true;
  newURL = failRedirection;
  fetchResults.why = jfa_FETCHFAIL;
  fetchResults.halt = true;
  pagecount = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++
bool jfDownloadResults::isOkay() const {
  return (fetchResults.why == jfa_DONE) && (resultData != NULL);
}
//============================================================================
// the thread data passer
jfThreadDataPasser::jfThreadDataPasser() {
  URL_Storage = NULL;
  resultData = NULL;
  dataEmpty.release();
  resultDataEmpty.release();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// sending url to download-parser
void jfThreadDataPasser::SendUrl(QString *URL_toGet, size_t in_pageindex) {
  dataEmpty.acquire();
  URL_Storage = URL_toGet;
  pageindex = in_pageindex;
  dataSet.release();  // dataSet starts at 0, this makes it aquirable
}
//------------------------------------------------
// download-parser getting URL
const QString* jfThreadDataPasser::GetUrl(size_t &out_pageindex) {
  dataSet.acquire();  // dataSet will lock unless SendUrl is called first
  QString* temp = URL_Storage;
  URL_Storage = NULL;
  out_pageindex = pageindex;
  dataEmpty.release();
  return temp;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returning results from download-parser
//------------------------------------------------
void jfThreadDataPasser::ReturnResults(jfDownloadResults* inResults) {
  assert(inResults != NULL);
  resultDataEmpty.acquire();
  resultData = inResults;
  resultDataSet.release();
}

//------------------------------------------------
void jfThreadDataPasser::ResultResultData(void* returnData, size_t new_pagecount) {
  assert(returnData != NULL);
  resultDataEmpty.acquire();
  resultData = new jfDownloadResults(returnData);
  resultData->pagecount = new_pagecount;
  resultDataSet.release();
}

//------------------------------------------------
// used by the controller to get the results
jfDownloadResults* jfThreadDataPasser::GetResults() {
  resultDataSet.acquire();
  jfDownloadResults* temp = resultData;
  resultDataEmpty.release();
  return temp;
}
/*****************************************************************************/
