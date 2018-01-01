/******************************************************************************
Name    :   ao3_specials2.cpp
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   October 12 2012
******************************************************************************/
#ifndef AO3_SPECIALS2_H_INCLUDED
  #include "ao3_specials2.h"
#endif // AO3_SPECIALS2_H_INCLUDED
//--------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-----------------------------------------------
#include <assert.h>
#include <QRegExp>
//---------------------------------
const QString warn3_ac = "VDRPN";
const QString warnlist[5] = {"Violence","Character Death","Rape","Underage Sex","Warnings Avoided"};
/*****************************************************************************/
// constructors
//---------------------------
jfAO3PairFilter::jfAO3PairFilter():jfBaseFilter() {
  pnames = NULL;
  alternate = true;
}
//------------------------------
jfAO3PairFilter::jfAO3PairFilter(const jfAO3PairFilter& source):jfBaseFilter() {
  // copying base elements
  description = source.description;
  num_id = source.num_id;
  validdata = source.validdata;
  name = source.name + ", copy";
  if (source.pnames==NULL) pnames = NULL;
  else {
    pnames = new QStringList(*source.pnames);
  }
  alternate = source.alternate;
}
//+++++++++++++++++++++++++++++++++++++++++++
// public methods implemented here
//---------------------------
bool jfAO3PairFilter::isEmpty() const {
  if (pnames==NULL) return true;
  else return (pnames->count()==0);
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// string conversion
//----------------------------------
bool jfAO3PairFilter::FromString(const QString& sourcedata) {
  validdata = false;
  if (pnames!=NULL) delete pnames;
  // parsing
  jfLineParse lparse(sourcedata);
  if (lparse.NNotX(2)) return false;
  // getting...
  if (!lparse.BoolVal(0,alternate)) return false;
  QString srcstuff = lparse.GetT(1);
  return SetNamesData(srcstuff);
}
//-------------------------------
QString jfAO3PairFilter::ToString() const {
  return Bool2Str(alternate) + ";" + GetNamesData();
}
//---------------------------------
bool jfAO3PairFilter::SetNamesData(const QString& sourcedata) {
  validdata = false;
  if (pnames!=NULL) delete pnames;
  QString qnames = sourcedata;
  qnames = qnames.trimmed();
  pnames = ParsePairs(qnames);
  if ((pnames==NULL) && (!qnames.isEmpty())) return false;
  validdata = true;
  return true;
}
//---------------------------------
QString jfAO3PairFilter::GetNamesData() const {
  if (pnames==NULL) return "";
  size_t pcount,pindex;
  QString result;
  // starting
  pcount = GetPairs();
  for (pindex=0;pindex<pcount;pindex++) {
    if (!result.isEmpty()) result += ",";
    result += pnames->at(2*pindex);
    result += "/";
    result += pnames->at(2*pindex+1);
  }
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++
// gets a description
QString jfAO3PairFilter::GetTypeDescription() const {
  return "Matches against the list of pairings that comes with an \
Archiveofourown.org fic. The filter contains lists of string pairs, like \
'harry' and 'ginny', at least one of the pairings must match one specified \
by the fic (unless the filter is empty).";
}
//-------------------------------
// copy
jfBaseFilter* jfAO3PairFilter::GenCopy() const {
  jfAO3PairFilter* newfilt;
  newfilt = new jfAO3PairFilter();
  CopyOver(newfilt);
  if (pnames!=NULL) {
    newfilt->pnames = new QStringList(*pnames);
  }
  return newfilt;
}
//------------------------------
// special meta-information
QString jfAO3PairFilter::GetTypeID() const {
  return "AO3PairFilter";
}
//------------------------------
// other methods
size_t jfAO3PairFilter::GetPairs() const {
  size_t psize;
  if (pnames==NULL) return 0;
  psize = (pnames->count()) / 2;
  return psize;
}
//--------------------------------
void jfAO3PairFilter::SetAlternates(bool inval) {
  alternate = inval;
}
//----------------------------------
bool jfAO3PairFilter::GetAlternate() const {
  return alternate;
}
//--------------------------------
// destructor
jfAO3PairFilter::~jfAO3PairFilter() {
  if (pnames!=NULL) delete pnames;
}
//+++++++++++++++++++++++++++++++++++++++++++
bool jfAO3PairFilter::CoreMatch(const jfBasePD* testelem) const {
  const QString fname = "jfAO3PairFilter::CoreMatch";
  // variables and test
  const jfAO3Fanfic* temp;
  QStringList* parsedinput;
  assert(testelem!=NULL);
  size_t oloop,iloop,olmax,ilmax;
  QString test1,test2,check2;
  bool isodd,found;
  size_t foundcount;
  // starting...
  /**/JDEBUGLOG(fname,1)
  temp = dynamic_cast<const jfAO3Fanfic*>(testelem);
  parsedinput = ParsePairs(temp->GetRelationships());
  if (parsedinput==NULL) return false;
  // matching loop prep
  olmax = GetPairs();
  ilmax = parsedinput->count();
  found = false;
  foundcount = 0;
  // outer matching loop.. using the filter info
  for (oloop=0;oloop<olmax;oloop++) {
    test1 = pnames->at(2*oloop);
    // the inner loop
    for (iloop = 0;iloop<ilmax;iloop++) {
      if ((parsedinput->at(iloop)).contains(test1)) {
        // we've found one name, we we check for the other
        isodd = (iloop&1);
        test1 = pnames->at(2*oloop+1);
        if (isodd) check2 = parsedinput->at(iloop-1);
        else check2 = parsedinput->at(iloop+1);
        found = check2.contains(test1);
        if ((!alternate) && found) foundcount++;
        if (found) break;
      }
    }
    if (alternate && found) break;
  }
  // after the loop, cleaning up and returning
  delete parsedinput;
  if (!alternate) found = (foundcount==olmax);
  return found;
}
//-------------------------------------------
QStringList* jfAO3PairFilter::ParsePairs(const QString& inval) const {
  const QString fname = "jfAO3PairFilter::ParsePairs";
  // variables
  QStringList* result;
  QString tinval,buffer;
  size_t startpos;
  int fpos;
  bool odd;
  QChar scar;
  // starting...
  tinval = inval.toLower();
  startpos = 0;
  result = new QStringList();
  odd = true;
  /**/JDEBUGLOGS(fname,1,tinval)
  // extraction loop
  while (true) {
    scar = (odd)?('/'):(',');
    /**/JDEBUGLOGS(fname,2,scar)
    // extracting
    fpos = tinval.indexOf(scar,startpos);
    if (fpos==-1) break;
    buffer = tinval.mid(startpos,fpos-startpos);
    buffer = buffer.trimmed();
    /**/JDEBUGLOGS(fname,3,buffer)
    // do not tolerate empty parts
    if (buffer.isEmpty()) {
      delete result;
      return NULL;
    }
    // finishing
    result->append(buffer);
    startpos = fpos + 1;
    // next one...
    odd = !odd;
    /**/JDEBUGLOGB(fname,4,odd)
  }
  // the last part
  buffer = tinval.mid(startpos);
  buffer = buffer.trimmed();
  /**/JDEBUGLOGS(fname,5,buffer)
  if (buffer.isEmpty()) {
    delete result;
    return NULL;
  }
  result->append(buffer);
  // testing the size
  startpos = result->count();
  /**/JDEBUGLOGST(fname,4,startpos)
  if (startpos&1) {
    delete result;
    return NULL;
  }
  /**/JDEBUGLOG(fname,5);
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++
// file i/o
//---------------------------
bool jfAO3PairFilter::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding
  (*outfile) << ToString() << "\n";
  return true;
}
//---------------------------
bool jfAO3PairFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfAO3PairFilter::ReadRestFromFile";
  // input data
  QString cline;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  // parsing the line
  if (!infile->ReadLine(cline,funcname)) return false;
  if (!FromString(cline)) return infile->BuildError("The characters do not match the filter!");
  return true;
}
//---------------------------
size_t jfAO3PairFilter::ExtraLines() const { return 1; }
//*************************************************************************
jfAO3WarnFilter::jfAO3WarnFilter():jfSpecialsFilter()  {}
//---------------------------
jfAO3WarnFilter::jfAO3WarnFilter(const QString& insrc):jfSpecialsFilter() {
  FromString(insrc);
}
//---------------------------
// string conversion
bool jfAO3WarnFilter::FromString(const QString& sourcedata) {
  QRegExp xrpos = QRegExp("[^"+warn3_ac+"]");
  if (!sourcedata.contains(xrpos)) {
    value = sourcedata;
    validdata = true;
  }
  else validdata = false;
  return validdata;
}

//---------------------------
// gets a description
QString jfAO3WarnFilter::GetTypeDescription() const {
  return "Excludes fics based on whether they have the \
specified archive warnings.";
}
//---------------------------
// copy
jfBaseFilter* jfAO3WarnFilter::GenCopy() const {
  jfAO3WarnFilter* qval = new jfAO3WarnFilter(value);
  return qval;
}
//---------------------------
// special meta-information
QString jfAO3WarnFilter::GetTypeID() const {
  return "AO3WarnFilter";
}
//---------------------------
// custom methods
void jfAO3WarnFilter::SetToFull() {
 value = warn3_ac;
}
//-----------------------------
bool jfAO3WarnFilter::IsFull() const {
  return value==warn3_ac;
}
//---------------------------
// the core matching method
bool jfAO3WarnFilter::CoreMatch(const jfBasePD* testelem) const {
  const QString fname = "jfAO3WarnFilter::CoreMatch";
  QRegExp qpval = QRegExp("["+value+"]");
  // starting
  const jfAO3Fanfic* temp = dynamic_cast<const jfAO3Fanfic*>(testelem);
  QChar warn1 = temp->GetWarnflag();
  QString warn2 = temp->GetWarntag();
  // contains warings
  if (warn1=='W') {
    if (warn2.contains(qpval)) return false;
    else return true;
  }
  else if (warn1=='N') {
    if (!value.contains(warn1)) return false;
    else return true;
  }
  else return true;
}
/*****************************************************************************/
