/*****************************************************************************
Name    : utils2.h
Basic   : Declares some more utility functions and things used in the other files
Author  : John Q Metro
Started : May 13, 2009
Conversion to QT started : February 22, 2013
Updated : August 25, 2023
Notes   :

******************************************************************************/
#ifndef UTILS2_H_INCLUDED
#define UTILS2_H_INCLUDED
#endif // UTILS2_H_INCLUDED
//---------------------------------------------------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "utils1.h"
#endif // UTILS1_H_INCLUDED

#include <QDate>
#include <QTextStream>
#include <QFile>
#include <QStringList>
//*****************************************************************************
extern const QString IO_DELIMS;
//============================================================================
// a class used to help in parsing input
class jfLineParse {
  public:
    // constructors
    jfLineParse();
    jfLineParse(const QString& instr);
    // basic info
    size_t Num() const;
    bool IsEmpt() const;
    bool NNotX(const size_t& X) const;
    // getting values at index
    QString UnEscStr(size_t index) const;
    QString UnEscStrE(size_t index) const;
    QString Get(size_t index) const;
    QString GetT(size_t index) const;
    bool BoolVal(size_t index, bool& outval) const;
    bool SIntVal(size_t index, size_t& outval) const;
    bool SBoundVal(size_t index, const size_t& ubound, size_t& outval, QString& oerr) const;
    bool UIntVal(size_t index, quint32& outval) const;
    bool IIntVal(size_t index, int& outval) const;
    bool IBoundVal(size_t index, const int& lbound, const int& ubound, int& outval, QString& oerr) const;
    bool DateVal(size_t index, QDate& outval) const;
    bool CharVal(size_t index, QChar& outval, const QString& oallow="") const;
    // finding a value after a tag
    bool ReturnByTag(QString tag, QString& value) const;
    bool ReturnByTagS(QString tag, size_t& value) const;
    bool ReturnByTagB(QString tag, bool& value) const;
    // getting multiple values
    QStringList* GetMultiple(size_t index, size_t item_count) const;
    QStringList* UnEscMultiple(size_t index, size_t item_count) const;
    QStringList* GetRest(size_t index) const;
    QStringList* UnEscRest(size_t index) const;
    size_t UnEscAppendRest(size_t index, QStringList& destination) const;
    // changing the value
    size_t SetString(const QString& instr);
    // destructor
    ~jfLineParse();
  private:
    QStringList *qres;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* sort of the opposite of jfLineParse, mean for inserting values in the string.
overrides some of the operators already there... we use ';' as separators and escape
string input... */
class jfOutString : public QString {
  public:
    jfOutString();

    jfOutString& operator<< (const QString& toadd);
    jfOutString& operator<< (const QChar* toadd);
    jfOutString& operator<< (const bool& toadd);
    jfOutString& operator<< (const size_t toadd);
    jfOutString& operator<< (const int toadd);
    jfOutString& operator<< (const QDate& toadd);
    jfOutString& operator<< (const QStringList& toadd);

    void FullClear();
  protected:
    bool item_added;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// converts a QStringList to a delimted list, the items are all escaped first
QString StrArrToEscList(QStringList *source, const QString& delims);
// the reverse of the above, converts an escaped, delimited list into a QStringList
QStringList *EscListToStrArr(const QString& source, const QString& delims);

//****************************************************************************
class jfFileReader {
  public:
    jfLineParse lp;
    QString err;
    // constructor
    jfFileReader(QString infname);
    // file stuff
    bool OpenFile();
    bool CloseFile();
    size_t LineIndex() const;
    // file reading
    bool ReadLine(QString& outline, const QString& finname = "");
    bool ReadParseSLine(size_t fcount, const QString& finname = "");
    bool ReadParseLine(size_t fcount, const QString& finname = "");
    bool ReadParseLine(size_t fmin, size_t fmax, const QString& finname = "");

    // we use the internal 'oldline' without reading a new one. Useful if you need to try and parse a line more than twice
    bool ParseOldSLine(size_t fcount, const QString& finname = "");

    bool ReadLast(const QString& finname = "");
    bool GetType(QString& outtype, const QString& finname = "");
    bool ReadUnEsc(QString& outline, const QString& finname = "");
    bool ReadULong(ulong& outnum, const QString& finname = "");
    bool AppendToStringList(QStringList* outlines, int addcount = -1, const QString& finname = "");
    // other methods
    bool SkipLine(const QString& finname = "");
    bool Backup(const QString& finname = "");
    bool BuildError(QString inerr);
    bool AtEnd() const;
    bool IsOpen() const;
    bool IsOpenBeforeEnd() const;
    ~jfFileReader();
  protected:
    // not really reading
    // internal methods
    bool StartRead(QString& outline);
    bool SetParse(const QString& inval, size_t check);
    // internal data
    QTextStream* thefile;
    QFile* inpfile;
    QString fname;
    QString func_name;
    int lineindex;
    // because ->pos() is far too slow
    QString oldline;
    bool usebackup, backup_possible;

};
//****************************************************************************
