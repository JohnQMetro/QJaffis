/*****************************************************************************
Name:        resultcollect.h
Author :     John Q Metro
Purpose :    A structure for holding one or more item collections
Created:     May 27, 2009
Conversion to Qt Started November 9, 2013
Updated:     June 20, 2016
******************************************************************************/
#ifndef RESULTCOLLECT_H_INCLUDED
#define RESULTCOLLECT_H_INCLUDED
#endif // RESULTCOLLECT_H_INCLUDED
//--------------------------------
#ifndef ITEMCOLLECT_H_INCLUDED
  #include "itemcollect.h"
#endif // ITEMCOLLECT_H_INCLUDED
#ifndef SEARCHCORE_H_INCLUDED
  #include "../objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
//----------------------------------
#include <QMutex>
#include <vector>
//*******************************************************************************
// some vector classes
class jfItemVector : public std::vector<jfSearchResultItemCollectionBase*> {};
class jfMutexVector : public std::vector<QMutex*> {};
//-------------------------------------------------------------------------------

class jfResultCollection : public jfBaseObj {
  public:
    // core constructor
    jfResultCollection(jfSearchCore* insearch, bool idfilter);
    // public methods
    jfSearchCore* GetSearch() const;
    bool HasCollections() const;
    virtual bool WriteToHTML(size_t result_category) = 0;
    virtual QString GetTypeID() const=0;
    // wrapper to access the collection at the current index
    size_t PageIndex() const;
    size_t ItemCount() const;
    jfSearchResultItemCollectionBase* GetCurrPointer();
    // extracting information from a particular item
    QString GetItemDescription(size_t c_index, size_t i_index) const;
    QString GetItemUrl(size_t c_index, size_t i_index) const;
    // destructor
    ~jfResultCollection();
  protected:
    // internal helper methods
    bool CheckIndex(size_t index) const;
    bool DestroyAll();
    bool AddItem(jfSearchResultItemCollectionBase* newitem);
    bool InsertItem(jfSearchResultItemCollectionBase* newitem);
    bool DeleteAtIndex(size_t dindex);
    // internal file i/o methods
    jfHtmlParams* OpenHTMLOutfile(size_t index);
    void WriteFooterClose(jfHtmlParams* indata);
    virtual void WriteHtmlHeader(jfHtmlParams* indata) = 0;
    // regular file i/o methods
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const =0;
    // file i/o input
    virtual jfSearchResultItemCollectionBase* MakeEmptyCollection() const = 0;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile) =0;
    // internal data
    jfItemVector* collections;
    jfMutexVector* cprotect;
    size_t itemcount;
    jfSearchCore* search;
    size_t cindex;
    jfIDStore* idstore;
};

//*******************************************************************************
