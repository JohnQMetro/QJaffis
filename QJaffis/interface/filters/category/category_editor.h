/******************************************************************************
// Name:        category_editor.h
// Author :     John Q Metro
// Purpose :    Declares category editing interface stuff
// Created:     April 1, 2009
// Conversion to Qt Started Oct 9, 2013
// Updated:     October 29, 2011
******************************************************************************/
#ifndef CATEGORY_EDITOR_H_INCLUDED
#define CATEGORY_EDITOR_H_INCLUDED
#endif // CATEGORY_EDITOR_H_INCLUDED
//-----------------------------------------------------------------------------
#ifndef WIDGETS1_H_INCLUDED
  #include "../../controls/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
#ifndef EXPR_EDITOR_H_INCLUDED
  #include "../base/expr_editor.h"
#endif // EXPR_EDITOR_H_INCLUDED
#ifndef CATEGORIES_H_INCLUDED
  #include "../../../core/structs/categories.h"
#endif // CATEGORIES_H_INCLUDED
//-----------------------------------
#include <QGroupBox>
//****************************************************************************
/* for filename setting, we have a special panel. We use the standard file
picker dialog (in save mode) to pick where to store the result file. By default
(unless the Absolute Path checkbox is selected) this path is then converted into
a path relative to the base stored in jfCategories. This is to allow changing
the location where all the results will be stores by just changing the base.
*/
class jfRelativeFilePicker : public QWidget {
      Q_OBJECT
  public:
    // the default
    jfRelativeFilePicker(jfCategories* catparent, size_t cateindex, bool vertical, QWidget* parent = NULL);
    // a helper method
    void ChangeCategoryIndex(size_t newindex);
    QString GetPath() const;
    bool IsAbsolute() const;
		void ReloadNames();
  public slots:
    void ProcessFnameCheck(int state);
    void ProcessFnameBtn(bool checked);
  protected:
    // local widgets
    QLabel *frlabel;
    QLabel *filepath;
    QCheckBox *pickabsolute;
    QPushButton *select_file;
    // sizers
    QHBoxLayout *filepath_sizer;
    QHBoxLayout *line2sizer;
    QBoxLayout *topsizer;
    // referenced data
    jfCategories* datalink;
    size_t this_index;
};
//****************************************************************************
/* the category ediotor has, arranged top to bottom:
-an editor for the category name and a delete button
- a description editor
- the expression editor (jfExpressionEditor)
- file name setting.

For the filename/path, it is either absolute or relative to an extenally
proveded path. This is to allow the results to be placed particular directories
which can be relative to a base.

*/
class jfCategoryEditor : public QWidget {
    Q_OBJECT
  public:
    // the default constructor
    jfCategoryEditor(jfCategories* catparent, size_t cateindex, bool multiline, QWidget* parent = NULL);
    // helper methods
    void ChangeCategoryIndex(size_t newindex);
    bool SaveData(QString& outmessage);
    QString MakeMainLabel() const;
		QString GetExpName() const;
		QString GetExpStr() const;
		void UnEnable(bool doit = true);
  public slots:
    void ProcessDeleteBtn(bool checked);
  signals :
    void SendDelete(size_t curr_index);
  protected:
    // top parts widgets
    jfNameDescEditor* namedesc;
    QPushButton* del_btn;
    // the expression editor
    QLabel* expr_label;
    jfExpressionEditor* expr_edit;
    // the filepicker
    jfRelativeFilePicker* filepicker;
    // top-level sizer
    QGroupBox* mbox;
    QVBoxLayout *innerl, *outerl;
    // internal data
    jfCategories* datalink;
    jfExpressionFilter* currdata;
    size_t this_index;
};

//****************************************************************************
