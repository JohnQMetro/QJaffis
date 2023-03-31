/*****************************************************************************
Name    : stringparser.cpp
Basic   : String parsing class
Author  : John Q Metro
Started : August 21, 2012 (split from utils2.cpp)
Conversion to QT started : March 2, 2013
Updated : July 11, 2022
Notes   :

******************************************************************************/
#ifndef STRINGPARSER_H_INCLUDED
  #include "stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
//----------------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "logging.h"
#endif // LOGGING_H_INCLUDED
//----------------------------------------
#include <assert.h>
#include <QRegularExpressionMatch>
/*****************************************************************************/
// the constructors
jfStringParser::jfStringParser() {
  ready = false;
  mainindex = 0;
  oldindex = 0;
  casesen = Qt::CaseInsensitive;
  xendl = makeNewlineExpression();
}
//--------------------------------------------------------------------------
jfStringParser::jfStringParser(QString indata, bool incasesen) {
  ChangeData(indata,incasesen);
  xendl = makeNewlineExpression();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// change indata
void jfStringParser::ChangeData(QString indata, bool incasesen) {
  rawdata = indata;
  mainindex = 0;
  oldindex = 0;
  skipped_data.clear();
  // a final check
  ready = !rawdata.isEmpty();
  if (incasesen)casesen = Qt::CaseSensitive;
  else casesen = Qt::CaseInsensitive;
}
// ----------------------------------------------------
void jfStringParser::Clear() {
    ready = false;
    mainindex = 0;
    oldindex = 0;
    rawdata.clear();
    skipped_data.clear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting basic info
//--------------------------------------------------------------------------
bool jfStringParser::IsReady() const {
  return ready;
}
//--------------------------------------------------------------------------
bool jfStringParser::IsCaseSensetive() const {
  return (casesen==Qt::CaseSensitive);
}
//--------------------------------------------------------------------------
size_t jfStringParser::GetIndex() const {
  return mainindex;
}
//--------------------------------------------------------------------------
size_t jfStringParser::GetLength() const {
  return rawdata.length();
}
//--------------------------------------------------------------------------
QString jfStringParser::GetData() const {
  return rawdata;
}
//--------------------------------------------------------------------------
QString jfStringParser::GetBlock(size_t innlen) const {
  // special cases
  if ((innlen==0) || (mainindex>=rawdata.length())) return "";
  // getting it
  return rawdata.mid(mainindex,innlen);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// changing some controllers
//--------------------------------------------------------------------------
void jfStringParser::SetCaseSen(bool incasesen) {
  if (incasesen)casesen = Qt::CaseSensitive;
  else casesen = Qt::CaseInsensitive;
}
//--------------------------------------------------------------------------
bool jfStringParser::SetIndex(size_t innindex) {
  oldindex = mainindex;
  mainindex = innindex;
  if (innindex>=rawdata.length()) return false;
  else return true;
}
//------------------------------------------------------------------
bool jfStringParser::GoBack() {
  if (oldindex==mainindex) return false;
  mainindex = oldindex;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// looks for <find>, if found, returns true and moves the index past the substring
bool jfStringParser::MovePast(QString find) {
  // local variables
  int fpos;
  // basic check
  if (!ready) return false;
  if (find.isEmpty()) return false;
  // looking...
  fpos = rawdata.indexOf(find,mainindex,casesen);
  if (fpos == -1) return false;
  else {
    skipped_data = rawdata.mid(mainindex,fpos-mainindex);
    SetIndex(fpos + find.length());
    return true;
  }
}
// ------------------------------------------------------------------
bool jfStringParser::MovePast(const QRegularExpression& find) {
    if (CheckRegex(find)) {
        QRegularExpressionMatch match = find.match(rawdata, mainindex);
        if (match.hasMatch()) {
            int startpos = match.capturedStart();
            int matchlen = match.capturedLength();
            skipped_data = rawdata.mid(mainindex,startpos-mainindex);
            SetIndex(startpos + matchlen);
            return true;
        }
        else return false;
    }
    else return false;
}

//------------------------------------------------------------------------
/* looks for the second instance of <find>. if found, returns true and moves
the index past the substring */
bool jfStringParser::MovePastTwice(QString find) {
  //constants
  QString fname = "jfStringParser::MovePastTwice";
  // local variables
  int fpos;
  size_t oindex;
  // starting and checks
  if (!ready) return false;
  oindex = mainindex;
  // looking once...
  fpos = rawdata.indexOf(find,mainindex,casesen);
  if (fpos == -1) return false;
  // we've found, so we look again
  mainindex = fpos + find.length();
  fpos = rawdata.indexOf(find,mainindex,casesen);
  if (fpos == -1) {
    mainindex = oindex;
    return false;
  }
  // we've found the item
  else {
    skipped_data = rawdata.mid(oindex,fpos-oindex);
    SetIndex(fpos + find.length());
    return true;
  }
}
//-------------------------------------------------------------------------
bool jfStringParser::MovePastLimit(QString find, QString limit) {
    // local variables
    int limpos,fpos;
    // basic check
    if (!ready) return false;
    if (limit.isEmpty()) return false;
    if (find.isEmpty()) return false;
    // looking...
    limpos = rawdata.indexOf(limit,mainindex,casesen);
    if (limpos < 0) return false;
    fpos = rawdata.indexOf(find,mainindex,casesen);
    if (fpos == -1) return false;
    if (fpos > limpos) return false;
    // moving and setting the new index
    skipped_data = rawdata.mid(mainindex,fpos-mainindex);
    SetIndex(fpos + find.length());
    return true;
}
//-------------------------------------------------------------------------
/* Sort of like using MovePast twice, but we keep the original mainindex
if we cannot find both substrings */
bool jfStringParser::MovePastTwo(QString first, QString second) {
  // local variables
  size_t secindex;
  int fpos;
  // basic check
  if (!ready) return false;
  // looking...
  fpos = rawdata.indexOf(first,mainindex,casesen);
  if (fpos == -1) return false;
  else {
    // looking again
    secindex = fpos + first.length();
    fpos = rawdata.indexOf(second,secindex,casesen);
    if (fpos == -1) return false;
    // handling a positive result
    skipped_data = rawdata.mid(mainindex,fpos-mainindex);
    SetIndex(fpos + second.length());
    return true;
  }
}
//-------------------------------------------------------------------------
// similar to MovePast, but uses Find1stPair to move past the first
bool jfStringParser::MovePastAlt(QString find1, QString find2) {
  // local variables
  size_t fpos,which;
  size_t whichl;
  bool locres;
  // we start
  locres = Find1stPair(find1,find2,which,fpos);
  if (!locres) return false;
  if (which==1) whichl = find1.length();
  else whichl = find2.length();
  skipped_data = rawdata.mid(mainindex,fpos-mainindex);
  SetIndex(fpos + whichl);
  return true;
}
//--------------------------------------------------------------------------
// uses the StartsWithAtIndex method to skip a substring if there
bool jfStringParser::MovePastStart(QString start) {
  // a special case
  if (!ready) return false;
  // the main finding...
  if (StartsWithAtIndex(start)) {
    SetIndex(mainindex + start.length());
    return true;
  }
  else return false;
};
//--------------------------------------------------------------------------
bool jfStringParser::MovePastStartAlt(QString start1, QString start2) {
  // a special case
  if (!ready) return false;
  // checking...
  bool found1, found2;
  found1 = StartsWithAtIndex(start1);
  if (found1 && (start1.length()>=start2.length())) {
    SetIndex(mainindex+start1.length());
    return true;
  }
  found2 = StartsWithAtIndex(start2);
  if (found2) {
    SetIndex(mainindex+start2.length());
    return true;
  }
  else if (found1) {
    SetIndex(mainindex+start1.length());
    return true;
  }
  else return false;
}
//--------------------------------------------------------------------------
QString jfStringParser::GetSkipped() const {
  return skipped_data;
}
//--------------------------------------------------------------------------
/* looks for <find>, if found, returns true, returns everything between the index
and the start of <find> in outbuf, and moves the index past the substring */
bool jfStringParser::GetMovePast(QString end, QString& outbuf) {
  int fpos;
  if (!ready) return false;
  fpos = rawdata.indexOf(end,mainindex,casesen);
  if (fpos == -1) return false;
  else {
    outbuf = rawdata.mid(mainindex,fpos-mainindex);
    SetIndex(fpos + end.length());
    return true;
  }
}
bool jfStringParser::GetMovePast(const QRegularExpression end, QString& outbuf) {
    if (CheckRegex(end)) {
        QRegularExpressionMatch match = end.match(rawdata, mainindex);
        if (match.hasMatch()) {
            int startpos = match.capturedStart();
            outbuf = rawdata.mid(mainindex,startpos-mainindex);
            SetIndex(startpos + match.capturedLength());
            return true;
        }
        else return false;
    }
    else return false;
}
//------------------------------------------------------------------------
bool jfStringParser::MovePastOrEnd(QString endval, QString& outbuf) {
  // returning false
  if (!ready) return false;
  if (mainindex>=rawdata.length()) return false;
  // variables
  bool qval = GetMovePast(endval,outbuf);
  if (!qval) {
    qval = GetToEnd(outbuf);
    if (qval) mainindex = rawdata.length();
  }
  return qval;
}
//-------------------------------------------------------------------------
bool jfStringParser::GetMovePastPair(QString end1, QString end2, QString& outbuf) {
  // local variables
  size_t wres, pres;
  // starting
  if (Find1stPair(end1,end2,wres,pres)) {
    outbuf = rawdata.mid(mainindex,pres-mainindex);
    if (wres==1) SetIndex(pres + end1.length());
    else SetIndex(pres + end2.length());
    return true;
  }
  else return false;
}
//--------------------------------------------------------------------------------------
bool jfStringParser::GetMovePastPairI(QString end1, QString end2, QString& outbuf, bool& owhich) {
  // local variables
  size_t wres, pres;
  // starting
  if (Find1stPair(end1,end2,wres,pres)) {
    outbuf = rawdata.mid(mainindex,pres-mainindex);
    if (wres==1) {
      SetIndex(pres + end1.length());
      owhich = true;
    }
    else {
      SetIndex(pres + end2.length());
      owhich = false;
    }
    return true;
  }
  else return false;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special move upto methods
bool jfStringParser::MoveUpto(QString ifind) {
  // local variables
  int fpos;
  // basic check
  if (!ready) return false;
  // looking...
  fpos = rawdata.indexOf(ifind,mainindex,casesen);
  if (-1) return false;
  else {
    skipped_data = rawdata.mid(mainindex,fpos-mainindex);
    SetIndex(fpos);
    return true;
  }
}
//------------------------------------------------------------
bool jfStringParser::GetUpto(QString end, QString& outres) {
  int fpos;
  if (!ready) return false;
  fpos = rawdata.indexOf(end,mainindex,casesen);
  if (fpos == -1) return false;
  else {
    outres = rawdata.mid(mainindex,fpos-mainindex);
    SetIndex(fpos);
    return true;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// similar to get move past, but tries to get a number
bool jfStringParser::GetMovePastULong(QString end, ulong& outval, QString& outerr) {
  // constants
  QString err_endnotfound = "The specified end of the field was not found!";
  QString err_notaninteger = "The field was found, but is not an integer!";
  // local variables
  QString buffer;
  size_t oindex;
  // we start...
  oindex = mainindex;
  if (!GetMovePast(end,buffer)) {
    outerr = err_endnotfound;
    return false;
  }
  buffer = buffer.trimmed();
  // numeric conversion
  if (!Str2ULongC(buffer,outval)) {
    outerr = err_notaninteger;
    mainindex = oindex;
    return false;
  }
  return true;
}
//------------------------
bool jfStringParser::GetMovePastAltULong(QString end1, QString end2, ulong& outval, QString& outerr) {
    // constants
    QString err_endnotfound = "The specified end of the field was not found!";
    QString err_notaninteger = "The field was found, but is not an integer!";
    // local variables
    QString buffer;
    size_t oindex;
    // we start...
    oindex = mainindex;
    if (!GetMovePastPair(end1,end2,buffer)) {
      outerr = err_endnotfound;
      return false;
    }
    buffer = buffer.trimmed();
    // numeric conversion
    if (!Str2ULongC(buffer,outval)) {
      outerr = err_notaninteger;
      mainindex = oindex;
      return false;
    }
    return true;
}

//--------------------------------------------------------------------------
/* a combination of MovePast and GetMovePast, return the value between <start> and
<end> (if found), and move the end past <end> */
bool jfStringParser::GetDelimited(QString start, QString end, QString& outbuf) {
  if (!MovePast(start)) return false;
  return GetMovePast(end,outbuf);
}
//--------------------------------------------------------------------------
// regular expression extract
bool jfStringParser::GetUsingRegex(const QRegularExpression& regex, bool first_capture, QString& outbuf) {
    if (CheckRegex(regex)) {
        if (first_capture && (regex.captureCount() < 1)) return false;
        QRegularExpressionMatch match = regex.match(rawdata, mainindex);
        if (match.hasMatch()) {
            int startpos = match.capturedStart();
            outbuf = match.captured((first_capture) ? 1 : 0);
            SetIndex(startpos + match.capturedLength());
            return true;
        }
        else return false;

    }
    else return false;
}
//--------------------------------------------------------------------------
bool jfStringParser::GetULongUsingRegex(const QRegularExpression& regex, bool first_capture, ulong& outval, QString& outerr) {
    if (CheckRegex(regex)) {
        if (first_capture && (regex.captureCount() < 1)) {
            outerr = "The regx contains no capture groups, but first_capture is true!";
            return false;
        }
        QRegularExpressionMatch match = regex.match(rawdata, mainindex);
        if (match.hasMatch()) {
            int startpos = match.capturedStart();
            QString capture = match.captured((first_capture) ? 1 : 0);
            // numeric conversion
            if (Str2ULongC(capture,outval)) {
                SetIndex(startpos + match.capturedLength());
                return true;
            }
            else {
                outerr = "The captured value could not be converted to an integer!";
                return false;
            }
        }
        else {
            outerr = "No match was found";
            return false;
        }
    }
    else {
        outerr = "Parameter error, the regex is defective OR the index is past the end of the string.";
        return false;
    }
}
//------------------------------------------------------------------------------------
// similar to GetDelimited, but tries to get a number.
bool jfStringParser::GetDelimitedULong(QString start, QString end, unsigned long& outval, QString& outerr) {
  // constants
  QString err_startnotfound = "The specified start of the field was not found!";
  QString err_endnotfound = "The specified end of the field was not found!";
  QString err_notaninteger = "The field was found, but is not an integer!";
  // local variables
  QString buffer;
  size_t oindex;
  // we start...
  oindex = mainindex;
  if (!MovePast(start)) {
    outerr = err_startnotfound;
    return false;
  }
  if (!GetMovePast(end,buffer)) {
    outerr = err_endnotfound;
    mainindex = oindex;
    return false;
  }
  buffer = buffer.trimmed();
  // numeric conversion
  if (!Str2ULongC(buffer,outval)) {
    outerr = err_notaninteger;
    mainindex = oindex;
    return false;
  }
  return true;
}
//-----------------------------------------------------------------------------------
const QString err_startnotfound = "The specified start of the field was not found!";
const QString err_endnotfound = "The specified end of the field was not found!";
const QString err_notaninteger = "The field was found, but is not an integer!";
//------------------------------------------------------------------------------------
bool jfStringParser::GetDelimitedFloat(QString start, QString end, double& outval, QString& outerr) {
  // local variables
  QString buffer;
  size_t oindex;
  bool okres;
  // we start...
  oindex = mainindex;
  if (!MovePast(start)) {
    outerr = err_startnotfound;
    return false;
  }
  if (!GetMovePast(end,buffer)) {
    outerr = err_endnotfound;
    mainindex = oindex;
    return false;
  }
  buffer = buffer.trimmed();
  // numeric conversion
  outval = buffer.toDouble(&okres);
  if (!okres) {
    outerr = err_notaninteger;
    mainindex = oindex;
    return false;
  }
  return true;
}

//--------------------------------------------------------------------------------------
/* another version of get delimited which uses a pair of possible ends */
bool jfStringParser::GetDelimitedEndPair(QString start, QString end1,QString end2, QString& outbuf) {
  // moving past the start
  if (!MovePast(start)) return false;
  // using find first pair
  return GetMovePastPair(end1,end2,outbuf);
}
//----------------------------------------------------------------------------
bool jfStringParser::GetDelimitedEndPairULong(QString start, QString end1,QString end2, ulong& outval, QString& outerr) {
    // local variables
    QString buffer;
    size_t oindex;
    bool okres;
    // we start...
    oindex = mainindex;
    if (!MovePast(start)) {
      outerr = err_startnotfound;
      return false;
    }
    if (!GetMovePastPair(end1,end2,buffer)) {
      outerr = err_endnotfound;
      mainindex = oindex;
      return false;
    }
    buffer = buffer.trimmed();
    // numeric conversion
    outval = buffer.toDouble(&okres);
    if (!okres) {
      outerr = err_notaninteger;
      mainindex = oindex;
      return false;
    }
    return true;
}

//----------------------------------------------------------------------------
/* return strue if a line starts with a certain string */
bool jfStringParser::StartsWith(QString start) const {
  return rawdata.startsWith(start,casesen);
}
//------------------------------------------------------------------------------
// returns true if the given substring is at the current index
bool jfStringParser::StartsWithAtIndex(QString start) const {
  int fpos;
  if (!ready) return false;
  fpos = rawdata.indexOf(start,mainindex,casesen);
  if (fpos == -1) return false;
  return (fpos==mainindex);
}
//-----------------------------------------------------------------------------
/* locates the first of a pair */
bool jfStringParser::Find1stPair(QString astr, QString bstr, size_t& which, size_t& outloc) {
  // variables
  int fpos1,fpos2;
  size_t slen1,slen2;
  bool npos1,npos2;
  // we search
  fpos1 = rawdata.indexOf(astr,mainindex,casesen);
  fpos2 = rawdata.indexOf(bstr,mainindex,casesen);
  npos1 = (fpos1==-1);
  npos2 = (fpos2==-1);
  // going thru the cases
  // both are not found
  if (npos1 && npos2) return false;
  // 1 is not found
  if (npos1) {
    which = 2;
    outloc = fpos2;
  }
  // 2 is not found
  else if (npos2) {
    which = 1;
    outloc = fpos1;
  }
  // both are found
  else {
    if (fpos1<fpos2) {
      which = 1;
      outloc = fpos1;
    }
    else if (fpos1==fpos2) {
      slen1 = astr.length();
      slen2 = bstr.length();
      outloc = fpos1;
      if (slen1<slen2) which = 2;
      else which = 1;
    }
    else {
      which = 2;
      outloc = fpos2;
    }
  }
  return true;
}
//----------------------------------------------------------------------------
/* locates the first of a multiple set */
bool jfStringParser::Find1stSet(const QStringList* instring, size_t& which, size_t& outloc) const {
  // local variables
  bool found = false;
  size_t lowpos,lowindex;
  size_t loopc,numstr;
  int fpos;
  QString cstring;
  // special cases
  if (instring==NULL) return false;
  // preparing for the loop
  numstr = instring->count();
  if (numstr==0) return false;
  lowpos = GetLength()+50;
  // the loop
  for (loopc=0;loopc<numstr;loopc++) {
    cstring = (*instring)[loopc];
    fpos = rawdata.indexOf(cstring,mainindex,casesen);
    if (fpos!=-1) {
      found = true;
      if (fpos<lowpos) {
        lowpos = fpos;
        lowindex = loopc;
      }
    }
  }
  // after the loop, we produce the results
  if (found) {
    which = lowindex;
    outloc = lowpos;
  }
  return found;
}
//----------------------------------------------------------------------------
bool jfStringParser::GetMovePastSet(const QStringList* instring, size_t& which, QString& outbuf) {
  // variables
  size_t outpos,endlen;
  bool isfound;
  // starting
  if (!ready) return false;
  isfound = Find1stSet(instring,which,outpos);
  if (!isfound) return false;
  // extracting
  outbuf = rawdata.mid(mainindex,outpos-mainindex);
  endlen = (instring->at(which)).length();
  SetIndex(outpos + endlen);
  return true;
}
//----------------------------------------------------------------------------
// * returns everything up to the end
bool jfStringParser::GetToEnd(QString& out) {
  if (!ready) return false;
  if (mainindex>=rawdata.length()) return false;
  out = rawdata.mid(mainindex,-1);
  return true;
}
//--------------------------------------------------------------------------------
bool jfStringParser::GetToEndULong(unsigned long& outval, QString& outerr) {
  // constants
  QString err_endfailed = "Getting to the end failed!";
  QString err_notaninteger = "The field was found, but is not an integer!";
  // local variables
  QString buffer;
  size_t oindex = mainindex;
  // we start...
  if (!GetToEnd(buffer)) {
    outerr = err_endfailed;
    mainindex = oindex;
    return false;
  }
  buffer = buffer.trimmed();
  // numeric conversion
  if (!Str2ULongC(buffer,outval)) {
    outerr = err_notaninteger;
    mainindex = oindex;
    return false;
  }
  return true;
}
//=====================================================================================
bool jfStringParser::GetDelimitedFromEnd(QString start, QString end, QString& outbuf,
        QString endmarker) {
  //local varibales
  int endmark_index,fpos;
  int nindex,startdex;
  // the endmarker
  if (endmarker.isEmpty()) endmark_index = rawdata.length()-1;
  else {
    fpos = rawdata.indexOf(endmarker,mainindex,casesen);
    if (fpos<0) return false;
    else endmark_index = fpos-1;
  }
  // searching back from the endmarker
  fpos = rawdata.lastIndexOf(end,endmark_index,casesen);
  if (fpos<0) return false;
  // we've found the last delimiter
  nindex = fpos + end.length();
  endmark_index = fpos;
  // looking for the first delimiter
  fpos = rawdata.lastIndexOf(start,endmark_index,casesen);
  if (fpos<0) return false;
  // extracting
  startdex = fpos+start.length();
  outbuf = rawdata.mid(startdex,endmark_index-startdex);
  skipped_data = rawdata.mid(mainindex,fpos-mainindex);
  // done
  oldindex = mainindex;
  mainindex = nindex;
  return true;
}
//------------------------------------------------------------------
bool jfStringParser::GetDelimitedFromEndULong(QString start, QString end,
        unsigned long& outbuf, QString& outerr, QString endmarker) {
  // constants
  const QString err_notfound = "The specified field was not found!";
  const QString err_notaninteger = "The field was found, but is not an integer!";
  // variables
  QString thevalue;
  // saving the old data
  QString old_skip = skipped_data;
  size_t ooindex = oldindex;
  size_t omindex = mainindex;
  // trying to extract
  if (!GetDelimitedFromEnd(start,end,thevalue,endmarker)) {
    outerr = err_notfound;
    return false;
  }
  thevalue = thevalue.trimmed();
  // trying to convert
  if (!Str2ULongC(thevalue,outbuf)) {
    skipped_data = old_skip;
    oldindex = ooindex;
    mainindex = omindex;
    outerr = err_notaninteger;
    return false;
  }
  else return true;
}
//------------------------------------------------------------------
bool jfStringParser::GetAfterLast(QString start, QString& outbuf, QString endmarker) {
  const QString fname = "jfStringParser::GetAfterLast";
  //local varibales
  int endmark_index,fpos;
  int startdex;
  // the endmarker
  if (endmarker.isEmpty()) {
    fpos = rawdata.lastIndexOf(start,-1,casesen);
  }
  else {
    fpos = rawdata.indexOf(endmarker,mainindex,casesen);
    if (fpos<0) return false;
    else endmark_index = fpos-1;
    fpos = rawdata.lastIndexOf(start,endmark_index,casesen);
  }
  if (fpos<0) return false;
  if (fpos<mainindex) return false;
  // we've found the first delimiter
  startdex = fpos + start.length();
  // extracting
  outbuf = rawdata.mid(startdex,endmark_index-startdex);
  skipped_data = rawdata.mid(mainindex,fpos-mainindex);
  // done
  oldindex = mainindex;
  mainindex = endmark_index+1;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// another custom method, truncates the search after an endmarker to simplify things
bool jfStringParser::ChopAfter(QString endmark, bool chopmarker) {
  // local variables
  int endlen, endindex, fpos;
  // special case
  if (endmark.isEmpty()) return false;
  // starting
  fpos = rawdata.indexOf(endmark,mainindex,casesen);
  if (fpos<0) return false;
  // we have found, we truncate
  if (chopmarker) endindex = fpos;
  else {
    endlen = endmark.length();
    endindex = fpos+endlen;
  }
  rawdata.truncate(endindex);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// nested block extract methods
//-----------------------------------------
bool jfStringParser::GetRestOfBlock(const QString& blockstart, const QString& blockend, QString& outbuf) {
    // start checks...
    assert(!blockstart.isEmpty());
    assert(!blockend.isEmpty());
    assert(blockstart != blockend);
    if (!ready) return false;
    if (mainindex>=rawdata.length()) return false;
    // variables
    size_t whichfound,wherefound;
    size_t startindex = mainindex;
    int level = 0;
    bool xvalid = false;
    // looping, logging starts and ends
    while (level >= 0) {
        xvalid = Find1stPair(blockstart,blockend,whichfound,wherefound);
        // the block must be terminated, end of file is an error
        if (!xvalid) break;
        // otherwise, we set the level and move the index
        if (whichfound == 1) {
            level++;
            mainindex = wherefound + blockstart.length();
        }
        else {
            level--;
            mainindex = wherefound + blockend.length();

        }
    }
    // at the end, have we found the end of the block?
    if (!xvalid) {
        mainindex = startindex;
        return false;
    }
    // wwe have found the end of the block, now we extract!
    else {
        outbuf = rawdata.mid(startindex,mainindex-startindex-blockend.length());
        oldindex = startindex;
        return true;
    }
}
//-----------------------------------------
bool jfStringParser::GetRestofBlockChar(QChar bstart, QChar bend, QString& outbuf) {
    // start checks...
    assert(bstart != bend);
    if (mainindex>=rawdata.length()) return false;
    if (!ready) return false;
    // variables
    size_t currdex = mainindex;
    QChar currchar;
    int level = 0;
    bool xvalid;
    // looping, character by character
    while (level >= 0) {
        // premature end check
        if (currdex>=rawdata.length()) {
            xvalid = false;
            break;
        }
        // checking the current character
        currchar = rawdata[(uint)currdex];
        if (currchar == bstart) {
            level++;
            xvalid = true;
        }
        else if (currchar == bend) {
            level--;
            xvalid = false;
        }
        else xvalid = false;
        // moving to the next character
        currdex++;
    }
    // at the end, have we found the end of the block?
    if (!xvalid) return false;
    // wwe have found the end of the block, now we extract!
    else {
        oldindex = mainindex;
        outbuf = rawdata.mid(mainindex,mainindex-currdex-1);
        mainindex = currdex;
        return true;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// line extract methods
//-----------------------------------------
bool jfStringParser::GetLine(QString& outbuf) {
    if (!ready) return false;
    if (mainindex>=rawdata.length()) return false;
    // initial
    int rpos = rawdata.indexOf(xendl,mainindex);
    if (rpos < 0) return false;
    outbuf = rawdata.mid(mainindex,rpos-mainindex);
    // calculating if we skip past one or two characters...
    oldindex = mainindex;
    size_t skipamount = CalcSkipAmount(rpos);
    mainindex = rpos + skipamount;
    return true;
}
//-----------------------------------------
bool jfStringParser::GetLineOrTo(const QString& end, QString& outbuf, bool& endfound) {
    if (!ready) return false;
    if (mainindex>=rawdata.length()) return false;
    // looking for the positions...
    int rpos1 = rawdata.indexOf(xendl,mainindex);
    int rpos2 = rawdata.indexOf(end,mainindex);
    size_t endpos;
    // checking the results
    if ((rpos1<0) && (rpos2<0)) return false;
    if (rpos1<0) {
        endfound = true;
        endpos = rpos2;
    }
    else if (rpos2<0) {
        endfound = false;
        endpos = rpos1;
    }
    else {
        if (rpos1 < rpos2) {
            endfound = false;
            endpos = rpos1;
        }
        else {
            endfound = true;
            endpos = rpos2;
        }
    }
    // extracting the string...
    outbuf = rawdata.mid(mainindex,endpos-mainindex);
    // figuring out how much we skip...
    size_t skipamount;
    if (endfound) skipamount = end.length();
    else skipamount = CalcSkipAmount(endpos);
    // finishing the indexes
    oldindex = mainindex;
    mainindex = endpos + skipamount;
    return true;
}
//-----------------------------------------
bool jfStringParser::GetLineOrToChar(const QChar end, QString& outbuf, bool& endfound) {
    if (!ready) return false;
    if (mainindex>=rawdata.length()) return false;
    // looking for the positions for the end line
    int rpos1 = rawdata.indexOf(xendl,mainindex);
    int stoppos;
    if (rpos1 < 0) stoppos = rawdata.length();
    else stoppos = rpos1;
    // now, we look for the character
    bool foundex = false;
    size_t chardex = mainindex;
    QChar currchar;
    while (chardex<stoppos) {
        currchar = rawdata[(uint)chardex];
        if (currchar == end) {
            foundex = true;
            break;
        }
        chardex++;
    }
    // check the results... and extract the string...
    if ((!foundex) && (rpos1<0)) return false;
    if (foundex) {
        // the character end is first
        outbuf = rawdata.mid(mainindex,chardex-mainindex);
        oldindex = mainindex;
        mainindex = chardex+1;
    }
    else {
        // the end of the line is first
        outbuf = rawdata.mid(mainindex,rpos1-mainindex);
        // figuring out how much to skip
        size_t skipamount = CalcSkipAmount(rpos1);
        oldindex = mainindex;
        mainindex = rpos1 + skipamount;
    }
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// skip and not skip whitespace
//-------------------------------------
bool jfStringParser::SkipWhitespace() {
    // initial checks
    if (!ready) return false;
    if (mainindex>=rawdata.length()) return false;
    QChar currchar;
    size_t currdex = mainindex;
    // incrementing loop
    while (currdex < rawdata.length()) {
        currchar = rawdata[(uint)currdex];
        if (!currchar.isSpace()) break;
        currdex++;
    }
    // false if nothing has been skipped
    if (currdex == mainindex) return false;
    oldindex = mainindex;
    mainindex = currdex;
    return true;
}
//-------------------------------------
bool jfStringParser::GetUntilWhitespace(QString& outbuf) {
    // initial checks
    if (!ready) return false;
    if (mainindex>=rawdata.length()) return false;
    QChar currchar;
    size_t currdex = mainindex;
    // incrementing loop
    while (currdex < rawdata.length()) {
        currchar = rawdata[(uint)currdex];
        if (currchar.isSpace()) break;
        currdex++;
    }
    // false if nothing has been skipped
    if (currdex == mainindex) return false;
    outbuf = rawdata.mid(mainindex,currdex-mainindex);
    oldindex = mainindex;
    mainindex = currdex;
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfStringParser::CalcSkipAmount(const size_t& lpos) const {
    size_t skipamount = 1;
    assert(lpos<rawdata.length());
    QChar nlchar = rawdata[(uint)lpos];
    if (lpos < (rawdata.length()-1)) {
        QChar nlchar2 = rawdata[(uint)(lpos+1)];
        if ((nlchar=='\r') && (nlchar2=='\n')) skipamount = 2;
        else if ((nlchar=='\n') && (nlchar2=='\r')) skipamount = 2;
    }
    return skipamount;
}
// -------------------------
bool jfStringParser::CheckRegex(const QRegularExpression& toMatch) const {
    if (!ready) return false;
    else if (toMatch.isValid() == false) return false;
    else if (mainindex >= rawdata.length()) return false;
    else return true;
}

/*****************************************************************************/
