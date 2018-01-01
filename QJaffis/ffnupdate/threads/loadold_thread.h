/******************************************************************************
* Name    :  res_bundle.h
* Author  :  John Q Metro
* Purpose :  Declares a class that holds all the data related to the update
* Created :  January 20, 2011
* Updated :  August 15, 2016
******************************************************************************/
#ifndef LOADOLD_THREAD_H
  #define LOADOLD_THREAD_H
#endif // LOADOLD_THREAD_H
//-------------------------------------
#ifndef THREADBASE_H
  #include "../../fetching/threadbase.h"
#endif // THREADBASE_H
#ifndef RES_BUNDLE_H_INCLUDED
  #include "../data/res_bundle.h"
#endif // RES_BUNDLE_H_INCLUDED
#ifndef FFN_MULTICOLL
  #include "../../fanficnet/data/ffn_mulcoll.h"
#endif
//------------------------------------
#include <QWaitCondition>
#include <QMutex>
/*****************************************************************************/
// skeleton thread class for loading old data and merging old and new data
class jfCategoryBulkThread : public jfThreadBase {
    Q_OBJECT
  public:
    jfCategoryBulkThread();
    bool SetData(jfFFN_UpdateBundle* in_source_data);
    // wake
    void WakeAfter();
  public slots:
    virtual void StartProcessing() = 0;
  protected:
    // virtual methods to implement
      virtual QString CategoryAction() const = 0;
      virtual bool HandleCategory() = 0;
    // helper methods
      void WaitAfterSend();
    // data
      jfFFN_UpdateBundle* source_data;
      jfItemInfoMsg cat_info;
      jfFFNResColl* indexed_collection;
    // wait!
      QMutex wlock;
      QWaitCondition syncher;
};


/*****************************************************************************/
