//**************************************************************************
// Name:        ao3_panel1.h
// Author :     John Q Metro
// Purpose :    agentofourown.org interface
// Created:     September 3, 2012
// Conversion to Qt Started April 6, 2014
// Updated:     July 17, 2022
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
#ifndef AO3_TAGPARAMS_H
    #include "ao3_tagparams.h"
#endif // AO3_TAGPARAMS_H
//-------------------------------------

#ifndef SFILT_EDIT_H_INCLUDED
  #include "../../../interface/filters/base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../../../interface/filters/extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
#ifndef CHARO_EDITOR_H_INCLUDED
  #include "../../../interface/controls/charo_editor.h"
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
    jfZeroToMaxEditor* kc_picker;
    QLabel* taglabel;
    jfSimpleExprEdit* tag_filedit;

    // an extra set of sizers
    QGridLayout* lsizer1;
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
    void JiggleSize();
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

   jfLabeledIntEdit* words_min;
   jfLabeledIntEdit* words_max;

   QCheckBox* english_only;
   QCheckBox* completed;
   QCheckBox* crossover_only;

   jfAO3_TagParameters* tag_parameters;

   // NEW LAYOUT
   // Grid w 2 rows, 3 columns
   // top row is a spanned horizontal layout of minor items
   // below, in 3 colums: orientation, warning excludes, and tags

   QGridLayout* mainsizer;
   QHBoxLayout* toprow;
   QGroupBox* warnex_wrapper;
   QVBoxLayout* warnex_column;

   // sizers
   QGroupBox* sbwrapper;
   QHBoxLayout* topwrap;
   // special
   bool wrapped;
   bool vertflex;

};

//**************************************************************************

