/******************************************************************************
Name    :   tag_widgets2.cpp
Author  :   John Q Metro
Purpose :   Tag categorizing widgets
Created :   July 27, 2016
Updated :   July 29, 2016
******************************************************************************/
#ifndef TAG_WIDGETS2_H
  #include "tag_widgets2.h"
#endif // TAG_WIDGETS2_H
//----------------------------------------
#include <QColor>
#include <QRegion>
#include <algorithm>
//----------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
/*****************************************************************************/
// --- [ Methods for jfTagRowView [ ----------------------------
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfTagRowView::jfTagRowView(bool do_alternate, QWidget* parent):QWidget(parent) {
  has_alternate = do_alternate;
  tstatus = jfts_NONE;
  SetupWidgets("Default");
}
//------------------------------------
jfTagRowView::jfTagRowView(bool do_alternate, QString tagtext, QWidget* parent):QWidget(parent) {
  has_alternate = do_alternate;
  tstatus = jfts_NONE;
  SetupWidgets(tagtext);
}
//------------------------------------
bool jfTagRowView::SetStatus(jfTAG_STATUS in_status) {
  if (in_status != tstatus) {
    if (in_status != jfts_NONE) {
      if (in_status == jfts_INCLUDE) handleIncludeClicked(true);
      else if (in_status == jfts_EXCLUDE) handleExcludeClicked(true);
      else if (in_status == jfts_ALTERNATE) {
        if (!has_alternate) return false;
        else handleAlternateClicked(true);
      }
    }
    else {
      // setting things to none
      if (tstatus == jfts_INCLUDE) include_btn->setPalette(*unpicked);
      else if (tstatus == jfts_EXCLUDE) exclude_btn->setPalette(*unpicked);
      else if (tstatus == jfts_ALTERNATE) alternate_btn->setPalette(*unpicked);
      else return false;
      tstatus = jfts_NONE;
      emit statusChange();
    }
  }
  return true;
}
//------------------------------------
bool jfTagRowView::SetTagString(const QString& intaglabel) {
  if(intaglabel.isEmpty()) return false;
  tag_display->setText(intaglabel);
  return true;
}
//------------------------------------
jfTAG_STATUS jfTagRowView::GetStatus() const {
  return tstatus;
}
//------------------------------------
QString jfTagRowView::GetTagString() const {
  return tag_display->text();
}
//------------------------------------
bool jfTagRowView::CheckAndSetStatus(const QString& ctaglabel,jfTAG_STATUS in_status) {
  if (ctaglabel != (tag_display->text())) return false;
  return SetStatus(in_status);
}
//----------------------------------
size_t jfTagRowView::getButtonSize() const {
  return button_size;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfTagRowView::handleIncludeClicked(bool checked) {
  const QString fname ="jfTagRowView::handleIncludeClicked";
  /**/JDEBUGLOG(fname,1);
  if (tstatus == jfts_INCLUDE) {
    include_btn->setPalette(*unpicked);
    tstatus = jfts_NONE;
  }
  else {
    exclude_btn->setPalette(*unpicked);
    include_btn->setPalette(*inc_picked);
    if (has_alternate) alternate_btn->setPalette(*unpicked);
    tstatus = jfts_INCLUDE;
  }
  emit statusChange();
}
//------------------------------------
void jfTagRowView::handleExcludeClicked(bool checked) {
  if (tstatus == jfts_EXCLUDE) {
    exclude_btn->setPalette(*unpicked);
    tstatus = jfts_NONE;
  }
  else {
    exclude_btn->setPalette(*excl_picked);
    include_btn->setPalette(*unpicked);
    if (has_alternate) alternate_btn->setPalette(*unpicked);
    tstatus = jfts_EXCLUDE;
  }
  emit statusChange();
}
//------------------------------------
void jfTagRowView::handleAlternateClicked(bool checked) {
  if (tstatus == jfts_ALTERNATE) {
    alternate_btn->setPalette(*unpicked);
    tstatus = jfts_NONE;
  }
  else {
    exclude_btn->setPalette(*unpicked);
    include_btn->setPalette(*unpicked);
    alternate_btn->setPalette(*alt_picked);
    tstatus = jfts_ALTERNATE;
  }
  emit statusChange();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfTagRowView::SetupWidgets(QString tagtext) {
  // creating the widgets
  tag_display = new QLabel(tagtext);
  include_btn = new QPushButton(QString(QChar(0x2714)));
  exclude_btn = new QPushButton(QString(QChar(0x2716)));
  alternate_btn = new QPushButton("1+");
  // finding the button size
  size_t textWidth1 = include_btn->fontMetrics().boundingRect(include_btn->text()).width();
  size_t textWidth2 = exclude_btn->fontMetrics().boundingRect(exclude_btn->text()).width();
  size_t textWidth3 = alternate_btn->fontMetrics().boundingRect(alternate_btn->text()).width();
  button_size = std::max(std::max(textWidth1,textWidth2),textWidth3) + 6;
  // setting up the buttons
  setupPalettes();
  setupButton(include_btn);
  setupButton(exclude_btn);
  // the alternates button
  if (!has_alternate){
    delete alternate_btn;
    alternate_btn = NULL;
  }
  else setupButton(alternate_btn);
  // top level layout
  layout = new QHBoxLayout();
  layout->setMargin(1);
  layout->addWidget(tag_display,1,Qt::AlignVCenter);
  layout->addWidget(include_btn,0);
  layout->addWidget(exclude_btn,0);
  if (has_alternate) {
    layout->addWidget(alternate_btn);
  }
  setLayout(layout);
  // connections
  connect(include_btn,SIGNAL(clicked(bool)),this,SLOT(handleIncludeClicked(bool)));
  connect(exclude_btn,SIGNAL(clicked(bool)),this,SLOT(handleExcludeClicked(bool)));
  if (has_alternate) {
    connect(alternate_btn,SIGNAL(clicked(bool)),this,SLOT(handleAlternateClicked(bool)));
  }

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setup helpers
//------------------------------------
void jfTagRowView::setupPalettes() {
  inc_picked = new QPalette();
  inc_picked->setColor(QPalette::ButtonText,Qt::darkGreen);
  inc_picked->setColor(QPalette::Button,QColor::fromRgb(215,255,215));
  excl_picked = new QPalette();
  excl_picked->setColor(QPalette::ButtonText,Qt::darkRed);
  excl_picked->setColor(QPalette::Button,QColor::fromRgb(255,215,215));
  alt_picked = new QPalette();
  alt_picked->setColor(QPalette::ButtonText,QColor::fromRgb(255,127,0));
  alt_picked->setColor(QPalette::Button,QColor::fromRgb(255, 239, 213));
  unpicked = new QPalette();
  unpicked->setColor(QPalette::ButtonText,Qt::lightGray);
  unpicked->setColor(QPalette::Button,QColor::fromRgb(250,250,250));
}

//------------------------------------
void jfTagRowView::setupButton(QPushButton* tbtn) {
  if (tbtn == NULL) return;
  tbtn->setFlat(true);
  tbtn->setAutoFillBackground(true);
  tbtn->setMinimumWidth(button_size);
  QSize bsize = QSize(button_size,button_size);
  tbtn->setFixedSize(bsize);
  /*
  QRect *rect = new QRect(0,0,button_size,button_size);
  QRegion* round = new QRegion(*rect,QRegion::Ellipse);
  tbtn->setMask(*round);
  */
  tbtn->setPalette(*unpicked);
  QFont f = tbtn->font();
  f.setStyleStrategy(QFont::PreferAntialias);
  tbtn->setFont(f);
}
/*****************************************************************************/
jfTagStatusPicker::jfTagStatusPicker(QString header, bool do_alternate, QWidget* parent):QWidget(parent) {
  has_alternate = do_alternate;
  tag_data = NULL;
  internal = false;

  main_list = new QListWidget();
  main_list->setUniformItemSizes(true);

  top_label = new QLabel(header);
  tsizer = new QVBoxLayout();
  tsizer->setMargin(3);
  tsizer->addWidget(top_label,0,Qt::AlignLeft);
  tsizer->addWidget(main_list,1);

  setLayout(tsizer);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
// load and get tags
//----------------------------------------
bool jfTagStatusPicker::SetTagList(jfTagListing* in_tags) {
  if (in_tags == NULL) return false;
  if (tag_data != NULL) ClearList();
  tag_data = in_tags;
  return SetListFromTags();
}
//----------------------------------------
bool jfTagStatusPicker::ChangeTagStatuses(jfTagListing* in_tags) {
  if (in_tags == NULL) return false;
  if (tag_data == NULL) return false;
  tag_data = in_tags;
  return LoadStatusesToList();
}
//----------------------------------------
bool jfTagStatusPicker::SetOrChangeTags(jfTagListing* in_tags) {
    const QString fname = "jfTagStatusPicker::SetOrChangeTags";
    /**/JDEBUGLOG(fname,1)
  if (in_tags == NULL) return false;
    /**/JDEBUGLOG(fname,2)
  if ((in_tags->size()) != (main_list->count())) {
        /**/JDEBUGLOG(fname,3)
    return SetTagList(in_tags);
  }
  else {
        /**/JDEBUGLOG(fname,4)
    return ChangeTagStatuses(in_tags);
  }
}

//----------------------------------------
jfTagListing* jfTagStatusPicker::GetTagList() {
  SaveStatusesFromList();
  return tag_data;
}
//----------------------------------------
bool jfTagStatusPicker::SetDefault(const QStringList* tags) {
  if (tags==NULL) return false;
  if (tag_data != NULL) {
    jfTagListing* stag_data = tag_data;
    ClearList();
    if (internal) delete stag_data;
  }
  tag_data = new jfTagListing();
  tag_data->FromStrArray(tags);
  internal = true;
  return SetListFromTags();
}
//----------------------------------------
bool jfTagStatusPicker::SetDefault(const QString tag_array[], size_t tagcount) {
  if (tag_array==NULL) return false;
  if (tagcount == 0) return false;
  if (tag_data != NULL) {
    jfTagListing* stag_data = tag_data;
    ClearList();
    if (internal) delete stag_data;
  }
  tag_data = new jfTagListing();
  tag_data->FromArray(tag_array,tagcount);
  internal = true;
  return SetListFromTags();
}
//-----------------------------------------
void jfTagStatusPicker::handleStatusChange() {
  main_list->update();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//----------------------------------------
bool jfTagStatusPicker::SetListFromTags() {
  if (tag_data ==NULL) return false;
  const QString fname = "jfTagStatusPicker::SetListFromTags";
  // variables
  QString tag;
  jfTAG_STATUS tag_status;
  QListWidgetItem* witem;
  jfTagRowView* cview;
  QSize minsize;
  bool minsize_defined = false;
  jfTagListing::const_iterator tagdex = tag_data->begin();
  // the main loop
  while (tagdex != (tag_data->end())) {
    tag = tagdex->first;
    tag_status = tagdex->second;
    // creating the custom view
    cview = new jfTagRowView(has_alternate,tag);
    /**/JDEBUGLOGI(fname,1,cview->getButtonSize());
    cview->SetStatus(tag_status);
    tagview_list.push_back(cview);
    // adding it to the list widget
    witem = new QListWidgetItem();
    if (! minsize_defined) {
      minsize = QSize(100,7 + cview->getButtonSize());
      minsize_defined = true;
    }
    witem->setSizeHint(minsize);
    connect(cview,SIGNAL(statusChange()),this,SLOT(handleStatusChange()));
    main_list->addItem(witem);
    main_list->setItemWidget(witem,cview);
     // done
    tagdex++;
  }
  // done
  return true;
}

//----------------------------------------
bool jfTagStatusPicker::LoadStatusesToList() {
  if (tag_data == NULL) return false;
  if ((tag_data->size())!=(main_list->count())) return false;
  // we do not check the actual tags
  jfTagListing::const_iterator tagdex = tag_data->begin();
  size_t lindex = 0;
  jfTAG_STATUS tag_status;
  // the main loop
  while (tagdex != (tag_data->end())) {
    tag_status = tagdex->second;
    tagview_list[lindex]->SetStatus(tag_status);
    // done
    tagdex++;
    lindex++;
  }
  return true;
}

//----------------------------------------
bool jfTagStatusPicker::SaveStatusesFromList() {
  if (tag_data == NULL) return false;
  if ((tag_data->size())!=(main_list->count())) return false;
  // variables
  QString curr_tag;  jfTAG_STATUS curr_status;
  // the loop
  for (size_t tindex = 0; tindex < tagview_list.size() ; tindex ++) {
    curr_tag = tagview_list[tindex]->GetTagString();
    curr_status = tagview_list[tindex]->GetStatus();
    (*tag_data)[curr_tag] = curr_status;
  }
  // done
  return true;
}

//----------------------------------------
void jfTagStatusPicker::ClearList() {
  main_list->clear();
  tagview_list.clear();
  tag_data = NULL;
}

/*****************************************************************************/
