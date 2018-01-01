/***************************************************************
 * Name:      initend.cpp
 * Purpose:   Special startup and shutdown methods
 * Author:    John Q Metro
 * Created:   Febuary 13, 2011
  Conversion to QT Started June 23, 2013
 * Updated:   December 30, 2017
 *
 **************************************************************/
#ifndef INITEND_H_INCLUDED
  #include "initend.h"
#endif // INITEND_H_INCLUDED
#include <QDir>
#include <assert.h>
#include <QCoreApplication>
#include <QMetaType>

#include <iostream>
#include <QFileInfo>
//---------------------------------------------------------
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
//****************************************************************

const QString jglobal::dirlabels[NUMDIRS] = {"Google Results","Google Save Files",
      "Fanfic Search Results","Fanfic Search Save Files","Downloaded Fanfics",
      "Default Open Directory","Source Directory for Sync","Target Directory for Sync"};
const QString jglobal::globals_filename = "globalvals.dat";
//================================================================
// -- [ METHODS for jglobal::jfGlobalSettings ] --------------------------
// constructor
jglobal::jfGlobalSettings::jfGlobalSettings() {
  globaldirs =NULL;
  input_file = NULL;
  ffn_catobtained = false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jglobal::jfGlobalSettings::MakeSettingsDefault() {
  dologging_next = dologging = true;
  ficsize_hint = 350;
  fimlimits[0] = 2160;  // initial compact page estimate (not really needed anymore)
  fimlimits[1] = 1; // start of first range of group ids
  fimlimits[2] = 2200; // end of first range of group ids
  fimlimits[3] = 197100;  // start of second range of group ids
  fimlimits[4] = 225000;  // end of second range of group ids
}
//--------------------------------------------
void jglobal::jfGlobalSettings::MakeDirectoriesDefault(){
  if (globaldirs != NULL) delete globaldirs;
  globaldirs = new QStringList();
  for (size_t lindex=0 ; lindex < NUMDIRS ; lindex++ ) {
    globaldirs->append(QDir::homePath());
  }
}
//--------------------------------------------
QString jglobal::jfGlobalSettings::GetDirectory(GDIR_TYPE intype) const {
  if (globaldirs==NULL) return "";
  return (*globaldirs)[intype];
}
//--------------------------------------------------
bool jglobal::jfGlobalSettings::SetDirectory(GDIR_TYPE intype, const QString& changeto) {
  if (globaldirs==NULL) return false;
  if  (intype>NUMDIRS) return false;
  (*globaldirs)[intype] = changeto;
  return true;
}
//--------------------------------------------
bool jglobal::jfGlobalSettings::LoadAtStart(){
  // constants and variables
  QString fname = "jfGlobalSettings::LoadAtStart";
  bool loadres;

  // start by registering meta types
  emit sendAction("Registering Meta Types");
  QCoreApplication::processEvents();
  RegisterMetaTypes();
  emit sendDone();

  // loading basic data from the settings file
  emit sendString("Looking for the settings file.");
  QCoreApplication::processEvents();
  loadres = OpenSettingsFile();
  if (loadres) {
    emit sendLoading("Basic Settings from file");
    QCoreApplication::processEvents();
    loadres = ParseSettingsFile();
    if (loadres) {
      TestGlobalDirectories();
      emit sendDone();
      QCoreApplication::processEvents();
    }
    CloseSettingsFile();
  }
  if (!loadres) {
    MakeSettingsDefault();
    MakeDirectoriesDefault();
    emit sendString("Settings file not found or malformed. Settings will be the default");
    QCoreApplication::processEvents();
  }

  // logging
  dologging_next = dologging;
  if (dologging) {
    emit sendString("Turning Logging On.");
    QCoreApplication::processEvents();
    jlog::InitLog();
  }

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

  // Google Stuff (not implemented)
  /*
  emit sendAction("Initializing Google Info");
  QCoreApplication::processEvents();
  m_google_info = new GoogleInfo();
  emit sendDone();
  QCoreApplication::processEvents();
  */


  // Archive of our own related stuff

  emit sendLoading("ArchiveOfOurOwn.org Fandoms");
  QCoreApplication::processEvents();
  ao3con::SetupLists();
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
  // LogDirectory(DownloadedFics);

  return true;
}

//--------------------------------------------
bool jglobal::jfGlobalSettings::WriteAtFinish() {
  const QString fname = "WriteAtFinish";
  filterglobals::Shutdown();
  WriteSkeletons();
  // delete m_google_info;
  ffn_catorganizer::main_manager->WriteToFile();
  delete ffn_catorganizer::main_manager;
  SaveSettings();
  ao3_catdata::Closing();
  fimcon::WriteTagsToFile(fimcon::fimtag_path);
  if (fimcon::tags != NULL) delete fimcon::tags;
  jlog::CloseLog();
  return true;
}

//--------------------------------------------
jfTextStatusDialog* jglobal::jfGlobalSettings::MakeLoadDisplay() {
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
//--------------------------------------------
jglobal::jfGlobalSettings::~jfGlobalSettings(){
  if (input_file != NULL) {
    input_file->CloseFile();
    delete input_file;
  }
  if (globaldirs !=NULL) delete globaldirs;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
void jglobal::jfGlobalSettings::RegisterMetaTypes() {
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

//--------------------------------------------
bool jglobal::jfGlobalSettings::OpenSettingsFile() {
  // starting
  input_file = new jfFileReader(globals_filename);
  if (!input_file->OpenFile()) {
    delete input_file;
    input_file = NULL;
    return false;
  }
  else return true;
}
//--------------------------------------------
bool jglobal::jfGlobalSettings::ParseSettingsFile() {
  const QString fname = "jfGlobalSettings::ParseSettingsFile";

  if (input_file == NULL) return false;
  // reading the smaller settings, like ficsize of fim constants
  if (!input_file->ReadParseLine(2,fname)) return false;
  else if (!(input_file->lp.SIntVal(0,ficsize_hint))) return false;
  else if (!(input_file->lp.BoolVal(1,dologging))) return false;

  if (!input_file->ReadParseLine(5,fname)) return false;
  else if (!(input_file->lp).SIntVal(0,fimlimits[0])) return false;
  else if (!(input_file->lp).SIntVal(1,fimlimits[1])) return false;
  else if (!(input_file->lp).SIntVal(2,fimlimits[2])) return false;
  else if (!(input_file->lp).SIntVal(3,fimlimits[3])) return false;
  else if (!(input_file->lp).SIntVal(4,fimlimits[4])) return false;


  globaldirs = new QStringList();
  bool fail = !input_file->AppendToStringList(globaldirs,-1,fname);
  // handling the result
  if (fail) {
    delete globaldirs;
    globaldirs = NULL;
  }
  return !fail;
}
//--------------------------------------------
bool jglobal::jfGlobalSettings::CloseSettingsFile() {
  if (input_file==NULL) return false;
  input_file->CloseFile();
  delete input_file;
  input_file = NULL;
  return true;
}

//--------------------------------------------
bool jglobal::jfGlobalSettings::SaveSettings(){
  // variables
  QTextStream* ofile;
  jfOutString ostring;
  QString oxstring;
  // starting
  ofile = GetOpenFile(globals_filename,true);
  if (ofile==NULL) return false;
  // making line 1
  ostring << ficsize_hint << dologging_next;
  oxstring = ostring;
  (*ofile) << oxstring << "\n";
  // line 2 : fim constants
  ostring.clear();
  for (size_t dxi = 0; dxi < 5 ; dxi++) ostring << fimlimits[dxi];
  oxstring = ostring;
  (*ofile) << oxstring << "\n";
  // the rest
  AppendAS2File(globaldirs,ofile);
  CloseFileStream(ofile,true);
  delete ofile;
  return true;
}
//----------------------------------------------------------
size_t jglobal::jfGlobalSettings::TestGlobalDirectories() {
  // variables
  size_t rcount = 0;
  QString testval;
  QDir testdir;
  // checking for special case, missing directories
  if ((globaldirs == NULL) || ((globaldirs->size()) < NUMDIRS)) {
    MakeDirectoriesDefault();
    return NUMDIRS;
  }
  // testing
  for ( size_t gindex = 0 ; gindex < COUNT_DIR_MUSTEXIST ; gindex++ ) {
    testval = (*globaldirs)[gindex];
    testdir = QDir(testval);
    if (!testdir.exists()) {
      (*globaldirs)[gindex] = QDir::homePath();
      rcount++;
    }
  }
  return rcount;
}
//++++++++++++++++++++++++++++++++++++++++++++
bool jglobal::jfGlobalSettings::LogDirectory(GDIR_TYPE whichdir) {
  const QString fname = "LogDirectory";
  // bad case
  if(globaldirs == NULL) return false;
  // variables
  QString curdir;
  size_t dirindex, dirmax;
  QStringList dirfile_list;
  QFileInfo curf;
  // setup
  jfLogMessage(fname,1,(*globaldirs)[whichdir]);
  bool getv = GetDirectoryTreeList((*globaldirs)[whichdir],true,dirfile_list);
  if (!getv) return false;
  dirmax = dirfile_list.size();
  // logging the contents of dirfile_list
  for (dirindex = 0 ; dirindex < dirmax ; dirindex++) {
    curdir = dirfile_list.at(dirindex);
    jfXLogString(curdir);
    curf = QFileInfo(curdir);
    jfLogMessageB(fname,2,curf.isDir());
    jfLogMessage(fname,3,curf.canonicalFilePath());
  }
  return false;
}

//===============================================
jglobal::jfGlobalSettings jglobal::settings;
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
