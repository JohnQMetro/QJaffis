/////////////////////////////////////////////////////////////////////////////
// Name:        baseobj.h
// Author :     John Q Metro
// Purpose :    Declares the most basic data classes. Based on the old
//              jaffis_base.h
// Created:     March 17, 2009
// Conversion to Qt started March 12, 2013
// Updated:     February 14, 2018 (virtual destructors)
/////////////////////////////////////////////////////////////////////////////
// For compilers that support precompilation, includes "wx.h".

//----------------------------------------------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #define BASEOBJ_H_INCLUDED
#endif // BASEOBJ_H_INCLUDED

#ifndef STRINGPARSER_H_INCLUDED
  #include "../utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED

#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED

#ifndef HTMLSKELTON_H_INCLUDED
  #include "../utils/htmlskelton.h"
#endif // HTMLSKELTON_H_INCLUDED

#include <QString>
#include <QObject>
#include <QStringList>
#include <QTextStream>
#include <QColor>
#include <bitset>
#include <vector>
//*****************************************************************************
/* Descendants of jfBaseObj are stored in text files the following way:
Line 1: "###" <TypeID String> ";" <num_id> ";" <validdata>
Line 2: <name> ";" <description> (with ; , #, and newlines escaped)
subsequent lines
*/
 //===========================================================================
class jfBaseObj {
  public:
    jfBaseObj();
    jfBaseObj(size_t src_id, const QString& nname);
    virtual ~jfBaseObj();
    // accessing the internals
    bool IsValid() const;
    QString GetName() const;
    bool SetName(const QString& newname);
    QString GetDescription() const;
    void SetDescription(const QString& newdesc);
    size_t GetID() const;
    void SetID(size_t src_id);
    // special meta-information
    virtual QString GetTypeID() const=0;
    // file i/o
    virtual bool AddToFile(QTextStream* outfile) const;
    bool GetFromFile(jfFileReader* infile);
  protected:
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const =0;
    virtual bool AddRestToFile(QTextStream* outfile) const =0;
    // file i/o input
    virtual bool ReadMiddleFromFile(jfFileReader* infile) =0;
    virtual bool ReadRestFromFile(jfFileReader* infile) =0;
    // internal data
    size_t num_id;          // stores an assigned id number
    bool validdata;           // whenever a data setting method fails, is false
    QString name,description; // stories item name and description
};
//==========================================================================
/* a class intermediate between jfBaseObj and jfBasePD, some things have urls but
are not search results */
class jfUrlBase : public jfBaseObj {
  public:
    std::bitset<32> mark;
    // the constructor
    jfUrlBase();
    // getting data methods
    QString GetUrl() const;
    const QStringList* GetTokTitle() const;
  protected:
    // internal methods
    void CopyFrom(const jfUrlBase* src);
    void CopyTo(jfUrlBase*dest) const;
    virtual void SetValid();
    // internal data
    QString primarylink;
};

//============================================================================
// a forward declaration
class jfResultUnit;
// a very basal data class for searched items such as fics and pages
class jfBasePD : public jfUrlBase {
  public:
    // basic search data
    bool included, ignore;
    // the base constructor
    jfBasePD();
    jfBasePD(const size_t& newrank);
    // getting data methods
    size_t GetRank() const;
    jfResultUnit* makeResultUnit() const;
    QString GetDescExtract(size_t lines, size_t maxlen) const;
    // to string functions
    virtual QString ToText() const =0;
    virtual QString ToDisplayHTML() const=0;
    // tokenized matching stuff
    const QStringList* GetTokSummary() const;
    void ClearTok() const;
    // added stuff
    void ChangeRank(const size_t& newrank);
    virtual bool LoadValues(jfSkeletonParser* inparser) const = 0;
    virtual void ProcessDescription() = 0;
    // the destructor
    virtual ~jfBasePD();
  protected:
    // other stuff
    void CopyOver(jfBasePD* destination) const;
    void LoadCoreValues(jfSkeletonParser* inparser) const;
    // i/o methods
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    size_t rank;
    // special stuff used in tokenized matching
    mutable QStringList *toksummary;
};
//============================================================================
/* The following class is basically a compact extract of jfBasePD used in
displaying results */
class jfResultUnit {
  friend class jfBasePD;
  public:
    QColor bgcolor;
    // getting the string data
    QString GetUrl() const;
    QString GetHTML() const;
    QString GetName() const;
    size_t GetID() const;
    // marking data
    bool HasMark()const;
    bool Marked(const size_t& catindex) const;
    // specials
    bool SameID(const size_t& cval) const;
    const jfBasePD* Sourcelink() const;

  protected:
    // internal data
    quint32 marked;
    QString htmldata;
    const jfBasePD* slink;
};

Q_DECLARE_METATYPE(jfResultUnit*);


//*****************************************************************************
