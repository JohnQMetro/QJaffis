//**************************************************************************
// Name:        ao3_tagparams.cpp
// Author :     John Q Metro
// Purpose :    a widget to package the tag related parameters for AO3
// Created:     July 17, 2022
// Updated:     July 18, 2022
//**************************************************************************
#ifndef AO3_TAGPARAMS_H
    #include "ao3_tagparams.h"
#endif // AO3_TAGPARAMS_H
//---------------------------------------
#include "../../../core/tagfilter/tagmatch.h"
//**************************************************************************
jfAO3_TagParameters::jfAO3_TagParameters(QWidget* parent):QWidget(parent) {
    const QString fname = "jfAO3_TagParameters::jfAO3_TagParameters";
    main_sizer = new QGridLayout();
    predefined_excludes_holder = new QVBoxLayout();
    MakeControls();
    ArrangeControls();
    setLayout(main_sizer);
}
//--------------------------
void jfAO3_TagParameters::SetDefaults() {

}
bool jfAO3_TagParameters::SaveToSearch(jfAO3Search* outsearch) const {
    const QString fname = "jfAO3_TagParameters::SaveToSearch";
    // bad input
    if (outsearch==NULL) return false;
    // tag excludes
    outsearch->SetTagExcludes(excluded_tags_edit->toPlainText(), exclude_insert_edit->text());
    // predefined tag excludes
    QStringList picked_items;
    for (int i = 0; i < exclude_list.size(); i++) {
        if (predefined_excludes->GetBoolState(i)) picked_items.append(exclude_list.at(i));
    }
    outsearch->SetPredefinedExcludes(picked_items);
    // includes
    outsearch->SetIncludes(required_tags_edit->text());

  return true;
}
//------------------------------
bool jfAO3_TagParameters::LoadFromSearch(jfAO3Search* insearch) {
    if (insearch==NULL) return false;

    // excluded tags
    excluded_tags_edit->setPlainText(insearch->GetExcludedTagsList());
    exclude_insert_edit->setText(insearch->GetExcludeReplacer());
    // predefined excludes
    QStringList specified_excluded = insearch->GetPredefinedExcludes();
    predefined_excludes->CheckNames(&specified_excluded);
    // includes
    required_tags_edit->setText(insearch->GetIncludedTagsList());
    // done
    return true;
}
//--------------------------
// special
void jfAO3_TagParameters::Disable(bool dis) {

    excluded_tags_edit->setEnabled(!dis);
    exclude_insert_edit->setEnabled(!dis);
    required_tags_edit->setEnabled(!dis);

    predefined_excludes->setDisabled(dis);

}
// ++++++++++++++++++++++++++++++++++++++++++
// setup methods
void jfAO3_TagParameters::MakeControls() {

    // excluded tags
    excluded_label = new QLabel("Tags to Exclude");
    excluded_tags_edit = new QPlainTextEdit();
    exclude_insert_label = new QLabel("Exclude Insert");
    exclude_insert_edit = new QLineEdit();

    // required tags
    required_tags_label = new QLabel("Tags to Require");
    required_tags_edit = new QLineEdit();

    // predefined excluded tag checkboxes
    predef_excl_label = new QLabel("Predefined Excludes");
    predefined_excludes = new jfCheckList(false);
    exclude_list = taglistings::excludeListsGroup->ExcludeListNames();
    predefined_excludes->replaceItems(exclude_list);

}
//--------------------------
void jfAO3_TagParameters::ArrangeControls() {
    // Grid has 4 columns, 3 rows, row and column 1 are stretch
    main_sizer->addWidget(excluded_label, 0, 0,Qt::AlignLeft);
    main_sizer->addWidget(exclude_insert_label, 0, 2, Qt::AlignLeft );
    main_sizer->addWidget(excluded_tags_edit,1,0,1,2);
    main_sizer->addWidget(exclude_insert_edit, 1,2,Qt::AlignTop);
    main_sizer->addWidget(required_tags_label, 2, 0, Qt::AlignRight);
    main_sizer->addWidget(required_tags_edit, 2, 1, 1, 2);
    main_sizer->setColumnStretch(1,1);
    main_sizer->setRowStretch(1,1);

    // the last column is for predefined excludes
    predefined_excludes_holder->addWidget(predef_excl_label);
    predefined_excludes_holder->addWidget(predefined_excludes,1);
    predefined_excludes_holder->setContentsMargins(0,0,0,0);

    main_sizer->addLayout(predefined_excludes_holder, 0, 3, 3, 1);
    main_sizer->setContentsMargins(0,0,0,0);

}

//**************************************************************************
