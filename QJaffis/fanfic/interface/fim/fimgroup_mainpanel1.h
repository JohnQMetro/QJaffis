/******************************************************************************
Name    :   fimgroup_mainpanel1.h
Author  :   John Q Metro
Purpose :   Declares the main interface for fimfiction.net group searching
Created :   August 10, 2013
// conversion to Qt satrted August 3, 2015
Updated :   August 11, 2013
******************************************************************************/
#ifndef FIMGROUP_MAINPANEL1_H_INCLUDED
#define FIMGROUP_MAINPANEL1_H_INCLUDED
#endif // FIMGROUP_MAINPANEL1_H_INCLUDED
//--------------------------------------
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "../../data/fim/fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
#ifndef SEARCH_PANELS_H
  #include "../../../interface/display/search_panels.h"
#endif // SEARCH_PANELS_H
#ifndef MAINSEARCH_H
  #include "../../../interface/display/mainsearch.h"
#endif // MAINSEARCH_H
/*****************************************************************************/
class jfFIMGroupSearchGrp : public jfSearchGrouping {
  public:
    // the constructor
    jfFIMGroupSearchGrp(jfFIMGroupSearch* searchin, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelBase*  MakeFirstPanel();
    virtual jfSearchCore*       MakeTypedSearch() const;
    virtual jfResultCollection* MakeTypedCollection() const;
    virtual jfBaseItemDownloader*    MakeTypedThread() const;
  protected:
    jfFIMGroupSearch* typed_search;
};
//===========================================================================
class jfFIMGroupOnePanelSearch : public jfOnePanelSearch {
  public:
    // the constructor
    jfFIMGroupOnePanelSearch(jfFIMGroupSearch* searchin, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelBase*  MakeFirstPanel();
    virtual jfSearchCore*       MakeTypedSearch() const;
    virtual jfResultCollection* MakeTypedCollection() const;
    virtual jfBaseItemDownloader* MakeTypedThread() const;
  protected:
    jfFIMGroupSearch* typed_search;
};
//****************************************************************************
class jfFIMGroupMain : public jfMainSearchGroup {
  public:
    // the constructor
    jfFIMGroupMain(jfSearchCore* searchin, bool notebook, QWidget* parent=NULL);
  protected:
    // implemented virtual method
    virtual jfSearchGroupingCore* MakeSearchGroup(jfSearchCore* xnewsearch, bool threepanels);
    virtual jfResultDisplay* MakeResultGroup( bool multi_cat, rtype_enum rinval, QWidget* inparent = NULL);
  // internal data
    jfSearchGroupingCore* typed_mpanel;
};
/*****************************************************************************/
