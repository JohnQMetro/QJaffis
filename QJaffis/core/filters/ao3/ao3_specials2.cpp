/******************************************************************************
Name    :   ao3_specials2.cpp
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   June 25, 2022 (Revise pair filter)
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
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//-----------------------------------------------
#include <assert.h>
#include <QRegExp>
//---------------------------------
const QString warn3_ac = "VDRPN";
const QString warnlist[5] = {"Violence","Character Death","Rape","Underage Sex","Warnings Avoided"};
const QChar sl = QChar('/');
const QChar am = QChar('&');
/*****************************************************************************/
jfAO3Pairing::jfAO3Pairing() {
    first = ""; second = ""; is_platonic = false;
    reader = false; original = false;
}
//----------------------------
jfAO3Pairing::jfAO3Pairing(const jfAO3Pairing& source) {
    first = source.first;
    second = source.second;
    is_platonic = source.is_platonic;
    reader = source.reader;
    original = source.original;
}
//----------------------------
jfAO3Pairing::jfAO3Pairing(const QString& A, const QString& B, const bool in_platonic ) {
    QString Alc = A.toLower();
    QString Blc = B.toLower();
    first = A;
    second = B;
    is_platonic = in_platonic;
    // checking reader and original
    reader = (Alc == "reader") || (Alc == "you") || Alc.endsWith(" reader");
    if (!reader) reader = (Blc == "reader") || (Blc == "you") || (Blc.endsWith(" reader"));
    original = Alc.startsWith("original") || (Alc == "ofc") || (Alc == "omc");
    if (!original) original = Blc.startsWith("original") || (Blc == "ofc") || (Blc == "omc");
}

QString jfAO3Pairing::toString() const {
    return first + ((is_platonic)? '&' : '/') + second;
}
//----------------------------
int jfAO3Pairing::posMin(int a, int b) {
    if (a < 0) return b;
    else if (b < 0) return a;
    else return (a < b) ? a : b;
}
//----------------------------
jfAO3Pairing* jfAO3Pairing::ParsePairing(const QString& source) {

    int slpos = source.indexOf(sl);  // /
    int ampos = source.indexOf(am); // &
    int slpos2 = source.indexOf(" x ",Qt::CaseInsensitive);
    if ((slpos < 0) && (ampos < 0) && (slpos2 < 0)) return NULL;

    // getting the split position and type
    int pos_split = posMin(posMin( slpos, ampos), slpos2);
    bool is_plat = (pos_split == ampos);

    // extracting the parts
    QString partA = source.left(pos_split).trimmed();
    if (partA.isEmpty()) return NULL;
    QString partB = source.mid(pos_split+1).trimmed();
    if (partB.isEmpty()) return NULL;
    // finishing
    return new jfAO3Pairing(partA,partB,is_plat);
}

//---------------------------------------
bool jfAO3Pairing::ComparePairing(const jfAO3Pairing& pattern,const jfAO3Pairing& target, bool notype, bool either) {
    const QString fname = "jfAO3Pairing::ComparePairing";
    bool cross_match = false;
    // the pattern names must be contained by the target names
    bool matchX = target.first.contains(pattern.first, Qt::CaseInsensitive);
    if (!matchX) {
        if (!either) return false;
        cross_match = target.second.contains(pattern.first, Qt::CaseInsensitive);
        if (!cross_match) return false;
    }
    if (!cross_match) {
        if (!target.second.contains(pattern.second, Qt::CaseInsensitive)) return false;
    }
    else {
        if (!target.first.contains(pattern.second, Qt::CaseInsensitive)) return false;
    }
    // if we get here, we might still need to match the type
    if (notype) return true;
    else return (pattern.is_platonic == target.is_platonic);
}

