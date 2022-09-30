//**************************************************************************
// Name:        ao3_searchfilter_panel.h
// Author :     John Q Metro
// Purpose :    GUI for AO3 search options and built-in filters
// Created:     July 16, 2022
// Updated:     July 16, 2022
//**************************************************************************

#ifndef AO3_SEARCHFILTER_PANEL_H
    #define AO3_SEARCHFILTER_PANEL_H
#endif // AO3_SEARCHFILTER_PANEL_H
//------------------------------------------------
#ifndef DISPLAY_PARTS_H
  #include "../../../interface/display/display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef AO3_SEARCH_H_INCLUDED
  #include "../../data/ao3/ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED

//-------------------------------------
#ifndef AO3_PANEL1_H_INCLUDED
  #include "ao3_panel1.h"
#endif // AO3_PANEL1_H_INCLUDED

#include <QVBoxLayout>
#include <QBoxLayout>
#include <QTabWidget>

//**************************************************************************

// packages the search options and the filters into a panel
class jfAO3_OptionsAndFiltersPanel : public QWidget {
  public:
    jfAO3_OptionsAndFiltersPanel(jfAO3Search* insearch_in, QWidget* parent = NULL);
    bool SaveToSearch();
    bool ChangeSearchAndLoad(jfAO3Search* new_search);
  protected:
    QVBoxLayout* main_layout;

    jfAO3_ESearchOptions* search_options;
    jfAO3_DFE* embedded_filters;
    jfAO3Search* insearch;

};

//==========================================================================
class jfAO3_SearchFilterPanel : public jfSearchPanelRoot {
  public:
    // the constructor
    jfAO3_SearchFilterPanel(jfAO3Search* insearch, QWidget* parent = NULL);
    // other external methods
    virtual bool LoadToObj();
    virtual bool ChangeObj(jfSearchCore* obj_data);
  protected:
    QBoxLayout *main_layout;
    QTabWidget* panel_holder;

    jf_Gen_CatPicker* main_picker;
    jfAO3_OptionsAndFiltersPanel* options_and_filters;

  // other data
    jfAO3Search* typed_search;

};
//**************************************************************************


