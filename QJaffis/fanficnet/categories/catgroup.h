/***************************************************************************
 * Name    : catgroup.h
 * Author  : John Q Metro
 * Purpose : Top level FFN category classes
 * Created : June 23, 2014
 * Updated : July 22, 2016
 **************************************************************************/
#ifndef CATGROUP_H
#define CATGROUP_H
#endif // CATGROUP_H
//----------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef CROSSOVER_GROUPS_H
  #include "crossover_groups.h"
#endif // CROSSOVER_GROUPS_H
#ifndef JFFFNSCATCORE
  #include "ffn_catcore.h"
#endif // JFFFNSCATCORE
#ifndef FFNCATPDATA
  #include "ffn_catpdata.h"
#endif
/**************************************************************************/
// section constant data
namespace jfFFN_SECDATA {
  const size_t scount = 9;
  const size_t scountm = scount-1;
  extern const QString section_names[];
  extern const QString crossover_names[];
  extern const QString section_strings[];
  int SectionName2Index(const QString& inname);
  QString IndexToURL(size_t index, bool cross);
}
//-----------------------------------------
// the main holder
class jfFFN_Categories : public jfBaseObj {
  public:
    // constructor
    jfFFN_Categories();
    // checking crossovers for contents
    bool ContainsCat(const QString& sec1, const QString& cat1, const QString& sec2, const QString& cat2) const;
    int AppendSection(const QString& sec1, const QString& cat1, const QString& sec2, jfStrBoolList* destination) const;
    // Getting Lists of Names
    QStringList* GetCategories(size_t sec_idx) const;
    QStringList* GetCrossoverCategories(size_t sec_idx, const QString& cross_name) const;
    QStringList* GetCrossSectionList(size_t sec_idx) const;
    // Adding downloaded data
    bool SetSection(jfFFNSection* new_section);
    bool SetCrossoverSection(jfFFN_CrossoverSection* new_cross_section);
    bool SetCrossoverGroup(jfFFN_CrossoverGroup* new_group);
    // indexing
    void ResetIndex();
    bool NextIndex();
    // url indexes
    QString UrlAtIndex() const;
    QString UrlAtNextIndex() const;
    QString NameAtIndex() const;
    QString NameAtUrlIndex(size_t section_index) const;
    bool UpdatableAtIndex() const;
    QString* CurrentCrossoverURL(size_t section_index) const;
    bool UrlIndexToStart(size_t section_index);
    bool NextUrlIndex(size_t section_index);
    bool NextUrlIndex(size_t section_index, bool& updatable);
    size_t CrossoverIndex() const;
    // item info
    size_t ItemsInSection() const;
    size_t ItemsInSection(size_t sec_idx) const;
    bool SortAndComplete();
    // applying the category selector
    jfFFN_CatList* MakeSearchList(jfFFN_CatP_SelCat* selector, size_t which);
    // other
    virtual QString GetTypeID() const;
    const jfFFN_CategoryCore* FindCatFromFinder(const QString& infind) const;
    void MakeCrossStore();
    jfFFN_CrossCatStore* GetCrossoverStoragePtr();
    // basic info
    size_t GetPhase() const;
    bool IsDataSet() const;
    // update mode methods
    void Set_OldDataLink(const jfFFN_Categories* in_old_data);
    bool CompareAndSetUpdates();
    bool CopyDataAtIndex();
    // destructor
    ~jfFFN_Categories();
  protected:
    // marking helper methods
    int ApplySectionAnd(const jfSimpleExpr* in_andexp, size_t which, size_t sindex);
    int ApplySectionOr(const jfSimpleExpr* in_orexp, size_t which, size_t sindex);
    // file i/o
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // the data
    bool dataset;
    jfFFNSection* sections[jfFFN_SECDATA::scount];
    jfFFN_CrossCatStore* cross_store;
    jfFFN_CrossoverSection* cross_sections[jfFFN_SECDATA::scount];
    // indexing
    size_t phase, sidx, cidx, ccount;
    // marking
    jfFFN_SectionMap* csecmap[jfFFN_SECDATA::scount];
    // update mode
    const jfFFN_Categories* old_data;
};
//==============================================================
// a forward declaration to be changed later...
class jfFFN_CatManager;

namespace ffn_catorganizer {
  extern jfFFN_CatManager* main_manager;
  extern const QString fname;
}
//=============================================================
class jfFFN_CatManager {
  public:
  // the constructor
  jfFFN_CatManager();
  // basic info and simple methods
  bool Empty() const;
  jfFFN_Categories* GetNewHolder();
  jfFFN_Categories* GetHolder();
  bool NewDone();
  bool DiscardNew();
  // file i/o
  bool WriteToFile() const;
  bool ReadFromFile();
  // registering searchers
  size_t RegisterSearch(const QWidget* in_search);
  bool RemoveSearch(size_t which);
  // destructor
  ~jfFFN_CatManager();
  protected:
    // the category holders
    bool isempty;
    bool changed;
    jfFFN_Categories* cur_holder;
    jfFFN_Categories* new_holder;
    // info on the searches that are currently active and using the holder
    size_t number_of_searches;
    const QWidget* searchers[32];
};

/**************************************************************************/
