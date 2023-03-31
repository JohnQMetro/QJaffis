/******************************************************************************
Name    :   multimatch.h
Author  :   John Q Metro
Purpose :   Widget for creating multi-tag-matchers
Created :   August 18, 2022
Updated :   August 28, 2022
******************************************************************************/
#pragma once

#ifndef WIDGETS1_H_INCLUDED
  #include "../../controls/widgets1.h"
#endif // WIDGETS1_H_INCLUDED

#ifndef WIDGETS3_H_INCLUDED
  #include "../../controls/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef WIDGETS4_H
    #include "../../controls/widgets4.h"
#endif // WIDGETS4_H

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED

#include "../../../core/tagfilter/multimatchmake.h"
// --------------------------------------------
#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QList>
#include <QCheckBox>
#include <QPushButton>

/*****************************************************************************/
class jfMultiMatchEditor : public jfExprEditBase {
    Q_OBJECT
  public:
    jfMultiMatchEditor(bool include_miss_name_input,QWidget* parent = NULL);

    void SetNamesToPickFrom(const QStringList& names);
    QStringList GetPickedNames() const;
    size_t SetPickedNames(const QStringList& names);
    jfSimpleExpr* CheckFilter(QString& outmessage);
    jfMultiMatchSource* getMatchData() const;
    virtual bool CheckExpr(const QString& inexpr,bool& outempty,QString& outmessage) const;
  protected slots:
    void HandleMissReport();
  protected:
    void SetupMissInput(bool yes);

    QStringList tagset_list;
    jfCheckList* tagset_picker;
    QVBoxLayout* right_column;

    bool miss_report_allowed;
    QHBoxLayout* miss_row;
    QLineEdit* miss_name_input;
    QPushButton* miss_report;
    mutable jfStringCounter* miss_counter;
};
// ==========================================================================
class jfPercentExprEdit : public jfMultiMatchEditor {
  public:
    jfPercentExprEdit(bool include_miss_name_input, QWidget* parent = NULL);
    // checking method
    size_t GetPercent() const;
    bool SetPercent(size_t percent_value) const;
    bool IsMatchEmpty() const;
    void SetMatchEmpty(bool value);
  protected:
    jfLabeledIntEdit* percent_editor;
    QCheckBox* match_empty;

};
/*****************************************************************************/
