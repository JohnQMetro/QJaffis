/******************************************************************************
Name    :   fim_coll.h
Author  :   John Q Metro
Purpose :   Collections for fimfiction.net data
Created :   May 22, 2012
Conversion to Qt started November 9, 2013
Updated :   March 19, 2023
******************************************************************************/
#ifndef FIM_COLL_H_INCLUDED
#define FIM_COLL_H_INCLUDED
#endif // FIM_COLL_H_INCLUDED
//---------------------------------------------------------------
#ifndef TYPEDCOLLECT_H_INCLUDED
  #include "../../../core/structs/typedcollect.h"
#endif // TYPEDCOLLECT_H_INCLUDED
#ifndef FIM_FICOBJ_H_INCLUDED
  #include "fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED
#ifndef RESULTCOLLECT_H_INCLUDED
  #include "../../../core/structs/resultcollect.h"
#endif // RESULTCOLLECT_H_INCLUDED
//****************************************************************************
class jfFIMSearch;
//============================================================================
class jfFIMItemCollection : public jfSearchResultsCollection<jfFIM_Fanfic> {
  public:
    // the constructors
    jfFIMItemCollection(QString&& in_name, size_t in_num_id);
    jfFIMItemCollection(const QString& in_name, size_t in_num_id);
    // implemented page methods
    virtual QString TypeId() const override;
    virtual bool LoadValues(jfSkeletonParser* inparser,size_t which) const override;
    // getting more information
    jfFicExtract_FIM* FicExt_AtIndex(size_t i_index) const;
  protected:
    // some more virtual i/o methods
    virtual bool ReadItemFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const override;
    virtual bool AddDelta(QTextStream* outfile) const override;
    virtual bool ReadDelta(jfFileReader* infile) override;
};
//========================================================================
class jfFIMResColl : public jfResultCollection {
  public:
    // the constructor
    jfFIMResColl(jfFIMSearch* xsearch_in);
    // implementede virtual methods
    virtual QString GetTypeID() const;
    // i/o
    bool WriteToHTML(size_t result_category);
    jfFicExtract_FIM* GetExtract(size_t xindex) const;
  protected:
    void WriteHtmlHeader(jfHtmlParams* indata);
    // implemented private i/o methods
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual jfSearchResultItemCollectionBase* MakeEmptyCollection() const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    jfFIMSearch* typed_search;

};
//****************************************************************************
