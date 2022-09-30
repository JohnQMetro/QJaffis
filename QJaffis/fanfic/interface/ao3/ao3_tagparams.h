//**************************************************************************
// Name:        ao3_tagparams.h
// Author :     John Q Metro
// Purpose :    a widget to package the tag related parameters for AO3
// Created:     July 17, 2022
// Updated:     July 18, 2022
//**************************************************************************
#ifndef AO3_TAGPARAMS_H
    #define AO3_TAGPARAMS_H
#endif // AO3_TAGPARAMS_H
// --------------------------------------
#ifndef AO3_SEARCH_H_INCLUDED
  #include "../../data/ao3/ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED
#ifndef WIDGETS1_H_INCLUDED
  #include "../../../interface/controls/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
//---------------------------------------
#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
//**************************************************************************
class jfAO3_TagParameters : public QWidget {
  public:
    // the constructor
    jfAO3_TagParameters(QWidget* parent = NULL);
    void SetDefaults();
    bool SaveToSearch(jfAO3Search* outsearch) const;
    bool LoadFromSearch(jfAO3Search* insearch);
    // special
    void Disable(bool dis=true);
  protected:
    // setup methods
    void MakeControls();
    void ArrangeControls();

    // the gui controls
    QLabel* excluded_label;
    QPlainTextEdit* excluded_tags_edit;
    QLabel* exclude_insert_label;
    QLineEdit* exclude_insert_edit;

    QLabel* required_tags_label;
    QLineEdit* required_tags_edit;

    QLabel* predef_excl_label;
    QStringList exclude_list;
    jfCheckList* predefined_excludes;

    QGridLayout* main_sizer;
    QVBoxLayout* predefined_excludes_holder;

};

//**************************************************************************
