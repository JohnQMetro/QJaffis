/******************************************************************************
Name    :   drange_edit.h
Author  :   John Q Metro
Purpose :   Declares some classes for editing date range filters
Created :   December 8, 2011
Conversion to Qt started October 13, 2013
Updated :   October 13, 2013 (moved jfDateRangePanel)
******************************************************************************/
#ifndef DRANGE_EDIT_H_INCLUDED
#define DRANGE_EDIT_H_INCLUDED
#endif // DRANGE_EDIT_H_INCLUDED
//--------------------------------------------------------------------------
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED

#ifndef DATERANGE_FILTER_H
  #include "../../../core/filters/extended/daterange.h"
#endif // DATERANGE_FILTER_H

//-------------------------------------------------
#include <QDateEdit>
/******************************************************************************/
// date range editing panel
class jfDateRangePanel : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfDateRangePanel(QWidget* parent = NULL);
    // setting and getting values
    bool CheckOk() const;
    bool StoreValue(jfDateRangeFilter* saveto) const;
    bool LoadValue(const jfDateRangeFilter* getfrom);
    void MakeUndefined();
    void ShowErrorMsg() const;
  public slots:
    void EvUndefinedFrom(int newstate);
    void EvUndefinedTo(int newstate);
  protected:
    // private methods
    bool MakeBlock(bool from);
    // gui elements
    QLabel* from_label, *to_label;
    QDateEdit* newdate;
    QDateEdit* oldate;
    QCheckBox* undefined_new;
    QCheckBox* undefined_old;
    // sizers
    QGridLayout* topsizer;
    QHBoxLayout *fsizer, *tsizer;

};
//***************************************************************************
// filter editor wrap for jfDateRangePanel, updated Version
class jfUpdatedDateFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfUpdatedDateFilterEditor(const jfUpdatedDateFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfDateRangePanel* insert_panel;
};
//============================================================
// filter editor wrap for jfDateRangePanel, Publishe Version
class jfPublishedDateFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfPublishedDateFilterEditor(const jfPublishedDateFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfDateRangePanel* insert_panel;
};
/******************************************************************************/
