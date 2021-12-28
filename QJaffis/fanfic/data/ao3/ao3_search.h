/******************************************************************************
Name    :   ao3_search.h
Author  :   John Q Metro
Purpose :   Declares search object of AO3
Created :   August 28, 2012
Conversion to Qt Started March 29, 2014
Updated :   December 28, 2021
******************************************************************************/
#ifndef AO3_SEARCH_H_INCLUDED
#define AO3_SEARCH_H_INCLUDED
#endif // AO3_SEARCH_H_INCLUDED
//-----------------------------------
#ifndef SEARCHCORE_H_INCLUDED
  #include "../../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef CAT_COLL_H_INCLUDED
  #include "../cats/cat_coll.h"
#endif // CAT_COLL_H_INCLUDED
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
#ifndef AO3_URLMAKE_H
    #include "ao3_urlmake.h"
#endif // AO3_URLMAKE_H

#ifndef JFMISCTYPES1
    #include "../../../core/objs/misc_types1.h"
#endif
/*****************************************************************************/
// archiveofourown specific search object
class jfAO3Search : public jfSearchCore {
  public:
    size_t search_index;        // 0 to 31
    // constructors
    jfAO3Search();
    jfAO3Search(jfGen_CatHolder* cat_datain, size_t search_indexin);
    // setting data
    bool SetData(jfGen_CatHolder* cat_datain, size_t search_indexin);
    void ApplySelection();
    size_t GetSSIndex() const;
    // setting search options
    void SetOrder(size_t in_oindex);
    void SetRatingIndex(size_t in_rindex);
    void SetWarningExcludes(bool in_violence, bool in_death, bool in_rape, bool in_underage);
    void SetTagExcludes(const QString& in_excludes, const QString& in_replacer);
    void SetPredefinedExcludes(bool in_gensex, bool in_emo, bool in_other);
    void SetIncludes(const QString& in_includes);
    void SetWordLimits(int in_words_from, int in_words_to);
    void SetExtras(bool in_completed_only, bool in_english_only);
    void SetCrossoverOnly(bool only_crossovers);
    // getting data
    jfGenCat_PickData* GetSelector();
    jfGen_CatHolder* GetHolder();

    // getting search options
    jfTagListing* GetOrientation();
    size_t GetOrderIndex() const;
    size_t GetRatingIndex() const;
    bool GetExcludedViolence() const;
    bool GetExcludedDeath() const;
    bool GetExcludedRape() const;
    bool GetExcludedUnderageSex() const;
    QString GetExcludedTagsList() const;
    QString GetExcludeReplacer() const;
    bool IsGendSexExcluded() const;
    bool IsEmoStuffExcluded() const;
    bool IsOtherStuffExcluded() const;
    QString GetIncludedTagsList() const;
    int GetMinWordCount() const;
    int GetMaxWordCount() const;
    bool IsCompletedOnly() const;
    bool IsEnglishOnly() const;
    bool IsCrossoverOnly() const;

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
    // source category indexing
    void ResetIndex();
    bool NextIndex();
    bool LastCat() const;
    size_t GetNumCats() const;
    void SetCurrCat(const jfAO3_Category* inval);
    const jfAO3_Category* GetCurrCat() const;
    // the destructor
    ~jfAO3Search();
  private:
    void MakeURLMaker();
    // i/o methods for ao3 specific data
    void AddTagListingToFile(jfTagListing* listing, QTextStream* outfile) const;
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    bool ReadTagListing(jfTagListing *&target, const QString& tlname,jfFileReader* infile);
    // category data
    jfGen_CatHolder* cat_data;  // holds downloaded category info
    size_t numscats;
    long icatindex;
    // source category index
    const jfAO3_Category* curr_cat;
    // selection data from the interface
    jfGenCat_PickData* selector;

    // search parameters and objects
    AO3UrlMaker* url_maker;

    // search parameters
    size_t rindex; // rating combo index, 0 to 7
    // warning excludes
    bool wx_violence; bool wx_death; bool wx_rape; bool wx_underage;
    jfTagListing* orientation_parameters;

    QString excludes; // raw list of exclude tags
    QString template_replacer; // used with excludes
    bool gensex; bool emo; bool other; // pre-defined exclude lists
    QString includes; // raw list of include tags
    bool conly;  // completed only
    bool eng_only; // english only
    size_t ord_index;
    int words_from; int words_to;
    bool cross_only; // crossover only
};
/*****************************************************************************/
