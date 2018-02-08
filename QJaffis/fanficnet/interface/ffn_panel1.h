//**************************************************************************
// Name:        ffn_panel1.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net search, first panel
// Created:     June 25, 2010
// conversion to Qt started July 27, 2014
// Updated:     February 8, 2018 (exclude genre built-in-filter)
//**************************************************************************
#define FFN_PANEL1

#ifndef JF_FFNSEARCH
  #include "../data/ffn_search.h"
#endif
#ifndef FFNCATPICK2
  #include "ffn_catpick2.h"
#endif
#ifndef DISPLAY_PARTS_H
  #include "../../interface/display/display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef WIDGETS2_H_INCLUDED
  #include "../../interface/misc/widgets2.h"
#endif // WIDGETS2_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../../interface/misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED

#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../../interface/filters/extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED

#ifndef FFN_FILTEREDIT1_H
  #include "../../interface/filters/ffn/ffn_filteredit1.h"
#endif // FFN_FILTEREDIT1_H

//-----------------------------------
#include <QComboBox>
//**************************************************************************
// default Fanfiction.Net filter names
const QString DEF_genretag_name = "Default Fanfiction.Net Genres Filter";
const QString DEF_ffnauthor_name = "Default Fanfiction.Net Author Filter";
const QString DEF_ffnchars_name = "Default Fanfiction.Net Characters Filter";
const QString DEF_ffnwc_name = "Default Fanfiction.Net Word Count Filter";
const QString DEF_ffncom_name = "Default Fanfiction.Net Completed Filter";
const QString DEF_ffnlang_name = "Default Fanfiction.Net Language Filter";
const QString DEF_ffnfavs_name = "Default Fanfiction.Net Favs Filter";

//==========================================================================
class jfFFN_DFE : public jfDefaultFilterEditorBase {
  public:
    // the constructor
    jfFFN_DFE(jfFFNSearch* inobj, QWidget* parent = NULL);
    // special methods
    void Disable(bool yes=true);
  protected:
		// internal constant methods
    // implemented virtual methds
    virtual bool SaveFiltersExtended();
    virtual bool ChangeSearchExtended(jfSearchCore* obj_data);
    // extra internal gui items
    jfTagStatusPicker* genre_picker;
    QLabel* auth_fillabel;
    QLabel* char_fillabel;
    jfSimpleExprEdit* auth_filedit;
    jfSimpleExprEdit* char_filedit;
    jfGenMinMaxEditor* wc_picker;
    jfFavsCountPanel* favs_filedit;
    jfComplFiltEdit* cpicker;
    QLabel* lang_fillabel;
    jfSimpleExprEdit* lang_filedit;
    // an extra set of sizers
    QGridLayout* lsizer1;
    QBoxLayout* lsizer2, *lsizer3, *lsizer4;
};
//=========================================================================
// panel for picking extra Fanfiction.Net search options
// for the parameters, dowarrper = true means encase all the controls in the staticboxsizer
class jfFFN_ESearchOptions : public QWidget {
  public:
    // the constructor
    jfFFN_ESearchOptions(jfFFNSearch* insearch, bool dowrapper, bool fvertical, QWidget* parent = NULL);
    // io
    bool SaveToSearch(jfFFNSearch* outsearch) const;
    bool LoadFromSearch(jfFFNSearch* insearch);
    void SetDefaults();
    // special
    void Disable(bool dis=true);
  protected:
    // setup methods
    void CreateSizers();
    void MakeControls();
    void ArrangeControls();
   // the gui controls
   QLabel* rat_label;
   QComboBox* ratings;
   QLabel* gen_label;
   QComboBox* genres;
   QLabel* wc_label;
   QComboBox* wordcount;
   QLabel* lan_label;
   QComboBox* languages;
   QLabel* excl_gen_label;
   QComboBox* excl_genres;
   QCheckBox* completed;
   // sizers
   QGroupBox* sbwrapper;
   QGridLayout* mainsizer;
   QHBoxLayout* topwrap;
   // special
   bool wrapped;
   bool vertflex;

};
//==========================================================================
class jfFFN_SearchPanel : public jfSearchOptionsBase {
  public:
    // constructor
    jfFFN_SearchPanel(jfFFNSearch* insearch, QWidget* parent = NULL);
    // object i/o
    bool LoadFrom(jfSearchCore* insearch);
    bool StoreTo(jfSearchCore* outsearch);
  protected:
    // gui elements
    jf_FFN_CatPicker* main_picker;
    jfFFN_ESearchOptions* soptions;
    QVBoxLayout* bsizer;
};
//==========================================================================
class jfFFN_Panel1 : public jfSearchPanelBase {
  public:
    // the constructor
    jfFFN_Panel1(jfFFNSearch* insearch, QWidget* parent = NULL);
    // other external methods
    virtual bool LoadToObj();
    virtual bool ChangeObj(jfSearchCore* obj_data);
  protected:
    virtual jfDefaultFilterEditorBase* MakeTypedFilEdit();
    jfFFN_SearchPanel* mpanel;


};
//**************************************************************************
