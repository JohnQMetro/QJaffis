/******************************************************************************
Name    :   fim_tags.h
Author  :   John Q Metro
Purpose :   Generalized tag storage for FIMFiction.Net
Created :   Dec 20, 2017
Updated :   Dec 30, 2017
******************************************************************************/
#ifndef FIM_TAGS_H
    #define FIM_TAGS_H
#endif // FIM_TAGS_H
//------------------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "../../../core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef JFMISCTYPES1
    #include "../../../core/objs/misc_types1.h"
#endif
//-------------------------------------------
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <vector>
#include <map>
#include <set>
/*****************************************************************************/
// the FIM Tag Type enum : series, character, rating, genre, warning
enum FIM_TagType { FIMT_series, FIMT_character, FIMT_rating, FIMT_genre, FIMT_warning, FIMT_content };
QString FIM_TagType_to_String(FIM_TagType intag);
bool String_to_FIM_TagType(QString input, FIM_TagType &output);

//==================================================================
// stores parsed tag data
class jfFIMTag {
  protected:
    // the stored data
    int id;
    QString name;
    FIM_TagType type;
    QString slug;
    int story_count;
    // helper methods
    QStringList splitRawSource(const QString& rawsource) const;
  public:
    // constructors
    jfFIMTag();
    // read-only 'properties'
    int getID() const;
    QString getName() const;
    FIM_TagType getType() const;
    QString getSlug() const;
    int getStoryCount() const;
    // string i/o
    bool SetFromSource(QString inHTML);
    QString ToString() const;
    bool fromString(QString inSource);

    // File I/O
    bool AddToFile(QTextStream* outfile) const;
    bool SetFromFile(jfFileReader* infile);

};
//==================================================================
// a <QString,jfFIMTag*> type (using a lexical compare)
typedef std::map<QString,jfFIMTag*,ltstr> jfFIMTagMap;
//===================================================================
// stores multiple tags in a big structure...
class jfFIMTag_Collection {
  protected:
    std::vector<jfFIMTag*> tagstore;
    // primary maps storing tag pointers for quick lookup
    jfFIMTagMap char_tags;
    jfFIMTagMap series_tags;
    jfFIMTagMap genre_tags;
    jfFIMTagMap rating_tags;
    jfFIMTagMap warning_tags;
    jfFIMTagMap content_tags;
    // additional data
    QStringList error_tags;
    // helper methods
    void clearContents();
    void addTag(jfFIMTag* new_tag);
    bool parseAndAddTag(const QString& rawSource);
    bool loadAndAddTag(const QString& rawSource);
    bool parseTagBlock(QString rawSource);
    QString findSlug(const jfFIMTagMap& tagmap, const QString& lookfor) const;
    QStringList* namesFromMap(const jfFIMTagMap& tagmap) const;
    QStringList* namesAndSlugsFromMap(const jfFIMTagMap& tagmap) const;
    QStringSet* importantNamesFromMap(const jfFIMTagMap& tagmap,size_t mincount ) const;
    void writeNames(QTextStream* outfile, const QString& mapname, const jfFIMTagMap& tagmap, bool includeCounts = false) const;
  public:
    jfFIMTag_Collection();
    ~jfFIMTag_Collection();
    // informational methods
    size_t totalCount() const;
    size_t countFor(FIM_TagType tagType) const;
    QString slugForName(const QString& lookupName, FIM_TagType tagType) const;
    QStringList* namesForType(FIM_TagType tagType) const;
    QStringList* namesAndSlugsForType(FIM_TagType tagType) const;
    QStringSet* importantCharacters(size_t min_amount) const;
    // special
    void SetTagsToEmpty(jfTagListing& listing, FIM_TagType source) const;
    // parsing from source
    bool SetFromSource(const QString& rawhtml);
    // input / output
    bool ReportToFile(QString filepath) const;
    bool AppendToFileStream(QTextStream* ofile) const;
    bool ReadFromFileReader(jfFileReader* infile);

};



/*****************************************************************************/
