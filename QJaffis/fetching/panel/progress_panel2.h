/*****************************************************************************
Name    : progress_panel2.h
Basic   : Child classes of progress panel which are still somewhat generic
Author  : John Q Metro
Started : November 17, 2013
Updated : August 2, 2016
******************************************************************************/
#ifndef PROGRESS_PANEL2_H
  #define PROGRESS_PANEL2_H
#endif // PROGRESS_PANEL2_H
//----------------------------------
#ifndef PROGRESS_PANEL_H
  #include "progress_panel.h"
#endif // PROGRESS_PANEL_H
#ifndef DOWNLOAD_H
  #include "../download.h"
#endif // DOWNLOAD_H
#ifndef DOWNLOAD2_H
  #include "../download2.h"
#endif // DOWNLOAD2_H
//-------------------------------------
#include <QHBoxLayout>
#include <QVBoxLayout>
/*****************************************************************************/
// progress panel for a Search that has no categories, just pages
class jfProgressPanelOne : public jfProgressPanelBase {
    Q_OBJECT
  public:
    // constructor
    jfProgressPanelOne(bool sidelayout, QWidget* parent = NULL);
    // special methods
    virtual bool DoReset();
    bool ChangeLayout(bool sidelayout);
  protected:
    virtual void MoreError(bool dohalt);
    virtual void DoLayout();
    // layout
    QHBoxLayout* btn_layout;
    QVBoxLayout* layout_stack;
    QHBoxLayout* side_layout;
    bool xside_layout;
};
/*****************************************************************************/
class jfProgressPanelSingle : public jfProgressPanelOne {
    Q_OBJECT
  public:
    jfProgressPanelSingle(bool sidelayout, QWidget* parent = NULL);
    // specials
    bool ConnectAndSetPauser(jfBaseDownloader* the_downloader);
  protected:
};

/*****************************************************************************/
// progress panel for multiple categories
class jfProgPanelMultiBase : public jfProgressPanelBase {
    Q_OBJECT
  public:
    // constructor
    jfProgPanelMultiBase(size_t loption, QWidget* parent = NULL);
    virtual bool DoReset();
  public slots:
    void HandleCatCount(size_t ccount);
    void HandleNewCategory(struct jfItemInfoMsg catdat);
    void HandleCategoryDone(bool skip);
    void HandleCatFail();
  protected:
    // internal methods
      void CreateCatWidgets();
      QBoxLayout* ArrangeButtons(bool stacked, bool sspacer);
      QVBoxLayout* MakeBarStack();
      void SetCatLabel(QString extra);
    // implemented methods
      virtual void MoreError(bool dohalt);
      virtual void DoLayout();
    // widgets
      QProgressBar* category_bar; // progress bar for categories
      QLabel* category_status; // more detailed status info
    // other data
      size_t catindex;
      jfItemInfoMsg catinfo;
      size_t layout_option;
    // layouts
      QBoxLayout* btn_layout;
      QVBoxLayout* pbar_stack;
      QHBoxLayout* horiz_layout;
      QVBoxLayout* top_stack;
};
//***************************************************************************
// progress panel for multiple categories
class jfProgPanelMultiple : public jfProgPanelMultiBase {
    Q_OBJECT
  public:
    // constructor
    jfProgPanelMultiple(size_t loption, QWidget* parent = NULL);
    bool ConnectAndSetPauser(jfMultiCatBaseDownloader* the_downloader);
};

/*****************************************************************************/
