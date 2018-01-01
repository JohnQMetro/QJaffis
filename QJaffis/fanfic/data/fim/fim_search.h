/******************************************************************************
Name    :   fim_search.h
Author  :   John Q Metro
Purpose :   Search Object for fimfiction.net
Created :   May 22, 2012
Conversion to Qt Started Nov 7, 2013
Updated :   December 30, 2017 (new tag search)
******************************************************************************/
#ifndef FIM_SEARCH_H_INCLUDED
#define FIM_SEARCH_H_INCLUDED
#endif // FIM_SEARCH_H_INCLUDED
//------------------------------------------------
#ifndef SEARCHCORE_H_INCLUDED
  #include "../../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef JFMISCTYPES1
	#include "../../../core/objs/misc_types1.h"
#endif
#ifndef FIM_TAGS_H
    #include "fim_tags.h"
#endif // FIM_TAGS_H
//****************************************************************************
//----------------------------------------------------------------------------
class jfFIMSearch : public jfSearchCore {
    Q_OBJECT
  public:
    // constructors
    jfFIMSearch();
    // setting data
    bool SetWordcount(size_t wcmin_in, size_t wcmax_in);
    void SetSString(QString inval);
    bool SetMinor(size_t inorder, QChar inrating);
    void SetCompleted(bool invalue);
    void SetMature(bool inval);
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
    QString GetSString() const;
    size_t GetMinWC() const;
    size_t GetMaxWC() const;
    IntPair* GetWC() const;
    bool GetCompleted() const;
    QChar GetRating() const;
    size_t GetOrder() const;
    bool GetMature() const;
    jfTagListing* GetGenres();
    jfTagListing* GetCharacters();
    jfTagListing* GetWarnings();
    jfTagListing* GetContentType();
    // more methods
    QString GetCompactUrl(size_t inp_index) const;
  private:
    // othermethods
    QString MakeTagQuery(FIM_TagType this_type, jfTagListing* this_filter) const;
    QString GetUrl(size_t index, bool is_iscompact) const;
    void MakeEmptyTags(FIM_TagType type);
    // i/o methods for fim search specific data
    void AddTagListingToFile(jfTagListing* listing,QTextStream* outfile) const;
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    bool ReadTagListing(jfTagListing *&target, const QString& tlname,jfFileReader* infile);
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    // search data used for building an url
    jfTagListing* genres_filter;
    jfTagListing* characters_filter;
    jfTagListing* warnings_filter;
    jfTagListing* contentt_filter;
    QString ssearch, esc_search;
    size_t min_wc, max_wc;
    bool completed;
    QChar rating;
    size_t order;
    bool viewmature;
};
//****************************************************************************
