/******************************************************************************
Name    :   ao3_urlmake.h
Author  :   John Q Metro
Purpose :   Defines a url generator for AO3 searches
Created :   October 31, 2021
Updated :   December 19, 2021
******************************************************************************/
#ifndef AO3_URLMAKE_H
    #define AO3_URLMAKE_H
#endif // AO3_URLMAKE_H
//------------------------------------------
#ifndef JFMISCTYPES1
    #include "../../../core/objs/misc_types1.h"
#endif
//-------------------------------------------
#include <QString>
/*****************************************************************************/
class AO3UrlMaker {
  public:
    AO3UrlMaker();
    AO3UrlMaker(const QString& domain);
    // adding parts
    bool setOrientationListing(const jfTagListing* source);
    bool setRatingChoice(size_t index);
    bool setFullExcludeQuery(bool gensex, bool emo, bool other, bool fluff, const QString& template_list, const QString& insert);
    bool setWarningExcludes(bool violence, bool death, bool rape, bool underage);
    bool setOrderingChoice(size_t order_choice);
    bool setTagIncludes(const QString raw_include);
    bool setExtraLimits(bool complete_only, bool crossover_only, QString lcode);
    bool setWordLimit(int words_from, int words_to);
    bool setCategoryName(const QString cat_name);
    // commiting
    bool isCommitted() const;
    bool commitParts();
    bool unCommit();
    // getting the url
    QString getUrlForPage(size_t page_index) const;
  protected:
    QString categoryNameEncode() const;
    // parts
    QString orientation;
    QString rating;
    QString excludes;
    QString warning_excludes;
    QString orderingChoice;
    QString includes;
    QString extra_limits;
    QString word_limit;
    QString category_name;
    // commit state
    bool commited;
    QString merged_url;
    QString category_part;
    // other
    QString domain_name;
};

/*****************************************************************************/
