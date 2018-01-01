/*****************************************************************************
Name    : progress_panel2.cpp
Basic   : Child classes of progress panel which are still somewhat generic
Author  : John Q Metro
Started : November 18, 2013
Updated : August 2, 2016
******************************************************************************/
#ifndef PROGRESS_PANEL2_H
  #include "progress_panel2.h"
#endif // PROGRESS_PANEL2_H
//----------------------------
#include <assert.h>
/*****************************************************************************/
// constructor
jfProgressPanelOne::jfProgressPanelOne(bool sidelayout, QWidget* parent):jfProgressPanelBase(parent) {
  xside_layout = sidelayout;
  btn_layout = NULL;
  layout_stack = NULL;
  side_layout = NULL;
  CreateWidgets();
  DoLayout();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// specials
//---------------------------
bool jfProgressPanelOne::DoReset() {
  Reset();
  main_status->setText(statstring);
  return true;
}
//----------------
bool jfProgressPanelOne::ChangeLayout(bool sidelayout) {
  // nothing to do
  if (xside_layout==sidelayout) return false;
  // switching from stacked to side by side
  if (sidelayout) {
    // removing old widgets
    layout_stack->removeItem(btn_layout);
    layout_stack->removeWidget(item_bar);
    layout_stack->removeWidget(item_status);
    // we need to delete the old layout
    delete layout();
    layout_stack = NULL;
  }
  // switching from side by side to stacked
  else {
    // removing
    side_layout->removeItem(layout_stack);
    side_layout->removeItem(btn_layout);
    delete layout();
    side_layout = NULL;
  }
  // finishing
  xside_layout = sidelayout;
  DoLayout();
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
void jfProgressPanelOne::MoreError(bool dohalt) {
  if (dohalt) {
    if (lasterror.why==jff_HALTED) statstring = "HALTED";
    else statstring = "FAILED";
    pause_resume->setEnabled(false);
  }
  else {
    statstring = "SKIPPING";
  }
  main_status->setText(statstring);
}
//--------------------------------------------------
void jfProgressPanelOne::DoLayout() {
  // setting up the buttons first
  if (btn_layout==NULL) {
    btn_layout = new QHBoxLayout();
    btn_layout->addWidget(main_status,0,Qt::AlignVCenter|Qt::AlignRight);
    btn_layout->addWidget(start_stop,0,Qt::AlignRight);
    btn_layout->addWidget(pause_resume,0,Qt::AlignRight);
  }
  // the top to bottom layout
  if (layout_stack==NULL) {
    layout_stack = new QVBoxLayout();
    layout_stack->addWidget(item_status,0,Qt::AlignLeft);
    layout_stack->addWidget(item_bar);
  }
  // next, options...
  if (xside_layout) {
    side_layout = new QHBoxLayout();
    side_layout->addLayout(btn_layout,0);
    side_layout->addLayout(layout_stack,1);
    setLayout(side_layout);
  }
  else {
    layout_stack->addLayout(btn_layout);
    setLayout(layout_stack);
  }
}
//****************************************************************************
jfProgressPanelSingle::jfProgressPanelSingle(bool sidelayout, QWidget* parent):
  jfProgressPanelOne(sidelayout,parent) {}
//-------------------------------------------------------------------
// specials
bool jfProgressPanelSingle::ConnectAndSetPauser(jfBaseDownloader* the_downloader) {
  const QString fname = "jfProgressPanelOne::ConnectAndSetPauser";
  if (the_downloader == NULL) return false;
  bool pstatus = MakePauser();
  if (!pstatus) return false;
  // setting up and connecting the downloader
  pstatus = the_downloader->setPauser(pauser);
  if (!pstatus) return false;
  return ConnectThread(the_downloader);
}

//****************************************************************************
// constructor
jfProgPanelMultiBase::jfProgPanelMultiBase(size_t loption, QWidget* parent):jfProgressPanelBase(parent) {
  // initialization
  assert(loption<3);
  layout_option = loption;
  category_bar = NULL;
  category_status = NULL;
  catindex = 0;
  // using submethods to setup things
  CreateWidgets();
  CreateCatWidgets();
  DoLayout();
}

//----------------------------------------
bool jfProgPanelMultiBase::DoReset() {
  Reset();
  main_status->setText(statstring);
  catindex = 0;
  return true;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfProgPanelMultiBase::HandleCatCount(size_t ccount) {
  assert(ccount!=0);
  category_bar->setMaximum(ccount);
}

//----------------------------------------
void jfProgPanelMultiBase::HandleNewCategory(struct jfItemInfoMsg catdat) {
  catinfo = catdat;
  item_bar->setValue(0);
  item_status->setText("");
  itemindex = 0;
  SetCatLabel("");
}

//----------------------------------------
void jfProgPanelMultiBase::HandleCategoryDone(bool skip) {
  catindex++;
  category_bar->setValue(catindex);
  catinfo.startaction = "Done with";
  SetCatLabel("");
}

//----------------------------------------
void jfProgPanelMultiBase::HandleCatFail() {
  statstring = "FAILED";
  main_status->setText(statstring);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal methods
//----------------------------------------
void jfProgPanelMultiBase::CreateCatWidgets() {
  category_status = new QLabel("Nothing Here Yet");
  category_bar = new QProgressBar();
}
//----------------------------------------
QBoxLayout* jfProgPanelMultiBase::ArrangeButtons(bool stacked, bool sspacer) {
  QBoxLayout* result;
  if (stacked) {
    result = new QBoxLayout(QBoxLayout::TopToBottom);
    result->addWidget(main_status,0);
    result->addWidget(start_stop,0,Qt::AlignCenter);
    result->addWidget(pause_resume,0,Qt::AlignCenter);
  }
  else {
    result = new QBoxLayout(QBoxLayout::LeftToRight);
    if (sspacer) result->addStretch(1);
    result->addWidget(main_status,0,Qt::AlignVCenter);
    result->addWidget(start_stop,0);
    result->addWidget(pause_resume,0);
  }
  return result;
}
//----------------------------------------
QVBoxLayout* jfProgPanelMultiBase::MakeBarStack() {
  QVBoxLayout* x_stack = new QVBoxLayout();
  x_stack->addWidget(category_status,0,Qt::AlignLeft);
  x_stack->addWidget(category_bar);
  x_stack->addSpacing(4);
  x_stack->addWidget(item_status,0,Qt::AlignLeft);
  x_stack->addWidget(item_bar);
  return x_stack;
}
//----------------------------------------
void jfProgPanelMultiBase::SetCatLabel(QString extra) {
  // building the label, bit by bit
  QString themsg = catinfo.startaction + " " + catinfo.item_label;
  if (catinfo.item_index!=-1) {
    themsg += " " + QString::number(catinfo.item_index) + " of ";
    if (catinfo.part_count!=-1) themsg += QString::number(catinfo.part_count);
  }
  if (!(catinfo.item_name.isEmpty())) {
    themsg += " : " + catinfo.item_name;
  }
  if (!extra.isEmpty()) {
    themsg += " , " + extra;
  }
  // setting the label
  category_status->setText(themsg);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented methods
//----------------------------------------
void jfProgPanelMultiBase::MoreError(bool dohalt) {

}

//----------------------------------------
void jfProgPanelMultiBase::DoLayout() {
  assert(layout_option<3);
  /* layout options :
   * 0 everything stacked
   * 1 buttons and status top right
   * 2 buttons and status vertically stacked to left */
  if (layout_option==0) {
    btn_layout = ArrangeButtons(false,true);
    pbar_stack = MakeBarStack();
    pbar_stack->addSpacing(5);
    pbar_stack->addLayout(btn_layout);
    top_stack = NULL;
    horiz_layout = NULL;
    setLayout(pbar_stack);
  }
  else if (layout_option==1) {
    btn_layout = ArrangeButtons(false,false);
    top_stack = new QVBoxLayout();
    top_stack->addWidget(category_status,0,Qt::AlignLeft);
    top_stack->addWidget(category_bar);
    horiz_layout = new QHBoxLayout();
    horiz_layout->addLayout(top_stack,1);
    horiz_layout->addLayout(btn_layout);
    pbar_stack = new QVBoxLayout();
    pbar_stack->addLayout(horiz_layout);
    pbar_stack->addSpacing(5);
    pbar_stack->addWidget(item_status,0,Qt::AlignLeft);
    pbar_stack->addWidget(item_bar);
    setLayout(pbar_stack);
  }
  else {
    btn_layout = ArrangeButtons(true,false);
    pbar_stack = MakeBarStack();
    horiz_layout = new QHBoxLayout();
    horiz_layout->addLayout(btn_layout,0);
    horiz_layout->addLayout(pbar_stack,1);
    top_stack = NULL;
    setLayout(horiz_layout);
  }

}
//*****************************************************************************************
jfProgPanelMultiple:: jfProgPanelMultiple(size_t loption, QWidget* parent):jfProgPanelMultiBase(loption,parent) {

}
//---------------------------------------------------------------------
bool jfProgPanelMultiple::ConnectAndSetPauser(jfMultiCatBaseDownloader* the_downloader) {
  if (the_downloader == NULL)  return false;
  bool pstatus = MakePauser();
  if (!pstatus) return false;
  // setting up and connecting the downloader
  the_downloader->setPauser(pauser);
  ConnectThread(the_downloader);
  connect(the_downloader,SIGNAL(SendCatCount(size_t)),this,SLOT(HandleCatCount(size_t)));
  connect(the_downloader,SIGNAL(SendNewCategory(struct jfItemInfoMsg)),this,SLOT(HandleNewCategory(struct jfItemInfoMsg)));
  connect(the_downloader,SIGNAL(SendCategoryDone(bool)),this,SLOT(HandleCategoryDone(bool)));
  connect(the_downloader,SIGNAL(SendCatFail()),this,SLOT(HandleCatFail()));
  return true;
}

/*****************************************************************************/
