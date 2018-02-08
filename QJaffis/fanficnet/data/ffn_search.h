/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_search.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net search
// Created:     June 9, 2010
// Conversion to Qt Started July 10, 2014
// Updated:     February 8, 2018 (exclude genres)

/////////////////////////////////////////////////////////////////////////////
#define JF_FFNSEARCH

#ifndef SEARCHCORE_H_INCLUDED
  #include "../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef SKELCOLL_H_INCLUDED
  #include "../../core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
#ifndef CATGROUP_H
  #include "../categories/catgroup.h"
#endif // CATGROUP_H
#ifndef FFNCATPDATA
  #include "../categories/ffn_catpdata.h"
#endif
//**************************************************************************
// fanfiction,net specific search object
class jfFFNSearch : public jfSearchCore {
    Q_OBJECT
  public:
    size_t search_index;        // 0 to 31
    // constructors
    jfFFNSearch();
    jfFFNSearch(jfFFN_Categories* cat_datain, size_t search_indexin);
    // setting data
    bool SetData(jfFFN_Categories* cat_datain, size_t search_indexin);
    void ApplySelection();
    bool SetSearchValues(size_t frin, size_t genin, size_t wcin, size_t lanin, size_t exlgenin, size_t compin);
    int GetSearchValue(size_t pindex) const;
    // getting data
    jfFFN_CatP_SelCat* GetSelector();
    jfFFN_Categories* GetHolder();
    size_t GetSSIndex() const;
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
    void SetCurrCat(const jfFFN_CategoryCore* inval);
    const jfFFN_CategoryCore* GetCurrCat() const;
    // the destructor
    ~jfFFNSearch();
  signals:
    void SendInsertSignal();
  private:
    bool WriteListOfCats(QString fname) const;
    // i/o methods for google specific data
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    // category data
    jfFFN_Categories* cat_data;  // holds downloaded category info
    // source category index
    const jfFFN_CategoryCore* curr_cat;
    jfFFN_CatList* list_to_search;
    int catdex;
    // selection data from the interface
    jfFFN_CatP_SelCat* selector;
    // search values used in building the url
    size_t sv[6];
};
//**************************************************************************
