/******************************************************************************
Name    :   fim_constants.h
Author  :   John Q Metro
Purpose :   I've decided to isolate the various FIM String and things here
Created :   May 2, 2013 (Qt)
Updated :   January 7, 2018 (fim group order constants)
******************************************************************************/
#ifndef FIM_CONSTANTS_H
    #define FIM_CONSTANTS_H
#endif // FIM_CONSTANTS_H
//-----------------------------
#ifndef FIM_TAGS_H
    #include "fim_tags.h"
#endif // FIM_TAGS_H
//----------------------------
#include <QString>
/*****************************************************************************/
namespace fimcon {

  // rating
  extern const QString ratchars;
  extern const QString ratlist[];
  QString RatLookup(QChar inrat);
  QString RatSearchLookup(QChar inrat);

  // ordering constants
  extern const size_t ordercount;
  extern const QString ordername[];
  extern const QString orderlabel[];
  extern const size_t group_ordercount;
  extern const QString group_ordername[];
  extern const QString group_orderlabel[];

  // Fimfiction.net tags
  extern jfFIMTag_Collection* tags;
  extern QStringSet* important_characters;
  bool ReplaceFIMTags(jfFIMTag_Collection* newtags);
  bool LoadTagsFromFile(QString filepath);
  bool WriteTagsToFile(QString filepath);
  extern const QString fimtag_path;

}
/*****************************************************************************/
