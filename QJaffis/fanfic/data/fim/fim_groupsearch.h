/******************************************************************************
Name    :   fim_groupsearch.h
Author  :   John Q Metro
Purpose :   Group Search Object for fimfiction.net
Created :   August 7, 2013
Started conversion to Qt August 3, 2015
Updated :   January 7, 2018 : A complete change, since FimFiction.net now has
        a search front end to the groups (a very simple one).
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
    bool SetSearchString(const QString& valin, size_t inorder);
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
    QString GetSearchString() const;
    size_t GetOrder() const;
  private:
    // i/o methods for fim group specific data
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    // search data
    QString sstring;
    QString esc_string;
    size_t order;

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

