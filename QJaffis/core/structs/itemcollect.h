/*****************************************************************************
Name:        itemcollect.h
Author :     John Q Metro
Purpose :    a vector collection base for jfBasePD that is used for storing
              and processitng items.
Created:     April 16, 2009
Conversion to Qt Started November 9, 2013
Updated:     March 19, 2023
******************************************************************************/
#ifndef ITEMCOLLECT_H_INCLUDED
#define ITEMCOLLECT_H_INCLUDED
#endif // ITEMCOLLECT_H_INCLUDED
//-----------------------------------------------------------------------------
#ifndef BASECOLLECT_H_INCLUDED
  #include "basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "vectors.h"
#endif // VECTORS_H_INCLUDED
#ifndef SKELSTORE_H_INCLUDED
  #include "../objs/skelstore.h"
#endif // SKELSTORE_H_INCLUDED

#include "../objs/baseitem.h"
#include "../objs/itemutils.h"
//-------------------------------------------------------------------------------
#include <QString>
//=============================================================================

/** This is a base class for storing lists of items derived from jfSearchResultItem,
 *  stored alongside their flags. Please note that templated descendent will handle
 * the various missing parts. */
class jfSearchResultItemCollectionBase {
  public:
    typedef typename std::vector<jfItemFlagGroup>::const_iterator i_const_iterator;

    jfSearchResultItemCollectionBase(QString&& in_name, size_t in_num_id);
    jfSearchResultItemCollectionBase(const QString& in_name, size_t in_num_id);
    // basic info
    size_t InsertIndex() const;
    size_t GetCount() const;
    size_t CountResultCategory(size_t which) const;
    QString PercentResultCategory(size_t which) const;
    virtual QString TypeId() const = 0;
    // extracting info at the specified index
    const QString& TitleAtIndex(size_t i_index) const;
    const QString& SummaryAtIndex(size_t i_index) const;
    const QString& UrlAtIndex(size_t i_index) const;
    // adding values
    bool AddResults(jfResultUnitVector* new_items);
    jfResultUnitVector* GetContentsAsResults(jfResultItemOutputter* outputter) const;
    // Id Filtering
    bool SetIDFiltering(jfIDStore* instore);
    bool HasIDFiltering() const;
    size_t IDFilterMarkResults(jfResultUnitVector* new_items);
    // grand total
    size_t AddToGrandTotal(size_t add_amount);
    size_t GetGrandTotal() const;
    // file I/O
    virtual bool AddToFile(QTextStream* outfile) const;
    bool GetFromFile(jfFileReader* infile);
    // ++++++++++++++++++++++++
    virtual bool LoadValues(jfSkeletonParser* inparser,size_t which) const = 0;
    bool WriteToHTML(jfHtmlParams* indata, bool single) const;
    // ++++++++++++++++++++++++
    void ClearContents(bool destroy_too);

    virtual ~jfSearchResultItemCollectionBase();
  protected:
    bool ToHTMLList(jfHtmlParams* indata, bool single) const;
    void LoadCoreValues(jfSkeletonParser* inparser,size_t which) const;
    // ++++++++++++++++++++++++++++++++++++
    bool WriteGroupToFile(const jfItemFlagGroup& group, QTextStream* outfile) const;
    bool ReadGroupFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const;
    virtual bool ReadItemFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const = 0;
    virtual bool AddDelta(QTextStream* outfile) const = 0;
    virtual bool ReadDelta(jfFileReader* infile) = 0;

    QString name;
    size_t num_id;

    // page (insertion) tracking
    size_t insert_index;
    size_t grand_total;
    // the collection
    std::vector<jfItemFlagGroup> mainlist;
    // id tracking for duplicate filtering
    jfIDStore* idstore;
};

// =======================================================================

/*
Base class for storing small item results.
*/
/*
class jfUrlItemCollection : public jfBaseCollection {
  public:
    QString parse_error;
    // constructor to initialize values
    jfUrlItemCollection();
    jfUrlItemCollection(size_t src_id, const QString& nname);
    // adding info
    virtual bool AddResults(jfResultUnitVector* new_items) = 0;
    size_t AddToGrandTotal(size_t add_amount);
    bool SetIDFiltering(jfIDStore* instore);
    bool hasIDFiltering() const;
    size_t IDFilterMarkResults(jfResultUnitVector* new_items);
    // getting some basic data
    size_t InsertIndex() const;
    size_t GetGrandTotal() const;
    virtual size_t CountRCategory(size_t which) const = 0;
    virtual QString PercRCategory(size_t which) const = 0;
    // extracting info at the specified index
    virtual QString NameAtIndex(size_t i_index) const = 0;
    virtual QString DescriptionAtIndex(size_t i_index) const = 0;
    virtual QString UrlAtIndex(size_t i_index) const = 0;
    // i/o
    virtual bool LoadValues(jfSkeletonParser* inparser,size_t which) const = 0;
    bool WriteToHTML(jfHtmlParams* indata, bool single) const;
  protected:
    // implemented virtual i/o methods
      virtual bool AddMid2ToFile(QTextStream* outfile) const;
      virtual bool ReadMid2FromFile(jfFileReader* infile);
      // loading values
      void LoadCoreValues(jfSkeletonParser* inparser,size_t which) const;
    // some more virtual i/o methods
      virtual bool AddDelta(QTextStream* outfile) const = 0;
      virtual bool ReadDelta(jfFileReader* infile) = 0;
      // more i/o
      virtual bool ToHTMLList(jfHtmlParams* indata, bool single) const = 0;
    // private data
      // page (insertion) tracking
      size_t insert_index;
      size_t grand_total;

      // id tracking for duplicate filtering
      jfIDStore* idstore;

};
*/

/******************************************************************************/

