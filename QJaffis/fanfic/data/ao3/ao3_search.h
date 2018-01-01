/******************************************************************************
Name    :   ao3_search.h
Author  :   John Q Metro
Purpose :   Declares search object of AO3
Created :   August 28, 2012
Conversion to Qt Started March 29, 2014
Updated :   July 20, 2017 (added English-Only option)
******************************************************************************/
#ifndef AO3_SEARCH_H_INCLUDED
#define AO3_SEARCH_H_INCLUDED
#endif // AO3_SEARCH_H_INCLUDED
//-----------------------------------
#ifndef SEARCHCORE_H_INCLUDED
  #include "../../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef CAT_COLL_H_INCLUDED
  #include "../cats/cat_coll.h"
#endif // CAT_COLL_H_INCLUDED
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
/*****************************************************************************/
// fanfiction,net specific search object
class jfAO3Search : public jfSearchCore {
  public:
    size_t search_index;        // 0 to 31
    // constructors
    jfAO3Search();
    jfAO3Search(jfGen_CatHolder* cat_datain, size_t search_indexin);
    // setting data
    bool SetData(jfGen_CatHolder* cat_datain, size_t search_indexin);
    void ApplySelection();
    void SetSearchOptions(size_t irindex, size_t isindex, size_t iwindex, QString iintag, bool iconly);
    void SetOrder(size_t in_oindex);
    // getting data
    jfGenCat_PickData* GetSelector();
    jfGen_CatHolder* GetHolder();
    size_t GetSSIndex() const;
    size_t GetSearchValue(size_t qindex) const;
    QString GetExtraSTag() const;
    bool GetCompletedValue() const;
    // implemented method for sending category data
    virtual void DispatchCategory();
    // implemeted virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetCatName() const;
    virtual QString GetCatHeader() const;
    virtual QString GetIndexUrl(size_t inp_index) const;
    virtual QString GetHTMLHeader(size_t result_category) const;
    virtual void MakeLocalSkeleton(jfSkeletonBase* src);
    void LoadValues(jfSkeletonParser* inparser, size_t result_categ);
    // source category indexing
    void ResetIndex();
    bool NextIndex();
    bool LastCat() const;
    size_t GetNumCats() const;
    void SetCurrCat(const jfAO3_Category* inval);
    const jfAO3_Category* GetCurrCat() const;
    // the destructor
    ~jfAO3Search();
  private:
    // i/o methods for google specific data
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    // category data
    jfGen_CatHolder* cat_data;  // holds downloaded category info
    size_t numscats;
    long icatindex;
    // source category index
    const jfAO3_Category* curr_cat;
    // selection data from the interface
    jfGenCat_PickData* selector;
    // search parameters
    size_t rindex, sindex, windex;
    QString intag;
    bool conly, eng_only;
    size_t ord_index;
};
/*****************************************************************************/
