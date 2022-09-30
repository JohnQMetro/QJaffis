/***************************************************************
 * Name:      initend.h
 * Purpose:   A global settings class, with loading and unloading
 * Author:    John Q Metro
 * Created:   Febuary 13, 2011
 Conversion to QT Started June 23, 2013
 * Updated:   July 25, 2021
Properly splitting app startup and stop from settings
 **************************************************************/
#ifndef INITEND_H_INCLUDED
  #define INITEND_H_INCLUDED
#endif // INITEND_H_INCLUDED
//------------------------------
#ifndef WIDGETS1_H_INCLUDED
  #include "interface/controls/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
#ifndef GLOBALSETTINGS_H
    #include "globalsettings.h"
#endif // GLOBALSETTINGS_H
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

extern jfGlobalSettings settings;

class AppStartAndStop : public QObject {
    Q_OBJECT
  public:
    AppStartAndStop();
    bool LoadAtStart();
    bool WriteAtFinish();
    jfTextStatusDialog* MakeLoadDisplay();
  signals:
    void sendAction(QString action_label);
    void sendDone();
    void sendLoading(QString loaded_thing);
    void sendString(QString statement_string);
  protected:
    void RegisterMetaTypes();

    bool ffn_catobtained;
};

extern AppStartAndStop startstop;

} // end of namespace jglobal
//===============================================
// changes the working directory to the executable directory
bool ChangeWDir(QString subdir = "");
//****************************************************************
