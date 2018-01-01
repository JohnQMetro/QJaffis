/////////////////////////////////////////////////////////////////////////////
// Name:        ffns_catcore.h
// Author :     John Q Metro
// Purpose :    Declares the Fanfiction.Net category core data classes
//              jaffis_base.h
// Created:     April 4, 2010
// Conversion to Qt Started September 25, 2013
// Updated:     July 30, 2015
/////////////////////////////////////////////////////////////////////////////
#define JFFFNSCATCORE

#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../../core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED

#include <vector>
#include <bitset>
//***************************************************************************
enum jf_FFCTYPE { jfx_CAT,jfx_HCROSS,jfx_CROSS };

// base ffn category info store
class jfFFN_CategoryCore : public jfUrlBase {
  public:
    bool discard;
    // constructors
    jfFFN_CategoryCore();
    jfFFN_CategoryCore(const jfFFN_CategoryCore& insource);
    // custom methods
    virtual bool SameCat(const jfFFN_CategoryCore* intest) const = 0;
    virtual jf_FFCTYPE GetCatType() const = 0;
    virtual QString MakeFinder() const = 0;
    size_t GetEstStoryCount() const;
    // virtual methods
    virtual QString GetTypeID() const=0;
    // destructor
    ~jfFFN_CategoryCore();
  protected:
    // addtional methods
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    size_t ccount;
};
//========================================================================
class jfFFN_CatList : public std::vector<jfFFN_CategoryCore*> {
  public:
    bool ClearMarks(size_t which);
};

//========================================================================
/* Due to similarities, we have a common base class for Section Categories
 * and Categories-for-all-crossovers-attached-to-one-category. */
class jfFFN_CatBase : public jfFFN_CategoryCore {
  public:
    // constructors
    jfFFN_CatBase();
    jfFFN_CatBase(const jfFFN_CatBase& incat);
    // additional methods
    QString GetSectionName() const;
    virtual bool SetFromSource(const QString& sname, const QString& inraw, QString& parse_err) = 0;
    // implemented virtual methods
    virtual bool SameCat(const jfFFN_CategoryCore* intest) const;
    virtual jf_FFCTYPE GetCatType() const = 0;
    virtual QString MakeFinder() const = 0;
    virtual QString GetTypeID() const = 0;
  protected:
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    QString secname;
};

//========================================================================
// ffn category class, non-crossover version
class jfFFN_SectionCategory : public jfFFN_CatBase {
  public:
    // constructors
    jfFFN_SectionCategory();
    jfFFN_SectionCategory(const jfFFN_SectionCategory& incat);
    jfFFN_SectionCategory(const QString& sname, const QString& inraw);
    // additional methods
    virtual bool SetFromSource(const QString& sname, const QString& inraw, QString& parse_err);
    // implemented virtual methods
    virtual jf_FFCTYPE GetCatType() const;
    virtual QString MakeFinder() const;
    virtual QString GetTypeID() const;
  protected:
};
//----------------------------------------------------------
/* 'half-crossovers' : The link points to a listing of crossover stories in
 * which at least one of the categories is the same. */
class jfFFN_HalfCrossover : public jfFFN_CatBase {
  public:
    bool doupdate;
    // constructors
    jfFFN_HalfCrossover();
    jfFFN_HalfCrossover(const jfFFN_HalfCrossover& incat);
    jfFFN_HalfCrossover(const QString& sname, const QString& inraw);
    // additional methods
    virtual bool SetFromSource(const QString& sname, const QString& inraw, QString& parse_err);
    // implemented virtual methods
    virtual jf_FFCTYPE GetCatType() const;
    virtual QString MakeFinder() const;
    virtual QString GetTypeID() const;
  protected:
};
//========================================================================
/* Crossover version of Fanfiction.Net category. The category represents
 * specific crossover X + Y, like 'Harry Potter + Doctor Who' */
class jfFFN_CrossoverCategory : public jfFFN_CategoryCore {
  public:
    // constructors
      jfFFN_CrossoverCategory();
      jfFFN_CrossoverCategory(const jfFFN_CrossoverCategory& incat);
      jfFFN_CrossoverCategory(const QString icatname, size_t icatid, const QString& inraw);
      jfFFN_CrossoverCategory(size_t id1, size_t id2);
    // extra methods
      QString GetCatPart(bool first) const;
      QString GetSectionPart(bool first) const;
      size_t GetSecondID() const;
      bool SetFromSource(const QString icatname, size_t icatid, const QString& inraw, QString& parse_err);
      bool SetSectionByID(size_t idptr, const QString& insection);
      void CopySections(const jfFFN_CrossoverCategory &source);
      int GetOtherID(size_t cid) const;
      QString GetOtherCombo(size_t cid) const;
      QString GetOtherSection(size_t cid) const;
      bool ContainsID(size_t cid) const;
    // implemented virtual methods
      virtual bool SameCat(const jfFFN_CategoryCore* intest) const;
      virtual jf_FFCTYPE GetCatType() const;
      virtual QString MakeFinder() const;
      virtual QString GetTypeID() const;
  protected:
  // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
  // internal data
    QString cat1, cat2;
    QString section1, section2;
    size_t cat2_id;
};
//-----------------------------------------------------------------------
// quick typedef
typedef std::vector<jfFFN_CategoryCore*> jfFFN_CrossCatList;
//***************************************************************************
