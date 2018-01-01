/******************************************************************************
Name    :   urlfilter_editor.h
Author  :   John Q Metro
Purpose :   Editors for jfExpressionFiler, and also jfUrlFilter
Created :   July 8, 2016 (split from expr_editor.h)
Conversion to Qt Started Oct 3, 2013
Updated :   July 8, 2016
******************************************************************************/
#ifndef URLFILTER_EDITOR_H
  #define URLFILTER_EDITOR_H
#endif // URLFILTER_EDITOR_H
//-----------------------------------------------------------------------------
#ifndef JFBASEFILTER
  #include "../../../core/filters/base/filterbase.h"
#endif
//------------------------------------------------------------------
#include <QWidget>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QBoxLayout>

//****************************************************************************
/* The url filter has two multiline Text control boxes, one to list alternate
includes, the other for excludes, and a checkbox to disbale the first. Another
checkbox controls whether https protocol is counted when applying the filter
*/
class jfUrlFilEdit : public QWidget {
    Q_OBJECT
  public:
    jfUrlFilEdit(bool side_arrangement, QWidget* parent = NULL);
    // getting data
    jfUrlFilter* GetData() const;
    bool isEmpty() const;
    // setting data
    bool SetData(const jfUrlFilter* indata);
  public slots:
    void ExcludesOnlyChecked(int state);
  protected:
    QStringList* TextCtrl2List(QPlainTextEdit* inedit) const;
    // the lists
    QPlainTextEdit* alt_incls;
    QPlainTextEdit* excludes;
    // the checkboxes
    QCheckBox* case_insensetive;
    QCheckBox* excludes_only;
    QCheckBox* https_counted;
    // text labels
    QLabel* alt_labl;
    QLabel* excl_labl;
    // sizers
    QBoxLayout* totalsizer;
    QVBoxLayout* left_sizer;
    QVBoxLayout* right_sizer;
    bool sidearr;
};

//*****************************************************************************

