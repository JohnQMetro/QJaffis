/*****************************************************************************
Name    : logbase.cpp
Basic   : Logging-related thread base class
Author  : John Q Metro
Started : Split from threadbase.cpp May 22, 2021
Updated : June 10, 2021 (split into release vs debug)

******************************************************************************/
#ifndef LOGBASE_H
    #include "logbase.h"
#endif // LOGBASE_H
//--------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
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
#ifdef QT_DEBUG
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
  thislog = new jfQLogger(jlog::logfoldername,rfname, true);
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// error methods
void jfLoggBase::tError(const QString &funcname, const QString& err_msg) const {
    tLog(funcname, 0, err_msg);
}
void jfLoggBase::tParseError(const QString &funcname, const QString& err_msg) const {
    tLog(funcname, 0, err_msg);
}
void jfLoggBase::tAssert(bool condition, const QString &funcname, const QString& err_msg) const {
    if (!condition) {
      tLog(funcname, 0, err_msg);
      qFatal("%s",err_msg.toLocal8Bit().constData());
    }
}
#else
// Logging Methods. The release implementation gets rid of most logging
//---------------------------
// if logging is set, creates a new open log file with logger
bool jfLoggBase::initLog(const QString& tname) {
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
}
//---------------------------
// logs an index and a string message
void jfLoggBase::tLog(const QString &funcname, size_t index, const QString& msg) const {
}
//---------------------------
// logs an index and a boolean value
void jfLoggBase::tLogB(const QString &funcname, size_t index, bool value) const {
}
//--------------------------------------
void jfLoggBase::tLog2B(const QString &funcname, size_t index, bool value1, bool value2) const {
}
//----------------------------
// logs an index and an unsigned integer value
void jfLoggBase::tLogS(const QString &funcname, size_t index, size_t value) const {
}
//----------------------------
void jfLoggBase::tLog2S(const QString &funcname, size_t index, size_t value1, size_t value2) const {
}
//----------------------------
void jfLoggBase::tLogI(const QString &funcname, size_t index, int value) const {
}
//----------------------------
void jfLoggBase::tLog2I(const QString &funcname, size_t index, int value1, int value2) const {
}
//----------------------------
void jfLoggBase::tLogL(const QString &funcname, size_t index, size_t loop_index) const {
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// error methods
void jfLoggBase::tError(const QString &funcname, const QString& err_msg) const {
    jerror::Log(funcname, err_msg);
}
void jfLoggBase::tParseError(const QString &funcname, const QString& err_msg) const {
    jerror::ParseLog(funcname, err_msg);
}
void jfLoggBase::tAssert(bool condition, const QString &funcname, const QString& err_msg) const {
    jerror::AssertLog(condition, funcname, err_msg);
}
#endif
