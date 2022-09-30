//*******************************************************************
// Name    : threadbase.h
// Basic   : Core thread class, with per thread logging and signals
// Author  : John Q Metro
// Started : Split from threadbase.h in May 2021
// Updated : June 6, 2021
//*******************************************************************
#ifndef LOGBASE_H
#define LOGBASE_H
#endif // LOGBASE_H
//--------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <QObject>
#include <QString>

//*******************************************************************
// base class with log methods
class jfLoggBase : public QObject {
    Q_OBJECT
  public:
    jfLoggBase();
    virtual ~jfLoggBase();
  // log methods
    void tLog(const QString &funcname, size_t index) const;
    void tLog(const QString &funcname, size_t index, const QString& msg) const;
    void tLogB(const QString &funcname, size_t index, bool value) const;
    void tLog2B(const QString &funcname, size_t index, bool value1, bool value2) const;
    void tLogS(const QString &funcname, size_t index, size_t value) const;
    void tLog2S(const QString &funcname, size_t index, size_t value1, size_t value2) const;
    void tLogI(const QString &funcname, size_t index, int value) const;
    void tLog2I(const QString &funcname, size_t index, int value1, int value2) const;
    void tLogL(const QString &funcname, size_t index, size_t loop_index) const;
  // error methods
    void tError(const QString &funcname, const QString& err_msg) const;
    void tParseError(const QString &funcname, const QString& err_msg) const;
    void tAssert(bool condition, const QString &funcname, const QString& err_msg) const;
  protected:
    // core per thread logging
    // methods
    bool initLog(const QString& tname);
    QString buildLog(const QString &funcname, const size_t& inindex) const;
    void CloseLog();

    // data
    static int tcount;
    mutable QMutex tcr;
    mutable jfQLogger* thislog;

};
//*******************************************************************
