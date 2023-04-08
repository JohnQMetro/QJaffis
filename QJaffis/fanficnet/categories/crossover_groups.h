/***************************************************************************
 * Name    : crossover_groups.h
 * Author  : John Q Metro
 * Purpose : Indirect indexes of FFN crossover caregories.
 * Created : May 28, 2014
 * Updated : December 9, 2018
 **************************************************************************/
#ifndef CROSSOVER_GROUPS_H
#define CROSSOVER_GROUPS_H
#endif // CROSSOVER_GROUPS_H
//------------------------------------------
#ifndef JFFFNSCATCORE
  #include "ffn_catcore.h"
#endif // JFFFNSCATCORE
//-------------------------------------------
#ifndef CAT_COLLECTIONS_H
  #include "cat_collections.h"
#endif // CAT_COLLECTIONS_H
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
/**************************************************************************/
/* Map for storing links to FFN Crossover categories that uses the names
 * for the other category as the key, specifically <Name> (<Section Name>) */
class jfFFN_CrossCatOtherMap : public std::map<QString,jfFFN_CrossoverCategory*,ltstr> {
  public:
    bool ContainsTag(const QString& intag) const;
    bool AddCheck(size_t this_id, jfFFN_CrossoverCategory* toadd);
    bool UnmarkAndRemove(const QString& ftag, size_t which);
    size_t AppendToList(jfFFN_CatList& inlist);
};
//------------------------------------------------------------------------
class jfFFN_CrossoverGroup : public jfBaseObj{
  public:
    // constructor...
    jfFFN_CrossoverGroup();
    jfFFN_CrossoverGroup(QString snamein, jfFFN_CrossCatStore* incatstore);
    // implemented virtual methods
    virtual QString GetTypeID() const;
    // getting info
    bool IsSorted() const;
    QStringList* GetKeyNames() const;
    QString GetCatName() const;
    size_t GetCount() const;
    bool ContainsKey(const QString& sec1, const QString& cat1) const;
    int AppendSection(const QString sec_name, jfStrBoolList* destination) const;
    const jfFFN_CrossoverCategory* GetCatPointer(const QString& sec1, const QString& cat1) const;
    // marking
    bool MarkAndAddByCombo(jfFFN_CrossCatOtherMap& mark_cgroup, const QString& incombo, size_t which);
    size_t MarkAndAddAll(jfFFN_CrossCatOtherMap& mark_cgroup, size_t which);
    bool MarkAndAddCurrent(jfFFN_CrossCatOtherMap& mark_cgroup, size_t which);
    // altering data
    void SetSectionName(QString insection);
    void SetCatName(const QString& incname);
    bool SortLinks();
    void ClearContents();
    bool AddCategoryWhenUnsorted(jfFFN_CrossoverCategory* incat);
    // indexing (sorted only)
    void ResetIndex();
    bool NextIndex();
    QString CurrentName() const;
    // Full Copy (sorted only)
    bool GetFullIntegratedCopy(jfFFN_CrossoverGroup*& out_group, jfFFN_CrossCatStore* new_catstore) const;
    // destructor
    ~jfFFN_CrossoverGroup();
  protected:
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    // file i/o input
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
  // internal data
    QString secname, catname;
    bool sorted;
    size_t tcatc;
  // index stuff
    bool before_first;
    jfFFN_CrossCatOtherMap::iterator cptr;
  // links to external data
    std::vector<jfFFN_CrossoverCategory*>* unsorted_links;
    jfFFN_CrossCatOtherMap sorted_links;
    jfFFN_CrossCatStore* cat_store;
};
//=========================================================================
/* We need to store 2 types of data. This type is intended to avoid having 2 maps.
   The markmap link is to store temporary lists getting cats to search. */
typedef struct  {
  jfFFN_HalfCrossover* catlink;
  jfFFN_CrossoverGroup* grouplink;
  jfFFN_CrossCatOtherMap* markmap;
} jf_HCC_Pair;
//---------------------------------------------------------------
class jfFFN_CrossSectionMap : public std::map<QString,jf_HCC_Pair,ltstr> {
  public:
    bool AddHCross(jfFFN_HalfCrossover* incat);
};
//--------------------------------------------------------------
class jfFFN_CrossoverSection : public jfBaseObj {
  public:
    // constructor...
    jfFFN_CrossoverSection();
    jfFFN_CrossoverSection(jfFFN_CrossCatStore* incat_store);
    // implemented virtual methods
    virtual QString GetTypeID() const;
    // indexes
    void ResetIndex();
    bool NextIndex();
    size_t CatCount() const;
    size_t UCatCount() const;
    // url indexes
    void ResetUrlIndex();
    bool NextUrlIndex();
    bool NextUrlIndex(bool& isupdatable);
    bool UpdatableAtUrlIndex() const;
    QString* DownURLAtIndex() const;
    // getting data
    QString IndexLink(bool next, bool plink) const;
    int AppendSection(const QString& subcat, const QString& sec_name, jfStrBoolList* destination) const;
    bool ContainsCat(const QString& cat1, const QString& sec2, const QString& cat2) const;
    QStringList* GetCrossList(const QString& catx) const;
    QStringList* GetCatNames() const;
    QString NameAtIndex(bool addcross) const;
    QString NameAtUrlIndex() const;
    int StoryCountAtIndex() const;
    int EstStoryCount(const QString& hcross_name) const;
    bool GetDoUpdateAtIndex() const;
    // getting crossover pointers
    const jfFFN_CrossoverCategory* GetCrossoverCatPointer(QString cat1, QString sec2, QString cat2) const;
    const jfFFN_HalfCrossover* GetCrossoverIndex(const QString& cat1) const;
    // altering data
    bool AddHCrossCategory(jfFFN_HalfCrossover* input);
    bool InsertGroupAtIndex(jfFFN_CrossoverGroup* in_group);
    bool SortAll();
    bool SetNoUpdateAtIndex();
    bool SetUpdatesByComparing(const jfFFN_CrossoverSection* old_data);
    // marking (non nullmode)
    size_t ApplyOrAtIndex(const jfSimpleExpr* or_exp, size_t which, bool& okay);
    size_t ApplyAndAtIndex(const jfSimpleExpr* and_exp, size_t which, bool& okay);
    bool MarkCombo(const QString& fcatname, const QString& markcombo, size_t which);
    bool MarkSpecificCrossover(const QString& fcatname, const QString& ssecname, const QString& scatname, size_t which);
    bool MarkAllCategory(const QString& fcatname, size_t which);
    bool MarkEverything(size_t which);
    bool ApplyExpressionMarking(const jfSimpleExpr* or_exp, const jfSimpleExpr* and_exp, size_t which);
    bool AddToCategoryList(jfFFN_CatList& inlist);
    // an update special
    bool GetIntegratedCopyByName(const QString& oldname, jfFFN_CrossoverGroup*& out_group, jfFFN_CrossCatStore* new_catstore) const;
    // destructor
    ~jfFFN_CrossoverSection();
  protected:
    QString CrossLinkModify(QString srclink) const;
    bool EmptyAtIndex() const;
    bool RemoveAtIndex();
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    // file i/o input
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
  // internal data
    bool before_first;
    jfFFN_CrossSectionMap::iterator index;
    bool before_first_url;
    jfFFN_CrossSectionMap::iterator url_index;
    jfFFN_CrossSectionMap crossgroups;
    bool nullmode;
    size_t tcount, uccount;
  // external links
    jfFFN_CrossCatStore* cat_store;
};

/**************************************************************************/
