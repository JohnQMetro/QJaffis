/******************************************************************************
Name    :   expr_editor.h
Author  :   John Q Metro
Purpose :   Editors for jfExpressionFiler, and also jfUrlFilter
Created :   March 26, 2009
Conversion to Qt Started Oct 3, 2013
Updated :   February 12, 2018 (Filter Check fixes)
******************************************************************************/
#ifndef EXPR_EDITOR_H_INCLUDED
#define EXPR_EDITOR_H_INCLUDED
#endif // EXPR_EDITOR_H_INCLUDED
//-----------------------------------------------------------------------------
#ifndef JFILTERGROUPS
  #include "../../../core/filters/filter_groups.h"
#endif
#ifndef EXPRESSION_H
  #include "../../../core/filters/base/expression.h"
#endif // EXPRESSION_H

#ifndef JFBASEFILTER
  #include "../../../core/filters/base/filterbase.h"
#endif
#ifndef WIDGETS4_H
    #include "../../controls/widgets4.h"
#endif // WIDGETS4_H
//------------------------------------------------------------------
#include <QWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
//*****************************************************************************
/* The expression editor has an edit box and two buttons: one for inserting
a filter reference (using the filter picker dialog), and the other for checking
the validity of an expression and it's filter links.
Two modes: single and multiple lines

Color coding will be used to determine errors:
0: Green, okay
1: Black, unchecked
2: Red, in error
*/
class jfExpressionEditor : public jfExprEditBase {
    Q_OBJECT
  public:
    jfExpressionEditor(jfFilterMap* inmap, bool notinmap, const bool& multiline, bool in_global, QWidget* parent = NULL);
    // checking method
    jfExpressionFilter* CheckFilter(QString& outmessage);
    virtual bool CheckExpr(const QString& inexpr,bool& outempty, QString& outmessage) const;
  public slots:
    // events
    void PressPickFilter(bool checked);
  protected:
    // additional internal controls
    QPushButton* finsert_btn;
    // sizers (horizontal, buttons on the right)
    // in multiline mode
    QVBoxLayout* rightsizer;
    // linked reference data
    jfFilterMap* local_fmap;
    bool map_nomap;
    bool isglobal;
};
//*****************************************************************************
