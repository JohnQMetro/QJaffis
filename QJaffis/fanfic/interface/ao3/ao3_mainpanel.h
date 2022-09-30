//**************************************************************************
// Name:        ao3_mainpanel.h
// Author :     John Q Metro
// Purpose :    Archiveofourown.org search interface
// Created:     September 3, 2012
// Conversion to Qt started April 12, 2014
// Updated:     June 4, 2022
//**************************************************************************
#ifndef AO3_MAINPANEL_H_INCLUDED
#define AO3_MAINPANEL_H_INCLUDED
#endif // AO3_MAINPANEL_H_INCLUDED
//-----------------------------------------------
#ifndef AO3_SEARCH_H_INCLUDED
  #include "../../data/ao3/ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED
#ifndef SEARCH_PANELS_H
  #include "../../../interface/display/search_panels.h"
#endif // SEARCH_PANELS_H
#ifndef MAINSEARCH_H
  #include "../../../interface/display/mainsearch.h"
#endif // MAINSEARCH_H

 //**************************************************************************
 class jfAO3SearchGrp : public jfSearchGrouping {
  public:
    // the constructor
    jfAO3SearchGrp(jfAO3Search* searchin, jfMainSearchGroup* holder , QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelRoot*   MakeFirstPanel();
    virtual jfSearchCore*        MakeTypedSearch() const;
    virtual jfResultCollection*  MakeTypedCollection() const;
    virtual jfDownloadRootItems* MakeTypedThread() const;
    // special methods
    int GetSearchIndex() const;
  protected:
    // internal helper
    jfAO3Search* typed_search;
    int sval;
};
//==============================================================================
 class jfAO3OnePanelSearch : public jfOnePanelSearch {
  public:
    // the constructor
    jfAO3OnePanelSearch(jfAO3Search* searchin, jfMainSearchGroup* holder, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelRoot*   MakeFirstPanel();
    virtual jfSearchCore*        MakeTypedSearch() const;
    virtual jfResultCollection*  MakeTypedCollection() const;
    virtual jfDownloadRootItems* MakeTypedThread() const;
    // special methods
    int GetSearchIndex() const;
  protected:
    jfAO3Search* typed_search;
    int sval;
};
//===============================================================================
class jfAO3Main : public jfMainSearchGroup {
    Q_OBJECT
  public:
    // the constructor
    jfAO3Main(jfSearchCore* searchin, bool notebook=true, QWidget* parent = NULL);
    // other methods
    size_t GetSCatIndex() const;
  protected:
    // implemented virtual methods
    virtual jfSearchGroupingCore* MakeSearchGroup(jfSearchCore* newsearch, bool threepanels);
    virtual jfResultDisplay* MakeResultGroup( bool multi_cat, rtype_enum rinval, QWidget* inparent = NULL);
    // typed search
    jfSearchGroupingCore* typed_mpanel;
};
