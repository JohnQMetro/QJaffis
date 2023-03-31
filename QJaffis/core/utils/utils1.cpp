/*****************************************************************************
Name    : utils1.cpp
Basic   : Defines some utility functions and things used in the other files
Author  : John Q Metro
Started : March 17, 2009
Conversion to QT started : February 21, 2013
Updated : February 18, 2023
Notes   :

******************************************************************************/
#ifndef UTILS1_H_INCLUDED
  #include "utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "logging.h"
#endif // LOGGING_H_INCLUDED

#include <QRegExp>

#include <assert.h>
#include <math.h>
#include <QtAlgorithms>

//****************************************************************************
const QString EXPR_DELIMS="&|~%();@#";
//===========================================================================
// converts a double to a string
QString Doubl2Str(const double& input) {
  return QString::number(input,'f',-1);
}
//--------------------------------------------------------------------------
// converts a boolean to a string
QString Bool2Str(const bool& inval) {
  // local variables
  QString result;
  result = (inval)?("true"):("false");
  return result;
}
//------------------------------------------------------------------------
// I prefer a boolean as the result, so I can embed the conversion in IF blocks
bool Str2ULong(const QString& inval, ulong& outval, bool hex) {
  // variables
  bool xresult;
  int base = (hex)?(16):(10);
  // starting
  outval = inval.toULong(&xresult,base);
  return xresult;
}
//--------------------------------------------------------------------
// a version of the above that handles commas
bool Str2ULongC(const QString& inval, ulong& outval) {
  QString ninval = inval;
  ninval.remove(QChar(','));
  return Str2ULong(ninval,outval);
}
//-------------------------------------------------------------------
// handles values like '1.9K'
bool Count2ULong(const QString& inval, unsigned long& outval) {
  //local variables
  double xval;
  uint slen;
  bool resval;
  QString zpval;
  // we start
  slen = inval.length();
  outval = 0;
  zpval = inval;
  // counts in the thousands
  if (inval[slen-1]=='K') {
    zpval.chop(1);
    xval = zpval.toDouble(&resval);
    if (resval) outval = floor(xval*1000);
    return resval;
  }
  // more precise under 1000 count
  else return  Str2ULongC(inval,outval);
}
//==========================================================================
/* takes an input string. if the string is "true" or "false", returns TRUE
and also passes back which in the reference parameter */
bool BoolFromStr(const QString& in_src, bool& result) {
    if (in_src=="true") {
      result = true;
      return true;
    }
    else if (in_src=="false") {
      result = false;
      return true;
    }
    else return false;
}
//=================================================================================
/* Takes a string and returns the result with the specified chars in delim
preceeded with a backslash. Backslashes are also escapsed. Do not repeat
characters in delims. */
QString EscapeString(const QString& in_base, const QString& delims) {
  // variables
  QRegExp delimexp("([" + delims + "\\\\])");
  QString incopy = in_base;
  // using reg expressions with capture
  incopy.replace(delimexp,"\\\\1");
  return incopy;
}
//==================================================================================
// this function finds the first character in delims that is not preceeded by
// a backslash
int FindNonEscaped(const QString& base, const QString& delims, size_t pos) {
  // local variables
  QRegExp delimexp1("^[" + delims + "]");
  QRegExp delimexp2("[^\\\\][" + delims + "]");
  int rpos;
  bool posz = (pos==0);
  // first find
  if (posz) {
    rpos = base.indexOf(delimexp1,pos);
    if (rpos==0) return 0;
  }
  // other find
  if (!posz) pos = pos -1;
  rpos = base.indexOf(delimexp2,pos);
  if (rpos==-1) return -1;
  else return (rpos+1);
}
//==========================================================================
/* using the delim list, replaces any escaped delimiters with the corresponding */
size_t UnEscape(QString& base, const QString& delims) {
  const QString fname = "UnEscape";
  // local variables
  QString xdelims;
  xdelims = delims;
  xdelims.replace("^","\\^");
  xdelims += "\\\\";
  QRegExp delimexp("\\\\([" + xdelims + "])");
  size_t dlen;
  // initial stuff
  dlen = delims.length();
  // a special case
  if (dlen==0) return 0;
  // doing the unescape
  base.replace(delimexp,"\\1");
  return delimexp.captureCount();
}
//============================================================================
/* Versions of EscapeString and UnEscape that also replace newlines with ^.
Thus, ^ is also a default escaped character and should not be in delims */
//---------------------------------------------------------------------------
QString EscapeNL(const QString& in_base, const QString& delims) {
  QString ndelims,xresult;
  if (in_base.length()==0) return xresult;
  // we prep the new delim line
  ndelims =  delims+ "\\^";
  xresult = EscapeString(in_base,ndelims);
  // past the special case, we start replacing newlines
  xresult.replace(QChar('\n'),QChar('^'));
  // doing the rest
  return xresult;
}
//-----------------------------------------------------------------------------
/* Grabs text between pos and the next instance of dchar not preceeded by an odd
  number of backslashes */
