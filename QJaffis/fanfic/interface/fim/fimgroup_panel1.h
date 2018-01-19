/******************************************************************************
Name    :   fimgroup_panel1.h
Author  :   John Q Metro
Purpose :   Declares the custom first panel for fimfiction.net group searching
Created :   August 10, 2013
Conversion to Qt Started August 3, 2015
Updated :   January 7, 2018 (changing to the new fimfiction.net group search).
******************************************************************************/
#ifndef FIMGROUP_PANEL1_H_INCLUDED
#define FIMGROUP_PANEL1_H_INCLUDED
#endif // FIMGROUP_PANEL1_H_INCLUDED
//--------------------------------------
#include <QString>
#include <QLineEdit>
#include <QComboBox>
//--------------------------------------
#ifndef DISPLAY_PARTS_H
  #include "../../../interface/display/display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "../../data/fim/fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../../../interface/misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED

//*****************************************************************************
const QString DEF_fimgstoryc_name = "Default Fimfiction.Net Group Story Count Filter";

class jfFIMGroup_DFE : public jfDefaultFilterEditorBase {
  public:
    // the constructor
    jfFIMGroup_DFE(jfFIMGroupSearch* inobj, QWidget* parent = NULL);
  protected:
		// internal constant methods
    // implemented virtual methds
    virtual bool SaveFiltersExtended();
    virtual bool ChangeSearchExtended(jfSearchCore* obj_data);
    // story count filter
    jfMinMaxEditor* sc_filedit;
    QVBoxLayout* sidesizer;
    // internal data
    jfFIMGroupSearch* typed_search;

};
//==============================================================================
class jfFIMGroup_SearchOptions : public jfSearchOptionsBase {
        Q_OBJECT
  public:
    // constructor
    jfFIMGroup_SearchOptions(QWidget* parent = NULL);
    // methods
    virtual bool LoadFrom(jfSearchCore* insearch);
    virtual bool StoreTo(jfSearchCore* outsearch);
  protected:
    // helper methods
    void CreateWidgets();
    void ArrangeWidgets();
    // gui entry data
    // jfGenMinMaxEditor* range_edit1;
    // jfGenMinMaxEditor* range_edit2;
    QLabel* main_label;
    QLineEdit* search_term_entry;
    QLabel* result_order_label;
    QComboBox* search_result_order;
    // the sizers
    QGridLayout* main_layout;

};
//==============================================================================
class jfFIMGroupPanel1 : public jfSearchPanelBase {
  public:
    // the constructor
    jfFIMGroupPanel1(jfFIMGroupSearch* insearch, QWidget* parent = NULL);
    // other external methods
    virtual bool LoadToObj();
    virtual bool ChangeObj(jfSearchCore* obj_data);
  protected:
    virtual jfDefaultFilterEditorBase* MakeTypedFilEdit();
    // other data
    jfFIMGroupSearch* typed_search;
};
//*****************************************************************************
