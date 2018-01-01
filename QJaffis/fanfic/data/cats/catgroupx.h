/******************************************************************************
Name    :   catgroupx.h
Author  :   John Q Metro
Purpose :   Data classes for making groupings of categories
Created :   September 9, 2016
Updated :   September 9, 2016
******************************************************************************/
#ifndef CATGROUPX_H
  #define CATGROUPX_H
#endif // CATGROUPX_H
//------------------------------------------
#ifndef GEN_CAT_H_INCLUDED
  #include "gen_cat.h"
#endif // GEN_CAT_H_INCLUDED
#ifndef CAT_COLL_H_INCLUDED
  #include "cat_coll.h"
#endif // CAT_COLL_H_INCLUDED
#ifndef BASEOBJ_H_INCLUDED
    #include "../../../core/utils/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
//------------------------------------------
#include <set>
#include <map>
#include <vector>
#include <QStringList>
/*****************************************************************************/
// needed by map/set for sorting and equivalence
//------------------------------------------------
struct gcSort_byName {
  bool operator()(const jfGeneralCategory* s1, const jfGeneralCategory* s2) const {
    assert(s1 != NULL);
    assert(s2 != NULL);
    int ri = (s1->GetName()).compare(s2->GetName());
    return (ri<0);
  }
};
//------------------------------------------------------------------
struct gcSort_byID {
  bool operator()(const jfGeneralCategory* s1, const jfGeneralCategory* s2) const {
    assert(s1 != NULL);
    assert(s2 != NULL);
    return ((s1->GetID())<(s2->GetID()));
  }
};
//============================================================================
// typedefs fors sets (which are otherwise too long)
typedef std::set<const jfGeneralCategory*,gcSort_byName> jfNCSet;
typedef std::set<const jfGeneralCategory*,gcSort_byID> jfIDSet;
//-----------------------------------------------------------------------
// The category group stores a set of catgeories, either by id or by name
class jfCategoryGroup : public jfBaseObj {
  public:
    // constructors
    jfCategoryGroup(bool idequiv_in);
    jfCategoryGroup(jfGen_CatHolder* source_ptr);
    // basic information
    bool ContainsCategory(const jfGeneralCategory* test_cat) const;
    size_t CategoryCount() const;
    QStringList* NamesList() const;
    std::vector<size_t>* IDList() const;
    virtual QString GetTypeID() const;
    // change or add data
    bool AddCategory(const jfGeneralCategory* input_cat);
    bool RetargetCategories(jfGen_CatHolder* new_category_source);

  protected:
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    // file i/o input
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // setting catlinks from rawdata
    bool SetFromCombos(const QStringList& combolist);
    bool SetFromIDs(const QStringList& sectionlist, const std::vector<size_t>& idlist );

    // data
    bool idequiv;  // true if we use gcSort_byID, otherwise we use gcSort_byName
    jfNCSet namestore;
    jfIDSet idstore;

    jfGen_CatHolder* loaddata_ptr;
    size_t loadcount;
};
//======================================================================
// This class is for storing multiple category groups, with i/o and a quick way
// to check for crossovers
class jfCatGroupCollection : public jfBaseObj {
  public:
    // constructors and destructors
    jfCatGroupCollection(bool idequiv_in);
    jfCatGroupCollection(jfGen_CatHolder* source_ptr);
    ~jfCatGroupCollection();
    // informational methods
    size_t CollectionCount() const;
    bool CategoryInGroupAtIndex(const jfGeneralCategory* categ_in, size_t cindex);
    jfCategoryGroup* GroupContainingCategory(jfGeneralCategory* categ_in);
    // altering info
    jfCategoryGroup* MakeNewGroup(const QString& name);
    bool InsertCategoryInGroupAt(size_t cindex, const jfGeneralCategory* categ_in );
    bool InsertCategoryIntoGroup(const jfCategoryGroup* thisgroup, const jfGeneralCategory* categ_in );

  protected:
    // helper methods
    bool BuildQuickLookup();

    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    // file i/o input
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);

    // primary data
    bool idequiv;
    std::vector<jfCategoryGroup*> maindata;

    // quick lookup
    std::map<QString,jfCategoryGroup*> name_lookup;
    std::map<size_t, jfCategoryGroup*> id_lookup;

    // load Data
    jfGen_CatHolder* loaddata_ptr;
    size_t loadcount;
};

/*****************************************************************************/