QString GetToUnescaped(const QString& base, QChar dchar, int& pos) {
  // variables
  QString resbuf;
  QChar curchar;
  size_t strlen, strindex;
  bool bseven;
  // objection!
  assert(dchar!=QChar('\\'));
  // initializing
  strlen = base.length();
  bseven = true;
  // special case
  if ((pos==-1) || (pos>=strlen)) {
    pos = -1;
    return "";
  }
  // the main character scanning loop
  for (strindex=pos;strindex<strlen;strindex++) {
    curchar = base.at(strindex);
    if (curchar==dchar) {
      if (bseven) break;
      else {
        resbuf += curchar;
        bseven = true;
      }
    }
    else if (curchar==QChar('\\')) {
      resbuf += curchar;
      bseven = !bseven;
    }
    else {
      resbuf += curchar;
      bseven = true;
    }
  }
  // post loop processing
  if (strindex<strlen) pos = strindex+1;
  else pos = -1;
  return resbuf;
}
//---------------------------------------------------------------------------
/* We replace ^ with a newline if it is unescaped, and the unescape the rest.
In the text, that means either it is preceded by no backslashes, or an even
number of backslashes. QRegExp might be able to handle this, but i'm sticking
with it directly.*/
size_t UnEscapeNL(QString& base, const QString& delims) {
  // variables
  const QString fname = "UnEscapeNL";
  QString resbuf,ndelims;
  size_t rcount;
  int inpos;
  // initializing
  rcount = 0;
  inpos = 0;
  // the string parsing loop
  while (inpos!=-1) {
    resbuf += GetToUnescaped(base,QChar('^'),inpos);
    if (inpos!=-1) {
      resbuf += QChar('\n');
      rcount++;
    }
  }
  // unescaping the other characters
  ndelims = QChar('^') + delims;
  base = resbuf;
  rcount += UnEscape(base,ndelims);
  return rcount;
}
//-------------------------------------------------------------------------
// converts a QStringList to a semi-colon delimited list
QString StrArrToList(QStringList *source, QChar ichar) {
  QString result;
  QString buffer,tdelim;
  // empty result cases
  if (source==NULL) return result;
  if (source->isEmpty()) return result;
  // constructing a non-empty result
  for (size_t i=0;i<(source->count());i++) {
    buffer = source->at(i);
    tdelim = QString('\\') + ichar;
    buffer.replace(ichar,tdelim);
    if (i!=(source->count()-1)) result+= buffer + ichar;
    else result+=buffer;
  }
  // done
  return result;
}
//==========================================================================
// converts a delimited list into a QStringList
QStringList *ListToStrArr(const QString& source, QChar ichar, bool trims) {
  // variables
  QStringList *result;
  QString list,buffer,delims;
  int inpos;
  // checking to see if the source is empty
  if (trims) list = source.trimmed();
  else list = source;
  if (list.isEmpty()) return NULL;
  // tokenizing
  inpos = 0;
  result = new QStringList();
  delims = ichar;
  // the main parsing loop
  while (inpos!=-1) {
    buffer = GetToUnescaped(list,ichar,inpos);
    if (trims) buffer = buffer.trimmed();
    UnEscape(buffer,delims);
    result->append(buffer);
  }
  // done with the loop, special case postprocessing
  if (result->isEmpty()) {
    delete result;
    result = NULL;
  }
  return result;
}
// =========================================================================
QStringList SplitTrim(const QString& source, const QString& delimiter) {
    QStringList res1 = source.split(delimiter);
    QStringList res2 = QStringList();
    for (QString part : res1) {
        QString part2 = part.trimmed();
        if (part2.isEmpty() == false) res2.append(part2);
    }
    return res2;
}
// ----------------------------------------------
QStringList SplitTrim(const QString& source, const QChar& delimiter) {
    QStringList res1 = source.split(delimiter);
    QStringList res2 = QStringList();
    for (QString part : res1) {
        QString part2 = part.trimmed();
        if (part2.isEmpty() == false) res2.append(part2);
    }
    return res2;
}
// ---------------------------------------------
QRegularExpression makeNewlineExpression() {
    // flexible regexp for newlines. Please note that due to compiler limitations,
    // Unicode literals cannot be used
    QString xo = "\\n|\\r\\n|\\r|\\n\\r|";
    xo += QString(QChar(0x2028)) + "|" + QChar(0x2029) + "|" + QChar(0x0085);
    return std::move(QRegularExpression(xo));
}
// ----------------------------------------------
QStringList SplitTrimNL(const QString& source) {
    QStringList res1 = source.split(makeNewlineExpression());
    QStringList res2 = QStringList();
    for (QString part : res1) {
        QString part2 = part.trimmed();
        if (part2.isEmpty() == false) res2.append(part2);
    }
    return res2;
}

