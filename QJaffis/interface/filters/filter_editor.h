/******************************************************************************
Name    :   filter_editor.h
Author  :   John Q Metro
Purpose :   Declares the interface for a local and global filter editor
Created :   August 13, 2009
Conversion to Qt Started Oct 24, 2013
Updated :   March 11, 2013 (column resizing for jfFilterPicker2)
******************************************************************************/
#ifndef FILTER_EDITOR_H_INCLUDED
#define FILTER_EDITOR_H_INCLUDED
#endif // FILTER_EDITOR_H_INCLUDED
#ifndef FILTER_WIDGETS_H_INCLUDED
  #include "filter_picker.h"
#endif // FILTER_WIDGETS_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef JFILTERGROUPS
  #include "../../core/filters/filter_groups.h"
#endif
//--------------------------------------------------------------------------------
#include <QComboBox>
#include <QGroupBox>
/******************************************************************************/
const size_t global_ratio = 5;
const size_t local_ratio = 2;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// a 2 columned filter picker
class jfFilterPicker2 : public jfFilterPickerBase {
    Q_OBJECT
  public:
    // the constructor
    jfFilterPicker2(QWidget* parent);
    // item manipulation
    bool InsertAtIndex(const size_t& nindex, const jfBaseFilter* infilt);
    bool ReplaceSelected(const size_t& nindex, const jfBaseFilter* infilt);
    bool RenameItem(size_t oldindex, size_t newindex, QString newname);
    bool ReplaceAtIndex(size_t oldindex, size_t newindex, const jfBaseFilter* infilt);
  protected:
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* A small dialog used in new filter creation, to pick the name and type */
class jfStartFilterDialog : public QDialog {
    Q_OBJECT
  public:
    // the constructor
    jfStartFilterDialog(const QString& startname, const QStringList* inpflist, QWidget* parent);
    // getting results
    QString GetNewName() const;
    size_t GetResIndex() const;
  public slots:
    void OKPressed(bool checked);
  protected:
    // the gui elements
    jfLabeledEdit* name_entry;
    QLabel* picker_label;
    QComboBox* type_picker;
    QVBoxLayout* top_sizer;
    QHBoxLayout* button_sizer;
    QPushButton *ok_btn, *can_btn;
    // choices
    const QStringList* ftchoices;
    // results
    QString newname;
    size_t typeindex;
};
//=============================================================================
/* the local filter editor, which handles one filter map. Includes a picker,
buttons to add and delete filters, and a panel to hold the filter-specific
editor for the currently selected filter */
class jfFilterMapEditor : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfFilterMapEditor(jfFilterMap* inlink,bool in_global, const QStringList* inpflist, QWidget* parent = NULL);
    //data setting
    bool ChangeMap(jfFilterMap* inlink);
    bool ChangeFilter(size_t findex);
    void SetNoFilter();
    // special methods
    void DisableContents(bool disable=true);
    bool CoreSave(bool saveold = false);
  public slots:
    void NewPressed(bool checked);
    void DeletePressed(bool checked);
    void SavePressed(bool checked);
    void FilterPicked();
  protected:
    // helper methods
    bool SaveFilter(long oindex);
    bool LoadSelected();
    QString MakeName(const QString& nfbase) const;
    void BtnEnable(bool enable);
    // gui elements
    QPushButton* new_button;
    QPushButton* del_button;
    QPushButton* save_button;
    jfFilterPicker2* picker;
    QWidget* blank_panel;
    jfBaseFilterEditor* editing_panel;
    bool isblank;
    bool disablec;
    // sizers
    QHBoxLayout* top_sizer;
    QVBoxLayout* side_sizer;
    QHBoxLayout* btn_sizer;
    // other data
    jfFilterMap* fdata;
    QString oldtype;
    bool is_inglobal;
    int selindex;
    // external link
    const QStringList* pflist;
};
//==============================================================================
/* Used in Global filter editor, a panel including a name editor and a local
filter editor */
class jfMapEditPanel : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfMapEditPanel(jfFilterMap* inlink, QWidget* parent = NULL);
    // accessible methods
    bool ChangeMap(jfFilterMap* inlink);
    QString GetName();
    QString GetDescription() const;
    void ChangeBoxName(QString newname);
  public slots:
    void CatchSaved(bool cval);
  signals:
    void ThrowSaved();
  protected:
    // gui elements
    jfNameDescEditor* ndesce;
    QPushButton* save_name;
    jfFilterMapEditor* map_editor;
    // sizers
    QGroupBox* wrapper;
    QVBoxLayout *innerl, *outerl;
    // internal data
    bool mappset;
    jfFilterMap* itemlink;
};
//==============================================================================
/* the global filter editor, which has a list of maps, a map editor, two buttons
for adding and deleting maps, and an editor for the map name+desc
*/
class jfGlobalFilterEditor : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfGlobalFilterEditor(jfMapOfFiltermaps* inlink, QWidget* parent = NULL);
    // methods
    void SetNoMap();
    bool LoadSelectedMap();
    bool ChangeMap(const size_t& index);
  public slots:
    //event handlers
    void NewPressed(bool inval);
    void DeletePressed(bool inval);
    void SavePressed();
    void MapPicked();
  protected:
    // helper methods
    size_t MakeDefaultMap(QString thename);
    // gui elemts
    QPushButton* add_btn;
    QPushButton* dele_btn;
    QListWidget* map_list;
    QWidget* blank_panel;
    jfMapEditPanel* main_mapedit;
    bool isblank;
    // sizers
    QHBoxLayout* btn_sizer;
    QVBoxLayout* side_sizer;
    QHBoxLayout* top_sizer;
    // internal data
    jfMapOfFiltermaps* maindata;
    size_t selindex;
    QString selname;
    jfFilterMap* selmap;
};
/******************************************************************************/
