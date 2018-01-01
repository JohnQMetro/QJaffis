/*****************************************************************************
Name:        html_display.cpp
Author :     John Q Metro
Purpose :    HTML listbox for showing results
Created:     April 26, 2011
Conversion Started : August 27, 2013
Updated:     July 31, 2016
******************************************************************************/
#ifndef HTML_DISPLAY_H_INCLUDED
  #include "html_display.h"
#endif // HTML_DISPLAY_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#include <assert.h>
//----------------------------------
#include <QListWidgetItem>
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>


//****************************************************************************
// using info from the internt at stackoverflow.com
//-------------------------------------------------
HtmlDelegate::HtmlDelegate(jfQSizeCache* in_sc_ptr, jfExtendedModel* model_ptr, QObject* parent):QStyledItemDelegate(parent) {
  sc_ptr = in_sc_ptr;
  modelptr = model_ptr;
  HtmlDelegate::basewidth = 0;
}
//-----------------------------------------------
void HtmlDelegate::ResetWidth() {
  basewidth = 0;
}
//-------------------------------------------------
void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  // variables and constants
    const QString fname = " %%%% &&&& HtmlDelegate::paint";
    QStyleOptionViewItemV4 optionV4 = option;
    QString html_data;
    QTextDocument doc;
  // some initialization
    initStyleOption(&optionV4, index);
    QStyle *style = (optionV4.widget)? (optionV4.widget->style()) : (QApplication::style());
    // setting the html document
    doc.setHtml(optionV4.text);
    int xwidth = optionV4.rect.width();
    if (basewidth == xwidth) basewidth = xwidth;
    doc.setTextWidth(xwidth);
    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);
    QAbstractTextDocumentLayout::PaintContext ctx;
    QColor xcolour = modelptr->getColorAtModelIndex(index);
    /**/JDEBUGLOGB(fname,1,xcolour.isValid());
    if (xcolour.isValid()) {
        /**/JDEBUGLOG(fname,2);
        ctx.palette.setColor(QPalette::Window,xcolour);
    }
    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected) {
      // ctx.palette.setColor(QPalette::Highlight,Qt::red);
      //ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));
      // ctx.palette.setColor(QPalette::Text,Qt::red);
    }
    /**/JDEBUGLOG(fname,3);
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}
//-----------------------------------------------------------------------------
QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
  const QString fname = " ^^^^ &&&& HtmlDelegate::sizeHint";
  // checking the width, option.rec.width is unreliable, but it might be all that we have
  int defwidth = (basewidth != 0)?(basewidth):(option.rect.width());
  // initial size cache check
  jfSizeCacheData* tpr = sc_ptr->object(index.row());
  if (tpr == NULL) {
    tpr = new jfSizeCacheData;
    tpr->widd4 = 999999999;
    sc_ptr->insert(index.row(),tpr);
  }
  // check to see if things are okay to use cache
  if ((tpr->widd4) != defwidth) {
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);
    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setTextWidth(defwidth);
    tpr->widd4 = defwidth;
    tpr->width = doc.idealWidth();
    tpr->height = doc.size().height();
  }
  return QSize(tpr->width,tpr->height);
}
//=========================================================================
jfExtendedModel::jfExtendedModel(size_t in_res_index):QAbstractListModel() {
  assert(in_res_index<33);
  res_index = in_res_index;
  itemcount = 0;
  catptr = -1;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// standard functions to implement
//-------------------------
int jfExtendedModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return itemcount;
}
//-------------------------
QVariant jfExtendedModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();
  int irow = index.row();
  if (irow>=itemcount) return QVariant();
  // returning the display data
  if (role == Qt::DisplayRole) {
    if (data_store[irow]!=NULL) return data_store[irow]->GetHTML();
    else return cat_store[irow];
  }
  else if (role == Qt::UserRole) {
      if (data_store[irow]!=NULL) return QVariant::fromValue(data_store[irow]);
    else return QVariant();
  }
  else return QVariant();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// might as well
