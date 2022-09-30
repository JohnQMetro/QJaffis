 /******************************************************************************
Name    :   fim_constants.cpp
Author  :   John Q Metro
Purpose :   I've decided to isolate the various FIM String and things here
Created :   May 2, 2013 (Qt)
Updated :   January 7, 2018 (Fim group ordering)
******************************************************************************/
#ifndef FIM_CONSTANTS_H
  #include "fim_constants.h"
#endif // FIM_CONSTANTS_H
//-----------------------------
#ifndef UTILS2_H_INCLUDED
  #include "../../../core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//-----------------------------
#include <assert.h>
#include <QMutex>
#include <QMutexLocker>
/*****************************************************************************/
// ratings
const QString fimcon::ratchars = "AETM";
const size_t ratcount = 4;
const QString fimcon::ratlist[ratcount] = {"All","Everyone","Teen","Mature"};
//--------------------------------------------
QString fimcon::RatLookup(QChar inrat) {
  int xrat = ratchars.indexOf(inrat);
  if (xrat<0) return "";
  else return ratlist[xrat];
}
//--------------------------------------------
QString fimcon::RatSearchLookup(QChar inrat) {
  if (inrat=='A') return "";
  else if (inrat=='E') return "everyone";
  else if (inrat=='T') return "teen";
  else if (inrat=='M') return "mature";
  else assert(false);
  return "";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// order
const size_t fimcon::ordercount = 7;
const QString fimcon::ordername[fimcon::ordercount] = { "latest", "updated", "top",
     "views", "words", "comments", "heat"};
const QString fimcon::orderlabel[fimcon::ordercount] = { QString("First Posted Date"), QString("Update Date"),
  QString("Rating"), QString("Views"), QString("Word Count"), QString("Comments"),
  QString("Hot")};
//---------------------------------------------
const size_t fimcon::group_ordercount = 3;
const QString fimcon::group_ordername[fimcon::group_ordercount] = {"members","stories","date_created"};
const QString fimcon::group_orderlabel[fimcon::group_ordercount] = {"Members","Stories","Date Created"};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Fimfiction.net tags
QMutex fimlock;
std::vector<QStringSet*> oldx;
//---------------------
jfFIMTag_Collection* fimcon::tags = NULL;
QStringSet* fimcon::important_characters = NULL;
//---------------------
bool fimcon::ReplaceFIMTags(jfFIMTag_Collection* newtags) {
    QMutexLocker mlock(&fimlock);
    if (newtags == NULL) return false;
    jfFIMTag_Collection* oldtags = tags;
    if (important_characters != NULL) oldx.push_back(important_characters);
    tags = newtags;
    important_characters = tags->importantCharacters(1500);
    if (oldtags != NULL) delete oldtags;
    return true;
}
//---------------------
bool fimcon::LoadTagsFromFile(QString filepath) {
    jfFileReader reader(filepath);
    if (!(reader.OpenFile())) return false;
    jfFIMTag_Collection* new_collection = new jfFIMTag_Collection();
    if (!(new_collection->ReadFromFileReader(&reader))) {
        delete new_collection;
        reader.CloseFile();
        return false;
    }
    reader.CloseFile();
    return ReplaceFIMTags(new_collection);
}
//---------------------
bool fimcon::WriteTagsToFile(QString filepath) {
    QMutexLocker mlock(&fimlock);
    if (tags == NULL) return false;
    // opening the file for output
    QTextStream* ofile = GetOpenFile(filepath,true);
    if (ofile == NULL) return false;
    tags->AppendToFileStream(ofile);
    // done
    CloseFileStream(ofile,true);
    return true;
}
//---------------------
const QString fimcon::fimtag_path = "config/fimtag.dat";
/*****************************************************************************/
