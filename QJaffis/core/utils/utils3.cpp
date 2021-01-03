/*****************************************************************************
Name    : utils3.h
Basic   : Declares some more utility functions and things used in the other files
Author  : John Q Metro
Started : January 7, 2011
Conversion to QT started : February 24, 2013
Updated : August 2, 2016 (adding file tree methods)
Notes   :

******************************************************************************/
#ifndef UTILS3_H_INCLUDED
  #include "utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>

#include <QTextStream>
#include <QFile>
#include <QRegExp>
#include <QByteArray>
#include <QUrl>
#include <QTextCodec>
#include <QDir>
#include <QFileInfo>

//****************************************************************************
const QString query_encode_keep = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_~";
//============================================================================
bool ToFile(QString indata, QString filename) {
  // variables
  QFile* outfile = NULL;
  QTextStream* outstream = NULL;
  // creating or opening the file
  outfile = new QFile(filename);
  if (!outfile->open(QIODevice::WriteOnly | QIODevice::Text)) {
    delete outfile;
    return false;
  }
  // creating the stream and writing
  outstream = new QTextStream(outfile);
  outstream->setCodec("UTF-8");
  outstream->setGenerateByteOrderMark(true);
  (*outstream) << indata;
  // closing
  outstream->flush();
  outfile->flush();
  outfile->close();
  // clearing
  delete outstream;
  delete outfile;
  return true;
}
//--------------------------------------------------------------------
// reads data from a file
bool FromFile(QString& outdata, QString filename) {
  // variables
  QFile* infile = NULL;
  QTextStream* instream = NULL;
  // opening the file
  infile = new QFile(filename);
  if (!infile->open(QIODevice::ReadOnly | QIODevice::Text)) {
    delete infile;
    return false;
  }
  // creating the stream and reading
  instream = new QTextStream(infile);
  instream->setCodec("UTF-8");
  outdata = instream->readAll();
  // done
  infile->close();
  delete instream;
  delete infile;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const QString UFNAME_DELIMS = " :;.,\'\\/\"?!@#$%^&";
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// produces a version of the string with some characters replaced by _
QString UBReplace(QString filename, QString delimchars) {
  // variables
  QRegExp dset("["+delimchars+"]");
  // using replace
  filename.replace(dset,"_");
  filename.replace("__","_");
	// done
	return filename;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o creator
QTextStream* GetOpenFile(QString filename, bool write, const char* charset) {
  // variables
  QFile* thefile;
  QTextStream* result;

  // starting
  thefile = new QFile(filename);
  // opening write version (no append here)
  if (write) {
    if (!thefile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
      delete thefile;
      return NULL;
    }
  }
  // opening read version
  else {
    if (!thefile->open(QIODevice::ReadOnly)) {
      delete thefile;
      return NULL;
    }
  }
  result = new QTextStream();
  result->setDevice(thefile);
  result->setCodec(charset);
  result->setAutoDetectUnicode(false);
  if (write) result->setGenerateByteOrderMark(true);
  // done
  return result;
}
//-----------------------------------------------
bool CloseFileStream(QTextStream* ofile, bool output) {
  // variables
  QFile* thefile;
  // checking
  if (ofile==NULL) return false;
  // doing
  if (output) ofile->flush();
  thefile = dynamic_cast<QFile*>(ofile->device());
  ofile->setDevice(0);
  thefile->close();
  delete thefile;
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// more file i/o
//-------------------------------------------------
bool AppendAS2File(QStringList* indata, QTextStream* target) {
  // the usual checks
  assert(indata!=NULL);
  assert(target!=NULL);
  // variables
  size_t lcount, lindex;
  // appending
  lcount = indata->count();
  for(lindex = 0; lindex<lcount; lindex++) {
    (*target)<<(indata->at(lindex));
    (*target)<< QChar('\n');
  }
  target->flush();
  return true;
}
//-------------------------------------------------
bool AppendFile2AS(QStringList* target, QTextStream* source) {
  // the usual checks
  assert(target!=NULL);
  assert(source!=NULL);
  // varibales
  size_t lread = 0;
  QString xinline;
  // the loop
  while(!source->atEnd()) {
    xinline = source->readLine();
    lread++;
    target->append(xinline);
  }
  return (lread!=0);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// QStringList To File and vice versa
//------------------------------------------------
bool ToFileAS(QStringList* indata, QString filename) {
  // variables
  QTextStream* result;
  bool rval;
  // special case
  if (indata==NULL) return false;
  // creating or opening the file
  result = GetOpenFile(filename,true);
  // doing
  rval = AppendAS2File(indata,result);
  assert(rval);
  // closing and writing
  CloseFileStream(result,true);
  delete result;
  return true;
}
//---------------------------------------------------
QStringList* FromFileAS(QString filename) {
  // variables
  QTextStream* infile;
  QStringList* result;
  bool rval;
  // we start
  infile = GetOpenFile(filename,false);
  if (infile==NULL) return NULL;
  // the file exists
  result = new QStringList();
  // doing
  rval = AppendFile2AS(result,infile);
  assert(rval);
  // finishing
  CloseFileStream(infile,false);
  delete infile;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Query encoding a string for URL query usage
QString QueryEncode(const QString invalue, bool encodetilde) {
  // special case
  if (invalue.isEmpty()) return "";
  // variables
  QByteArray ba;
  QString result;
  // converting
  if (encodetilde) ba = QUrl::toPercentEncoding(invalue,QByteArray(),"~");
  else ba = QUrl::toPercentEncoding(invalue);
  result = QString::fromAscii(ba.data());
  return result;
}
//=================================================================
/* String tokenizer, ignoring delimiters. If incmatch is true, '?' and '*' are not treated
 as delimiters.*/
QStringList* TokenizeString(QString source, bool incmatch) {
  QStringList* result;
  // we use regular expressions to split
  QRegExp split_match = QRegExp("[^*\?\\w]");
  QRegExp split_delims = QRegExp("\\W");
  // preliminaries
  result = new QStringList();
  if (incmatch) {
    source.replace("*?","?*");
    source.replace("**","*");
  }
  // going ahead
  if (incmatch) (*result) = source.split(split_match,QString::SkipEmptyParts);
  else (*result) = source.split(split_delims,QString::SkipEmptyParts);
  // done
  return result;
}
//=====================================================================
// deletes a directory and it's contents
bool DeleteDirectoryTree(QString dir_to_delete) {
  // variables
  QDir delete_dir(dir_to_delete);
  QDir dummy_dir; // why is this even needed
  QFileInfo currf;
  bool result, perfect;
  // a quick check
  if (!delete_dir.exists()) return true;
  // starting
  delete_dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot|QDir::Hidden|QDir::System);
  QFileInfoList fl = delete_dir.entryInfoList();
  perfect = true;
  // the deletion loop
  while(!fl.isEmpty()) {
    currf = fl.takeFirst();
    if (currf.isDir()) result = DeleteDirectoryTree(currf.absoluteFilePath());
    else result = QFile::remove(currf.absoluteFilePath());
    if (!result) perfect = false;
  }
  if (!perfect) return false;
  return dummy_dir.rmdir(dir_to_delete);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// gets a list of all files and subdirectories
bool GetDirectoryTreeList(const QString indir, bool native, QStringList& outdata) {
  // variables
  QDir report_dir(indir);
  QFileInfo curf;
  QString result;
  // special case
  if (!report_dir.exists()) return false;
  report_dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);
  report_dir.setSorting(QDir::Name);
  QFileInfoList dlist = report_dir.entryInfoList();
  for (size_t dindex = 0 ; dindex < dlist.size() ; dindex++) {
    curf = dlist.at(dindex);
    result = curf.canonicalFilePath();
    if (curf.isDir()) result += "/";
    if (native) outdata.append(QDir::toNativeSeparators(result));
    else outdata.append(result);
    if (curf.isDir()) GetDirectoryTreeList(result,native,outdata);
  }
  return true;
}
//****************************************************************************
