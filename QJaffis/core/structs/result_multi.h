/*****************************************************************************
Name:        result_multi.h
Author :     John Q Metro
Purpose :    A derivation of jfResultCollection for multi-category results
Created:     July 16, 2016
Updated:     July 16, 2016
******************************************************************************/
#ifndef RESULT_MULTI_H
  #define RESULT_MULTI_H
#endif // RESULT_MULTI_H
//-----------------------------------
#ifndef RESULTCOLLECT_H_INCLUDED
  #include "resultcollect.h"
#endif // RESULTCOLLECT_H_INCLUDED
/*****************************************************************************/
class
    jfResultMultiCollection : public jfResultCollection {
  public:
    // core constructor
    jfResultMultiCollection(jfSearchCore* insearch, bool idfilter);
    // virtual public methods
    virtual bool WriteToHTML(size_t result_category) = 0;
    virtual QString GetTypeID() const = 0;
    // public methods for multiple collections
    size_t CollectionCount() const;
    int CollectionIndex() const;
    bool CollectionIndexIsValid() const;
    bool toFirstCollection();
    bool toNextCollection();
    bool NoResults() const;
    jfUrlItemCollection* AppendNewCollection();
    jfUrlItemCollection* AppendNewCollection(const QString& cname, const size_t& cid);

  protected:
    // internal file i/o methods
    virtual void WriteHtmlHeader(jfHtmlParams* indata) = 0;
    // regular file i/o methods
    // file i/o output
    virtual bool AddRestToFile(QTextStream* outfile) const =0;
    // file i/o input
    virtual jfUrlItemCollection* MakeEmptyCollection() const = 0;
    virtual bool ReadRestFromFile(jfFileReader* infile) =0;

};
/*****************************************************************************/
