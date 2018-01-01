/*****************************************************************************
Name    : utils3.h
Basic   : Defines some more utility functions and things used in the other files
Author  : John Q Metro
Started : January 7, 2011
Conversion to QT started : February 24, 2013
Updated : August 2, 2016 (added DirectoryTree methods)
Notes   :

******************************************************************************/
#define UTILS3_H_INCLUDED

#include <QString>
#include <QtGlobal>
#include <QStringList>
#include <QTextStream>
//****************************************************************************
// writes data to a file, not append, overwrite
bool ToFile(QString indata, QString filename);
// reads data from a file
bool FromFile(QString& outdata, QString filename);
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// produces a version of the string with some characters replaced by _
extern const QString UFNAME_DELIMS;
QString UBReplace(QString filename, QString delimchars);
// file i/o creator
QTextStream* GetOpenFile(QString filename, bool write, const char* charset = "UTF-8");
bool CloseFileStream(QTextStream* ofile, bool output);
// more file i/o
bool AppendAS2File(QStringList* indata, QTextStream* target);
bool AppendFile2AS(QStringList* target, QTextStream* source);
// QStringList To File and vice versa
bool ToFileAS(QStringList* indata, QString filename);
QStringList* FromFileAS(QString filename);
/* Query encoding a string for URL query usage. I know about QUrl::toPercentEncoding, but
that method returns a QByteArray instead of a QString */
QString QueryEncode(const QString invalue, bool encodetilde=false);
/* String tokenizer, ignoring delimiters. If incmatch is true, '?' and '*' are not treated
 as delimiters.*/
QStringList* TokenizeString(QString source, bool incmatch);

// deletes a directory and it's contents. CANNOT HANDLE SYMLINKS
bool DeleteDirectoryTree(QString dir_to_delete);

// gets a list of all files and subdirectories (NO SYMLINKS)
bool GetDirectoryTreeList(const QString indir, bool native, QStringList& outdata);

//*****************************************************************************
