/******************************************************************************
Name    :   ficextract.cpp
Author  :   John Q Metro
Purpose :   Defines Basic types for downloading fanfics
Created :   March 16, 2012
Conversion to QT started : April 18, 2013
Updated :   April 18, 2018
******************************************************************************/
#ifndef FICEXTRACT_H_INCLUDED
  #include "ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
//--------------------------------------------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#include <assert.h>
//****************************************************************************
QString Fictype2String(jf_FICTYPE intype) {
  QString result;
  if (intype==jfft_FFN) result="FFN";
  else if (intype==jfft_AO3) result="AO3";
  else if (intype==jfft_MMO) result="MMO";
  else if (intype==jfft_FIM) result="FIM";
  else if (intype==jfft_HPF) result="HPF";
  else result = "UNKNOWN!";
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool String2FicType(const QString& inval, jf_FICTYPE& resval) {
  bool result = true;
  if (inval=="FFN") resval = jfft_FFN;
  else if (inval=="AO3") resval=jfft_AO3;
  else if (inval=="MMO") resval=jfft_MMO;
  else if (inval=="FIM") resval=jfft_FIM;
  else if (inval=="HPF") resval=jfft_HPF;
  else result = false;
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool SiteHasIndexpage(const jf_FICTYPE intype) {
  return (intype==jfft_MMO) || (intype == jfft_FIM) || (intype == jfft_HPF);
}

//========================================================================
// step 1 in string to fic extract conversion
jfLineParse* ParseExtractSource(const QString& inval, jf_FICTYPE& resval) {
  // variables
  jfLineParse* lparser;
  QString typestring;
  // starting
  lparser = new jfLineParse(inval);
  if (!lparser->ReturnByTag("FICTYPE",typestring)) resval = jfft_FFN;
  else {
    if (!String2FicType(typestring,resval)) {
      delete lparser;
      return NULL;
    }
  }
  return lparser;
}
//****************************************************************************
// constructors
//-------------------------------
jfFicExtract::jfFicExtract() {
  complete = false;
  fic_id = 0;
  fnamelist = NULL;
  pcount = 0;
  auth_id = 0;
}
//-------------------------------
jfFicExtract::jfFicExtract(const jfFicExtract& source) {
  fic_title = source.fic_title;
  fic_id = source.fic_id;
  author_name = source.author_name;
  auth_id = source.auth_id;
  pcount =source.pcount;
  updated_date = source.updated_date;
  complete = source.complete;
  partnames = source.partnames;
  fnamelist = NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFicExtract::LoadCoreValues(jfSkeletonParser* inparser) const {
  assert(inparser!=NULL);
  inparser->AddText("FIC_TITLE",fic_title);
  inparser->AddText("AUTHOR_NAME",author_name);
  inparser->AddUInt("AUTH_ID",auth_id);
  inparser->AddUInt("FIC_ID",fic_id);
  inparser->AddUInt("FIC_PARTCOUNT",pcount);
  inparser->AddText("FIC_STARTURL",FirstUrl());
  inparser->AddText("AUTH_URL",MakeAuthlink());
  inparser->AddText("UPDATED_DATE",updated_date.toString("MM-dd-yyyy"));
  inparser->AddText("TEXT_FORM",MakeTextForm());
  inparser->AddBool("COMPLETE",complete);
  if (!partnames.isEmpty()) {
    // using REVERSED SEMICOLON as a separator should be safe...
    inparser->AddText("PARTNAMES",partnames.join(QChar(0x204F)));
  }
  LoadMoreValues(inparser);
}
//---------------------------------------------------------------------------
/* constains a text representation intended to be embedded in an HTML file */
QString jfFicExtract::MakeTextForm() const {
  // variables
  QString result;
  result = "<!--\n";
  result += STARTINFO_HEADER + "\n";
  result += "FICTYPE;" + Fictype2String(GetFicType());
  result += ";FIC_TITLE;" + fic_title;
  result += ";AUTHOR_NAME;" + author_name;
  result += ";AUTH_ID;" + QString::number(auth_id);
  result += ";FIC_ID;" + QString::number(fic_id);
  result += ";FIC_PARTCOUNT;" + QString::number(pcount);
  result += ";UPDATED_DATE;" + updated_date.toString("MM-dd-yyyy");
  result += ";FNAMECORE;" + fname_core;
  result += ";COMPLETE;" + Bool2Str(complete);
  // result += ";PARTNAMES;" + partnames.join(QChar(0x204F));
  result += MoreTextForm();
  result += QString("\n -->\n");
  // done
  return result;
}
//-----------------------------------------------------------------
bool jfFicExtract::GetFromText(QString inval) {
  // variables
  jfLineParse* lparser;
  // starting
  lparser = new jfLineParse(inval);
  if (!GetFromParse(lparser)) {
    delete lparser;
    return false;
  }
  delete lparser;
  return true;
}
//------------------------------------------------------------------
bool jfFicExtract::GetFromParse(jfLineParse* lparser) {
  // constants
  const QString fname = "jfFicExtract::GetFromParse";
  // variables
  QString Dateval,part_names;
  // getting values
  if (!lparser->ReturnByTag("FIC_TITLE",fic_title)) return false;
  if (!lparser->ReturnByTag("AUTHOR_NAME",author_name)) return false;
  if (!lparser->ReturnByTagS("AUTH_ID",auth_id)) return false;
  if (!lparser->ReturnByTagS("FIC_ID",fic_id)) return false;
  if (!lparser->ReturnByTagS("FIC_PARTCOUNT",pcount)) return false;
  if (!lparser->ReturnByTag("UPDATED_DATE",Dateval)) return false;
  // converting the date
  updated_date = QDate::fromString(Dateval,"MM-dd-yyyy");
  if (!updated_date.isValid()) return false;
  // the rest of the values
  if (!lparser->ReturnByTag("FNAMECORE",fname_core)) return false;
  if (!lparser->ReturnByTagB("COMPLETE",complete)) complete = false;
  // partnames are optional
  /*
  if (lparser->ReturnByTag("PARTNAMES",part_names)) {
    partnames = part_names.split(QChar(0x204F));
    if ((partnames.isEmpty()) && (pcount==1)) partnames.append("");
  }
  */
  if (!MoreFromText(lparser)) return false;
  return true;
}
//------------------------------------------------------------------
QString jfFicExtract::ToLabelString() {
  QString resval = fic_title + " by " + author_name + ", ";
  resval += QString::number(pcount) + " Part";
  if (pcount>1) resval+= "s";
  resval += ", at ";
  jf_FICTYPE ft = GetFicType();
  if (ft==jfft_FFN) resval += "Fanfiction.Net";
  else if (ft==jfft_FIM) resval += "Fimfiction.Net";
  else if (ft==jfft_MMO) resval += "MediaMiner.org";
  else if (ft==jfft_AO3) resval += "An Archive of our Own";
  else if (ft==jfft_HPF) resval += "HarryPotterFanficion.com";
  else resval += "Unknown";
  resval += ".";
  return resval;
}
//----------------------------------------------------------------
QString jfFicExtract::NameForPart(size_t index) const {
  const QString fname = "jfFicExtract::NameForPart";
  if (pcount != partnames.size()) return "";
  /**/JDEBUGLOGST(fname,2,index)
  assert(index<=pcount);
  /**/JDEBUGLOG(fname,3)
  assert(index!=0);
  return partnames.at(index-1);
}
//===========================================================================
// constructors
//-------------------------
jfFicExtract_FFN::jfFicExtract_FFN():jfFicExtract() {}
//--------------------------
jfFicExtract_FFN::jfFicExtract_FFN(const jfFicExtract_FFN& source):jfFicExtract(source) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFicExtract_FFN::MakeUrl(size_t inval) const {
  assert(inval>0);
  QString result = "https://www.fanfiction.net/s/";
  result += QString::number(fic_id) + "/";
  result += QString::number(inval) + "/";
  return result;
}
//--------------------------------------------------------------------
QString jfFicExtract_FFN::FirstUrl() const {
  return MakeUrl(1);
}
//-----------------------------------------------------------------------
QString jfFicExtract_FFN::MakeAuthlink() const {
  QString result = "https://www.fanfiction.net/u/";
  result += QString::number(auth_id) + "/";
  return result;
}
//-----------------------------------------------------------------------
jf_FICTYPE jfFicExtract_FFN::GetFicType() const {
  return jfft_FFN;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods to augment core methods
//------------------------------------------------------------------------
void jfFicExtract_FFN::LoadMoreValues(jfSkeletonParser* inparser) const {
}
//-----------------------------------------------------------------------
QString jfFicExtract_FFN::MoreTextForm() const {
  return "";
}
//-----------------------------------------------------------------------
bool jfFicExtract_FFN::MoreFromText(jfLineParse* lparser_in) {
  assert(lparser_in!=NULL);
  return true;
}
//===========================================================================
// constructors
//----------------------
jfFicExtract_FIM::jfFicExtract_FIM():jfFicExtract() {
  wordcount = 0;
}
//--------------------
jfFicExtract_FIM::jfFicExtract_FIM(const jfFicExtract_FIM& source):jfFicExtract(source) {
  wordcount = source.wordcount;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFicExtract_FIM::MakeUrl(size_t inval) const {
  assert(inval>0);
  QString result = FirstUrl();
  result += QString::number(inval) + "/";
  return result;
}
//-------------------------------------------------------------------
QString jfFicExtract_FIM::FirstUrl() const {
  QString result = "https://www.fimfiction.net/story/";
  result += QString::number(fic_id) + "/";
  return result;
}
//-----------------------------------------------------------------------
QString jfFicExtract_FIM::MakeAuthlink() const {
  QString result = "https://www.fimfiction.net/user/";
  result += author_name;
  return result;
}
//-----------------------------------------------------------------------
jf_FICTYPE jfFicExtract_FIM::GetFicType() const {
  return jfft_FIM;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods to augment core methods
//------------------------------------------------------------------------
void jfFicExtract_FIM::LoadMoreValues(jfSkeletonParser* inparser) const {
  assert(inparser!=NULL);
  inparser->AddUInt("WORDCOUNT",wordcount);
}
//-----------------------------------------------------------------------
QString jfFicExtract_FIM::MoreTextForm() const {
  QString result = ";WORDCOUNT;" + QString::number(wordcount);
  return result;
}
//-----------------------------------------------------------------------
bool jfFicExtract_FIM::MoreFromText(jfLineParse* lparser_in) {
  assert(lparser_in!=NULL);
  if (!lparser_in->ReturnByTagS("WORDCOUNT",wordcount)) wordcount = 0;
  return true;
}
//========================================================================
// constructors
//-----------------------------
jfFicExtract_cids::jfFicExtract_cids():jfFicExtract() {}
//-----------------------------
jfFicExtract_cids::jfFicExtract_cids(const jfFicExtract_cids& source):jfFicExtract(source) {
  size_t xcount = source.chapterids.size();
  for(size_t xindex =0; xindex<xcount; xindex++) {
    chapterids.push_back(source.chapterids[xindex]);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods to augment core methods
//------------------------------------------------------------------------
void jfFicExtract_cids::LoadMoreValues(jfSkeletonParser* inparser) const {
  assert(inparser!=NULL);
  QString cidvalue;
  cidvalue = chapterids.ToDelimString(',');
  inparser->AddText("CHAPTER_IDS",cidvalue);
  LoadEvenMoreValues(inparser);
}
//-----------------------------------------------------------------------
QString jfFicExtract_cids::MoreTextForm() const {
  QString result = ";CHAPTER_IDS;" + chapterids.ToDelimString(',');
  result += EvenMoreTextForm();
  return result;
}
//-----------------------------------------------------------------------
bool jfFicExtract_cids::MoreFromText(jfLineParse* lparser_in) {
  assert(lparser_in!=NULL);
  QString srcbuf;
  bool qval;
  if (!lparser_in->ReturnByTag("CHAPTER_IDS",srcbuf)) return false;
  qval = chapterids.SetFromString(srcbuf,',');
  if (!qval) return false;
  return EvenMoreFromText(lparser_in);
}
//===========================================================================
// constructors
jfFicExtract_HPF::jfFicExtract_HPF():jfFicExtract_cids() {}
//-----------------------------------
jfFicExtract_HPF::jfFicExtract_HPF(const jfFicExtract_HPF& source):jfFicExtract_cids(source) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods
//-----------------------------------------------------
QString jfFicExtract_HPF::MakeUrl(size_t inval) const {
  QString result = "https://harrypotterfanfiction.com/viewstory.php?chapterid=";
  assert(inval<=chapterids.size());
  result += QString::number(chapterids[inval-1]);
  return result + "&showRestricted";
}
//-----------------------------------------------------
QString jfFicExtract_HPF::FirstUrl() const {
  QString result = "https://harrypotterfanfiction.com/viewstory.php?psid=";
  result += QString::number(fic_id);
  return result + "&showRestricted";
}
//-----------------------------------------------------
QString jfFicExtract_HPF::MakeAuthlink() const {
  QString result = "https://harrypotterfanfiction.com/viewuser.php?showuid=";
  result += QString::number(auth_id);
  return result + "&showRestricted";
}
//-----------------------------------------------------
jf_FICTYPE jfFicExtract_HPF::GetFicType() const {
  return jfft_HPF;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// more virtual methods to augment core methods
//-----------------------------------------------------
void jfFicExtract_HPF::LoadEvenMoreValues(jfSkeletonParser* inparser) const {
}
//-----------------------------------------------------
QString jfFicExtract_HPF::EvenMoreTextForm() const {
  return "";
}
//-----------------------------------------------------
bool jfFicExtract_HPF::EvenMoreFromText(jfLineParse* lparser_in) {
  assert(lparser_in!=NULL);
  return true;
}
//===========================================================================
// constructors
//---------------------------
jfFicExtract_MMO::jfFicExtract_MMO():jfFicExtract_cids() {}
//---------------------------
jfFicExtract_MMO::jfFicExtract_MMO(const jfFicExtract_MMO& source):jfFicExtract_cids(source) {
  url_cname_part = source.url_cname_part;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods
//-----------------------------------------------------
QString jfFicExtract_MMO::MakeUrl(size_t inval) const {
  QString result = "https://www.mediaminer.org/fanfic/";
  assert(inval<=chapterids.size());
  result += "c/" + url_cname_part;
  result += "/" + QString::number(fic_id);
  result += "/" + QString::number(chapterids[inval-1]);
  return result;
}
//-----------------------------------------------------
QString jfFicExtract_MMO::FirstUrl() const {
  QString result = "https://www.mediaminer.org/fanfic/";
  result += "s/" + url_cname_part;
  result += "/" + QString::number(fic_id);
  return result;
}
//-----------------------------------------------------
QString jfFicExtract_MMO::MakeAuthlink() const {
  QString result = "https://www.mediaminer.org/user_info.php/";
  result += QString::number(auth_id);
  return result;
}
//-----------------------------------------------------
jf_FICTYPE jfFicExtract_MMO::GetFicType() const {
  return jfft_MMO;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// more virtual methods to augment core methods
//-----------------------------------------------------
void jfFicExtract_MMO::LoadEvenMoreValues(jfSkeletonParser* inparser) const {
  assert(inparser!=NULL);
  inparser->AddText("URLCNAMEPART",url_cname_part);
}
//-----------------------------------------------------
QString jfFicExtract_MMO::EvenMoreTextForm() const {
  return ";URLCNAMEPART;" + url_cname_part;
}
//-----------------------------------------------------
bool jfFicExtract_MMO::EvenMoreFromText(jfLineParse* lparser_in) {
  assert(lparser_in!=NULL);
  if (!lparser_in->ReturnByTag("URLCNAMEPART",url_cname_part)) return false;
  return true;
}
//===========================================================================
// constructors
//-----------------------------
jfFicExtract_AO3::jfFicExtract_AO3():jfFicExtract_cids() {}
//-----------------------------
jfFicExtract_AO3::jfFicExtract_AO3(const jfFicExtract_AO3& source):jfFicExtract_cids(source) {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// virtual methods
//-----------------------------------------------------
QString jfFicExtract_AO3::MakeUrl(size_t inval) const {
  QString result = "https://archiveofourown.org/works/";
  assert(inval<=chapterids.size());
  result += QString::number(fic_id) + "/";
  if (pcount!=1) {
    result += "chapters/";
    result += QString::number(chapterids[inval-1]);
  }
  result += "?view_adult=true";
  return result;
}
//-----------------------------------------------------
QString jfFicExtract_AO3::FirstUrl() const {
  QString result;
  result = "https://archiveofourown.org/works/";
  result += QString::number(fic_id) + "?view_adult=true";
  return result;
}
//-----------------------------------------------------
QString jfFicExtract_AO3::MakeAuthlink() const {
  QString result = "https://archiveofourown.org/users/";
  result += author_name + "/";
  return result;
}
//-----------------------------------------------------
jf_FICTYPE jfFicExtract_AO3::GetFicType() const {
  return jfft_AO3;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// more virtual methods to augment core methods
//-----------------------------------------------------
void jfFicExtract_AO3::LoadEvenMoreValues(jfSkeletonParser* inparser) const {
}
//-----------------------------------------------------
QString jfFicExtract_AO3::EvenMoreTextForm() const {
  return "";
}
//-----------------------------------------------------
bool jfFicExtract_AO3::EvenMoreFromText(jfLineParse* lparser_in) {
  assert(lparser_in!=NULL);
  return true;
}
//==========================================================================
jfFicExtract* CopyExtract(jfFicExtract* source) {
  jfFicExtract_AO3* ao3_temp;
  jfFicExtract_FIM* fim_temp;
  jfFicExtract_FFN* ffn_temp;
  jfFicExtract_HPF* hpf_temp;
  jfFicExtract_MMO* mmo_temp;

  if (source ==NULL) return NULL;
  if (source->GetFicType() == jfft_FFN) {
    ffn_temp = dynamic_cast<jfFicExtract_FFN*>(source);
    return new jfFicExtract_FFN(*ffn_temp);
  }
  else if (source->GetFicType() == jfft_AO3) {
    ao3_temp = dynamic_cast<jfFicExtract_AO3*>(source);
    return new jfFicExtract_AO3(*ao3_temp);
  }
  else if (source->GetFicType() == jfft_FIM) {
    fim_temp = dynamic_cast<jfFicExtract_FIM*>(source);
    return new jfFicExtract_FIM(*fim_temp);
  }
  else if (source->GetFicType() == jfft_HPF) {
    hpf_temp = dynamic_cast<jfFicExtract_HPF*>(source);
    return new jfFicExtract_HPF(*hpf_temp);
  }
  else if (source->GetFicType() == jfft_MMO) {
    mmo_temp = dynamic_cast<jfFicExtract_MMO*>(source);
    return new jfFicExtract_MMO(*mmo_temp);
  }
  else assert(false);
  return NULL;
}

//****************************************************************************
