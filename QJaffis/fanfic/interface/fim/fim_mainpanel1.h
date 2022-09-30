/******************************************************************************
Name    :   fim_mainpanel1.h
Author  :   John Q Metro
Purpose :   Declares the main interface for fimfiction.net searching
Created :   June 24, 2012
Conversion to Qt started February 3, 2014
Updated :   June 4, 2022
******************************************************************************/
#ifndef FIM_MAINPANEL1_H_INCLUDED
#define FIM_MAINPANEL1_H_INCLUDED
#endif // FIM_MAINPANEL1_H_INCLUDED
//--------------------------------------
#ifndef FIM_SEARCH_H_INCLUDED
  #include "../../data/fim/fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED
#ifndef SEARCH_PANELS_H
  #include "../../../interface/display/search_panels.h"
#endif // SEARCH_PANELS_H
#ifndef MAINSEARCH_H
  #include "../../../interface/display/mainsearch.h"
#endif // MAINSEARCH_H
/*****************************************************************************/
class jfFIMSearchGrp : public jfSearchGrouping {
  public:
    // the constructor
    jfFIMSearchGrp(jfFIMSearch* searchin, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelBase*   MakeFirstPanel();
    virtual jfSearchCore*        MakeTypedSearch() const;
    virtual jfResultCollection*  MakeTypedCollection() const;
    virtual jfDownloadRootItems* MakeTypedThread() const;
  protected:
    jfFIMSearch* typed_search;
};
//===========================================================================
class jfFIMOnePanelSearch : public jfOnePanelSearch {
  public:
    // the constructor
    jfFIMOnePanelSearch(jfFIMSearch* searchin, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelBase*   MakeFirstPanel();
    virtual jfSearchCore*        MakeTypedSearch() const;
    virtual jfResultCollection*  MakeTypedCollection() const;
    virtual jfDownloadRootItems* MakeTypedThread() const;
  protected:
    jfFIMSearch* typed_search;
};
//****************************************************************************
class jfFIMMain : public jfMainSearchGroup {
    Q_OBJECT
  public:
    // the constructor
    jfFIMMain(jfSearchCore* searchin, bool notebook, QWidget* parent=NULL);
    // right click menu event handlers
   protected:
    // implemented virtual method
    virtual jfSearchGroupingCore* MakeSearchGroup(jfSearchCore* newsearch, bool threepanels);
    virtual jfResultDisplay* MakeResultGroup( bool multi_cat, rtype_enum rinval, QWidget* inparent = NULL);
  // internal data
    jfSearchGroupingCore* typed_mpanel;
};
/*****************************************************************************/
