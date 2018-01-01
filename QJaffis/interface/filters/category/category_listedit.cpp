/******************************************************************************
// Name:        category_listedit.cpp
// Author :     John Q Metro
// Purpose :    Split off from category_editor, handles multipl category edits
// Created:     October 23, 2009
// Conversion to Qt started October 22, 2013
// Updated:     January 1, 2011
******************************************************************************/
#ifndef CATEGORY_LISTEDIT_H_INCLUDED
  #include "category_listedit.h"
#endif // CATEGORY_LISTEDIT_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-----------------------------------------------
#include <assert.h>
#include <QMessageBox>
#include <QInputDialog>
//==============================================================================
// the constructor
jfCategoryEditorsNotebook::jfCategoryEditorsNotebook(jfCategories* in_datalink, QWidget* parent):QWidget(parent) {
  // we start
	const QString fname = "jfCategoryEditorsNotebook::jfCategoryEditorsNotebook";
  // local variables
  size_t ccount,cloop,ncloop;
  // setting the default values
  defcat_proxy = NULL;
  incl_default = false;
  // first, we check and assign the categories data object
  assert(in_datalink!=NULL);
  datalink = in_datalink;
  // we create the top-level sizer and set it
  mainsizer = new QHBoxLayout();
  mainbook = new QTabWidget();
  mainsizer->addWidget(mainbook,1);
  // we now load the categories
  ccount = datalink->GetCount();
  for (cloop=0;cloop<ccount;cloop++) {
    catedits[cloop] = new jfCategoryEditor(datalink,cloop,true);
    mainbook->addTab(catedits[cloop],datalink->NameAtIndex(cloop));
  }
  // for the remainder, we set null
  for (ncloop=ccount;ncloop<32;ncloop++) catedits[ncloop] = NULL;
  // done
  for (cloop=0;cloop<ccount;cloop++) {
    connect(catedits[cloop], SIGNAL(jfCategoryEditor::SendDelete(size_t)), this, SLOT(ProcessDelete(size_t)));
  }
  setLayout(mainsizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//---------------------------------------------------------------------------
bool jfCategoryEditorsNotebook::DeleteCategory(size_t cindex) {
  size_t ccount,cloop;
	const QString fname = "jfCategoryEditorsNotebook::DeleteCategory";
  // starting up...
  ccount = datalink->GetCount();
  assert(cindex<ccount);
  // we then remove and destroy associated editor
  mainbook->removeTab(cindex);
  // we delete the physical data object
  datalink->DeleteAtIndex(cindex);
  delete catedits[cindex];
  // we re-assign the indexes for categories after the deleted one
  for (cloop=(cindex+1);cloop<ccount;cloop++) {
    catedits[cloop-1] = catedits[cloop];
    catedits[cloop-1]->ChangeCategoryIndex(cloop-1);
  }
  catedits[ccount-1] = NULL;
  // redrawing
  // done
  return true;
}
//---------------------------------------------------------------------------
bool jfCategoryEditorsNotebook::SaveAll() {
  const QString verif_error = "Error in ";
  const QString fname = "jfCategoryEditorsNotebook::SaveAll";
  // local variables
  size_t ccount,cloop;
  bool tres;
  QString err_string, outstring;
  QMessageBox emsg;
  // starting
  ccount = datalink->GetCount();
  // the loop
  for (cloop=0;cloop<ccount;cloop++) {
    tres = catedits[cloop]->SaveData(err_string);
    if (!tres) {
      outstring = verif_error + catedits[cloop]->MakeMainLabel() + ":\n";
      outstring += err_string;
      emsg.setIcon(QMessageBox::Critical);
      emsg.setText(outstring);
      emsg.setWindowTitle("Expression Error!");
      emsg.exec();
      return false;
    }
  }
  return true;
}
//---------------------------------------------------------------------------
bool jfCategoryEditorsNotebook::CategoryAdded() {
	// constants
	const QString fname = "jfCategoryEditorsNotebook::CategoryAdded";
	// variables
  size_t ccount;
  // we start
  ccount = datalink->GetCount();
  assert(catedits[ccount-1]==NULL);
  // making the category
  catedits[ccount-1] = new jfCategoryEditor(datalink,ccount-1,true);
  // adding it to the sizer
  mainbook->addTab(catedits[ccount-1],datalink->NameAtIndex(ccount-1));
  return true;
}
//------------------------------------------------------------------------
void jfCategoryEditorsNotebook::UnEnable(bool doit) {
  size_t cloop,ccount;
  ccount = datalink->GetCount();
  // the loop
  for (cloop=0;cloop<ccount;cloop++) {
    catedits[cloop]->UnEnable(doit);
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// change default
//----------------------------------
bool jfCategoryEditorsNotebook::SetToInclDefault(jfDefaultCategoryEditor* in_defcatproxy) {
  // some tests
  if (incl_default) return false;
  if (in_defcatproxy==NULL) return false;
  // going past the tests
  defcat_proxy = in_defcatproxy;
  incl_default = true;
  // adding to the notebook
  mainbook->insertTab(0,in_defcatproxy,"Default");
  // done
  return true;
}
//----------------------------------
bool jfCategoryEditorsNotebook::SetToExclDefault() {
  if (!incl_default) return false;
  // doing
    mainbook->removeTab(0);
    defcat_proxy = NULL;
    incl_default = false;
    return false;
}
//------------------------------------------------------
QTabWidget* jfCategoryEditorsNotebook::GetNotebookPointer() const {
  return mainbook;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handlers
void jfCategoryEditorsNotebook::ProcessDelete(size_t curr_index) {
  DeleteCategory(curr_index);
  emit SendDelete(curr_index);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// no checking done here
size_t jfCategoryEditorsNotebook::GIndex(size_t inindex) const {
  return (incl_default)?(inindex+1):(inindex);
}
//============================================================================
// the constructor
jfDefaultCategoryEditor::jfDefaultCategoryEditor(jfCategories* in_datalink, QWidget* parent):QWidget(parent) {
  // setting the links
  assert(in_datalink!=NULL);
  datalink = in_datalink;
  // the group box and inner layout
  gbox = new QGroupBox("Default Catgeory");
  inlayout = new QVBoxLayout();
  gbox->setLayout(inlayout);
  // name and description editor
  ndesc_edit = new jfNameDescEditor(NULL,false,false,datalink->GetName(),datalink->GetDescription());
  // making the filepicker
  filepicker = new jfRelativeFilePicker(datalink,32,false);
  // arranging within the top level
  inlayout->addWidget(ndesc_edit,0);
  inlayout->addWidget(filepicker,0);
  // by default, the default catgeory is disbaled
  use_default = new QCheckBox("Use remainder as an extra Category");
  SetDefaultEnable(false);
  // finishing sizers
  outlayout = new QVBoxLayout();
  outlayout->addWidget(use_default,0,Qt::AlignLeft);
  outlayout->addWidget(gbox,1);
  // value setting
  SetDefaultEnable(datalink->UsesDefault());
  use_default->setChecked(datalink->UsesDefault());
  // last
  connect(use_default, SIGNAL(stateChanged(int)), this, SLOT(ProcessCheckDefault(int)));
  setLayout(outlayout);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// methods
//---------------------------------------------------
void jfDefaultCategoryEditor::SetDefaultEnable(bool xenable) {
  ndesc_edit->setEnabled(xenable);
  filepicker->setEnabled(xenable);
}
//---------------------------------------------------
void jfDefaultCategoryEditor::SaveDefault() {
  datalink->SetName(ndesc_edit->TryGetName());
  datalink->SetDescription(ndesc_edit->GetDesc());
  datalink->SetUseDefault(use_default->isChecked());
}
//------------------------------------------------------
void jfDefaultCategoryEditor::EnableCheck(bool value) {
  use_default->setEnabled(value);
}
//---------------------------------------------------------
void jfDefaultCategoryEditor::ReloadNames() {
	ndesc_edit->SetFromObj(datalink);
	filepicker->ReloadNames();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// events
void jfDefaultCategoryEditor::ProcessCheckDefault(int state) {
  if (!(use_default->isChecked())) {
    SaveDefault();
    SetDefaultEnable(false);
  }
  else SetDefaultEnable(true);
}
//=============================================================================
// the constructor for the top-level catgeories editing panel
jfMainCategoriesEditor::jfMainCategoriesEditor(jfCategories* in_datalink, QWidget* parent):QWidget(parent) {
  // constants
	const QString fname="jfMainCategoriesEditor::jfMainCategoriesEditor";
  // setting up basic stuff
  assert(in_datalink!=NULL);
  datalink = in_datalink;
	vertical = true;
  // next up, we have the box which contains the proper category editor
  mbox = new QGroupBox("Category Editing Notebook");
  innerlayout = new QVBoxLayout();
  mbox->setLayout(innerlayout);
		// the default category editor
    dfcat_panel = new jfDefaultCategoryEditor(datalink);
    // making the widgets
    mainlist_panel = new jfCategoryEditorsNotebook(datalink,this);
    addcat_btn = new QPushButton("Add Category");
    // the sizer, and arranging
    innerlayout->addWidget(addcat_btn,0,Qt::AlignLeft);
    innerlayout->addWidget(mainlist_panel,1);
  // making the explanantory labels that go at the top
    QString desclabel = "Categories are used to sort the results of a ";
    desclabel += "search after filtering) into multiple groups, each of";
    desclabel += " which has a separate results display window and ";
    desclabel += "results file. An expression  for each category is used";
    desclabel += " to match results to determine if it belongs to that ";
    desclabel += "that catgory. Individual results can belong to more ";
    desclabel += "than 1 category. A maximum of 32 categories are ";
    desclabel += "allowed, but you can treat whatever might be left over";
    desclabel += " as another category.";
    mainlabel = new QLabel("Category Editing");
    maindesc = new QLabel(desclabel);
    maindesc->setWordWrap(true);
  // making the toplevel sizer and arrangine things
    topsizer = new QVBoxLayout();
    topsizer->addWidget(mainlabel,0);
    topsizer->addWidget(maindesc,0);
    topsizer->addWidget(dfcat_panel,0);
    topsizer->addWidget(mbox,1);
    setLayout(topsizer);
    // connections
    connect(addcat_btn, SIGNAL(clicked(bool)), this, SLOT(ProcessAddBtn(bool)));
    connect(mainlist_panel, SIGNAL(SendDelete(size_t)), this, SLOT(ProcessDeleteBtn(size_t)));
    ChangeToHorizontal();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfMainCategoriesEditor::SaveCategories() {
  dfcat_panel->SaveDefault();
  return mainlist_panel->SaveAll();
}
//---------------------------------------------------------------------------
void jfMainCategoriesEditor::SetUnEditable(bool unedit) {
  dfcat_panel->EnableCheck(!unedit);
  addcat_btn->setEnabled(!unedit);
  mainlist_panel->UnEnable(unedit);
}
//--------------------------------------------------------------------------
bool jfMainCategoriesEditor::ChangeSearchName(QString sname_in) {
  // constants and asserts
  const QString fname = "jfMainCategoriesEditor::ChangeSearchName";
	assert(datalink!=NULL);
	// changing the data
	bool rval = datalink->ChangeDefaultFName(sname_in);
	if (!rval) return false;
	// updating the views
	dfcat_panel->ReloadNames();
	// done
	return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
//--------------------------------------------------------------------------
/* we have 2 configurations here, vertical, where the default category info is
in a separate panel above the notebook of category editors, and horizontal, where
the editor for the default category is the first tab in the notebook */
bool jfMainCategoriesEditor::ChangeToHorizontal() {
  if (!vertical) return false;
  // creating the new default panel
  jfDefaultCategoryEditor* newedit,*oldedit;
  newedit = new jfDefaultCategoryEditor(datalink);
  mainlist_panel->SetToInclDefault(newedit);
  // removing the old panel
  oldedit = dfcat_panel;
  topsizer->removeWidget(dfcat_panel);
  dfcat_panel = newedit;
  // destroying the old panel
  delete oldedit;
  // finishing
  vertical = false;
  return true;
}
//--------------------------------------------------------------------------
bool jfMainCategoriesEditor::ChangeToVertical() {
  if (vertical) return false;
  // creating the new panel
  jfDefaultCategoryEditor* newedit,*oldedit;
  newedit = new jfDefaultCategoryEditor(datalink);
  // removing the old panel
  oldedit = dfcat_panel;
  mainlist_panel->SetToExclDefault();
  // inserting the new panel
  dfcat_panel = newedit;
  topsizer->insertWidget(2,dfcat_panel,0);
  // destroying the old panel
  delete oldedit;
  // finishing
  vertical = true;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handlers
//----------------------------------------------------------------------------
/* most of deletion is handles at the categories editor list, but we do have
this here since the Add Button can be disabled if the number of categories is
at 32 */
void jfMainCategoriesEditor::ProcessDeleteBtn(size_t delindex) {
  addcat_btn->setEnabled(true);
}
//----------------------------------------------------------------------------
void jfMainCategoriesEditor::ProcessAddBtn(bool inval) {
  const QString pen="Please enter a non-blank name for the Category";
  const QString msgcap="Make a Category Name";
  QString result;
  bool inok;
  // getting the name from the user
  result =  QInputDialog::getText(this,msgcap,pen,QLineEdit::Normal,"New_Category",&inok);
  if (!result.isEmpty()) {
		// now, we add the category to the collections list
		datalink->AddNewCategory(result);
		// and we tell the categories editing list to add it
		mainlist_panel->CategoryAdded();
		// also, we check if we have reached the category max size
    if ((datalink->GetCount())==32) addcat_btn->setEnabled(false);
	}
}
//*****************************************************************************
