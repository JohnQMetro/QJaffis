/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_catpick2.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net catgeory picker, set 2, taken from ffn_catpick1
// Created:     August 22, 2010
// Conversion to Qt started July 26, 2014
// Updated:     July 26, 2014
/////////////////////////////////////////////////////////////////////////////
#define FFNCATPICK2

#ifndef FFNCATPICK
  #include "ffn_catpick1.h"
#endif
//-------------------------------------------------
#ifndef FFNCATPDATA
  #include "../categories/ffn_catpdata.h"
#endif
#ifndef CATGROUP_H
  #include "../categories/catgroup.h"
#endif // CATGROUP_H
#ifndef SEXPR_EDIT_H_INCLUDED
  #include "../../interface/filters/base/sexpr_edit.h"
#endif // SEXPR_EDIT_H_INCLUDED

//**************************************************************************
class jfCPickerDispl : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfCPickerDispl(jfFFN_CatP_SelCat* inselection, QWidget* parent = NULL);
    // special methods
    void UpdateDisplay();
    void Disable(bool yes=true);
    bool ChangeSelector(jfFFN_CatP_SelCat* newselection);
  signals:
    void ItemDeleted(QString dstring);
  public slots:
    void HandleDelete(bool cclick);
  protected:
    // gui elements
    QListWidget* display;
    QPushButton* del_btn;
    QVBoxLayout* top_sizer;
    // external data
    jfFFN_CatP_SelCat* selection_data;
};


//----------------------------------------------------------------------------
class jf_FFN_CatPicker : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jf_FFN_CatPicker(jfFFN_Categories* inmain, jfFFN_CatP_SelCat* inselection, QWidget* parent = NULL);
    // additional stuff
    bool ChangeSelector(jfFFN_CatP_SelCat* newselection);
    bool SaveFilters();
  public slots:
    void HandleCheck(bool xcheck);
    void HandleListDelete(QString in_dstring);
  protected:
    // gui elements
    QTabWidget* panel_holder;
    jfFFN_SectionPicker* section_panel;
    jfFFN_CrossoverPicker*   crossover_panel;
    jfCPickerDispl*     picked_display;
    // expression editing
    jfSimpleExprEdit *and_edit, *or_edit;
    QLabel *and_label, *or_label;
    QGridLayout* expredit_sizer;
    // sizers
    QHBoxLayout* middle_sizer;
    QVBoxLayout* top_sizer;
    // external data
    jfFFN_Categories* main_catdata;
    jfFFN_CatP_SelCat* selection_data;
};
