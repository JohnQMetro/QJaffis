/******************************************************************************
Name    :   gcat_picker.h
Author  :   John Q Metro
Purpose :   Category picking panel for generic section/category picking
Created :   August 25, 2012
Conversion to Qt Started April 6, 2014
Updated :   April 9, 2014
******************************************************************************/
#ifndef GCAT_PICKER_H_INCLUDED
  #define GCAT_PICKER_H_INCLUDED
#endif // GCAT_PICKER_H_INCLUDED
//------------------------------------------------
#ifndef CAT_COLL_H_INCLUDED
  #include "../data/cats/cat_coll.h"
#endif // CAT_COLL_H_INCLUDED
#ifndef SEXPR_EDIT_H_INCLUDED
  #include "../../interface/filters/base/sexpr_edit.h"
#endif // SEXPR_EDIT_H_INCLUDED
#ifndef WIDGETS1_H_INCLUDED
  #include "../../interface/misc/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
/*****************************************************************************/
// two checkbox list boxes to pick catgeories
class jfTwinPanelGCPicker : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfTwinPanelGCPicker(jfGen_CatHolder* inmain, jfGenCat_PickData* inselection, size_t prop, QWidget* parent = NULL);
    // special methods
    bool UnCheck(QString insname, QString incatname);
    bool ChangeSelector(jfGenCat_PickData* newselection, bool selzero);
  signals:
    void sendCheckSig(bool value);
  public slots:
    // events
    void HanSectionChecked(size_t which, Qt::CheckState what);
    void HanCategoryChecked(size_t which, Qt::CheckState what);
    void HanSectionChanged(int currentRow);
    void HanCategoryChanged(int currentRow);
  protected:
    // internal methods to set the selection
    virtual bool SetSection(size_t sindex);
    virtual bool LoadCatNames(size_t sindex);
    // setting the checks
    virtual bool CheckSelectedSection();
    virtual bool SetSectionChecks();
    // other virtual methods
    virtual bool AddRemove(bool add, QString sname);
    virtual bool CatAddRemove(bool add, QString sname, QString cname);
    // helper methods
    void SetCheckAll(bool value);
    bool NameUncheck(const QString& name);
    // the gui elements
    QHBoxLayout* top_sizer;
    jfCheckList* section_list;
    jfCheckList* cat_list;
    // external data
    jfGen_CatHolder* main_catdata;
    jfGenCat_PickData* selection_data;

};
//----------------------------------------------------------------------------
/* A panel which contains a list to diplay the jfFFN_CatP_SelCat object, and a
button to delete items in that */
class jfGCPickerDispl : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfGCPickerDispl(jfGenCat_PickData* inselection, QWidget* parent = NULL);
    // methods
    void UpdateDisplay();
    void Disable(bool yes=true);
    bool ChangeSelector(jfGenCat_PickData* newselection);
  signals:
    void SendSItemDeleted(QString del_value);
  public slots:
    void OnDeleteBtn(bool checked);
  protected:
    int FindDisplayName(const QString& fname, bool cassen) const;
    // GUI ELEMETS
    QListWidget* display;
    QPushButton* del_button;
    QVBoxLayout* top_sizer;
    // EXTERNAL DATA
    jfGenCat_PickData* selection_data;
};
//----------------------------------------------------------------------------
class jf_Gen_CatPicker : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jf_Gen_CatPicker(jfGen_CatHolder* inmain, jfGenCat_PickData* inselection, size_t prop, QWidget* parent = NULL);
    // additional stuff
    bool ChangeSelector(jfGenCat_PickData* newselection);
    bool SaveFilters();
  public slots:
    // event handlers
    void OnCheckEvent(bool value);
    void OnSItemDeleted(QString del_value);
  protected:
    // gui elements
    jfTwinPanelGCPicker* section_panel;
    jfGCPickerDispl*     picked_display;
    // expression editing
    jfSimpleExprEdit *and_edit, *or_edit;
    QLabel *and_label, *or_label;
    QGridLayout* expredit_sizer;
    // sizers
    QHBoxLayout* middle_sizer;
    QVBoxLayout* top_sizer;
    // external data
    jfGen_CatHolder* main_catdata;
    jfGenCat_PickData* selection_data;
};
/*****************************************************************************/
