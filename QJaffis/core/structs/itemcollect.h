/*****************************************************************************
Name:        itemcollect.h
Author :     John Q Metro
Purpose :    a vector collection base for jfBasePD that is used for storing
              and processitng items.
Created:     April 16, 2009
Conversion to Qt Started November 9, 2013
Updated:     June 27, 2016
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
//-------------------------------------------------------------------------------
#include <QString>
//=============================================================================
/*
Base class for storing small item results.
*/
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
/******************************************************************************/

