/******************************************************************************
Name    :   cat_misc.h
Author  :   John Q Metro
Purpose :   Misc category data stuff
Created :   August 19, 2012
Conversion to Qt Started September 28, 2013
Updated :   July 13, 2016 (add NameAtIndex)
******************************************************************************/
#ifndef CAT_MISC_H_INCLUDED
#define CAT_MISC_H_INCLUDED
#endif // CAT_MISC_H_INCLUDED
//-----------------------------------------------------------------
#ifndef CAT_COLL_H_INCLUDED
  #include "cat_coll.h"
#endif // CAT_COLL_H_INCLUDED
//---------------------------------------------
#include <QMutex>
//***************************************************************************
class jfGenSecData {
  public:
    // constructor
    jfGenSecData();
    // indexes
    void ResetIndex();
    bool NextIndex();
    QString NextName();
    // Getting Data
    QString CurrName() const;
    QString CurrUrl() const;
    QString CurrType() const;
    size_t ItemCount() const;
    QString NameAtIndex(size_t index) const;
    // Adding Data
    bool AddStrings(size_t INBLOCK, const QString inname[], const QString inurls[], QString intype);
    // destuctor
    virtual ~jfGenSecData();
  protected:
    // the data
    QStringList* secname;
    QStringList* sectype;
    QStringList* securl;
    // index
    size_t item_count;
    int item_index;
    int index;
    // other
    mutable QMutex acc;
};
//=================================
// wraps i/o and management for cat holders
class jfGenCatManager {
  public:
    QString err;
    // constructor
    jfGenCatManager(QString infilepath, QString inname);
    // basic info
    QString GetName() const;
    QString GetFilename() const;
    bool HasData() const;
    jfGen_CatHolder* GetData();
    // file i/o
    void ToFile() const;
    bool FromFile();
    // handling New
    void NewDone();
    void DiscardNew();
    jfGen_CatHolder* StartNew();
    // registering searchers
    int RegisterSearch(const QWidget* in_search);
    bool RemoveSearch(size_t which);
    // destructor
    virtual ~jfGenCatManager();
  protected:
    jfGen_CatHolder* current;
    jfGen_CatHolder* newdata;
    QMutex locker;
    QString filepath;
    QString name;
    // info on the searches that are currently active and using the holder
    const QWidget* searchers[32];

};
//***************************************************************************
