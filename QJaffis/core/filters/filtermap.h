/******************************************************************************
Name    :   filtermap.h
Author  :   John Q Metro
Purpose :   A Filter collection using the new Base Filter type
Created :   April 3, 2023
Updated :   August 6, 2023
******************************************************************************/
#pragma once

#ifndef FILTERMAP_H
    #define FILTERMAP_H
#endif // FILTERMAP_H

#include <QString>
#include "base/basefilter.h"

#include "../utils/ltstr.h"
/*****************************************************************************/
class stl_FilterMap : public std::map<QString,jfFilterBase*,ltstr> {
};
// =======================================================================
class jfFilterMap {
  public:
    jfFilterMap();
    jfFilterMap(const QString& map_name_in);
    bool SetMapPointer(const jfFilterMap* nval);
    size_t CopyLoad();
    virtual ~jfFilterMap();
    void DeleteAll();

    // data
    QString GetName() const;
    bool SetName(const QString& newname);
    const QString GetDescription() const;
    void SetDescription(const QString& new_description);
    bool IsEmpty() const;
    size_t FilterCount() const;
    bool VerifyNames(QString& omsg, bool notinmap) const;
    size_t PairFilterLogging(bool write);

    // item operations
    bool NameExists(const QString& fname) const;
    stl_FilterMap::const_iterator RenameElem(bool& ok, const QString& oldname, const QString& newname);
    bool DeleteByName(const QString& findname);
    jfFilterBase* GetItem(const QString& itemname) const;
    bool AddFilter(jfFilterBase* infilter, size_t& outindex);
    bool ReplaceByName(QString nameold, jfFilterBase* newfilter, size_t& outindex);
    bool ReplaceSame(jfFilterBase* newfilter, size_t& outindex);

    bool MatchAll(const jfSearchResultItem* inval) const;
    stl_FilterMap::const_iterator GetStartIterator() const;
    bool IsEnd(const stl_FilterMap::const_iterator& iterator) const;

    // file i/o
    virtual bool AddToFile(QTextStream* outfile) const;
    bool GetFromFile(jfFileReader* infile);
  protected:
    bool LoadFiltersFromFile(jfFileReader* infile, size_t expected_count);
    bool GetFromFileCompat(jfFileReader* infile);
    bool GetIndexByName(const QString& findname, size_t& outindex) const;

    // internal data
    QString map_name;
    QString map_description;

    stl_FilterMap coredata;

    // external links
    const jfFilterMap* exlocmap;
};

/*****************************************************************************/
