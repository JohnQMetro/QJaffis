/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_itemcoll.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net item object
// Created:     June 1, 2010
// Conversion to Qt Started July 11, 2014
// Updated:     August 15, 2016
/////////////////////////////////////////////////////////////////////////////
#define FFN_ITEMCOLL

#ifndef TYPEDCOLLECT_H_INCLUDED
  #include "../../core/structs/typedcollect.h"
#endif // TYPEDCOLLECT_H_INCLUDED
#ifndef JFFNFICOBJ
  #include "ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef JFFFNSCATCORE
  #include "../categories/ffn_catcore.h"
#endif // JFFFNSCATCORE
//***************************************************************************
// forward declarations
class jfFFNSearch;
//==========================================================================
class jfFFNItemCollection : public jfTypedCollection<jfFFNItem> {
  public:
    size_t first_id;
    // the constructors
    jfFFNItemCollection();
    jfFFNItemCollection(const jfFFN_CategoryCore* cat_linkin);
    jfFFNItemCollection(jfFFNItemCollection* insrc, bool copy);
    // implemented  methods
    virtual QString GetTypeID() const;
    virtual bool LoadValues(jfSkeletonParser* inparser,size_t which) const;
    // category information methods
    QString GetCatUrl() const;
    QString GetFinder() const;
    const jfFFN_CategoryCore* GetCat() const;
    QString GetCatName() const;
    bool IsSameCat(const jfFFN_CategoryCore* test_cat);
    // category manipulation
    bool ReplaceCat(const jfFFN_CategoryCore* cat_linkin);
    // getting more information
    jfFicExtract* FicExt_AtIndex(size_t i_index) const;
    // used for updates
    bool InsertNewResults(jfFFNItemCollection* insrc);
    jfResultUnitVector* GetContentsAsResults();
    // destructor
    ~jfFFNItemCollection();
  protected:
    // some more virtual i/o methods
    virtual bool AddDelta(QTextStream* outfile) const;
    virtual bool ReadDelta(jfFileReader* infile);
    // external data
    const jfFFN_CategoryCore* cat_link;
    jfFFNSearch* sdata_typed;
    // locating the category
    QString caturl;
    QString cfinder;
    // update helper data
    // size_t recordtotal;
};

//***************************************************************************

