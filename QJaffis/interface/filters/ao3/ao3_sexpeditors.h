//***************************************************************************
// Name:        ao3_sexpeditors.h
// Author :     John Q Metro
// Purpose :    Simple expression filter editors specific to archiveofourown.org
// Created:     September 8, 2012
// Conversion to Qt Started October 19, 2013
// Updated:     April 7, 2013
//***************************************************************************
#ifndef AO3_SEXPEDITORS_H_INCLUDED
#define AO3_SEXPEDITORS_H_INCLUDED
#endif // AO3_SEXPEDITORS_H_INCLUDED
//-----------------------------------------------
#ifndef AO3_STRINGFILTERS_H_INCLUDED
  #include "../../../core/filters/ao3/ao3_stringfilters.h"
#endif // AO3_STRINGFILTERS_H_INCLUDED
#ifndef SFILT_EDIT_H_INCLUDED
  #include "../base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
//***************************************************************************
/* a wrapper for jfSimpleExprEdit that is for an character filter */
class jfExtraTagFilterEditor : public jfSimpleSFilterEditor {
  public:
    // the default constructor
    jfExtraTagFilterEditor(const jfExtraTagFilter* infilt, QWidget* parent);
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval);
};
//***************************************************************************
