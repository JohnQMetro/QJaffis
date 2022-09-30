/*****************************************************************************
Name    : logging.cpp
Basic   : Separates out logging from the other stuff
Author  : John Q Metro
Started : January 13, 2011
Conversion to QT started : February 25, 2013
Updated : July 16, 2022
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
#include <QDir>

#include <assert.h>

//*****************************************************************************
/* Unlike wxWidgets, Qt 4.x does not have built-in to file logging. There are a
  slew of third-part Qt logging classes available, but since I have a bunch of
  custom wrapper classes anyway, I think a simple custom built logger is best */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//---------------------------
jfQLogger::jfQLogger(QString logfolder) {
  xostream = NULL;
  ofile = NULL;
  llinecount = 0;
  subfolder = logfolder;
}
//---------------------------
jfQLogger::jfQLogger(QString logfolder, QString inname, bool opennow) {
  xostream = NULL;
  ofile = NULL;
  llinecount = 0;
  subfolder = logfolder;
  if (opennow) open(inname);
  else lfilename = inname;
}

//---------------------------
bool jfQLogger::open(QString inname) {
  // basic starting steps
  if (inname.isEmpty()) return false;
  if (isOpen()) close();
  // folder
  if (subfolder.isEmpty()) {
      lfilename = inname;
  }
  else {
      makeFolder();
      lfilename = subfolder + "/" + inname;
  }
  //
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
//-----------------------------------
/* differs from logLine in that there is no datetime, and a separate
 * open is not needed */
bool jfQLogger::logError(QString outstring) {
    if (!isOpen()) {
        if (!open(lfilename)) return false;
    }
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfQLogger::makeFolder() const {
    QDir dir = QDir();
    if (dir.exists(subfolder)) return true;
    else return dir.mkdir(subfolder);
}

//===================================================================
// one logfile and some associated variables
const QString jlog::logfoldername = "logs";
const QString jlog::logfilename = "trace0.txt";
jfQLogger* jlog::logtarget = NULL;
QMutex jlog::locklog;
bool jlog::dologging = true;

// initializes the main log (logtarget)
void jlog::InitLog() {
  QMutexLocker lk(&locklog);
  if (logtarget!=NULL) delete logtarget;
  if (dologging) logtarget = new jfQLogger(logfoldername,logfilename, true);
  else logtarget = new jfQLogger(logfoldername);
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
void jfLogxMessage(QString funcname,size_t index, QString info) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
	res_msg += info;
  jlog::logtarget->logLine(res_msg);
}
//---------------------------------------------------------------------------
void jfLogxMessageS(QString funcname,size_t index, size_t data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data);
  jlog::logtarget->logLine(res_msg);
}
//-----------------------------------------------------------------------
void jfLogxMessageI(QString funcname,size_t index, int data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data);
  jlog::logtarget->logLine(res_msg);
}
//-----------------------------------------------------------------------
void jfLogxMessageU(QString funcname,size_t index, ulong data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data);
  jlog::logtarget->logLine(res_msg);
}
//--------------------------------------------------------------------
void jfLogxMessageB(QString funcname,size_t index, bool data) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += Bool2Str(data);
  jlog::logtarget->logLine(res_msg);
}
//--------------------------------------------------------------------
void jfLogxMessage(QString funcname,size_t index) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,false);
  jlog::logtarget->logLine(res_msg);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfLogxMessage2S(QString funcname,size_t index, size_t data1, size_t data2) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  QString res_msg = BLogCore(funcname,index,true);
  res_msg += QString::number(data1) + " : ";
  res_msg += QString::number(data2);
  jlog::logtarget->logLine(res_msg);
}
//-----------------------------------------------------------------------------
void jfLogxMessageBytes(QString funcname, size_t index, const QByteArray* indata, size_t iindex, size_t length) {
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
void jfLogString(QString in_msg) {
  QMutexLocker lk(&jlog::locklog);
  jlog::logtarget->logLine(in_msg);
}
//---------------------------
// conditional log
void jfCLogxMessage(bool cond, QString funcname, size_t index, QString info) {
  assert(!funcname.isEmpty());
  QMutexLocker lk(&jlog::locklog);
  if (cond) {
    QString res_msg = BLogCore(funcname,index,true);
    res_msg += info;
    jlog::logtarget->logLine(res_msg);
  }
}
//==============================================================
// error log
const QString jerror::errfilename = "errors.txt";
jfQLogger* jerror::errtarget = NULL;
QMutex jerror::lockerr;
//---------------------------------------
// creates the error log object
void jerror::InitELog() {
    QMutexLocker lk(&lockerr);
    if (errtarget != NULL) delete errtarget;
    errtarget = new jfQLogger(jlog::logfoldername,errfilename, false);
}
// closes and deletes the logger
void jerror::CloseELog() {
    QMutexLocker lk(&lockerr);
    if (errtarget != NULL) {
        delete errtarget;
        errtarget = NULL;
    }
}
//--------------------------------------
void jerror::Log(QString funcname, QString errmsg) {
    QMutexLocker lk(&lockerr);
    errtarget->logError(funcname + "| " + errmsg);
}
//--------------------------------------
void jerror::ParseLog(QString funcname, QString errmsg) {
    QMutexLocker lk(&lockerr);
    errtarget->logError(funcname + "| " + "Parse: " + errmsg);
}
//--------------------------------------
void jerror::AssertLog(bool condition, QString funcname, QString outmsg) {
    QMutexLocker lk(&lockerr);
    if (!condition) {
        errtarget->logError(funcname + "| " + outmsg);
        qFatal("%s",outmsg.toLocal8Bit().constData());
    }
}
//*****************************************************************************
