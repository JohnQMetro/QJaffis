/*****************************************************************************
Name    : logging.h
Basic   : Separates out logging from the other stuff
Author  : John Q Metro
Started : January 13, 2011
Conversion to QT started : February 25, 2013
Updated : July 15, 2022 (logfolder)
Notes   :

******************************************************************************/
#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED
#endif // LOGGING_H_INCLUDED
//-----------------------------------------------
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QtGlobal>
#include <QByteArray>
#include <QMutex>

//*****************************************************************************
/* Unlike wxWidgets, Qt 4.x does not have built-in to file logging. There are a
  slew of third-part Qt logging classes available, but since I have a bunch of
  custom wrapper classes anyway, I think a simple custom built logger is best */
class jfQLogger {
  public:
    // constructors
    jfQLogger(QString logfolder);
    jfQLogger(QString logfolder, QString inname, bool opennow);
    bool open(QString inname);
    // info methods
    bool isOpen() const;
    size_t linesWritten() const;
    QString getFilename() const;
    // logging methods
    bool logLine(QString outstring);
    bool logError(QString outstring);
    // destructors
    bool close();
    ~jfQLogger();
  protected:
    bool makeFolder() const;

    QString subfolder;
    QString lfilename;
    QTextStream* xostream;
    QFile* ofile;
    size_t llinecount;
};
//===================================================================
// the primary log file.
namespace jlog {
  extern const QString logfoldername;
  extern const QString logfilename;
  extern jfQLogger* logtarget;
  extern QMutex locklog;
  extern bool dologging;

  // initializes the main log (logtarget)
  void InitLog();
  // closes and deletes the logger
  void CloseLog();
}

//================================================================
// using pre-processor macros to optionally remove most logging messages
#ifdef QT_DEBUG
  #define JDEBUGLOG(a,b) jfLogxMessage(a,b);
  #define JDEBUGLOGS(a,b,c) jfLogxMessage(a,b,c);
  #define JDEBUGLOGB(a,b,c) jfLogxMessageB(a,b,c);
  #define JDEBUGLOGST(a,b,c) jfLogxMessageS(a,b,c);
  #define JDEBUGLOGI(a,b,c) jfLogxMessageI(a,b,c);
  #define JDEBUGLOGU(a,b,c) jfLogxMessageU(a,b,c);
#else
  #define JDEBUGLOG(a,b)
  #define JDEBUGLOGS(a,b,c)
  #define JDEBUGLOGB(a,b,c)
  #define JDEBUGLOGST(a,b,c)
  #define JDEBUGLOGI(a,b,c)
  #define JDEBUGLOGU(a,b,c)
#endif

//======================================================================
// logging methods that include a function name and index
void jfLogxMessage(QString funcname,size_t index, QString info);
void jfLogxMessageS(QString funcname,size_t index, size_t data);
void jfLogxMessageI(QString funcname,size_t index, int data);
void jfLogxMessageU(QString funcname,size_t index, ulong data);
void jfLogxMessageB(QString funcname,size_t index, bool data);
void jfLogxMessage(QString funcname,size_t index);
void jfLogxMessage2S(QString funcname,size_t index, size_t data1, size_t data2);
void jfLogxMessageBytes(QString funcname,size_t index, const QByteArray* indata, size_t iindex, size_t length);
//-------------------------------------------------------------
// a core methods that handles building the first part of the above result
QString BLogCore(QString funcname, size_t index, bool more);
//----------------------------------------------------------------------------------------
// log just a string
void jfLogString(QString in_msg);
// conditional log
void jfCLogxMessage(bool cond, QString funcname, size_t index, QString info);
//============================================================
// assert with logging
// error log
namespace jerror {
  extern const QString errfilename;
  extern jfQLogger* errtarget;
  extern QMutex lockerr;

  // creates the error log object
  void InitELog();
  // closes and deletes the logger
  void CloseELog();

  void Log(QString funcname, QString errmsg);
  void ParseLog(QString funcname, QString errmsg);
  void AssertLog(bool condition, QString funcname, QString outmsg);
}

//*****************************************************************************
