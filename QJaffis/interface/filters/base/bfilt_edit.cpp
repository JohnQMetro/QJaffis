/******************************************************************************
Name    :   bfilt_edit.cpp
Author  :   John Q Metro
Purpose :   Defines some base class for filter diting
Created :   July 28, 2009
Conversion to Qt Started Oct 3, 2013
Updated :   April 8, 2023
******************************************************************************/
#ifndef BFILT_EDIT_H_INCLUDED
  #include "bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
//---------------------------------------------------------------------------
#include <assert.h>
#include <QMessageBox>
/*****************************************************************************/
// messaages
const QString pfmsg = "Problem with filter contents!";

//---------------------------------------------------------------------------------
jfBaseFilterEditor::jfBaseFilterEditor(const jfFilterBase* infilt, QWidget* parent):QWidget(parent) {
  // checks
  assert(infilt!=NULL);
  // setting the pointers
  filt_pointer = infilt;
  // we start creating the gui elements
  topsizer = new QVBoxLayout();
  mbox = new QGroupBox(filt_pointer->GetName());
  namedesc_edit = new jfNameDescEditor(NULL,true,false,"1");
  reload_btn = new QPushButton("Reload");
  namedesc_edit->AddItem(reload_btn);
  namedesc_edit->SetFromFilter(filt_pointer);
  /* the rest of the arranging should be done by the arrange sizers method,
  which should be called by the constructor of child classes */
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* changes the filter without loading the contents. Useful, for example,
if you have deleted the original filter and have replaced it by what is
returned by GetFilter()  */
bool jfBaseFilterEditor::ChangeFilter(const jfFilterBase* infilter) {
  if (infilter==NULL) return false;
  filt_pointer = infilter;
  return true;
}
//--------------------------------------------------------------------------
// calls the virtaul method LoadFilter
void jfBaseFilterEditor::ReloadFilter() {
  LoadFilter(filt_pointer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* checks if the name in the name editor is elsewhere in the list, returns
FALSE is this is so */
bool jfBaseFilterEditor::NameCheck(const jfFilterMap* filter_group) const {
  // starting...
  QString tname;
  tname = namedesc_edit->TryGetName();
  if (tname==(filt_pointer->GetName())) return true;
  // at this point, the name has been changed, we look for it
  if (filter_group == NULL) return true;
  else return !(filter_group->NameExists(tname));
}
//--------------------------------------------------------------------------
void jfBaseFilterEditor::ChangeBoxName(QString newname) {
  mbox->setTitle(newname);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfBaseFilterEditor::ReloadBtnPressed(bool check) {
  ReloadFilter();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfBaseFilterEditor::ArrangeSizers() {
  // we run some basic checks
  assert(insert!=NULL);
  // creating the top sizer
  topsizer->addWidget(namedesc_edit,0);
  topsizer->addLayout(insert,1);
  mbox->setLayout(topsizer);
  realtopsizer = new QHBoxLayout();
  realtopsizer->addWidget(mbox,1);
  // done..
  setLayout(realtopsizer);
  connect(reload_btn, SIGNAL(clicked(bool)), this, SLOT(ReloadBtnPressed(bool)));
}
//----------------------------------------------------------------------------
// returns true if NameCheck() fails after showing an error MessageBox
bool jfBaseFilterEditor::NameNUniq(const jfFilterMap* filter_group) const {
  QMessageBox xmsg;
  if (!NameCheck(filter_group)) {
    xmsg.setIcon(QMessageBox::Critical);
    xmsg.setText("The name has been changed to one that is already in the filter list. Names must be unique!");
    xmsg.setStandardButtons(QMessageBox::Abort);
    xmsg.exec();
    return true;
  }
  else return false;
}
//----------------------------------------------------------------
void jfBaseFilterEditor::NameLoad() {
  namedesc_edit->SetFromFilter(filt_pointer);
  ChangeBoxName(filt_pointer->GetName());
}

/*****************************************************************************/
/*****************************************************************************/

// the default constructor
jfExprFilterEditor::jfExprFilterEditor(const jfFilterMap* infmap, bool is_inglobal,
      const jfExpressionFilter* infilt, QWidget* parent):jfBaseFilterEditor(infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
  assert(infmap!=NULL);
  is_global = is_inglobal;
  // we create the insert.. the *actual* expression editor
  insert_panel = new jfExpressionEditor(const_cast<jfFilterMap*>(infmap),false,true,is_global);
  insert_panel->SetData(filt_pointer->ToString(),omsg);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfExprFilterEditor::LoadFilter(const jfFilterBase* infilter) {
  assert(infilter!=NULL);
  QString omsg;
  // loading the data
  filt_pointer = infilter;
  insert_panel->SetData(filt_pointer->ToString(),omsg);
  NameLoad();
}
//------------------------------------------------------------------------------
jfFilterBase* jfExprFilterEditor::GetFilter() {
    return GetExpressionFilter();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// typed methods
//-------------------------------
void jfExprFilterEditor::LoadExpressionFilter(const jfExpressionFilter* infilter) {
    assert(infilter!=NULL);
    QString omsg;
    // loading the data
    filt_pointer = infilter;
    insert_panel->SetData(filt_pointer->ToString(),omsg);
    NameLoad();
}
//-------------------------------
jfExpressionFilter* jfExprFilterEditor::GetExpressionFilter() const {
    jfExpressionFilter* toutput;
    QString omsg;
    // getting the output
    toutput = insert_panel->CheckFilter(omsg);
    namedesc_edit->ChangeFilter(toutput);
    // done
    return toutput;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfExprFilterEditor::GeneralCheck(const jfFilterMap* filter_group) const {
  // local variables
  jfExpressionFilter* toutput;
  QString omsg;
  QMessageBox xmsg;
  // first we see if the name is correct
  if (NameNUniq(filter_group)) return false;
  // next, we check the filter
  toutput = insert_panel->CheckFilter(omsg);
  if (toutput!=NULL) {
    // things are ok
    delete toutput;
    return true;
  }
  else {
    // things are not ok
    xmsg.setIcon(QMessageBox::Warning);
    xmsg.setText(omsg);
    xmsg.setStandardButtons(QMessageBox::Close);
    xmsg.setWindowTitle(pfmsg);
    xmsg.exec();
    return false;
  }
}

/*****************************************************************************/
