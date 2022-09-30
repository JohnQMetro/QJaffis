/******************************************************************************
Name    :   filter_match.h
Author  :   John Q Metro
Purpose :   Widget for creating filtered percentage multi-tag-matchers
Created :   August 26, 2022
Updated :   August 26, 2022
******************************************************************************/
#pragma once

#include "multimatch.h"

#include <QWidget>
#include <QVBoxLayout>
/*****************************************************************************/


class jfFilteredTagMatcherEdit : public QWidget {
    Q_OBJECT
  public:
    jfFilteredTagMatcherEdit(bool include_miss_name_input, QWidget* parent = NULL);
    void SetNamesToPickFrom(const QStringList& names);

    // getting data
    jfMultiMatchSource* GetPrefilterData() const;
    jfSimpleExpr* CheckPrefilterExpression(QString& outmessage);
    jfMultiMatchSource* GetMatchData() const;
    jfSimpleExpr* CheckMatchExpression(QString& outmessage);
    size_t GetPercent() const;
    bool IsMatchEmpty() const;
    bool AreExpressionsValid(bool& in_prefilter, QString& outmessage);

    // setting prefilter data
    void SetPrefilterNames(const QStringList& names);
    bool SetPrefilterExpression(const QString& inexpr,QString& outmessage);

    // setting main matcher data
    void SetMatcherNames(const QStringList& names);
    bool SetMatcherExpression(const QString& inexpr,QString& outmessage);
    bool SetPercent(size_t percent_value);
    void SetMatchEmpty(bool value);

  protected:
    QVBoxLayout* main_layout;
    QLabel* prefilter_label;
    jfMultiMatchEditor* prefilter_edit;
    QLabel* match_label;
    jfPercentExprEdit* match_edit;
};

/*****************************************************************************/
