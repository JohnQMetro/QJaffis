/******************************************************************************
Name    :   listexpr_edit.h
Author  :   John Q Metro
Purpose :   An expression editor for list expression filters
Created :   March 2, 2023
Updated :   March 3, 2023
******************************************************************************/
#ifndef LISTEXPR_EDIT_H
   #define LISTEXPR_EDIT_H
#endif // LISTEXPR_EDIT_H
//-------------------------------------------------------------------------------------
#pragma once

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED

#include "../../../core/filters/extended/list_sexp.h"

#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED

#ifndef WIDGETS4_H
    #include "../../controls/widgets4.h"
#endif // WIDGETS4_H
//----------------------------------------------
#include <QComboBox>
#include <QVBoxLayout>
//****************************************************************************
/* This will have a match type picker, an edit box or line, and a check button.
*/
class jfListExprEdit : public jfExprEditBase {
    Q_OBJECT
  public:
    jfListExprEdit(bool mline_in, QWidget* parent = NULL);
    // match mode
    jfListMatchMode GetMatchMode() const;
    void SetMatchMode(jfListMatchMode match_mode_in);
    // checking method
    jfSimpleExpr* CheckFilter(QString& outmessage);
    virtual bool CheckExpr(const QString& inexpr,bool& outempty,QString& outmessage) const;
  protected:
    QComboBox* match_type_picker;
    QVBoxLayout* right_column;
};
// ===================================================================
/* This is the core filter wrapper for jfListExprEdit */
class jfBaseListExprFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfBaseListExprFilterEditor(const jfFilterMap* infmap, const jfListExprFilterBase* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  protected:
    // internal methods
    virtual jfListExprFilterBase* MakeTypedFilter(jfListMatchMode match_mode, jfSimpleExpr* inpval) = 0;
    // internal data
    jfListExprEdit* insert_panel;
    const jfListExprFilterBase* typed_fpointer;
};
// ================================================================================
/* List expression Filter Editor for jfCharListExprFilter. */
class jfCharacterListExprFilterEditor : public jfBaseListExprFilterEditor {
  public:
    // the default constructor
    jfCharacterListExprFilterEditor(const jfFilterMap* infmap, const jfCharListExprFilter* infilt, QWidget* parent = NULL);
  protected:
    // internal methods
    virtual jfListExprFilterBase* MakeTypedFilter(jfListMatchMode match_mode, jfSimpleExpr* inpval);
};
//****************************************************************************
