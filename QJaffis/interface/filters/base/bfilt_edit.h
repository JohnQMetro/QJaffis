/******************************************************************************
Name    :   bfilt_edit.h
Author  :   John Q Metro
Purpose :   Declares some base class for filter diting
Created :   July 27, 2009
Conversion to Qt Started Oct 3, 2013
Updated :   April 8, 2023 (changing base filter type, removing url filter)
******************************************************************************/
#ifndef BFILT_EDIT_H_INCLUDED
#define BFILT_EDIT_H_INCLUDED
#endif // BFILT_EDIT_H_INCLUDED
//---------------------------------------------------------------------------

#include "../../../core/filters/base/basefilter.h"

#ifndef BASECOLLECT_H_INCLUDED
  #include "../../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef WIDGETS1_H_INCLUDED
  #include "../../controls/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
#ifndef EXPR_EDITOR_H_INCLUDED
  #include "expr_editor.h"
#endif // EXPR_EDITOR_H_INCLUDED
#ifndef MINMAX_FILTER_H
  #include "../../../core/filters/extended/minmax.h"
#endif // MINMAX_FILTER_H
#ifndef WIDGETS3_H_INCLUDED
  #include "../../controls/widgets3.h"
#endif // WIDGETS3_H_INCLUDED

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
    jfBaseFilterEditor(const jfFilterBase* infilt, QWidget* parent = NULL);
    // external filter accessing methods
    virtual void LoadFilter(const jfFilterBase* infilter) = 0;
    bool ChangeFilter(const jfFilterBase* infilter);
    void ReloadFilter();
    virtual jfFilterBase* GetFilter() =0;
    // checks and external methods
    bool NameCheck(const jfFilterMap* filter_group) const;
    virtual bool GeneralCheck(const jfFilterMap* filter_group) const = 0;
    void ChangeBoxName(QString newname);
  public slots:
    void ReloadBtnPressed(bool click);
  protected:
    // internal helper methods
    void ArrangeSizers();
    bool NameNUniq(const jfFilterMap* filter_group) const;
    void NameLoad();
    // internal GUI elements
    jfNameDescEditor* namedesc_edit;
    QPushButton* reload_btn;
    QGroupBox* mbox;
    QVBoxLayout* topsizer;
    QHBoxLayout* realtopsizer;
    QBoxLayout* insert;
    // data pointers
    const jfFilterBase* filt_pointer;
};
/*****************************************************************************/
/* a wrapper for jfExpressionEditor that is an editor for an expression
filter */
class jfExprFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfExprFilterEditor(const jfFilterMap* infmap, bool is_inglobal, const jfExpressionFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    // typed methods
    void LoadExpressionFilter(const jfExpressionFilter* infilter);
    jfExpressionFilter* GetExpressionFilter() const;
    // checking
    bool GeneralCheck(const jfFilterMap* filter_group) const override;
  private:
    jfExpressionEditor* insert_panel;
    bool is_global;
    const jfFilterMap* local_fmap;
};
/*****************************************************************************/
