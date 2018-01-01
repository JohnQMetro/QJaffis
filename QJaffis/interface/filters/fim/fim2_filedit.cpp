/******************************************************************************
Name    :   fim2_filedit.cpp
Author  :   John Q Metro
Purpose :   More Editors for fim filters
Created :   August 5, 2015
Updated :   August 8, 2015
******************************************************************************/
#ifndef FIM2_FILEDIT_H
#include "fim2_filedit.h"
#endif // FIM2_FILEDIT_H
//------------------------------------------------
#include <assert.h>
/*****************************************************************************/

// the default constructor
jfFIMShortDesc_FilterEditor::jfFIMShortDesc_FilterEditor(const jfFilterMap* infmap,
   const jfFIM_ShortDesc_ExprFilter* infilt, QWidget* parent):jfSimpleSFilterEditor(infmap,infilt,parent) {
 // things are handled by the parent
}

//------------------------------------------------------------------------------
// internal methods
jfSimpleExpFilterCore* jfFIMShortDesc_FilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfFIM_ShortDesc_ExprFilter* result = new jfFIM_ShortDesc_ExprFilter(inpval);
  return result;
}
//==========================================================================================
// the default constructor
jfFIMDualDesc_FilterEditor::jfFIMDualDesc_FilterEditor(const jfFilterMap* infmap,
   const jfFIM_DualDesc_ExprFilter* infilt, QWidget* parent):jfSimpleSFilterEditor(infmap,infilt,parent) {
 // things are handled by the parent
}

//------------------------------------------------------------------------------
// internal methods
jfSimpleExpFilterCore* jfFIMDualDesc_FilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfFIM_DualDesc_ExprFilter* result = new jfFIM_DualDesc_ExprFilter(inpval);
  return result;
}

//===========================================================================================
jfFIMGroupSCFilterEditor::jfFIMGroupSCFilterEditor(const jfFilterMap* infmap, const jfFIMGroupSCFilter* infilt,
          QWidget* parent):jfZeroToMaxFilterEditor("Story Count Range",999999,infmap,infilt,parent) {
  // not much
  insert_panel->SetMinMax(0,0);
}

//--------------------------------------------------------------------
jfMinMaxUFilter* jfFIMGroupSCFilterEditor::MakeTypedMinMax() const {
  return new jfFIMGroupSCFilter();
}
//====================================================================
jfFIMGroupMCFilterEditor::jfFIMGroupMCFilterEditor(const jfFilterMap* infmap, const jfFIMGroupMCFilter* infilt,
          QWidget* parent):jfZeroToMaxFilterEditor("Story Count Range",30000,infmap,infilt,parent) {
  // not much
  insert_panel->SetMinMax(0,0);
}

//--------------------------------------------------------------------
jfMinMaxUFilter* jfFIMGroupMCFilterEditor::MakeTypedMinMax() const {
  return new jfFIMGroupMCFilter();
}
/*****************************************************************************/
jfFimThumbPercentFilterEditor::jfFimThumbPercentFilterEditor(const jfFilterMap* infmap,
        const jfFimThumbPercentFilter* infilt, QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfLabeledIntEdit(NULL,"Percentage of Thumbs up", true, 0, 100);
  if (infilt!=NULL) {
    insert_panel->SetValue(infilt->GetPercent());
  }
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  insert_panel->SetValue(85);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfFimThumbPercentFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  const jfFimThumbPercentFilter* xvalue;
  filt_pointer = infilter;
  xvalue = dynamic_cast<const jfFimThumbPercentFilter*>(filt_pointer);
  insert_panel->SetValue(xvalue->GetPercent());
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfFimThumbPercentFilterEditor::GetFilter() {
  // the values
  int tval;
  jfFimThumbPercentFilter* outfilt;
  // getting the value
  tval = insert_panel->GetValue();
  // creating
  outfilt = new jfFimThumbPercentFilter();
  outfilt->SetPercent(tval);
  namedesc_edit->ChangeObj(outfilt);
  return outfilt;
}
//--------------------------------------------------------------
bool jfFimThumbPercentFilterEditor::GeneralCheck() const {
  return true;
}
/*****************************************************************************/
