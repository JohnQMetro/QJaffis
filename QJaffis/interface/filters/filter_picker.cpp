/******************************************************************************
Name    :   filter_picker.cpp
Author  :   John Q Metro
Purpose :   Defines some custom interface elements for a filter picker dialog.
Created :   March 25,
Conversion to Qt Started Oct 3, 2013
Updated :   October 29, 2013
******************************************************************************/
#ifndef FILTER_WIDGETS_H_INCLUDED
  #include "filter_picker.h"
#endif // FILTER_WIDGETS_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef JFILTERGLOBALS
  #include "../../core/filters/filter_globals.h"
#endif
//----------------------------------------------------
#include <QHeaderView>
#include <assert.h>
#include <math.h>
#include <QSize>
//*****************************************************************************
// the constructor
jfFilterPickerBase::jfFilterPickerBase(bool three_cols, QWidget* parent):QTableWidget(parent) {
  QStringList collabels;
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  // with the basic constructor done, we add the 3 columns
  collabels.append("Filter Name");
  collabels.append("Filter Type");
  if (three_cols) {
    collabels.append("Description");
    setColumnCount(3);
  }
  else setColumnCount(2);
  setHorizontalHeaderLabels(collabels);
  if (three_cols) horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
  else horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
  // setting local variables
  fnames = NULL;
  ftypes = NULL;
  fdesc = NULL;
  loaded = false;
  three_columns = three_cols;
  old_selection = -1;
  select_index = -1;
  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(OnItemSelect()));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// uses information from data the set the list
