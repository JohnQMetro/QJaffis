/******************************************************************************
Name    :   gen_cat.h
Author  :   John Q Metro
Purpose :   General category data and info (non fanfiction.net)
Created :   August 8, 2012
// Conversion to Qt Started September 26, 2013
Updated :   September 14, 2016
******************************************************************************/
#ifndef GEN_CAT_H_INCLUDED
#define GEN_CAT_H_INCLUDED
#endif // GEN_CAT_H_INCLUDED
//-------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "../../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
//-------------------------------
#include <assert.h>
#include <QStringList>
#include <map>
//****************************************************************************
class jfGeneralCategory : public jfUrlBase {
  public:
    // constructors
    jfGeneralCategory();
    jfGeneralCategory(const jfGeneralCategory& inval);
    // getting data
    QString GetSection(size_t index = 0) const;
    size_t SectionCount() const;
    bool AddNewSection(const QString& new_section);
    QString GetCombo() const;
    // tests
    bool IsCategory(QString tsname, QString tcatname) const;
    // other
    virtual bool SetFromString(const QString& invalin, const QString& insection) = 0;
  protected:
    // file i/o
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    // internal data
    bool multisection;
    QStringList section_names;
};
//==========================================================================
class jfGenSection : public jfBaseCollection {
  public:
    // constructors
    jfGenSection(bool idlookup_in);
    jfGenSection(QString& inname, bool id_lookupin);
    bool AppendCategory(jfGeneralCategory* category_in);
    // information by index
    int FindByName(const QString& inname) const;
    QString NameAtIndex(const size_t& inindex) const;
    QString UrlAtIndex(const size_t& inindex) const;
    const jfGeneralCategory* ItemAtIndex(const size_t& inindex) const;
    // lookup methods
    const jfGeneralCategory* LookupByID(const size_t& idin) const;
    const jfGeneralCategory* LookupByName(const QString& namein) const;
    // mark manipulation
    bool MarkByName(const QString& inname, bool value, size_t which);
    bool MarkByIndex(size_t inindex, size_t which, bool value=true);
    bool GetMarkByIndex(size_t inindex, size_t which) const;
    bool GetMarkByName(const QString& inname, size_t which) const;
    size_t MarkCount(size_t which) const;
    size_t MarkAll(size_t which, bool value=true);
    // implemented index methods
    virtual bool NextIndex();
    virtual bool BackIndex();
    virtual void ResetIndex();
    virtual bool ToIndex(const int& newindex);
    // some more abstract methods
    virtual QStringList* GetNameList() const;
    virtual QStringList* GetDescriptionList() const;
    virtual QStringList* GetTypeList() const;
    virtual bool DeleteAtIndex(const size_t& dindex);
    // info



    // destructor
    ~jfGenSection();
  protected:
    // special methods
    virtual jfGeneralCategory* MakeTypedCategory() const = 0;
    // file I/O
    virtual bool AddMid2ToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMid2FromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    std::vector<jfGeneralCategory*> maindata;
    size_t matchcount[32];

    // rapid lookup maps
    bool idlookup;
    mutable std::map<QString,size_t> name_lookup;
    mutable std::map<size_t,size_t> id_lookup;
};

//****************************************************************************
