/***************************************************************
 * Name:      initend.h
 * Purpose:   A global settings class, with loading and unloading
 * Author:    John Q Metro
 * Created:   Febuary 13, 2011
 Conversion to QT Started June 23, 2013
 * Updated:   July 18, 2016
 * rewritten from a collection of functions and variables starting
 * July 18, 2016
 **************************************************************/
#ifndef INITEND_H_INCLUDED
  #define INITEND_H_INCLUDED
#endif // INITEND_H_INCLUDED
//------------------------------
#ifndef WIDGETS1_H_INCLUDED
  #include "interface/misc/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
//------------------------------
#include <QObject>
#include <QStringList>
#include <QString>
//****************************************************************
/* Generally used for default directories...
0: Google Results
1: Google Save Files
2: FFN Results
3: FFN Save Files
4: FFN Saved Fics
*/
namespace jglobal {

  enum GDIR_TYPE { GoogleResults = 0, GoogleFiles = 1, FanficSearchResults =2,
                   FanficSaveFiles =3, DownloadedFics = 4, OpenFile = 5,
                   SourceForSync = 6, DestinationForSync = 7};

  const size_t NUMDIRS = 8;
  const size_t COUNT_DIR_MUSTEXIST = 6;
  extern const QString dirlabels[];
  extern const QString globals_filename;

class jfGlobalSettings : public QObject {
    Q_OBJECT
  public:
    // data i've decided to makepublic
    size_t fimlimits[5];
    bool dologging_next;
    size_t ficsize_hint;
    // methods
    jfGlobalSettings();
    void MakeSettingsDefault();
    void MakeDirectoriesDefault();
    QString GetDirectory(GDIR_TYPE intype) const;
    bool SetDirectory(GDIR_TYPE intype, const QString& changeto);
    bool LoadAtStart();
    bool WriteAtFinish();
    jfTextStatusDialog* MakeLoadDisplay();
    ~jfGlobalSettings();
  signals:
    void sendAction(QString action_label);
    void sendDone();
    void sendLoading(QString loaded_thing);
    void sendString(QString statement_string);
  protected:
    // helper methods
    void RegisterMetaTypes();
    bool OpenSettingsFile();
    bool ParseSettingsFile();
    bool CloseSettingsFile();
    bool SaveSettings();
    size_t TestGlobalDirectories();
    bool LogDirectory(GDIR_TYPE whichdir);

    // internal data
    bool dologging;
    jfFileReader* input_file;
    QStringList* globaldirs;
    bool ffn_catobtained;

};

extern jfGlobalSettings settings;

} // end of namespace jglobal
//===============================================
// changes the working directory to the executable directory
bool ChangeWDir(QString subdir = "");
//****************************************************************
