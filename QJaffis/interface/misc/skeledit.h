/******************************************************************************
Name    :   skeledit.h
Author  :   John Q Metro
Purpose :   Skeleton editing panels
Created :   November 25, 2009
Conversion to Qt started : September 15, 2013
Updated :   September 18, 2013
******************************************************************************/
#ifndef SKELEDIT_H_INCLUDED
#define SKELEDIT_H_INCLUDED
#endif
//-----------------------------------------------------------------
#ifndef WIDGETS1_H_INCLUDED
  #include "../controls/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
#ifndef WIDGETS2_H_INCLUDED
  #include "../controls/widgets2.h"
#endif // WIDGETS2_H_INCLUDED
#ifndef SKELSTORE_H_INCLUDED
  #include "../../core/objs/skelstore.h"
#endif // SKELSTORE_H_INCLUDED
//------------------------------------------------------------------
#include <QWidget>

/*****************************************************************************/
class jfSkelEditCore : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfSkelEditCore(QWidget* parent = NULL);
    // virtual methods
    virtual bool ChangeSkeleton(jfSkeletonCore* source) = 0;
    virtual bool LoadSkeleton() = 0;
    virtual bool SaveSkeleton() = 0;
    // additional methods
    jfSkeletonCore* GetSkeleton();
    jfs_Skeltype GetSkelType() const;
  public slots:
    // button press event handlers
    virtual void SaveButton() = 0;
    virtual void RestoreButton() = 0;
  protected:
    // internal helper methods
    bool CLoadItems(jfSkeletonCore* source);
    bool CStoreItems(jfSkeletonCore* destination) const;
    // gui elements
    jfNameDescEditor* ndesce;
    QPushButton* save_btn;
    QPushButton* rest_btn;
    QPlainTextEdit* help_desc;
    QTabWidget* panelstore;
    jfTwoTexts* thepanels[5] ;
    // sizers
    QVBoxLayout* topsizer;
    QHBoxLayout* sizesizer;
    // internaldata
    jfSkeletonCore* item;

};
//============================================================================
class jfSkelEditBase : public jfSkelEditCore {
  public:
    // the constructor
    jfSkelEditBase(QWidget* parent = NULL);
  protected:
    // internal helper methods
    bool CLoadExtra(jfSkeletonBase* source);
    bool CStoreExtra(jfSkeletonBase* destination) const;
};
//=============================================================================
class jfSkelEditSingle : public jfSkelEditBase {
    Q_OBJECT
  public:
    // the constructor
    jfSkelEditSingle(jfSkeletonSingle* indata, QWidget* parent = NULL);
    // virtual methods
    virtual bool ChangeSkeleton(jfSkeletonCore* source);
    virtual bool LoadSkeleton();
    virtual bool SaveSkeleton();
  public slots:
    // button press event handlers
    virtual void SaveButton();
    virtual void RestoreButton();
  protected:
    // internaldata
    jfSkeletonSingle* typ_item;
};
//=============================================================================
class jfSkelEditMultiple : public jfSkelEditBase {
    Q_OBJECT
  public:
    // the constructor
    jfSkelEditMultiple(jfSkeletonMultiple* indata, QWidget* parent = NULL);
    // virtual methods
    virtual bool ChangeSkeleton(jfSkeletonCore* source);
    virtual bool LoadSkeleton();
    virtual bool SaveSkeleton();
  public slots:
    // button press event handlers
    virtual void SaveButton();
    virtual void RestoreButton();
  protected:
    // internaldata
    jfSkeletonMultiple* typ_item;
};
//=============================================================================
class jfSkelEditFicPart : public jfSkelEditCore {
    Q_OBJECT
  public:
    // the constructor
    jfSkelEditFicPart(jfSkeletonFicPart* indata, QWidget* parent = NULL);
    // virtual methods
    virtual bool ChangeSkeleton(jfSkeletonCore* source);
    virtual bool LoadSkeleton();
    virtual bool SaveSkeleton();
  public slots:
    // button press event handlers
    virtual void SaveButton();
    virtual void RestoreButton();
  protected:
    // internaldata
    jfSkeletonFicPart* typ_item;
};
/*****************************************************************************/
