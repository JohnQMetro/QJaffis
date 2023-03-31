//***************************************************************************
// Name:        pairfilter_tools.cpp
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters are complicated enough to make a
//              separate source file for utilities and data
// Created:     December 29, 2010
// Conversion to QT Started April 11, 2013
// Updated:     May 29, 2012
//***************************************************************************
#ifndef PAIRFILTER_TOOLS_H_INCLUDED
  #include "pairfilter_tools.h"
#endif // PAIRFILTER_TOOLS_H_INCLUDED
//---------------------------------------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../../utils/utils2.h"
#endif // UTILS1_H_INCLUDED
#ifndef JFEXPRBASE
  #include "../../express/expr_base.h"
#endif
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../utils/utils3.h"
#endif // UTILS3_H_INCLUDED

#include <assert.h>
//***************************************************************************
// a method to strip duplicate characters from a string
bool StripDuplicateChars(QString& delm_str) {
  // variables
  QString result;
  size_t str_len;
  uint str_loop;
  // start
  delm_str = delm_str.trimmed();
  if (delm_str.isEmpty()) return false;
  else {
    // here, we really start, the technique is a copying loop that checks
    // if a char is already in the result before copying
    str_len = delm_str.length();
    for (str_loop=0;str_loop<str_len;str_loop++) {
      if (!result.contains(delm_str[str_loop])) result+= delm_str[str_loop];
    }
    delm_str = result;
  }
  return true;
}
//==========================================================================
jfPF_flaghold::jfPF_flaghold() {
  usetokens = false;
  sidespaces = true;
  nofriend = true;
  nospace = true;
  curr_char = '/';
}
//-------------------------------------------------------------------------
// to and from string
//--------------------------
QString jfPF_flaghold::ToString() const {
  jfOutString result;
  result << usetokens << sidespaces << nofriend << nospace;
  return QString(result);
}
//---------------------------------------------
bool jfPF_flaghold::FromString(QString inval) {
  jfLineParse parser;
  // basic parsing
  parser.SetString(inval);
  if (parser.NNotX(4)) return false;
  // field extraction
  if (!parser.BoolVal(0,usetokens)) return false;
  if (!parser.BoolVal(1,sidespaces)) return false;
  if (!parser.BoolVal(2,nofriend)) return false;
  if (!parser.BoolVal(3,nospace)) return false;
  // done
  return true;
}
//==========================================================================
/* used in the following function.. very restricted, basically handles
checking a single set, with tokenized vs not, and frienship excluding
*/
bool LookForPatternString(QString lookin, QString pattern, const jfPF_flaghold& flags) {
  // variables
  jfSubstringElem* slooker;
  bool result;
  // starting...
  assert(!flags.usetokens);
  slooker = new jfSubstringElem(pattern);
  result = slooker->Match(lookin);
  delete slooker;
  if (result && flags.nofriend) {
    pattern += " friendship";
    slooker = new jfSubstringElem(pattern);
    result = slooker->Match(lookin);
    delete slooker;
    return (!result);
  }
  else return result;
}
//----------------------------------------------------------------------------------
bool LookForPatternToken(const QStringList* lookin, QString pattern, const jfPF_flaghold& flags) {
  // variables
  jfTokenMElem*    tlooker;
  bool result;
  // starting...
  tlooker = new jfTokenMElem(pattern);
  result = tlooker->Match(lookin);
  delete tlooker;
  if (result && flags.nofriend) {
    pattern += " friendship";
    tlooker = new jfTokenMElem(pattern);
    result = tlooker->Match(lookin);
    delete tlooker;
    return (!result);
  }
  else return result;

}
//--------------------------------------------------------------------------
// function to look for a name pair using one character
bool LookForPairSingleString(QString lookin, QString name1, QString name2, const jfPF_flaghold& flags) {
  // variables
  QString lookstring;
  bool rval;
  // special cases
  assert(!name1.isEmpty());
  assert(!name2.isEmpty());
  assert(!flags.usetokens);
  if (lookin.isEmpty()) return false;
  // case 1 : name1 + char + name2
  lookstring = name1 + flags.curr_char + name2;
  rval = LookForPatternString(lookin,lookstring,flags);
  if (rval) return true;
  // case 2 : name2 + char + name1
  lookstring = name2 + flags.curr_char + name1;
  rval = LookForPatternString(lookin,lookstring,flags);
  if (rval) return true;
  // cases 3 and 4 are done if we have sidespaces
  if (flags.sidespaces) {
    // case 3 : name1 + space + char + space + name2
    lookstring = name1 + " ";
    lookstring += flags.curr_char;
    lookstring += " " + name2;
    rval = LookForPatternString(lookin,lookstring,flags);
    if (rval) return true;
    // case 2 : name2 + space + char + space + name1
    lookstring = name2 + " ";
    lookstring += flags.curr_char;
    lookstring += " " + name1;
    rval = LookForPatternString(lookin,lookstring,flags);
    if (rval) return true;
  }
  // done with nothing found!
  return false;
}
//---------------------------------------------------------------------------------------------
// function to look for a name pair using one character
bool LookForPairSingleToken(const QStringList* lookin, QString name1, QString name2, const jfPF_flaghold& flags) {
  // variables
  QString lookstring;
  bool rval;
  // special cases
  assert(!name1.isEmpty());
  assert(!name2.isEmpty());
  if (lookin==NULL) return false;
  if (lookin->isEmpty()) return false;
  assert(flags.usetokens);
  // case 1 : name1 + char + name2
  lookstring = name1 + flags.curr_char + name2;
  rval = LookForPatternToken(lookin,lookstring,flags);
  if (rval) return true;
  // case 2 : name2 + char + name1
  lookstring = name2 + flags.curr_char + name1;
  rval = LookForPatternToken(lookin,lookstring,flags);
  if (rval) return true;
  // cases 3 and 4 are done if we have sidespaces
  if (flags.sidespaces) {
    // case 3 : name1 + space + char + space + name2
    lookstring = name1 + " ";
    lookstring += flags.curr_char;
    lookstring += " " + name2;
    rval = LookForPatternToken(lookin,lookstring,flags);
    if (rval) return true;
    // case 2 : name2 + space + char + space + name1
    lookstring = name2 + " ";
    lookstring += flags.curr_char;
    lookstring += " " + name1;
    rval = LookForPatternToken(lookin,lookstring,flags);
    if (rval) return true;
  }
  // done with nothing found!
  return false;
}
//---------------------------------------------------------------------------------------------
// functions to look for a name pair via multiple characters
//---------------------------------------------------------------------------------------------
bool LookForPairString(QString lookin, QString name1, QString name2, QString inchars, jfPF_flaghold& flags) {
  // variables
  size_t clen;
  uint cloop;
  bool rval;
  QString looker;
  // checks
  assert(!name1.isEmpty());
  assert(!name2.isEmpty());
  assert(!flags.usetokens);
  if (lookin.isEmpty()) return false;
  // starting
  clen = inchars.length();
  // the main loop
  for (cloop=0;cloop<clen;cloop++) {
    flags.curr_char = inchars[cloop];
    rval = LookForPairSingleString(lookin,name1,name2,flags);
    if (rval) return true;
  }
  // at the end, there is another option, no separation at all
  if (flags.nospace) {
    flags.curr_char = '\0';
    looker = name1+name2;
    rval = LookForPatternString(lookin,looker,flags);
    if (rval) return true;
    looker = name2+name1;
    rval = LookForPatternString(lookin,looker,flags);
    return rval;
  }
  else return false;
}
//---------------------------------------------------------------------------------------------
bool LookForPairToken(const QStringList* lookin, QString name1, QString name2, QString inchars, jfPF_flaghold& flags) {
  // variables
  size_t clen;
  uint cloop;
  bool rval;
  QString looker;
  // checks
  assert(!name1.isEmpty());
  assert(!name2.isEmpty());
  assert(flags.usetokens);
  assert(lookin!=NULL);
  if (lookin->isEmpty()) return false;
  // starting
  clen = inchars.length();
  // the main loop
  for (cloop=0;cloop<clen;cloop++) {
    flags.curr_char = inchars[cloop];
    rval = LookForPairSingleToken(lookin,name1,name2,flags);
    if (rval) return true;
  }
  // at the end, there is another option, no separation at all
  if (flags.nospace) {
    flags.curr_char = '\0';
    looker = name1+name2;
    rval = LookForPatternToken(lookin,looker,flags);
    if (rval) return true;
    looker = name2+name1;
    rval = LookForPatternToken(lookin,looker,flags);
    return rval;
  }
  else return false;
}
//=========================================================================
bool jfPaircount::AddOrIncrement(const QString& inval) {
  if (find(inval)==end()) {
    (*this)[inval] = 1;
    return true;
  }
  else {
    ((*this)[inval]++);
    return false;
  }
}
//=========================================================================
// constructor
jfPairStats::jfPairStats(QString inchars) {
  // setting the separator list, we assume it has already been checked
  sepchars = inchars;
  // the first item in charfcount is reserved for nothing at all
  charfcount.push_back(0);
  // next, we initialize the other charfcount slots
  for(int cloop=0; cloop < inchars.length(); cloop++) charfcount.push_back(0);
  // other stuff
  maxname_length = 0;
  maxspec_length = 0;
  filename = "pairstats_default.txt";
  topname = "Default";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the data gathering method
bool jfPairStats::LogFind(QString name1, QString name2, QChar inchar) {
  // variables
  int findex = 0;
  QString logname;
  // checking which char was used for the separator
  if (inchar=='\0') charfcount[0]++;
  else {
    if (sepchars.contains(inchar)) charfcount[findex+1]++;
    else assert(false);
  }
  // next up, we log the name combo in the map
  logname = name1 + " / " + name2;
  size_t loglen = (size_t)(logname.length());
  if (loglen >maxname_length) maxname_length = loglen;
  return maindata.AddOrIncrement(logname);
}
//------------------------------------------------------------------
bool jfPairStats::LogSpecial(QString identifier) {
  assert(!identifier.isEmpty());
  size_t id_len = (size_t)(identifier.length());
  if (id_len > maxspec_length) maxspec_length = id_len;
  return specials.AddOrIncrement(identifier);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file name stuff
QString jfPairStats::GetFilename() const {
  return filename;
}
//---------------------------------------------------------------------
void jfPairStats::SetFilename(QString infilename, QString newname) {
  assert(!infilename.isEmpty());
  filename = infilename;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
void jfPairStats::WriteResults() const {
  // local variables
  jfPaircount::const_iterator iloop;
  size_t charmax, charloop;
  QTextStream* outfile;
  QString curline;
  // setting the values
  charmax = sepchars.length()+1;
  // we get the output file
  outfile = OpenResult();
  // character stats
  curline = "Separator Stats";
  WriteSecHead(curline,outfile,true);
  for (charloop=0;charloop<charmax;charloop++) {
    curline = MakeCharResult(charloop);
    (*outfile) << curline << "\n";
  }
  // specials
  curline = "Special Keyword Stats";
  WriteSecHead(curline,outfile,true);
  iloop = specials.begin();
  while (iloop!=specials.end()) {
    curline = MakeNameResult(iloop,maxspec_length);
    (*outfile) << curline << "\n";
    iloop++;
  }
  // name pairs
  curline = "Name Pair Stats";
  WriteSecHead(curline,outfile,true);
  iloop = maindata.begin();
  while (iloop!=maindata.end()) {
    curline = MakeNameResult(iloop,maxname_length);
    (*outfile) << curline << "\n";
    iloop++;
  }
  // done
  CloseFileStream(outfile,true);
  delete outfile;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//----------------------------------------------
QString jfPairStats::MakeCharResult(size_t index) const {
  assert(index <= ((size_t)sepchars.length()));
  QString result;
  // we build the label part first
  if (index==0) result = " null : ";
  else if (sepchars[(uint)index]==' ') result = "space : ";
  else {
    result = "  ";
    result += sepchars[(uint)index];
    result += "  : ";
  }
  // then we add the number
  result += QString::number(charfcount[(uint)index]);
  // done
  return result;
}
//----------------------------------------------
QString jfPairStats::MakeNameResult(const jfPaircount::const_iterator& inval, size_t padlength) const {
  // result
  QString result;
  QString cval;
  // we will assume here that inval is valid and correct
  cval = inval->first;
  // padding
  cval = cval.rightJustified(padlength);
  // building the label
  result = cval + " : ";
  // we add the number
  result += QString::number(inval->second);
  // done
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o helper methods
QTextStream* jfPairStats::OpenResult() const {
  // variables
  QTextStream* result;
  QString bufline;
  // creating or opening the file
  result = GetOpenFile(filename, true);
  // we add the header
  bufline = "Statistics for : " + topname;
  WriteSecHead(bufline,result);
  // done
  return result;
}
//--------------------------------------------------------------------------
void jfPairStats::WriteSecHead(QString inname, QTextStream* outfile_in, bool spacebefore) const {
  // variables
  QString bufline;
  // we do the usual checks
  assert(outfile_in!=NULL);
  assert(!inname.isEmpty());
  // space before, first
  if (spacebefore) (*outfile_in) << "\n";
  // the name
  (*outfile_in) << inname << "\n";
  // the -------------------
  bufline.fill('-',inname.length()+5);
  (*outfile_in) << bufline << "\n";
  // done
}

//**************************************************************************
