/******************************************************************************
Name    :   ao3_coll.h
Author  :   John Q Metro
Purpose :   Declares category results collection for AO3
Created :   September 1, 2012
Conversion to Qt Started March 29, 2014
Updated :   July 17, 2016
******************************************************************************/
#ifndef AO3_COLL_H_INCLUDED
#define AO3_COLL_H_INCLUDED
#endif // AO3_COLL_H_INCLUDED
//-----------------------------------
#ifndef AO3_SEARCH_H_INCLUDED
  #include "ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED
#ifndef TYPEDCOLLECT_H_INCLUDED
  #include "../../../core/structs/typedcollect.h"
#endif // TYPEDCOLLECT_H_INCLUDED
#ifndef RESULT_MULTI_H
  #include "../../../core/structs/result_multi.h"
#endif // RESULT_MULTI_H
/*****************************************************************************/
class jfAO3_FicList : public jfTypedCollection<jfAO3Fanfic> {
  public:
    // the constructors
    jfAO3_FicList();
    jfAO3_FicList(size_t src_id, const QString& nname);
    jfAO3_FicList(const jfAO3_Category* cat_linkin);
    // implemented page methods
    virtual QString GetTypeID() const;
    virtual bool LoadValues(jfSkeletonParser* inparser,size_t which) const;
    // category information methods
    QString GetCatUrl() const;
    QString GetFinder() const;
    const jfAO3_Category* GetCat() const;
    QString GetCatName() const;
    // category manipulation
    bool ReplaceCat(const jfAO3_Category* cat_linkin);
    // getting more information
    jfFicExtract* FicExt_AtIndex(size_t i_index) const;
  protected:
    // some more virtual i/o methods
    virtual bool AddDelta(QTextStream* outfile) const;
    virtual bool ReadDelta(jfFileReader* infile);
    // external data
    const jfAO3_Category* cat_link;
    jfAO3Search* sdata_typed;
    // locating the category
    QString caturl;
    QString cfinder;
};
//=====================================================================================
// the multiple category results class
class jfAO3ResColl : public jfResultMultiCollection {
  public:
    // the constructor
    jfAO3ResColl(jfAO3Search* xsearch_in);
    bool SetCatLinks(const jfGen_CatHolder* inlinks);
    // other stuff
    virtual QString GetTypeID() const;
    const jfAO3_Category* GetCat(size_t zindex) const;
    // i/o
    bool WriteToHTML(size_t result_category);
    // extracting information from a particular item
    jfFicExtract* GetFicExtract(size_t c_index, size_t i_index) const;
  protected:
    void WriteHtmlHeader(jfHtmlParams* indata);
    // i/o methods
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual jfUrlItemCollection* MakeEmptyCollection() const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // typed search item
    jfAO3Search* typed_search;
    // other data
    const jfGen_CatHolder* cat_links;
};
/*****************************************************************************/
