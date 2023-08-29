/******************************************************************************
Name    :   widgets1.cpp
Author  :   John Q Metro
Purpose :   Some misc user interface stuff
Created :   April 1, 2009
Conversion to Qt : Started September 8, 2013
Updated :   April 16, 2023 (filter no long base obj)
******************************************************************************/
#ifndef WIDGETS1_H_INCLUDED
  #include "widgets1.h"
#endif // WIDGETS1_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------
#include <assert.h>
#include <QMessageBox>
//****************************************************************************
// methods for jfNameDescEditor
//=============================================================================
// the constructor
jfNameDescEditor::jfNameDescEditor(QWidget* parent,bool isvert, bool ismulti,
      QString default_name,QString default_desc):QWidget(parent) {
  // we start with checking as default name
  assert(!default_name.isEmpty());
  // we set the flags
  vertical = isvert;
  multiline = ismulti;
  extra_count = 0;
  // we start with creating the gui elements
  // name
  name_label = new QLabel("Name :");
  name_edit = new QLineEdit(default_name);
  backup_data = default_name;
  // description label
  desc_label = new QLabel("Description :");
  // making the description editor
  if (!multiline) {
    desc_edit1 = new QLineEdit(default_desc);
    desc_edit2 = NULL;
  }
  else {
    desc_edit2 = new QPlainTextEdit(default_desc);
    desc_edit1 = NULL;
  }
  // now, we start with sizers and arrangement
  if (isvert) {
    // non-grid layout: the description is below it's label
    topline = new QHBoxLayout();
    mainsizer = NULL;
    topline->addWidget(name_label,0,Qt::AlignVCenter);
    topline->addWidget(name_edit,0);
    vertmsizer = new QVBoxLayout();
    vertmsizer->addLayout(topline);
    vertmsizer->addWidget(desc_label,0,Qt::AlignLeft);
    if (multiline) vertmsizer->addWidget(desc_edit2,1);
    else vertmsizer->addWidget(desc_edit1,0);
    vertmsizer->setMargin(5);
    setLayout(vertmsizer);
  }
  else {
    // grid layout
    mainsizer = new QGridLayout();
    topline = NULL;
    vertmsizer = NULL;
    mainsizer->addWidget(name_label,0,0,Qt::AlignRight);
    mainsizer->addWidget(name_edit,0,1);
    if (multiline) {
      mainsizer->addWidget(desc_label,1,0,Qt::AlignRight|Qt::AlignTop);
      mainsizer->addWidget(desc_edit2,1,1);
    }
    else {
      mainsizer->addWidget(desc_label,1,0,Qt::AlignVCenter);
      mainsizer->addWidget(desc_edit1,1,1);
    }
    mainsizer->setColumnStretch(1,1);
    if (multiline) mainsizer->setRowStretch(1,1);
    mainsizer->setMargin(5);
    setLayout(mainsizer);
  }
  // setting the connection
  connect(name_edit, SIGNAL(textEdited(const QString&)),this, SLOT(HandleNameChange(const QString&)));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// adding extra objects or sizers
//------------------------------------------------------------------
// adding a control to the toprow
void jfNameDescEditor::AddItem(QWidget* widget, size_t prop) {
  // a basic check
  assert(widget!=NULL);
  // adding
  if (vertical) topline->addWidget(widget,prop);
  else {
    mainsizer->addWidget(widget,0,2+extra_count,Qt::AlignVCenter);
    if (multiline) {
      mainsizer->removeWidget(desc_edit2);
      mainsizer->addWidget(desc_edit2,1,1,1,2+extra_count);
    }
    else {
      mainsizer->removeWidget(desc_edit1);
      mainsizer->addWidget(desc_edit1,1,1,1,2+extra_count);
    }
    mainsizer->setColumnStretch(2+extra_count,prop);
  }
  extra_count++;
}
//------------------------------------------------------------------
// adding a sizer to the top row
void jfNameDescEditor::AddItem(QLayout* sizer, size_t prop) {
  // a basic check
  assert(sizer!=NULL);
  // adding
  if (vertical)  topline->addLayout(sizer,prop);
  else {
    mainsizer->addLayout(sizer,0,2+extra_count,Qt::AlignVCenter);
    if (multiline) {
      mainsizer->removeWidget(desc_edit2);
      mainsizer->addWidget(desc_edit2,1,1,1,2+extra_count);
    }
    else {
      mainsizer->removeWidget(desc_edit1);
      mainsizer->addWidget(desc_edit2,1,1,1,2+extra_count);
    }
    mainsizer->setColumnStretch(2+extra_count,prop);
  }
  extra_count++;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting some info
//------------------------------------------------------------------
// gets the name (does not allow blanks)
QString jfNameDescEditor::TryGetName() const {
  if (!TryCopy()) {
    // the copying has failed, we show the error
    ShowEmptyMessage();
    name_edit->setText(backup_data);
    name_edit->setModified(false);
  }
  return name_edit->text();
}
//--------------------------------------------------------------
bool jfNameDescEditor::IsNameEmpty() const {
  QString tval;
  tval = name_edit->text();
  tval = tval.trimmed();
  return tval.isEmpty();
}
//------------------------------------------------------------------
// gets the description
QString jfNameDescEditor::GetDesc() const {
  QString res;
  if (multiline) res = desc_edit2->toPlainText();
  else res = desc_edit1->text();
  res = res.trimmed();
  return res;
}
//------------------------------------------------------------------
// return true is the name edit is not blank, false otherwise
bool jfNameDescEditor::NameStatus() const {
  return TryCopy();
}
//------------------------------------------------------------------
// returns true if the description label is above the editor
bool jfNameDescEditor::IsVertical() const {
  return vertical;
}
//------------------------------------------------------------------
// returns true if the description editor is multiline
bool jfNameDescEditor::IsMultiline() const {
  return multiline;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// changes the name and description in the filter to what is in the controls
bool jfNameDescEditor::ChangeFilter(jfFilterBase* target_filter) const {
    // things that make us return false
    if (target_filter==NULL) return false;
    if (!NameStatus()) return false;
    // setting the data
    target_filter->SetName(TryGetName());
    target_filter->SetDescription(GetDesc());
    return true;
}
// --------------------------------------------------
bool jfNameDescEditor::ChangeFilterMap(jfFilterMap* target_map) const {
    // things that make us return false
    if (target_map==NULL) return false;
    if (!NameStatus()) return false;
    // setting the data
    target_map->SetName(TryGetName());
    target_map->SetDescription(GetDesc());
    return true;
}
// -------------------------------------------------------
bool jfNameDescEditor::ChangeSkeleton(jfSkeletonCore* target_skeleton) const {
    // things that make us return false
    if (target_skeleton==NULL) return false;
    if (!NameStatus()) return false;
    // setting the data
    target_skeleton->SetName(TryGetName());
    target_skeleton->SetDescription(GetDesc());
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting some info
//------------------------------------------------------------------
// name setting
bool jfNameDescEditor::SetName(QString newname){
  // we have to check the name first
  newname = newname.trimmed();
  if (newname.isEmpty()) return false;
  // setting the name
  name_edit->setText(newname);
  return true;
}
//------------------------------------------------------------------
// setting the description
void jfNameDescEditor::SetDescription(const QString& newdesc) {
  if (!multiline) desc_edit1->setText(newdesc);
  else desc_edit2->setPlainText(newdesc);
}
//------------------------------------------------------------------
bool jfNameDescEditor::SetFromFilter(const jfFilterBase* source_filter) {
    if (source_filter==NULL) return false;

    if (!SetName(source_filter->GetName())) return false;
    SetDescription(source_filter->GetDescription());
    return true;
}
//------------------------------------------------------------------
bool jfNameDescEditor::SetFromFilterMap(const jfFilterMap* source_map) {
    if (source_map==NULL) return false;

    if (!SetName(source_map->GetName())) return false;
    SetDescription(source_map->GetDescription());
    return true;
}
// -----------------------------------------------------
bool jfNameDescEditor::SetFromSkeleton(const jfSkeletonCore* source_skeleton) {
    if (source_skeleton == NULL) return false;
    if (!SetName(source_skeleton->GetName())) return false;
    SetDescription(source_skeleton->GetDescription());
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfNameDescEditor::TryCopy() const {
  QString tvalue;
  if (name_edit->isModified()) {
    tvalue = name_edit->text();
    tvalue = tvalue.trimmed();
    if (tvalue.isEmpty()) return false;
    else {
      backup_data = name_edit->text();
      name_edit->setModified(false);
      return true;
    }
  }
  else return true;
}
//-----------------------------------------------------------------------
QString jfNameDescEditor::GetBackup() const {
  return backup_data;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// meant to handle changes in the name only
void jfNameDescEditor::HandleNameChange(const QString& newname) {
  QString localcopy = newname.trimmed();
  if (!localcopy.isEmpty()) emit SendNewName(localcopy);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// shows the error message that the string is empty
void jfNameDescEditor::ShowEmptyMessage() const {
  // constants and local varibales
  QMessageBox emsg;
  const QString ermsg="Blank names are not allowed! Reverting to the previous name.";
  // we create the dialog
  emsg.setIcon(QMessageBox::Warning);
  emsg.setText(ermsg);
  emsg.exec();
}
//**************************************************************************
// a buttonless non-modal text label status display dialog
//+++++++++++++++++++++++++++++++++++++++++++++++++
// the constructor
jfTextStatusDialog::jfTextStatusDialog(QWidget *parent, const QString& title, const QString& display_msg, QSize insize):QDialog(parent) {
	// creating the elements
  info_display = new QLabel(display_msg);
  top_sizer = new QHBoxLayout();
	// finishing
  top_sizer->addWidget(info_display,1,Qt::AlignCenter);
  setLayout(top_sizer);
  setWindowTitle(title);
  setFixedSize(insize);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// other methods
//----------------------------------------
QString jfTextStatusDialog::GetMessage() const {
  return info_display->text();
}
//----------------------------------------
void jfTextStatusDialog::ChangeMessage(const QString new_message) {
  info_display->setText(new_message);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//public slots:
//-------------------------------------------
void jfTextStatusDialog::handleAction(QString action_label) {
  info_display->setText(action_label + " ... ");
}
//-------------------------------------------
void jfTextStatusDialog::handleDone() {
  QString otext = info_display->text();
  info_display->setText(otext + "Done!");
}
//-------------------------------------------
void jfTextStatusDialog::handleLoading(QString loaded_thing) {
  QString intext = "Loading " + loaded_thing + " ...";
  info_display->setText(intext);
}
//-------------------------------------------
void jfTextStatusDialog::handleString(QString statement_string) {
  info_display->setText(statement_string);
}
//****************************************************************************
/* Qt replacement for wxCheckListBox Qt::CheckState */
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the constructor
jfCheckList::jfCheckList(bool is_tristate, QWidget* parent):QListWidget(parent) {
  tristate = is_tristate;
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setUniformItemSizes(true);
  connect(this,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(handleItemChanged(QListWidgetItem*)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// adding items
//---------------------
// handles adding the item (just text with check)
void jfCheckList::appendCheckItem(const QString& label) {
  blockSignals(true);
  QListWidgetItem* new_item = new QListWidgetItem(label);
  if (tristate) new_item->setFlags(new_item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
  else new_item->setFlags((new_item->flags() | Qt::ItemIsUserCheckable) & ~Qt::ItemIsTristate);
  new_item->setCheckState(Qt::Unchecked);
  addItem(new_item);
  cs_store.push_back(Qt::Unchecked);
  blockSignals(false);
}
//---------------------
// handles multiple additons
void jfCheckList::appendMultiCheckItems(const QStringList& labels) {
  int lcount = labels.count();
  for (int lindex = 0;lindex<lcount;lindex++) {
    appendCheckItem(labels.at(lindex));
  }
}
//---------------------
void jfCheckList::appendMultiCheckItems(const QString inlabels[], size_t labcount) {
  if (labcount>0) {
    for (size_t lindex = 0;lindex<labcount;lindex++) {
      appendCheckItem(inlabels[lindex]);
    }
  }
}
//----------------------
void jfCheckList::replaceItems(const QStringList& labels) {
  if (!isEnabled()) setEnabled(true);
  clear();
  cs_store.clear();
  appendMultiCheckItems(labels);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// information
//---------------------
// gets the check status at the list index
Qt::CheckState jfCheckList::GetState(size_t which) const {
  QListWidgetItem* titem;
  size_t rcount = count();
  if (which>=rcount) return Qt::Unchecked;
  titem = item(which);
  return titem->checkState();
}
//---------------------
// gets the boolean check status (false if unchecked, true otherwise) at the list index
bool jfCheckList::GetBoolState(size_t which) const {
  QListWidgetItem* titem;
  size_t rcount = count();
  if (which>=rcount) return false;
  titem = item(which);
  return (Qt::Unchecked!=(titem->checkState()));
}
//---------------------
// returns the first index where the label equals svalue, -1 if not found
int jfCheckList::FindString(const QString& svalue, bool casesen) const {
  // variables
  int findex;
  QList<QListWidgetItem *> zogy;
  Qt::MatchFlags matcher = Qt::MatchFixedString;
  // special cases
  if (count()==0) return -1;
  if (svalue.isEmpty()) return -1;
  // doing the find
  if (casesen) matcher |= Qt::MatchCaseSensitive;
  zogy = findItems(svalue,matcher);
  // checking the result
  if (zogy.count()==0) return -1;
  findex = row(zogy[0]);
  return findex;
}
//---------------------
// convenience function, returns true if enabled and the current selection is not checked
bool jfCheckList::CurrNotChecked() const {
  if (!isEnabled()) return false;
  int rindex = currentRow();
  if (rindex<0) return false;
  QListWidgetItem* titem = currentItem();
  return (Qt::Unchecked==(titem->checkState()));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// changing checkstate
//---------------------
size_t jfCheckList::SetCheckAll(Qt::CheckState new_value) {
  const QString fname = "jfCheckList::SetCheckAll";
  // special cases
  if ((!tristate) && (new_value==Qt::PartiallyChecked)) return 0;
  if (count()==0) return 0;
  // variables
  int lcount, lindex;
  QListWidgetItem* titem;
  // general case
  lcount = count();
  blockSignals(true);
  for (lindex = 0; lindex<lcount ; lindex++ ) {
    titem = item(lindex);
    titem->setFlags(titem->flags() | Qt::ItemIsEnabled);
    titem->setCheckState(new_value);
    cs_store[lindex] = new_value;
  }
  blockSignals(false);
  return lcount;
}
//---------------------
int jfCheckList::CheckByName(QString name, Qt::CheckState new_value) {
  // special case, wrong check state
  if ((!tristate) && (new_value==Qt::PartiallyChecked)) return -1;
  // variables
  QListWidgetItem* titem;
  QList<QListWidgetItem *> zogy;
  int lwcount, lwindex;
  // starting
  zogy = findItems(name,Qt::MatchFixedString|Qt::MatchCaseSensitive);
  lwcount = zogy.count();
  if (lwcount==0) return -1;
  // there really should be only one, we will ignore any others
  titem = zogy[0];
  lwindex = row(titem);
  // doing it!
  blockSignals(true);
  titem->setCheckState(new_value);
  cs_store[lwindex] = new_value;
  blockSignals(false);
  // done
  return lwindex;
}
//---------------------
bool jfCheckList::SetCheck(size_t which,Qt::CheckState new_value) {
  QListWidgetItem* titem;
  if (which >= ((size_t)count())) return false;
  titem = item(which);
  titem->setCheckState(new_value);
  return true;
}
//---------------------
bool jfCheckList::SetCheckNS(size_t which,Qt::CheckState new_value) {
  QListWidgetItem* titem;
  if (which >= ((size_t)count())) return false;
  titem = item(which);
  blockSignals(true);
  titem->setCheckState(new_value);
  cs_store[which] = new_value;
  blockSignals(false);
  return true;
}
//----------------------
void jfCheckList::CheckAllDisable(bool check) {
  if (check) {
    SetCheckAll(Qt::Checked);
    setEnabled(false);
  }
  else {
    setEnabled(true);
    SetCheckAll(Qt::Unchecked);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfCheckList::CheckNames(const QStringList* inlist) {
    assert(inlist != NULL);
    // variables
    QListWidgetItem* titem;
    QList<QListWidgetItem *> zogy;
    int lwcount, lwindex;
    size_t lindex, lcount;
    size_t rcounter;
    // starting
    lcount = inlist->size();
    rcounter = 0;
    blockSignals(true);
    // the main loop
    for (lindex = 0 ; lindex < lcount; lindex++) {
        // starting
        zogy = findItems(inlist->at(lindex),Qt::MatchFixedString|Qt::MatchCaseSensitive);
        lwcount = zogy.count();
        if (lwcount==0) continue;
        // there really should be only one, we will ignore any others
        titem = zogy[0];
        lwindex = row(titem);
        // doing it!
        titem->setCheckState(Qt::Checked);
        cs_store[lwindex] = Qt::Checked;
        rcounter++;
    }
    blockSignals(false);
    return rcounter;
}
//-------------------------------------------
size_t jfCheckList::CheckAndDisableNames(jfStrBoolList* inlist) {
  if (inlist==NULL) return 0;
  // variables
  QListWidgetItem* titem;
  QList<QListWidgetItem *> zogy;
  int lwcount, lwindex;
  size_t lindex, lcount;
  size_t rcounter;
  QString qname;
  // starting
  lcount = inlist->size();
  rcounter = 0;
  blockSignals(true);
  // the main loop
  for (lindex = 0 ; lindex < lcount; lindex++) {
    // starting
    qname = (inlist->at(lindex)).name;
    zogy = findItems(qname,Qt::MatchFixedString|Qt::MatchCaseSensitive);
    lwcount = zogy.count();
    if (lwcount==0) continue;
    // there really should be only one, we will ignore any others
    titem = zogy[0];
    lwindex = row(titem);
    // doing it!
    titem->setCheckState(Qt::Checked);
    cs_store[lwindex] = Qt::Checked;
    if ((inlist->at(lindex)).dis) titem->setFlags((titem->flags() & (~Qt::ItemIsEnabled)));
    else titem->setFlags(titem->flags() | Qt::ItemIsEnabled);
    rcounter++;
  }
  blockSignals(false);
  return rcounter;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
// other
bool jfCheckList::setSelectionNS(size_t which) {
  if (which >= ((size_t)count())) return false;
  blockSignals(true);
  setCurrentRow(which);
  blockSignals(false);
  return true;
}
//---------------------------------------------
bool jfCheckList::clearAll() {
  clear();
  cs_store.clear();
  return true;
}
//------------------------------------
bool jfCheckList::RemoveCheckItem(size_t index) {
  if (index >= ((size_t)count())) return false;
  QListWidgetItem* titem;
  titem = takeItem(index);
  cs_store.erase(cs_store.begin()+index);
  delete titem;
  return true;
}
//------------------------------------
bool jfCheckList::RemoveCheckItem(const QString& label) {
  // variables
  QListWidgetItem* titem;
  QList<QListWidgetItem *> zogy;
  int lwcount, lwindex;
  // starting
  zogy = findItems(label,Qt::MatchFixedString|Qt::MatchCaseSensitive);
  lwcount = zogy.count();
  if (lwcount==0) return false;
  // there really should be only one, we will ignore any others
  titem = zogy[0];
  lwindex = row(titem);
  return RemoveCheckItem(lwindex);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfCheckList::handleItemChanged(QListWidgetItem* item) {
  assert(item!=NULL);
  Qt::CheckState tvalue;
  int lwindex = row(item);
  if (lwindex>=0) {
    tvalue = item->checkState();
    if (tvalue!=cs_store[lwindex]) {
      // finishing
      cs_store[lwindex] = tvalue;
      emit sendChangeCheck(lwindex,tvalue);
    }
  }
}

//****************************************************************************
