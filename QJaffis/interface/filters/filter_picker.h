/******************************************************************************
Name    :   filter_picker.h
Author  :   John Q Metro
Purpose :   Declares some custom interface elements for a filter picker dialog.
Created :   March 25, 2009
Conversion to Qt Started Oct 3, 2013
Updated :   August 22, 2023
******************************************************************************/
#ifndef FILTER_WIDGETS_H_INCLUDED
#define FILTER_WIDGETS_H_INCLUDED
#endif // FILTER_WIDGETS_H_INCLUDED
//----------------------------------------------------------------------------
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef JFILTERGROUPS
  #include "../../core/filters/filter_groups.h"
#endif
//----------------------------------------------------------------------------
#include <QTableWidget>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QDialog>
//*****************************************************************************
/* A 3-columed list box for picking filters. Column 1 is the filter name,
Column 2 is the filter type, and Column 3 (optional)is the filter description */
class jfFilterPickerBase : public QTableWidget {
    Q_OBJECT
  public:
    jfFilterPickerBase(bool three_cols, QWidget* parent = NULL);
    size_t SetContents(const jfFilterMap* data, QString inprefix="");
    bool ClearLoaded();
    // selected data info
    bool IsFilterSelected() const;
    bool ChangeSelected(size_t nindex);
    long SelectedIndex() const;
    long OldSelected() const;
    QString SelectedName() const;
    QString InsertString() const;
    void Deselect();
    // getting extra stuff
    QString NameAt(size_t nindex) const;
    // column sizing
    void SetColWidth(size_t prop1, size_t prop2, size_t prop3);
  public slots:
    void OnItemSelect();
  signals:
    void SendSelected();
  protected:
    // a helper method for setting contents
    bool InsertFLine(const size_t& iindex, const stl_FilterMap::const_iterator& data_index);
    bool SelectNoEvent(size_t index, bool issel);
    bool SetSelected();
    // local stuff
    bool loaded;
    QString prefix;
    // selection info
    bool sloaded;
    bool three_columns;
    QString select_name;
    long select_index;
    long old_selection;
    bool bypass;

};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* A 3-columed derivate of jfFilterPickerBase. Includes some methods and things
for creating an embedded link string for expressions */
class jfFilterPicker1 : public jfFilterPickerBase {
    Q_OBJECT
  public:
    jfFilterPicker1(QLabel* displaylink, QWidget* parent = NULL);
    // insert string item
    QString InsertString() const;
  public slots:
    void OnItemSelect();
  protected:
    // displaying the selection
    QLabel* display;
};
//============================================================================
/* global filter picker, with a listbox to pick the group, and a
jfFilterPicker to pick the individual filter */
class jfGlobalFilterPicker : public QWidget {
    Q_OBJECT
  public:
    jfGlobalFilterPicker(QLabel* displaylink, QWidget* parent = NULL);
    // loads data
    bool LoadData(jfMapOfFiltermaps* inlink);
    // sets the data in the picker control to the map at the index
    bool LoadPicker(const size_t& pindex);
    // returns some internal data
    bool IsFilterChosen() const;
    QString CurrentMapname() const;
    QString InsertString() const;
    // the event
    void MapSelected();
  protected:
    // the controls
    QHBoxLayout*     topsizer;
    jfFilterPicker1* fpicker;
    QListWidget*     lbox;
    // external data structure links
    bool loaded;
    jfMapOfFiltermaps* proxy_link;
    const jfFilterMap* sel_map;
    QString sel_mapname;
};
//============================================================================
/* A filter picking dialog, with two tabs; one for the local list, and one
for selecting a global filter. This produces a string reference, ready to be
inserted into an expression */
class jfFilterPickerDialog : public QDialog {
    Q_OBJECT
  public:
    jfFilterPickerDialog(const QString& title ,const jfFilterMap* inlocal_map, QString* inresult_ptr, QWidget* parent);
  public slots:
    void ProcessPick(bool clicked);
  protected:
    // arrangement controls
    QTabWidget* display_book;
    QVBoxLayout* topsizer;
    QHBoxLayout* bottom_sizer;
    // notebook pages
    jfFilterPicker1       *local_page;
    jfGlobalFilterPicker  *global_page;
    // stuff at the bottom
    QPushButton* pick_btn;
    QPushButton* cancel_btn;
    QLabel* labelresult;
    QLabel* showresult;
    // external links
    const jfFilterMap* local_map;
    QString* result_ptr;

};
//=============================================================================
/* New filter picker dialog -- decided that there is no such thing as a local
filter for the global filters, the filter picker there must be global only */
class jfGlobalFilterPickerDialog : public QDialog {
    Q_OBJECT
  public:
    jfGlobalFilterPickerDialog(const QString& title ,QString* inresult_ptr, QWidget* parent);
  public slots:
    void ProcessPick(bool clicked);
  protected:
    // arrangement controls
    QVBoxLayout* topsizer;
    QHBoxLayout* bottom_sizer;
    // notebook pages
    jfGlobalFilterPicker  *global_page;
    // stuff at the bottom
    QPushButton* pick_btn;
    QPushButton* cancel_btn;
    QLabel* labelresult;
    QLabel* showresult;
    // external links
    QString* result_ptr;
};

//*****************************************************************************
