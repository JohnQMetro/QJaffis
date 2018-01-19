/******************************************************************************
// Name:        basecollect.h
// Author :     John Q Metro
// Purpose :    Declares core item collection classes
// Created:     March 19, 2009
// Conversion to QT Started April 8, 2013
// Updated:     January 18, 2018 (added the isEmpty method to jfFilterMap)
******************************************************************************/
#ifndef BASECOLLECT_H_INCLUDED
  #define BASECOLLECT_H_INCLUDED
#endif // BASECOLLECT_H_INCLUDED
#ifndef BASEOBJ_H_INCLUDED
    #include "../objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef JFBASEFILTER
  #include "../filters/base/filterbase.h"
#endif
//----------------------------------
#include <map>
#include <QString>
#include <QStringList>
//*****************************************************************************
// defines some common things for an ancestor collection class
class jfBaseCollection : public jfBaseObj {
  public:
    // default constructors
    jfBaseCollection();
    jfBaseCollection(size_t src_id, const QString& nname);
    // implementable data methods
    size_t GetCount() const;
    bool isEmpty() const;
    // general index info
    int GetIndex() const;
    bool ItemV() const;
    // stuff for the current index
    QString CurrName() const;
    size_t CurrID() const;
    QString CurDesc() const;
    // index manipulation
    virtual void SaveIndex();
    virtual bool RestoreIndex();
    virtual bool NextIndex() =0;
    virtual bool BackIndex() =0;
    virtual void ResetIndex() =0;
    virtual bool ToIndex(const int& newindex) = 0;
    // some more abstract methods
    virtual QStringList* GetNameList() const = 0;
    virtual QStringList* GetDescriptionList() const = 0;
    virtual QStringList* GetTypeList() const = 0;
    virtual bool DeleteAtIndex(const size_t& dindex) = 0;
  protected:
    // a private Read helper method
    size_t CalcNewIndex(size_t oindex) const;
    // file I/O
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddMid2ToFile(QTextStream* outfile) const = 0;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadMid2FromFile(jfFileReader* infile) = 0;
    // common data
    size_t item_count;
    // indexes
    int item_index;
    jfBaseObj* curr_item;
    int backup_index;
    jfBaseObj* backup_item;
};
//*****************************************************************************
// how the map template will calculate it's comaparisons
struct ltstr {
  bool operator()(const QString& s1, const QString& s2) const {
    int ri = s1.compare(s2);
    return (ri<0);
  }
};
//*****************************************************************************
class stl_FilterMap : public std::map<QString,jfBaseFilter*,ltstr> {
};
//*****************************************************************************
// the filter map uses the filter names as keys, store multiple types of filters
class jfFilterMap : public jfBaseCollection {
  public:
    // the default constructor
    jfFilterMap();
    bool SetMapPtr(jfFilterMap* nval);
    // simple filter map specific methods
    bool NameExists(const QString& lookfor) const;
    stl_FilterMap::iterator RenameElem(bool& ok, const QString& oldname, const QString& newname);
    jfBaseFilter* GetItem(const QString& itemname);
    bool MatchAll(const jfBasePD* inval) const;
    bool isEmpty() const;
    // some additonal methods
    bool AddFilter(jfBaseFilter* infilter, size_t& outindex);
    bool GetIndexByName(const QString& findname, size_t& outindex) const;
    jfBaseFilter* GetItem(const size_t& findex);
    bool DeleteByName(const QString& findname);
    bool ReplaceByName(QString nameold, jfBaseFilter* newfilter, size_t& outindex);
    bool ReplaceSame(jfBaseFilter* newfilter,size_t& outindex);
    size_t CopyLoad();
    bool VerifyNames(QString& omsg, bool notinmap);
    size_t PairFilterLogging(bool write);
    // overriding the older classes works better
    virtual void SaveIndex();
    virtual bool RestoreIndex();
    // implemented abstract methods
    virtual bool NextIndex();
    virtual bool BackIndex();
    virtual void ResetIndex();
    virtual bool ToIndex(const int& newindex);
    virtual QString GetTypeID() const;
    // some more abstract methods
    virtual QStringList* GetNameList() const;
    virtual QStringList* GetDescriptionList() const;
    virtual QStringList* GetTypeList() const;
    virtual bool DeleteAtIndex(const size_t& dindex);
    // extra methods
    void DeleteAll();
  protected:
    // implemented virtual methods
    virtual bool AddMid2ToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMid2FromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    stl_FilterMap coredata;
    // indexes
    stl_FilterMap::iterator curr_iterat;
    stl_FilterMap::iterator backup_iterat;
    // external links
    jfFilterMap* exlocmap;
};
//*****************************************************************************
