/******************************************************************************
Name    :   ao3_consts.h
Author  :   John Q Metro
Purpose :   Declares some constants and utility functions for AO3
Created :   September 8, 2014
Conversion to Qt started October 15, 2014
Updated :   June 20, 2017 (added English-only option)
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

  extern const size_t rat_count;
  extern const QString rat_labels[];
  extern const QString rat_ids[];
  extern QStringList* rat_list;

  extern const size_t soc_count;
  extern const QString soc_labels[];
  extern const QString soc_ids[];
  extern QStringList* soc_list;

  extern const size_t warn_count;
  extern const QString warn_labels[];
  extern const QString warn_ids[];
  extern QStringList* warn_list;

  extern const size_t order_count;
  extern const QString order_labels[];
  extern const QString order_ids[];
  extern QStringList* order_list;


  extern const QString url_start;

  QString MakeUrlPart(size_t rindex, size_t sindex, size_t windex, QString intag, bool conly, bool ieng_only);

  QString AO3Encode(const QString inval);

  void SetupLists();

}
//*****************************************************************************
