/******************************************************************************
Name    :   fim_tags.cpp
Author  :   John Q Metro
Purpose :   Generalized tag storage for FIMFiction.Net
Created :   Dec 25, 2017
Updated :   Dec 30, 2017
******************************************************************************/
#ifndef FIM_TAGS_H
    #include "fim_tags.h"
#endif // FIM_TAGS_H
#ifndef UTILS1_H_INCLUDED
  #include "../../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef STRINGPARSER_H_INCLUDED
  #include "../../../core/utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------------
#include <QStringList>
#include <assert.h>
/*****************************************************************************/
// the FIM Tag Type enum : series, character, rating, genre, warning

QString FIM_TagType_to_String(FIM_TagType intag) {
    switch(intag) {
        case FIMT_series    : return "series";
        case FIMT_character : return "character";
        case FIMT_rating    : return "rating";
        case FIMT_genre     : return "genre";
        case FIMT_warning   : return "warning";
        case FIMT_content   : return "content";
    }
    return "";
}

bool String_to_FIM_TagType(QString input, FIM_TagType &output) {
    if (input.isEmpty()) return false;
    if (input == "series") {
        output = FIMT_series;
        return true;
    }
    if (input == "character") {
        output = FIMT_character;
        return true;
    }
    if (input == "rating") {
        output = FIMT_rating;
        return true;
    }
    if (input == "genre") {
        output = FIMT_genre;
        return true;
    }
    if (input == "warning") {
        output = FIMT_warning;
        return true;
    }
    if (input == "content") {
        output = FIMT_content;
        return true;
    }
    return false;
}

