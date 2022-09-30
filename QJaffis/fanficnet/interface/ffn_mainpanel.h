//**************************************************************************
// Name:        ffn_mainpanel.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net search interface
// Created:     July 1, 2010
// conversion to Qt started July 29, 2014
// Updated:     June 4, 2022
//**************************************************************************
#define FFN_MAINPANEL

#ifndef JF_FFNSEARCH
  #include "../data/ffn_search.h"
#endif
#ifndef SEARCH_PANELS_H
  #include "../../interface/display/search_panels.h"
#endif // SEARCH_PANELS_H
#ifndef MAINSEARCH_H
  #include "../../interface/display/mainsearch.h"
#endif // MAINSEARCH_H
 //**************************************************************************
// forward declaration
class jfFFNMain;

 class jfFFNSearchGrp : public jfSearchGrouping {
  public:
    // the constructor
    jfFFNSearchGrp(jfFFNSearch* searchin, jfFFNMain* tholder, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelBase*   MakeFirstPanel();
    virtual jfSearchCore*        MakeTypedSearch() const;
    virtual jfResultCollection*  MakeTypedCollection() const;
    virtual jfDownloadRootItems* MakeTypedThread() const;
    // special methods
    int GetSearchIndex() const;
  protected:
    // internal helper
    jfFFNSearch* typed_search;
    int sval;
};
//==============================================================================
 class jfFFNSearchOnePanel : public jfOnePanelSearch {
  public:
    // the constructor
    jfFFNSearchOnePanel(jfFFNSearch* searchin, jfFFNMain* tholder, QWidget* parent = NULL);
    // custom  methods
    virtual jfSearchPanelBase*   MakeFirstPanel();
    virtual jfSearchCore*        MakeTypedSearch() const;
    virtual jfResultCollection*  MakeTypedCollection() const;
    virtual jfDownloadRootItems* MakeTypedThread() const;
    // special methods
    int GetSearchIndex() const;
  protected:
    // internal helper
    jfFFNSearch* typed_search;
    int sval;
};
//===============================================================================
 class jfFFNMain : public jfMainSearchGroup {
    Q_OBJECT
  public:
    // the constructor
    jfFFNMain(jfSearchCore* searchin, bool notebook=true, QWidget* parent = NULL);
    // other methods
    size_t GetSCatIndex() const;
    bool WriteFFNBundle(QString inpath) const;
  protected:
    // implemented virtual methods
    virtual jfSearchGroupingCore* MakeSearchGroup(jfSearchCore* newsearch, bool threepanels);
    virtual jfResultDisplay* MakeResultGroup( bool multi_cat, rtype_enum rinval, QWidget* inparent = NULL);
    // typed search
    jfSearchGroupingCore* typed_mpanel;
};
 //**************************************************************************

