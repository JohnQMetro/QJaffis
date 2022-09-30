/***************************************************************
 * Name:      initend.cpp
 * Purpose:   Special startup and shutdown methods
 * Author:    John Q Metro
 * Created:   Febuary 13, 2011
  Conversion to QT Started June 23, 2013
 * Updated:   August 27, 2022
 *
 **************************************************************/
#ifndef INITEND_H_INCLUDED
  #include "initend.h"
#endif // INITEND_H_INCLUDED
//---------------------------------
#ifndef HTMLPARSE_H_INCLUDED
  #include "core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
#ifndef SKELCOLL_H_INCLUDED
  #include "core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
#ifndef CONNECT_HELPERS_H
#include "fetching/connect_helpers.h"
#endif // CONNECT_HELPERS_H

#ifndef VECTORS_H_INCLUDED
  #include "core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED

#ifndef CATGROUP_H
  #include "fanficnet/categories/catgroup.h"
#endif // CATGROUP_H

#ifndef JFILTERGLOBALS
  #include "core/filters/filter_globals.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef SEARCHCORE_H_INCLUDED
  #include "core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED

#ifndef AO3_CONSTS_H_INCLUDED
  #include "fanfic/data/ao3/ao3_consts.h"
#endif

#ifndef FFN_LINKDATA_H_INCLUDED
  #include "fanficnet/data/ffn_linkdata.h"
#endif // FFN_LINKDATA_H_INCLUDED

#ifndef AO3_CATS_H_INCLUDED
  #include "fanfic/data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED

#ifndef FIM_CONSTANTS_H
  #include "fanfic/data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H

#ifndef FICEXTRACT_H_INCLUDED
  #include "ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef FICUPDATE_THREAD_H
  #include "ficdown/threads/ficupdate_thread.h"
#endif // FICUPDATE_THREAD_H

#ifndef UTILS3_H_INCLUDED
  #include "core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED

#ifndef HTML_DISPLAY_H_INCLUDED
  #include "interface/display/html_display.h"
#endif // HTML_DISPLAY_H_INCLUDED

#ifndef DEFCOLORS_H
    #include "interface/defcolors.h"
#endif // DEFCOLORS_H

#ifndef DISPLAYHTMLSPEC_H
    #include "fanfic/data/displayhtmlspec.h"
#endif // DISPLAYHTMLSPEC_

#include "core/tagfilter/tagmatch.h"
//----------------------------------
#include <QCoreApplication>
#include <QDir>
#include <iostream>
//****************************************************************
jglobal::jfGlobalSettings jglobal::settings;

// -- [ METHODS for jglobal::AppStartAndStop ] --------------------------
jglobal::AppStartAndStop::AppStartAndStop() {
    ffn_catobtained = false;
}
//--------------------------------------------
bool jglobal::AppStartAndStop::LoadAtStart(){
  // constants and variables
  QString fname = "AppStartAndStop::LoadAtStart";
  bool loadres;
  // start by registering meta types
  emit sendAction("Registering Meta Types");
  QCoreApplication::processEvents();
  RegisterMetaTypes();
  emit sendDone();
  // loading basic data from the settings file
  emit sendString("Looking for the settings file.");
  QCoreApplication::processEvents();
  loadres = settings.OpenSettingsFile();
  if (loadres) {
    emit sendLoading("Basic Settings from file");
    QCoreApplication::processEvents();
    loadres = settings.ParseAndFinishLoad();
    if (loadres) {
      emit sendDone();
      QCoreApplication::processEvents();
    }
  }
  if (!loadres) {
    settings.MakeSettingsDefault();
    settings.paths.MakePathsDefault();
    emit sendString("Settings file not found or malformed. Settings will be the default");
    QCoreApplication::processEvents();
  }
  // logging
  if (settings.DoLogging()) {
    emit sendString("Turning Logging On.");
    QCoreApplication::processEvents();
    jlog::InitLog();
  }
  jerror::InitELog();

  // tag lists
  taglistings::InitializeListsFromFile();

  // html entities
  emit sendLoading("HTML entitites");
  QCoreApplication::processEvents();
  loadres = htmlparse::LoadEntities();
  if (loadres) emit sendDone();
  else sendString("Loading HTML entities failed.");
  QCoreApplication::processEvents();
  if (!loadres) return false;
  // loading HTML Skeletons
  emit sendLoading("Skeletons for HTML Output");
  QCoreApplication::processEvents();
  loadres = LoadSkeletons();
  if (loadres) emit sendDone();
  else sendString("Loading \'Skeletons for HTML Output\' failed.");
  QCoreApplication::processEvents();
  if (!loadres) return false;

  // for global filters, there is a single setup function
  emit sendLoading("Global Filters");
  QCoreApplication::processEvents();
  loadres = filterglobals::LoadStrings();
  if (loadres) emit sendDone();
  else sendString("Loading Global Filters failed.");
  QCoreApplication::processEvents();

  // FanFiction.Net related stuff
  ffn_catorganizer::main_manager = new jfFFN_CatManager();
  emit sendLoading("Fanfiction.Net Fandom Categories");
  QCoreApplication::processEvents();
  ffn_catobtained = ffn_catorganizer::main_manager->ReadFromFile();
  if (ffn_catobtained) emit sendDone();
  else emit sendString("Loading Fanfiction.Net Categories failed: file missing or corrupt.");
  QCoreApplication::processEvents();
  ffn_consts::SetupLists();

  // Archive of our own related stuff

  emit sendLoading("ArchiveOfOurOwn.org Fandoms");
  QCoreApplication::processEvents();
  loadres = ao3_catdata::Initialize();
  if (loadres) emit sendDone();
  else emit sendString("Loading ArchiveOfOurOwn.org Fandoms failed: file missing or corrupt.");
  QCoreApplication::processEvents();

  // Fimfiction.net
  emit sendLoading("Fimfiction.net Tags");
  QCoreApplication::processEvents();
  fimcon::LoadTagsFromFile(fimcon::fimtag_path);
  // also
  defcolour::SetupColours();
  HTMLHelpers::Initialize();
  // LogDirectory(DownloadedFics);

  return true;
}

