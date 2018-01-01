/******************************************************************************
Name    :   bfilt_edit.h
Author  :   John Q Metro
Purpose :   Declares some base class for filter diting
Created :   July 27, 2009
Conversion to Qt Started Oct 3, 2013
Updated :   August 5, 2015
******************************************************************************/
#ifndef BFILT_EDIT_H_INCLUDED
#define BFILT_EDIT_H_INCLUDED
#endif // BFILT_EDIT_H_INCLUDED
//---------------------------------------------------------------------------
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef WIDGETS1_H_INCLUDED
  #include "../../misc/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
#ifndef EXPR_EDITOR_H_INCLUDED
  #include "expr_editor.h"
#endif // EXPR_EDITOR_H_INCLUDED
#ifndef MINMAX_FILTER_H
  #include "../../../core/filters/extended/minmax.h"
#endif // MINMAX_FILTER_H
#ifndef WIDGETS3_H_INCLUDED
  #include "../../misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef URLFILTER_EDITOR_H
  #include "urlfilter_editor.h"
#endif // URLFILTER_EDITOR_H
//-----------------------------------------
#include <QGroupBox>
#include <QVBoxLayout>
#include <QBoxLayout>
/*****************************************************************************/
/*
-The base filter editor has a name editor and a description editor.
-The editor should not directly modify the filter, but rather should use the
data to load the widgets with info.
-The editor returns a new filter using the data. it us up to the calling
method to insert the filter or replace the old one in the map.
-The editor will use the map to check for naming conflicts
-There is a buttton for re-loading the filter data from the object into the
widgets.

*/
class jfBaseFilterEditor : public QWidget {
    Q_OBJECT
  public:
    jfBaseFilterEditor(const jfFilterMap* infmap, const jfBaseFilter* infilt, QWidget* parent = NULL);
    // external filter accessing methods
    virtual void LoadFilter(const jfBaseFilter* infilter) = 0;
    bool ChangeFilter(const jfBaseFilter* infilter);
    void ReloadFilter();
    virtual jfBaseFilter* GetFilter() =0;
    // checks and external methods
    bool NameCheck() const;
    virtual bool GeneralCheck() const = 0;
    void ChangeBoxName(QString newname);
  public slots:
    void ReloadBtnPressed(bool click);
  protected:
    // internal helper methods
    void ArrangeSizers();
    bool NameNUniq() const;
    void NameLoad();
    // internal GUI elements
    jfNameDescEditor* namedesc_edit;
    QPushButton* reload_btn;
    QGroupBox* mbox;
    QVBoxLayout* topsizer;
    QHBoxLayout* realtopsizer;
    QBoxLayout* insert;
    // data pointers
    const jfFilterMap* local_fmap;
    const jfBaseFilter* filt_pointer;
};
/*****************************************************************************/
/* a wrapper for jfExpressionEditor that is an editor for an expression
filter */
class jfExprFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfExprFilterEditor(const jfFilterMap* infmap, bool is_inglobal, const jfBaseFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  private:
    jfExpressionEditor* insert_panel;
    bool is_global;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* a wrapper for jfUrlFilEdit that is an editor for an url filter */
class jfUrlFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfUrlFilterEditor(const jfFilterMap* infmap, const jfBaseFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  private:
    jfUrlFilEdit* insert_panel;
};
/*****************************************************************************/
