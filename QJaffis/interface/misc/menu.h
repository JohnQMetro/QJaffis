/*****************************************************************************
Name:        menu.h
Author :     John Q Metro
Purpose :    Declares custom popup menu classes
Created:     January 5, 2010
Conversion to Qt started February 22, 2014
Updated:     March 25, 2023
******************************************************************************/
#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED
#endif // MENU_H_INCLUDED
//------------------
#include <QMenu>
//---------------------

#include "../../core/objs/baseitem.h"
/*****************************************************************************/
// a forward declaration
class jfMainSearchGroup;
typedef struct jfRClickData jfRClickData;
//--------------------------------------------------------
class jfPopupMenu : public QMenu {
    Q_OBJECT
  public:
    // constructor
    jfPopupMenu();
    ~jfPopupMenu();
    // setting data
    void SetPopupData(jfRClickData indata);
  signals:
    void sendDescription(QString outdesc);
  public slots:
    void OnLoadURL();
    void OnAppendToExpressionFilter();
  protected:
    virtual void EnableDisable() = 0;
    bool xNN() const;
    jfResultUnit* GetSource() const;
    // internal data
    jfRClickData* sdata;
    // action pointers
    QAction* load_action;
    QAction* append_action;

};

//------------------------------------------------------------------
// base class for Fanfic and Fim Group menus
class jfItemPopupMenuBase : public jfPopupMenu {
    Q_OBJECT
  public:
    jfItemPopupMenuBase();
  public slots:
    void OnToggleIgnore();
  protected:
    virtual void EnableDisable();
    virtual void EnableDisableMore() = 0;
    QAction* ignore_toggle;
};

//-------------------------------------------------------------------
class jfFanficPopupMenu : public jfItemPopupMenuBase {
    Q_OBJECT
  public:
    jfFanficPopupMenu();
  public slots:
    void OnDownloadFic();
    virtual void EnableDisableMore();
  protected:
    QAction* down_fanfic;
};

//------------------------------------------------------------------
class jfFIMGPopupMenu : public jfItemPopupMenuBase {
    Q_OBJECT
  public:
    jfFIMGPopupMenu();
  public slots:
    virtual void EnableDisableMore();
  protected:

};
/*****************************************************************************/
