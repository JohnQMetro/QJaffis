/******************************************************************************
* Name    :  res_bundle.h
* Author  :  John Q Metro
* Purpose :  Declares a class that holds all the data related to the update
* Created :  January 20, 2011
* Updated :  August 15, 2016
******************************************************************************/
#ifndef RES_BUNDLE_H_INCLUDED
#define RES_BUNDLE_H_INCLUDED
#endif // RES_BUNDLE_H_INCLUDED
//-------------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef JF_FFNSEARCH
  #include "../../fanficnet/data/ffn_search.h"
#endif
#ifndef FFN_MULTICOLL
  #include "../../fanficnet/data/ffn_mulcoll.h"
#endif
#ifndef CATGROUP_H
#include "../../fanficnet/categories/catgroup.h"
#endif // CATGROUP_H
#ifndef AUTH_COLL_H_INCLUDED
  #include "auth_coll.h"
#endif // AUTH_COLL_H_INCLUDED
//--------------------------------------------------
#include <QMutex>
#include <QDateTime>
/*****************************************************************************/
/* when we save results for future updating, we package the search object and the results
here. We also have some objects for storing update result data and some special methods */
class jfFFN_UpdateBundle : public jfBaseObj {
  public:
    // public data
    size_t sindex;
    jfFFNSearch* search_data;
    jfFFNResColl* result_data;
    QDateTime lastdate;
    // temporary info
    //jfFFN_NewResCollection* new_data;
    jfFFNResColl* new_data;
    jfAuthorCollection* auth_data;
    jfFFN_Categories* chlink;
    // constructors
    jfFFN_UpdateBundle();
    // special meta-information
    virtual QString GetTypeID() const;
    // information about the result
    size_t ResultCatCount() const;
    size_t GetIDLimit(size_t limindex) const;
    bool SameCategory(size_t index, const jfFFN_CategoryCore* testee);
    // special information
    const jfFFN_CategoryCore* GetOldCategory() const;
    // update status flags
    bool AuthDone();
    bool NewDone();
    bool CheckSourceNNull() const;
    // destructors
    ~jfFFN_UpdateBundle();
  protected:
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    // file i/o input
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal values
    mutable QMutex upd_task_lock;
    bool auth_check;
    bool new_fics;
};
/*****************************************************************************/
