/******************************************************************************
Name    :   ao3_lookvals.h
Author  :   John Q Metro
Purpose :   Some constants and utility functions for making AO3 searches
Created :   June 12, 2021
Updated :   December 24, 2021
******************************************************************************/
#ifndef AO3_LOOKVALS_H
    #define AO3_LOOKVALS_H
#endif // AO3_LOOKVALS_H
//-----------------------------------
#ifndef JFMISCTYPES1
    #include "../../../core/objs/misc_types1.h"
#endif
//-----------------------------------
#include <QString>
#include <QStringList>
#include <QMap>
//*****************************************************************************
/*
 *complete ~
 *crossover ~
 *from date
 *to date
 * orientation ~
 * ratings ~
 *excluded tag ~
 *language ~
 *included tag ~
 *query
 *sort ~
 *words from ~
 *words to ~
 */


    /*
    Orientations: 'Gen', 'F/F', 'F/M', 'M/M', 'Multi', 'Other'
              IDS:  21  ,  116 ,  22  ,  23,    224,     24
        Since a fic can actually have any combination of these (including none),
        we have to have a flexible picker.
    */
    class AO3OrientMake {
      public:
        AO3OrientMake();
        size_t Count() const;
        // 0 = empty, 1 = no relationships, 2+ no slash
        jfTagListing* MakeOrientationListing(size_t choice) const;
        jfTagListing* MakeOrientationPickOne(size_t which) const;
        QString OrientationLookupID(const QString& tomatch) const;
        QString OrientationListingToQueryPart(const jfTagListing* source) const;
        // filter helper methods
        void SetTagsToEmpty(jfTagListing& listing) const;
        QStringList* GetOrientations() const;

        virtual ~AO3OrientMake();
      protected:
        QStringList orientation_labels;
        QStringList orientation_ids;
    };

    /* For rating, possible options are G (General), T (Teen), M (Mature), E (Explicit), _ (Unspecified)
    ( codes are "10","11","12","13","9"). You can pick one, or exclude any combo that does not include
    unspecified. 
    Special Strings: ~ starts an exclude, otherwise we treat it as include
    choices: G T GT ~ME M ~E E _
    */
    class AO3RatingMake {
      public:
        AO3RatingMake();
        QString MakeIncludeQuery(QChar includeCode) const;
        QString MakeExcludeQuery(const QString& excludeCodes) const;
        QString MakeQuery(const QString& specialCodes) const;
        QStringList GetPredefined() const;
        QString MakeUsingPredefined(size_t index) const;
        virtual ~AO3RatingMake();
      protected:
        QMap<QChar,QString> code_lookup;
        QStringList choice_labels;
        QStringList choice_codes;
    };

    class AO3TagExcludeMake {
      public:
        AO3TagExcludeMake();
        QString QueryPart(const QStringList& basic_list) const;
        // make query exclude from 2 lists. In the second one, % is replaced by insert
        QString QueryPartWithTemplate(const QStringList& basic_list, const QStringList& template_list, const QString& insert) const;
        QStringList MakePredefinedExcludes(bool gensex, bool emo, bool other) const;
        QString MakeFullExcludeQuery(bool gensex, bool emo, bool other, const QString& template_list, const QString& insert) const;
      protected:
        QStringList gensex_excludes;
        QStringList other_excludes;
        QStringList emo_excludes;
        QStringList fluff_excludes;
    };

    class AO3MoreMake {
      public:
        AO3MoreMake();
        QString QueryWarningExcludes(bool violence, bool death, bool rape, bool underage) const;
        QStringList GetOrderingList() const;
        QString GetOrderingQuery(size_t order_choice) const;
        QString QueryTagInclude(const QString raw_include) const;
        QString QueryExtraLimits(bool complete_only, bool crossover_only, QString lcode) const;
        QString QueryWordLimit(int words_from, int words_to) const;
      protected:
        QStringList ordering_labels;
        QStringList ordering_ids;
    };



    
namespace ao3values {
    extern AO3OrientMake orientMaker;
    extern AO3RatingMake ratingMaker;
    extern AO3TagExcludeMake excludeMaker;
    extern AO3MoreMake moreMaker;

    QString PercentEncode(const QString& inval);
}
//*****************************************************************************
