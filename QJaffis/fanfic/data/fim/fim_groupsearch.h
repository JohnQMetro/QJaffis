/******************************************************************************
Name    :   fim_groupsearch.h
Author  :   John Q Metro
Purpose :   Group Search Object for fimfiction.net
Created :   August 7, 2013
Started conversion to Qt August 3, 2015
Updated :   June 21, 2016
******************************************************************************/
#ifndef FIM_GROUPSEARCH_H_INCLUDED
#define FIM_GROUPSEARCH_H_INCLUDED
#endif // FIM_GROUPSEARCH_H_INCLUDED
//------------------------------------------------
#ifndef SEARCHCORE_H_INCLUDED
  #include "../../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef RESULTCOLLECT_H_INCLUDED
  #include "../../../core/structs/resultcollect.h"
#endif // RESULTCOLLECT_H_INCLUDED
//*****************************************************************************
class jfFIMGroupSearch : public jfSearchCore {
  public:
    // constructors
    jfFIMGroupSearch();
    // setting data
    bool SetLimits(bool first,size_t fmin_in,size_t fmax_in);
    // implemented method for sending category data
    virtual void DispatchCategory();
    // implemeted virtual methods
    virtual QString GetTypeID() const;
    virtual QString GetCatName() const;
    virtual QString GetCatHeader() const;
    virtual QString GetIndexUrl(size_t inp_index) const;
    virtual QString GetHTMLHeader(size_t result_category) const;
    virtual void MakeLocalSkeleton(jfSkeletonBase* src);
    void LoadValues(jfSkeletonParser* inparser, size_t result_categ);
    // returning data data
    size_t GetMinGroup(bool first) const;
    size_t GetMaxGroup(bool first) const;
    bool NoFirst() const;
    size_t GroupsToSearch(bool first) const;
  private:
    // i/o methods for fim group specific data
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    // search data
    size_t min_group1, max_group1;
    size_t min_group2, max_group2;
    bool nofirst;

};
//===========================================================================
class jfFIMGroupResColl : public jfResultCollection {
  public:
    // the constructor
    jfFIMGroupResColl(jfFIMGroupSearch* xsearch_in);
    // implementede virtual methods
    virtual QString GetTypeID() const;
    // i/o
    bool WriteToHTML(size_t result_category);
  protected:
    void WriteHtmlHeader(jfHtmlParams* indata);
    virtual jfUrlItemCollection* MakeEmptyCollection() const;
    // implemented private i/o methods
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    jfFIMGroupSearch* typed_search;

};
//*****************************************************************************