//=============================================================================
// constructors
//---------------------------
jfAO3PairFilter::jfAO3PairFilter():jfBaseFilter() {
  pnames = NULL;
  alternate = true;
  match_reader = false;
  match_original = false;
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
      pnames = new QVector<jfAO3Pairing*>(source.pnames->size());
      for (int pdex = 0; pdex < source.pnames->size(); pdex++) {
          pnames->replace(pdex,new jfAO3Pairing(*(source.pnames->at(pdex))));
      }
  }
  alternate = source.alternate;
  match_reader = source.match_reader;
  match_original = source.match_original;
}
//+++++++++++++++++++++++++++++++++++++++++++
// public methods implemented here
//---------------------------
bool jfAO3PairFilter::isEmpty() const {
  if ((pnames != NULL) && (!pnames->isEmpty())) return false;
  return !(match_reader || match_original);
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// string conversion
//----------------------------------
bool jfAO3PairFilter::FromString(const QString& sourcedata) {
    const QString fname = "jfAO3PairFilter::FromString";
  validdata = false;
  if (pnames!=NULL) delete pnames;
  // parsing
  jfLineParse lparse(sourcedata);
  size_t nx = lparse.Num();
  if ((nx < 2) || (nx > 4)) return false;
  // getting...
  if (!lparse.BoolVal(0,alternate)) return false;
  QString srcstuff = lparse.GetT(1);
  if (!SetNamesData(srcstuff)) return false;
  if (!lparse.BoolVal(2,match_reader)) return false;
  return lparse.BoolVal(3,match_original);
}
//-------------------------------
QString jfAO3PairFilter::ToString() const {
    QString ox = Bool2Str(alternate) + ";" + GetNamesData() + ";";
    return ox + Bool2Str(match_reader) + ";" + Bool2Str(match_original);
}
//---------------------------------
bool jfAO3PairFilter::SetNamesData(const QString& sourcedata) {
  validdata = false;
  QVector<jfAO3Pairing*>* nnames = NULL;
  // parsing the input
  QString qnames = sourcedata.trimmed();
  if (qnames.isEmpty()) {
      nnames = new QVector<jfAO3Pairing*>();
  }
  else {
      nnames = ParsePairs(qnames);
      if (nnames == NULL) return false;
  }

  // setting the input
  if (pnames!=NULL) delete pnames;
  pnames = nnames;
  validdata = true;
  return true;
}
//---------------------------------
QString jfAO3PairFilter::GetNamesData() const {
  if (pnames==NULL) return "";
  QString result = "";
  // looping over the pairs
  for (int pindex = 0; pindex < pnames->size(); pindex++) {
    if (!result.isEmpty()) result += ",";
    result += (pnames->at(pindex))->toString();
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

  if (pnames != NULL) {
      newfilt->pnames = new QVector<jfAO3Pairing*>(pnames->size());
      for (int pdex = 0; pdex < pnames->size(); pdex++) {
          (newfilt->pnames)->replace(pdex,new jfAO3Pairing(*(pnames->at(pdex))));
      }
  }
  newfilt->alternate = alternate;
  newfilt->match_reader = match_reader;
  newfilt->match_original = match_original;
  return newfilt;
}
//------------------------------
// special meta-information
QString jfAO3PairFilter::GetTypeID() const {
  return "AO3PairFilter";
}
//--------------------------------
void jfAO3PairFilter::SetAlternates(bool inval) {
  alternate = inval;
}
//----------------------------------
bool jfAO3PairFilter::GetAlternate() const {
  return alternate;
}
//----------------------------------
void jfAO3PairFilter::SetMatchReader(bool inval) {
    match_reader = inval;
}

//----------------------------------
bool jfAO3PairFilter::GetMatchReader() const {
    return match_reader;
}
//----------------------------------
void jfAO3PairFilter::SetMatchOriginal(bool inval) {
    match_original = inval;
}
//----------------------------------
bool jfAO3PairFilter::GetMatchOriginal() const {
    return match_original;
}
//--------------------------------
// destructor
jfAO3PairFilter::~jfAO3PairFilter() {
  if (pnames!=NULL) {
      jfAO3PairFilter::EmptyPairList(*pnames);
      delete pnames;
      pnames = NULL;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++
void jfAO3PairFilter::EmptyPairList(QVector<jfAO3Pairing*>& plist) {
    for (int pdex = 0; pdex < plist.size(); pdex++) {
        if (plist[pdex] != NULL) {
            delete plist[pdex];
            plist[pdex] = NULL;
        }
    }
    plist.clear();
}

//-------------------------------------------
bool jfAO3PairFilter::CoreMatch(const jfBasePD* testelem) const {
  const QString fname = "jfAO3PairFilter::CoreMatch";
  // variables and test
  const jfAO3Fanfic* temp;
  QVector<jfAO3Pairing*>* parsedinput = NULL;
  assert(testelem!=NULL);
  jfAO3Pairing* test1 = NULL;
  jfAO3Pairing* test2 = NULL;
  int foundcount = 0;
  bool found = false;

  // starting...
  /**/JDEBUGLOG(fname,1)
  temp = dynamic_cast<const jfAO3Fanfic*>(testelem);
  parsedinput = ParsePairs(htmlparse::HTML2Text(temp->GetRelationships()));
  if (parsedinput == NULL) return false;
  // we first check vs the special flags
  if (match_original || match_reader) {
      for (int qloop = 0; qloop < (parsedinput->size()); qloop++) {
          test2 = parsedinput->at(qloop);
          if (match_original && (test2->original)) {
              found = true;
              break;
          }
          if (match_reader && (test2->reader)) {
              found = true;
              break;
          }
      }
      if (found) {
          jfAO3PairFilter::EmptyPairList(*parsedinput);
          delete parsedinput;
          return true;
      }
  }


  /**/JDEBUGLOG(fname,2)
  if ((pnames == NULL) || (pnames->isEmpty())) return false;
  // outer matching loop.. using the filter info
  for (int oloop = 0; oloop < (pnames->size()); oloop++) {
      found = false;
      test1 = pnames->at(oloop);
      // the inner loop
      for (int iloop = 0; iloop < (parsedinput->size()); iloop++) {
          test2 = parsedinput->at(iloop);
          found = jfAO3Pairing::ComparePairing(*test1,*test2,false,false);
          // 'alternate' is true if we need only one match
          if (found) {
              if (!alternate) foundcount++;
              break;
          }
      }
      if (alternate && found) break;
  }
  // after the loop, cleaning up and returning
  jfAO3PairFilter::EmptyPairList(*parsedinput);
  delete parsedinput;
  if (!alternate) found = (foundcount == (pnames->size()));
  return found;
}
//-------------------------------------------
QVector<jfAO3Pairing*>* jfAO3PairFilter::ParsePairs(const QString& inval) const {
  const QString fname = "jfAO3PairFilter::ParsePairs";
  // initial trimming and splitting
  QString clist = inval.trimmed();
  /**/JDEBUGLOGS(fname,1,clist)
  if (clist.isEmpty()) return NULL;
  QStringList slist = clist.split(",");
  // making the result
  QVector<jfAO3Pairing*>* res = new QVector<jfAO3Pairing*>();
  res->reserve(slist.size());
  /**/JDEBUGLOGI(fname,2,slist.size())
  // looping and converting the list
  jfAO3Pairing* phold = NULL;
  for (int sdex = 0; sdex < slist.count(); sdex++) {
      phold = jfAO3Pairing::ParsePairing(slist[sdex]);
      if (phold == NULL) continue;
      res->append(phold);
  }
  /**/JDEBUGLOGI(fname,3,res->size())
  // we return NULL if the list is empty
  if (res->isEmpty()) {
      delete res;
      return NULL;
  }
  /**/JDEBUGLOG(fname,4)
  return res;
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
