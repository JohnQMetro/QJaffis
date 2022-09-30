/***************************************************************
 * Name:      ficdown_single.cpp
 * Purpose:   Fanfic downloader thread for one fanfic
 * Author:    John Q Metro
 * Created:   July 12, 2016
 * Updated:   July 7, 2022
 *
 **************************************************************/
#ifndef FICDOWN_SINGLE_H
  #include "ficdown_single.h"
#endif // FICDOWN_SINGLE_H
//------------------------------
#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED
//----------------------------
#include <QDir>
/**************************************************************/
// constructors for class to hold fic data passed to the downloader
//----------------------------------------
jfFicDownParameters::jfFicDownParameters() {
  extract = NULL;
  sizeguide = jglobal::settings.ficsize_hint;
  filename_base = "TheFic";
  write_to_directory = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
}
//----------------------------------------
jfFicDownParameters::jfFicDownParameters(jfFicExtract* extract_in) {
  sizeguide = jglobal::settings.ficsize_hint;;
  write_to_directory = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
  extract = extract_in;
  if (extract !=NULL) {
    filename_base = extract->fname_core;
    fictype = extract->GetFicType();
  }
  else filename_base = "TheFic";
}
//==================================
// -- [ METHODS for jfSingleFicDownloader ] ------------------------
// constructor
jfSingleFicDownloader::jfSingleFicDownloader():jfFicDownloaderBase() {
  params = NULL;
}
//---------------------------
// setting data on what to download and how to output it
bool jfSingleFicDownloader::SetFicParameters(jfFicDownParameters* in_ficparams) {
  const QString fname = "jfSingleFicDownloader::SetFicParameters";
  if (in_ficparams == NULL) return false;
  params = in_ficparams;
  fictype = params->fictype;
  fic_data = params->extract;
  if (fic_data==NULL) firsturl = params->firsturl;
  return true;
}
//---------------------------
// doing the action...
void jfSingleFicDownloader::StartProcessing() {
  const QString fname = "jfSingleFicDownloader::StartProcessing";
  initLog("SingleFanfic");
  emit SendStart(true);
  /**/tLog(fname,1);
  started = true;
  SetupWorkers(true);
  bool dresult = xDownloadFic();
  /**/tLogB(fname,2,dresult);
  ClearWorkers(true);
  // done
  AllDone(dresult);
}
//++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-----------------------------------
bool jfSingleFicDownloader::TestForAbort(jfFicExtract* new_extract) const{
  return false; //we never abort...
}
//-----------------------------------
// loads fic output details into the fanfic object
bool jfSingleFicDownloader::SetFileBase() {
  if ((params==NULL)||(fanfic==NULL)) return false;
  fanfic->SetFileBase(params->filename_base,params->write_to_directory,params->sizeguide);
  return true;
}



/**************************************************************/
