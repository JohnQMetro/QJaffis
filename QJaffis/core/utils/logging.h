/*****************************************************************************
Name    : logging.h
Basic   : Separates out logging from the other stuff
Author  : John Q Metro
Started : January 13, 2011
Conversion to QT started : February 25, 2013
Updated : July 19, 2016 (renamed again, added Macros)
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
    jfQLogger();
    jfQLogger(QString inname);
    bool open(QString inname);
    // info methods
    bool isOpen() const;
    size_t linesWritten() const;
    QString getFilename() const;
    // logging methods
    bool logLine(QString outstring);
    // destructors
    bool close();
    ~jfQLogger();
  protected:
    QString lfilename;
    QTextStream* xostream;
    QFile* ofile;
    size_t llinecount;
};
//===================================================================
// the primary log file.
namespace jlog {
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
  #define JDEBUGLOG(a,b) jfLogMessage(a,b);
  #define JDEBUGLOGS(a,b,c) jfLogMessage(a,b,c);
  #define JDEBUGLOGB(a,b,c) jfLogMessageB(a,b,c);
  #define JDEBUGLOGST(a,b,c) jfLogMessageS(a,b,c);
  #define JDEBUGLOGI(a,b,c) jfLogMessageI(a,b,c);
  #define JDEBUGLOGU(a,b,c) jfLogMessageU(a,b,c);
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
void jfLogMessage(QString funcname,size_t index, QString info);
void jfLogMessageS(QString funcname,size_t index, size_t data);
void jfLogMessageI(QString funcname,size_t index, int data);
void jfLogMessageU(QString funcname,size_t index, ulong data);
void jfLogMessageB(QString funcname,size_t index, bool data);
void jfLogMessage(QString funcname,size_t index);
void jfLogMessage2S(QString funcname,size_t index, size_t data1, size_t data2);
void jfLogMessageBytes(QString funcname,size_t index, const QByteArray* indata, size_t iindex, size_t length);
//-------------------------------------------------------------
// a core methods that handles building the first part of the above result
QString BLogCore(QString funcname, size_t index, bool more);
//----------------------------------------------------------------------------------------
// log just a string
void jfXLogString(QString in_msg);
// conditional log
void jfCLogMessage(bool cond, QString funcname, size_t index, QString info);
//============================================================
// assert with logging
void jfAssertLog(bool condition, QString funcname, QString outmsg);
//*****************************************************************************
