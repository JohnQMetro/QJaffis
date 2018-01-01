//***************************************************************************
// Name:        ao3_sexpeditors.cpp
// Author :     John Q Metro
// Purpose :    Simple expression filter editors specific to archiveofourown.org
// Created:     September 8, 2012
// Conversion to Qt Started October 19, 2013
// Updated:     April 7, 2013
//***************************************************************************
#ifndef AO3_SEXPEDITORS_H_INCLUDED
  #include "ao3_sexpeditors.h"
#endif // AO3_SEXPEDITORS_H_INCLUDED
//-----------------------------------------------
//***************************************************************************
// the default constructor
jfExtraTagFilterEditor::jfExtraTagFilterEditor(const jfFilterMap* infmap, const jfExtraTagFilter* infilt,
                QWidget* parent):jfSimpleSFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating a filter from a simple expression
jfSimpleExpFilterCore* jfExtraTagFilterEditor::MakeTypedFilter(jfSimpleExpr* inpval) {
  jfExtraTagFilter* result = new jfExtraTagFilter(inpval);
  return result;
}
//***************************************************************************
