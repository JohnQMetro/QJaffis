/******************************************************************************
Name    :   mainsearch.h
Author  :   John Q Metro
Purpose :   Declares the top level base class for the Seacrh+Results interface
Created :   August 23, 2009 (orginal version)
Conversion to Qt Started November 24, 2013 (and renamed from display_base2.h)
Updated :   January 12, 2016
******************************************************************************/
#ifndef MAINSEARCH_H
#define MAINSEARCH_H
#endif // MAINSEARCH_H
//-----------------------
#ifndef RESULT_PANEL_H_INCLUDED
  #include "result_panel.h"
#endif // RESULT_LISTBOX_H_INCLUDED
#ifndef DISPLAY_PARTS_H
  #include "display_parts.h"
#endif // DISPLAY_PARTS_H
#ifndef SEARCH_PANELS_H
  #include "search_panels.h"
#endif // SEARCH_PANELS_H
#ifndef WIDGETS2_H_INCLUDED
  #include "../controls/widgets2.h"
#endif // WIDGETS2_H_INCLUDED
/*****************************************************************************/
class DisplayPanelCore : public QWidget {
  public:
    // we need a constructor
    DisplayPanelCore(QWidget* parent = NULL);
    // shared virtual methods
    virtual bool WriteAll2Html() =0;
    virtual bool WriteCurrent2Html() =0;
    /* returns true if a search is being stopped, false if there is no
     *ongoing search */
    virtual bool CloseSearch() = 0;
  protected:
};
//===========================================================================
class jfMainSearchGroup : public DisplayPanelCore {
    Q_OBJECT
  public:
    // the constructor
    jfMainSearchGroup(jfSearchCore* searchin, rtype_enum rinval, bool notebook, bool multiple, QWidget* parent=NULL );
    // status data
    bool InProgress() const;
    QString GetSearchName() const;
    QString GetDisplayName() const;
    // doing output
    virtual bool WriteAll2Html();
    virtual bool WriteCurrent2Html();
    bool SaveToFile(QString fname);
    bool SaveToFile();
    bool HasBeenSaved() const;
    // other methods
    jfSearchCore* GetSearchPointer();
    virtual bool CloseSearch();
  signals:
    void SendDoneSignal(bool complete);
    void SendStartingSignal();
  public slots:
    void HandleDisplayStart();
    void HandleDisplayDone();
    void HandleAppendText(const QString text2append);
    void HandleNameChange(QString newname);
  protected:
    // internal methods
    virtual jfSearchGroupingCore* MakeSearchGroup(jfSearchCore* newsearch, bool threepanels) = 0;
    virtual jfResultDisplay* MakeResultGroup( bool multi_cat, rtype_enum rinval, QWidget* inparent = NULL) = 0;
    void TopArranger();
    void MakeThreadCollection();
    bool BuildSearch();
    void ClearOldData();
    QString GSearchName() const;
    bool ConsStepTwo(const QStringList* infclist);
    bool ConnectResult();
    // gui elements
    bool noteb;
    bool threepanel;
    jfSearchGroupingCore* p_search;
    jfResultDisplay*  r_panel;
    jfPanelTwo* panel_two;
    QTabWidget*  container;
    jfNameEdit* nedit;
    // the one and only sizer
    QHBoxLayout* top_sizer;
    // internal data
    bool launched;
    jfResultCollection* results;
    jfDownloadRootItems* thread_holder;
    jfSearchCore* search_data;
    // save data
    bool beensaved;
    QString savfpath;
    QDateTime starttime;
    // menu data
    jfPopupMenu* rc_menu;

};
/*****************************************************************************/
