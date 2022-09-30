/////////////////////////////////////////////////////////////////////////////
// Name:        ffns_catcore.cpp
// Author :     John Q Metro
// Purpose :    Declares the Fanfiction.Net category core data classes
//              jaffis_base.h
// Created:     April 4, 2010
// Conversion to Qt Started September 25, 2013
// Updated:     June 29, 2022
/////////////////////////////////////////////////////////////////////////////
#ifndef JFFFNSCATCORE
  #include "ffn_catcore.h"
#endif // JFFFNSCATCORE
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//----------------------------------------------
#include <assert.h>

//***************************************************************************
// unfortunatly, the links provided by the category index pages are sometimes just wrong
const QString sublinks[4] = {"/movie/symmetry/","/movie/asymmetry/",
          "/tv/Law-Order-Division-of-Field-Investigation-",
          "/tv/Law-Order-Division-of-Field-Investigation/"};
//------------------------------------------------------------------
// the correct links can usually be guessed at, though
void CatLinkSub(QString& inoutvalue) {
  const size_t numf = 2;
  for (size_t xindex=0;xindex<numf;xindex++) {
    if (inoutvalue.contains(sublinks[2*xindex])) {
      inoutvalue = sublinks[2*xindex+1];
      break;
    }
  }
}
//***************************************************************************
// constructors
//------------------------
jfFFN_CategoryCore::jfFFN_CategoryCore():jfUrlBase() {
  discard = false;
}
//------------------------
jfFFN_CategoryCore::jfFFN_CategoryCore(const jfFFN_CategoryCore& insource):jfUrlBase() {
  CopyFrom(&insource);
  ccount = insource.ccount;
  discard = insource.discard;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfFFN_CategoryCore::GetEstStoryCount() const {
  return ccount;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFN_CategoryCore::~jfFFN_CategoryCore() {

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// addtional methods
//------------------------
bool jfFFN_CategoryCore::AddMiddleToFile(QTextStream* outfile) const {
  QString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // the line will have the fic count and link
  (*outfile) << QString::number(ccount) << ';';
  (*outfile) << EscapeNL(primarylink,IO_DELIMS);
  (*outfile) << "\n";
  return true;
}
//------------------------
bool jfFFN_CategoryCore::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_CategoryCore::ReadMiddleFromFile";
  // line 3 is just the link...
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,funcname)) return false;
  // field 0 is the estimated number of fics
  if (!infile->lp.SIntVal(0,ccount)) return infile->BuildError("Fic Count is Bad!");
  // field 1 is the primary link
  primarylink = infile->lp.UnEscStr(1);
  // done
  return true;
}
//**************************************************************************
bool jfFFN_CatList::ClearMarks(size_t which) {
  if (which>=32) return false;
  size_t cindex, ccount;
  ccount = size();
  // looping
  for (cindex=0;cindex<ccount;cindex++) {
    ((*this)[cindex]->mark).reset(which);
  }
  return true;
}

