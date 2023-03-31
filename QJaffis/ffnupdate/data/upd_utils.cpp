/******************************************************************************
Name    :   upd_utils.cpp
Author  :   John Q Metro
Purpose :   Parsers for Fanfiction.Net fic items for Updating.
Created :   March 26, 2023
Updated :   March 26, 2023
******************************************************************************/
#include "upd_utils.h"

/*****************************************************************************/
/* Parser for jfFFNItemAuthor. */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNAuthorFanficParser::jfFFNAuthorFanficParser():jfFFNFanficParseBase() {
    ffn_fic = NULL;
    storyid_start = QRegularExpression(
                        "<a\\s+class=['\"]?stitle['\"]?\\s+href=['\"]?/s/",
                        QRegularExpression::CaseInsensitiveOption);
    title_img = QRegularExpression("<img\\s+class=['\"]?lazy cimage",
                        QRegularExpression::CaseInsensitiveOption);
    catname_start = QRegularExpression("class=['\"](?:(?:gray\\s+z-padtop2)|(?:z-padtop2\\s+xgray))['\"]\\s*>",
                            QRegularExpression::CaseInsensitiveOption);
}
// custom methods
// void SetCategory(const jfFFN_CategoryCore* in_cat_link);
//void ClearCategory();
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFNAuthorFanficParser::Clear() {
    ClearCore(true);
    ffn_fic = NULL;
}
//------------------------
const jfSearchResultItem* jfFFNAuthorFanficParser::GetUntypedItem() {
    ffn_fic = NULL;
    return jfItemParseBase::GetUntypedItem();
}
//------------------------
jfFFNItemAuthor* jfFFNAuthorFanficParser::GetFanfic() {
    jfFFNItemAuthor* bak = ffn_fic;
    ffn_fic = NULL;
    untyped_item = NULL;
    return bak;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNAuthorFanficParser::~jfFFNAuthorFanficParser() {
    ffn_fic = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNItemCore* jfFFNAuthorFanficParser::fanfic() const {
    return ffn_fic;
}
//------------------------
void jfFFNAuthorFanficParser::ClearItem() {
    if (ffn_fic != NULL) {
        delete ffn_fic;
        ffn_fic = NULL;
    }
    untyped_item = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfItemParseResultState jfFFNAuthorFanficParser::GetLinkTitle() {
    const QString perr = "Error while parsing fic in FFN Author page: ";

    // next up is the story id
    if (!xparser.MovePast(storyid_start)) {
        return SetError(perr + "Start of Fic ID not found!");
    }
    ulong qval;  QString oerr;
    if (!xparser.GetMovePastULong("/",qval,oerr)) {
        return SetError(perr + oerr + " When getting fic id.");
    }
    ffn_fic->numeric_id = qval;

    // we get the rest of the url
    QString buffer;
    if (!xparser.GetMovePast("\">",buffer)) {
        return SetError(perr + "End of fic link not found!");
    }
    // we then build the url...
    ffn_fic->url = "https://www.fanfiction.net/s/" + QString::number(ffn_fic->numeric_id);
    ffn_fic->url += (QString("/") + buffer);
    // next up is the title
    if (!xparser.MovePast(title_img)) {
        return SetError(perr + "Problems finding image in author link!");
    }
    if (!xparser.GetDelimited(">","</a>",buffer)) {
        return SetError(perr + "End of title field not found!");
    }
    ffn_fic->title = buffer.trimmed();
    return jfItemParseResultState::SUCCEESS;
}
//------------------------
jfItemParseResultState jfFFNAuthorFanficParser::InnerParse() {
    const QString perr = "Error while parsing fic in FFN Author page: ";

    ffn_fic = NULL;
    // we need to create the result first
    ffn_fic = new jfFFNItemAuthor();
    untyped_item = ffn_fic;

    // processing fic id, title, and making the link
    jfItemParseResultState first_result = GetLinkTitle();
    if (first_result != jfItemParseResultState::SUCCEESS) return first_result;

    // get we the descrtption
    QString oerr; QString summary;
    if (!ExtractDescription(oerr, summary)) {
        return SetError(perr + oerr);
    }

    // getting the category name
    if (!xparser.MovePast(catname_start)) {
        return SetError(perr + "Category Name not found! A");
    }
    QString buffer;
    if (!xparser.GetUpto("- Rated:",buffer)) {
        return SetError(perr + "Category Name not found! B");
    }
    ffn_fic->catname = buffer.trimmed();

    // the rest is just like jfFFNItem
    // next, the rating, language, genres, chapter count, and word count
     if (!Rating2Words(oerr)) {
        return SetError(perr + oerr);
     }
    // we skip past the reviews
    if (!xparser.MovePast("Reviews:")) {
      // this is okay, sometimes there are no reviews
    }
    if (!Dates_and_Completion(oerr)) {
        return SetError(perr + oerr);
    }
    // done with all the parsing, just some finishing up...
    return jfItemParseResultState::SUCCEESS;
}

    // jfFFNItemAuthor* ffn_fic;

/*****************************************************************************/
