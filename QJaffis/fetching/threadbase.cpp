/*****************************************************************************
Name    : threadbase.cpp
Basic   : Core thread class, with per thread logging and signals
Author  : John Q Metro
Started : October 17, 2015
Updated : August 8, 2016

******************************************************************************/
#ifndef THREADBASE_H
  #include "threadbase.h"
#endif // THREADBASE_H
//-----------------------------
#include <assert.h>
/*****************************************************************************/
int jfLoggBase::tcount = 0;  // static initialization
//------------------------
jfLoggBase::jfLoggBase():QObject() {
  thislog = NULL;
}
//------------------------
jfLoggBase::~jfLoggBase() {
  tcr.lock();
  if (thislog!=NULL) {
    thislog->close();
    delete thislog;
    tcount--;
  }
  tcr.unlock();
}
//++++++++++++++++++++++++++++
// Logging Methods
//---------------------------
// if logging is set, creates a new open log file with logger
bool jfLoggBase::initLog(const QString& tname) {
  // special faliure conditions
  if (tname.isEmpty()) return false;
  if (!(jlog::dologging)) return false;
  // starting
  tcr.lock();
  // building the log file name
  QString rfname;
  ++tcount;
  if (tcount<10) rfname = "t0";
  else rfname = "t";
  rfname += QString::number(tcount) + " " + tname + ".txt";
  // opening the log file
  if (thislog!=NULL) {
    thislog->close();
    delete thislog;
  }
  thislog = new jfQLogger(rfname);
  // done
  tcr.unlock();
  return true;
}
//---------------------------
QString jfLoggBase::buildLog(const QString &funcname, const size_t& inindex) const {
  QString rstring = " In " + funcname + " at ";
  if (inindex<10) rstring += " ";
  rstring += QString::number(inindex);
  return rstring;
}
//---------------------------
void jfLoggBase::CloseLog() {
  tcr.lock();
  if (thislog!=NULL) {
    thislog->close();
    delete thislog;
    thislog = NULL;
    tcount--;
  }
  tcr.unlock();
}
//++++++++++++++++++++++++++++++++++++++++++

// logs an index
void jfLoggBase::tLog(const QString &funcname, size_t index) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  thislog->logLine(rstring);
}
//---------------------------
// logs an index and a string message
void jfLoggBase::tLog(const QString &funcname, size_t index, const QString& msg) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : " + msg;
  thislog->logLine(rstring);
}
//---------------------------
// logs an index and a boolean value
void jfLoggBase::tLogB(const QString &funcname, size_t index, bool value) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : " + QString((value)?("true"):("false"));
  thislog->logLine(rstring);
}
//--------------------------------------
void jfLoggBase::tLog2B(const QString &funcname, size_t index, bool value1, bool value2) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : " + QString((value1)?("true"):("false"));
  rstring += " : " + QString((value2)?("true"):("false"));
  thislog->logLine(rstring);
}
//----------------------------
// logs an index and an unsigned integer value
void jfLoggBase::tLogS(const QString &funcname, size_t index, size_t value) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : " + QString::number(value);
  thislog->logLine(rstring);
}
//----------------------------
void jfLoggBase::tLog2S(const QString &funcname, size_t index, size_t value1, size_t value2) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : " + QString::number(value1)  + " : " + QString::number(value2);
  thislog->logLine(rstring);
}
//----------------------------
void jfLoggBase::tLogI(const QString &funcname, size_t index, int value) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : " + QString::number(value);
  thislog->logLine(rstring);
}
//----------------------------
void jfLoggBase::tLog2I(const QString &funcname, size_t index, int value1, int value2) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : " + QString::number(value1)  + " : " + QString::number(value2);
  thislog->logLine(rstring);
}
//----------------------------
void jfLoggBase::tLogL(const QString &funcname, size_t index, size_t loop_index) const {
  if (thislog==NULL) return;
  QString rstring = buildLog(funcname,index);
  rstring += " : Loop Index " + QString::number(loop_index);
  thislog->logLine(rstring);
}
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
