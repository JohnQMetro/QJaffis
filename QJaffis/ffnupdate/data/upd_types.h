/////////////////////////////////////////////////////////////////////////////
// Name:        upd_types.h
// Author :     John Q Metro
// Purpose :    Misc types and untils helpful in fanfiction.net updating
// Created:     January 18, 2011
// Conversion to Qt Started September 25, 2013
// Updated:     August 8, 2016
/////////////////////////////////////////////////////////////////////////////
#ifndef UPD_TYPES_H_INCLUDED
#define UPD_TYPES_H_INCLUDED
#endif // UPD_TYPES_H_INCLUDED
//---------------------------------------------
#ifndef JFFNFICOBJ
  #include "../../fanficnet/data/ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED

//****************************************************************************
// simple wrapping of author info
class jfAuthorInfo {
  public:
    // constructor
    jfAuthorInfo(size_t in_authorid, QString in_authname);
    jfAuthorInfo(const jfAuthorInfo& inval);
    // getting info
    size_t GetAuthorID() const;
    QString GetAuthorName() const;
    QString GetUrl() const;
  protected:
    QString author_name;
    size_t author_id;
};
//=========================================================================
// the fic object class when extracted from an author's page
class jfFFNItemAuthor : public jfFFNItemCore {
  public:
    // constructors
    jfFFNItemAuthor();
    // setting data
    bool SetFromString(const QString& indata, QString& outerr);
    // getting data
    QString GetCatname() const;
    // implemented virtual methods
    virtual QString GetTypeID() const;
    virtual bool LoadValues(jfSkeletonParser* inparser) const;
  protected:
    virtual bool GetLinkTitle(jfStringParser& xparser, QString& outerr);
    // io methods
    virtual bool AddMoreExtraStuff(QTextStream* outfile) const;
    virtual bool ReadMoreExtraStuff(jfFileReader* infile);
    // since we do not use category links, we do check the category name
    QString catname;
};
//=====================================================================
// bundles fic id and published date for a limit: do not get fics with lower id or earlier date
class jfFicLimit {
  public:
    // constructors and data methods
    jfFicLimit(QDate dlimit, size_t idlimit);
    void ChangeLimit(size_t inval, bool innodate);
    size_t IdLimit() const;
    // testing...
    bool IsLater(const jfFFNItemCore* test) const;
  protected:
    QDate fpublished;
    size_t id_limit;
    bool nodate;
};
//****************************************************************************
