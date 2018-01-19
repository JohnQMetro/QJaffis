/*****************************************************************************
Name    :   searchcore.h
Author  :   John Q Metro
Purpose :   Declares a base class containing search info and parameters
Created :   May 4, 2009
Conversion to Qt started November 5, 2013
Updated :   November 9, 2013
******************************************************************************/
#ifndef SEARCHCORE_H_INCLUDED
#define SEARCHCORE_H_INCLUDED
#endif // SEARCHCORE_H_INCLUDED
//----------------------------------------------------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef BASECOLLECT_H_INCLUDED
  #include "../structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef CATEGORIES_H_INCLUDED
  #include "../structs/categories.h"
#endif // CATEGORIES_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../structs/vectors.h"
#endif // VECTORS_H_INCLUDED
#ifndef SKELSTORE_H_INCLUDED
  #include "skelstore.h"
#endif // SKELSTORE_H_INCLUDED
//---------------------------------
#include <QMetaType>
//***************************************************************************
// initial category info
struct jfCategoryInfoMsg {
    QString catname;
    QString catheader;
};
Q_DECLARE_METATYPE(jfCategoryInfoMsg);
//=========================================================================
class jfSearchCore : public QObject, public jfBaseObj {
    Q_OBJECT
  public:
    jfFilterMap* def_filtermap;
    jfFilterMap* local_map;
    jfCategories* categories;
    // the constructor, just makes sure links are NULL
    jfSearchCore();
    // checks the startup set for validity
    bool Check() const;
    // checks if the filters are empty
    bool NoFilters() const;
    // makes a set of empty maps and categories
    void MakeEmpty();
    size_t GetPagelimit() const;
    // copy load (called just before search)
    void CopyLoad();
    bool VerifyNames(QString&outmsg);
    void PairFilterLogging(bool write);
    // creates and dispatches via event a Result Organizer
    virtual void DispatchCategory() = 0;
    // dispatches the event containing result data (to the organizer)
    void DispatchResults(size_t stotal, jfResultUnitVector* inresult);
    // sending a count, triggered externally
    void SendNewcount(size_t icount);
    /* if we do not use the default (leftover) result category, we return
    TRUE if the paramster has no category marks */
    bool NoCatTest(const jfBasePD* testee) const;
    // used to create/overwrite the local customer html skeleton
    virtual void MakeLocalSkeleton(jfSkeletonBase* src) = 0;
    void ClearLocalSkeleton();
    jfSkeletonBase* GetLocalSkeleton();
    virtual void LoadValues(jfSkeletonParser* inparser, size_t result_categ) = 0;
    // category header data
    virtual QString GetCatName() const = 0;
    virtual QString GetCatHeader() const = 0;
    virtual QString GetIndexUrl(size_t inp_index) const = 0;
    virtual QString GetHTMLHeader(size_t result_category) const = 0;
  signals:
    void SendResults(jfResultUnitVector* inresult);
    void DispatchCount(size_t icount);
    void SendCatInfo(struct jfCategoryInfoMsg outcat);
  protected:
    // internal helper methods
    void LoadCoreValues(jfSkeletonParser* inparser, size_t result_categ) const;
    //  implemented virtual i/o method
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    size_t pagelimit;
    jfSkeletonBase* skel_link;
    bool local_skel;
    size_t catindex;
};
/*****************************************************************************/
