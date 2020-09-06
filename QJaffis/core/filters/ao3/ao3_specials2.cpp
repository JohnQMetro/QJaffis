/******************************************************************************
Name    :   ao3_specials2.cpp
Author  :   John Q Metro
Purpose :   AO3 pairing and warning filters
Created :   October 4, 2012
Conversion to Qt Started Oct 2, 2013
Updated :   September 6, 2020 (Revise pair filter)
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
}
//----------------------------
jfAO3Pairing::jfAO3Pairing(const jfAO3Pairing& source) {
    first = source.first;
    second = source.second;
    is_platonic = source.is_platonic;
}
//----------------------------
jfAO3Pairing::jfAO3Pairing(const QString& A, const QString& B, const bool in_platonic ) {
    first = A.toLower();
    second = B.toLower();
    is_platonic = in_platonic;
}

QString jfAO3Pairing::toString() const {
    return first + ((is_platonic)? '&' : '/') + second;
}
//----------------------------
jfAO3Pairing* jfAO3Pairing::ParsePairing(const QString& source) {
    int slpos = source.indexOf(sl);
    int ampos = source.indexOf(am);
    if ((slpos < 0) && (ampos < 0)) return NULL;
    // getting the split position and type
    int pos_split = 0;
    if (slpos < 0) pos_split = ampos;
    else if (ampos < 0) pos_split = slpos;
    else pos_split = (slpos < ampos) ? slpos : ampos;
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
    bool matchX = target.first.contains(pattern.first);
    if (!matchX) {
        if (!either) return false;
        cross_match = target.second.contains(pattern.first);
        if (!cross_match) return false;
    }
    if (!cross_match) {
        if (!target.second.contains(pattern.second)) return false;
    }
    else {
        if (!target.first.contains(pattern.second)) return false;
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
}
//+++++++++++++++++++++++++++++++++++++++++++
// public methods implemented here
//---------------------------
bool jfAO3PairFilter::isEmpty() const {
  if (pnames==NULL) return true;
  else return (pnames->isEmpty());
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
  QVector<jfAO3Pairing*>* nnames = NULL;
  // parsing the input
  QString qnames = sourcedata.trimmed();
  if (qnames.isEmpty()) return false;
  nnames = ParsePairs(qnames);
  if (nnames == NULL) return false;
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
  /**/JDEBUGLOG(fname,2)
  if (parsedinput == NULL) return false;
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
