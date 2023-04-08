/******************************************************************************
Name    :   skeledit.cpp
Author  :   John Q Metro
Purpose :   Skeleton editing panels
Created :   November 25, 2009
Conversion to Qt started : September 15, 2013
Updated :   September 18, 2013
******************************************************************************/
#ifndef SKELEDIT_H_INCLUDED
  #include "skeledit.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------------------------------------------
#include <assert.h>

/*****************************************************************************/
// the constructor
jfSkelEditCore::jfSkelEditCore(QWidget* parent):QWidget(parent) {
  const QString fname = "jfSkelEditCore::jfSkelEditCore";
  // we start with the name editor
  ndesce = new jfNameDescEditor(NULL,false,false);
  // buttons
  save_btn = new QPushButton("Save");
  rest_btn = new QPushButton("Restore");
  ndesce->AddItem(save_btn);
  ndesce->AddItem(rest_btn);
  // the text control for note displaying
  help_desc = new QPlainTextEdit();
  // creating the notebook
  panelstore = new QTabWidget();
  // creating the pages
  // page 0, for the main header and footer
  thepanels[0] = new jfTwoTexts("Main Header Skeleton","Main Footer Skeleton",6,true);
  // page 1, for items
  thepanels[1] = new jfTwoTexts("Item Skeleton","Item Separator Skeleton",7,true);
  // page 2, category headers and footers
  // we start arranging
  sizesizer = new QHBoxLayout();
  sizesizer->addWidget(panelstore,3);
  sizesizer->addWidget(help_desc,2);
  // sticking panels in the notebook
  panelstore->addTab(thepanels[0],"Header and Footer");
  panelstore->addTab(thepanels[1],"Items");
  // more arranging
  topsizer = new QVBoxLayout();
  topsizer->addWidget(ndesce,0);
  topsizer->addLayout(sizesizer,1);
  // finishing
  thepanels[2] = NULL;
  thepanels[3] = NULL;
  thepanels[4] = NULL;
  setLayout(topsizer);
  item = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// additional methods
jfSkeletonCore* jfSkelEditCore::GetSkeleton() {
  return item;
}
//---------------------------------------------------------------------------
jfs_Skeltype jfSkelEditCore::GetSkelType() const {
  assert(item!=NULL);
  return item->GetSkelType();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper methods
bool jfSkelEditCore::CLoadItems(jfSkeletonCore* source) {
  const QString fname = "jfSkelEditCore::CLoadItems";
  if (source==NULL) return false;
  help_desc->setPlainText(source->GetGuide());
  thepanels[0]->SetBoth(source->header_skel,source->footer_skel);
  thepanels[1]->SetBoth(source->item_skel,source->item_separator);
  ndesce->SetFromObj(source);
  return true;
}
//---------------------------------------------------------------------------
bool jfSkelEditCore::CStoreItems(jfSkeletonCore* destination) const {
  if (destination==NULL) return false;
  destination->SetGuide(help_desc->toPlainText());
  thepanels[0]->GetBoth(destination->header_skel,destination->footer_skel);
  thepanels[1]->GetBoth(destination->item_skel,destination->item_separator);
  ndesce->ChangeObj(destination);
  return true;
}
//===============================================================================
// the constructor
jfSkelEditBase::jfSkelEditBase(QWidget* parent):jfSkelEditCore(parent) {
  // page 2, category headers and footers
  thepanels[2] = new jfTwoTexts("Category Header Skeleton","Category Footer Skeleton",6,true);
  // sticking panels in the notebook
  panelstore->addTab(thepanels[2],"Category Header and Footer");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal helper methods
bool jfSkelEditBase::CLoadExtra(jfSkeletonBase* source) {
  if (source==NULL) return false;
  thepanels[2]->SetBoth(source->cat_header,source->cat_footer);
  return true;
}
//---------------------------------------------------------------------------
bool jfSkelEditBase::CStoreExtra(jfSkeletonBase* destination) const {
  if (destination==NULL) return false;
  thepanels[2]->GetBoth(destination->cat_header,destination->cat_footer);
  return true;
}
//===============================================================================
// the constructor
jfSkelEditSingle::jfSkelEditSingle(jfSkeletonSingle* indata, QWidget* parent):jfSkelEditBase(parent) {
  const QString fname = "jfSkelEditSingle::jfSkelEditSingle";
  // stuff for this constructor
  typ_item = indata;
  item = indata;
  // connecting
  connect(save_btn, SIGNAL(clicked()), this, SLOT(SaveButton()));
  connect(rest_btn, SIGNAL(clicked()), this, SLOT(RestoreButton()));
  if (indata!=NULL) ChangeSkeleton(indata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods
//-------------------------------------------------------------------
bool jfSkelEditSingle::ChangeSkeleton(jfSkeletonCore* source) {
  const QString fname = "jfSkelEditSingle::ChangeSkeleton";
  if (source==NULL) return false;
  if (source->GetSkelType()!=jfs_SINGLE) return false;
  typ_item = dynamic_cast<jfSkeletonSingle*>(source);
  item = typ_item;
  return LoadSkeleton();
}
//-------------------------------------------------------------------
bool jfSkelEditSingle::LoadSkeleton() {
  if (item==NULL) return false;
  return CLoadItems(item) && CLoadExtra(dynamic_cast<jfSkeletonBase*>(item));
}
//-------------------------------------------------------------------
bool jfSkelEditSingle::SaveSkeleton() {
  if (item==NULL) return false;
  return CStoreItems(item) && CStoreExtra(dynamic_cast<jfSkeletonBase*>(item));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// button press event handlers
//-------------------------------------------------------------------
void jfSkelEditSingle::SaveButton() {
  SaveSkeleton();
}
//-------------------------------------------------------------------
void jfSkelEditSingle::RestoreButton() {
  LoadSkeleton();
}
//===============================================================================
// the constructor
jfSkelEditMultiple::jfSkelEditMultiple(jfSkeletonMultiple* indata, QWidget* parent):jfSkelEditBase(parent) {
  // stuff for this constructor
  typ_item = indata;
  item = indata;
  // extra panels for editing
  // page 3, for category links
  thepanels[3] = new jfTwoTexts("Category Link","Category Link Separator",7,true);
  // page 4, for whats left
  thepanels[4] = new jfTwoTexts("Header Below Category Links","Category Separator",6,true);
  // sticking panels in the notebook
  panelstore->addTab(thepanels[3],"Category Links");
  panelstore->addTab(thepanels[4],"Extra");
  // connecting
  connect(save_btn, SIGNAL(clicked()), this, SLOT(SaveButton()));
  connect(rest_btn, SIGNAL(clicked()), this, SLOT(RestoreButton()));
  // changing the item
  if (indata!=NULL) ChangeSkeleton(indata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods
//-------------------------------------------------------------------
bool jfSkelEditMultiple::ChangeSkeleton(jfSkeletonCore* source) {
  if (source==NULL) return false;
  if ((source->GetSkelType())!=jfs_MULTIPLE) return false;
  typ_item = dynamic_cast<jfSkeletonMultiple*>(source);
  item = typ_item;
  return LoadSkeleton();
}
//-------------------------------------------------------------------
bool jfSkelEditMultiple::LoadSkeleton() {
  if (item==NULL) return false;
  thepanels[3]->SetBoth(typ_item->catlink,typ_item->catlink_separator);
  thepanels[4]->SetBoth(typ_item->top_footer,typ_item->cat_separator);
  return CLoadItems(item) && CLoadExtra(dynamic_cast<jfSkeletonBase*>(item));
}
//-------------------------------------------------------------------
bool jfSkelEditMultiple::SaveSkeleton() {
  if (item==NULL) return false;
  thepanels[3]->GetBoth(typ_item->catlink,typ_item->catlink_separator);
  thepanels[4]->GetBoth(typ_item->top_footer,typ_item->cat_separator);
  return CStoreItems(item) && CStoreExtra(dynamic_cast<jfSkeletonBase*>(item));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// button press event handlers
//-------------------------------------------------------------------
void jfSkelEditMultiple::SaveButton() {
  SaveSkeleton();
}
//-------------------------------------------------------------------
void jfSkelEditMultiple::RestoreButton() {
  LoadSkeleton();
}
//===============================================================================
// the constructor
jfSkelEditFicPart::jfSkelEditFicPart(jfSkeletonFicPart* indata, QWidget* parent):jfSkelEditCore(parent) {
  // stuff for this constructor
  typ_item = indata;
  item = indata;
  // extra panels for editing
  // page 2, group links
  thepanels[2] = new jfTwoTexts("Group Link","Group Link Separator",7,true);
  // page 3, for part links
  thepanels[3] = new jfTwoTexts("Part Link","Part Link Separator",7,true);
  // sticking panels in the notebook
  panelstore->addTab(thepanels[2],"Group Links");
  panelstore->addTab(thepanels[3],"Part Links");
  // connecting
  connect(save_btn, SIGNAL(clicked()), this, SLOT(SaveButton()));
  connect(rest_btn, SIGNAL(clicked()), this, SLOT(RestoreButton()));
  // changing the item
  if (indata!=NULL) ChangeSkeleton(indata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods
//-------------------------------------------------------------------
bool jfSkelEditFicPart::ChangeSkeleton(jfSkeletonCore* source) {
  if (source==NULL) return false;
  if ((source->GetSkelType())!=jfs_FICPART) return false;
  typ_item = dynamic_cast<jfSkeletonFicPart*>(source);
  item = typ_item;
  return LoadSkeleton();
}
//-------------------------------------------------------------------
bool jfSkelEditFicPart::LoadSkeleton() {
  if (item==NULL) return false;
  thepanels[2]->SetBoth(typ_item->grouplink,typ_item->grouplink_separator);
  thepanels[3]->SetBoth(typ_item->partlink,typ_item->partlink_separator);
  return CLoadItems(item);
}
//-------------------------------------------------------------------
bool jfSkelEditFicPart::SaveSkeleton() {
  if (item==NULL) return false;
  thepanels[2]->GetBoth(typ_item->grouplink,typ_item->grouplink_separator);
  thepanels[3]->GetBoth(typ_item->partlink,typ_item->partlink_separator);
  return CStoreItems(item);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// button press event handlers
//-------------------------------------------------------------------
void jfSkelEditFicPart::SaveButton() {
  SaveSkeleton();
}
//-------------------------------------------------------------------
void jfSkelEditFicPart::RestoreButton() {
  LoadSkeleton();
}
/*****************************************************************************/

