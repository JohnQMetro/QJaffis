/******************************************************************************
Name    :   global_filter_editor.cpp
Author  :   John Q Metro
Purpose :   Defines the interface for a local and global filter editor
Created :   August 15, 2009
Separated from filter_editor April 9, 2023
Updated :   April 16, 2023
******************************************************************************/

#include "global_filter_editor.h"
/*
#ifndef UTILS1_H_INCLUDED
  #include "../../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
*/
/*
#ifndef FILTER_UTILS_H_INCLUDED
  #include "filter_utils.h"
#endif // FILTER_UTILS_H_INCLUDED
*/
/*
#ifndef JFILTERGLOBALS
  #include "../../core/filters/filter_globals.h"
#endif
#ifndef JFILTERGROUPS
  #include "../../../core/filters/filter_groups.h"
#endif
*/

//------------------------------------
#include <assert.h>
#include <QMessageBox>
#include <QInputDialog>
/*****************************************************************************/

// methods for jfMapEditPanel
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfMapEditPanel::jfMapEditPanel(jfFilterMap* inlink, QWidget* parent):QWidget(parent) {
  // initial stuff
  itemlink = inlink;
  mappset = !(inlink==NULL);
  // creating the overall holder
  wrapper = new QGroupBox("NULL");
  innerl = new QVBoxLayout();
  wrapper->setLayout(innerl);
  // making the name editor
  ndesce = new jfNameDescEditor(NULL,false,false);
  save_name = new QPushButton("Save Names and Descriptions");
  ndesce->AddItem(save_name);
  // the main editor
  map_editor = new jfFilterMapEditor(NULL,true,NULL);
  // Arranging things
  innerl->addWidget(ndesce,0);
  innerl->addWidget(map_editor,1);
  // done
  outerl = new QVBoxLayout();
  outerl->addWidget(wrapper,1);
  setLayout(outerl);
  connect(save_name, SIGNAL(clicked(bool)), this, SLOT(CatchSaved(bool)));
  if (mappset) ChangeMap(inlink);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// accessible methods
bool jfMapEditPanel::ChangeMap(jfFilterMap* inlink) {
  if (inlink==NULL) return false;
  // we set the name information
  wrapper->setTitle(inlink->GetName());
  ndesce->SetFromFilterMap(inlink);
  // passing the map to the contents editor
  map_editor->ChangeMap(inlink);
  // setting soem values
  itemlink = inlink;
  mappset = true;
  return true;
}
//--------------------------------------------------------------------------
QString jfMapEditPanel::GetName() {
  return ndesce->TryGetName();
}
//--------------------------------------------------------------------------
QString jfMapEditPanel::GetDescription() const {
  return ndesce->GetDesc();
}
//--------------------------------------------------------------------------
void jfMapEditPanel::ChangeBoxName(QString newname) {
  wrapper->setTitle(newname);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfMapEditPanel::CatchSaved(bool cval) {
  emit ThrowSaved();
}

//*****************************************************************************
// --- methods for jfGlobalFilterEditor ----------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfGlobalFilterEditor::jfGlobalFilterEditor(jfMapOfFiltermaps* inlink, QWidget* parent):QWidget(parent) {
  assert(inlink!=NULL);
  maindata = inlink;
  QStringList* mstr;
  // we start with the things in the side panel
  add_btn = new QPushButton("New Map");
  dele_btn = new QPushButton("Delete Map");
  mstr = maindata->GetMapNames();
  if (mstr == NULL) mstr = new QStringList();
  map_list = new QListWidget();
  map_list->addItems(*mstr);
  delete mstr;
  // we start arranging the size panel
  btn_sizer = new QHBoxLayout();
  btn_sizer->addWidget(add_btn,1);
  btn_sizer->addWidget(dele_btn,1);
  side_sizer = new QVBoxLayout();
  side_sizer->addLayout(btn_sizer,0);
  side_sizer->addWidget(map_list,1);
  // the right side has the map editing panel or a blank panel
  blank_panel = new QWidget();
  main_mapedit = new jfMapEditPanel(NULL);
  // we create the topsizer
  top_sizer = new QHBoxLayout();
  top_sizer->addLayout(side_sizer,1);
  // we start with a blank panel, no maps
  main_mapedit->hide();
  top_sizer->addWidget(blank_panel,global_ratio);
  dele_btn->setEnabled(false);
  isblank = true;
  setLayout(top_sizer);
  // then, if we have a map, we load it
  if (!(maindata->empty())) ChangeMap(0);
  // connections
  connect(add_btn, SIGNAL(clicked(bool)), this, SLOT(NewPressed(bool)));
  connect(dele_btn, SIGNAL(clicked(bool)), this, SLOT(DeletePressed(bool)));
  connect(main_mapedit, SIGNAL(clicked(bool)), this, SLOT(ThrowSaved()));
  connect(map_list, SIGNAL(currentRowChanged(int)), this, SLOT(MapPicked()));

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//event handlers
//--------------------------------------------------------------------------
// this event is for making a new, empty filter map
void jfGlobalFilterEditor::NewPressed(bool inval) {
  // constants
  const QString headmsg = "Please enter a name";
  const QString prompt = "Please create a unique, non-blank Group name:";
  // local variables
  QString newname;
  bool cancel;
  bool ok;
  size_t nindex;
  // we start with the generic dialog!
  newname = QInputDialog::getText(this,headmsg,prompt,QLineEdit::Normal,"Filter Group",&ok);
  newname = newname.trimmed();
  cancel = true;
  // the checking loop
  while(true) {
    if (newname.isEmpty()) break;
    else if (maindata->NameExists(newname)) {
      newname = QInputDialog::getText(this,headmsg,"The name is already in use! "+prompt,QLineEdit::Normal,"Filter Group",&ok);
    }
    else {
      cancel = false;
      break;
    }
  }
  // here we are!
  if (!cancel) {
    // making the new map
    nindex = MakeDefaultMap(newname);
    // done
    ChangeMap(nindex);
  }

}
//--------------------------------------------------------------------------
// deletes the currently selected filter group
void jfGlobalFilterEditor::DeletePressed(bool inval) {
  // a special case
  if ((map_list->currentRow())>=0) {
    // we now remove the map from the maindata and list
    maindata->erase(selname);
    QListWidgetItem* doom = map_list->takeItem(selindex);
    delete doom;
    // we pick a new index
    if (maindata->empty()) SetNoMap();
    else {
      if ((maindata->size())>=selindex) selindex--;
      ChangeMap(selindex);
    }
    /* now that the map is removed and a new focus is picked, we
    dispose of the old map */
    selmap->DeleteAll();
    delete selmap;
  }

}
//--------------------------------------------------------------------------
// this event is for saving the name and description of the map
void jfGlobalFilterEditor::SavePressed() {
  // asserts
  assert(!isblank);
  // variables
  size_t newindex;
  QString newname = main_mapedit->GetName();
  bool atest;
  // starting
  if (newname!=selname) {
    // we rename the map
    atest = maindata->RenameMap(selname,newname,newindex) ;
    assert(atest);
    // changing the name in the list box
    QListWidgetItem* doom = map_list->takeItem(selindex);
    delete doom;
    map_list->insertItem(newindex,newname);
    map_list->setCurrentRow(newindex);
    // updating some variables
    selname = newname;
    selindex = newindex;
    main_mapedit->ChangeBoxName(selname);
  }
  selmap->SetDescription(main_mapedit->GetDescription());
}
//--------------------------------------------------------------------------
void jfGlobalFilterEditor::MapPicked() {
  LoadSelectedMap();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// changing the editing panel to the blank one
void jfGlobalFilterEditor::SetNoMap() {
  if (!isblank) {
    top_sizer->removeWidget(main_mapedit);
    main_mapedit->hide();
    blank_panel->show();
    top_sizer->addWidget(blank_panel,global_ratio);
    dele_btn->setEnabled(false);
    isblank = true;
    map_list->setCurrentRow(-1);
    dele_btn->setEnabled(false);
  }
}
//--------------------------------------------------------------------------
// loads the map at the selected in dex
bool jfGlobalFilterEditor::LoadSelectedMap() {
  // a special case
  int q = map_list->currentRow();
  if (q<0) return false;
  else {
    // getting the map...
    selindex = map_list->currentRow();
    selname = map_list->item(selindex)->text();
    selmap = (*maindata)[selname];
    // loading the editor
    main_mapedit->ChangeMap(selmap);
    // now, we see if the main editor is blank!
    if (isblank) {
      top_sizer->removeWidget(blank_panel);
      blank_panel->hide();
      main_mapedit->show();
      top_sizer->addWidget(main_mapedit,global_ratio);
      dele_btn->setEnabled(true);
      isblank = false;
    }
    // done
    return true;
  }
}
//--------------------------------------------------------------------------
// changes the map, mostly consists of setting it and using LoadSelectedMap
bool jfGlobalFilterEditor::ChangeMap(const size_t& index) {
  if (index>=(maindata->size())) return false;
  map_list->setCurrentRow(index);
  return LoadSelectedMap();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creates a map and adds it to the map group
size_t jfGlobalFilterEditor::MakeDefaultMap(QString thename) {
  // variables
  size_t nindex;
  bool atest;
  // we make the map
  (*maindata)[thename] = new jfFilterMap();
  (*maindata)[thename]->SetName(thename);
  // for debugging only
  atest = (*maindata)[thename]->AddFilter(new jfExpressionFilter(thename),nindex);
  assert(atest);
  // test 1
  atest = maindata->FindMapIndex(thename,nindex);
  assert(atest);
  // adding it to the list ctrl
  map_list->insertItem(nindex,thename);
  return nindex;
}
/*****************************************************************************/
