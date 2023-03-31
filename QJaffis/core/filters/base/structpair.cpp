/******************************************************************************
Name    :   structpair.cpp
Author  :   John Q Metro
Purpose :   'Structured Pair' filters
Created :   February 16, 2023
Updated :   March 29, 2023
******************************************************************************/

#include "structpair.h"

#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED

#ifndef FICBASE_H
  #include "../../../fanfic/data/ficbase.h"
#endif // FICBASE_H

#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
/*****************************************************************************/
/* static methods for parsing and fiddling with pairs. */
const QChar sl = QChar('/');
const QChar am = QChar('&');
const QChar dp = QChar(0x2016); // double pipe, used when storing because | or / is ambiguous.
//+++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFilterUtils::isReader(const QString& name) {
    return (name == "reader") || (name == "you") || name.endsWith(" reader");
}
// -------------------------------
bool jfPairFilterUtils::isOriginal(const QString& name) {
    return name.startsWith("original") || (name == "ofc") || (name == "omc") || (name == "oc");
}
// -------------------------------
int jfPairFilterUtils::posMin(int a, int b) {
    if (a < 0) return b;
    else if (b < 0) return a;
    else return (a < b) ? a : b;
}
//----------------------------------
/* This is used when parsing loaded from storage pairings. */
PairType jfPairFilterUtils::isStoragePlatonic(const QString& source) {
    int slpos = source.indexOf(dp);  // ‖
    int ampos = source.indexOf(am); // &
    if ((slpos < 0) && (ampos < 0)) return PairType::Unknown;

    // getting the split position and type
    int pos_split = posMin( slpos, ampos);
    return (pos_split == ampos) ? PairType::Platonic : PairType::SlashPair;
}
// ------------------------------------
/* This function check if the incoming string contains a platonic or sexual pairing.
This is not needed for Fanfiction.Net */
PairType jfPairFilterUtils::isAO3Platonic(const QString& source) {
    int slpos = source.indexOf(sl);  // /
    int ampos = source.indexOf(am); // &
    int slpos2 = source.indexOf(" x ",Qt::CaseInsensitive);
    if ((slpos < 0) && (ampos < 0) && (slpos2 < 0)) return PairType::Unknown;

    // getting the split position and type
    int pos_split = posMin(posMin( slpos, ampos), slpos2);
    if (pos_split == ampos) return PairType::Platonic;
    else if (pos_split == slpos) return PairType::SlashPair;
    else return PairType::xPair;
}
// -----------------------------------------------------
PairType jfPairFilterUtils::isFilterPlatonic(const QString& source) {
    int slpos = source.indexOf(sl);  // /
    int ampos = source.indexOf(am); // &
    if ((slpos < 0) && (ampos < 0)) return PairType::Unknown;

    // getting the split position and type
    int pos_split = posMin( slpos, ampos);
    return (pos_split == ampos) ? PairType::Platonic : PairType::SlashPair;
}
// -------------------------------
// used when parsing an AO3 Pairing
jfPairingStruct* jfPairFilterUtils::ParsePairingDisplay(const QString& source) {
    if (source.trimmed().isEmpty()) return NULL;

    PairType type = isAO3Platonic(source);
    if (type == PairType::Unknown) return new jfPairingStruct(source.trimmed());
    bool is_platonic = (type == PairType::Platonic);

    QStringList parts;
    if (type == PairType::Platonic) parts = SplitTrim(source,am);
    else if (type == PairType::xPair) parts = SplitTrim(source,QString(" x "));
    else parts = SplitTrim(source,sl);
    if (parts.size() < 2) return NULL;

    return new jfPairingStruct(parts, is_platonic);
}
// ----------------------------------
// used when parsing a pairing loaded from storage
jfPairingStruct* jfPairFilterUtils::ParsePairingStorage(const QString& source) {
    if (source.trimmed().isEmpty()) return NULL;

    PairType type = isStoragePlatonic(source);
    if (type == PairType::Unknown) return new jfPairingStruct(source.trimmed());
    bool is_platonic = (type == PairType::Platonic);

    QStringList parts;
    if (type == PairType::Platonic) parts = SplitTrim(source,am);
    else parts = SplitTrim(source,dp);
    if (parts.size() < 2) return NULL;

    return new jfPairingStruct(parts, is_platonic);
}
// -------------------------------
/* used when parsing a filter pair item (from storage or from the filter).
 * The text in the filter description says / for romatic/sexual pairs, so
 * the user had better not think they can use it as part of the name. */
