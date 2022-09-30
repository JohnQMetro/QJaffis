/******************************************************************************
Name    :   tag_widgets.cpp
Author  :   John Q Metro
Purpose :   Tag categorizing widgets
Created :   July 27, 2016
Updated :   December 31, 2017
******************************************************************************/
#ifndef TAG_WIDGETS_H
  #include "tag_widgets.h"
#endif // TAG_WIDGETS_H
//-------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#include <assert.h>
#include <QMimeData>
/*****************************************************************************/
jfTC_ListBox::jfTC_ListBox(QWidget* parent):QListWidget(parent) {
  setSortingEnabled(true);
  // drag and drop stuff
  setDragEnabled(true);
  viewport()->setAcceptDrops(true);
  setDropIndicatorShown(true);
  setDefaultDropAction(Qt::MoveAction);
  boldset = NULL;
}
//---------------------------------------
bool jfTC_ListBox::addAllowedSourceLink(const jfTC_ListBox* ok_ptr) {
  if (ok_ptr == NULL) return false;
  for (size_t ptrdex = 0; ptrdex < drag_sources.size() ; ptrdex++) {
    if (drag_sources[ptrdex] == ok_ptr) return false;
  }
  drag_sources.push_back(ok_ptr);
  return true;
}
//--------------------------------------
void jfTC_ListBox::setBoldStrings(const QStringSet* boldset_in) {
    boldset = boldset_in;

    if (count() > 0) {
        QListWidgetItem* citem;
        QFont xfont;
        bool makebold = false;
        for (int crow = 0; crow < count() ; crow++) {
            citem = item(crow);
            xfont = citem->font();
            makebold = (boldset != NULL) && (boldset->contains(citem->text()));
            xfont.setBold(makebold);
            citem->setFont(xfont);
        }
    }
}
//-----------------------------------------
/* This is annoyingly complicated, mostly due to QListWidget providing no result
 * when you add an item to a sorted view, you need to track it down. Therefore,
 * this only works properly when items are unique. */
