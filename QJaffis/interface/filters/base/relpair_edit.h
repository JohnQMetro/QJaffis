/******************************************************************************
Name    :   relpair_edit.h
Author  :   John Q Metro
Purpose :   GUI Editing for Structured Pair Filter
Created :   February 24, 2023
Created :   February 25, 2023
******************************************************************************/
#pragma once

#ifndef RELPAIR_EDIT_H
   #define RELPAIR_EDIT_H
#endif // RELPAIR_EDIT_H

#include "../../../core/filters/base/structpair.h"

#ifndef BFILT_EDIT_H_INCLUDED
  #include "bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
//---------------------------------
#include <QWidget>
#include <QString>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
/*****************************************************************************/
class jfStrPairingPanel : public QWidget {
  public:
    // constructor
    jfStrPairingPanel(QWidget* parent = NULL);
    // i/o
    void SetFromString(const QString& inval);
    void SetMatchAllCheck(bool cval);
    void SetReaderCheck(bool cval);
    void SetOriginalCheck(bool cval);
    QString GetStringValue() const;
    bool GetMatchAllCheck() const;
    bool GetReaderCheck() const;
    bool GetOriginalCheck() const;
    // filter based i/o
    jfStructuredPairFilter* GetFilter() const;
    bool StoreToFilter(jfStructuredPairFilter* outval);
    bool LoadFromFilter(const jfStructuredPairFilter* inval);
  protected:
    // GUI data
    QCheckBox* matchallbox;
    QCheckBox* readerbox;
    QCheckBox* originalbox;
    QLabel* listheader;
    QPlainTextEdit* pairlistbox;
    QVBoxLayout* bsizer;
};
// ======================================================================== /
// ao3 ratings filter editor
class jfStr_PairingFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfStr_PairingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfStrPairingPanel* insert_panel;
};
/*****************************************************************************/
