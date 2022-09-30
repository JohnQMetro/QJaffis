/******************************************************************************
Name    :   display_parts.h
Author  :   John Q Metro
Purpose :   Declares core parts and base classes for the search/filter interface
Created :   August 23, 2009 (originally from display_base.h)
Updated :   July 16, 2022
******************************************************************************/
#ifndef DISPLAY_PARTS_H
#define DISPLAY_PARTS_H
#endif // DISPLAY_PARTS_H
//-------------------------------
#ifndef SEARCHCORE_H_INCLUDED
  #include "../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef EXPR_EDITOR_H_INCLUDED
  #include "../filters/base/expr_editor.h"
#endif // EXPR_EDITOR_H_INCLUDED
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef RESULTCOLLECT_H_INCLUDED
  #include "../../core/structs/resultcollect.h"
#endif // RESULTCOLLECT_H_INCLUDED

//-------------------------------
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
//****************************************************************************
const QString defexprname = "Default Expression Filter";
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* because default filters can be complicated, I've decided to put them on a
separate panel */
class jfDefaultFilterEditorBase : public QWidget {
  public:
    // the constructor
    jfDefaultFilterEditorBase(jfSearchCore* inobj, size_t proph, size_t propv, QWidget* parent = NULL);
    // data i/o
    bool SaveFilters();
    bool ChangeSearch(jfSearchCore* obj_data);
  protected:
    // internal abstract methods
    virtual bool SaveFiltersExtended() = 0;
    virtual bool ChangeSearchExtended(jfSearchCore* obj_data) = 0;
    // internal links
    jfFilterMap* embedded_filters;
    jfSearchCore* mainobj;
    // gui elements
    QLabel*       mainlabel;
    jfExpressionEditor* main_expedit;
    QVBoxLayout*         exp_sizer;
    QHBoxLayout*         top_sizer;
    QVBoxLayout*         main_sizer;
};
//===========================================================================
// abstract class for non-filter search options
class jfSearchOptionsBase : public QWidget {
  public:
    // constructor
    jfSearchOptionsBase(QWidget* parent = NULL);
    // methods
    virtual bool LoadFrom(jfSearchCore* insearch) = 0;
    virtual bool StoreTo(jfSearchCore* outsearch) = 0;
  protected:
};

//============================================================================
// an even more base class for the custom first panel, to allow more freedom
class jfSearchPanelRoot : public QWidget {
  public:
    jfSearchPanelRoot(jfSearchCore* obj_data, QWidget* parent = NULL);
    virtual bool LoadToObj() =0;
    virtual bool ChangeObj(jfSearchCore* obj_data) =0;
    void ReloadObj();
  protected:
    // internal links
    jfSearchCore* mainobj;

};
// ===========================================================================
// (no longer) base class for the custom first panel
class jfSearchPanelBase : public jfSearchPanelRoot {
  public:
    jfSearchPanelBase(jfSearchCore* obj_data,bool noteb, bool svert, size_t prop, QWidget* parent = NULL);
    virtual bool LoadToObj() =0;
    virtual bool ChangeObj(jfSearchCore* obj_data) =0;
  protected:
    // internal methods
    bool ArrangePanels();
    virtual jfDefaultFilterEditorBase* MakeTypedFilEdit() = 0;
    // gui elements
    jfDefaultFilterEditorBase* filt_panel;
    jfSearchOptionsBase* SearchPanel;
    QTabWidget* nb;
    bool isnotebook;
    size_t propstore;
    // sizers
    QGroupBox *param_box, *defil_box;
    QVBoxLayout* param_sizer;
    QVBoxLayout* defil_sizer;
    QBoxLayout* topsizer;
};
//==========================================================================
/* Top level, we have either a panel with the Search, or a panel with Search,
 *Custom Filters, and Result Categories. This class serves a base for either one. */
class jfSearchGroupingCore : public QWidget {
  public:
    // the constructor
    jfSearchGroupingCore(QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelRoot*  MakeFirstPanel() = 0;
    virtual jfSearchCore*       MakeTypedSearch() const = 0;
    virtual jfResultCollection* MakeTypedCollection() const = 0;
    virtual jfDownloadRootItems* MakeTypedThread() const = 0;
    // returning data
    jfSearchCore* GetSearchObj();
    virtual bool UpdateSearchData() = 0;
    // special methods
    void ReloadPanel1();
  protected:
    virtual void FinishConstruction() = 0;
    // the main panels
    jfSearchPanelRoot* search_and_filter_panel;
    // the sizer
    QBoxLayout* top_sizer;
    // internal data
    jfSearchCore* searchdata;
};
//****************************************************************************