//--------------------------------------------
bool jglobal::AppStartAndStop::WriteAtFinish() {
  const QString fname = "WriteAtFinish";
  filterglobals::Shutdown();
  WriteSkeletons();
  // delete m_google_info;
  ffn_catorganizer::main_manager->WriteToFile();
  delete ffn_catorganizer::main_manager;
  settings.SaveSettings();
  ao3_catdata::Closing();
  fimcon::WriteTagsToFile(fimcon::fimtag_path);
  if (fimcon::tags != NULL) delete fimcon::tags;
  taglistings::SaveAndClearLists();
  HTMLHelpers::Clear();
  jlog::CloseLog();
  jerror::CloseELog();
  return true;
}

//--------------------------------------------
jfTextStatusDialog* jglobal::AppStartAndStop::MakeLoadDisplay() {
  // constants
  const QString wmsg = "Starting ...";
  const QString cmsg = "QJaffis is Starting";
  // variables
  jfTextStatusDialog* res_dlg;
  QSize dsize(400,80);
  // starting
  res_dlg = new jfTextStatusDialog(NULL, cmsg, wmsg, dsize);
  // connecting
  connect(this,SIGNAL(sendAction(QString)),res_dlg,SLOT(handleAction(QString)));
  connect(this,SIGNAL(sendDone()),res_dlg,SLOT(handleDone()));
  connect(this,SIGNAL(sendLoading(QString)),res_dlg,SLOT(handleLoading(QString)));
  connect(this,SIGNAL(sendString(QString)),res_dlg,SLOT(handleString(QString)));
  // done
  return res_dlg;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
void jglobal::AppStartAndStop::RegisterMetaTypes() {
  qRegisterMetaType<jfItemInfoMsg>("jfItemInfoMsg");
  qRegisterMetaType<jfFetchErrorBundle>("jfFetchErrorBundle");
  // qRegisterMetaType<jfTAG_MODATA>("jfTAG_MODATA");
  qRegisterMetaType<jfResultUnitVector*>("jfResultUnitVector");
  qRegisterMetaType<jfCategoryInfoMsg>("jfCategoryInfoMsg");
  qRegisterMetaType<size_t>("size_t");
  qRegisterMetaType<jfFicExtract*>("jfFicExtract*");
  qRegisterMetaType<jfFETCH_AFTERMATH>("jfFETCH_AFTERMATH");
  qRegisterMetaType<ficur::jfFICUR>("ficur::jfFICUR");
  qRegisterMetaType<jfRClickData>("jfRClickData");
}
//==============================================
jglobal::AppStartAndStop jglobal::startstop;

//===============================================
// changes the working directory to the executable directory
bool ChangeWDir(QString subdir) {
  bool rval;
  // getting the executable directory
  QString expath = QCoreApplication::applicationDirPath();
  // appending subdir (if nonempty)
  if (!subdir.isEmpty()) expath += QDir::separator() + subdir;
  // finally changing the directory
  rval = QDir::setCurrent(expath);
  return rval;
}
//****************************************************************
