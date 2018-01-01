/******************************************************************************
// Name:        htmlskelton.h
// Author :     John Q Metro
// Purpose :    For filling in html i/o skeltons with output info
// Created:     November 15, 2009
// Conversion to Qt started March 10, 2013
// Updated:     November 15, 2009
******************************************************************************/
#ifndef HTMLSKELTON_H_INCLUDED
#define HTMLSKELTON_H_INCLUDED
#endif // HTMLSKELTON_H_INCLUDED

#include <QString>
#include <QStringList>
#include <QVector>

//****************************************************************************/
/*
-The html skeltons are a way to control the html formatting of the html output.
-The raw data has name tags that are replaced with the specified data.
-There are also conditional sections which are not included in the result if
 the data associated with them is empty, false, or zero.
-Includion of conditional sections is decided before the name tags are replaced.

format for conditional section:
<<<&NAME_TAG&>>> html data (perhaps including a name tag) <<&>>

format for name tags
<<#NAME_TAG#>>

-The class jfSkeletonParser is preloaded with NAME_TAGS and thier associated
 data before calling the replacement routines, first for conditionals, then
 for general replacements.
*/
//****************************************************************************/
enum jfsp_DATATYPE {jfsk_TEXT, jfsk_BOOL, jfsk_UINT};
//----------------------------------------------
class jfSkeletonParser {
  public:
    // the constructor
    jfSkeletonParser();
    // data and parsing
    bool SetSkeleton(QString inskel);
    bool ProduceIntermediate();
    bool MakeResult();
    QString GetResult() const;
    // adding items
    bool AddText(QString nametag, const QString& value);
    bool AddBool(QString nametag, const bool& value);
    bool AddUInt(QString nametag, const size_t& value);
    // the destructor
    ~jfSkeletonParser();
  protected:
    // helper methods
    bool FindTag(QString nametag, jfsp_DATATYPE& ftype, size_t& fpos) const;
    bool NotEmpty(jfsp_DATATYPE ftype, size_t fpos) const;
    // text items
    QStringList* nametags_text;
    QStringList* values_text;
    size_t text_count;
    // boolean items
    QStringList* nametags_bool;
    QVector<bool>*  values_bool;
    size_t bool_count;
    // integer items
    QStringList* nametags_uint;
    QVector<uint>* values_uint;
    size_t uint_count;
    // other
    size_t total_count;
    // parsing stuff
    QString src_skelton;
    QString int_result;
    QString dne_result;
    bool done;
};
//****************************************************************************/
