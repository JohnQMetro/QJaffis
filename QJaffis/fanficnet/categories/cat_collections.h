/***************************************************************************
 * Name    : cat_collections.h
 * Author  : John Q Metro
 * Purpose : Declares the groupings / collections that directly hold FFN
 *            Category objects.
 * Created : May 25, 2014
 * Updated : September 6, 2016
 **************************************************************************/
#ifndef CAT_COLLECTIONS_H
#define CAT_COLLECTIONS_H
#endif // CAT_COLLECTIONS_H
//---------------------------------------
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef JFFFNSCATCORE
  #include "ffn_catcore.h"
#endif // JFFFNSCATCORE
//---------------------------------------
#include <set>
#include <map>
#include <bitset>
#include <QMutex>
//**************************************************************************
/* Core collection for section catgeories, using name map. The name is
 * contained in the value object, but we need quick access using only the
 * name, so a map is used instead of a set */
class jfFFN_SectionMap : public std::map<QString,jfFFN_SectionCategory*,ltstr> {
  public:
    // some convenience methods
    bool Contains(const QString& cckey) const;
    bool AddCheck(jfFFN_SectionCategory* incat);
    void Add(jfFFN_SectionCategory* incat);
    bool MarkByName(const QString& inname, bool value, size_t which);
    bool GetMarkByName(const QString& inname, size_t which) const;
    bool TestAndMarkByName(const QString& inname, size_t which, bool value, int& result);
    size_t DeleteAndClear();
    jfFFN_SectionMap::iterator IteratorByIndex(size_t idxval);
    // extra methods for the marked categories storage
    size_t AppendToList(jfFFN_CatList& inlist);

};
//-------------------------------------------------------------------------
/* Now, the actual collection class for FFN Sections, with i/o and all the
 * other usual extra methods. For marking, we will use a separate jfFFN_SectionMap
 * to keep a quick access list until marking is done. */
class jfFFNSection : public jfBaseCollection {
  public:
    // constructor
    jfFFNSection();
    // implemented virtual methods
    virtual QString GetTypeID() const;
    // setting info
    bool AddCategory(jfFFN_SectionCategory* cat_to_add);
    bool MarkAll(size_t which, bool value);
    bool MarkByName(const QString& inname, bool value, size_t which);
    bool MarkAndAddByName(jfFFN_SectionMap& marked_collection,const QString& inname, size_t which);
    bool MarkAndAddAll(jfFFN_SectionMap& marked_collection, size_t which);
    bool UnmarkAndRemoveByName(jfFFN_SectionMap& marked_collection,const QString& inname, size_t which);
    // index manipulation
    virtual void SaveIndex();
    virtual bool RestoreIndex();
    virtual bool NextIndex();
    virtual bool BackIndex();
    virtual void ResetIndex();
    virtual bool ToIndex(const int& newindex);
    // current item operations
    jfFFN_SectionCategory* GetCurrent();
    QString GetCurrentName() const;
    bool MarkCurrentAndAdd(jfFFN_SectionMap& marked_collection, size_t which);
    // some more abstract methods
    virtual QStringList* GetNameList() const;
    virtual QStringList* GetDescriptionList() const;
    virtual QStringList* GetTypeList() const;
    virtual bool DeleteAtIndex(const size_t& dindex);
    // extra methods
    const jfFFN_SectionCategory* GetCatByName(const QString& catname) const;
    // destructor
    ~jfFFNSection();
  protected:
    // implemented virtual io methods
    virtual bool AddMid2ToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMid2FromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    jfFFN_SectionMap maindata;
    jfFFN_SectionMap::iterator cptr;
    jfFFN_SectionMap::iterator bptr;
    // marking
    size_t markcount[32];
};
//**************************************************************************
// functor object used in creating a set of crossover categories
struct FFN_CrossCat_IDCmp {
    bool operator()(const jfFFN_CrossoverCategory* const &a, const jfFFN_CrossoverCategory* const &b) const {
      if ((a->GetID())<(b->GetID())) return true;
      if ((a->GetID())>(b->GetID())) return false;
      return ((a->GetSecondID())<(b->GetSecondID()));
    }
};
//--------------------------------------------------------------
/* Core collection crossover catgeories, using set based upon the 2 IDs */
class jfFFN_CrossoverSet : public std::set<const jfFFN_CrossoverCategory*,FFN_CrossCat_IDCmp> {
  public:
    // some convenience methods
    bool Contains(const jfFFN_CrossoverCategory* look) const;
    bool AddCheck(jfFFN_CrossoverCategory* incat);
    size_t DeleteAndClear();
    jfFFN_CrossoverSet::iterator IteratorByIndex(size_t idxval);
};

//-------------------------------------------------------------------------
/* Now, the actual collection class for **ALL** FFN Crossover categories, with i/o
 * and all the other usual extra methods */
class jfFFN_CrossCatStore : public jfBaseCollection {
  public:
    // constructor
    jfFFN_CrossCatStore();
    // implemented virtual methods
    virtual QString GetTypeID() const;
    // manipulating contents
    jfFFN_CrossoverCategory* AddCategory(const jfFFN_CrossoverCategory* toadd, bool& success);
    jfFFN_CrossoverCategory* AddCategoryAndSetSection(const jfFFN_CrossoverCategory* toadd,
                                            bool& success,size_t idptr, const QString& insection);
    jfFFN_CrossoverCategory* AddUpdateCategory(jfFFN_CrossoverCategory* toadd, bool& newinsert);
    jfFFN_CrossoverCategory* GetByIDs(size_t inid1, size_t inid2);
    void ClearMarks(const size_t& which);
    // index manipulation
    virtual void SaveIndex();
    virtual bool RestoreIndex();
    virtual bool NextIndex();
    virtual bool BackIndex();
    virtual void ResetIndex();
    virtual bool ToIndex(const int& newindex);
    // some more abstract methods
    virtual QStringList* GetNameList() const;
    virtual QStringList* GetDescriptionList() const;
    virtual QStringList* GetTypeList() const;
    virtual bool DeleteAtIndex(const size_t& dindex);
    // destructor
    ~jfFFN_CrossCatStore();
  protected:
    // implemented virtual io methods
    virtual bool AddMid2ToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMid2FromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    jfFFN_CrossoverSet maindata;
    jfFFN_CrossoverSet::iterator cptr;
    jfFFN_CrossoverSet::iterator bptr;
    mutable QMutex mlock;
};
//**************************************************************************
