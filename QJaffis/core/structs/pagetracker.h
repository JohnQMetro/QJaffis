/*****************************************************************************
Name:        pagetracker.h
Author :     John Q Metro
Purpose :    Declares a class to keep track of top-level notebook pages
Created:     November 7, 2009
Qt Conversion Started September 14, 2013
Updated:     February 16, 2013
******************************************************************************/
#ifndef PAGETRACKER_H_INCLUDED
#define PAGETRACKER_H_INCLUDED
#endif // PAGETRACKER_H_INCLUDED
//------------------------------------------------------------------------------
#include <vector>
#include <QWidget>
/******************************************************************************/
// identifies page type
enum jf_tlPageType {jftl_GLOBFILEDIT,jftl_SKELEDIT, jftl_GOOGSEARCH, jftl_FFNSEARCH, jftl_FFNUPDATE,
                        jftl_FIMSEARCH, jftl_AO3SEARCH, jftl_FICUPDATE, jftl_FIMGSEARCH};
//-----------------------------------------------------
class jfPTypeVector : public std::vector<jf_tlPageType> {};
//=============================================================================
class jfPageTracker {
  public:
    // the constructor
    jfPageTracker();
    // basic information
    size_t GetCount() const;
    int    GetWindowIndex(QWidget* incheck) const;
    bool   IsHtmlReady(size_t index) const;
    int    FindGlobEdit() const;
    int    FindSkelEdit() const;
    bool   GlobEditLoaded() const;
    bool   SkelEditLoaded() const;
    bool   IsGlobEdit(size_t index) const;
    bool   IsGlobSkel(size_t index) const;
    bool   NotHtml(size_t index) const;
    bool   IsUpdater(size_t index) const;
    bool   IsFFNSearch(size_t index) const;
    bool   IsFicUpdate(size_t index) const;
    jf_tlPageType GetType(size_t index) const;
    // maniupulating pages
    bool AddPage(QWidget* inpage,jf_tlPageType ptype);
    bool RemovePage(size_t index);
    bool SetHtmlReady(QWidget* inpage, bool inval);
    // the destructor
    virtual ~jfPageTracker();
  protected:
    size_t num_items;
    std::vector<QWidget*>* pagelist;
    std::vector<bool>*     htmlready;
    jfPTypeVector*         typelist;
};
/******************************************************************************/
