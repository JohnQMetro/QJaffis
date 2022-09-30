/***************************************************************
 * Name:      globalsettings.cpp
 * Purpose:   Special startup and shutdown methods
 * Author:    John Q Metro
 * Created:   July 25, 2021
 * Updated:   August 27, 2022
 * Split from initend.cpp
 **************************************************************/
#ifndef GLOBALSETTINGS_H
    #include "globalsettings.h"
#endif // GLOBALSETTINGS_H
//--------------------------
#ifndef UTILS3_H_INCLUDED
  #include "core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//--------------------------
#include <QDir>

//****************************************************************

const QString jglobal::globals_filename = "config/globalvals.dat";

//================================================================
// -- [ METHODS for jglobal::jfGlobalSettings ] --------------------------
// constructor
jglobal::jfGlobalSettings::jfGlobalSettings() {
  input_file = NULL;
  vers = 2;
  parse_fetch_package_manager = NULL;
}
bool jglobal::jfGlobalSettings::DoLogging() const {
    return dologging;
}
void jglobal::jfGlobalSettings::SetDoLogging(bool new_value) {
    dologging = new_value;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jglobal::jfGlobalSettings::MakeSettingsDefault() {
  dologging = true;
  ficsize_hint = 350;
  fimlimits[0] = 2160;  // initial compact page estimate (not really needed anymore)
  fimlimits[1] = 1; // start of first range of group ids
  fimlimits[2] = 2200; // end of first range of group ids
  fimlimits[3] = 197100;  // start of second range of group ids
  fimlimits[4] = 225000;  // end of second range of group ids

  python_command = "python";
}
//--------------------------
jglobal::jfFetchBasics jglobal::jfGlobalSettings::FindFetchTypeFor(jf_FICTYPE site, jfFICPAGE_TYPE page_type) const {
    for (int i = 0; i < fetch_types.size(); i++) {
        if (fetch_types.at(i).ForType(site, page_type)) return fetch_types.at(i);
    }
    // general is the fallback if a more specific type is not found
    if (page_type != FPT_GENERAL) {
        for (int i = 0; i < fetch_types.size(); i++) {
            if (fetch_types.at(i).ForType(site, FPT_GENERAL)) return fetch_types.at(i);
        }
    }
    // non valid value
    return jfFetchBasics();
}
//++++++++++++++++++++++++++++++++++++++++++++++
// python fetch stuff
QString jglobal::jfGlobalSettings::PythonCommand() const {
    return python_command;
}

bool jglobal::jfGlobalSettings::SetPythonCommand(const QString& inValue) {
    if (inValue.isEmpty()) return false;
    python_command = inValue;
    if (parse_fetch_package_manager != NULL) {
        parse_fetch_package_manager->newPythonCommand(inValue);
    }
    return true;
}
jfParseFetchPackageMaker* jglobal::jfGlobalSettings::getFetchPackageMaker() {
    return parse_fetch_package_manager;
}

//++++++++++++++++++++++++++++++++++++++++++++++
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
bool jglobal::jfGlobalSettings::ParseAndFinishLoad() {
    bool loadres = ParseSettingsFile();
    if (loadres) {
      paths.MakeInvalidPathDefault();
      parse_fetch_package_manager = new jfParseFetchPackageMaker(python_command, "transfer");
    }
    CloseSettingsFile();
    return loadres;
}
//-------------------------------------------
bool jglobal::jfGlobalSettings::SaveSettings(){
  // variables
  QTextStream* ofile;
  jfOutString ostring;
  QString oxstring;
  // starting
  ofile = GetOpenFile(globals_filename,true);
  if (ofile==NULL) return false;
  // version line 0
  (*ofile) << "V2" << "\n";
  // making line 1 (fic size hint, logging flag, extra flag)
  ostring << ficsize_hint << dologging << false;
  oxstring = ostring;
  (*ofile) << oxstring << "\n";
  // line 2 : fim constants
  ostring.FullClear();
  for (size_t dxi = 0; dxi < 5 ; dxi++) ostring << fimlimits[dxi];
  oxstring = ostring;
  (*ofile) << oxstring << "\n";
  // line 3: python stuff
  ostring.FullClear();
  ostring << python_command;
  oxstring = ostring;
  (*ofile) << oxstring << "\n";

  // fetch defines
  ostring.FullClear();
  (*ofile) << fetch_types.length() << "\n";
  for (int fdi = 0; fdi < fetch_types.length(); fdi++) {
      (*ofile) << fetch_types.at(fdi).ToIOString() << "\n";
  }

  // finally. the default folders
  paths.AddPathsTo(ofile);
  // closing
  CloseFileStream(ofile,true);
  delete ofile;
  return true;
}
//--------------------------------------------
jglobal::jfGlobalSettings::~jfGlobalSettings(){
  if (input_file != NULL) {
    input_file->CloseFile();
    delete input_file;
  }
  if (parse_fetch_package_manager != NULL) {
      delete parse_fetch_package_manager;
      parse_fetch_package_manager = NULL;
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods

//--------------------------------------------
bool jglobal::jfGlobalSettings::ParseSettingsFile() {
  const QString fname = "jfGlobalSettings::ParseSettingsFile";
  if (input_file == NULL) return false;
  // checking if we have a 'version' setting
  QString line1;
  if (!input_file->ReadLine(line1, fname)) return false;
  if (line1.startsWith("V")) {
      bool ok;
      int num = line1.mid(1).toInt(&ok);
      if (!ok) return false;
      vers = num;
  }
  else {
      if (!input_file->Backup(fname)) return false;
      vers = 0;
  }

  // reading the smaller settings, like logging, fic size
  if (!input_file->ReadParseLine(3,fname)) return false;
  else if (!(input_file->lp.SIntVal(0,ficsize_hint))) return false;
  else if (!(input_file->lp.BoolVal(1,dologging))) return false;
  // fim constants
  if (!input_file->ReadParseLine(5,fname)) return false;
  for (size_t dxi = 0; dxi < 5 ; dxi++) {
      if (!(input_file->lp.SIntVal(dxi,fimlimits[dxi]))) return false;
  }
  // python stuff
  if (!input_file->ReadUnEsc(python_command,fname)) return false;

  // fetch defines
  ulong num_defines;
  if (!input_file->ReadULong(num_defines,fname)) return false;
  QString infd;
  jfFetchBasics fetch_item;
  for (size_t fdi = 0; fdi < num_defines; fdi++) {
      if (!input_file->ReadUnEsc(infd, fname)) return false;
      fetch_item = jfFetchBasics();
      if (fetch_item.SetFromSCString(infd)) {
          fetch_types.append(fetch_item);
      }
      else continue;
  }

  // global folders
  if (!paths.LoadPathsFrom(input_file)) return false;
  // finishing off
    return true;
}
//--------------------------------------------
bool jglobal::jfGlobalSettings::CloseSettingsFile() {
  if (input_file==NULL) return false;
  input_file->CloseFile();
  delete input_file;
  input_file = NULL;
  return true;
}


//****************************************************************
