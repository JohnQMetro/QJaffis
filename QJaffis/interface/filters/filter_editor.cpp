/******************************************************************************
Name    :   filter_editor.cpp
Author  :   John Q Metro
Purpose :   Defines the interface for a local and global filter editor
Created :   August 15, 2009
Conversion to Qt Started Oct 24, 2013
Updated :   March 11, 2013 (Save on change filter)
******************************************************************************/
#ifndef FILTER_EDITOR_H_INCLUDED
  #include "filter_editor.h"
#endif // FILTER_EDITOR_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef FILTER_UTILS_H_INCLUDED
  #include "filter_utils.h"
#endif // FILTER_UTILS_H_INCLUDED
#ifndef JFILTERGLOBALS
  #include "../../core/filters/filter_globals.h"
#endif
#ifndef JFILTERGROUPS
  #include "../../../core/filters/filter_groups.h"
#endif
//------------------------------------
#include <assert.h>
#include <QMessageBox>
#include <QInputDialog>
/*****************************************************************************/
//============================================================================
// the constructor for this picker
jfFilterPicker2::jfFilterPicker2(QWidget* parent):jfFilterPickerBase(false,parent) {
  // not much to do here
  sloaded = false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// item manipulation
//---------------------------------------------------------------------------
// inserts a name at the specified index
bool jfFilterPicker2::InsertAtIndex(const size_t& nindex, const jfBaseFilter* infilt) {
  const QString fname = "jfFilterPicker2::InsertAtIndex";
  // variables
  QTableWidgetItem* titem;
  // checks
  /**/JDEBUGLOG(fname,1);
  if (infilt==NULL) return false;
  if (nindex>rowCount()) return false;
  /**/JDEBUGLOGST(fname,2,nindex);
  // we start with the insert
  insertRow(nindex);
  titem = new QTableWidgetItem(infilt->GetName());
  /**/JDEBUGLOG(fname,3);
  setItem(nindex,0,titem);
  titem = new QTableWidgetItem(infilt->GetTypeID());
  setItem(nindex,1,titem);
  /**/JDEBUGLOGS(fname,4,infilt->GetTypeID());
  // if the new index is less than selected, we increment the selected index
  if (nindex<=select_index) select_index++;
  // done
  return true;
}
//---------------------------------------------------------------------------
/* we delete the current selected item, and use InsertAtIndex to place the
new selected item onto the list */
bool jfFilterPicker2::ReplaceSelected(const size_t& nindex, const jfBaseFilter* infilt) {
  const QString fname = "jfFilterPicker2::ReplaceSelected";
  // checks
  /**/JDEBUGLOG(fname,1);
  if (infilt==NULL) return false;
  if (nindex>=rowCount()) return false;
  if (!IsFilterSelected()) return false;
  /**/JDEBUGLOG(fname,2);
  // we delete the selected filter
  removeRow(select_index);
  /**/JDEBUGLOG(fname,3);
  // add the new one
  if (!InsertAtIndex(nindex,infilt)) return false;
  // setting the index
  /**/JDEBUGLOG(fname,4);
  return ChangeSelected(nindex);
}
//-----------------------------------------------------------------------------
bool jfFilterPicker2::RenameItem(size_t oldindex, size_t newindex, QString newname) {
  // variables
  QString typeval;
  long sval;
  bool issval;
  int del = 0;
  long lbackval;
  QTableWidgetItem* titem;
  // special checks
  if (oldindex>=rowCount()) return false;
  if (newindex>=rowCount()) return false;
  if (newname.isEmpty()) return false;
  // special case
  if (oldindex==newindex) {
    item(oldindex,0)->setText(newname);
    return true;
  }
  // otherwise
  typeval = item(oldindex,1)->text();
  // checking the selected value
  sval = select_index;
  issval = (sval==oldindex);
  // calculating the selected index shift
  if (!issval) {
    if (oldindex<newindex) {
      if (sval<=newindex) del = -1;
    }
    else {
      if (sval<newindex) del = 1;
    }
  }
  // removing the old item, inserting the new
  removeRow(oldindex);
  insertRow(newindex);
  titem = new QTableWidgetItem(newname);
  setItem(newindex,0,titem);
  titem = new QTableWidgetItem(typeval);
  setItem(newindex,1,titem);
  // changing the selection if need be
  if (del!=0) {
    lbackval = old_selection;
    SelectNoEvent(sval+del,true);
    old_selection = lbackval;
  }
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// replace at index
bool jfFilterPicker2::ReplaceAtIndex(size_t oldindex, size_t newindex, const jfBaseFilter* infilt) {
  // special checks
  if (oldindex>=rowCount()) return false;
  if (newindex>=rowCount()) return false;
  if (infilt==NULL) return false;
  // we delete the oldindex filter
  removeRow(oldindex);
  // add the new one
  if (!InsertAtIndex(newindex,infilt)) return false;
  SetSelected();
  // done
  return true;
}
//=============================================================================
// the constructor
jfStartFilterDialog::jfStartFilterDialog(const QString& startname, const QStringList* inpflist, QWidget* parent): QDialog(parent) {
  // asserts
  assert(!startname.isEmpty());
  assert(inpflist!=NULL);
  // the name
  name_entry = new jfLabeledEdit(NULL,"New Filter Name",true,startname);
  // the label for the type picker
  picker_label = new QLabel("Filter Type:");
  // the type picker
  ftchoices = inpflist;
  type_picker = new QComboBox();
  type_picker->addItems(*ftchoices);
  // the buttons
  ok_btn = new QPushButton("OK");
  can_btn = new QPushButton("Cancel");
  button_sizer = new QHBoxLayout();
  button_sizer->addWidget(ok_btn,0);
  button_sizer->addWidget(can_btn,0);

  // we set up the sizers
  top_sizer = new QVBoxLayout();
  top_sizer->addWidget(name_entry,0);
  top_sizer->addSpacing(3);
  top_sizer->addWidget(picker_label,0);
  top_sizer->addWidget(type_picker,0);
  top_sizer->addLayout(button_sizer,0);
  // the last things
  setLayout(top_sizer);
  connect(ok_btn, SIGNAL(clicked(bool)), this, SLOT(OKPressed(bool)));
  connect(can_btn, SIGNAL(clicked(bool)), this, SLOT(reject()));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting results
//----------------------------------------
QString jfStartFilterDialog::GetNewName() const {
  return newname.trimmed();
}
//----------------------------------------
size_t jfStartFilterDialog::GetResIndex() const {
  return typeindex;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handler
//---------------------------------------------------------------------------
void jfStartFilterDialog::OKPressed(bool checked) {
  newname = name_entry->GetText();
  typeindex = type_picker->currentIndex();
  accept();
}
//=================================================================================
// the default constructor
jfFilterMapEditor::jfFilterMapEditor(jfFilterMap* inlink,bool in_global, const QStringList* inpflist, QWidget* parent):QWidget(parent) {
  // starting
  fdata = inlink;
  is_inglobal = in_global;
  if (!in_global) {
    assert(inpflist!=NULL);
    pflist = inpflist;
  }
  else pflist = filterglobals::fclist[0];
  // button creation
  new_button = new QPushButton("New...");
  del_button = new QPushButton("Delete");
  save_button = new QPushButton("Save");
  // sticking the buttons in the sizer
  btn_sizer = new QHBoxLayout();
  btn_sizer->addWidget(new_button,1);
  btn_sizer->addWidget(del_button,1);
  btn_sizer->addWidget(save_button,1);
  // next up, we create the picker and the side sizer
  picker = new jfFilterPicker2(NULL);
  side_sizer = new QVBoxLayout();
  side_sizer->addLayout(btn_sizer,0);
  side_sizer->addWidget(picker,1);
  // we make a blank panel
  blank_panel = new QWidget(this);
  editing_panel = NULL;
  isblank = true;
  // arranging that
  top_sizer = new QHBoxLayout();
  top_sizer->addLayout(side_sizer,1);
  top_sizer->addWidget(blank_panel,local_ratio);
  setLayout(top_sizer);
  // connections
  connect(new_button, SIGNAL(clicked(bool)), this, SLOT(NewPressed(bool)));
  connect(del_button, SIGNAL(clicked(bool)), this, SLOT(DeletePressed(bool)));
  connect(save_button, SIGNAL(clicked(bool)), this, SLOT(SavePressed(bool)));
  connect(picker, SIGNAL(SendSelected()), this, SLOT(FilterPicked()));
  // setting the data
  BtnEnable(false);
  disablec = false;
  selindex = -1;
  if (inlink!=NULL) ChangeMap(inlink);

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//data setting
bool jfFilterMapEditor::ChangeMap(jfFilterMap* inlink) {
  // basic checks
  size_t fcounty;
  assert(inlink!=NULL);
  if (fdata!=inlink) fdata = inlink;
  // we start loading the data
  picker->SetContents(fdata);
  fcounty = fdata->GetCount();
  // if the filter map is empty, we remove the editing panel and use a blank
  if (fdata->noFilters()) SetNoFilter();
  // the filter map is not empty, we have to change the filter to the first one
  else {
    ChangeFilter(0);
    BtnEnable(true);
  }
  // done
  return true;
}
//------------------------------------------------------------------------
// changes the filter to one at a particular index
bool jfFilterMapEditor::ChangeFilter(size_t findex) {
    const QString fname = "jfFilterMapEditor::ChangeFilter";
  // things that make us return false
    /**/JDEBUGLOGB(fname,1,fdata==NULL);
  if (fdata->noFilters()) return false;
  /**/JDEBUGLOG(fname,2);
  if ((fdata->GetCount())<=findex) return false;
  /**/JDEBUGLOG(fname,3);
  // doing the thing, then returning true
  bool cs_res = picker->ChangeSelected(findex);
  /**/JDEBUGLOG(fname,4);
  assert(cs_res);
  /**/JDEBUGLOG(fname,5);
  selindex = findex;
  /**/JDEBUGLOG(fname,6);
  LoadSelected();
  /**/JDEBUGLOG(fname,7);
  return true;
}
//--------------------------------------------------------------------------
void jfFilterMapEditor::SetNoFilter() {
  // basic checks
  assert(blank_panel!=NULL);
  // we only need to do something if we are non-blank
  if (!isblank) {
    assert(editing_panel!=NULL);
    // replacing
    top_sizer->removeWidget(editing_panel);
    editing_panel->hide();
    top_sizer->addWidget(blank_panel,local_ratio);
    blank_panel->show();
    // removing the old panel
    delete editing_panel;
    editing_panel = NULL;
    isblank = true;
    picker->Deselect();
    selindex = -1;
    oldtype = "";
    BtnEnable(false);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//event handlers
//--------------------------------------------------------------------------
// making a blank new filter and adding it to the map: we have to pick the name
void jfFilterMapEditor::NewPressed(bool checked) {
  // constants
  const QString fname = "jfFilterMapEditor::NewPressed";
  const QString nfbasen="NewFilter";
  const QString namprob = "Problem with the Name";
  // local variables
  jfStartFilterDialog*  newdialog;
  jfBaseFilter*          newf1;
  int moderes;
  size_t tresult, oindex;
  QString newname;
  bool okchosen = false;
  // the main loop
  while(true) {
    // making the dialog and showing it
    newdialog = new jfStartFilterDialog(MakeName(nfbasen),pflist,this);
    moderes = newdialog->exec();
    // we check the result
    if (moderes==0) break;
    // here, the result is OK, we check the name
    else {
      newname = newdialog->GetNewName();
      // checking for empty
      if (newname.isEmpty()) {
        QMessageBox::critical(this,namprob,"The Name must not be empty!",QMessageBox::Retry);
      }
      // if the name is already in use, we show an error message and loop
      else if (fdata->NameExists(newname)) {
        QMessageBox::critical(this,namprob,"The name \'"+newname+
                              "\' already exists. Please pick another.",QMessageBox::Retry);
      }
      else {
        okchosen = true;
        break;
      }
    }
  }
  // we have got our result, we make and insert the new filter if need be
  if (okchosen) {
    tresult = newdialog->GetResIndex();
    assert(tresult<(pflist->count()));
    // we create the filter
    newf1 = MakeFilter((*pflist)[tresult],newname);
    // insert the filter
    QString oname = newf1->GetName();
    bool atest = fdata->AddFilter(newf1,oindex);
    assert(atest);
    atest = picker->InsertAtIndex(oindex,newf1);
    assert(atest);
    /**/JDEBUGLOGST(fname,1,oindex);
    /**/JDEBUGLOGST(fname,2,fdata->GetCount());
    // we change the current filter to the inserted one
    atest = ChangeFilter(oindex);
    assert(atest);
    /**/JDEBUGLOG(fname,4);
    BtnEnable(true);
  }
  delete newdialog;
}
//--------------------------------------------------------------------------
// deleting a filter
void jfFilterMapEditor::DeletePressed(bool checked) {
  const QString fname = "jfFilterMapEditor::DeletePressed";
  // we record the selected index to determine what it will be
  long oselected = picker->SelectedIndex();
  /**/JDEBUGLOGI(fname,1,oselected);
  if (oselected>=0) {
    // we delete the actual filter first
    /**/JDEBUGLOG(fname,2);
    QString filname = picker->SelectedName();
    /**/JDEBUGLOGS(fname,3,filname);
    fdata->DeleteByName(filname);
    // we remove the item from the picker
    /**/JDEBUGLOG(fname,4);
    picker->blockSignals(true);
    picker->removeRow(oselected);
    // now we decide on the new selected index
    int icount = picker->rowCount();
    picker->blockSignals(false);
    /**/JDEBUGLOGI(fname,5,icount);
    // there is nothing left
    if (icount==0)  SetNoFilter();
    else {
      // if the selected insex is now past the last, select the last
      if (icount==oselected) oselected-=1;
      /**/JDEBUGLOG(fname,6);
      ChangeFilter(oselected);
    }
  }
}
//--------------------------------------------------------------------------
// saving a filter
void jfFilterMapEditor::SavePressed(bool checked) {
  const QString fname = "jfFilterMapEditor::SavePressed";
  /**/JDEBUGLOG(fname,1)
  assert(!isblank);
  /**/JDEBUGLOG(fname,2)
  // we only go ahead with the save if the check passes
  if (editing_panel->GeneralCheck()) {
      /**/JDEBUGLOG(fname,3)
    CoreSave();
      /**/JDEBUGLOG(fname,4)
  }
}
//--------------------------------------------------------------------------
void jfFilterMapEditor::FilterPicked() {
  const QString fname = "jfFilterMapEditor::FilterPicked";
  int newindex = picker->SelectedIndex();
  /**/JDEBUGLOGI(fname,1,newindex);
  if (selindex!=newindex) {
    /**/JDEBUGLOGI(fname,2,selindex);
    CoreSave(true);
    /**/JDEBUGLOG(fname,3);
    selindex = picker->SelectedIndex();
    /**/JDEBUGLOGI(fname,4,selindex);
    LoadSelected();
    /**/JDEBUGLOG(fname,5);
  }
}
//--------------------------------------------------------------------------
// special methods
void jfFilterMapEditor::DisableContents(bool disable) {
  disablec = disable;
  // setting the buttons
  new_button->setEnabled(!disable);
  del_button->setEnabled(!disable);
  save_button->setEnabled(!disable);
  // the main thing...
  if (!isblank) {
    editing_panel->setEnabled(!disablec);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterMapEditor::LoadSelected() {
  // const
  const QString fname = "jfFilterMapEditor::LoadSelected";
  // local variables
  jfBaseFilter* tfilter;
  QString tfiltype;
  jfBaseFilterEditor* eholder;
  // checks
  /**/JDEBUGLOG(fname,1)
  assert(picker->IsFilterSelected());
  /**/JDEBUGLOGS(fname,2,picker->SelectedName())
  // we get the filter in question
  tfilter = fdata->GetItem(picker->SelectedName());
  /**/JDEBUGLOG(fname,3)
  assert(tfilter!=NULL);
  /**/JDEBUGLOG(fname,4)
  // we get the type in question
  tfiltype = tfilter->GetTypeID();
  /**/JDEBUGLOG(fname,4)
  // we do not need to change the panel
  if (tfiltype==oldtype) {
    /**/JDEBUGLOG(fname,6)
    editing_panel->LoadFilter(tfilter);
    /**/JDEBUGLOG(fname,7)
  }
  // we do need to change the panel
  else {
    /**/JDEBUGLOG(fname,8)
    // making the editor
    eholder = MakeFilterEditor(tfiltype,this,fdata,tfilter,is_inglobal);
    /**/JDEBUGLOG(fname,9)
    eholder->setEnabled(!disablec);
    /**/JDEBUGLOG(fname,10)
    // how we replace it depends on whether it is blank
    if (isblank) {
      /**/JDEBUGLOG(fname,11)
      top_sizer->removeWidget(blank_panel);
      blank_panel->hide();
      top_sizer->addWidget(eholder,local_ratio);
      isblank = false;
    }
    else {
      /**/JDEBUGLOG(fname,12)
      top_sizer->removeWidget(editing_panel);
      top_sizer->addWidget(eholder,local_ratio);
      delete editing_panel;
    }
    // finializing steps
   /**/JDEBUGLOG(fname,13)
    editing_panel = eholder;
    oldtype = tfiltype;
  }
  // done
  return true;
}
//------------------------------------------------------------------------
// makes up a general filter name
QString jfFilterMapEditor::MakeName(const QString& nfbase) const {
  // variables
  size_t loopc=1;
  QString buffer;
  // starting
  assert(fdata!=NULL);
  // the loop
  while (true) {
    buffer = nfbase + QString::number(loopc);
    if (!(fdata->NameExists(buffer))) break;
    loopc++;
  }
  return buffer;
}
//---------------------------------------------------------------------------
void jfFilterMapEditor::BtnEnable(bool enable) {
  del_button->setEnabled(enable);
  save_button->setEnabled(enable);
}
//------------------------------------------------------------------
bool jfFilterMapEditor::CoreSave(bool saveold) {
  //constants
  const QString fname = "jfFilterMapEditor::CoreSave";
  // variables
  QString repname;
  // starting
  /**/JDEBUGLOG(fname,1);
  if (!isblank) {
    // variables
    jfBaseFilter *newfilter;
    size_t ins_place;
    bool atest;
    // we get the replacement filter
    /**/JDEBUGLOG(fname,2);
    newfilter = editing_panel->GetFilter();
    assert(newfilter!=NULL);
    // replace the filter
    /**/JDEBUGLOGB(fname,3,saveold);
    if (saveold) repname = picker->NameAt(selindex);
    else repname = picker->SelectedName();
    atest = fdata->ReplaceByName(repname,newfilter,ins_place);
    assert(atest);
    /**/JDEBUGLOG(fname,4);
    picker->blockSignals(true);
    // and change the selection index (only truly needed if the name has changed)
    if (saveold) picker->ReplaceAtIndex(selindex, ins_place, newfilter);
    else picker->ReplaceSelected(ins_place,newfilter);
    picker->blockSignals(false);
    selindex = ins_place;
    /**/JDEBUGLOGST(fname,5,ins_place);
    if (!saveold) {
      editing_panel->ChangeBoxName(newfilter->GetName());
      /**/JDEBUGLOG(fname,6);
      editing_panel->ChangeFilter(newfilter);
    }
    // done
  return atest;
  }
  else return false;
}
//-------------------------------------------------------------------------
// saving a filter when we are switching to another
bool jfFilterMapEditor::SaveFilter(long oindex) {
  if (oindex<0) return false;
  // variables
  jfBaseFilter *newfilter;
  size_t ins_place;
  bool atest;
  // we get the replacement filter
  newfilter = editing_panel->GetFilter();
  assert(newfilter!=NULL);
  // replace the filter
  atest = fdata->ReplaceByName(picker->NameAt(oindex),newfilter,ins_place);
  assert(atest);
	return true;
}
//*****************************************************************************
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
  ndesce->SetFromObj(inlink);
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
  atest = (*maindata)[thename]->AddFilter(new jfUrlFilter(),nindex);
  assert(atest);
  // test 1
  atest = maindata->FindMapIndex(thename,nindex);
  assert(atest);
  // adding it to the list ctrl
  map_list->insertItem(nindex,thename);
  return nindex;
}
/*****************************************************************************/
