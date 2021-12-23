/******************************************************************************
Name    :   ao3_consts.h
Author  :   John Q Metro
Purpose :   Declares some constants and utility functions for AO3
Created :   September 8, 2014
Conversion to Qt started October 15, 2014
Updated :   October 31, 2021 (url making stuff)
******************************************************************************/
#ifndef AO3_CONSTS_H_INCLUDED
  #define AO3_CONSTS_H_INCLUDED
#endif // AO3_CONSTS_H_INCLUDED
//-----------------------------------
#include <QString>
#include <QStringList>
//*****************************************************************************

/* Ratings     : G (General), T (Teen), M (Mature), E (Explicit), _ (Unspecified)
   Orientation : F (Femslash), G (General), H (Heterosexual), M (Multi),
                O (Other), S (Slash), _ (Unspecified)
   Warnings    : W (Warnings, see warntags), _ (No warnings apply), N (warnings
                 deliberatly avoided), E (External Work)
   warntags    : V (violence), D (major character death), R (rape), P (underage sex)
*/

namespace ao3con {
  extern const QString rating_ac;
  extern const QString orient_ac;
  extern const QString warn1_ac;
  extern const QString warn2_ac;
  extern const QChar unchar;

}
//*****************************************************************************
