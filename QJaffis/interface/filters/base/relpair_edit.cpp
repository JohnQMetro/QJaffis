/******************************************************************************
Name    :   relpair_edit.cpp
Author  :   John Q Metro
Purpose :   GUI Editing for Structured Pair Filter
Created :   February 24, 2023
Created :   February 25, 2023
******************************************************************************/

#include "relpair_edit.h"
/*****************************************************************************/
jfStrPairingPanel::jfStrPairingPanel(QWidget* parent):QWidget(parent) {

  // creating GUI elemnts
  matchallbox = new QCheckBox("Check if all pairings in the fic must match.");
  readerbox = new QCheckBox("Check to match any pairing including 'Reader' or 'You'.");
  readerbox->setChecked(false);
  originalbox = new QCheckBox("Check to match any original character pairing.");
  originalbox->setChecked(false);

  QString labelmsg = "Enter the pairings to find, one per line. Use / to separate romantic/sexual pairs. ";
  labelmsg += "Use & to separate platonic pairs. A string prefixed with a %% will be applied to non-splittable ";
  labelmsg += "information in the fanfic pair list, like a special pair 'codeword'. ";
  labelmsg += "Matches are done via substring, so the names do not need to be exact.";
  listheader = new QLabel(labelmsg);
  listheader->setWordWrap(true);

  pairlistbox = new QPlainTextEdit();

  // arranging
  bsizer = new QVBoxLayout();
  bsizer->addWidget(matchallbox,0);
  bsizer->addWidget(readerbox,0);
  bsizer->addWidget(originalbox,0);
  bsizer->addSpacing(6);
  bsizer->addWidget(listheader, 0);
  bsizer->addWidget(pairlistbox,1);
  // finishing
  setLayout(bsizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o
//-------------------------------------
void jfStrPairingPanel::SetFromString(const QString& inval) {
    pairlistbox->setPlainText(inval);
}
//-------------------------------------
void jfStrPairingPanel::SetMatchAllCheck(bool cval) {
    matchallbox->setChecked(cval);
}
// -----------------------------------
void jfStrPairingPanel::SetReaderCheck(bool cval) {
    readerbox->setChecked(cval);
}
// ---------------------------------
void jfStrPairingPanel::SetOriginalCheck(bool cval) {
    originalbox->setChecked(cval);
}
//-------------------------------------
QString jfStrPairingPanel::GetStringValue() const {
    return pairlistbox->toPlainText();
}
//-------------------------------------
bool jfStrPairingPanel::GetMatchAllCheck() const {
    return matchallbox->isChecked();
}
//-------------------------------------
bool jfStrPairingPanel::GetReaderCheck() const {
    return readerbox->isChecked();
}
//-------------------------------------
bool jfStrPairingPanel::GetOriginalCheck() const {
    return originalbox->isChecked();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// filter based i/o
//-------------------------------------
jfStructuredPairFilter* jfStrPairingPanel::GetFilter() const {
  jfStructuredPairFilter* result = new jfStructuredPairFilter();
  bool rb = readerbox->isChecked();
  bool ob = originalbox->isChecked();
  bool hn = result->SetNamesData(pairlistbox->toPlainText(), true);
  if ((!hn) && (!rb) && (!ob)) {
    delete result;
    return NULL;
  }
  result->SetMatchAll(matchallbox->isChecked());
  result->SetMatchReader(rb);
  result->SetMatchOriginal(ob);
  return result;
}
//-------------------------------------
bool jfStrPairingPanel::StoreToFilter(jfStructuredPairFilter* outval) {
  assert(outval!=NULL);
  outval->SetNamesData(pairlistbox->toPlainText(), true);
  outval->SetMatchAll(matchallbox->isChecked());
  outval->SetMatchReader(readerbox->isChecked());
  outval->SetMatchOriginal(originalbox->isChecked());
  return true;
}
//-------------------------------------
bool jfStrPairingPanel::LoadFromFilter(const jfStructuredPairFilter* inval) {
  assert(inval!=NULL);
  if (!inval->IsValid()) return false;
  pairlistbox->setPlainText(inval->GetNamesData(true));
  matchallbox->setChecked(inval->GetMatchAll());
  readerbox->setChecked(inval->GetMatchReader());
  originalbox->setChecked(inval->GetMatchOriginal());
  return true;
}
//=============================================================================
jfStr_PairingFilterEditor::jfStr_PairingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap,
               QWidget* parent):jfBaseFilterEditor(infmap,infilt,parent) {
  // we start...
  // we create the insert.. the *actual*  editor
  insert_panel = new jfStrPairingPanel();
  // finalizing things
  insert = new QBoxLayout(QBoxLayout::TopToBottom);
  insert->addWidget(insert_panel,1);
  // calling the sizers
  ArrangeSizers();
  if (infilt!=NULL) LoadFilter(infilt);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//-------------------------------------
void jfStr_PairingFilterEditor::LoadFilter(const jfBaseFilter* infilter) {
  assert(infilter!=NULL);
  filt_pointer = infilter;
  const jfStructuredPairFilter* temp = dynamic_cast<const jfStructuredPairFilter*>(infilter);
  insert_panel->LoadFromFilter(temp);
  NameLoad();
}
//-------------------------------------
jfBaseFilter* jfStr_PairingFilterEditor::GetFilter() {
  jfStructuredPairFilter* result = insert_panel->GetFilter();
  namedesc_edit->ChangeObj(result);
  return result;
}
//--------------------------------------------------------------
bool jfStr_PairingFilterEditor::GeneralCheck() const {
  const jfStructuredPairFilter* result = insert_panel->GetFilter();
  if (result == NULL) return false;
  bool resval = result->IsValid();
  delete result;
  return resval;
}



/*****************************************************************************/