//***************************************************************************
// constructors
//-------------------------------------------------
jfFFN_CatBase::jfFFN_CatBase():jfFFN_CategoryCore() {}
//-------------------------------------------------
jfFFN_CatBase::jfFFN_CatBase(const jfFFN_CatBase& incat):jfFFN_CategoryCore(incat) {
  secname = incat.secname;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// additional methods
//-------------------------------------------------
QString jfFFN_CatBase::GetSectionName() const {
  return secname;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------------------
bool jfFFN_CatBase::SameCat(const jfFFN_CategoryCore* intest) const {
  // special case
  if (intest==NULL) return false;
  // type incompatability
  if (intest->GetCatType()==jfx_CROSS) return false;
  if (GetCatType()!=(intest->GetCatType())) return false;
  // we test by URL
  return (GetUrl()==(intest->GetUrl()));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//-------------------------------------------------
bool jfFFN_CatBase::AddRestToFile(QTextStream* outfile) const {
  QString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // the line will have the section name
  (*outfile) << EscapeNL(secname,IO_DELIMS) << "\n";
  return true;
}
//-------------------------------------------------
bool jfFFN_CatBase::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_SectionCategory::ReadRestFromFile";
  // line 3 is just the link...
  assert(infile!=NULL);
  if (!infile->ReadUnEsc(secname,funcname)) return infile->BuildError("Could not read Section name!");
  // done
  return true;
}
//***************************************************************************
// constructors
//-------------------------------------------------
jfFFN_SectionCategory::jfFFN_SectionCategory():jfFFN_CatBase() {}
//-------------------------------------------------
jfFFN_SectionCategory::jfFFN_SectionCategory(const jfFFN_SectionCategory& incat):jfFFN_CatBase(incat) {}
//-------------------------------------------------
jfFFN_SectionCategory::jfFFN_SectionCategory(const QString& sname, const QString& inraw):jfFFN_CatBase() {
  QString parse_message;
  validdata = SetFromSource(sname,inraw,parse_message);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// additional methods
//-------------------------------------------------
bool jfFFN_SectionCategory::SetFromSource(const QString& sname, const QString& inraw, QString& parse_err) {
  // local variables
  const QString fname = "jfFFN_SectionCategory::SetFromSource";
  QString buffer, oerr;
  unsigned long ccount_out;
  double cfx_out;
  jfStringParser xparser;
  // we start...
  validdata = false;
  xparser.ChangeData(inraw);
  // looking for the link...
  if (!xparser.GetDelimited("<a href=\"","\"",buffer)) {
    parse_err = "Could not find URL in source data!";
    return false;
  }
  // making the url
  CatLinkSub(buffer);
  primarylink = "https://www.fanfiction.net" + buffer;
  // since the name inside the linke is sometimes abbreviated...
  if (!xparser.GetDelimited("title=\"","\"",buffer)) {
      parse_err = "Could not extract Category Name! " + inraw;
      return false;
  }

  // setting the name
  buffer = buffer.trimmed().replace("\\'","'").replace("  "," ");
  buffer = htmlparse::ConvertEntities(buffer,false);
  name = buffer;
  // next up, we extract the number of items...
  if (xparser.GetDelimitedFloat("'gray'>(","K)",cfx_out,oerr)) ccount = cfx_out*1000;
  else if (xparser.GetDelimitedFloat("\"gray\">(","K)",cfx_out,oerr)) ccount = cfx_out*1000;
  else if (xparser.GetDelimitedULong("'gray'>(",")",ccount_out,oerr)) ccount = ccount_out;
  else if (xparser.GetDelimitedULong("\"gray\">(",")",ccount_out,oerr)) ccount = ccount_out;
  else {
    QString testo = xparser.GetBlock(1000);
    parse_err = "Could not find number of items in category! : " + oerr;
    return false;
  }
  // finishing
  secname = sname;
  // done
  validdata = true;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------------------
jf_FFCTYPE jfFFN_SectionCategory::GetCatType() const {
  return jfx_CAT;
}
//-------------------------------------------------
QString jfFFN_SectionCategory::MakeFinder() const {
  jfOutString res_data;
  res_data << secname << name;
  // done
  return QString(res_data);
}
//-------------------------------------------------
QString jfFFN_SectionCategory::GetTypeID() const {
  return "FFN_SectionCategory";
}
//*************************************************************************
// constructors
//----------------------------
jfFFN_HalfCrossover::jfFFN_HalfCrossover():jfFFN_CatBase() {
  doupdate = true;
}
//----------------------------
jfFFN_HalfCrossover::jfFFN_HalfCrossover(const jfFFN_HalfCrossover& incat):jfFFN_CatBase(incat) {
  doupdate = true;
}
//----------------------------
jfFFN_HalfCrossover::jfFFN_HalfCrossover(const QString& sname, const QString& inraw):jfFFN_CatBase() {
  doupdate = true;
  QString parse_err;
  validdata = SetFromSource(sname,inraw,parse_err);
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// additional methods
bool jfFFN_HalfCrossover::SetFromSource(const QString& sname, const QString& inraw, QString& parse_err) {
  // local variables
  const QString fname = "jfFFN_HalfCrossover::SetFromSource";
  QString buffer,oerr;
  unsigned long ccount_out, idvalue;
  jfStringParser xparser;
  // we start...
  validdata = false;
  xparser.ChangeData(inraw);
  // looking for the link...
  if (!xparser.GetDelimited("<a href=\"/crossovers/","/",buffer)) {
    parse_err = "Could not find URL in source data!";
    return false;
  }
  // next up, the numeric id
  if (!xparser.GetMovePastULong("/",idvalue,oerr)) {
    parse_err = "Could not extract ID! : " + oerr;
    return false;
  }
  num_id = idvalue;
  /**/jfCLogxMessage(num_id==8074,fname,1,inraw);
  /**/jfCLogxMessage(num_id==8074,fname,2,xparser.GetBlock(200));
  // building the link
  primarylink = "https://www.fanfiction.net/" + buffer + "-Crossovers/";
  primarylink += QString::number(idvalue) + "/0/";
  // we use the title= to get the actual name because the link text is sometimes abbreviated
  if (!xparser.GetDelimited("title=\"","\"",buffer)) {
    parse_err = "Could not extract Category Name! " + inraw;
    return false;
  }
  /**/jfCLogxMessage(num_id==8074,fname,3,buffer);
  // setting the name
  buffer = buffer.trimmed().replace("\\'","'").replace("  "," ");
  buffer = htmlparse::ConvertEntities(buffer,false);
  /**/jfCLogxMessage(num_id==8074,fname,4,buffer);
  name = buffer;
  // next up, we extract the number of items...
  if (xparser.GetDelimitedULong("'gray'>(","K)",ccount_out,oerr)) ccount = ccount_out*1000;
  else if (xparser.GetDelimitedULong("\"gray\">(","K)",ccount_out,oerr)) ccount = ccount_out*1000;
  else if (xparser.GetDelimitedULong("'gray'>(",")",ccount_out,oerr)) ccount = ccount_out;
  else if (xparser.GetDelimitedULong("\"gray\">(",")",ccount_out,oerr)) ccount = ccount_out;
  else {
    parse_err = "Could not find number of items in category! : " + oerr;
    return false;
  }
  // finishing
  secname = sname;
  // done
  validdata = true;
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//----------------------------
jf_FFCTYPE jfFFN_HalfCrossover::GetCatType() const {
  return jfx_HCROSS;
}
//----------------------------
QString jfFFN_HalfCrossover::MakeFinder() const {
  jfOutString res_data;
  res_data << (secname + " Crossovers") << name;
  // done
  return QString(res_data);
}
//----------------------------
QString jfFFN_HalfCrossover::GetTypeID() const {
  return "FFN_HalfCrossover";
}

//*************************************************************************
// constructors
//----------------------------
jfFFN_CrossoverCategory::jfFFN_CrossoverCategory():jfFFN_CategoryCore() {
  cat2_id = 0;
}
//----------------------------
jfFFN_CrossoverCategory::jfFFN_CrossoverCategory(const jfFFN_CrossoverCategory& incat):jfFFN_CategoryCore(incat) {
  cat2_id = incat.cat2_id;
  cat1 = incat.cat1;
  cat2 = incat.cat2;
  section1 = incat.section1;
  section2 = incat.section2;
}
//----------------------------
jfFFN_CrossoverCategory::jfFFN_CrossoverCategory(const QString icatname, size_t icatid, const QString& inraw) {
  QString perr;
  validdata = SetFromSource(icatname,icatid,inraw,perr);
}
//----------------------------
jfFFN_CrossoverCategory::jfFFN_CrossoverCategory(size_t id1, size_t id2) {
  if (id1<id2) {
    num_id = id1;
    cat2_id = id2;
  }
  else {
    num_id = id2;
    cat2_id = id1;
  }
  validdata = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//----------------------------
QString jfFFN_CrossoverCategory::GetCatPart(bool first) const {
  if (first) return cat1;
  else return cat2;
}
//----------------------------
QString jfFFN_CrossoverCategory::GetSectionPart(bool first) const {
  if (first) return section1;
  else return section2;
}
//----------------------------
size_t jfFFN_CrossoverCategory::GetSecondID() const {
  return cat2_id;
}
//----------------------------
bool jfFFN_CrossoverCategory::SetFromSource(const QString icatname, size_t icatid, const QString& inraw, QString& parse_err) {
  /* typical src string for crossover below:
  <a href="/Naruto-and-Pita-Ten-Crossovers/1402/1990/">Pita Ten</a> <SPAN CLASS='gray'>(1)</SPAN>
  */
  // constants and local variables
  const QString fname = "jfFFN_CrossoverCategory::SetFromSource";
  QString linkname,oerr, tempname;
  unsigned long ccount_out;
  jfStringParser xparser;
  // we start...
  xparser.ChangeData(inraw);
  validdata = false;
  // getting the first part of the link
  if (!xparser.GetDelimited("<a href=\"/","/",linkname)) {
    parse_err = "Could not find URL in source data!";
    return false;
  }
  // next up, the ids
  if (!xparser.GetMovePastULong("/",ccount_out,oerr)) {
    parse_err = "Problem with getting first id : " + oerr;
    return false;
  }
  num_id = ccount_out;
  if (!xparser.GetMovePastULong("/",ccount_out,oerr)) {
    parse_err = "Problem with getting second id : " + oerr;
    return false;
  }
  cat2_id = ccount_out;
  if (num_id==cat2_id) {
    parse_err = "Crossover category ids are the same!";
    discard = true; // this actually happens!
    return false;
  }
  // creating the link
  primarylink = "https://www.fanfiction.net/" + linkname + "/";
  primarylink += QString::number(num_id) + "/";
  primarylink += QString::number(cat2_id) + "/";
  // next, one of the names
  if (!xparser.GetDelimited(">","</a>",tempname)) {
    parse_err = "Cannot Extract Name!";
    return false;
  }
  // assigning names
  if (num_id==icatid) {
    cat1 = icatname;
    cat2 = htmlparse::ConvertEntities(tempname,false);
  }
  else if (cat2_id==icatid) {
    cat1 = htmlparse::ConvertEntities(tempname,false);
    cat2 = icatname;
  }
  else {
    parse_err = "Neither category id matches the parent id!";
    return false;
  }
  // creating the main name
  name = cat1 + " and " + cat2 + " Crossovers";
  // next up, we extract the number of items...
  if (xparser.GetDelimitedULong("\"gray\">(","K)",ccount_out,oerr)) ccount = ccount_out*1000;
  else if (xparser.GetDelimitedULong("'gray'>(","K)",ccount_out,oerr)) ccount = ccount_out*1000;
  else if (xparser.GetDelimitedULong("\"gray\">(",")",ccount_out,oerr)) ccount = ccount_out;
  else if (xparser.GetDelimitedULong("'gray'>(",")",ccount_out,oerr)) ccount = ccount_out;
  else {
    parse_err = "Could not find number of items in category! : " + oerr;
    return false;
  }
  // finishing off
  validdata = true;
  // done
  return true;
}
//-----------------------------------------------------------
bool jfFFN_CrossoverCategory::SetSectionByID(size_t idptr, const QString& insection) {
  if (!validdata) return false;
  if (idptr==num_id) section1 = insection;
  else if (idptr==cat2_id) section2 = insection;
  else return false;
  return true;
}
//-----------------------------------------------------------
void jfFFN_CrossoverCategory::CopySections(const jfFFN_CrossoverCategory &source) {
  section1 = source.section1;
  section2 = source.section2;
}
//-----------------------------------------------------------
int jfFFN_CrossoverCategory::GetOtherID(size_t cid) const {
  if (cid==num_id) return cat2_id;
  else if (cid==cat2_id) return num_id;
  else return -1;
}
//-----------------------------------------------------------
QString jfFFN_CrossoverCategory::GetOtherCombo(size_t cid) const {
  if (num_id==cid) return cat2 + " (" + section2 + ")";
  else if (cat2_id==cid) return cat1 + " (" + section1 + ")";
  else {
      assert(false);
  }
  return "";
}
//-----------------------------------------------------------
QString jfFFN_CrossoverCategory::GetOtherSection(size_t cid) const {
  if (num_id==cid) return section2;
  else if (cat2_id==cid) return section1;
  else assert(false);
  return "";
}
//--------------------------------------------------------
bool jfFFN_CrossoverCategory::ContainsID(size_t cid) const {
  return ((cid==num_id) || (cid==cat2_id));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//----------------------------
bool jfFFN_CrossoverCategory::SameCat(const jfFFN_CategoryCore* intest) const {
  if (intest==NULL) return false;
  if ((intest->GetCatType()!=jfx_CROSS)) return false;
  const jfFFN_CrossoverCategory* xtest = dynamic_cast<const jfFFN_CrossoverCategory*>(intest);
  if (num_id!=xtest->GetID()) return false;
  return (cat2_id==xtest->GetSecondID());
}
//----------------------------
jf_FFCTYPE jfFFN_CrossoverCategory::GetCatType() const {
  return jfx_CROSS;
}
//----------------------------
QString jfFFN_CrossoverCategory::MakeFinder() const {
  jfOutString res_data;
  res_data << section1 << cat1 << section2 << cat2;
  // done
  return QString(res_data);
}
//----------------------------
QString jfFFN_CrossoverCategory::GetTypeID() const {
  return "FFN_CrossoverCategory";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
//----------------------------
bool jfFFN_CrossoverCategory::AddRestToFile(QTextStream* outfile) const {
  jfOutString result;
  // checking and special conditions
  if (outfile==NULL) return false;
  // the line will have the 2nd cat id, and the category names
  result << cat2_id << section1 << cat1 << section2 << cat2;
  (*outfile) << QString(result) << "\n";
  return true;
}
//----------------------------
bool jfFFN_CrossoverCategory::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFFN_CrossoverCategory::ReadRestFromFile";
  // reading the line
  assert(infile!=NULL);
  if (!infile->ReadParseLine(5,funcname)) return false;
  // field 0 is the second category id
  if (!infile->lp.SIntVal(0,cat2_id)) return infile->BuildError("Second Category ID is Bad!");
  // field 1 is the first category section name
  section1 = infile->lp.UnEscStr(1);
  // field 2 is the first category name
  cat1 = infile->lp.UnEscStr(2);
  // field 3 is the second category section name
  section2 = infile->lp.UnEscStr(3);
  // field 4 is the second category name
  cat2 = infile->lp.UnEscStr(4);
  // done
  return true;
}

//***************************************************************************

