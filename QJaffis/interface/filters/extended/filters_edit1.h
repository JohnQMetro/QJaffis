/******************************************************************************
Name    :   filters_edit1.h
Author  :   John Q Metro
Purpose :   Editors for more filters : tag and completed
Created :   December 6, 2010
Conversion to Qt started October 13, 2013
Updated :   April 15, 2023
******************************************************************************/
#ifndef FILTERS_EDIT1_H_INCLUDED
#define FILTERS_EDIT1_H_INCLUDED
#endif // FILTERS_EDIT1_H_INCLUDED
//--------------------------------------------
#ifndef WIDGETS3_H_INCLUDED
  #include "../../controls/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED

#ifndef MOREFILTERS1_H_INCLUDED
  #include "../../../core/filters/extended/morefilters1.h"
#endif
#ifndef TAG_WIDGETS_H
  #include "../../../interface/controls/tag_widgets.h"
#endif // TAG_WIDGETS_H
#ifndef TAG_WIDGETS2_H
  #include "../../../interface/controls/tag_widgets2.h"
#endif // TAG_WIDGETS2_H
//--------------------------------------------
//******************************************************************************/
// a generic tag filter editor
class jfTagFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfTagFilterEditor(const jfTagFilterCore* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    // extra methods
    virtual jfTagFilterCore* GetTagFilter() = 0;
    virtual bool isListLong() const = 0;
    // checking
    virtual bool GeneralCheck(const jfFilterMap* infmap) const override;
  protected:
    void CompleteConstruction(QString name, const jfTagFilterCore* infilt);
    virtual void LoadBlank() = 0;
    jfTagStatusPicker* shortlist_panel;
    jfTagSorter* longlist_panel;
};

//==========================================================================

// completed filter editor (single checkbox version)
class jfCFE_Core1 : public QWidget {
  public:
    // constructor
    jfCFE_Core1(const jfCompletedFilter* inmap, QWidget* parent = NULL);
    // setting and getting values
    bool StoreValue(jfCompletedFilter* saveto) const;
    bool LoadValue(const jfCompletedFilter* getfrom);
    jfCompletedFilter* GetValue() const;
  protected:
    // gui elements
    QCheckBox* mainval1;
    QHBoxLayout* mainsizer;
};

//==========================================================================

// filter editor wrap for jfComplFiltEdit
class jfCompletedFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfCompletedFilterEditor(const jfCompletedFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter);
    virtual jfFilterBase* GetFilter();
    jfCompletedFilter* GetCompletedFilter(const QString& name) const;
    virtual bool GeneralCheck(const jfFilterMap* infmap) const;
  protected:
    jfCFE_Core1* insert_panel;
};
//******************************************************************************/
