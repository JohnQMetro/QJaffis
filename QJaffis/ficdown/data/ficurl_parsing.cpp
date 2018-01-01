/****************************************************************************
Name:        ficurl_parsing.cpp
Author  :    John Q Metro
Purpose :    When given a fic url to download, it has to be checked
Created :    July 8, 2016
Updated :    July 12, 2016 (conversionto Qt)

****************************************************************************/
#ifndef FICURL_PARSING_H_INCLUDED
  #include "ficurl_parsing.h"
#endif // FICURL_PARSING_H_INCLUDED
//----------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
/***************************************************************************/
const QString FFN_TAG = "www.fanfiction.net";
const QString FIM_TAG = "www.fimfiction.net";
const QString AO3_TAG = "archiveofourown.org";
const QString MMO_TAG = "www.mediaminer.org";
const QString HPF_TAG = "www.harrypotterfanfiction.com";

//================================================================================
// constructor
jfFicURLParser::jfFicURLParser() {
  validURL = false;
  type = jfft_FFN;
}
//++++++++++++++++++++++++++++++++++++++
// data in
bool jfFicURLParser::ParseURL(const QString& input_url) {
  // variables
  bool okay = true;
  // initial setup
  validURL = false;
  inputURL = input_url;
  fname_base.clear();
  QStringList* slashparsed = ListToStrArr(input_url,'/',false);
  if (slashparsed==NULL) return false;
  // shipping possible stuff before the domain...
  if (!ProcessStart(slashparsed,writehttp)) {
    delete slashparsed;
    return false;
  }
  // checking the domain
  QString domain = slashparsed->at(0);
  slashparsed->removeFirst();
  // fanfiction.Net
  if      (domain == FFN_TAG) okay = HandleFFN(slashparsed);
  else if (domain == FIM_TAG) okay = HandleFIM(slashparsed);
  else if (domain == AO3_TAG) okay = HandleAO3(slashparsed);
  else if (domain == MMO_TAG) okay = HandleMMO(slashparsed);
  else if (domain == HPF_TAG) okay = HandleHPF(slashparsed);
  else okay = false;

  // finishing
  delete slashparsed;
  validURL = okay;
  return okay;
}
//++++++++++++++++++++++++++++++++++++++
// data out
//---------------------------
bool jfFicURLParser::isValid() const {
  return validURL;
}
//---------------------------
jf_FICTYPE jfFicURLParser::getType() const {
  return type;
}
//---------------------------
QString jfFicURLParser::firstURL() {
  return outputURL;
}
//---------------------------
QString jfFicURLParser::FilenameBase() {
  return fname_base;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFicURLParser::ProcessStart(QStringList* urlarr, bool& writehttp) {
  if ((urlarr->count()) < 2) return false;
  writehttp = true;
  // the first item might be the protocol
  QString item0 = urlarr->at(0);
  if ((item0 == "http:") || (item0 == "https:")) {
    outputURL = item0 + "//";
    writehttp = false;
    urlarr->removeFirst();
    urlarr->removeFirst();
    if ((urlarr->count()) < 2)  return false;
  }
  // it might be an empty protocol or one slash (we will treat this as okay)
  else if (item0 == "") {
    urlarr->removeFirst();
    if ((urlarr->at(0)) == "") urlarr->removeFirst();
  }
  // done, there must still be parts left
  return ((urlarr->count()) >= 2);
}
//--------------------------------------------------
bool jfFicURLParser::HandleFFN(QStringList* urlarr) {
  // variables
  QString buffer,aftdomain;
  unsigned long tval;
  // starting...
  type = jfft_FFN;
  aftdomain = urlarr->at(0);
  if (aftdomain != "s") return false;
  if (writehttp) outputURL = "https://";
  outputURL += FFN_TAG;
  // checking to see if we have a fic id
  if ((urlarr->count()) < 2) return false;
  // validating the fic id
  buffer = urlarr->at(1);
  if (!Str2ULong(buffer,tval)) return false;
  // building the result...
  outputURL += "/s/" + buffer;
  outputURL += "/1/";
  // there can be a name at the end
  if ((urlarr->count()) >= 4) {
    fname_base = urlarr->at(3);
    outputURL += fname_base;
  }
  return true;
}
//--------------------------------------------------------------
bool jfFicURLParser::HandleFIM(QStringList* urlarr) {
  // variables
  QString buffer,aftdomain;
  unsigned long tval;
  size_t ucount;
  // starting...
  type = jfft_FIM;
  aftdomain = urlarr->at(0);
  if (aftdomain != "story") return false;
  ucount = urlarr->count();
  if (writehttp) outputURL = "http://";
  outputURL += FIM_TAG;
  // checking to see if we have a fic id
  if (ucount < 2) return false;
  // validating the fic id
  buffer = urlarr->at(1);
  if (!Str2ULong(buffer,tval)) return false;
  // building the result...
  outputURL += "/story/" + buffer;
  outputURL += "/";
  // there might be a name afterwards
  if (ucount >= 3) {
    buffer = urlarr->at(2);
    if (!Str2ULong(buffer,tval)) {
      fname_base = buffer;
    }
    else if (ucount >= 4) {
      fname_base = urlarr->at(3);
    }
    outputURL += fname_base;
  }
  return true;
}
//-------------------------------------------------
bool jfFicURLParser::HandleAO3(QStringList* urlarr) {
  // variables
  QString buffer1,buffer2,aftdomain;
  unsigned long tval;
  int spos;
  // starting...
  type = jfft_AO3;
  aftdomain = urlarr->at(0);
  if (aftdomain != "works") return false;
  if (writehttp) outputURL = "http://";
  outputURL += AO3_TAG;
  // checking to see if we have a fic id
  size_t ucount = urlarr->count();
  if (ucount < 2) return false;
  // validating the fic id
  buffer1 = urlarr->at(1);
  spos = buffer1.indexOf('?');
  if (spos >= 0) buffer2 = buffer1.left(spos);
  else buffer2 = buffer1;
  if (!Str2ULong(buffer2,tval)) return false;
  // building the result...
  outputURL += "/works/" + buffer2;
  outputURL += "?view_adult=true";
  // names are not included
  return true;
}
//----------------------------------------------
bool jfFicURLParser::HandleHPF(QStringList* urlarr) {
  // variables and constants
  const QString vsPREFIX = "viewstory.php?psid=";
  QString aftdomain,buffer;
  unsigned long tval;
  // starting...
  type = jfft_HPF;
  aftdomain = urlarr->at(0);
  if (writehttp) outputURL = "http://";
  outputURL += HPF_TAG + "/";
  // harry potter fanfic urls must be index pages
  // format "viewstory.php?psid=<num>"
  if (!aftdomain.startsWith(vsPREFIX)) return false;
  buffer = aftdomain.mid(vsPREFIX.length());
  // checking the fic id
  if (!Str2ULong(buffer,tval)) return false;
  // building the result...
  outputURL += vsPREFIX + buffer;
  // names are not included
  return true;
}
//-----------------------------------------------
bool jfFicURLParser::HandleMMO(QStringList* urlarr) {
  // variables
  QString buffer,aftdomain;
  unsigned long tval;
  size_t ucount;
  // starting...
  type = jfft_MMO;
  aftdomain = urlarr->at(0);
  if (aftdomain != "fanfic") return false;
  ucount = urlarr->count();
  if (ucount < 5) return false;
  if (writehttp) outputURL = "http://";
  outputURL += MMO_TAG;
  // the fourth item can be used as the fic name
  fname_base = urlarr->at(3);
  // checking the fic id
  buffer = urlarr->at(4);
  if (!Str2ULong(buffer,tval)) return false;
  // building the result...
  outputURL += "/fanfic/s/" + urlarr->at(2);
  outputURL += "/" + fname_base;
  outputURL += "/" + buffer;
  return true;
}

/***************************************************************************/
