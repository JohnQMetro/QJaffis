/*****************************************************************************
Name    : baseobj.cpp
Basic   : Defines the most basic data classes
Author  : John Q Metro
Started : March 17, 2009
// Conversion to Qt started March 12, 2013
Updated : February 14, 2018
Notes   :

******************************************************************************/
#ifndef BASEOBJ_H_INCLUDED
    #include "baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
//****************************************************************************
jfBaseObj::jfBaseObj() {
  num_id = 0;
  name = "BaseObj Default Name";
  validdata = true;
}
//------------------------------------------------------------------
jfBaseObj::jfBaseObj(size_t src_id, const QString& nname) {
  num_id = src_id;
  validdata = true;
  SetName(nname);
}
jfBaseObj::~jfBaseObj() {}
//============================================================================
bool jfBaseObj::IsValid() const {
  return validdata;
}
//--------------------------------------------------------------------------
QString jfBaseObj::GetName() const {
  return name;
}
//-------------------------------------------------------------------------
bool jfBaseObj::SetName(const QString& newname) {
  if (newname.isEmpty()) {
    validdata = false;
    return false;
  }
  else {
    name = newname;
    return true;
  }
}
//------------------------------------------------------------------------
QString jfBaseObj::GetDescription() const {
  return description;
}
//-------------------------------------------------------------------------
void jfBaseObj::SetDescription(const QString& newdesc) {
  description = newdesc;
}
//-------------------------------------------------------------------------
size_t jfBaseObj::GetID() const {
  return num_id;
}
//-------------------------------------------------------------------------
void jfBaseObj::SetID(size_t src_id) {
  num_id = src_id;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfBaseObj::AddToFile(QTextStream* outfile) const {
  // constants and variables
  const QString fname = "jfBaseObj::AddToFile";
  jfOutString s_out;
  bool rbuf;
  QString qbuf;
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 1
  s_out << GetTypeID() << num_id << validdata;
  qbuf = "###" + QString(s_out);
  (*outfile) << qbuf << '\n';
  s_out.FullClear();
  // line 2
  s_out << name << description;
  (*outfile) << s_out << '\n';
  // virtual calls
  rbuf = AddMiddleToFile(outfile);
  if (!rbuf) return false;
  rbuf = AddRestToFile(outfile);
  if (!rbuf) return false;
  // adding on the footer
  (*outfile) << "###\n";
  return true;
}
//-----------------------------------------------------------------------------
bool jfBaseObj::GetFromFile(jfFileReader* infile) {
  const QString funcname="jfBaseObj::GetFromFile";
  // input data
  QString buffer;

  // starting checks
  assert(infile!=NULL);
  // Reading and processing line 1
  if (!infile->ReadParseSLine(3,funcname)) return false;
  buffer = infile->lp.UnEscStr(0);
  if (buffer!=GetTypeID()) {
    return infile->BuildError("The type (" + buffer + ") does not match!");
  }
  // field two is a number id
  if (!infile->lp.SIntVal(1,num_id)) {
    return infile->BuildError("The id is not an integer!");
  }
  // field number 3 is the validdata attribute
  if (!infile->lp.BoolVal(2,validdata)) {
    return infile->BuildError("Validdata is not a boolean!");
  }
  // we get the next line
  if (!infile->ReadParseLine(2,funcname)) return false;
  // the name
  name = infile->lp.UnEscStr(0);
  if (name.isEmpty()) infile->BuildError("The name is blank!");
  // description
  description = infile->lp.UnEscStr(1);
  // we do the virtual methods
  if (!ReadMiddleFromFile(infile)) return false;
  if (!ReadRestFromFile(infile)) return false;
  // we should be left just before a final line...
  if (!infile->ReadLast(funcname)) return false;
  return true;
  // done
}
//****************************************************************************
jfUrlBase::jfUrlBase():jfBaseObj() {
  validdata = false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfUrlBase::GetUrl() const {
  return primarylink;
}
//-------------------------------------------------------------
const QStringList* jfUrlBase::GetTokTitle() const {
  return TokenizeString(name,true);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal methods
//-------------------------------------------------------------
void jfUrlBase::CopyFrom(const jfUrlBase* src) {
  assert(src!=NULL);
  num_id = src->num_id;
  name = src->name;
  description = src->description;
  validdata = src->validdata;
  primarylink = src->primarylink;
}
//-----------------------------------------------------------
void jfUrlBase::CopyTo(jfUrlBase* dest) const {
  assert(dest!=NULL);
  dest->num_id = num_id;
  dest->name = name;
  dest->description = description;
  dest->validdata = validdata;
}
//---------------------------------------------------------------------
void jfUrlBase::SetValid() {
  validdata = (!name.isEmpty()) && (!primarylink.isEmpty());
}

//*****************************************************************************
// --- jfBasePD Methods
// ==========================================================================
// the default constructor
jfBasePD::jfBasePD():jfUrlBase() {
  included = true;
  ignore = false;
  toksummary = NULL;
  rank = 0;
}
//---------------------------------------------------------------------------
// the default constructor
jfBasePD::jfBasePD(const size_t& newrank):jfUrlBase() {
  included = true;
  ignore = false;
  toksummary = NULL;
  rank = newrank;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gived the order in which the item appaeared in the source
size_t jfBasePD::GetRank() const {
  return rank;
}
//---------------------------------------------------------------------------
// returns some info for displaying the item is a gui result list
jfResultUnit* jfBasePD::makeResultUnit() const {
  jfResultUnit* result;
  result = new jfResultUnit();
  result->marked = mark.to_ulong();
  result->htmldata = ToDisplayHTML();
  result->slink = this;
  return result;
}
//-----------------------------------------------------------------------
/* full fimfanfic.net descriptions are sometimes so long that HTML listview boxes
are messed up. This method is intended to get a truncated version, limited in number
of lines and total characters */
QString jfBasePD::GetDescExtract(size_t lines, size_t maxlen) const {
  // special cases
  if ((lines==0) || (maxlen==0)) return "";
  if (description.length()==0) return "";
  // variables
  QString buffer, runres;
  size_t runline, runmax, linelen, templine;
  jfStringParser* dparse;
  int rpos1,rpos2;
  bool truncated;
  int lendif;
  // initializing
  dparse = new jfStringParser(description);
  runline = 0;
  runmax = 0;
  truncated = false;
  // the parsing loop
  while (dparse->MovePastOrEnd("<br />",buffer)) {
    // line check
    runline++;
    if (runline>lines) {
      truncated = true;
      break;
    }
    // line length check
    linelen = buffer.length();
    templine = runmax + 4 + linelen;
    if (templine>maxlen) {
      truncated = true;
      lendif = templine - maxlen;
      lendif = linelen -lendif - 4;
      if (lendif<0) break;
      buffer.truncate(lendif);
      linelen = lendif;
    }
    // appending the value
    if (!runres.isEmpty()) runres += "<br>";
    runres += buffer;
    runmax += linelen + 4;
    if (truncated) break;
  }
  // post processing
  delete dparse;
  // finally, we make sure we do not end inside a tag
  rpos1 = runres.lastIndexOf('<');
  if (rpos1>=0) {
    rpos2 = runres.lastIndexOf('>');
    if ((rpos2<0) || (rpos2<rpos1)) {
      runres.truncate(rpos1);
      truncated = true;
    }
  }
  if (truncated) runres += " ...";
  runres += "</u></a></p>";
  return runres;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const QStringList* jfBasePD::GetTokSummary() const {
  if (toksummary==NULL) {
    toksummary = TokenizeString(description,true);
  }
  return toksummary;
}
// ----------------------------------------------------------------------------
void jfBasePD::ClearTok() const{
  if (toksummary!=NULL) {
    delete toksummary;
    toksummary = NULL;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// changing the rank
void jfBasePD::ChangeRank(const size_t& newrank) {
  rank = newrank;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfBasePD::~jfBasePD() {
  ClearTok();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfBasePD::CopyOver(jfBasePD* destination) const {
  assert(destination!=NULL);
  CopyTo(dynamic_cast<jfUrlBase*>(destination));
  destination->rank = rank;
  destination->included = included;
  destination->ignore = ignore;
  destination->mark = mark;
}
//-----------------------------------------------------------------------------
void jfBasePD::LoadCoreValues(jfSkeletonParser* inparser) const {
  // runs some checks first
  assert(inparser!=NULL);
  // adding the values
  inparser->AddText("ITEM_NAME",name);
  inparser->AddText("ITEM_DESCRIPTION",description);
  inparser->AddText("ITEM_URL",primarylink);
  inparser->AddUInt("ITEM_ID",num_id);
  inparser->AddUInt("ITEM_RANK",rank);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfBasePD::AddMiddleToFile(QTextStream* outfile) const {
  QString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 3
  result = QString::number(mark.to_ulong()) + ';';
  result += QString::number(rank) + ';';
  result +=  EscapeNL(primarylink,IO_DELIMS);
  (*outfile) << result << '\n';
  return true;
}
//--------------------------------------------------------------------------
// reading from the file
bool jfBasePD::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfBasePD::ReadMiddleFromFile";
  // input data
  QString buffer;
  quint32 marked;
  // reading the line
  if (!infile->ReadParseLine(3,funcname)) return false;
  if (!infile->lp.UIntVal(0,marked)) {
    return infile->BuildError("The marked field is not an integer!");
  }
  // I have to use long long to get around a VC10 bug
  std::bitset<32> mopee((unsigned long long)marked);
  mark = mopee;
  // field two is the rank, implemented as an integer
  if (!infile->lp.SIntVal(0,rank)) {
    return infile->BuildError("The rank is not an integer!");
  }
  // field three is the url
  primarylink = infile->lp.UnEscStr(2);
  return true;
}
//===========================================================================
// getting the string data
QString jfResultUnit::GetUrl() const {
  return (slink->GetUrl());
}
//--------------------------------------------------------------------------
QString jfResultUnit::GetHTML() const {
  return htmldata;
}
//--------------------------------------------------------------------------
QString jfResultUnit::GetName() const {
  return (slink->GetName());
}
//----------------------------------------------
size_t jfResultUnit::GetID() const {
  return (slink->GetID());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// marking data
//--------------------------------------------------------------------------
bool jfResultUnit::HasMark()const {
  return (marked!=0);
}
//--------------------------------------------------------------------------
bool jfResultUnit::Marked(const size_t& catindex) const {
  assert(catindex<32);
  size_t mask, res;
  // constructing the mask
  mask = size_t(1)<<catindex;
  // doing a bitwise and to get the result
  res = mask & marked;
  if (res==0) return false;
  else return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// specials
bool jfResultUnit::SameID(const size_t& cval) const {
  return (cval==(slink->GetID()));
}
//-------------------------------------------------------------------------
const jfBasePD* jfResultUnit::Sourcelink() const {
  return slink;
}

//****************************************************************************
