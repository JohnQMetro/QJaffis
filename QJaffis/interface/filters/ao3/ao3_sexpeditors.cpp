//***************************************************************************
// Name:        ao3_sexpeditors.cpp
// Author :     John Q Metro
// Purpose :    Simple expression filter editors specific to archiveofourown.org
// Created:     September 8, 2012
// Conversion to Qt Started October 19, 2013
// Updated:     August 12, 2023
//***************************************************************************
#ifndef AO3_SEXPEDITORS_H_INCLUDED
  #include "ao3_sexpeditors.h"
#endif // AO3_SEXPEDITORS_H_INCLUDED
//-----------------------------------------------
//***************************************************************************
// the default constructor
jfExtraTagFilterEditor::jfExtraTagFilterEditor(const jfExtraTagFilter* infilt,
                QWidget* parent):jfSimpleSFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfExtraTagFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfExtraTagFilter* result = new jfExtraTagFilter(namedesc_edit->TryGetName(), inpval);
  return result;
}
//***************************************************************************
