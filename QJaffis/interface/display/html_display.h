/*****************************************************************************
Name:        html_display.h
Author :     John Q Metro
Purpose :    HTML listbox for showing results
Created:     April 26, 2011
Conversion Started : August 27, 2013
Updated:     July 13 2017 (Extra right click data)
******************************************************************************/
#ifndef HTML_DISPLAY_H_INCLUDED
#define HTML_DISPLAY_H_INCLUDED
#endif // HTML_DISPLAY_H_INCLUDED
//----------------------------------------------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef CATEGORIES_H_INCLUDED
  #include "../../core/structs/categories.h"
#endif // CATEGORIES_H_INCLUDED
#ifndef DISPLAYINDEX_H
  #include "displayindex.h"
#endif // DISPLAYINDEX_H
#ifndef MENU_H_INCLUDED
  #include "../misc/menu.h"
#endif // MENU_H_INCLUDED
//----------------------------------
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QTabWidget>
#include <QLabel>
// #include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextOption>
#include <QCache>
#include <QListView>
#include <QStandardItemModel>
//****************************************************************************
// quick datatype for QSize cache.
typedef struct jCData {
    int widd4;
    int width;
    int height;
} jfSizeCacheData;

typedef QCache<int,jfSizeCacheData> jfQSizeCache;

//===========================================================================
class jfHtmlResultCollection;
class jfExtendedModel;

/* Qt provides no equivalent to wxHtmlListBox, but one can be built via QListWidget and
 a Delegate that uses QTextDocument. */
class HtmlDelegate : public QStyledItemDelegate {
  public:
    HtmlDelegate(jfQSizeCache* in_sc_ptr, jfExtendedModel* model_ptr, QObject* parent=0);
    void ResetWidth();
  protected:
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    jfQSizeCache* sc_ptr;
    jfExtendedModel* modelptr;
    mutable size_t basewidth;

};

//================================================================================
/* Qt provides complex classes that lack features. I have to subclass to add multiple
 *  items at once to a 'model' */
class jfExtendedModel : public QAbstractListModel {
  public:
    // the constructor
    jfExtendedModel(size_t in_res_index);
    // standard functions to implement
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    // might as well
    void AppendItem(jfResultUnit* in_result);
    void AppendHtml(const QString& inhtml);
    void AppendCatHeader(const QString& inhtml);
    void NullCatHeader();
    bool UpdateCatHeader(const QString& inhtml);
    bool SignalRowChanged(size_t wrow);
    // the method for which this class was created
    void AppendSelectedItems(jfResultUnitVector* in_results, size_t addcount);
    // a simplified version of the above
    void AppendItems(jfResultUnitVector* in_results);
    // informational methods
    size_t getItemcount() const;
    jfResultUnit* getDataAtModelIndex(const QModelIndex& inval);
    QColor getColorAtModelIndex(const QModelIndex& inval) const;

  protected:
    QList<jfResultUnit*> data_store;
    QList<QString> cat_store;
    size_t res_index;
    size_t itemcount;
    int catptr;
};
//--------------------------------------------
typedef struct jfRClickData {
    size_t list_index;    // index of the clicked on list
    int row_index;
    jfExtendedModel* model_pointer;  // pointer to the data model of the clicked on list
    jfResultUnit* item_clicked;   // summary data on the clicked on item
} jfRClickData;
Q_DECLARE_METATYPE(jfRClickData);
//---------------------------------------------
class jfHtmlListView : public QListView {
    Q_OBJECT
  public:
    // constructor
    jfHtmlListView(QWidget * parent, size_t resindex);
    ~jfHtmlListView();
    // external to add items/text
    int AppendItem(jfResultUnit* in_result);  // add one item
    // might add items and maybe a header, each result has to be checked for category
    int AppendItems(jfResultUnitVector* in_results, bool doheader, QString inheader = QString() );
    // similar to the above, but no checking is done, everything in in_results is added
    int AppendItemsSimple(jfResultUnitVector* in_results, bool doheader, QString inheader = QString() );
    int AppendHtml(QString inhtml);
    // category header related
    int AppendCatHeader(const QString& inhtml);
    bool UpdateCatHeader(const QString& inhtml);
    void SetNoCatHeader();
    // needed for handleRightClick, exposed because it could be useful in other ways
    jfResultUnit* GetByPoint(const QPoint& p);
  signals:
    void SendRightClick(jfRClickData clickData, const QPoint& pout);
  public slots:
    void handleRightClick(const QPoint& pos);
  protected:
    virtual void resizeEvent( QResizeEvent* e );
    jfHtmlResultCollection* gparent;
    HtmlDelegate* htmlshow;
    size_t res_index;
    jfExtendedModel* datastore;
    jfQSizeCache* size_cache;
};

//===========================================================================
class jfHtmlResultCollection : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfHtmlResultCollection(header_enum inhtype, QWidget* parent);
    // extenal custom methods
    void SetPopupMenu(jfPopupMenu* inmenu);
    bool SetupViewers(jfCategories* input);
    bool ClearViewers();
    bool CheckRCounts(bool uses_default,size_t ch_rcats) const;
    int SelectedIndex() const;
  public slots:
    virtual void HandleNewResults(jfResultUnitVector* inresults) = 0;
    virtual void HandleIncreaseCount(size_t nitem_count) = 0;
    virtual void HandleNewCategory(struct jfCategoryInfoMsg incat) = 0;
    void HandleRightClick(jfRClickData clickData, const QPoint& pout);
  protected:
    void UpdateResultsLabel();
    bool ConnectRClick(size_t inpan);
    // GUI elements
    QTabWidget* mainbook;
    // jfHtmlResultViewer* panels[33];
    jfHtmlListView* panels[33];
    QLabel* result_label;
    // layouts
    QVBoxLayout* mlayout;
    QHBoxLayout* brlayout;
    // internal basic info
    bool use_default;
    size_t num_rcats, tpcount;
    header_enum hype;
    size_t numitems;
    // storing index info
    jfDisplayIndexGroup* mgroup;
    jfPopupMenu* popup_menu;
};


//****************************************************************************