//==================================================================
// +++ [ Methods for jfFIMTag ] ++++++++++++++++++++++++++
QStringList jfFIMTag::splitRawSource(const QString& rawsource) const {
    QStringList splitRaw = rawsource.split("&quot;");
    if (splitRaw.count() != 21) assert(false);
    // we discard all the separators
    QStringList result,temp;
    for(size_t getdex = 1; getdex < 21; getdex += 2) {
        result.append(splitRaw.at(getdex).trimmed());
    }
    return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
jfFIMTag::jfFIMTag() {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++ [ Read-only 'properties' ] ++++++++++++++++++++++++
int jfFIMTag::getID() const           { return id;   }
QString jfFIMTag::getName() const     { return name; }
FIM_TagType jfFIMTag::getType() const { return type; }
QString jfFIMTag::getSlug() const     { return slug; }
int jfFIMTag::getStoryCount() const   { return story_count; }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++ [ String I/O ] ++++++++++++++++++++++++++++++++++++
// converts a snipped downloaded from fimfiction.net to internal tag data
// a typical snippet is : &quot;id&quot;:&quot;5&quot;,&quot;name&quot;:&quot;Fluttershy&quot;,&quot;type&quot;:&quot;character&quot;,&quot;slug&quot;:&quot;fluttershy&quot;,&quot;num_stories&quot;:&quot;8127&quot;
bool jfFIMTag::SetFromSource(QString inHTML) {
    const QString fname = "jfFIMTag::SetFromSource";
    // splitting the raw source
    QStringList splitSource = splitRawSource(inHTML);
    // id
    if (splitSource[0]!="id") return false;
    ulong tempid;
    if (!Str2ULong(splitSource[1],tempid)) return false;
    id = (int)tempid;
    // name
    if (splitSource[2]!="name") return false;
    if (splitSource[3].isEmpty()) return false;
    name = splitSource[3];
    name.replace("&#039;","'");
    name.replace("\\u00e9","\u00e9");
    name.replace("\\/","/");
    // type...
    if (splitSource[4]!="type") return false;
    if (!String_to_FIM_TagType(splitSource[5],type)) return false;
    // slug
    if (splitSource[6]!="slug") return false;
    if (splitSource[7].isEmpty()) return false;
    slug = splitSource[7];
    // the story count (most useful for seeing which series is the biggest)
    if (splitSource[8]!="num_stories") return false;
    ulong tempsc;
    if (!Str2ULongC(splitSource[9],tempsc)) return false;
    story_count = (int)tempsc;
    return true;
}
//------------------------------------------------
// to semicolon delimited list
QString jfFIMTag::ToString() const {
    jfOutString result;
    result << id << name << FIM_TagType_to_String(type) << slug << story_count;
    return QString(result);
}
//------------------------------------------------
// from semicolon delimited list
bool jfFIMTag::fromString(QString inSource) {
    jfLineParse split(inSource);
    if (split.NNotX(5)) return false;
    if (!split.IIntVal(0,id)) return false;
    // the name
    QString temp_out = split.UnEscStr(1);
    if (temp_out.isEmpty()) return false;
    name = temp_out;
    // type
    temp_out = split.UnEscStr(2);
    if (!String_to_FIM_TagType(temp_out,type)) return false;
    // slug
    temp_out = split.UnEscStr(3);
    if (temp_out.isEmpty()) return false;
    slug = temp_out;
    // story count
    if (!split.IIntVal(4,story_count)) return false;
    return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++ [ File I/O ] ++++++++++++++++++++++++++++++++++++++
bool jfFIMTag::AddToFile(QTextStream* outfile) const {
    if (outfile == NULL) return false;
    QString outline = ToString();
    (*outfile) << outline << "\n";
    return true;
}
//----------------------------------------
bool jfFIMTag::SetFromFile(jfFileReader* infile) {
    const QString funcname = "jfFIMTag::SetFromFile";
    // starting checks
    assert(infile!=NULL);
    // Reading and processing the line
    if (!infile->ReadParseLine(5,funcname)) return false;
    if (!(infile->lp).IIntVal(0,id)) return false;  // id
    // name
    QString tempstr = (infile->lp).UnEscStr(1);
    if (tempstr.isEmpty()) return false;
    name = tempstr;
    // type
    tempstr = (infile->lp).UnEscStr(2);
    if (!String_to_FIM_TagType(tempstr,type)) {
        return infile->BuildError("Invalid Tag Type (" + tempstr + ")");
    }
    // slug
    tempstr = (infile->lp).UnEscStr(3);
    if (tempstr.isEmpty()) {
        return infile->BuildError("Slug is empty!");
    }
    slug = tempstr;
    // story count
    if (!(infile->lp).IIntVal(4,story_count)) return false;  // story count
    // done
    return true;
}
//==========================================================================
// helper methods
void jfFIMTag_Collection::clearContents() {
    char_tags.clear();
    series_tags.clear();
    genre_tags.clear();
    rating_tags.clear();
    warning_tags.clear();
    content_tags.clear();
    for (size_t tagdex = 0; tagdex < tagstore.size(); tagdex++) {
        delete tagstore[tagdex];
        tagstore[tagdex] = NULL;
    }
    tagstore.clear();
    error_tags.clear();
}
//--------------------
void jfFIMTag_Collection::addTag(jfFIMTag* new_tag) {
    assert(new_tag != NULL);
    switch(new_tag->getType()) {
        case FIMT_series   : series_tags[new_tag->getName()] = new_tag;  break;
        case FIMT_character: char_tags[new_tag->getName()] = new_tag;    break;
        case FIMT_genre    : genre_tags[new_tag->getName()] = new_tag;   break;
        case FIMT_rating   : rating_tags[new_tag->getName()] = new_tag;  break;
        case FIMT_warning  : warning_tags[new_tag->getName()] = new_tag; break;
        case FIMT_content   :content_tags[new_tag->getName()] = new_tag; break;
        default: assert(false);
    }
}
//--------------------
bool jfFIMTag_Collection::parseAndAddTag(const QString& rawSource) {
    jfFIMTag *new_tag = new jfFIMTag();
    if (new_tag->SetFromSource(rawSource)) {
        tagstore.push_back(new_tag);
        addTag(new_tag);
        return true;
    }
    else {
        delete new_tag;
        error_tags.append(rawSource);
        return false;
    }
}
//--------------------
bool jfFIMTag_Collection::loadAndAddTag(const QString& rawSource) {
    jfFIMTag *new_tag = new jfFIMTag();
    if (new_tag->fromString(rawSource)) {
        tagstore.push_back(new_tag);
        addTag(new_tag);
        return true;
    }
    else {
        delete new_tag;
        error_tags.append(rawSource);
        return false;
    }
}
//--------------------
// inputs the array literal found in Fimfiction.Net source, to parse and get all the tags
bool jfFIMTag_Collection::parseTagBlock(QString rawSource) {
    const QString fname = "jfFIMTag_Collection::parseTagBlock";
    int spos,epos;
    QString buffer;
    int npos = 0;
    size_t counter = 0;
    while(true) {
        spos = rawSource.indexOf('{',npos);
        if (spos < 0) break;
        epos = rawSource.indexOf('}',spos+1);
        if (epos < 0) {
            return false;
        }
        buffer = rawSource.mid(spos+1,epos-spos-1).trimmed();
        if (!parseAndAddTag(buffer)) {
            return false;
        }
        npos = epos+1;
        counter++;
    }
    return true;
}
//------------------------
QString jfFIMTag_Collection::findSlug(const jfFIMTagMap& tagmap, const QString& lookfor) const {
    jfFIMTagMap::const_iterator xloc = tagmap.find(lookfor);
    if (xloc == tagmap.end()) return "";
    else return (xloc->second)->getSlug();
}
//------------------------
QStringList* jfFIMTag_Collection::namesFromMap(const jfFIMTagMap& tagmap) const {
    QStringList* result = new QStringList();
    jfFIMTagMap::const_iterator mapptr = tagmap.begin();
    while(mapptr != tagmap.end()) {
        result->append(mapptr->first);
        mapptr++;
    }
    return result;
}
//------------------------
QStringList* jfFIMTag_Collection::namesAndSlugsFromMap(const jfFIMTagMap& tagmap) const {
    QStringList* result = new QStringList();
    jfFIMTagMap::const_iterator mapptr = tagmap.begin();
    while(mapptr != tagmap.end()) {
        result->append(mapptr->first);
        result->append((mapptr->second)->getSlug());
        mapptr++;
    }
    return result;
}
//------------------------
QStringSet* jfFIMTag_Collection::importantNamesFromMap(const jfFIMTagMap& tagmap,size_t mincount ) const {
    QStringSet* result = new QStringSet();
    jfFIMTagMap::const_iterator mapptr = tagmap.begin();
    const jfFIMTag* currtag;
    while(mapptr != tagmap.end()) {
        currtag = mapptr->second;
        if ((currtag->getStoryCount()) >= mincount) {
            result->insert(currtag->getName());
        }
        mapptr++;
    }
    return result;
}
//------------------------
void jfFIMTag_Collection::writeNames(QTextStream* outfile, const QString& mapname, const jfFIMTagMap& tagmap, bool includeCounts) const {
    const QString fname = "jfFIMTag_Collection::writeNames";
    assert(outfile != NULL);
    (*outfile) << mapname << " : " << tagmap.size() << "\n";
    QStringList* currentList = namesFromMap(tagmap);
    jfFIMTagMap::const_iterator item;
    jfFIMTag* current_tag = NULL;
    QString cname;
    for (size_t listdex = 0; listdex < currentList->count() ; listdex++) {
        cname = currentList->at(listdex);
        (*outfile) << cname;
        if (includeCounts) {
            item = tagmap.find(currentList->at(listdex));
            current_tag = item->second;
            (*outfile) << " : " << current_tag->getStoryCount();
        }
        (*outfile) << "\n";
    }
    (*outfile) << "\n";
    delete currentList;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfFIMTag_Collection::jfFIMTag_Collection() {
    tagstore.reserve(200);
}
//------------------------------------------------------
// destructor
jfFIMTag_Collection::~jfFIMTag_Collection() {
    clearContents();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// informational methods
//-----------------------------
size_t jfFIMTag_Collection::totalCount() const {
    return tagstore.size();
}
//-----------------------------
size_t jfFIMTag_Collection::countFor(FIM_TagType tagType) const {
    switch(tagType) {
        case FIMT_character: return char_tags.size();
        case FIMT_genre    : return genre_tags.size();
        case FIMT_series   : return series_tags.size();
        case FIMT_rating   : return rating_tags.size();
        case FIMT_warning  : return warning_tags.size();
        case FIMT_content  : return content_tags.size();
    }
    return 0;
}
//-----------------------------
QString jfFIMTag_Collection::slugForName(const QString& lookupName, FIM_TagType tagType) const {
    switch(tagType) {
        case FIMT_character: return findSlug(char_tags,lookupName);
        case FIMT_genre    : return findSlug(genre_tags,lookupName);
        case FIMT_series   : return findSlug(series_tags,lookupName);
        case FIMT_rating   : return findSlug(rating_tags,lookupName);
        case FIMT_warning  : return findSlug(warning_tags,lookupName);
        case FIMT_content  : return findSlug(content_tags,lookupName);
    }
    return "";
}
//------------------------------
QStringList* jfFIMTag_Collection::namesForType(FIM_TagType tagType) const {
    switch(tagType) {
        case FIMT_character: return namesFromMap(char_tags);
        case FIMT_genre    : return namesFromMap(genre_tags);
        case FIMT_series   : return namesFromMap(series_tags);
        case FIMT_rating   : return namesFromMap(rating_tags);
        case FIMT_warning  : return namesFromMap(warning_tags);
        case FIMT_content  : return namesFromMap(content_tags);
    }
    return NULL;
}
//------------------------------
QStringList* jfFIMTag_Collection::namesAndSlugsForType(FIM_TagType tagType) const {
    switch(tagType) {
        case FIMT_character: return namesAndSlugsFromMap(char_tags);
        case FIMT_genre    : return namesAndSlugsFromMap(genre_tags);
        case FIMT_series   : return namesAndSlugsFromMap(series_tags);
        case FIMT_rating   : return namesAndSlugsFromMap(rating_tags);
        case FIMT_warning  : return namesAndSlugsFromMap(warning_tags);
        case FIMT_content  : return namesAndSlugsFromMap(content_tags);
    }
    return NULL;
}
//------------------------------
QStringSet* jfFIMTag_Collection::importantCharacters(size_t min_amount) const {
    return importantNamesFromMap(char_tags,min_amount);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special
void jfFIMTag_Collection::SetTagsToEmpty(jfTagListing& listing, FIM_TagType source) const {
    // picking the correct set of tags
    const jfFIMTagMap* curr_map;
    if (source == FIMT_character) curr_map = &char_tags;
    else if (source == FIMT_genre) curr_map = &genre_tags;
    else if (source == FIMT_series) curr_map = &series_tags;
    else if (source == FIMT_rating) curr_map = &rating_tags;
    else if (source == FIMT_warning) curr_map = &warning_tags;
    else curr_map = &content_tags;
    // clearing the old and preparing for the new...
    listing.clear();
    jfFIMTagMap::const_iterator cptr = curr_map->begin();
    QString cname;
    // looping to get and add all the tag names
    while (cptr != (curr_map->end())) {
        cname = cptr->first;
        listing[cname] =  jfts_NONE;
        cptr++;
    }

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parsing from source
bool jfFIMTag_Collection::SetFromSource(const QString& rawhtml){
    jfStringParser xparser(rawhtml);
    if (!xparser.MovePast("<div class=\"label search-text\"")) return false;
    QString buffer;
    if (!xparser.GetDelimited("data-tags=\"[","]\">",buffer)) return false;
    // finally
    return parseTagBlock(buffer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// input / output
//---------------------------
// for debugging purposes
bool jfFIMTag_Collection::ReportToFile(QString filepath) const {
    const QString fname = "jfFIMTag_Collection::ReportToFile";
    // opening the file
    QTextStream* ofile = GetOpenFile(filepath,true);
    if (ofile == NULL) return false;
    // start with the tag count
    (*ofile) << "Number of Tags: " << tagstore.size() << "\n\n";
    // names divided by tag type
    writeNames(ofile,"RATINGS",rating_tags);
    writeNames(ofile,"WARNINGS",warning_tags);
    writeNames(ofile,"GENRES",genre_tags);
    writeNames(ofile,"SERIES",series_tags,true);
    writeNames(ofile,"CHARACTERS",char_tags,true);
    writeNames(ofile,"CONTENT",content_tags);
    // next, errors
    (*ofile) << "\n" << "-----------------------" << "ERRORS : ";
    (*ofile) << error_tags.count() << "\n";
    for (size_t errdex = 0; errdex < error_tags.count(); errdex++) {
        (*ofile) << error_tags.at(errdex) << "\n";
    }
    (*ofile) << "\n";
    // string file output for the correctly parsed data
    (*ofile) << "DATA : " << tagstore.size() << "\n";
    for (size_t tagdex = 0; tagdex < tagstore.size(); tagdex++) {
        (*ofile) << (tagstore[tagdex]->ToString()) << "\n";
    }
    // done
    CloseFileStream(ofile,true);
    return true;
}
//---------------------------
bool jfFIMTag_Collection::AppendToFileStream(QTextStream* ofile) const {
    if (ofile == NULL) return false;
    // start with the tag count
    (*ofile) << tagstore.size() << "\n";
    // string file output for the correctly parsed data
    for (size_t tagdex = 0; tagdex < tagstore.size(); tagdex++) {
        (*ofile) << (tagstore[tagdex]->ToString()) << "\n";
    }
    // done
    return true;
}
//---------------------------
bool jfFIMTag_Collection::ReadFromFileReader(jfFileReader* infile) {
    const QString fname = "jfFIMTag_Collection::ReadFromFileReader";
    // checks and preparation
    if (infile == NULL) return false;
    if (infile->AtEnd()) return false;
    /**/JDEBUGLOG(fname,1)
    clearContents();
    // we start properly, by reading the tag count
    ulong tagcount;
    infile->ReadULong(tagcount,fname);
    /**/JDEBUGLOGU(fname,2,tagcount)
    tagstore.reserve(tagcount);
    // we loop tagcount times. first, some variables
    QString readline;
    jfFIMTag* newtag = NULL;
    /**/JDEBUGLOG(fname,3)
    // the loop
    for (size_t tagindex = 0; tagindex < tagcount; tagindex++) {
        newtag = new jfFIMTag();
        if (!(newtag->SetFromFile(infile))) {
            /**/JDEBUGLOG(fname,4)
            delete newtag;
            return false;
        }
        tagstore.push_back(newtag);
        addTag(newtag);
    }
    /**/JDEBUGLOG(fname,5)
    return true;
}

/*****************************************************************************/
// <div class="label search-text"
// data-tags="[ to ]">
