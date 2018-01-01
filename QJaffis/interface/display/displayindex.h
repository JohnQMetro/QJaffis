/*****************************************************************************
Name:        displayindex.h
Author :     John Q Metro
Purpose :    Organizing the indexes of results for the viewing listboxes
Created:     September 1, 2013
Updated:     April 30, 2014
******************************************************************************/
#ifndef DISPLAYINDEX_H
#define DISPLAYINDEX_H
#endif // DISPLAYINDEX_H
//---------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
#ifndef SEARCHCORE_H_INCLUDED
  #include "../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
//---------------------------------
#include <vector>
#include <QMutex>
//*****************************************************************************
/* displaying headers in the result list : we only use he_ONECATEGORY and
 * he_NONEMPTY right now */
enum header_enum {he_ONECATEGORY,he_NOHEADER,he_NONEMPTY,he_ALWAYS};
//----------------------------------------
/* This class includes result data and indexes for one source category only */
class jfResultIndexes_Category {
  public:
    // constructor
    jfResultIndexes_Category(header_enum inhead, bool in_usedefault, size_t in_numrcats);
    // setting data
    bool InitializeFromEndpoints(std::vector<size_t>* inpoints);
    void SetCatData(struct jfCategoryInfoMsg in_catdata);
    size_t AddResults(jfResultUnitVector* inresults);
    size_t AddToTotal(size_t increment);
    // getting data
    std::vector<size_t>* GetEndpoints() const;
    size_t GetNumres(size_t index) const;
    QString GetCatname() const;
    QString GetCatHtml() const;
    QString GetResCatHeader(size_t rcidx) const;
    // destructor
    ~jfResultIndexes_Category();
  protected:
    // making the count/percentage line
    QString MakeCountLine(size_t rcidx) const;
    // internal data
    header_enum hval;
    bool use_default;     // are we using the default result catgeory?
    size_t item_count;
    size_t rcat_count;     // number of non-default result categories.
    size_t cat_offset[33];  // offset indexes in the result html list box
    size_t num_results[33]; // number of items per result category
    jfResultUnitVector results; // the actual result items
    struct jfCategoryInfoMsg catdata;
};
//============================================================================
class jfDisplayIndexGroup {
  public:
    // constructor
    jfDisplayIndexGroup(header_enum inhead, bool in_usedefault, size_t in_numrcats);
    // setting data
    void NewCategory(struct jfCategoryInfoMsg in_catdata);
    size_t AddResults(jfResultUnitVector* inresults);
    size_t AddToTotal(size_t increment);
    // getting data
    bool NoCurrentResults(size_t rcat_index) const;
    QString GetCurrentCatHtml(size_t rcidx) const;
    // destructor
    ~jfDisplayIndexGroup();
  protected:
    // basic result category info
    header_enum hval;
    bool use_default;     // are we using the default result catgeory?
    size_t num_rcats;     // number of non-default result categories.
    // the data
    std::vector<jfResultIndexes_Category*> catresults;
    // indexes
    size_t catdex;
    mutable QMutex mlock;
};


//*****************************************************************************