//-------------------------
void jfExtendedModel::AppendItem(jfResultUnit* in_result) {
  if (in_result==NULL) return;
  beginInsertRows(QModelIndex(),itemcount,itemcount);
  data_store.append(in_result);
  cat_store.append(QString());
  itemcount++;
  endInsertRows();
}
//-------------------------
void jfExtendedModel::AppendHtml(const QString& inhtml) {
  beginInsertRows(QModelIndex(),itemcount,itemcount);
  data_store.append(NULL);
  cat_store.append(inhtml);
  itemcount++;
  endInsertRows();
}
//-------------------------
void jfExtendedModel::AppendCatHeader(const QString& inhtml) {
  beginInsertRows(QModelIndex(),itemcount,itemcount);
  data_store.append(NULL);
  cat_store.append(inhtml);
  itemcount++;
  catptr = itemcount-1;
  endInsertRows();
}
//-------------------------
void jfExtendedModel::NullCatHeader() {
  catptr = -1;
}

//-------------------------
bool jfExtendedModel::UpdateCatHeader(const QString& inhtml) {
  if (catptr<0) return false;
  QModelIndex rowref = index(catptr);
  cat_store[catptr] = inhtml;
  emit dataChanged(rowref,rowref);
  return true;
}
//-------------------------
bool jfExtendedModel::SignalRowChanged(size_t wrow) {
    if (wrow >= itemcount) return false;
    QModelIndex rowref = index(wrow);
    emit dataChanged(rowref,rowref);
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
// the method for which this class was created
void jfExtendedModel::AppendSelectedItems(jfResultUnitVector* in_results, size_t addcount) {
  // do nothing
  assert(addcount>0);
  assert(in_results!=NULL);
  // special variables
  jfResultUnit* cval;
  size_t icount = in_results->size();
  bool addthis;
  // preparing for the addition loop
  beginInsertRows(QModelIndex(),itemcount,(itemcount+addcount)-1);  // addcount must be accurate!
  // the addition loop
  for (size_t ndex = 0; ndex < icount; ndex++) {
    cval = in_results->at(ndex);
    // an annoyingly complicated set of conditions
    addthis = ((res_index<32) && (cval->Marked(res_index)));
    addthis = (addthis || ( (res_index==32)&& (!(cval->HasMark())) ) );
    // appending
    if (addthis) {
      data_store.append(cval);
      cat_store.append(QString());
    }
  }
  // after the loop
  itemcount += addcount;
  endInsertRows();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
// a simplified version of the above
void jfExtendedModel::AppendItems(jfResultUnitVector* in_results) {
  // assert(addcount>0);
  assert(in_results!=NULL);
  // special variables
  jfResultUnit* cval;
  size_t icount = in_results->size();
  // preparing for the addition loop
  beginInsertRows(QModelIndex(),itemcount,(itemcount+icount)-1);
  // the addition loop
  for (size_t ndex = 0; ndex < icount; ndex++) {
    cval = in_results->at(ndex);
    data_store.append(cval);
    cat_store.append(QString());
  }
  // after the loop
  itemcount += icount;
  endInsertRows();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
// informational method
size_t jfExtendedModel::getItemcount() const {
  return itemcount;
}
//-------------------------
jfResultUnit* jfExtendedModel::getDataAtModelIndex(const QModelIndex& inval) {
  if (!inval.isValid()) return NULL;
  if (inval.row()>=itemcount) return NULL;
  return data_store[inval.row()];
}
//-------------------------
QColor jfExtendedModel::getColorAtModelIndex(const QModelIndex& inval) const {
    if (inval.isValid()) return QColor();
    if (inval.row()>=itemcount) return QColor();
    if (data_store[inval.row()]==NULL) return QColor();
    else return (data_store[inval.row()])->bgcolor;
}
//=========================================================================
// constructor
jfHtmlListView::jfHtmlListView(QWidget * parent, size_t resindex):QListView(parent) {
  res_index = resindex;
  datastore = new jfExtendedModel(resindex);
  // setting
  setModel(datastore);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  // setResizeMode(QListView::Adjust);

  QPalette palettex = palette();
  palettex.setColor(QPalette::Highlight,QColor::fromRgb(245,245,255));
  setPalette(palettex);

  size_cache = new jfQSizeCache(1000);
  htmlshow = new HtmlDelegate(size_cache,datastore);
  setItemDelegate(htmlshow);
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this,SIGNAL(customContextMenuRequested(QPoint)),SLOT(handleRightClick(QPoint)));
}
//-------------------------
jfHtmlListView::~jfHtmlListView() {
  delete datastore;
  delete htmlshow;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// external methods to add items/text
int jfHtmlListView::AppendItem(jfResultUnit* in_result) {
  if (in_result==NULL) return -1;
  datastore->AppendItem(in_result);
  return (datastore->getItemcount());
}
//-------------------------------------------------------
int jfHtmlListView::AppendItems(jfResultUnitVector* in_results, bool doheader, QString inheader) {
  const QString fname = "jfHtmlListView::AppendItems";
  if (in_results==NULL) return -1;
  // special variables
  jfResultUnit* cval;
  size_t icount = in_results->size();
  size_t addcount = 0;
  /**/JDEBUGLOGI(fname,1,sizeHintForColumn(0))
  // the loop that counts how many items we *do* add
  for (size_t idex = 0; idex < icount; idex++) {
    cval = in_results->at(idex);
    if (res_index<32) {
      if (cval->Marked(res_index)) addcount++;
    }
    else if (!(cval->HasMark())) addcount++;
  }
  // the nothing to add case
  if (addcount==0) return 0;
  // doing the additions
  if (doheader) AppendCatHeader(inheader);
  datastore->AppendSelectedItems(in_results,addcount);
  // finishing off
  /**/JDEBUGLOGI(fname,2,sizeHintForColumn(0))
  return addcount;
}
//-------------------------------------------------------
int jfHtmlListView::AppendItemsSimple(jfResultUnitVector* in_results, bool doheader, QString inheader) {
  const QString fname = "jfHtmlListView::AppendItemsSimple";
  if (in_results==NULL) return -1;
  size_t icount = in_results->size();
  if (icount==0) return 0;
  // doing the additions
  /**/JDEBUGLOGI(fname,1,sizeHintForColumn(0))
  if (doheader) AppendCatHeader(inheader);
  datastore->AppendItems(in_results);
  // finishing off
  /**/JDEBUGLOGI(fname,2,sizeHintForColumn(0))
  return icount;
}
//-------------------------------------------------------
int jfHtmlListView::AppendHtml(QString inhtml) {
  datastore->AppendHtml(inhtml);
  return (datastore->getItemcount());
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// category header related
int jfHtmlListView::AppendCatHeader(const QString& inhtml) {
  datastore->AppendCatHeader(inhtml);
  return (datastore->getItemcount());
}
//-------------------------------------------------------
bool jfHtmlListView::UpdateCatHeader(const QString& inhtml) {
  return datastore->UpdateCatHeader(inhtml);
}
//-------------------------------------------------------
void jfHtmlListView::SetNoCatHeader() {
  datastore->NullCatHeader();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfResultUnit* jfHtmlListView::GetByPoint(const QPoint& p) {
  QModelIndex mptr = indexAt(p);
  return datastore->getDataAtModelIndex(mptr);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfHtmlListView::handleRightClick(const QPoint& pos) {
    jfRClickData outdata;
    QModelIndex mptr = indexAt(pos);
    outdata.row_index = mptr.row();
    outdata.item_clicked = datastore->getDataAtModelIndex(mptr);
    if (outdata.item_clicked != NULL) {
        outdata.list_index = res_index;
        outdata.model_pointer = datastore;
        emit SendRightClick(outdata,pos);
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfHtmlListView::resizeEvent( QResizeEvent* e ) {
  if (htmlshow!=NULL) htmlshow->ResetWidth();
  QListView::resizeEvent(e);
}

//*************************************************************************
jfHtmlResultCollection::jfHtmlResultCollection(header_enum inhtype, QWidget* parent):QWidget(parent) {
  // basic info init
  hype = inhtype;
  numitems = 0;
  use_default = 0;
  num_rcats = 0;
  tpcount = 0;
  // nulling values
  mgroup = NULL;
  popup_menu = NULL;
  for (size_t q = 0;q<33;q++) panels[q] = NULL;
  // GUI layout
  mainbook = new QTabWidget();
  result_label = new QLabel("Not Started");
  brlayout = new QHBoxLayout();
  mlayout = new QVBoxLayout();
  mlayout->addWidget(mainbook,1);
  brlayout->addWidget(result_label,1,Qt::AlignLeft);
  mlayout->addLayout(brlayout,0);
  // setting the layout
  setLayout(mlayout);
}

//+++++++++++++++++++++++++++++++++++++++++++
// extenal custom methods
//------------------------------------------
void jfHtmlResultCollection::SetPopupMenu(jfPopupMenu* inmenu) {
  assert(inmenu!=NULL);
  popup_menu = inmenu;
}
//------------------------------------------
bool jfHtmlResultCollection::SetupViewers(jfCategories* input) {
  // checks and asserts
  assert(input!=NULL);
  // setting up some basic values
  use_default = input->UsesDefault();
  num_rcats = input->GetCount();
  if (use_default) tpcount = 1;
  else tpcount = 0;
  tpcount += num_rcats;
  // the data
  mgroup = new jfDisplayIndexGroup(hype,use_default,num_rcats);
  // we set up the HTML viewers
  if (use_default) {
    panels[32] = new jfHtmlListView(NULL,32);
    mainbook->addTab(panels[32],"Default");
    ConnectRClick(32);
  }
  for(size_t qrun=0;qrun<num_rcats;qrun++) {
    panels[qrun] = new jfHtmlListView(mainbook,qrun);
    mainbook->addTab(panels[qrun],input->NameAtIndex(qrun));
    ConnectRClick(qrun);
  }
  // done
  return true;
}
//------------------------------------------
bool jfHtmlResultCollection::ClearViewers() {
  const QString fname = "jfHtmlResultCollection::ClearViewers";
  // variables
  size_t offset;
  // checks and asserts
  offset = (use_default)?1:0;
  // removing the HTML Viewers list
  for(int qrun=(num_rcats-1);qrun>=0;qrun--) {
    mainbook->removeTab(qrun+offset);
    delete panels[qrun];
    panels[qrun] = NULL;
  }
  if (use_default) {
    mainbook->removeTab(0);
    delete panels[32];
    panels[32] = NULL;
  }
  // deleting the data
  delete mgroup;
  mgroup = NULL;
  // setting the default values
  use_default = false;
  num_rcats = 0;
  numitems = 0;
  tpcount = 0;
  result_label->setText("Not Started");
  // done
  return true;
}
//------------------------------------------
bool jfHtmlResultCollection::CheckRCounts(bool uses_default,size_t ch_rcats) const {
  if (uses_default!=uses_default) return false;
  return (ch_rcats==num_rcats);
}
//--------------------------------------------
int jfHtmlResultCollection::SelectedIndex() const {
  return mainbook->currentIndex();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfHtmlResultCollection::HandleRightClick(jfRClickData clickData, const QPoint& pout) {
  if (popup_menu!=NULL) {
    popup_menu->SetPopupData(clickData);
    popup_menu->popup(panels[clickData.list_index]->mapToGlobal(pout));
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfHtmlResultCollection::UpdateResultsLabel() {
  QString labfile;
  if (numitems==0) labfile = "No";
  else labfile = QString::number(numitems);
  labfile += " Results";
  result_label->setText(labfile);
}
//--------------------------------------------------------------------
bool jfHtmlResultCollection::ConnectRClick(size_t inpan) {
  if (panels[inpan]==NULL) return false;
  else {
    connect(panels[inpan],SIGNAL(SendRightClick(jfRClickData, const QPoint&)),
            this,SLOT(HandleRightClick(jfRClickData, const QPoint&)));
    return true;
  }
}

//****************************************************************************
