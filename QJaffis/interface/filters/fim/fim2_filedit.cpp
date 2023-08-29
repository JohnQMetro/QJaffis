/******************************************************************************
Name    :   fim2_filedit.cpp
Author  :   John Q Metro
Purpose :   More Editors for fim filters
Created :   August 5, 2015
Updated :   August 22, 2023
******************************************************************************/
#ifndef FIM2_FILEDIT_H
#include "fim2_filedit.h"
#endif // FIM2_FILEDIT_H
//------------------------------------------------
#include <assert.h>
/*****************************************************************************/

// the default constructor
jfFIMShortDesc_FilterEditor::jfFIMShortDesc_FilterEditor(const jfFIM_ShortDesc_ExprFilter* infilt,
                                        QWidget* parent):jfSimpleSFilterEditor(infilt,parent) {
 // things are handled by the parent
}

//------------------------------------------------------------------------------
// internal methods
jfSimpleExpFilterCore* jfFIMShortDesc_FilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfFIM_ShortDesc_ExprFilter* result = new jfFIM_ShortDesc_ExprFilter(namedesc_edit->TryGetName(),inpval);
  return result;
}
//==========================================================================================
// the default constructor
jfFIMDualDesc_FilterEditor::jfFIMDualDesc_FilterEditor( const jfFIM_DualDesc_ExprFilter* infilt,
                                        QWidget* parent):jfSimpleSFilterEditor(infilt,parent) {
 // things are handled by the parent
}

//------------------------------------------------------------------------------
// internal methods
jfSimpleExpFilterCore* jfFIMDualDesc_FilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfFIM_DualDesc_ExprFilter* result = new jfFIM_DualDesc_ExprFilter(namedesc_edit->TryGetName(), inpval);
  return result;
}

//===========================================================================================
jfFIMGroupSCFilterEditor::jfFIMGroupSCFilterEditor(const jfFIMGroupSCFilter* infilt,
          QWidget* parent):jfZeroToMaxFilterEditor("Story Count Range",999999,infilt,parent) {
  // not much
  insert_panel->SetMinMax(0,0);
}

//--------------------------------------------------------------------
jfMinMaxUFilter* jfFIMGroupSCFilterEditor::MakeTypedMinMax() const {
  return new jfFIMGroupSCFilter(namedesc_edit->TryGetName());
}
//====================================================================
jfFIMGroupMCFilterEditor::jfFIMGroupMCFilterEditor(const jfFIMGroupMCFilter* infilt, QWidget* parent):
                        jfZeroToMaxFilterEditor("Story Count Range",30000, infilt,parent) {
  // not much
  insert_panel->SetMinMax(0,0);
}

//--------------------------------------------------------------------
jfMinMaxUFilter* jfFIMGroupMCFilterEditor::MakeTypedMinMax() const {
  return new jfFIMGroupMCFilter(namedesc_edit->TryGetName());
}
/*****************************************************************************/
jfFimThumbPercentPanel::jfFimThumbPercentPanel(QWidget* parent):QWidget(parent) {
    min_perc = new jfLabeledIntEdit(NULL,"Percentage of Thumbs up",true,0,100);
    include_rdisabled = new QCheckBox("Include Disabled");
    top_layout = new QHBoxLayout();
    top_layout->addWidget(min_perc,2,Qt::AlignVCenter);
    top_layout->addWidget(include_rdisabled,2,Qt::AlignVCenter);
    setLayout(top_layout);
}
//----------------------------------------------
jfFimThumbPercentFilter* jfFimThumbPercentPanel::GetNewFilter(const QString& name) const {
    // the values
    int tval;
    jfFimThumbPercentFilter* outfilt;
    // getting the value
    tval = min_perc->GetValue();
    // creating
    outfilt = new jfFimThumbPercentFilter(name);
    outfilt->SetPercent(tval);
    outfilt->SetIncludeDisabled(include_rdisabled->isChecked());
    return outfilt;
}
//-----------------------------------------------
bool jfFimThumbPercentPanel::SetFromObj(const jfFimThumbPercentFilter* inval) {
    // the cases where things are wrong
    if (inval==NULL) return false;
    // setting things
    min_perc->SetValue(inval->GetPercent());
    include_rdisabled->setChecked(inval->GetIncludeDisabled());
    return true;
}
//------------------------------------------------
void jfFimThumbPercentPanel::SetDefault() {
    min_perc->SetValue(85);
    include_rdisabled->setChecked(false);
}

/*****************************************************************************/
jfFimThumbPercentFilterEditor::jfFimThumbPercentFilterEditor(const jfFimThumbPercentFilter* infilt,
                                              QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfFimThumbPercentPanel();
  if (infilt!=NULL) {
    insert_panel->SetFromObj(infilt);
  }
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  insert_panel->SetDefault();
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfFimThumbPercentFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  assert(infilter!=NULL);
  const jfFimThumbPercentFilter* xvalue;
  filt_pointer = infilter;
  xvalue = dynamic_cast<const jfFimThumbPercentFilter*>(filt_pointer);
  insert_panel->SetFromObj(xvalue);
  NameLoad();
}
//-------------------------------------
jfFilterBase* jfFimThumbPercentFilterEditor::GetFilter() {
  // the values
  jfFimThumbPercentFilter* outfilt = insert_panel->GetNewFilter(namedesc_edit->TryGetName());
  namedesc_edit->ChangeFilter(outfilt);
  return outfilt;
}
//--------------------------------------------------------------
bool jfFimThumbPercentFilterEditor::GeneralCheck(const jfFilterMap* filter_group) const {
    if (NameNUniq(filter_group)) return false;
    else return true;
}
/*****************************************************************************/
