/******************************************************************************
Name    :   sfilt_edit.cpp
Author  :   John Q Metro
Purpose :   Filter editors (wraaping the core). Simple expression & mo0re
Created :   December 5, 2010
Conversion to Qt Started October 8, 2013
Updated :   November 15, 2011
******************************************************************************/
#ifndef SFILT_EDIT_H_INCLUDED
  #include "sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
//+++++++++++++++++++++++++++++++++++
#include <assert.h>
#include <QMessageBox>
//****************************************************************************
// messaages
const QString pfmsg = "Problem with filter contents!";
/*****************************************************************************/
// the default constructor
jfSimpleSFilterEditor::jfSimpleSFilterEditor(const jfFilterMap* infmap, const jfSimpleExpFilterCore* infilt,
                  QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // most things are handled by the parent constructor
  QString omsg;
  assert(infmap!=NULL);
  typed_fpointer = infilt;
  // we create the insert.. the *actual* expression editor
  insert_panel = new jfSimpleExprEdit(true);
  insert_panel->SetData(filt_pointer->ToString(),omsg);
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
  if (infilt!=NULL) NameLoad();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfSimpleSFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  QString omsg;
  // loading the data
  filt_pointer = infilter;
  insert_panel->SetData(filt_pointer->ToString(),omsg);
  NameLoad();
}
//------------------------------------------------------------------------------
jfBaseFilter* jfSimpleSFilterEditor::GetFilter() {
  jfSimpleExpr* toutput;
  jfSimpleExpFilterCore* result;
  QString omsg;
  // getting the output
  toutput = insert_panel->CheckFilter(omsg);
  result = MakeTypedFilter(toutput);
  namedesc_edit->ChangeObj(result);
  // done
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks if the contents are ok, returns false if they are not
bool jfSimpleSFilterEditor::GeneralCheck() const {
  // local variables
  jfSimpleExpr* toutput;
  QString omsg;
  QMessageBox emsg;
  // first we see if the name is correct
  if (NameNUniq()) return false;
  // next, we check the filter
  toutput = insert_panel->CheckFilter(omsg);
  if ((toutput!=NULL) && (toutput->IsValid())) {
    // things are ok
    delete toutput;
    return true;
  }
  else {
    // things are not okay
    emsg.setIcon(QMessageBox::Critical);
    emsg.setText(omsg);
    emsg.setWindowTitle(pfmsg);
    emsg.exec();
    if (toutput!=NULL) delete toutput;
    return false;
  }
}
//****************************************************************************
