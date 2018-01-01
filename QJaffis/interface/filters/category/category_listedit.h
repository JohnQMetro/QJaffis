/******************************************************************************
// Name:        category_listedit.h
// Author :     John Q Metro
// Purpose :    Split off from category_editor, handles multipl category edits
// Created:     October 23, 2009
// Conversion to Qt started October 22, 2013
// Updated:     November 17, 2011
******************************************************************************/
#ifndef CATEGORY_LISTEDIT_H_INCLUDED
#define CATEGORY_LISTEDIT_H_INCLUDED
#endif // CATEGORY_LISTEDIT_H_INCLUDED
#ifndef CATEGORY_EDITOR_H_INCLUDED
  #include "category_editor.h"
#endif // CATEGORY_EDITOR_H_INCLUDED
//--------------------------------------------------------------------
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
//=========================================================================
class jfDefaultCategoryEditor;
/* I'm having definite problems with getting jfCategoryEditorsList to scroll,
so time the try the notebook verion! */
class jfCategoryEditorsNotebook : public QWidget {
    Q_OBJECT
  public:
    jfCategoryEditorsNotebook(jfCategories* in_datalink, QWidget* parent = NULL);
    // helper methods
    bool DeleteCategory(size_t cindex);
    bool SaveAll();
    bool CategoryAdded();
    void UnEnable(bool doit=true);
    // change default
    bool SetToInclDefault(jfDefaultCategoryEditor* in_defcatproxy);
    bool SetToExclDefault();
    QTabWidget* GetNotebookPointer() const;
  public slots:
    void ProcessDelete(size_t curr_index);
  signals:
    void SendDelete(size_t curr_index);
  protected:
    // helper methods
    size_t GIndex(size_t inindex) const;
    // gui elements
    QTabWidget* mainbook;
    jfCategoryEditor* catedits[32];
    QHBoxLayout* mainsizer;
    // default (if available)
    bool incl_default;
    jfDefaultCategoryEditor* defcat_proxy;
    // internal elements
    jfCategories* datalink;

};
//==========================================================================
/* Panel with controls to edit the default category. I've separated this one
out because as it takes up too much space on small screens */
class jfDefaultCategoryEditor : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfDefaultCategoryEditor(jfCategories* in_datalink, QWidget* parent = NULL);
    // methods
    void SetDefaultEnable(bool xenable=true);
    void SaveDefault();
    void EnableCheck(bool value=true);
    void ReloadNames();
public slots:
    void ProcessCheckDefault(int state);
  protected:
    // internal methods
    // gui controls for default stuff
      QCheckBox* use_default;
      QGroupBox* gbox;
      QVBoxLayout *inlayout, *outlayout;
      // name and description editor
      jfNameDescEditor* ndesc_edit;
      // the filepicker
      jfRelativeFilePicker* filepicker;
    // links
      jfCategories* datalink;
};

//===========================================================================
/* This is the main top level categories editor, with the editor list, some
controls to edit the default category, an add button, and some labels
*/
class jfMainCategoriesEditor : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfMainCategoriesEditor(jfCategories* in_datalink, QWidget* parent = NULL);
    // helper methods
    void SetDefaultEnable(bool xenable=true);
    bool SaveCategories();
    void SetUnEditable(bool unedit = true);
		bool ChangeSearchName(QString sname_in);
		// special methods
		bool ChangeToHorizontal();
		bool ChangeToVertical();
public slots:
    void ProcessDeleteBtn(size_t delindex);
    void ProcessAddBtn(bool inval);
  protected:
    // the linked data
    jfCategories* datalink;
    bool vertical;
    // default category
    jfDefaultCategoryEditor* dfcat_panel;
    // next up is the proper catgeories section
      QGroupBox* mbox;
      jfCategoryEditorsNotebook *mainlist_panel;
      QPushButton* addcat_btn;
      // labels at the top
      QLabel *mainlabel;
      QLabel* maindesc;
      QVBoxLayout* topsizer, *innerlayout;
};
