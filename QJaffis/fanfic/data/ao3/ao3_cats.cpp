/******************************************************************************
Name    :   ao3_cats.cpp
Author  :   John Q Metro
Purpose :   Category classes for archiveofourown.org
Created :   August 20, 2012
Conversion to Qt Started September 28, 2013
Updated :   July 19, 2017
******************************************************************************/
#ifndef AO3_CATS_H_INCLUDED
  #include "ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//--------------------------------------------------
#include <assert.h>
/*****************************************************************************/
const QString AO3_STYPES = "AO3Section";
//=====================================================
jfAO3_Category::jfAO3_Category():jfGeneralCategory() {
  num_id = 0;
  rcount = 0;
  validdata = false;
}
//-------------------------------------
bool jfAO3_Category::SetFromString(const QString& invalin, const QString& insection) {
  // variables
  QString outerr;
  validdata = false;
  unsigned long tval;
  // basic test
  if (insection.isEmpty()) return false;
  // parsing...
  jfStringParser* sparser = new jfStringParser(invalin);
  if (!sparser->MovePast("<a ")) {
    delete sparser;
    return false;
  }
  if (!sparser->GetDelimited("href=\"","\"",primarylink)) {
    delete sparser;
    return false;
  }
  if (!sparser->GetDelimited(">","</a>",name)) {
    delete sparser;
    return false;
  }
  if (!sparser->GetDelimitedULong("(",")",tval,outerr)) {
    rcount = 1;
  }
  else rcount = tval;
  delete sparser;
  // post processing
  name = htmlparse::HTML2Text(name);
  primarylink.replace("&#x27;","'");
  primarylink = "http://archiveofourown.org" + primarylink;
  // setting values
  validdata = true;
  AddNewSection(insection);
  return true;
}
//-------------------------------------
QString jfAO3_Category::GetTypeID() const {
  return  "AO3Category";
}
//-------------------------------------
size_t jfAO3_Category::GetRCount() const {
  return rcount;
}
//-------------------------------------
QString jfAO3_Category::GetParseErr() const {
  return parse_err;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfAO3_Category::parseErr(const QString& in_err_msg) {
  parse_err = "AO3 Category Parse Error : " + in_err_msg;
  return false;
}

//---------------------------------------
bool jfAO3_Category::AddRestToFile(QTextStream* outfile) const {
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding
  (*outfile) << QString::number(rcount) << "\n";
  return true;
}
//-------------------------------------
bool jfAO3_Category::ReadRestFromFile(jfFileReader* infile) {
  const QString fname="jfAO3_Category::ReadRestFromFile";
  assert(infile!=NULL);
  unsigned long oval;
  if (!infile->ReadULong(oval,fname)) return false;
  rcount = oval;
  return true;
}
//============================================================================
// constructors
//----------------------------------------------
jfAO3Section::jfAO3Section():jfGenSection(false) {}
//----------------------------------------------
jfAO3Section::jfAO3Section(QString& inname):jfGenSection(inname,false) {}
//-----------------------------------------------------
QString jfAO3Section::GetTypeID() const {
  return AO3_STYPES;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
//----------------------------------------------
jfGeneralCategory* jfAO3Section::MakeTypedCategory() const {
  return new jfAO3_Category();
}
//****************************************************************************
const size_t AO3_SECCOUNT = 11;
const QString catnames[AO3_SECCOUNT] = {"Anime & Manga","Books & Literature",
  "Cartoons & Comics & Graphic Novels","Celebrities & Real People",
  "Movies","Music & Bands","Other Media","Theater","TV Shows",
  "Video Games","Uncategorized Fandoms"};
const QString caturls[AO3_SECCOUNT] = {
  "http://archiveofourown.org/media/Anime%20*a*%20Manga/fandoms",
  "http://archiveofourown.org/media/Books%20*a*%20Literature/fandoms",
  "http://archiveofourown.org/media/Cartoons%20*a*%20Comics%20*a*%20Graphic%20Novels/fandoms",
  "http://archiveofourown.org/media/Celebrities%20*a*%20Real%20People/fandoms",
  "http://archiveofourown.org/media/Movies/fandoms",
  "http://archiveofourown.org/media/Music%20*a*%20Bands/fandoms",
  "http://archiveofourown.org/media/Other%20Media/fandoms",
  "http://archiveofourown.org/media/Theater/fandoms",
  "http://archiveofourown.org/media/TV%20Shows/fandoms",
  "http://archiveofourown.org/media/Video%20Games/fandoms",
  "http://archiveofourown.org/media/Uncategorized%20Fandoms/fandoms"
  };
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const QString ao3_catdata::aname = "ao3mmcat.dat";
//---------------------------------
jfGenSecData* ao3_catdata::ao3_nameslinks = NULL;
//-----------------------------------
jfGenCatManager* ao3_catdata::ao3_catmanager = NULL;
//--------------------------------
bool ao3_catdata::Initialize() {
  // the namelinks
  ao3_nameslinks = new jfGenSecData();
  ao3_nameslinks->AddStrings(AO3_SECCOUNT,catnames,caturls,AO3_STYPES);
  // setting up the catmanager
  ao3_catmanager = new jfGenCatManager(aname,"Archive Of Our Own category data");
  return ao3_catmanager->FromFile();
}
//-------------------------------------
void ao3_catdata::Closing() {
  ao3_catmanager->ToFile();
  delete ao3_nameslinks;
  delete ao3_catmanager;
}
/*****************************************************************************/
