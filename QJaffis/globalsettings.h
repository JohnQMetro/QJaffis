/***************************************************************
 * Name:      globalsettings.h
 * Purpose:   A global settings class, with loading and unloading
 * Author:    John Q Metro
 * Created:   July 26, 2021
 * Updated:   August 27, 2022
Sticking in the special paths object to replace the internal one.
 **************************************************************/
#ifndef GLOBALSETTINGS_H
    #define GLOBALSETTINGS_H
#endif // GLOBALSETTINGS_H
//-----------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef DEFAULTPATHS_H
    #include "defaultpaths.h"
#endif // DEFAULTPATHS_H

#ifndef PARSEFETCH_PACKAGEMAKER_H
    #include "fetchparse/parsefetch_packagemaker.h"
#endif // PARSEFETCH_PACKAGEMAKER_H

//------------------------------------
#include <QObject>
#include <QStringList>
#include <QString>
#include <QList>
/**************************************************************/
namespace jglobal {

extern const QString globals_filename;

// class holding global settings, including directory paths
class jfGlobalSettings : public QObject {
    Q_OBJECT
  public:
    // data i've decided to make public
    size_t fimlimits[5];
    bool DoLogging() const;
    void SetDoLogging(bool new_value);
    size_t ficsize_hint;
    jfDefaultPaths paths;
    QList<jfFetchBasics> fetch_types;

    // methods
    jfGlobalSettings();
    void MakeSettingsDefault();
    jfFetchBasics FindFetchTypeFor(jf_FICTYPE site, jfFICPAGE_TYPE page_type) const;

    // python fetch stuff
    QString PythonCommand() const;
    bool SetPythonCommand(const QString& inValue);
    jfParseFetchPackageMaker* getFetchPackageMaker();

    // external load methods
    bool OpenSettingsFile();
    bool ParseAndFinishLoad();
    bool SaveSettings();

    ~jfGlobalSettings();
  protected:
    // helper methods
    bool ParseSettingsFile();
    bool CloseSettingsFile();

    // internal data
    int vers;
    bool dologging;
    jfFileReader* input_file;
    QString python_command;

    jfParseFetchPackageMaker* parse_fetch_package_manager;

};

}
/**************************************************************/