size_t jfFilterPickerBase::SetContents(const jfFilterMap* data, QString inprefix) {
  size_t loopv, tcount;
  // some checks
  assert(data!=NULL);
  // clearing the old stuff
  clearContents();
  // a special case
  tcount = data->GetCount();
  prefix = inprefix;
  if (tcount==0) return 0;
  // extracting the lists
  fnames = data->GetNameList();
  if (three_columns) fdesc = data->GetDescriptionList();
  ftypes = data->GetTypeList();
  loaded = true;
  // now, do do the loading list
  for (loopv=0;loopv<tcount;loopv++) {
    InsertFLine(loopv);
  }
  // we no longer need the lists
  ClearLoaded();
  // setting the column width
  resizeColumnToContents(0);
  if (three_columns) resizeColumnToContents(1);
  // done
  return tcount;
}
//----------------------------------------------------------------------------
bool jfFilterPickerBase::ClearLoaded() {
  if (loaded) {
    fnames->clear();
    delete fnames;
    fnames = NULL;
    if (three_columns) {
      fdesc->clear();
      delete fdesc;
      fdesc = NULL;
    }
    ftypes->clear();
    delete ftypes;
    ftypes = NULL;
    loaded = false;
    return true;
  }
  else return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterPickerBase::IsFilterSelected() const {
  return sloaded;
}
//---------------------------------------------------------------------------
bool jfFilterPickerBase::ChangeSelected(size_t nindex) {
  // if there is a selected item, we unselect it
  if (sloaded) Deselect();
  // we select
  return SelectNoEvent(nindex,true);
}
//----------------------------------------------------------------------------
long jfFilterPickerBase::SelectedIndex() const {
  if (sloaded) return select_index;
  else return -1;
}
//----------------------------------------------------------------
long jfFilterPickerBase::OldSelected() const {
  return old_selection;
}
//----------------------------------------------------------------------------
QString jfFilterPickerBase::SelectedName() const {
  if (!sloaded) return "";
  else return select_name;
}
//------------------------------------------------------------------------------
void jfFilterPickerBase::Deselect() {
  if (sloaded) SelectNoEvent(select_index,false);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting extra stuff
QString jfFilterPickerBase::NameAt(size_t nindex) const {
  QTableWidgetItem* twi = item(nindex,0);
  return twi->text();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// column sizing (alternative to pixel, uses proportions)
void jfFilterPickerBase::SetColWidth(size_t prop1, size_t prop2, size_t prop3) {
  // variables
  size_t propsum;
  double dprop1, dprop2;
  int cwidth, ssize1, ssize2;
  QSize controlsize;
  // getting the control width
  controlsize = size();
  cwidth = controlsize.width();
  // calculating the fractional proportions
  propsum = prop1 + prop2;
  if (three_columns) propsum += prop3;
  dprop1 = (double)prop1 / (double)propsum;
  dprop2 = (double)prop2 / (double)propsum;
  /*
  if (three_columns) {
    dprop3 = (double)prop3 / (double)propsum;
  }
  */
  // getting the new integer width
  ssize1 = floor(dprop1*cwidth);
  ssize2 = floor(dprop2*cwidth);
  // if (three_columns) ssize3 = floor(dprop3*cwidth);
  // applying the widths
  setColumnWidth(0,ssize1);
  if (three_columns) setColumnWidth(1,ssize2);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// events
void jfFilterPickerBase::OnItemSelect() {
  if (!bypass) {
    // looking for the selected index
    bool atest = SetSelected();
    // we pass the event back up
    if (atest) emit SendSelected();
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterPickerBase::InsertFLine(const size_t& iindex) {
  // variables
  QTableWidgetItem* titem;
  QString buffer;
  // we do some checks first
  assert(loaded);
  assert(iindex<(fnames->count()));
  // creating the new line
  insertRow(iindex);
  // name
  titem = new QTableWidgetItem(fnames->at(iindex));
  setItem(iindex,0,titem);
  // type
  titem = new QTableWidgetItem(ftypes->at(iindex));
  setItem(iindex,1,titem);
  // adding description
  if (three_columns) {
    titem = new QTableWidgetItem(fdesc->at(iindex));
    setItem(iindex,2,titem);
  }
  // done
  return true;
}
//--------------------------------------------------------------------------
bool jfFilterPickerBase::SelectNoEvent(size_t index, bool issel) {
  if (index>=rowCount()) return false;
  bypass = true;
  if (issel) {
    selectRow(index);
    select_name = item(index,0)->text();
    old_selection = select_index;
    select_index = index;
    sloaded = true;

  }
  else {
    clearSelection();
    sloaded = false;
    old_selection = select_index;
    select_index = -1;
    select_name = "";
  }
  bypass = false;
  return true;
}
//-----------------------------------------------------------------------------
bool jfFilterPickerBase::SetSelected() {
  long itemindex = -1;
  itemindex = currentRow();
  // found
  if (itemindex<0) return false;
  select_name = item(itemindex,0)->text();
  old_selection = select_index;
  select_index = itemindex;
  sloaded = true;
  return true;
}
//============================================================================
jfFilterPicker1::jfFilterPicker1(QLabel* displaylink, QWidget* parent):jfFilterPickerBase(true,parent) {
  // most of what we need to do has been handles by the base class
  assert(displaylink!=NULL);
  display = displaylink;
  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(OnItemSelect()));
}
//---------------------------------------------------------------------------
// preoduces the selected filter name. properly escaped and wrapped in @
QString jfFilterPicker1::InsertString() const {
  QString outbuf;
  // starting
  if (!sloaded) return "";
  // if we have a prefix, we encode that first
  if (!(prefix.isEmpty())) {
    outbuf = EscapeString(prefix,EXPR_DELIMS);
    outbuf += ';';
  }
  // next, we add the selected name
  outbuf += EscapeString(select_name,EXPR_DELIMS);
  // we wrap the results in the delimiters
  outbuf = '@'+outbuf+'@';
  // done
  return outbuf;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the only used event
void jfFilterPicker1::OnItemSelect() {
  QString lbuffer;
  bool atest;
  // setting the label
  atest = SetSelected();
  assert(atest);
  lbuffer = InsertString();
  display->setText(lbuffer);
}
//=============================================================================
jfGlobalFilterPicker::jfGlobalFilterPicker(QLabel* displaylink, QWidget* parent):QWidget(parent) {
  // local variables
  assert(displaylink!=NULL);
  // creating the elements
  topsizer = new QHBoxLayout();
  lbox = new QListWidget();
  fpicker = new jfFilterPicker1(displaylink);
  // arranging them
  topsizer->addWidget(lbox,2);
  topsizer->addWidget(fpicker,5);
  setLayout(topsizer);
  // initializing local variables
  loaded = false;
  proxy_link = NULL;
  sel_map = NULL;
  sel_mapname = "";
  connect(lbox, SIGNAL(itemSelectionChanged()), this, SLOT(MapSelected()));
}
//-----------------------------------------------------------------------------
/* we load the data in the Map of Maps we are given */
bool jfGlobalFilterPicker::LoadData(jfMapOfFiltermaps* inlink) {
  QStringList* names_of_maps;
  // local variables
  size_t mom_size;
  // first things first, we check what we get
  if (inlink==NULL) return false;
  // an empty map of maps is treated the same as nothing
  mom_size = inlink->size();
  if (mom_size==0) return false;
  // otherwise, we start
  proxy_link = inlink;
  names_of_maps = proxy_link->GetMapNames();
  assert(names_of_maps!=NULL);
  // loading the names of maps...
  lbox->addItems(*names_of_maps);
  delete names_of_maps;
  // now we select the first one
  lbox->setCurrentRow(0);
  // and load
  LoadPicker(0);
  return true;
}
//----------------------------------------------------------------------------
// sets the data in the picker control to the map at the index
bool jfGlobalFilterPicker::LoadPicker(const size_t& pindex) {
  //local variables
  jfMapOfFiltermaps::iterator indexer;
  size_t counter, maxsize;
  // some initial checking
  assert(proxy_link!=NULL);
  maxsize = proxy_link->size();
  assert(maxsize>0);
  assert(pindex<maxsize);
  /* we loop thru the map, because the map key is string names,
  not integer indexes */
  indexer = proxy_link->begin();
  counter = 0;
  // the loop
  while (counter<pindex) {
    ++indexer;
    counter++;
  }
  // we are at the item, we set values...
  sel_mapname = indexer->first;
  sel_map = indexer->second;
  // the nest step is to load the data into the picker control
  fpicker->SetContents(sel_map,sel_mapname);
  // done (I think)
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfGlobalFilterPicker::IsFilterChosen() const {
  if (loaded) return fpicker->IsFilterSelected();
  else return false;
}
//----------------------------------------------------------------------------
QString jfGlobalFilterPicker::CurrentMapname() const {
  if (loaded) return sel_mapname;
  else return "";
}
//----------------------------------------------------------------------------
QString jfGlobalFilterPicker::InsertString() const {
  if (loaded) return fpicker->InsertString();
  else return "";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGlobalFilterPicker::MapSelected() {
  int sel_item;
  sel_item = lbox->currentRow();
  LoadPicker(sel_item);
}
//============================================================================
jfFilterPickerDialog::jfFilterPickerDialog(const QString& title ,const jfFilterMap* inlocal_map,
      QString* inresult_ptr, QWidget* parent):QDialog(parent) {
  // first, we check on the inputs
  assert(inlocal_map!=NULL);
  assert(inresult_ptr);
  // assign the inputs
  local_map = inlocal_map;
  result_ptr = inresult_ptr;
  // we start  work on control creation
  pick_btn = new QPushButton("Pick");
  cancel_btn = new QPushButton("Cancel");
  labelresult = new QLabel("Filter Reference:");
  showresult = new QLabel("");
  // create and arrange the bottom row
  bottom_sizer = new QHBoxLayout();
  bottom_sizer->addWidget(pick_btn,0);
  bottom_sizer->addWidget(cancel_btn,0);
  bottom_sizer->addSpacing(8);
  bottom_sizer->addWidget(labelresult,0);
  bottom_sizer->addWidget(showresult,1);
  // we create the notebook
  display_book = new QTabWidget();
  // arranging things
  topsizer = new QVBoxLayout();
  topsizer->addWidget(display_book,1);
  topsizer->addLayout(bottom_sizer);
  // we make the pages the pages
  local_page = new jfFilterPicker1(showresult);
  local_page->SetContents(local_map);
  global_page = new jfGlobalFilterPicker(showresult);
  global_page->LoadData(filterglobals::global_filters);
  // adding the pages to the notebook
  display_book->addTab(local_page,"Local Filters");
  display_book->addTab(global_page,"Global Filters");
  // now, set the sizer and recalculate
  setWindowTitle(title);
  setLayout(topsizer);
  connect(pick_btn, SIGNAL(clicked(bool)), this, SLOT(ProcessPick(bool)));
  connect(cancel_btn, SIGNAL(clicked(bool)), this, SLOT(reject()));
  QSize msize = QSize(700,350);
  resize(msize);
  // centre on screen

  // done!
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFilterPickerDialog::ProcessPick(bool checked) {
  (*result_ptr) = showresult->text();
  accept();
}
//============================================================================
jfGlobalFilterPickerDialog::jfGlobalFilterPickerDialog(const QString& title ,QString* inresult_ptr, QWidget* parent):QDialog(parent) {
  // first, we check on the inputs
  assert(inresult_ptr);
  // assign the inputs
  result_ptr = inresult_ptr;
  // we start  work on control creation
  pick_btn = new QPushButton("Pick");
  cancel_btn = new QPushButton("Cancel");
  labelresult = new QLabel("Filter Reference:");
  showresult = new QLabel("");
  // create and arrange the bottom row
  bottom_sizer = new QHBoxLayout();
  bottom_sizer->addWidget(pick_btn,0);
  bottom_sizer->addWidget(cancel_btn,0);
  bottom_sizer->addSpacing(8);
  bottom_sizer->addWidget(labelresult,0);
  bottom_sizer->addWidget(showresult,0);
  // making the panel
  global_page = new jfGlobalFilterPicker(showresult);
  // arranging things
  topsizer = new QVBoxLayout();
  topsizer->addWidget(global_page,1);
  topsizer->addLayout(bottom_sizer,0);
  // loading the data
  global_page->LoadData(filterglobals::global_filters);
  setWindowTitle(title);
  // now, set the sizer and recalculate
  setLayout(topsizer);
  connect(pick_btn, SIGNAL(clicked(bool)), this, SLOT(ProcessPick()));
  connect(cancel_btn, SIGNAL(clicked(bool)), this, SLOT(reject()));
  QSize xsize = QSize(550,350);
  resize(xsize);
  // done!
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGlobalFilterPickerDialog::ProcessPick(bool clicked) {
  (*result_ptr) = showresult->text();
  accept();
}
//*****************************************************************************
