/******************************************************************************
Name    :   global_skel.h
Author  :   John Q Metro
Purpose :   A global skeleton editor
Created :   December 1, 2009
Conversion to Qt started : September 15, 2013
Updated :   September 19, 2013
******************************************************************************/
#ifndef SKELEDIT_H_INCLUDED
  #include "skeledit.h"
#endif
#ifndef SKELSTORE_H_INCLUDED
  #include "../../core/objs/skelstore.h"
#endif // SKELSTORE_H_INCLUDED
#ifndef SKELCOLL_H_INCLUDED
  #include "../../core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
//------------------------------------------------------------------
#ifndef GLOBAL_SKEL_H_INCLUDED
#define GLOBAL_SKEL_H_INCLUDED
#endif // GLOBAL_SKEL_H_INCLUDED
//--------------------------------------------
#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
/*****************************************************************************/
class jfGlobalSkeletonEdit : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfGlobalSkeletonEdit(jfSkelCollection* icoll_link,QWidget* parent = NULL);
  public slots:
    void ListSelect();
  protected:
    // helper methods
    bool ToSingle(jfSkeletonSingle* inval);
    bool ToMultiple(jfSkeletonMultiple* inval);
    bool ToFicPart(jfSkeletonFicPart* inval);
    // more helper methods
    bool ChangePanel(jfSkelEditCore* inskel);
    // gui elements
    QListWidget* selector;
    jfSkelEditSingle* single;
    jfSkelEditMultiple* multiple;
    jfSkelEditFicPart* ficpart;
    // local values
    jfs_Skeltype stype;
    jfSkelCollection* coll_link;
    size_t i_index;
    // sizers
    QHBoxLayout* topsizer;

};

/*****************************************************************************/
