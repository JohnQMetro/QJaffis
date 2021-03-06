/******************************************************************************
Name    :   ao3_ficobj.h
Author  :   John Q Metro
Purpose :   Declares fanfic object data of archoveofourown.org
Created :   August 26, 2012
Conversion to Qt Started September 28, 2013
Updated :  July 1, 2016
******************************************************************************/
#ifndef AO3_FICOBJ_H_INCLUDED
#define AO3_FICOBJ_H_INCLUDED
#endif // AO3_FICOBJ_H_INCLUDED
//---------------------------------------
#ifndef AO3_CATS_H_INCLUDED
  #include "ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
#ifndef STRINGPARSER_H_INCLUDED
  #include "../../../core/utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
#ifndef FICBASE_H
  #include "../ficbase.h"
#endif // FICBASE_H


#include <vector>
//**************************************************************************
class jfAO3Fanfic : public jfGenericFanfic2 {
  public:
    // default constructors
    jfAO3Fanfic();
    jfAO3Fanfic(const jfAO3Fanfic& src);
    // other methods
    bool SetFromString(QString inval, const jfAO3_Category* incat, QString& parse_err);
    // special meta-information
    virtual QString GetTypeID() const;
    // getting info
    QChar GetRating() const;
    QChar GetOrientation() const;
    QChar GetWarnflag() const;
    QString GetWarntag() const;
    bool TestWarntag(QChar inval) const;
    int GetEstPCount() const;
    size_t GetKudos() const;
    QString GetRelationships() const;
    QString GetCharacters() const;
    QString GetExtraTags() const;
    bool InSeries() const;
    size_t SeriesIndex() const;
    QString SeriesName() const;
    QString SeriesUrl() const;
    QString GetFandoms() const;
    bool MultipleFandoms() const;
    QString GetFandom(size_t inindex) const;
    size_t FandomCount() const;
    // special url construction
    // virtual QString GetURLforPart(size_t partindex) const;
    virtual QString MakeAuthorUrl() const;
    // to string functions
    virtual QString ToText() const;
    virtual QString ToDisplayHTML() const;
    // added stuff
    virtual bool LoadValues(jfSkeletonParser* inparser) const;
    virtual void ProcessDescription();
  protected:
    // internal to string helper methods
    QString RatingToString() const;
    QString OrientToString() const;
    QString WarnToString() const;
    // internal parsing methods
    bool ParseStart();
    bool ParseMiddle();
    bool ParseTags();
    bool ParseEnd();
    bool parseError(const QString in_msg);
    // special methods
    virtual jfFicExtract* MakeExtract() const;
    virtual void LoadIntoExtract(jfFicExtract* into) const;
    // file i/o output
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    virtual bool AddExtraStuff(QTextStream* outfile) const;
    virtual bool ReadExtraStuff(jfFileReader* infile);
    // more data
    QChar rating, orient, warn;
    QString warntags;
    int eccount;
    size_t kudcount;
    QString relationships, characters, extratags;
    // categories
    std::vector<const jfAO3_Category*> cats;
    // series
    size_t series_index;
    QString series_name, series_url;

    // parsing
    jfStringParser* x_parser;
    QString parser_error;
};
//**************************************************************************