//===========================================================================
// splits a string at the location, not including that location
bool SplitString(const QString& input, size_t point, QString& out1, QString& out2) {
  size_t slen;
  slen = input.length();
  if (point>=slen) return false;
  // getting part1
  out1 = input.left(point);
  out1 = out1.trimmed();
  // the second part
  out2 = input.mid(point+1);
  out2 = out2.trimmed();
  // done
  return true;
}
//===========================================================================
// takes an input double, and outputs it as a percentage
QString ToPercent(const double& insrc, const size_t& prec) {
  QString res;
  double pvalue;
  // basic checks
  assert(prec<15);
  assert(insrc>=0);
  assert(insrc<9000000000000.0);
  // taking the input value and preparing
  pvalue = insrc*100;
  // producing the result
  res = QString::number(pvalue,'f',prec);
  return res.trimmed() + QChar('%');
}
//============================================================================
/* Qt does not seem to have locale based caseconversion, and even if it did,
 * it still might be useful to do case-insensetive turkish/azeri matches alongside
 * other languages. */
QString toLowercaseTurkishAzeri(QString input) {
  // setting up the specials
  QChar dotlessi = QChar((ushort)0x0131u);  // i (dotless i)
  QChar dottedI = QChar((ushort)0x0130u);  // I (dotted I)
  // converting the I's
  input.replace("I",dotlessi);
  input.replace(dottedI,"i");
  return input.toLower();
}
//=============================================================
bool caseInsensitiveLessThan(const QString &s1, const QString &s2) {
    return s1.toLower() < s2.toLower();
}
//------------------------------------------------------------------

void SortStringList(QStringList* tosort, bool casesen) {
  if (tosort!=NULL){
    if (casesen) tosort->sort();
    else std::sort(tosort->begin(),tosort->end(),caseInsensitiveLessThan);
  }
}

//****************************************************************************
