/******************************************************************************
Name    :   ao3_urlmake.cpp
Author  :   John Q Metro
Purpose :   Defines a url generator for AO3 searches
Created :   November 20, 2021
Updated :   January 27, 2022
******************************************************************************/
#ifndef AO3_URLMAKE_H
    #include "ao3_urlmake.h"
#endif // AO3_URLMAKE_H
//------------------------------------------
#ifndef AO3_LOOKVALS_H
    #include "ao3_lookvals.h"
#endif // AO3_LOOKVALS_H

/*****************************************************************************/
AO3UrlMaker::AO3UrlMaker() {
    commited = false;
    domain_name = "archiveofourown.org";
}

AO3UrlMaker::AO3UrlMaker(const QString& domain) {
    commited = false;
    if (domain.isEmpty()) domain_name = "archiveofourown.org";
    else domain_name = domain;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// adding parts
bool AO3UrlMaker::setOrientationListing(const jfTagListing* source) {
    if (source == NULL) orientation.clear();
    else orientation = ao3values::orientMaker.OrientationListingToQueryPart(source);
    return !orientation.isEmpty();
}
bool AO3UrlMaker::setRatingChoice(size_t index) {
    rating = ao3values::ratingMaker.MakeUsingPredefined(index);
    return !orientation.isEmpty();
}
bool AO3UrlMaker::setFullExcludeQuery(bool gensex, bool emo, bool other, bool fluff, const QString& template_list, const QString& insert) {
    excludes = ao3values::excludeMaker.MakeFullExcludeQuery(gensex, emo, other, fluff, template_list, insert);
    return !orientation.isEmpty();
}
bool AO3UrlMaker::setWarningExcludes(bool violence, bool death, bool rape, bool underage) {
    warning_excludes = ao3values::moreMaker.QueryWarningExcludes(violence, death, rape, underage);
    return !warning_excludes.isEmpty();
}
bool AO3UrlMaker::setOrderingChoice(size_t order_choice) {
    orderingChoice = ao3values::moreMaker.GetOrderingQuery(order_choice);
    return !orderingChoice.isEmpty();
}
bool AO3UrlMaker::setTagIncludes(const QString raw_include) {
    includes = ao3values::moreMaker.QueryTagInclude(raw_include);
    return !includes.isEmpty();
}
bool AO3UrlMaker::setExtraLimits(bool complete_only, bool crossover_only, QString lcode) {
    extra_limits = ao3values::moreMaker.QueryExtraLimits(complete_only, crossover_only, lcode);
    return !extra_limits.isEmpty();
}
bool AO3UrlMaker::setWordLimit(int words_from, int words_to) {
    word_limit = ao3values::moreMaker.QueryWordLimit(words_from, words_to);
    return !word_limit.isEmpty();
}
bool AO3UrlMaker::setCategoryName(const QString cat_name) {
    category_name = cat_name;
    if (category_name.isEmpty()) {
        category_part = "";
        return false;
    }
    else {
        category_part = "tag_id=" + categoryNameEncode();
        return true;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// commiting
bool AO3UrlMaker::isCommitted() const {
    return commited;
}
bool AO3UrlMaker::commitParts() {
    if (commited) return false;
    merged_url = "https://" + domain_name + "/" + "works?utf8=%E2%9C%93&commit=Sort+and+Filter";
    if (!orientation.isEmpty()) {
        merged_url += '&' + orientation;
    }
    if (!rating.isEmpty()) {
        merged_url += '&' + rating;
    }
    if (!warning_excludes.isEmpty()) {
        merged_url += '&' + warning_excludes;
    }
    if (!excludes.isEmpty()) {
        merged_url += '&' + excludes;
    }
    if (!orderingChoice.isEmpty()) {
        merged_url += '&' + orderingChoice;
    }
    if (!includes.isEmpty()) {
        merged_url += '&' + includes;
    }
    if (!extra_limits.isEmpty()) {
        merged_url += '&' + extra_limits;
    }
    if (!word_limit.isEmpty()) {
        merged_url += '&' + word_limit;
    }

    commited = true;
    return true;
}

bool AO3UrlMaker::unCommit() {
    if (!commited) return false;
    commited = false;
    merged_url = "";
    return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++
// getting the url
QString AO3UrlMaker::getUrlForPage(size_t page_index) const {
    if ((!commited) || category_name.isEmpty()) return "";
    else {
        QString merge_url = merged_url + "&" + category_part;
        if (page_index > 1) merge_url += "&page=" + QString::number(page_index);
        return merge_url;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
/* the ao3 process for &tag_id= seems to be :
-Take the name, replace & with *a*, . with *d*, ? with *q*, and / with *s* .
-Convert to to utf-8 and then treat each byte separatly.
-Characters of 128 or more, as well as selected characters less than than, are %
encoded. Selected characters are %<>#{}\^`';/:@=$+()"
-Spaces are replaced with + */
QString AO3UrlMaker::categoryNameEncode() const {
    // constants
    const QByteArray dont_encode = QByteArray("!&*,/?[]| ");
    const QString pesc_chars = "%<>#{}\\^`';/:@=$+()\"";
    // special case
    QString inval = category_name;
    if (category_name.isEmpty()) return inval;
    // variables
    QString resval;
    QByteArray temres;
    QByteArray mbcopy;
    // conversion
    mbcopy = inval.toUtf8();
    temres = mbcopy.toPercentEncoding(dont_encode);
    // special replacements
    resval = QString(temres);
    resval.replace('&',"*a*");
    resval.replace('.',"*d*");
    resval.replace('?',"*q*");
    resval.replace('/',"*s*");
    resval.replace(' ',"+");
    // don
    return resval;
}


/*****************************************************************************/
