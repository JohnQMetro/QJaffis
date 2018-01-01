/*****************************************************************************
Name    : logging.cpp
Basic   : Separates out logging from the other stuff
Author  : John Q Metro
Started : January 13, 2011
Conversion to QT started : February 25, 2013
Updated : March 9, 2013
Notes   :

******************************************************************************/
#ifndef LOGGING_H_INCLUDED
  #include "logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------------
#ifndef UTILS3_H_INCLUDED
  #include "utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "utils1.h"
#endif // UTILS1_H_INCLUDED

#include <QDateTime>

#include <assert.h>

//*****************************************************************************
/* Unlike wxWidgets, Qt 4.x does not have built-in to file logging. There are a
  slew of third-part Qt logging classes available, but since I have a bunch of
  custom wrapper classes anyway, I think a simple custom built logger is best */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//---------------------------
jfQLogger::jfQLogger() {
  xostream = NULL;
  ofile = NULL;
  llinecount = 0;
}
//---------------------------
jfQLogger::jfQLogger(QString inname) {
  xostream = NULL;
  ofile = NULL;
  llinecount = 0;
  open(inname);
}

//---------------------------
bool jfQLogger::open(QString inname) {
  // basic starting steps
  if (inname.isEmpty()) return false;
  if (isOpen()) close();
  lfilename = inname;
  // opening
  ofile = new QFile(lfilename);
  if (!ofile->open(QIODevice::WriteOnly | QIODevice::Text| QIODevice::Truncate)) {
    delete ofile;
    ofile = NULL;
    return false;
  }
  // assigning a stream
  xostream = new QTextStream(ofile);
  xostream->setCodec("UTF-8");
  xostream->setGenerateByteOrderMark(true);
  llinecount = 0;
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info methods
//---------------------------
bool jfQLogger::isOpen() const {
  return (ofile!=NULL);
}
//---------------------------
size_t jfQLogger::linesWritten() const {
  return llinecount;
}
//---------------------------
QString jfQLogger::getFilename() const {
  return lfilename;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// logging methods
bool jfQLogger::logLine(QString outstring) {
  if (!isOpen()) return false;
  (*xostream) << QDateTime::currentDateTime().toString(Qt::ISODate) << ' ';
  (*xostream) << outstring << '\n';
  xostream->flush();
  ++llinecount;
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructors
//---------------------------
bool jfQLogger::close() {
  if (!isOpen()) return false;
  // closing
  xostream->flush();
  xostream->setDevice(0);
  ofile->close();
  // clearing
  delete ofile;
  ofile = NULL;
  delete xostream;
  xostream = NULL;
  // done
  return true;
}

//---------------------------
jfQLogger::~jfQLogger() {
  close();
}

//===================================================================
// one logfile and some associated variables
const QString jlog::logfilename = "trace0.txt";
jfQLogger* jlog::logtarget = NULL;
QMutex jlog::locklog;
bool jlog::dologging = true;

// initializes the main log (logtarget)
void jlog::InitLog() {
  QMutexLocker lk(&locklog);
  if (logtarget!=NULL) delete logtarget;
  if (dologging) logtarget = new jfQLogger(logfilename);
  else logtarget = new jfQLogger();
}

// closes and deletes the logger
void jlog::CloseLog() {
  QMutexLocker lk(&locklog);
  if (logtarget!=NULL) {
    delete logtarget;
    logtarget = NULL;
  }
}

//========================================================
void jfLogMessage(QString funcname,size_t index, QString info) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
	res_msg += info;
  jlog::logtarget->logLine(res_msg);
}
//---------------------------------------------------------------------------
void jfLogMessageS(QString funcname,size_t index, size_t data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data);
  jlog::logtarget->logLine(res_msg);
}
//-----------------------------------------------------------------------
void jfLogMessageI(QString funcname,size_t index, int data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data);
  jlog::logtarget->logLine(res_msg);
}
//-----------------------------------------------------------------------
void jfLogMessageU(QString funcname,size_t index, ulong data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data);
  jlog::logtarget->logLine(res_msg);
}
//--------------------------------------------------------------------
void jfLogMessageB(QString funcname,size_t index, bool data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += Bool2Str(data);
  jlog::logtarget->logLine(res_msg);
}
//--------------------------------------------------------------------
void jfLogMessage(QString funcname,size_t index) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,false);
  jlog::logtarget->logLine(res_msg);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfLogMessage2S(QString funcname,size_t index, size_t data1, size_t data2) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data1) + " : ";
  res_msg += QString::number(data2);
  jlog::logtarget->logLine(res_msg);
}
//-----------------------------------------------------------------------------
void jfLogMessageBytes(QString funcname, size_t index, const QByteArray* indata, size_t iindex, size_t length) {
  assert(!funcname.isEmpty());
  assert(indata!=NULL);
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
	for (size_t offset=0;offset<length;offset++) {
    res_msg += QString::number((uint)(indata->at(iindex+offset))) + " ";
	}
  jlog::logtarget->logLine(res_msg);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString BLogCore(QString funcname, size_t index, bool more) {
  // check
  assert(funcname!=NULL);
  // make the result
  QString res_msg = QString(funcname) + " ";
  QString buffer = QString::number(index);
  if (index<10) buffer = "0" + buffer;
	res_msg+= buffer;
  if (more) res_msg += " : ";
	// done
	return res_msg;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// log just a string
void jfXLogString(QString in_msg) {
  QMutexLocker lk(&jlog::locklog);
  jlog::logtarget->logLine(in_msg);
}
//---------------------------
// conditional log
void jfCLogMessage(bool cond, QString funcname, size_t index, QString info) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  if (cond) {
    QString res_msg = BLogCore(funcname,index,true);
    res_msg += info;
    jlog::logtarget->logLine(res_msg);
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// assert with logging
void jfAssertLog(bool condition, QString funcname, QString outmsg) {
  QString result;
  if (!condition) {
    jlog::locklog.lock();
    result = "Assertion failed in ";
    result += funcname;
    result += " : ";
    result += outmsg;
    jlog::logtarget->logLine(result);
    jlog::locklog.unlock();
    assert(false);
  }
}
//*****************************************************************************
