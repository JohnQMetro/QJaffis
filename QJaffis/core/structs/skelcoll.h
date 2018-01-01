/*****************************************************************************
Name:        skelcoll.h
Author :     John Q Metro
Purpose :    Grouping multiple HTML skeltons together
Created:     November 21, 2009
Conversion to QT Started April 14, 2013
Updated:     August 21, 2012 (adding jfFileReader)
******************************************************************************/
#ifndef SKELCOLL_H_INCLUDED
#define SKELCOLL_H_INCLUDED
#endif // SKELCOLL_H_INCLUDED
#ifndef SKELSTORE_H_INCLUDED
  #include "../objs/skelstore.h"
#endif // SKELSTORE_H_INCLUDED
#ifndef BASECOLLECT_H_INCLUDED
  #include "basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#include <vector>
/*****************************************************************************/
const QString skelg_fname = "htmlskel.dat";

//----------------------------------------------------------------
class jfSkelVector : public std::vector<jfSkeletonCore*> {};
//=============================================================================
class jfSkelCollection : public jfBaseCollection {
  public:
    // default constructors
    jfSkelCollection();
    // implemented index methods
    virtual bool NextIndex();
    virtual bool BackIndex();
    virtual void ResetIndex();
    virtual bool ToIndex(const int& newindex);
    // some more former abstract methods
    virtual QStringList* GetNameList() const;
    virtual QStringList* GetDescriptionList() const;
    virtual QStringList* GetTypeList() const;
    virtual bool DeleteAtIndex(const size_t& dindex);
    virtual QString GetTypeID() const;
    // additional methods
    jfSkeletonSingle*   AtIndexS(size_t index);
    jfSkeletonMultiple* AtIndexM(size_t index);
    jfSkeletonFicPart* AtIndexF(size_t index);
    jfs_Skeltype GetSkelType(size_t index) const;
    bool IsMultiple(size_t index) const;
    void AppendBlankSkeleton(jfs_Skeltype intype,QString name);
    // the destructor
    ~jfSkelCollection();
  protected:
    // helper methods
    void ClearDatalist();
    // file I/O
    virtual bool AddMid2ToFile(QTextStream* outfile) const ;
    virtual bool ReadMid2FromFile(jfFileReader* infile);
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // private internal data
    jfSkelVector* datalist;

};
//============================================================================
extern jfSkelCollection* mainskelc;
//------------
bool LoadSkeletons();
void WriteSkeletons();
/*****************************************************************************/