void jfTC_ListBox::addItemSpecial(const QString& newitem) {
    addItem(newitem);
    if ( (boldset != NULL) && (boldset->contains(newitem)) ) {
        bool boldres = makeStringBold(newitem);
        assert(boldres);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfTC_ListBox::dragEnterEvent(QDragEnterEvent * event) {
    if (event!=NULL) {
        // I have no idea why the return type of source was changed in Qt 5
        QWidget* sourceW = dynamic_cast<QWidget*>(event->source());
        if (!AllowedSource(sourceW)) event->ignore();
        else QListWidget::dragEnterEvent(event);
    }
}
//------------------------------------------
// overriden so I can make sure certain items are bold...
void jfTC_ListBox::dropEvent(QDropEvent *event) {
    const QMimeData* data = event->mimeData();
    QString text = data->text();
    QListWidget::dropEvent(event);
    if ((boldset != NULL) && (boldset->contains(text))) {
        makeStringBold(text);
    }
}
//------------------------------------------
bool jfTC_ListBox::makeStringBold(const QString& this_item) {
    // tracking down the item
    QList<QListWidgetItem *> fitems = findItems(this_item,Qt::MatchFixedString|Qt::MatchCaseSensitive);
    if (fitems.isEmpty()) return false;
    QListWidgetItem* target = fitems.last();
    // setting its font to bold
    QFont bfont = target->font();
    bfont.setBold(true);
    target->setFont(bfont);
    return true;
}
//---------------------------------------------------------
bool jfTC_ListBox::AllowedSource(const QWidget* source){
  if (source==NULL) return false;
  for (size_t ptrdex = 0; ptrdex < drag_sources.size() ; ptrdex++) {
    if (drag_sources[ptrdex] == source) return true;
  }
  return false;
}

//=========================================================================
// a single list of tags of a particular status type (like 'include', 'exclude', etc)
// with drag and drop
//+++++++++++++++++++++++++++++++++++++++++++++
//constructor
jfTagContainer::jfTagContainer(jfTAG_STATUS in_stat_type, QWidget* parent):QWidget(parent) {
  stat_type = in_stat_type;
  // setting up the widgets
  type_label = new QLabel("generic");
  tag_list = new jfTC_ListBox();

  stack_layout = new QVBoxLayout();
  stack_layout->addWidget(type_label,0,Qt::AlignHCenter);
  stack_layout->addWidget(tag_list,1);
  setLayout(stack_layout);
}
//--------------------------------
// setup
bool jfTagContainer::SetTagLabel(const QString& in_label) {
  if (in_label.isEmpty()) return false;
  tag_label = in_label;
  QString labelstring;
  if (stat_type == jfts_ALTERNATE) {
    labelstring = "Any of the " + tag_label;
    labelstring = " below are required.";
  }
  else {
    labelstring = tag_label + " below are ";
    if (stat_type== jfts_INCLUDE) labelstring += "required.";
    else if (stat_type== jfts_EXCLUDE) labelstring += "excluded";
    if (stat_type== jfts_NONE) labelstring += "ignored.";
  }
  // done
  type_label->setText(labelstring);
  return true;
}
//------------------------------------
void jfTagContainer::setBoldStrings(const QStringSet* boldset_in) {
    if (tag_list != NULL) {
        tag_list->setBoldStrings(boldset_in);
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++
// basic info data
//--------------------------------
jfTAG_STATUS jfTagContainer::getStatusType() const {
  return stat_type;
}
//---------------------------------
bool jfTagContainer::ContainsTag(const QString& tag) const {
  QList<QListWidgetItem*> findres;
  findres = tag_list->findItems(tag,Qt::MatchFixedString|Qt::MatchCaseSensitive);
  return (!findres.isEmpty());
}
//---------------------------------
const jfTC_ListBox* jfTagContainer::getListPointer() const {
  return tag_list;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfTagContainer::AddTag(const QString& tag) {
  if (tag.isEmpty()) return false;
  if (ContainsTag(tag)) return false;
  tag_list->addItemSpecial(tag);
  return true;
}
//--------------------------------
void jfTagContainer::ClearContents() {
  tag_list->clear();
}
//-------------------------------
QStringList* jfTagContainer::Contents() const {
  QStringList* result;
  QListWidgetItem* citem;
  // a special case
  if ((tag_list->count()) == 0) return NULL;
  // otherwise
  result = new QStringList();
  for (int tindex = 0; tindex < tag_list->count(); tindex++) {
    citem = tag_list->item(tindex);
    result->append(citem->text());
  }
  return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++
// adding a place where dragged tags can come from
bool jfTagContainer::addContainerLink(const jfTC_ListBox* ok_ptr) {
  return tag_list->addAllowedSourceLink(ok_ptr);
}
//----------------------------------------------
void jfTagContainer::linkToContainers(jfTagContainer *contr1, jfTagContainer *contr2,jfTagContainer *contr3) {
  if (contr1 != NULL) {
    addContainerLink(contr1->getListPointer());
  }
  if (contr2 != NULL) {
    addContainerLink(contr2->getListPointer());
  }
  if (contr3 != NULL) {
    addContainerLink(contr3->getListPointer());
  }
}

//======================================================================


//======================================================================
// -- [ METHODS for jfTagSorter ] ---------------------------
//++++++++++++++++++++++++++++++++++++++++++
// constructor and setup
//--------------------------------
jfTagSorter::jfTagSorter(bool leftnone, bool doalternate, QWidget* parent):QWidget(parent) {
  has_alternate = doalternate;
  none_to_left = leftnone;
  internal = false;
  bold_these_items = NULL;
  //building the containers
  NoneHolder = new jfTagContainer(jfts_NONE);
  IncludeHolder = new jfTagContainer(jfts_INCLUDE);
  ExcludeHolder = new jfTagContainer(jfts_EXCLUDE);
  // making the column that holds tags of interest
  interest_column = new QVBoxLayout();
  interest_column->addWidget(IncludeHolder,1);
  interest_column->addSpacing(10);
  interest_column->addWidget(ExcludeHolder,1);
  //now, there might be an 'Alternate' (must include at least one tag) option
  if (has_alternate){
    AlternateHolder = new jfTagContainer(jfts_ALTERNATE);
    interest_column->addSpacing(10);
    interest_column->addWidget(AlternateHolder,1);
  }
  else AlternateHolder = NULL;
  // top level layout
  main_layout = new QHBoxLayout();
  if (none_to_left) {
    main_layout->addWidget(NoneHolder,5);
    main_layout->addLayout(interest_column,4);
  }
  else {
    main_layout->addLayout(interest_column,4);
    main_layout->addWidget(NoneHolder,5);
  }
  // setting up the links (so lists can check where drops come from)
  NoneHolder->linkToContainers(IncludeHolder,ExcludeHolder,AlternateHolder);
  IncludeHolder->linkToContainers(NoneHolder,ExcludeHolder,AlternateHolder);
  ExcludeHolder->linkToContainers(IncludeHolder,NoneHolder,AlternateHolder);
  if (has_alternate) {
    AlternateHolder->linkToContainers(NoneHolder,IncludeHolder,ExcludeHolder);
  }
  // done
  setLayout(main_layout);
}

//--------------------------------
bool jfTagSorter::SetInitialData(const QString& tag_label, jfTagListing* in_tag_data, bool copy) {
  if (in_tag_data ==NULL) return false;
  // setting the labels
  if(!SetTagLabel(tag_label)) return false;
  // setting the tag data
  if (copy) {
    tag_data= new jfTagListing(*in_tag_data);
    internal = true;
  }
  else tag_data = in_tag_data;
  return LoadDataToContainers(false);
}
//--------------------------------
bool jfTagSorter::SetInitialEmpty(const QString& tag_label, const QStringList* tags) {
  if (tags == NULL) return false;
  if (tags->count()< 1) return false;
  // setting the labels
  if(!SetTagLabel(tag_label)) return false;
  // setting the data data and the container boxes
  QString ctag;
  size_t tagcount = tags->size();
  tag_data = new jfTagListing();
  internal = true;
  for (size_t tindex = 0; tindex < tagcount; tindex++) {
    ctag = tags->at(tindex);
    NoneHolder->AddTag(ctag);
  }
  tag_data->FromStrArray(tags);
  return true;
}
//--------------------------------
bool jfTagSorter::SetInitialEmpty(const QString& tag_label, const QString tag_array[], size_t tagcount) {
  if (tag_array == NULL) return false;
  if (tagcount < 1) return false;
  // setting the labels
  if(!SetTagLabel(tag_label)) return false;
  // setting the data data and the container boxes
  QString ctag;
  tag_data = new jfTagListing();
  internal = true;
  for (size_t tindex = 0; tindex < tagcount; tindex++) {
    ctag = tag_array[tindex];
    NoneHolder->AddTag(ctag);
  }
  tag_data->FromArray(tag_array,tagcount);
  return true;
}
//----------------------------------
void jfTagSorter::setBoldStrings(const QStringSet* boldset_in) {
    bold_these_items = boldset_in;
    if (NoneHolder != NULL) NoneHolder->setBoldStrings(boldset_in);
    if (IncludeHolder != NULL) IncludeHolder->setBoldStrings(boldset_in);
    if (ExcludeHolder != NULL) ExcludeHolder->setBoldStrings(boldset_in);
    if (AlternateHolder != NULL) AlternateHolder->setBoldStrings(boldset_in);
}
//+++++++++++++++++++++++++++++++++++++++++
// changing data afterwards
bool jfTagSorter::ChangeTagData(jfTagListing* in_tagdata, bool copy, bool delete_old) {
  if (in_tagdata == NULL) return false;
  // initial setup before we clear and load the data
  if ((delete_old || internal) && (tag_data != NULL)) delete tag_data;
  if (copy) {
    tag_data = new jfTagListing(*in_tagdata);
    internal = true;
  }
  else {
    tag_data = in_tagdata;
    internal = false;
  }
  // clearing
  NoneHolder->ClearContents();
  IncludeHolder->ClearContents();
  ExcludeHolder->ClearContents();
  if (has_alternate) AlternateHolder->ClearContents();
  // loading
  return LoadDataToContainers(false);
}
//---------------------------------------------------
jfTagListing* jfTagSorter::SaveAndGetTagData(){
  SaveToTagData(false);
  internal = false;
  return tag_data;
}
//++++++++++++++++++++++++++++++++++++++++++
bool jfTagSorter::LoadDataToContainers(bool alt_to_none) {
  assert(tag_data != NULL);
  // variables
  jfTagListing::const_iterator tag_pointer;
  jfTAG_STATUS curr_stat;
  QString curr_tag;
  // the scanning loop
  tag_pointer = tag_data->begin();
  while (tag_pointer != (tag_data->end())) {
    curr_tag = tag_pointer->first;
    curr_stat = tag_pointer->second;
    if (curr_stat == jfts_NONE) NoneHolder->AddTag(curr_tag);
    else if (curr_stat == jfts_INCLUDE) IncludeHolder->AddTag(curr_tag);
    else if (curr_stat == jfts_EXCLUDE) ExcludeHolder->AddTag(curr_tag);
    else if (curr_stat == jfts_ALTERNATE){
      if (has_alternate) AlternateHolder->AddTag(curr_tag);
      else if (alt_to_none) NoneHolder->AddTag(curr_tag);
      else return false;
    }
    tag_pointer++;
  }
  // done
  return true;
}

//--------------------------------------
bool jfTagSorter::SaveToTagData(bool make_data) {
  const QString fname = "jfTagSorter::SaveToTagData";
  if (tag_data==NULL){
    if (make_data) tag_data = new jfTagListing();
    else return false;
  }
  // saving the various holders
  SaveTagsForHolder(NoneHolder);
  SaveTagsForHolder(IncludeHolder);
  SaveTagsForHolder(ExcludeHolder);
  SaveTagsForHolder(AlternateHolder);
  return true;
}
//---------------------------------
void jfTagSorter::SaveTagsForHolder(jfTagContainer* hold_ptr) {
  if (hold_ptr!=NULL) {
    QStringList* clist;
    QString ctag;
    // none
    clist = hold_ptr->Contents();
    if (clist==NULL) return;
    for (size_t cindex = 0 ; cindex < clist->size() ; cindex++ ){
      ctag = clist->at(cindex);
      (*tag_data)[ctag] = hold_ptr->getStatusType();
    }
    delete clist;
  }
}
//---------------------------------
bool jfTagSorter::SetTagLabel(const QString& tag_label) {
  if (tag_label.isEmpty()) return false;
  // setting the labels
  NoneHolder->SetTagLabel(tag_label);
  IncludeHolder->SetTagLabel(tag_label);
  ExcludeHolder->SetTagLabel(tag_label);
  if (has_alternate) AlternateHolder->SetTagLabel(tag_label);
  return true;
}

/*****************************************************************************/