jfPairFilterPair* jfPairFilterUtils::ParseFilterPair(const QString source) {
    if (source.trimmed().isEmpty()) return NULL;

    if (source.startsWith("%%")) {
        QString newSource = source.mid(2).trimmed().toLower();
        if (newSource.isEmpty()) return NULL;
        else return new jfPairFilterPair(newSource);
    }

    PairType type = isFilterPlatonic(source);
    bool is_platonic = (type == PairType::Platonic);

    QStringList parts;
    if (type == PairType::Platonic) parts = SplitTrim(source,am);
    else parts = SplitTrim(source,sl);
    if (parts.size() < 1) return NULL;
    for (int i = 0; i < parts.size(); i++) {
        parts[i] = parts.at(i).toLower();
    }

    return new jfPairFilterPair(is_platonic, parts);
}
// ===========================================================================
jfPairingStruct::jfPairingStruct() {
    is_platonic = false;
    reader = false; original = false;
}
//----------------------------
jfPairingStruct::jfPairingStruct(const jfPairingStruct& source) {
    participants = source.participants;
    is_platonic = source.is_platonic;
    reader = source.reader;
    original = source.original;
}
//----------------------------
jfPairingStruct::jfPairingStruct(const QString& A, const QString& B, const bool& in_platonic ) {
    participants.append(A);
    participants.append(B);
    QString Alc = A.toLower();
    QString Blc = B.toLower();
    is_platonic = in_platonic;
    // checking reader and original
    reader = jfPairFilterUtils::isReader(Alc) || jfPairFilterUtils::isReader(Blc);
    original = jfPairFilterUtils::isOriginal(Alc) || jfPairFilterUtils::isOriginal(Blc);
}
//----------------------------
jfPairingStruct::jfPairingStruct(const QStringList& people, const bool& in_platonic ) {
    is_platonic = in_platonic;
    reader = false;
    original = false;
    QString lc;
    for (int i = 0; i < people.size(); i++) {
        lc = people.at(i).toLower();
        reader = reader || jfPairFilterUtils::isReader(lc);
        original = original || jfPairFilterUtils::isOriginal(lc);
        if (reader && original) break;
    }
    participants = people;
}
//----------------------------
jfPairingStruct::jfPairingStruct(const QString& source_string) {
    is_platonic = false;
    reader = jfPairFilterUtils::isReader(source_string);
    original = jfPairFilterUtils::isOriginal(source_string);
    participants.append(source_string);
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfPairingStruct::toDisplayString(bool rom_pipe) const {
    if (participants.size() == 0) return QString("");
    else {
        QString result = participants.at(0);
        for (int i = 1; i < participants.size(); i++) {
            if (is_platonic) result += " & " + participants.at(i);
            else result += ((rom_pipe)? " | " : " / ") + participants.at(i);
        }
        return result;
    }
}
// -----------------------------
/* FFN uses / for alternate names, AO3 uses | for alternate names. To store pairs for both without
 * custom parsing, I need a different custom delimiter. */
QString jfPairingStruct::toStorageString() const {
    if (participants.size() == 0) return QString("");
    else {
        QString result = participants.at(0);
        for (int i = 1; i < participants.size(); i++) {
            if (is_platonic) result += "&" + participants.at(i);
            else result += QChar(0x2016) + participants.at(i);
        }
        return result;
    }
}

//----------------------------
bool jfPairingStruct:: NotAPair() const {
    return (participants.size() == 1);
}
// =======================================================================================
jfPairFilterPair::jfPairFilterPair() {
    is_platonic = false;
    not_pair = true;
}
// ---------------------------
jfPairFilterPair::jfPairFilterPair(const jfPairFilterPair& source) {
    is_platonic = source.is_platonic;
    participants = source.participants;
    not_pair = source.not_pair;
}
// ---------------------------
jfPairFilterPair::jfPairFilterPair(bool in_platonic,const QStringList& people) {
    is_platonic = in_platonic;
    participants = people;
    not_pair = false;
}
// ---------------------------
jfPairFilterPair::jfPairFilterPair(const QString& source_string) {
    is_platonic = false;
    not_pair = true;
    participants.append(source_string);
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfPairFilterPair::toString() const {
    if (participants.size() == 0) return QString("");
    else if (not_pair) {
        return "%%" + participants.at(0);
    }
    else {
        QString result = participants.at(0);
        for (int i = 1; i < participants.size(); i++) {
            result += ((is_platonic)? '&' : '/') + participants.at(i);
        }
        return result;
    }
}
bool jfPairFilterPair::NotForPairs() const {
    return not_pair;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFilterPair::IsAMatchFor(bool list_platonic, const QStringList& in_participants) const {
    if (in_participants.size() == 0) return false;
    // for 'not pairs', the in list must have 1 item
    else if (not_pair) {
        if (in_participants.size() == 1) {
            return in_participants.at(0).contains(participants.at(0), Qt::CaseSensitive);
        }
        else return false;
    }
    // if the in list has 1 item, then platonic vs sexual does not matter
    else if (in_participants.size() == 1) {
        // we do a substring contains only if the filter has 1 person
        if (participants.size() == 1) {
            return in_participants.at(0).contains(participants.at(0), Qt::CaseSensitive);
        }
        else return false;
    }
    /* Otheriwse, we check platonic or not, and then each filter participant must
     * also be found in the incoming participants. */
    else if (list_platonic == is_platonic) {
        if (participants.size() == 0) return true;
        else if (participants.size() > in_participants.size()) return false;
        else {
            for (int pdex = 0; pdex < participants.size(); pdex++) {
                QString base = participants.at(pdex);
                bool found = false;
                for (int tdex = 0; tdex < in_participants.size(); tdex++) {
                    if (in_participants.at(tdex).contains(base,Qt::CaseSensitive)) {
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }
            return true;
        }
    }
    else return false;
}

//=============================================================================
// constructors
//---------------------------
jfStructuredPairFilter::jfStructuredPairFilter():jfBaseFilter() {
    match_all = false;
    match_reader = false;
    match_original = false;
    filter_pairs = QVector<const jfPairFilterPair*>();
}
//------------------------------
jfStructuredPairFilter::jfStructuredPairFilter(const jfStructuredPairFilter& source):jfBaseFilter() {
  // copying base elements
  description = source.description;
  num_id = source.num_id;
  validdata = source.validdata;
  name = source.name + ", copy";

  filter_pairs = QVector<const jfPairFilterPair*>(source.filter_pairs.size());
  for (int i = 0; i < source.filter_pairs.size(); i++) {
      filter_pairs.append(new jfPairFilterPair(*source.filter_pairs.at(i)));
  }
  match_all = source.match_all;
  match_reader = source.match_reader;
  match_original = source.match_original;
}
//+++++++++++++++++++++++++++++++++++++++++++
// public methods implemented here
//---------------------------
bool jfStructuredPairFilter::isEmpty() const {
    if (filter_pairs.isEmpty()) {
        return !(match_reader || match_original);
    }
    else return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// string conversion
//----------------------------------
bool jfStructuredPairFilter::FromString(const QString& sourcedata) {
    const QString fname = "jfStructuredPairFilter::FromString";
    validdata = false;
    filter_pairs.clear();
    // parsing
    jfLineParse lparse(sourcedata);
    if (lparse.NNotX(4)) return false;
    // getting...
    if (!lparse.BoolVal(0,match_all)) return false;
    if (!lparse.BoolVal(1,match_reader)) return false;
    if (!lparse.BoolVal(2,match_original)) return false;
    QString srcstuff = lparse.UnEscStr(3);
    bool okay = SetNamesData(srcstuff, false);
    if (okay) validdata = true;
    return validdata;
}
//-------------------------------
QString jfStructuredPairFilter::ToString() const {
    jfOutString res;
    res << match_all << match_reader << match_original;
    res << GetNamesData(false);
    return QString(res);
}
//---------------------------------
bool jfStructuredPairFilter::SetNamesData(const QString& sourcedata, bool nl_split) {
  QVector<const jfPairFilterPair*> new_names;
  // parsing the input
  QString qnames = sourcedata.trimmed();
  if (! qnames.isEmpty()) {
      if (! ParsePairs(qnames, nl_split ,new_names)) return false;
  }
  // setting the input
  EmptyPairList(filter_pairs);
  filter_pairs = new_names;
  return true;
}
//---------------------------------
QString jfStructuredPairFilter::GetNamesData(bool nl_split) const {
  if (filter_pairs.isEmpty()) return "";
  QString result = "";
  // looping over the pairs
  bool not_first = false;
  for (const jfPairFilterPair* pair : filter_pairs) {
      if (not_first) result += (nl_split) ? "\n" :",";
      result += pair->toString();
      not_first = true;
  }
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++
// gets a description
QString jfStructuredPairFilter::GetTypeDescription() const {
  return "Matches against the list of pairings that comes with fics from some sites. The filter contains lists of string pairs, like \
'harry' and 'ginny', at least one of the pairings must match one or all of the pairs (or polys) specified \
by the fic (unless the filter is empty).";
}
//-------------------------------
// copy
jfBaseFilter* jfStructuredPairFilter::GenCopy() const {
  jfStructuredPairFilter* newfilt;
  newfilt = new jfStructuredPairFilter();
  CopyOver(newfilt);

  // copying over the parts
  for (const jfPairFilterPair* source_pair : filter_pairs) {
      (newfilt->filter_pairs).append(new jfPairFilterPair(*source_pair));
  }
  newfilt->match_all = match_all;
  newfilt->match_reader = match_reader;
  newfilt->match_original = match_original;
  return newfilt;
}
//------------------------------
// special meta-information
QString jfStructuredPairFilter::GetTypeID() const {
  return "StructuredPairFilter";
}
//--------------------------------
void jfStructuredPairFilter::SetMatchAll(bool inval) {
  match_all = inval;
}
//----------------------------------
bool jfStructuredPairFilter::GetMatchAll() const {
  return match_all;
}
//----------------------------------
void jfStructuredPairFilter::SetMatchReader(bool inval) {
    match_reader = inval;
}

//----------------------------------
bool jfStructuredPairFilter::GetMatchReader() const {
    return match_reader;
}
//----------------------------------
void jfStructuredPairFilter::SetMatchOriginal(bool inval) {
    match_original = inval;
}
//----------------------------------
bool jfStructuredPairFilter::GetMatchOriginal() const {
    return match_original;
}
//--------------------------------
// destructor
jfStructuredPairFilter::~jfStructuredPairFilter() {
    EmptyPairList(filter_pairs);
}
//+++++++++++++++++++++++++++++++++++++++++++
void jfStructuredPairFilter::EmptyPairList(QVector<const jfPairFilterPair*>& plist) {
    for (int pdex = 0; pdex < plist.size(); pdex++) {
        if (plist[pdex] != NULL) {
            delete plist[pdex];
            plist[pdex] = NULL;
        }
    }
    plist.clear();
}
//---------------------------------------------
/* Matches the filter against a single target fic pairing. */
bool jfStructuredPairFilter::DoesPairingMatch(const jfPairingStruct& target) const {
    // the special original and reader cases
    if (match_original && target.original) return true;
    if (match_reader && target.reader) return true;
    // special case
    if (filter_pairs.isEmpty()) return false;
    else {
        // we convert the target names to lowercase to avoid having to do repeated case insenitive searches
        QStringList lc;
        for (QString sname : target.participants) {
            lc.append(sname.toLower());
        }
        // looping over the filter pairs
        for (const jfPairFilterPair* filter_pair : filter_pairs) {
            if (filter_pair->IsAMatchFor(target.is_platonic, lc)) return true;
        }
        return false;
    }
}
//-------------------------------------------
const jfFanficPairsMixin* jfStructuredPairFilter::CheckCastElement(const jfSearchResultItem* testelem) const {
    if (testelem == NULL) return NULL;
    if (testelem->GetTypeLabels().contains(jfFanficPairsMixin::FANFIC_PAIRS_TYPE_ID)) {
        return dynamic_cast<const jfFanficPairsMixin*>(testelem);
    }
    else return NULL;
}
//-------------------------------------------
bool jfStructuredPairFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  const QString fname = "jfStructuredPairFilter::CoreMatch";

  const jfFanficPairsMixin* pair_fanfic = CheckCastElement(testelem);
  assert(pair_fanfic != NULL);

  // if there is no pair info, we return false
  if (pair_fanfic->RelationshipCount() == 0) return false;
  else {
      // match all means every item must match
      if (match_all) {
          for (const jfPairingStruct* relationship : pair_fanfic->GetRelationships()) {
              bool this_match = DoesPairingMatch(*relationship);
              if (! this_match) return false;
          }
          return true;
      }
      // otherwise, only one item must match
      else {
          for (const jfPairingStruct* relationship : pair_fanfic->GetRelationships()) {
              bool this_match = DoesPairingMatch(*relationship);
              if (this_match) return true;
          }
          return false;
      }
  }
}
//-------------------------------------------
bool jfStructuredPairFilter::ParsePairs(const QString& inval, bool nl, QVector<const jfPairFilterPair*>& target) const {
  const QString fname = "jfStructuredPairFilter::ParsePairs";
  // initial trimming and splitting
  QString clist = inval.trimmed();
  /**/JDEBUGLOGS(fname,1,clist)

  EmptyPairList(target);
  // the special case of an empty input
  if (clist.isEmpty()) return true;
  else {
      QStringList rawpairs;
      if (nl) rawpairs = SplitTrimNL(clist);
      else rawpairs = SplitTrim(clist, ",");
      for (QString rawpart : rawpairs) {
          jfPairFilterPair* parsed_part = jfPairFilterUtils::ParseFilterPair(rawpart);
          if (parsed_part == NULL) return false;
          target.append(parsed_part);
      }
      if (target.isEmpty()) return false;
  }
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++
// file i/o
//---------------------------
bool jfStructuredPairFilter::AddRestToFile(QTextStream* outfile) const {
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding
  (*outfile) << ToString() << "\n";
  return true;
}
//---------------------------
bool jfStructuredPairFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfStructuredPairFilter::ReadRestFromFile";
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
size_t jfStructuredPairFilter::ExtraLines() const { return 1; }
// ******************************************************************************************************

