//*******************************************************************************
// Name:        ffn_mulcoll.h
// Author :     John Q Metro
// Purpose :    Holder for multiple-category search results from Fanfiction.Net
// Created:     June 17, 2010
// Conversion to Qt Started July 11, 2014
// Updated:     August 15, 2016
//*******************************************************************************
#define FFN_MULTICOLL

#ifndef JF_FFNSEARCH
  #include "ffn_search.h"
#endif
#ifndef FFN_ITEMCOLL
  #include "ffn_itemcoll.h"
#endif
#ifndef RESULT_MULTI_H
  #include "../../core/structs/result_multi.h"
#endif // RESULT_MULTI_H
//*******************************************************************************
// a forward declaration
class jfFFN_Categories;

// the multiple category results class
class jfFFNResColl : public jfResultMultiCollection {
  public:
    // the constructor
    jfFFNResColl(jfFFNSearch* xsearch_in);
    bool SetCatLinks(const jfFFN_Categories* inlinks);
    // other stuff
    virtual QString GetTypeID() const override;
    const jfFFN_CategoryCore* GetCat(size_t zindex) const;
    size_t FirstIDAtIndex(size_t index_in) const;
    // methods for integrating updates
    bool InsertNewCollection(jfFFNItemCollection* new_coll_src);
    bool InsertFromNewCollection(jfFFNItemCollection* new_coll_src);
    // i/o
    bool WriteToHTML(size_t result_category) override;
    // extracting information from a particular item
    jfFicExtract* GetFicExtract(size_t c_index, size_t i_index) const;
  protected:
    void WriteHtmlHeader(jfHtmlParams* indata) override;
    // i/o methods
    virtual bool AddRestToFile(QTextStream* outfile) const override;
    virtual jfSearchResultItemCollectionBase* MakeEmptyCollection() const override;
    virtual jfSearchResultItemCollectionBase* MakeEmptyCollection(const QString& name, size_t cid) const override;
    virtual bool ReadRestFromFile(jfFileReader* infile) override;
    // typed search item
    jfFFNSearch* typed_search;
    // other data
    const jfFFN_Categories* cat_links;
};
//*******************************************************************************

