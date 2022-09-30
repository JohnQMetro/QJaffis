/*****************************************************************************
Name:        vectors.h
Author :     John Q Metro
Purpose :    vector declarations
Created:     April 29, 2009
Conversion to QT started : April 18, 2013
Updated:     June 4 (result unit vector change due to downloader changes)
******************************************************************************/
#ifndef VECTORS_H_INCLUDED
#define VECTORS_H_INCLUDED
#endif // VECTORS_H_INCLUDED

//-------------------------------------------------------------------------------
#include <vector>
#include <QString>
#include <QObject>
#include <QMetaType>
#include <set>
#include <map>
/******************************************************************************/
// forward declarations
class jfResultUnit;
class jfDownloadRootItems;

// first off, a vector of result units
class jfResultUnitVector : public std::vector<jfResultUnit*> {
  public:
    // since this class is passed back as a resuly from item parsing, we
    // use it to pass extra data as well.
    bool limitreached; // ffn updating, reached point where old results start if true
    size_t stotal;  // total number of item item sparsed frompage, before filtering
    size_t firstid; //ffn, id of first item parsed


    // constructor
    jfResultUnitVector();
    void setWake(jfDownloadRootItems* wake_target);
    bool WakeAfter();
  protected:
    // used by display to tell downloader it is done
    jfDownloadRootItems* source_new;


};

Q_DECLARE_METATYPE(jfResultUnitVector);
//==========================================================================
// next, a vector for indexes
class jfIndexVector : public std::vector<size_t> {
  public:
    QString ToDelimString(QChar dval) const;
    bool SetFromString(QString inval, QChar dval);
};
//===========================================================================
class jfBasePD;
// a vector of jfBasePD items
class jfPDVector : public std::vector<jfBasePD*> {};
//===========================================================================
class jfBaseObj;

// class for storing fic ids, used for eliminating duplicates
class jfIDStore : public std::set<size_t> {
  public:
    /* DiscardTest returns true if the input is in the set, otherwise it
    adds the input to the set and returns false */
    bool DiscardTest(size_t testval);
    bool DiscardTest(jfBaseObj* testobj);
};
//======================================================================
typedef struct xjfBSPair {
  QString name;
  bool dis;
} jfStrBoolPair;
//------------------------------------------------------------
class jfStrBoolList : public std::vector<jfStrBoolPair> {
  public:
    void AddItem(const QString& inname, bool indis);
};
//======================================================================
// ID key, String data, map. Used for storing compact summaries for fimfiction
class jfIDStringStore : public std::map<size_t,QString> {
  public:
    jfIDStringStore();
    bool Contains(const size_t &inid) const;
    QString GetString(const size_t& idkey) const;
    size_t est_itemcount;
};

/******************************************************************************/
