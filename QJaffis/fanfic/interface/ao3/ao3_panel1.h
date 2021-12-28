//**************************************************************************
// Name:        ao3_panel1.h
// Author :     John Q Metro
// Purpose :    agentofourown.org interface
// Created:     September 3, 2012
// Conversion to Qt Started April 6, 2014
// Updated:     December 27, 2021 (Removed Orientation filter)
//**************************************************************************
#ifndef AO3_PANEL1_H_INCLUDED
#define AO3_PANEL1_H_INCLUDED
#endif // AO3_PANEL1_H_INCLUDED
//------------------------------------------------
#ifndef DISPLAY_PARTS_H
  #include "../../../interface/display/display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef AO3_SEARCH_H_INCLUDED
  #include "../../data/ao3/ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED
#ifndef GCAT_PICKER_H_INCLUDED
  #include "../gcat_picker.h"
#endif // GCAT_PICKER_H_INCLUDED
//-------------------------------------
#ifndef SFILT_EDIT_H_INCLUDED
  #include "../../../interface/filters/base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../../../interface/filters/extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
#ifndef CHARO_EDITOR_H_INCLUDED
  #include "../../../interface/misc/charo_editor.h"
#endif // CHARO_EDITOR_H_INCLUDED
#ifndef AO3_SEXPEDITORS_H_INCLUDED
  #include "../../../interface/filters/ao3/ao3_sexpeditors.h"
#endif // AO3_SEXPEDITORS_H_INCLUDED
#ifndef AO3_SPECIAL3_H
  #include "../../../interface/filters/ao3/ao3_special3.h"
#endif // AO3_SPECIAL3_H
//**************************************************************************
// default Archiveofourown.org filter names
const QString DEF_ao3author_name = "Default Archiveofourown.org Author Filter";
const QString DEF_ao3chars_name = "Default Archiveofourown.org Characters Filter";
const QString DEF_ao3wc_name = "Default Archiveofourown.org Word Count Filter";
const QString DEF_ao3com_name = "Default Archiveofourown.org Completed Filter";
const QString DEF_ao3rat_name = "Default Archiveofourown.org Ratings Filter";
const QString DEF_ao3etf_name = "Default Archiveofourown.org Extra Tags Filter";
const QString DEF_ao3kudof_name = "Default Archiveofourown.org Kudos Filter";

//==========================================================================
class jfAO3_DFE : public jfDefaultFilterEditorBase {
  public:
    // the constructor
    jfAO3_DFE(jfAO3Search* inobj, QWidget* parent = NULL);
    // special methods
    void Disable(bool yes=true);
  protected:
		// internal constant methods
    // implemented virtual methds
    virtual bool SaveFiltersExtended();
    virtual bool ChangeSearchExtended(jfSearchCore* obj_data);
    // extra internal gui items
    QLabel* auth_fillabel;
    QLabel* char_fillabel;
    jfSimpleExprEdit* auth_filedit;
    jfSimpleExprEdit* char_filedit;
    jfZeroToMaxEditor* wc_picker;
    jfZeroToMaxEditor* kc_picker;
    jfComplFiltEdit* cpicker;
    jfCharCheckBoxGroup *ratpicker;
    QLabel* taglabel;
    jfSimpleExprEdit* tag_filedit;
    // an extra set of sizers
    QGridLayout* lsizer1;
    QHBoxLayout* lsizer2;
    QVBoxLayout* lsizer3;
};
//=========================================================================
// panel for picking extra Fanfiction.Net search options
// for the parameters, dowarrper = true means encase all the controls in the staticboxsizer
class jfAO3_ESearchOptions : public QWidget {
  public:
    // the constructor
    jfAO3_ESearchOptions(jfAO3Search* insearch, bool dowrapper, bool fvertical, QWidget* parent = NULL);
    // io
    bool SaveToSearch(jfAO3Search* outsearch) const;
    bool LoadFromSearch(jfAO3Search* insearch);
    void SetDefaults();
    // special
    void Disable(bool dis=true);
  protected:
    // setup methods
    void CreateSizers();
    void MakeControls();
    void ArrangeControls();
   // the gui controls
    // sex orientation
   jfTagStatusPicker* orient_entry;
   // rating combo picker
   QLabel* rat_label;
   QComboBox* ratings;
   // result order picker
   QLabel* reso_label;
   QComboBox* result_order;
   // warning excludes
   QCheckBox* wx_violence;
   QCheckBox* wx_death;
   QCheckBox* wx_rape;
   QCheckBox* wx_underage_sex;

   jfLabeledEdit* excluded_tags;
   jfLabeledEdit* etag_template;

   QCheckBox* tag_excl_gendex;
   QCheckBox* tag_excl_emo;
   QCheckBox* tag_excl_other;

   jfLabeledEdit* extratags;

   jfLabeledIntEdit* words_min;
   jfLabeledIntEdit* words_max;

   QCheckBox* english_only;
   QCheckBox* completed;

    // columns
   // 1 orientation
   // 2: ratings and result order (vbox)
   //  3 group warning excludes
   // 4: tags in 4 rows: include, 2+3 exclude, 4 row or checkbox excludes
   // 5 : other stuff

   QHBoxLayout* mainsizer;
   QVBoxLayout* column2;
   QGroupBox* warnex_wrapper;
   QVBoxLayout* warnex_column;
   QGridLayout* tagbox; // column4
   QVBoxLayout* column5;


   // sizers
   QGroupBox* sbwrapper;
   QHBoxLayout* topwrap;
   // special
   bool wrapped;
   bool vertflex;

};
//==========================================================================
class jfAO3_SearchOptions : public jfSearchOptionsBase {
  public:
    // constructor
    jfAO3_SearchOptions(jfAO3Search* insearch, QWidget* parent = NULL);
    // methods
    virtual bool LoadFrom(jfSearchCore* insearch);
    virtual bool StoreTo(jfSearchCore* outsearch);
  protected:
    // gui entry data
    jf_Gen_CatPicker* main_picker;
    jfAO3_ESearchOptions* soptions;
    QVBoxLayout* mlayout;
    jfAO3Search* istore;
};

//==========================================================================
class jfAO3_Panel1 : public jfSearchPanelBase {
  public:
    // the constructor
    jfAO3_Panel1(jfAO3Search* insearch, QWidget* parent = NULL);
    // other external methods
    virtual bool LoadToObj();
    virtual bool ChangeObj(jfSearchCore* obj_data);
  protected:
    virtual jfDefaultFilterEditorBase* MakeTypedFilEdit();
  // other data
    jfAO3Search* typed_search;

};
//**************************************************************************

