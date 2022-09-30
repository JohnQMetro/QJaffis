/******************************************************************************
Name    :   fim_panel1.h
Author  :   John Q Metro
Purpose :   Declares the custom first panel for fimfiction.net searching
Created :   June 23, 2012
Conversion to Qt started February 3, 2014
Updated :   October 15, 2019
******************************************************************************/
#ifndef FIM_PANEL1_H_INCLUDED
#define FIM_PANEL1_H_INCLUDED
#endif // FIM_PANEL1_H_INCLUDED
//--------------------------------------
#ifndef WIDGETS2_H_INCLUDED
  #include "../../../interface/controls/widgets2.h"
#endif // WIDGETS2_H_INCLUDED
#ifndef FIM_SEARCH_H_INCLUDED
  #include "../../data/fim/fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED
#ifndef FIM1_FILEDIT_H_INCLUDED
  #include "../../../interface/filters/fim/fim1_filedit.h"
#endif // FIM1_FILEDIT_H_INCLUDED
#ifndef FIM2_FILEDIT_H_INCLUDED
  #include "../../../interface/filters/fim/fim2_filedit.h"
#endif // FIM1_FILEDIT_H_INCLUDED
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../../../interface/filters/extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
#ifndef DISPLAY_PARTS_H
  #include "../../../interface/display/display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef SEXPR_EDIT_H_INCLUDED
  #include "../../../interface/filters/base/sexpr_edit.h"
#endif // SEXPR_EDIT_H_INCLUDED
#ifndef TAG_WIDGETS_H
  #include "../../../interface/controls/tag_widgets.h"
#endif // TAG_WIDGETS_H
#ifndef TAG_WIDGETS2_H
  #include "../../../interface/controls/tag_widgets2.h"
#endif // TAG_WIDGETS2_H
//-------------------------------------
#include <QComboBox>

//*****************************************************************************
const QString DEF_fimthumbs_name = "Default Fimfiction.Net Thumbs Up/Down Filter";
const QString DEF_fimpthumbs_name = "Default Fimfiction.Net Percent Thumbs Up/Down Filter";
const QString DEF_fimdualdesc_name = "Default Fimfiction.Net Both Descriptions Expression Filter";

class jfFIM_DFE : public jfDefaultFilterEditorBase {
  public:
    // the constructor
    jfFIM_DFE(jfFIMSearch* inobj, QWidget* parent = NULL);
  protected:
		// internal constant methods
    // implemented virtual methds
    virtual bool SaveFiltersExtended();
    virtual bool ChangeSearchExtended(jfSearchCore* obj_data);
    // extra filters
    jfFIMThumbsPanel* tud_filedit;
    jfFimThumbPercentPanel* tper_edit;
    jfSimpleExprEdit* dual_expredit;
    QLabel* dual_label;
    // layout
    QVBoxLayout* sidesizer;
    // internal data
    jfFIMSearch* typed_search;

};
//=============================================================================
class jfFIM_SearchOptions : public jfSearchOptionsBase {
    Q_OBJECT
  public:
    // constructor
    jfFIM_SearchOptions(QWidget* parent = NULL);
    // methods
    virtual bool LoadFrom(jfSearchCore* insearch);
    virtual bool StoreTo(jfSearchCore* outsearch);
  public slots:
    void egcbChanged(int state);
  protected:
    // helper methods
    void CreateWidgets();
    void ArrangeWidgets();

    // gui entry data
    QLabel* search_label;
    QLineEdit* search_edit;
    // jfTagCategorizer* genre_sentry;
    // jfTagCategorizer* char_sentry;
    jfTagStatusPicker* genres_entry;
    jfTagStatusPicker* content_entry;
    jfTagSorter* char_sentry;
    jfZeroToMaxEditor* wcount;
    QCheckBox* cbox;
    QLabel* rat_label;
    QComboBox* rating_picker;
    QLabel* order_label;
    QComboBox* order_picker;
    QCheckBox* matures;
    QCheckBox* equestria_girls_cb;
    jfTagStatusPicker* warnings_entry;
    // the sizers
    QGroupBox *char_wrap;
    QHBoxLayout *char_layt;
    QHBoxLayout* topbar, *bottombar;
    QVBoxLayout* pre_sidebar;
    QVBoxLayout* sidebar;
    QVBoxLayout* stack;
};

//==============================================================================
class jfFIMPanel1 : public jfSearchPanelBase {
  public:
    // the constructor
    jfFIMPanel1(jfFIMSearch* insearch, QWidget* parent = NULL);
    // other external methods
    virtual bool LoadToObj();
    virtual bool ChangeObj(jfSearchCore* obj_data);
  protected:
    virtual jfDefaultFilterEditorBase* MakeTypedFilEdit();
    // other data
    jfFIMSearch* typed_search;
};
//*****************************************************************************
