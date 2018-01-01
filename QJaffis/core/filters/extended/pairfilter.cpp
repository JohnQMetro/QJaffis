//***************************************************************************
// Name:        pairfilter.cpp
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters are complicated.
// Created:     January 1, 2011
// Conversion to QT Started April 14, 2013
// Updated:     May 29, 2012
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
jfPairFilterCore::jfPairFilterCore() {
  specials = NULL;
  logdata = NULL;
  logstats = false;
  sepchars = "/&X+";
}
//-------------------------------------------------------------------------
// copy constructor
jfPairFilterCore::jfPairFilterCore(const jfPairFilterCore& insrc) {
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
  sepchars = inval;
  if (!StripDuplicateChars(sepchars)) return false;
  return (!sepchars.isEmpty());
}
//----------------------------------------
jfPF_flaghold& jfPairFilterCore::GetFlagsRef() const{
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
  // clearing the old specials
  if (specials!=NULL) {
    delete specials;
    specials = NULL;
  }
  // checking for emptiness
  inval = inval.trimmed();
  if (inval.isEmpty()) return false;
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
//-----------------------------------------------------------------------------
// returns a general filter type. The default is 0, unless overriden
size_t jfPairFilterCore::GetFilType() const { return 2;}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFilterCore::isEmpty() const {
  if ((specials!=NULL) && (specials->size()!=0)) return false;
  return (namedata.size()==0);
}
//-----------------------------------------------------
bool jfPairFilterCore::FromString(const QString& sourcedata) {
  // local variables
  QString ldata,buffer;
  jfLineParse data;
  size_t ncount,nloop;
  // parses the source string
  // preliminary steps
  data.SetString(sourcedata);
  // starting
  buffer = data.UnEscStr(0);
  if (!flags.FromString(buffer)) return false;
  if (!SetSeparators(data.UnEscStr(1))) return false;
  if (!AddSpecials(data.UnEscStr(2))) return false;
  // next up is the item
  if (!data.SIntVal(3,ncount)) return false;
  for (nloop=0;nloop<ncount;nloop++) {
    if (!AddNameset(data.UnEscStr(4+nloop))) return false;
  }
  return validdata;
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
bool jfPairFilterCore::MainMatch(const jfBasePD* intest) const {
  // variables
  QString sumvalue;
  const QStringList* token_sum;
  bool eval;
  // checks
  assert(intest!=NULL);
  // special
  if (isEmpty()) return false;
  // 2 main options here, tokenized or not
  if (flags.usetokens) {
    token_sum = intest->GetTokSummary();
    eval = MatchSpecialsToken(token_sum);
    if (!eval) eval = MatchMultipleToken(token_sum,0);
  }
  else {
    sumvalue = intest->GetDescription();
    eval = MatchSpecialsString(sumvalue);
    if (!eval) eval = MatchMultipleString(sumvalue,0);
  }
  // done
  return eval;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfPairFilterCore::ExtraLines() const {
  return (4+namedata.size());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfPairFilterCore::AddRestToFile(QTextStream* outfile) const {
  // variables
  QString buffer;
  size_t nslst;
  // checking and special conditions
  if (outfile==NULL) return false;
  // flags first
  buffer = EscapeNL(flags.ToString(),IO_DELIMS);
  (*outfile) << buffer << "\n";
  // separators next line
  buffer = EscapeNL(sepchars,IO_DELIMS);
  (*outfile) << buffer << "\n";
  // specials next
  if (specials==NULL) buffer = "";
  else buffer = StrArrToList(specials,';');
  buffer = EscapeNL(buffer,IO_DELIMS);
  (*outfile) << buffer << "\n";
  // the number of namesets
  buffer = QString::number(namedata.size());
  (*outfile) << buffer << "\n";
  // finally, the namesets
  for (nslst=0;nslst<namedata.size();nslst++) {
    buffer = StrArrToList(namedata[nslst],';');
    buffer = EscapeNL(buffer,IO_DELIMS);
    (*outfile) << buffer << "\n";
  }
  // done
  return true;
}
//---------------------------------------------
bool jfPairFilterCore::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfPairFilterCore::ReadRestFromFile";
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
//=================================================================================
// core constructor
//---------------------------------------------
jfPairFilterSingle::jfPairFilterSingle():jfPairFilterCore() {}
//---------------------------------------------
jfPairFilterSingle::jfPairFilterSingle(const jfPairFilterSingle& insrc):jfPairFilterCore(insrc) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// overloaded filter/baseobj methods
//---------------------------------------------
QString jfPairFilterSingle::GetTypeID() const {
  return "PairFilterSingle";
}
//---------------------------------------------
QString jfPairFilterSingle::GetTypeDescription() const {
  return "Pairing filters look for pairs of names in the summary/description text, \
separated by a delimiter or nothing. This is typically used to mark a romance, like \
\"Buffy/Spike\", \"LeiaHan\", or \"Usagi X Mamoru\". We also include looking for special \
keywords that the fandom might use to denote a particular relationship. This version \
of pairing filter looks for 1 pair of names.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------
jfBaseFilter* jfPairFilterSingle::GenCopy() const {
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual method
bool jfPairFilterSingle::CoreMatch(const jfBasePD* testelem) const {
  return MainMatch(testelem);
}
//=================================================================================
// core constructor
//---------------------------------------------
jfPairFilterList::jfPairFilterList():jfPairFilterCore() {}
//---------------------------------------------
jfPairFilterList::jfPairFilterList(const jfPairFilterList& insrc):jfPairFilterCore(insrc) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// overloaded filter/baseobj methods
//---------------------------------------------
QString jfPairFilterList::GetTypeID() const {
  return "PairFilterList";
}
//---------------------------------------------
QString jfPairFilterList::GetTypeDescription() const {
  return "Pairing filters look for pairs of names in the summary/description text, \
separated by a delimiter or nothing. This is typically used to mark a romance, like \
\"Buffy/Spike\", \"LeiaHan\", or \"Usagi X Mamoru\". We also include looking for special \
keywords that the fandom might use to denote a particular relationship. This version \
of pairing filter matches 1 set of names against a list.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------
jfBaseFilter* jfPairFilterList::GenCopy() const {
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
bool jfPairFilterList::CoreMatch(const jfBasePD* testelem) const {
  return MainMatch(testelem);
}
//=================================================================================
// core constructor
//---------------------------------------------
jfPairFilterMultiple::jfPairFilterMultiple():jfPairFilterCore() {}
//---------------------------------------------
jfPairFilterMultiple::jfPairFilterMultiple(const jfPairFilterMultiple& insrc):jfPairFilterCore(insrc) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// overloaded filter/baseobj methods
//---------------------------------------------
QString jfPairFilterMultiple::GetTypeID() const {
  return "PairFilterMultiple";
}
//---------------------------------------------
QString jfPairFilterMultiple::GetTypeDescription() const {
  return "Pairing filters look for pairs of names in the summary/description text, \
separated by a delimiter or nothing. This is typically used to mark a romance, like \
\"Buffy/Spike\", \"LeiaHan\", or \"Usagi X Mamoru\". We also include looking for special \
keywords that the fandom might use to denote a particular relationship. This version \
of pairing filter matches all the sets of names against each other.";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------
jfBaseFilter* jfPairFilterMultiple::GenCopy() const {
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
bool jfPairFilterMultiple::CoreMatch(const jfBasePD* testelem) const {
  // variables
  QString sumvalue;
  const QStringList* token_sum;
  bool eval;
  size_t nloop, nmax;
  // checks
  assert(testelem!=NULL);
  // special
  if (isEmpty()) return false;
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
    sumvalue = testelem->GetDescription();
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
