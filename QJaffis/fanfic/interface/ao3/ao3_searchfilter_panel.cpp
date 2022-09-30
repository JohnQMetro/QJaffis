//**************************************************************************
// Name:        ao3_searchfilter_panel.cpp
// Author :     John Q Metro
// Purpose :    GUI for AO3 search options and built-in filters
// Created:     July 16, 2022
// Updated:     July 16, 2022
//**************************************************************************

#ifndef AO3_SEARCHFILTER_PANEL_H
    #include "ao3_searchfilter_panel.h"
#endif // AO3_SEARCHFILTER_PANEL_H
//------------------------------------------------
#include <QVBoxLayout>

//**************************************************************************
jfAO3_OptionsAndFiltersPanel::jfAO3_OptionsAndFiltersPanel(jfAO3Search* insearch_in, QWidget* parent):QWidget(parent) {
    insearch = insearch_in;

    main_layout = new QVBoxLayout();
    search_options = new jfAO3_ESearchOptions(insearch_in, true,false);
    embedded_filters = new jfAO3_DFE(insearch_in);

    main_layout->addWidget(search_options,5);
    main_layout->addSpacing(8);
    main_layout->addWidget(embedded_filters,5);

    setLayout(main_layout);
}
// ----------------------------
bool jfAO3_OptionsAndFiltersPanel::SaveToSearch() {
    if (insearch == NULL) return false;
    // setting the category selection
    bool okay = search_options->SaveToSearch(insearch);
    if (!okay) return false;
    // starting
    return embedded_filters->SaveFilters();
}
// ----------------------------
bool jfAO3_OptionsAndFiltersPanel::ChangeSearchAndLoad(jfAO3Search* new_search) {
    if (new_search == NULL) return false;
    bool okay = search_options->LoadFromSearch(new_search);
    if (!okay) return false;
    okay = embedded_filters->ChangeSearch(new_search);
    if (!okay) {
        search_options->LoadFromSearch(insearch);
        return false;
    }
    insearch = new_search;
    return true;
}

//==========================================================================
jfAO3_SearchFilterPanel::jfAO3_SearchFilterPanel(jfAO3Search* insearch, QWidget* parent):jfSearchPanelRoot(insearch, parent) {
    typed_search = insearch;

    // creating the panels
    main_picker = new jf_Gen_CatPicker(insearch->GetHolder() ,insearch->GetSelector(),7);
    options_and_filters = new jfAO3_OptionsAndFiltersPanel(insearch);

    // layout
    main_layout = new QVBoxLayout();
    panel_holder = new QTabWidget();
    main_layout->addWidget(panel_holder,1);
    setLayout(main_layout);

    // sticking the panels in
    panel_holder->addTab(main_picker,"Search Categories");
    panel_holder->addTab(options_and_filters,"Options and Filters");
    panel_holder->setCurrentIndex(0);
}
//--------------------
bool jfAO3_SearchFilterPanel::LoadToObj() {
    const QString fname = "jfAO3_SearchFilterPanel::LoadToObj";
    // starting
    if (!(options_and_filters->SaveToSearch())) return false;
    // setting the category selection
    main_picker->SaveFilters();
    typed_search->ApplySelection();
    size_t ncats = typed_search->GetNumCats();
    return (ncats!=0);
}
// -------------------------
bool jfAO3_SearchFilterPanel::ChangeObj(jfSearchCore* obj_data) {
    const QString fname = "jfAO3_SearchFilterPanel::ChangeObj";

    if (obj_data == NULL) return false;
    jfAO3Search* istore = dynamic_cast<jfAO3Search*>(obj_data);
    if (!main_picker->ChangeSelector(istore->GetSelector())) return false;
    if (options_and_filters->ChangeSearchAndLoad(istore)) {
        mainobj = istore;
        typed_search = istore;
        return true;
    }
    else return false;
}

//**************************************************************************


