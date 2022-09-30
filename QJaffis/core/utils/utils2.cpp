/*****************************************************************************
Name    : utils2.cpp
Basic   : Defines some more utility functions and things used in the other files
Author  : John Q Metro
Started : May 13, 2009
Conversion to QT started : February 23, 2013
Updated : August 20, 2022
Notes   :

******************************************************************************/
#ifndef UTILS2_H_INCLUDED
  #include "utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "utils3.h"
#endif // UTILS3_H_INCLUDED

#include <assert.h>
#include <iostream>

//*****************************************************************************
const QString IO_DELIMS=";#{}";
//=============================================================================
// constructors
//---------------------------------------------------------------
jfLineParse::jfLineParse() {
  qres = NULL;
}
//---------------------------------------------------------------
jfLineParse::jfLineParse(const QString& instr) {
  qres = NULL;
  SetString(instr);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic info
//---------------------------------------------------------------
size_t jfLineParse::Num() const {
  if (qres==NULL) return 0;
  else return qres->count();
}
//---------------------------------------------------------------
bool jfLineParse::IsEmpt() const {
  return (qres==NULL);
}
//---------------------------------------------------------------
bool jfLineParse::NNotX(const size_t& X) const {
  return (Num()!=X);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting values at index
QString jfLineParse::UnEscStr(size_t index) const {
  QString buffer;
  // starting
  assert(index<Num());
  buffer = qres->at(index);
  UnEscapeNL(buffer,IO_DELIMS);
  return buffer;
}
//---------------------------------------------------------------
QString jfLineParse::UnEscStrE(size_t index) const {
  QString buffer;
  // starting
  assert(index<Num());
  buffer = qres->at(index);
  UnEscapeNL(buffer,EXPR_DELIMS);
  return buffer.trimmed();
}
//---------------------------------------------------------------
QString jfLineParse::Get(size_t index) const {
  assert(index<Num());
  return qres->at(index);
}
//------------------------------------------------------
QString jfLineParse::GetT(size_t index) const {
  QString rval;
  rval = Get(index).trimmed();
  return rval;
}
//---------------------------------------------------------------
// we try and get the boolean value at the index
bool jfLineParse::BoolVal(size_t index, bool& outval) const {
  bool rval;
  //starting...
  assert(index<Num());
  rval = BoolFromStr(qres->at(index),outval);
  return rval;
}
//---------------------------------------------------------------
// tries to get the integer value at the index
bool jfLineParse::SIntVal(size_t index, size_t& outval) const {
  ulong tval;
  bool res;
  QString buffer;
  // starting
  assert(index<Num());
  buffer = qres->at(index);
  res = Str2ULong(buffer,tval);
  if (res) outval = tval;
  return res;
}
//---------------------------------------------------------------
// similar to SIntVal, but also returns an error if the value >= ubound
bool jfLineParse::SBoundVal(size_t index, const size_t& ubound, size_t& outval, QString& oerr) const {
  unsigned long oval;
  QString buffer;
  // starting
  assert(index<Num());
  buffer = (*qres)[index];
  if (!Str2ULong(buffer,oval)) {
    oerr = "is not a number!";
    return false;
  }
  else if (oval>=ubound) {
    oerr = "is too high!";
    return false;
  }
  outval = oval;
  return true;
}
//------------------------------------------------------------
bool jfLineParse::UIntVal(size_t index, quint32& outval) const {
  ulong tval;
  bool res;
  QString buffer;
  // starting
  assert(index<Num());
  buffer = qres->at(index);
  res = Str2ULong(buffer,tval);
  if (res) outval = tval;
  return res;
}
//------------------------------------------------------------
bool jfLineParse::IIntVal(size_t index, int& outval) const {
  int tval;
  bool* res;
  bool res2;
  QString buffer;
	// starting
	assert(index<Num());
  buffer = qres->at(index);
  res = new bool;
  tval = buffer.toInt(res);
  if (*res) outval = tval;
  res2 = *res;
  delete res;
  return res2;
}
// ----------------------------------------------------------
bool jfLineParse::IBoundVal(size_t index, const int& lbound, const int& ubound, int& outval, QString& oerr) const {
    int converted;
    if (IIntVal(index, converted)) {
        if (converted < lbound) {
            oerr = "is too small!";
            return false;
        }
        else if (converted >= ubound) {
            oerr = "is too large!";
            return false;
        }
        outval = converted;
        return true;
    }
    else {
        oerr = "is not a number!";
        return false;
    }
}
//------------------------------------------------------------
bool jfLineParse::DateVal(size_t index, QDate& outval) const {
  QString buffer = UnEscStr(index);
  outval = QDate::fromString(buffer,"M'-'d'-'yy");
  if (!outval.isValid()) return false;
  else return true;
}
//--------------------------------------------------------------
// returns a char in a certain range only
bool jfLineParse::CharVal(size_t index, QChar& outval, const QString& oallow) const {
  QString buffer = UnEscStr(index);
  if (buffer.length()!=1) return false;
  if (!oallow.isEmpty()) {
    if (!oallow.contains(buffer[0])) return false;
  }
  outval = buffer[0];
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// finding a value after a tag
bool jfLineParse::ReturnByTag(QString tag, QString& value) const {
  // special cases
  if (qres==NULL) return false;
  if (qres->count()<2) return false;
  // variables
  int rval;
  rval = qres->indexOf(tag);
  // checking for bad results
  if (rval==-1) return false;
  if (rval==(qres->count()-1)) return false;
  // okay, go ahead
  value = qres->at(rval+1);
  return true;
}
//---------------------------------------------------------------
bool jfLineParse::ReturnByTagS(QString tag, size_t& value) const {
  // variables
  ulong uresult;
  QString intval;
  // getting the value
  if (!ReturnByTag(tag,intval)) return false;
  // converting
  if (!Str2ULong(intval,uresult)) return false;
  // returning
  value = uresult;
  return true;
}
//---------------------------------------------------------------
bool jfLineParse::ReturnByTagB(QString tag, bool& value) const {
  QString intval;
  // getting the value
  if (!ReturnByTag(tag,intval)) return false;
  // converting
  if (!BoolFromStr(intval,value)) return false;
  // returning
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QStringList* jfLineParse::GetMultiple(size_t index, size_t item_count) const {
  // the checks
  assert(index<Num());
  assert(item_count!=0);
  assert((index+item_count-1)<Num());
  // local variables
  QStringList* result;
  // we start
  result = new QStringList;
  (*result) = qres->mid(index,item_count);
  return result;
}
//----------------------------------------------------------------
QStringList* jfLineParse::UnEscMultiple(size_t index, size_t item_count) const {
  // the checks
  assert(index<Num());
  assert(item_count!=0);
  assert((index+item_count-1)<Num());
  // local variables
  QStringList* result;
  // we start
  result = new QStringList();
  for (size_t loopc=index;loopc<(index+item_count);loopc++) result->append(UnEscStr(loopc));
  return result;
}
//----------------------------------------------------------------
QStringList* jfLineParse::GetRest(size_t index) const {
  assert(index<Num());
  QStringList* result;
  // we start
  result = new QStringList();
  (*result) = qres->mid(index);
  return result;
}
//----------------------------------------------------------------
QStringList* jfLineParse::UnEscRest(size_t index) const {
  assert(index<Num());
  QStringList* result;
  // we start
  result = new QStringList();
  for (size_t loopc=index;loopc<Num();loopc++) result->append(UnEscStr(loopc));
  return result;
}
//----------------------------------------------------------------
size_t jfLineParse::UnEscAppendRest(size_t index, QStringList& destination) const {
  assert(index<Num());
  for (size_t loopc = index;loopc<Num();loopc++) destination.append(UnEscStr(loopc));
  return (Num()-index);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// changing the value
size_t jfLineParse::SetString(const QString& instr) {
  if (qres!=NULL) delete qres;
  qres = ListToStrArr(instr,';');
  // if the input is empty, qres will be NULL
  return Num();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfLineParse::~jfLineParse() {
  if (qres!=NULL) delete qres;
}
//*****************************************************************************
/* sort of the opposite of jfLineParse, mean for inserting values in the string.
overrides some of the operators already there... we use ';' as separators and escape
string input... */
//-----------------------------------------------------------------------------
jfOutString::jfOutString():QString() {
    item_added = false;
}
jfOutString& jfOutString::operator<< (const QString& toadd) {
  if (item_added) {
    append(";");
  }
  append(EscapeNL(toadd,IO_DELIMS));
  item_added = true;
  return *this;
}
//-----------------------------------------------------------------------------
jfOutString& jfOutString::operator<< (const QChar* toadd) {
  if (item_added) {
    append(";");
  }
  QString buf1 = (*toadd);
  append(EscapeNL(buf1,IO_DELIMS));
  item_added = true;
  return *this;
}
//-----------------------------------------------------------------------------
jfOutString& jfOutString::operator<< (const bool& toadd) {
  if (item_added) {
    append(";");
  }
  if (toadd) append("true");
  else append("false");
  item_added = true;
  return *this;
}
//-----------------------------------------------------------------------------
jfOutString& jfOutString::operator<< (const size_t toadd) {
  if (item_added) {
    append(";");
  }
  append(QString::number(toadd));
  item_added = true;
  return *this;
}
//-----------------------------------------------------------------------------
jfOutString& jfOutString::operator<< (const int toadd) {
  if (item_added) {
    append(";");
  }
  append(QString::number(toadd));
  item_added = true;
  return *this;
}
//---------------------------------------------------------------------------
jfOutString& jfOutString::operator<< (const QDate& toadd) {
  if (item_added) {
    append(";");
  }
  append(EscapeNL(toadd.toString("M'-'d'-'yy"),IO_DELIMS));
  return *this;
}
//---------------------------------------------------------------------------
jfOutString& jfOutString::operator<< (const QStringList& toadd) {
  assert(!toadd.isEmpty());
  if (item_added) {
    append(";");
  }
  for (size_t add_idx = 0 ; add_idx < toadd.count() ; add_idx++) {
    append(EscapeNL(toadd[add_idx],IO_DELIMS));
    if (add_idx != toadd.count()-1) append(";");
  }
  item_added = true;
  return *this;
}
//---------------------------------------------------------------------------
void jfOutString::FullClear() {
    clear();
    item_added = false;
}

//******************************************************************************
// converts a QStringList to a delimted list, the items are all escaped first
QString StrArrToEscList(QStringList *source, const QString& delims) {
  const QString ichar = ";";
  QString result;
  QString buffer;
  // empty result cases
  if (source==NULL) return result;
  if (source->isEmpty()) return result;
  // constructing a non-empty result
  for (size_t i=0;i<(source->count());i++) {
    buffer = source->at(i);
    buffer = EscapeString(buffer,delims);
    buffer.replace(ichar,"\\"+ichar);
    result+= buffer + ichar;
  }
  // done
  return result;
}
//----------------------------------------------------------------------------
// the reverse of the above, converts an escaped, delimited list into a QStringList
QStringList *EscListToStrArr(const QString& source, const QString& delims) {
  // variables
  QChar ichar = QChar(';');
  QStringList *result;
  QString list,buffer,mdelims;
  int inpos;
  // checking to see if the source is empty
  list = source.trimmed();
  if (list.isEmpty()) return NULL;
  // tokenizing
  inpos = 0;
  result = new QStringList();
  mdelims = delims + ichar;
  // the main parsing loop
  while (inpos!=-1) {
    buffer = GetToUnescaped(list,ichar,inpos);
    buffer = buffer.trimmed();
    UnEscapeNL(buffer,mdelims);
    result->append(buffer);

  }
  // done with the loop, special case postprocessing
  if (result->isEmpty()) {
    delete result;
    result = NULL;
  }
  return result;
}
//****************************************************************************
// constructor
jfFileReader::jfFileReader(QString infname) {
  thefile = NULL;
  inpfile = NULL;
  fname = infname;
  lineindex = -1;
  usebackup = false;
  backup_possible = false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file stuff
//------------------------------
bool jfFileReader::OpenFile() {
  CloseFile();
  inpfile = new QFile(fname);
  if (!inpfile->open(QIODevice::ReadOnly | QIODevice::Text)) {
    err = "Could not open file!";
    delete inpfile;
    inpfile = NULL;
    return false;
  }
  thefile = new QTextStream(inpfile);
  thefile->setCodec("UTF-8");
  lineindex = -1;
  return true;
}
//------------------------------
bool jfFileReader::CloseFile() {
  if (inpfile==NULL) return false;
  thefile->flush();
  inpfile->flush();
  inpfile->close();
  delete thefile;
  delete inpfile;
  inpfile = NULL;
  thefile = NULL;
  return true;
}
//-------------------------------------
size_t jfFileReader::LineIndex() const {
  return lineindex;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file reading
//------------------------------
bool jfFileReader::ReadLine(QString& outline, const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  return StartRead(outline);
}
//------------------------------
bool jfFileReader::ReadParseSLine(size_t fcount, const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  QString oline;
  bool bres = StartRead(oline);
  if (!bres) return false;
  if (oline.left(3)!="###") {
    return BuildError("Line does not start with ###! "+oline);
  }
  else {
    oline = oline.mid(3);
    return SetParse(oline,fcount);
  }
}
//------------------------------
bool jfFileReader::ReadParseLine(size_t fcount, const QString& finname) {
  QString popname = "jfFileReader::ReadParseLine A";
  if (!finname.isEmpty()) func_name = finname;
  QString oline;
  if (!StartRead(oline)) return false;
  return SetParse(oline,fcount);
}
//--------------------------------------------------
bool jfFileReader::ReadParseLine(size_t fmin, size_t fmax, const QString& finname) {
  QString popname = "jfFileReader::ReadParseLine B";
  assert(fmin<=fmax);
  if (!finname.isEmpty()) func_name = finname;
  QString oline;
  if (!StartRead(oline)) return false;
  lp.SetString(oline);
  if (lp.Num()<fmin) return BuildError("The line has less than " + QString::number(fmin) + " fields!");
  if (lp.Num()<fmin) return BuildError("The line has more than " + QString::number(fmax) + " fields!");
  return true;
}
//------------------------------
bool jfFileReader::ReadLast(const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  QString oline;
  if (!StartRead(oline)) return false;
  if (oline!="###") return BuildError("Line is not ###! is " + oline);
  else return true;
}
//------------------------------
bool jfFileReader::GetType(QString& outtype, const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  QString oline;
  if (!StartRead(oline)) return false;
  if (oline.left(3)!="###") {
    Backup();
    return BuildError("Line does not start with ###!" + oline);
  }
  else {
    oline = oline.mid(3);
    lp.SetString(oline);
    Backup();
    if (lp.Num()==0) return BuildError("No Fields!");
    outtype = lp.UnEscStr(0);
    if (outtype.isEmpty()) return BuildError("Type is empty!");
    return true;
  }
}
//------------------------------
bool jfFileReader::ReadUnEsc(QString& outline, const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  if (!StartRead(outline)) return false;
  UnEscapeNL(outline,IO_DELIMS);
  return true;
}
//------------------------------
bool jfFileReader::ReadULong(unsigned long& outnum, const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  QString outline;
  if (!StartRead(outline)) return false;
  if (!Str2ULongC(outline,outnum)) {
    return BuildError("Line is not a valid unsigned integer!");
  }
  else return true;
}
//-----------------------------------
bool jfFileReader::AppendToStringList(QStringList* outlines, int addcount, const QString& finname) {
  // handling the basics
  if (!finname.isEmpty()) func_name = finname;
  if (outlines==NULL) return BuildError("Cannot append to NULL String List!");
  if (addcount==0) return true;
  // variables used in reading
  size_t lcount = 0;
  bool lrsucc;
  QString rline;
  // the appending loop
  while (true) {
    lrsucc = StartRead(rline);
    // successful line read
    if (lrsucc) {
      outlines->append(rline);
      lcount++;
      if (lcount==addcount) return true;
    }
    // failed line read
    else {
      if (addcount<0) return true; // the number of lines to read was not specified
      else return false; // we have failed to reach the line count
    }
  }
  // control never gets here
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFileReader::SkipLine(const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  // at end
  if (thefile->atEnd()) {
    return BuildError("Cannot skip to a line after the last!!");
  }
  thefile->readLine();
  lineindex++;
  return true;
}
//----------------------------------
bool jfFileReader::Backup(const QString& finname) {
  if (!finname.isEmpty()) func_name = finname;
  // special cases
  if (!backup_possible) return BuildError("Cannot backup!");
  else {
    backup_possible = false;
    usebackup = true;
    return true;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
//------------------------------
bool jfFileReader::BuildError(QString inerr) {
  const QString fname = "jfFileReader::BuildError";
  err = inerr + " IN " + func_name + ", LINE: " + QString::number(lineindex);
  err += ", FILE: " + fname;
  return false;
}
//----------------------------------------------------
bool jfFileReader::AtEnd() const {
  return thefile->atEnd();
}
// --------------------
bool jfFileReader::IsOpen() const {
  return (inpfile != NULL);
}
// --------------------
bool jfFileReader::IsOpenBeforeEnd() const {
    return (inpfile != NULL) && (thefile != NULL) && (thefile->atEnd() == false);
}
//------------------------------
jfFileReader::~jfFileReader() {
  CloseFile();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal methods
//------------------------------
bool jfFileReader::StartRead(QString& outline) {
  assert(thefile!=NULL);
  // at end
  if (thefile->atEnd()) {
    return BuildError("Unexpected end of file!");
  }
  // reading or backup?
  if (usebackup) {
    outline = oldline;
    backup_possible = false;
    usebackup = false;

  }
  else {
    outline = thefile->readLine();
    oldline = outline;
    lineindex++;
    backup_possible = true;
  }
  return true;
}
//------------------------------
bool jfFileReader::SetParse(const QString& inval, size_t check) {
  QString faname = "jfFileReader::SetParse";
  lp.SetString(inval);
  if ((check > 0) && lp.NNotX(check)) {
    /**//**/JDEBUGLOG(faname,1);
    return BuildError("The line does not have " + QString::number(check) + " fields!");
  }
  else return true;
}
//-----------------------------

//*****************************************************************************
