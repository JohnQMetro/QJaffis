//***************************************************************************
// Name:        pairfilter.cpp
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters are complicated.
// Created:     January 1, 2011
// Conversion to QT Started April 14, 2013
// Updated:     April 30, 2023
//***************************************************************************
#ifndef PAIRFILTER_H_INCLUDED
  #include "pairfilter.h"
#endif // PAIRFILTER_H_INCLUDED
#ifndef JFEXPRBASE
  #include "../../express/expr_base.h"
#endif
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef JFILTERGLOBALS
  #include "../filter_globals.h"
#endif
//----------------------
#include <assert.h>
//***************************************************************************
jfPairFilterCore::jfPairFilterCore(const QString& filter_name):jfFilterBase(filter_name) {
    specials = NULL;
    logdata = NULL;
    logstats = false;
    sepchars = "/&X+";
}
// --------------------------------
jfPairFilterCore::jfPairFilterCore(QString&& filter_name):jfFilterBase(filter_name) {
    specials = NULL;
    logdata = NULL;
    logstats = false;
    sepchars = "/&X+";
}
//-------------------------------------------------------------------------
// copy constructor
jfPairFilterCore::jfPairFilterCore(const jfPairFilterCore& insrc):jfFilterBase(insrc.name) {
    description = insrc.description;
    // variables
    size_t iloop, icount;
    QStringList* newthing;
    // things simple to copy
    flags = insrc.flags;
    sepchars = insrc.sepchars;
    if (insrc.specials == NULL) specials = NULL;
    else specials = new QStringList(*insrc.specials);
    // copying the namesets
    icount = insrc.namedata.size();
    for (iloop=0;iloop<icount;iloop++) {
        newthing = new QStringList(*insrc.namedata[iloop]);
        namedata.push_back(newthing);
    }
    logstats = false;
    logdata = NULL;
    // done
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
//----------------------------------------
bool jfPairFilterCore::SetSeparators(QString inval) {
    StripDuplicateChars(inval);
    if (inval.isEmpty()) return false;
    sepchars = inval;
    return true;
}
//----------------------------------------
const jfPF_flaghold& jfPairFilterCore::GetFlagsRef() const{
  return flags;
}
// ---------------------------------------
jfPF_flaghold& jfPairFilterCore::GetMutFlagsRef() {
    return flags;
}
//----------------------------------------
bool jfPairFilterCore::AddNameset(QString inval) {
    QStringList* invaltok;
    // checking for emptiness
    if (inval.isEmpty()) return false;
    // going ahead
    inval = inval.trimmed();
    invaltok = ListToStrArr(inval,';');
    namedata.push_back(invaltok);
    return true;
}
//----------------------------------------
bool jfPairFilterCore::AddSpecials(QString inval) {

    // checking for emptiness
    inval = inval.trimmed();
    if (inval.isEmpty()) return false;
    // clearing the old specials
    if (specials!=NULL) {
        delete specials;
        specials = NULL;
    }
    // going ahead
    specials = ListToStrArr(inval,';');
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting data
//----------------------------------------
QString jfPairFilterCore::GetSeparators() const {
    return sepchars;
}
//----------------------------------------
QString jfPairFilterCore::GetSpecials() const {
    QString result;
    if (specials==NULL) return "";
    else {
        result = StrArrToList(specials,';');
        return result;
    }
}
//----------------------------------------
size_t jfPairFilterCore::GetNamesetCount() const {
    return namedata.size();
}
//----------------------------------------
QString jfPairFilterCore::GetNameset(size_t nindex) const {
    assert(nindex<namedata.size());
    QString result = StrArrToList(namedata[nindex],';');
    return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
//----------------------------------------
bool jfPairFilterCore::ChangeName(QString inname) {
    bool snres;
    snres = SetName(inname);
    assert(snres);
    if (logstats) {
        logdata->SetFilename(inname+"_stats.txt",inname);
    }
    return logstats;
}
//----------------------------------------
// turning on the logging
bool jfPairFilterCore::StartLogging() {
    QString nameval;
    if (!logstats) {
        nameval = GetName();
        logstats = true;
        logdata = new jfPairStats(sepchars);
        logdata->SetFilename(nameval+"_stats.txt",nameval);
        return true;
    }
    else return false;
}
//----------------------------------------
bool jfPairFilterCore::WriteLog() {
  if (!logstats) return false;
  else {
    logdata->WriteResults();
    return true;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFilterCore::IsEmpty() const {
  if ((specials!=NULL) && (specials->size()!=0)) return false;
  return (namedata.size()==0);
}
//-----------------------------------------------------
QString jfPairFilterCore::ToString() const {
    // local variables
    QString buffer;
    jfOutString result;
    size_t nslst;
    // flags first
    result << flags.ToString();
    // separators next line
    result << sepchars;
    // specials next
    if (specials==NULL) buffer = "";
    else buffer = StrArrToList(specials,';');
    result << buffer;
    // the number of namesets
    result << namedata.size();
    // finally, the namesets
    for (nslst=0;nslst<namedata.size();nslst++) {
        buffer = StrArrToList(namedata[nslst],';');
        result << buffer;
    }
    // done
    return QString(buffer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfPairFilterCore::~jfPairFilterCore() {
  if (specials!=NULL) {
    delete specials;
    specials = NULL;
  }
  if (logdata!=NULL) {
    delete logdata;
    logstats = false;
    logdata = NULL;
  }
  for (size_t nloop=0;nloop<namedata.size();nloop++) {
    if (namedata[nloop]!=NULL) {
      delete namedata[nloop];
      namedata[nloop] = NULL;
    }
  }
  namedata.clear();
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFilterCore::FromStringInner(const QString& sourcedata, QString& error_out) {
    // local variables
    QString ldata,buffer;
    jfLineParse data;
    size_t ncount,nloop;
    // parses the source string
    // preliminary steps
    data.SetString(sourcedata);
    // starting
    buffer = data.UnEscStr(0);
    jfPF_flaghold new_flags;
    if (!new_flags.FromString(buffer)) {
        error_out = "Flags part failed to parse!";
        return false;
    }
    if (!SetSeparators(data.UnEscStr(1))) {
        error_out = "The separators are empty!";
        return false;
    }
    if (!AddSpecials(data.UnEscStr(2))) return false;
    // next up is the item
    if (!data.SIntVal(3,ncount)) return false;
    for (nloop=0;nloop<ncount;nloop++) {
      if (!AddNameset(data.UnEscStr(4+nloop))) return false;
    }

    flags = new_flags;
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper methods
//--------------------------------
bool jfPairFilterCore::DoMatchString(QString inbase, size_t ninindex1, size_t ninindex2) const {
    const QString fname = "jfPairFilterCore::DoMatchString";
    // the usual checks
    assert(!flags.usetokens);
    assert(ninindex2>ninindex1);
    assert(ninindex2<namedata.size());
    // local variables
    size_t size1, size2;
    size_t loop1,loop2;
    bool resval = false;
    QString name1, name2;
    // initialization
    size1 = namedata[ninindex1]->size();
    size2 = namedata[ninindex2]->size();
    // the outer loop over the first set of names
    for (loop1=0; loop1<size1; loop1++) {
        name1 = (*namedata[ninindex1])[loop1];
        // a quick check to aviod multiple searches
        if (!inbase.contains(name1,Qt::CaseInsensitive)) continue;
        // the inner loop over the second set of names
        for (loop2=0;loop2<size2;loop2++) {
            name2 = (*namedata[ninindex2])[loop2];
            resval = LookForPairString(inbase,name1,name2,sepchars,flags);
            if (resval) break;
        }
    }
    // here we are, at the end
    // we might need to log
    if (resval && logstats) {
        assert(logdata!=NULL);
        logdata->LogFind(name1,name2,flags.curr_char);
    }
    return resval;
}
//--------------------------------
bool jfPairFilterCore::DoMatchToken(const QStringList* inbase, size_t ninindex1, size_t ninindex2) const {
  const QString fname = "jfPairFilterCore::DoMatchToken";
  // the usual checks
  assert(flags.usetokens);
  assert(ninindex2>ninindex1);
  assert(ninindex2<namedata.size());
  // special checks
  if (inbase==NULL) return false;
  if (inbase->isEmpty()) return false;
  // local variables
  size_t size1, size2;
  size_t loop1,loop2;
  bool resval = false;
  QString name1, name2;
  // initialization
  size1 = namedata[ninindex1]->size();
  size2 = namedata[ninindex2]->size();
  // the outer loop over the first set of names
  for (loop1=0; loop1<size1; loop1++) {
    name1 = (*namedata[ninindex1])[loop1];
    if (!inbase->contains(name1,Qt::CaseInsensitive)) continue;
    // the inner loop over the second set of names
    for (loop2=0;loop2<size2;loop2++) {
      name2 = (*namedata[ninindex2])[loop2];
      resval = LookForPairToken(inbase,name1,name2,sepchars,flags);
      if (resval) break;
    }
  }
  // here we are, at the end
  // we might need to log
  if (resval && logstats) {
    assert(logdata!=NULL);
    logdata->LogFind(name1,name2,flags.curr_char);
  }
  return resval;
}
//--------------------------------
bool jfPairFilterCore::MatchSpecialsString(QString inbase) const {
  // values
  bool result = false;
  int sloop,scount;
  // special case
  if (specials==NULL) return false;
  // starting
  scount = specials->size();
  // the loop
  for (sloop=0;sloop<scount;sloop++) {
    result = LookForPatternString(inbase,(*specials)[sloop],flags);
    if (result) break;
  }
  if (result && logstats) {
    assert(logdata!=NULL);
    logdata->LogSpecial((*specials)[sloop]);
  }
  return result;
}
//--------------------------------
bool jfPairFilterCore::MatchSpecialsToken(const QStringList* inbase) const {
  // values
  bool result = false;
  int sloop,scount;
  // special case
  if (specials==NULL) return false;
  // starting
  scount = specials->size();
  // the loop
  for (sloop=0;sloop<scount;sloop++) {
    result = LookForPatternToken(inbase,(*specials)[sloop],flags);
    if (result) break;
  }
  if (result && logstats) {
    assert(logdata!=NULL);
    logdata->LogSpecial((*specials)[sloop]);
  }
  return result;
}
//--------------------------------
/* tries matching combinations of the nameset at startat and all subsequent
namesets in the list */
bool jfPairFilterCore::MatchMultipleString(QString inbase, size_t startat) const {
  // variables
  size_t loop2;
  bool resval;
  // checks
  assert(!flags.usetokens);
  assert(startat<(namedata.size()-1));
  // the main loop
  for (loop2 = startat+1;loop2<namedata.size();loop2++) {
    resval = DoMatchString(inbase,startat,loop2);
    if (resval) break;
  }
  return resval;
}
//--------------------------------
bool jfPairFilterCore::MatchMultipleToken(const QStringList* inbase, size_t startat) const {
    // variables
  size_t loop2;
  bool resval;
  // checks
  assert(inbase!=NULL);
  assert(flags.usetokens);
  assert(startat<(namedata.size()-1));
  // the main loop
  for (loop2 = startat+1;loop2<namedata.size();loop2++) {
    resval = DoMatchToken(inbase,startat,loop2);
    if (resval) break;
  }
  return resval;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the matching function is really all we will ever need for most stuff
bool jfPairFilterCore::MainMatch(const jfSearchResultItem* intest) const {
  // variables
  QString sumvalue;
  const QStringList* token_sum;
  bool eval;
  // checks
  assert(intest!=NULL);
  // special
  if (IsEmpty()) return false;
  // 2 main options here, tokenized or not
  if (flags.usetokens) {
    token_sum = intest->GetTokSummary();
    eval = MatchSpecialsToken(token_sum);
    if (!eval) eval = MatchMultipleToken(token_sum,0);
  }
  else {
    sumvalue = intest->GetSummary();
    eval = MatchSpecialsString(sumvalue);
    if (!eval) eval = MatchMultipleString(sumvalue,0);
  }
  // done
  return eval;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFilterCore::CompatReadRestFromFile(jfFileReader* infile) {
    const QString funcname = "jfPairFilterCore::CompatReadRestFromFile";
    // input data
    QString cline;
    unsigned long oval;
    size_t lindex;
    // starting checks (and reading the line)
    assert(infile!=NULL);
    if (!infile->ReadUnEsc(cline,funcname)) return false;
    // this can be fairly complicated
    // flags
    if (!flags.FromString(cline)) return infile->BuildError("The flags line is incorrect");
    // separator chars
    if (!infile->ReadUnEsc(cline,funcname)) return false;
    if (!SetSeparators(cline)) return infile->BuildError("The separators are wrong!");
    // special labels
    if (!infile->ReadUnEsc(cline,funcname)) return false;
    AddSpecials(cline);
    // the count of namesets
    if (!infile->ReadULong(oval,funcname)) return false;
    // name sets
    for (lindex=0;lindex<oval;lindex++) {
        if (!infile->ReadUnEsc(cline,funcname)) return false;
        if (!AddNameset(cline)) return infile->BuildError("Nameset empty or wrong are wrong!");
    }
    // we are done here!
    return true;
}

//---------------------------------------------

//=================================================================================
const jfFilterTypeMeta SINGLE_PAIR_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::PAIR, "PairFilterSingle", "Single Pair Summary Filter",
          QString("Pairing filters look for pairs of names in the summary/description ") +
                  "text, separated by a delimiter or nothing. This is typically used " +
                  "to mark a romance, like \"Buffy/Spike\", \"LeiaHan\", or " +
                  "\"Usagi X Mamoru\". We also include looking for special keywords " +
                  "that the fandom might use to denote a particular relationship. " +
                  "This version of pairing filter looks for 1 pair of names.",
          IdForBaseSearch(), createFilter<jfPairFilterSingle> );
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// core constructor
//---------------------------------------------
jfPairFilterSingle::jfPairFilterSingle(const QString& filter_name):jfPairFilterCore(filter_name) {
}
// -----------------------------------
jfPairFilterSingle::jfPairFilterSingle(QString&& filter_name):jfPairFilterCore(filter_name) {

}
//---------------------------------------------
jfPairFilterSingle::jfPairFilterSingle(const jfPairFilterSingle& insrc):jfPairFilterCore(insrc) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------
jfFilterBase* jfPairFilterSingle::GenCopy() const {
  return Copy();
}
//---------------------------------------------
jfPairFilterCore* jfPairFilterSingle::PairFilterCopy() const {
  return Copy();
}
//---------------------------------------------
jfPairFilterSingle* jfPairFilterSingle::Copy() const {
  return new jfPairFilterSingle(*this);
}
// --------------------------------------------------
const jfFilterTypeMeta& jfPairFilterSingle::GetTypeMetaInfo() const {
    return SINGLE_PAIR_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual method
bool jfPairFilterSingle::CoreMatch(const jfSearchResultItem* testelem) const {
  return MainMatch(testelem);
}


//=================================================================================
const jfFilterTypeMeta LIST_PAIR_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::PAIR, "PairFilterList", "List Pair Summary Filter",
          QString("Pairing filters look for pairs of names in the summary/description ") +
                  "text, separated by a delimiter or nothing. This is typically used " +
                  "to mark a romance, like \"Buffy/Spike\", \"LeiaHan\", or " +
                  "\"Usagi X Mamoru\". We also include looking for special keywords " +
                  "that the fandom might use to denote a particular relationship. " +
                  "This version of pairing filter matches 1 set of names against a list.",
          IdForBaseSearch(), createFilter<jfPairFilterList> );
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// core constructor
//---------------------------------------------
jfPairFilterList::jfPairFilterList(const QString& filter_name):jfPairFilterCore(filter_name) {

}
// --------------------------
jfPairFilterList::jfPairFilterList(QString&& filter_name):jfPairFilterCore(filter_name) {

}
//---------------------------------------------
jfPairFilterList::jfPairFilterList(const jfPairFilterList& insrc):jfPairFilterCore(insrc) {}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfPairFilterList::GetTypeMetaInfo() const {
    return LIST_PAIR_FILTER_INFO;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------
jfFilterBase* jfPairFilterList::GenCopy() const {
  return Copy();
}
//---------------------------------------------
jfPairFilterCore* jfPairFilterList::PairFilterCopy() const {
  return Copy();
}
//---------------------------------------------
jfPairFilterList* jfPairFilterList::Copy() const {
  return new jfPairFilterList(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual method
bool jfPairFilterList::CoreMatch(const jfSearchResultItem* testelem) const {
  return MainMatch(testelem);
}
//=================================================================================
const jfFilterTypeMeta MULTIPLE_PAIR_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::PAIR, "PairFilterMultiple", "Cross Pair Summary Filter",
          QString("Pairing filters look for pairs of names in the summary/description ") +
                  "text, separated by a delimiter or nothing. This is typically used " +
                  "to mark a romance, like \"Buffy/Spike\", \"LeiaHan\", or " +
                  "\"Usagi X Mamoru\". We also include looking for special keywords " +
                  "that the fandom might use to denote a particular relationship. " +
                  "This version of pairing filter matches all the sets of names " +
                  "against each other.",
          IdForBaseSearch(), createFilter<jfPairFilterMultiple> );


// core constructor
//---------------------------------------------
jfPairFilterMultiple::jfPairFilterMultiple(const QString& filter_name):jfPairFilterCore(filter_name) {}
jfPairFilterMultiple::jfPairFilterMultiple(QString&& filter_name):jfPairFilterCore(filter_name) {}

//---------------------------------------------
jfPairFilterMultiple::jfPairFilterMultiple(const jfPairFilterMultiple& insrc):jfPairFilterCore(insrc) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfPairFilterMultiple::GetTypeMetaInfo() const {
    return MULTIPLE_PAIR_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------
jfFilterBase* jfPairFilterMultiple::GenCopy() const {
  return Copy();
}
//---------------------------------------------
jfPairFilterCore* jfPairFilterMultiple::PairFilterCopy() const {
  return Copy();
}
//---------------------------------------------
jfPairFilterMultiple* jfPairFilterMultiple::Copy() const {
  return new jfPairFilterMultiple(*this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual method
bool jfPairFilterMultiple::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  QString sumvalue;
  const QStringList* token_sum;
  bool eval;
  size_t nloop, nmax;
  // checks
  assert(testelem!=NULL);
  // special
  if (IsEmpty()) return false;
  nmax = namedata.size();
  // 2 main options here, tokenized or not
  if (flags.usetokens) {
    token_sum = testelem->GetTokSummary();
    eval = MatchSpecialsToken(token_sum);
    if (!eval) {
      for (nloop=0; nloop<(nmax-1); nloop++) {
        eval = MatchMultipleToken(token_sum,nloop);
        if (eval) break;
      }
    }
  }
  else {
    sumvalue = testelem->GetSummary();
    eval = MatchSpecialsString(sumvalue);
    if (!eval) {
      for (nloop=0; nloop<(nmax-1); nloop++) {
        eval = MatchMultipleString(sumvalue,nloop);
        if (eval) break;
      }
    }
  }
  // done
  return eval;
}
//***************************************************************************
bool IsOldPairFilter(jfFilterBase* filter) {
    if (filter == NULL) return false;
    else {
        QString ident = filter->GetTypeIdentifier();
        if (ident == SINGLE_PAIR_FILTER_INFO.Identifier()) return true;
        else if (ident == LIST_PAIR_FILTER_INFO.Identifier()) return true;
        else if (ident == MULTIPLE_PAIR_FILTER_INFO.Identifier()) return true;
        else return false;
    }
}
