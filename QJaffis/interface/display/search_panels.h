/******************************************************************************
Name    :   search_panels.h
Author  :   John Q Metro
Purpose :   Declares the search interface classes, assembled from display_base.h
            and display_base1.h for the Qt version
Created :   Originally Septmber 2009, new Qt version, November 23, 2013
Updated :   June 4, 2022
******************************************************************************/
#ifndef SEARCH_PANELS_H
#define SEARCH_PANELS_H
#endif // SEARCH_PANELS_H
//-------------------------------
#ifndef DISPLAY_PARTS_H
  #include "display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef FILTER_EDITOR_H_INCLUDED
  #include "../filters/filter_editor.h"
#endif // FILTER_EDITOR_H_INCLUDED
#ifndef CATEGORY_LISTEDIT_H_INCLUDED
  #include "../filters/category/category_listedit.h"
#endif // CATEGORY_LISTEDIT_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
/*****************************************************************************/
/* There are 2 options for search, 1 panel and 2 panels (for a total of 2 or 3
 *panels for the entire search+result interface). This file contains declarations
 *for both variants of "Panel 1", as well as "Panel 2" if we have a 2 panel search */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// The search below does not have the custom filters or result category info
class jfOnePanelSearch : public jfSearchGroupingCore {
  public:
    // the constructor
    jfOnePanelSearch(QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelRoot*  MakeFirstPanel() = 0;
    virtual jfSearchCore*       MakeTypedSearch() const = 0;
    virtual jfResultCollection* MakeTypedCollection() const = 0;
    virtual jfDownloadRootItems*   MakeTypedThread() const = 0;
    // returning data
    virtual bool UpdateSearchData();
  protected:
    virtual void FinishConstruction();
    // the sizer
    QVBoxLayout* top_sizer;
    // internal data
    bool custinit;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* The search below DOES include the custom filters and result category, and
 * therefore has an internal Tabbed picker */
class jfSearchGrouping : public jfSearchGroupingCore {
    Q_OBJECT
  public:
    // the constructor
    jfSearchGrouping(const QStringList* infclist, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelRoot*  MakeFirstPanel() = 0;
    virtual jfSearchCore*       MakeTypedSearch() const = 0;
    virtual jfResultCollection* MakeTypedCollection() const = 0;
    virtual jfDownloadRootItems*   MakeTypedThread() const = 0;
    // returning data
    virtual bool UpdateSearchData();
    //extra methods
    bool NameChangeForCategory(const QString& newname_in);
  public slots:
    void HandleNewDirectory();
  protected:
    virtual void FinishConstruction();
    // the main panels
    jfFilterMapEditor*        panel2;
    jfMainCategoriesEditor*   panel3;
    // other gui elements
    QTabWidget* panel_holder;
    jfDirPicker* dirpicker;
    QLabel* desc_label;
    QLineEdit* desc_entry;
    QHBoxLayout* top_bar;
    // the sizer
    QVBoxLayout* top_sizer;
    // internal data
    bool custinit;
    bool ninit;
    // external data
    const QStringList* fclist;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* If we go for a total of three panels, the second has the custom filter maker
 * and result category maker */
class jfPanelTwo : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfPanelTwo(jfSearchCore* onsearch, const QStringList* infclist, QWidget* parent = NULL);
    // returning data
    jfSearchCore* GetSearchObj();
    bool UpdateSearchData();
    //extra methods
    bool NameChangeForCategory(const QString& newname_in);
  public slots:
    void HandleNewDirectory();
  protected:
    void FinishConstruction();
    // the main panels
    jfFilterMapEditor*        panel2;
    jfMainCategoriesEditor*   panel3;
    // other gui elements
    QTabWidget* panel_holder;
    // jfNameDescEditor* nam_desc_e;
    jfDirPicker* dirpicker;
    QLabel* desc_label;
    QLineEdit* desc_entry;
    // the sizer
    QVBoxLayout* top_sizer;
    QHBoxLayout *top_bar;
    // internal data
    jfSearchCore* searchdata_link;
    bool ninit;
    // external data
    const QStringList* fclist;
  // the event handline declaration
};
/*****************************************************************************/
