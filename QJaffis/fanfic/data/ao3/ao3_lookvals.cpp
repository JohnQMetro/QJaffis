/******************************************************************************
Name    :   ao3_lookvals.cpp
Author  :   John Q Metro
Purpose :   Some constants and utility functions for making AO3 searches
Created :   June 12, 2021
Updated :   March 6, 2022
******************************************************************************/
#ifndef AO3_LOOKVALS_H
    #include "ao3_lookvals.h"
#endif // AO3_LOOKVALS_H
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-----------------------------------
#include <assert.h>
#include <QUrl>
//*****************************************************************************
// Orientation stuff

AO3OrientMake::AO3OrientMake() {
    orientation_labels << "Gen" << "F/F" << "F/M" << "M/M" << "Multi" << "Other";
    orientation_ids << "21" << "116" << "22" << "23" << "224" << "24";
}
size_t AO3OrientMake::Count() const {
    return orientation_labels.length();
}
// 0 = empty, 1 = no relationships, 2+ no slash
jfTagListing* AO3OrientMake::MakeOrientationListing(size_t choice) const {
    jfTagListing* res = new jfTagListing();
    if (choice == 0) {
        // empty
        res->FromStrArray(&orientation_labels);
    }
    else if (choice == 2) {
        // no relationships means exclude everything but gen
        // (this still allows cases where orientation is unmarked)
        res->insert(std::pair<QString,jfTAG_STATUS>(orientation_labels.first(), jfts_NONE));
        for (size_t odex = 1; odex < orientation_labels.count(); odex++) {
            res->insert(std::pair<QString,jfTAG_STATUS>(orientation_labels.at(odex), jfts_EXCLUDE));
        }
    }
    else {
        // exclude F/F and M/M
        res->FromStrArray(&orientation_labels);
        (*res)["F/F"] = jfts_EXCLUDE;
        (*res)["M/M"] = jfts_EXCLUDE;
    }
    return res;
}
jfTagListing* AO3OrientMake::MakeOrientationPickOne(size_t which) const {
    jfTagListing* res = new jfTagListing();
    res->FromStrArray(&orientation_labels);
    // anything other than 1 to 6 is any
    if ((which == 0) || (which > 6)) return res;
    // the old order is different than the new order
    QString old_labels[] = {"Any", "Gen", "F/M", "M/M", "F/F", "Multi", "Other"};
    (*res)[old_labels[which]] = jfts_INCLUDE;
    return res;
}
QString AO3OrientMake::OrientationLookupID(const QString& tomatch) const {
    for (size_t sdex = 0; sdex < orientation_labels.length(); sdex++) {
        if (tomatch == orientation_labels[sdex]) {
            return orientation_ids.at(sdex);
        }
    }
    return "";
}
QString AO3OrientMake::OrientationListingToQueryPart(const jfTagListing* source) const {
    const QString fname = "AO3OrientMake::OrientationListingToQueryPart";
    const QString incl = ao3values::PercentEncode("include_work_search[category_ids][]") + "=";
    const QString excl = ao3values::PercentEncode("exclude_work_search[category_ids][]") + "=";
    assert(source != NULL);
    // looping through the labels
    QString outurl = "";
    outurl.reserve(200);
    for(size_t odex = 0; odex < orientation_labels.length(); odex++) {
        jfTagListing::const_iterator vp = source->find(orientation_labels[odex]);
        if (vp == source->end()) continue;
        jfTAG_STATUS stat = vp->second;
        if (stat == jfts_INCLUDE) {
            if (outurl.length() > 0) outurl += '&';
            outurl += incl;
            outurl += orientation_ids.at(odex);
        }
        else if (stat == jfts_EXCLUDE) {
            if (outurl.length() > 0) outurl += '&';
            outurl += excl;
            outurl += orientation_ids.at(odex);
        }
    }
    return outurl;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
// filter helper methods
void AO3OrientMake::SetTagsToEmpty(jfTagListing& listing) const {
    // clearing the old and preparing for the new...
    listing.clear();
    // loop for get and add all of the tag names
    for (int odex = 0; odex < orientation_labels.length(); odex++) {
        listing[orientation_labels.at(odex)] = jfts_NONE;
    }
}
QStringList* AO3OrientMake::GetOrientations() const {
    return new QStringList(orientation_labels);
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++
AO3OrientMake::~AO3OrientMake() {

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* For rating, possible options are G (General), T (Teen), M (Mature), E (Explicit), _ (Unspecified)
    ( codes are "10","11","12","13","9"). You can pick one, or exclude any combo that does not include
    unspecified. 
    Special Strings: ~ starts an exclude, otherwise we treat it as include
    choices: G T GT ~ME M ~E E _
    */
AO3RatingMake::AO3RatingMake() {
    // char to code
    code_lookup.insert(QChar('G'),QString("10"));
    code_lookup.insert(QChar('T'),QString("11"));
    code_lookup.insert(QChar('M'),QString("12"));
    code_lookup.insert(QChar('E'),QString("13"));
    code_lookup.insert(QChar('_'),QString("9"));

    choice_labels << "General" << "Teen" << "General & Teen" << "No Mature & Porn" <<
                                 "Mature" << "No Porn" << "Porn" << "Unspecified";
    choice_codes << "G" << "T" << "~ME_" << "~ME" << "M" << "~E" << "E" << "_";
}
QString AO3RatingMake::MakeIncludeQuery(QChar includeCode) const {
    if (code_lookup.contains(includeCode)) {
        QString code = code_lookup.value(includeCode);
        return ao3values::PercentEncode("include_work_search[rating_ids][]") + "=" + code;
    }
    else return "";
}
QString AO3RatingMake::MakeExcludeQuery(const QString& excludeCodes) const {
    if (excludeCodes.isEmpty()) return "";
    QString exclude_pfx = ao3values::PercentEncode("exclude_work_search[rating_ids][]") + "=";
    QString result = "";
    for (int chardex = 0; chardex < excludeCodes.count(); chardex++) {
        QChar cx = excludeCodes.at(chardex);
        if (code_lookup.contains(cx)) {
            QString code = code_lookup.value(cx);
            if (!result.isEmpty()) result += "&";
            result += exclude_pfx + code;
        }
    }
    return result;
}
QString AO3RatingMake::MakeQuery(const QString& specialCodes) const {
    if (specialCodes.isEmpty()) return "";
    if (specialCodes.startsWith("~")) {
        QString exclude = specialCodes.right(specialCodes.length()-1);
        return MakeExcludeQuery(exclude);
    }
    else {
        QChar include = specialCodes.at(0);
        return MakeIncludeQuery(include);
    }
}
QStringList AO3RatingMake::GetPredefined() const {
    return choice_labels;
}
QString AO3RatingMake::MakeUsingPredefined(size_t index) const {
    if (index >= choice_codes.size()) return "";
    else return MakeQuery(choice_codes.at(index));
}
AO3RatingMake::~AO3RatingMake() {

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AO3TagExcludeMake::AO3TagExcludeMake() {
    gensex_excludes << "Alpha/Beta/Omega Dynamics" << "Coming Out" << "Trans Character" <<
        "Trans Male Character" << "Trans Female Character" << "LGBTQ Themes" << "Genderbending" <<
        "Mpreg" << "Sex Pollen" << "Anal Sex" << "Gender Identity" << "Sexual Identity" <<
                       "Alternate Universe - Gender Changes" << "Dysphoria" << "Sexuality Crisis" <<
                       "Masturbation" << "Explicit Sexual Content";
    other_excludes << "Reader" << "Alternate Universe - Soulmates" << "Alternate Universe - Merpeople" <<
                      "Alternate Universe - Dragons" << "Pack Dynamics" << "Alternate Universe - Shifters" <<
                      "Shifter AU" << "Mermaids" << "Vampire AU" << "Wingfic" << "Selkies" <<
                      "Hanahaki Disease";
    emo_excludes << "Suicidal Thoughts" << "Angst" << "Hurt/Comfort" << "Implied/Referenced Self-Harm" <<
                    "Touch-Starved" << "Mental Breakdown" << "Panic Attacks" << "Implied/Referenced Suicide" <<
                    "Hurt No Comfort" << "Anxiety" << "Dissociation" << "Feral" << "Eating Disorders" <<
                    "Guilt" << "Childhood Trauma" << "Sad" << "Implied/Referenced Suicide" <<
                    "Emotional Hurt" << "Hurt No Comfort" << "Suicide Attempt" << "Self-Harm" <<
                    "Self-Esteem Issues" << "Anorexia";
    fluff_excludes << "Fluff" << "Family Feels" << "Family Bonding" << "Family Fluff" <<
                      "Sibling Bonding" << "Cute" << "Brotherly Bonding" << "Batfamily Feels" <<
                      "Age Regression" << "Adorable" << "Soft" << "Snuggling & Cuddling" << "Cuddle Pollen";
    whump_excludes << "Whump" << "Whumptober" << "Sickfic" << "Health Issues" << "Seizures" << "Fainting";
}
QString AO3TagExcludeMake::QueryPart(const QStringList& basic_list) const {
    if (basic_list.isEmpty()) return "";
    QString res = ao3values::PercentEncode("work_search[excluded_tag_names]") + "=";
    QString vres = "";
    for(int exdex = 0; exdex < basic_list.size() ; exdex++) {
        if (exdex != 0) vres += ",";
        vres += basic_list.at(exdex);
    }
    return res + ao3values::PercentEncode(vres);
}
// make query exclude from 2 lists. In the second one, % is replaced by insert
QString AO3TagExcludeMake::QueryPartWithTemplate(const QStringList& basic_list, const QStringList& template_list, const QString& insert) const {
    if (basic_list.isEmpty() && template_list.isEmpty()) return "";
    if (template_list.isEmpty()) return QueryPart(basic_list);
    else {
        QString start;
        if (basic_list.isEmpty()) start = ao3values::PercentEncode("work_search[excluded_tag_names]") + "=";
        else start = QueryPart(basic_list) + ",";
        QString rest = "";
        for(int exdex = 0; exdex < template_list.size() ; exdex++) {
            if (exdex > 0) rest += ",";
            rest += QString(template_list.at(exdex)).replace("%",insert);
        }
        return start + ao3values::PercentEncode(rest);
    }
}
QStringList AO3TagExcludeMake::MakePredefinedExcludes(bool gensex, bool emo, bool other, bool fluff) const {
    QStringList res;
    if (gensex) res << gensex_excludes;
    if (emo) res << emo_excludes;
    if (other) res << other_excludes;
    if (fluff) res << fluff_excludes;
    return res;
}
QString AO3TagExcludeMake::MakeFullExcludeQuery(bool gensex, bool emo, bool other, bool fluff, const QString& template_list, const QString& insert) const {
    bool has_flag_excludes = gensex || emo || other;
    bool has_template_list = template_list.length() > 0;
    QStringList base_list;
    if (has_flag_excludes) base_list = MakePredefinedExcludes(gensex, emo, other, fluff);
    if (has_template_list) {
        QStringList user_list = template_list.split(",",QString::SkipEmptyParts);
        // trim any whitespace in the parts
        QStringList excl_list;
        for(size_t pos = 0; pos < user_list.length(); pos++) {
            QString trimp = user_list.at(pos).trimmed();
            if (trimp.isEmpty()) continue;
            else excl_list.append(trimp);
        }
        // after...
        if (excl_list.isEmpty()) return QueryPart(base_list);
        else return QueryPartWithTemplate(base_list, excl_list, insert);
    }
    else if (has_flag_excludes) {
        return QueryPart(base_list);
    }
    else return "";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AO3MoreMake::AO3MoreMake() {
    ordering_labels << "Date Posted" << "Date Updated" << "Author" << "Title" <<
                       "Word Count" << "Hits" << "Kudos" <<"Comments" << "Bookmarks";
    ordering_ids << "created_at" << "revised_at" << "authors_to_sort_on" << "title_to_sort_on" <<
                    "word_count" << "hits" << "kudos_count" << "comments_count" << "bookmarks_count";
}

QString AO3MoreMake::QueryWarningExcludes(bool violence, bool death, bool rape, bool underage) const {
    const QString base = ao3values::PercentEncode("exclude_work_search[archive_warning_ids][]") + "=";
    QString out = (violence)?(base + "17"):"";
    if (death) {
        if (!out.isEmpty()) out += "&";
        out += base + "18";
    }
    if (rape) {
        if (!out.isEmpty()) out += "&";
        out += base + "19";
    }
    if (underage) {
        if (!out.isEmpty()) out += "&";
        out += base + "20";
    }
    return out;
}
QStringList AO3MoreMake::GetOrderingList() const {
    return ordering_labels;
}
QString AO3MoreMake::GetOrderingQuery(size_t order_choice) const {
    if (order_choice >= ordering_ids.length()) return "";
    else return ao3values::PercentEncode("work_search[sort_column]") + "=" + ordering_ids.at((int)order_choice);
}
QString AO3MoreMake::QueryTagInclude(const QString raw_include) const {
    if (raw_include.isEmpty()) return "";
    else {
        QString result = ao3values::PercentEncode("work_search[other_tag_names]") + "=";
        result += ao3values::PercentEncode(raw_include);
        return result;
    }
}
QString AO3MoreMake::QueryExtraLimits(bool complete_only, bool crossover_only, QString lcode) const {
    QString out = (complete_only) ? (ao3values::PercentEncode("work_search[complete]") + "=T") : "";
    if (crossover_only) {
        if (!out.isEmpty()) out += "&";
        out += ao3values::PercentEncode("work_search[crossover]") + "=T";
    }
    if (!lcode.isEmpty()) {
        if (!out.isEmpty()) out += "&";
        out += ao3values::PercentEncode("work_search[language_id]") + "=" + lcode;
    }
    return out;
}
QString AO3MoreMake::QueryWordLimit(int words_from, int words_to) const {
    if ((words_from <= 0) && (words_to <= 0)) return "";
    QString part = "";
    if (words_from > 0) {
        part = ao3values::PercentEncode("work_search[words_from]") + "=" + QString::number(words_from);
    }
    if (words_to >= 0) {
        if (!part.isEmpty()) part += "&";
        part += ao3values::PercentEncode("work_search[words_to]") + "=" + QString::number(words_to);
    }
    return part;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AO3OrientMake ao3values::orientMaker = AO3OrientMake();
AO3RatingMake ao3values::ratingMaker = AO3RatingMake();
AO3TagExcludeMake ao3values::excludeMaker = AO3TagExcludeMake();
AO3MoreMake ao3values::moreMaker = AO3MoreMake();

QString ao3values::PercentEncode(const QString& inval) {
    if (inval.isEmpty()) return inval;
    QByteArray temres = QUrl::toPercentEncoding(inval," ");
    temres.replace(' ','+');
    return QString(temres);
}
