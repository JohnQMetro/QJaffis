/******************************************************************************
Name    :   fim_ficutils.cpp
Author  :   John Q Metro
Purpose :   Parsers and Displayers for the Fanfic object for fimfiction.net
Created :   March 13, 2023
Updated :   March 18, 2023
******************************************************************************/
#include "fim_ficutils.h"

#ifndef FIM_CONSTANTS_H
  #include "fim_constants.h"
#endif // FIM_CONSTANTS_H

#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED

/*****************************************************************************/
jfFIMFanficOutputter::jfFIMFanficOutputter():jfFanficItemOutputterBase(HTMLHelpers::fim_item_helper) {
    fim_fanfic = NULL;
}
// -------------------------------------
jfFIMFanficOutputter::jfFIMFanficOutputter(const jfDisplayHTMLHelper* in_html_html_helper):jfFanficItemOutputterBase(in_html_html_helper) {
    fim_fanfic = NULL;
}
// --------------------------------------
void jfFIMFanficOutputter::SetFanfic(const jfFIM_Fanfic* in_fim_fanfic) {
    fim_fanfic = in_fim_fanfic;
    fanfic_source = in_fim_fanfic;
    source = in_fim_fanfic;
}
// --------------------------------------
void jfFIMFanficOutputter::SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) {
    assert(in_source != NULL);
    assert(in_source->GetTypeLabel() == IdForFIMFanfics());
    fim_fanfic = dynamic_cast<const jfFIM_Fanfic*>(in_source);
    fanfic_source = fim_fanfic;
    source = in_source;
    flags = in_flags;
}
// ++++++++++++++++++++++++++++++++++++++++++++
// to string functions
// ---------------------
QString jfFIMFanficOutputter::ToText() const {
    QString result,buffer;
    // building the title and author line
    result += fim_fanfic->GetTitle() + " by " + fim_fanfic->GetAuthor() + "\n";
    // next line: link display
    result += fim_fanfic->GetURL() + "\n";
    // adding the main description
    result += GetSummaryExtract(6,750) + "\n";;
    // next up.. two extra lines
    result += "Published: " + (fim_fanfic->GetPublishedDate()).toString("M-d-yyyy");
    result += " - Updated: " + (fim_fanfic->GetUpdates()).toString("M-d-yyyy");
    // the rating...
    result += " - Rating: " + fim_fanfic->GetRating();
    // part count and word count
    result += " - Parts: " + QString::number(fim_fanfic->GetChapterCount());
    result += " - Words: " + QString::number(fim_fanfic->GetWordcount()) + "\n";
    // first of 2 possible lines
    bool hasGenre = !(fim_fanfic->GetGenres()).isEmpty();
    bool hasCT = !(fim_fanfic->GetContentTypes()).isEmpty();
    bool hasWarn = !(fim_fanfic->GetWarnings()).isEmpty();
    if (hasGenre) {
        result += "Genres: " + fim_fanfic->GetGenres();
    }
    if (hasCT) {
        if (hasGenre) result += " - ";
        result += "Types: " + fim_fanfic->GetContentTypes();
    }
    if (hasWarn) {
        if (hasGenre || hasCT) result += " - ";
        result += "Warnings: " + fim_fanfic->GetWarnings();
    }
    // second of 2 possible lines
    bool hasChar = !(fim_fanfic->GetCharacterList()).isEmpty();
    if (hasChar || (fim_fanfic->IsComplete())) result += "\n";
    // character data
    if (hasChar) {
        result += "Characters Specified: " + (fim_fanfic->GetCharacterList()).join(", ");
    }
    if (fim_fanfic->IsComplete()) {
        if (hasChar) result += " - ";
        result += "Complete";
    }
    // finishing off
    result += "\n";
    // done
    return result;
}
// --------------------
QString jfFIMFanficOutputter::ToDisplayHTML() const {
    const QString fname = "jfFFNItem::ToDisplayHTML";

    QString result,buffer;

    // we start with the table
    result = DisplayHTMLHeader(html_helper);
    result += "</td></tr>\n";
    // next line: link display
    result += "<tr><td>";
    // next line: link display
    result += html_helper->WrapText("ficlink", fim_fanfic->GetURL(), true);

    // adding the main description
    result += "<div style=\"margin-left:10px;\">";
    QString rdesc = GetSummaryExtract(6,1000);
    result += html_helper->WrapTextNoEsc("description",rdesc,false);
    result += "</div>\n";

    // next up.. metadata
    QString mdata = "Published: " + (fim_fanfic->GetPublishedDate()).toString("MM-dd-yyyy");
    mdata += " - Updated: " + (fim_fanfic->GetUpdates()).toString("MM-dd-yyyy");
    mdata += " - Rating: " + fim_fanfic->GetRating();
    // part count and word count
    mdata += " - Parts: " + QString::number(fim_fanfic->GetChapterCount());
    mdata += " - Words: " + QString::number(fim_fanfic->GetWordcount());
    // thumbs up and down
    int thumbsup = fim_fanfic->GetThumbs(false);
    if (thumbsup < 0) mdata += " - Rating Disabled";
    else {
        mdata += " - Thumbs Up: " + QString::number(thumbsup);
        mdata += " - Thumbs Down: " + QString::number(fim_fanfic->GetThumbs(true));
    }
    result += "<div>";
    result += html_helper->WrapText("basicinfo", mdata, false);
    if (fim_fanfic->IsComplete()) {
        result += html_helper->WrapTextNoEsc("basicinfo"," - <b>Complete</b>",false);
    }
    result += "<br>\n";

    // genres and content type...
    result += html_helper->ConditionalWrapText("genres", false, "Genres: ", true, fim_fanfic->GetGenres(), false);
    result += html_helper->ConditionalWrapText("basicinfo", true, "Types: ", true, fim_fanfic->GetContentTypes(), false);
    result += html_helper->ConditionalWrapText("warnings", true, "Warnings: ", true, fim_fanfic->GetWarnings(), false);

    result += "<br>\n";
    // characters
    result += html_helper->ConditionalWrapText("characters", false, "Characters: ", true,
                                               (fim_fanfic->GetCharacterList()).join(", "), false);

    // finishing off
    result += "</div></td></tr>\n</table>";
    // done
    return result;
}
// +++++++++++++++++++++++++++++++
jfFIMFanficOutputter::~jfFIMFanficOutputter() {
    fim_fanfic = NULL;
}
// ==========================================================================
// ==========================================================================
// class jfFIMFanficItemParser : public jfItemParseBase {

jfFIMFanficItemParser::jfFIMFanficItemParser():jfItemParseBase(false) {
    result = NULL;
    english_locale = QLocale(QLocale::English, QLocale::UnitedStates);
}
// ++++++++++++++++++++++++++++++++++++++++
// clearing or getting results
void jfFIMFanficItemParser::Clear() {
    ClearCore(true);
    result = NULL;
}
// ---------------------------------
const jfSearchResultItem* jfFIMFanficItemParser::GetUntypedItem() {
    result = NULL;
    return jfItemParseBase::GetUntypedItem();
}
// ---------------------------------
jfFIM_Fanfic* jfFIMFanficItemParser::GetFanfic() {
    untyped_item = NULL;
    jfFIM_Fanfic* result_bak = result;
    result = NULL;
    return result_bak;
}
// ++++++++++++++++++++++++++++++++++++++++
// destructor
jfFIMFanficItemParser::~jfFIMFanficItemParser() {
    result = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++
void jfFIMFanficItemParser::ClearItem() {
    if (result != NULL) {
        delete result;
        result = NULL;
    }
    untyped_item = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++++
// parsing methods
jfItemParseResultState jfFIMFanficItemParser::InnerParse() {
    const QString fname = "jfFIMFanficItemParser::InnerParse";
    const QString perr = "Error parsing FIM Fanfic: ";
    // constants
    const QString avatartag = "<img src=\"//www.fimfiction-static.net/images/avatars/";
    const QString startdesc = "<span class=\"description-text bbcode\">";
    const QString enddesc = "</section>";
    const QString datestart = "<span class=\"date\"><b>&#xb7;</b>";

    // variables
    QString oerr;
    // starting
    result = NULL;

    // avatar author id checking...
    unsigned long qval;
    size_t author_id;
    if (!xparser.GetDelimitedULong(avatartag,"_64.jpg\"", qval, oerr)) author_id = 0;
    else author_id = qval;

    // rating
    if (!xparser.MovePast(" data-controller=\"rate-story\">")) {
        return SetError(perr + "Cannot find likes! A");
    }
    QString buffer;
    int thumbsup, thumbsdown;

    // looking for likes
    if (xparser.GetDelimited("<span class=\"likes\">","</span>",buffer)) {
        if (buffer=="") thumbsup = 0;
        else if (Str2ULongC(buffer,qval)) thumbsup = qval;
        else return SetError(perr + "Likes value is not a number!");

        if (!xparser.GetDelimited("<span class=\"dislikes\">","</span>",buffer)) {
            return SetError(perr + "Cannot find dislikes!");
        }
        if (buffer=="") thumbsdown = 0;
        else if (Str2ULongC(buffer,qval)) thumbsdown = qval;
        else return SetError(perr + "Dislikes value is not a number!");
    }
    // failing that, we look for 'Ratings Disabled'
    else if (xparser.MovePastLimit("Ratings Disabled","<div class=\"divider\">")) {
        thumbsup = -1;
        thumbsdown = 0;
    }
    else return SetError(perr + "Cannot find likes! B");

    // getting the fic id
    if (!xparser.GetDelimitedULong("id=\"story_","\"",qval,oerr)) {
        return SetError(perr + oerr + " While getting story id.");
    }
    size_t num_id = qval;

    // next, the rating
    if (!xparser.MovePast("<a href=\"/stories?content_rating=")) {
        return SetError(perr + "Could not find rating!");
    }
    if (!xparser.GetDelimited(">","</a>", buffer)) {
        return SetError(perr + "Could not extract rating!");
    }
    QString rating = fimcon::RatLookup((buffer.trimmed())[0]);

    // building the primary link
    if (!xparser.MovePast("<a class=\"story_name\" href=\"/story/")) {
        return SetError(perr + "Cannot find primary link info!");
    }
    if (!xparser.GetDelimited("/","\"",buffer)) {
        return SetError(perr + "Cannot find end of primary link!");
    }
    QString primarylink = "https://www.fimfiction.net/story/" + QString::number(num_id) + "/" + buffer;

    // the title
    if (!xparser.GetDelimited(">","</a>",buffer)) {
        return SetError(perr + "Cannot find title!");
    }
    QString title = htmlparse::HTML2Text(buffer);

    result = new jfFIM_Fanfic();
    untyped_item = result;
    result->numeric_id = num_id;
    result->title = title;
    result->url = primarylink;
    result->author_id = author_id;
    result->thumbsup = thumbsup;
    result->thumbsdown = thumbsdown;
    result->rating = rating;

    // the author name
    if (!xparser.MovePast("<a href=\"/user/")) {
        return SetError(perr + "Cannot find start of author id name!");
    }
    // getting the author ID and link-name
    if (!xparser.GetMovePastULong("/",qval,oerr)) {
        return SetError(perr + "Could not get Author ID : " + oerr);
    }
    if (result->author_id == 0) result->author_id = qval;
    if (!xparser.GetMovePast("\">",buffer)) {
        return SetError(perr + "Could find end of Author link!");
    }
    // setting the author link
    QString author_url = "https://www.fimfiction.net/user/" + QString::number(result->author_id);
    author_url += "/" + buffer;
    result->author_url = author_url;
    // getting the author name
    if (!xparser.GetMovePast("</a>",buffer)) {
        return SetError(perr + "Cannot find author name!");
    }
    result->author_name = buffer;

    // moving to the 'categories' (aka genres)
    if (!xparser.MovePast("<ul class=\"story-tags\">")) {
        return SetError(perr + "Story-Tags box not found!");
    }

    // parsing the tags
    jfItemParseResultState tagResult = ParseFIMTags();
    if (tagResult != jfItemParseResultState::SUCCEESS) {
        return tagResult;
    }

    // getting the summary
    if (!xparser.GetDelimited(startdesc,enddesc,buffer)) {
        return SetError(perr + "Description missing!");
    }
    int lpos = buffer.lastIndexOf("</span>",-1,Qt::CaseInsensitive);
    if (lpos != -1) buffer.truncate(lpos+7);
    result->summary = CleanUpSummary(buffer.trimmed());
    ;
    /* Chapter links. Due to a lack of updated dates in fimfiction 4, we collect the dates for
     * each chapter to determine the updated date. */
    int findwhich = 0;
    QDate latedate = QDate(1900,1,1);
    QDate currdate;
    while (xparser.MovePast("<li data-published class=\"")) {
        findwhich++;
        if (!xparser.GetDelimitedEndPair(datestart,"<span ","</span>",buffer)) {
            return SetError(perr + "Unable to extract updated date for chapter!");
        }
        buffer = buffer.trimmed();
        if (!ParseFIMDate(buffer,currdate)) {
            return SetError(perr + "Unable to parse chapter date!");
        }
        if (currdate > latedate) latedate = currdate;
    }
    if (findwhich==0) {
        // this actually happens, so not really a parsing error
        ClearItem();
        return jfItemParseResultState::DEFECTIVE;
    }

    result->part_count = findwhich;

    // status
    if (!xparser.MovePast("<span class=\"completed-status")) {
        return SetError(perr + "No status span found!");
    }
    if (!xparser.GetDelimited(">","</span>",buffer)) return SetError(perr + "Status not found!");
    result->completed = (buffer.trimmed() == "Complete");

    // published date
    jfItemParseResultState dateResult = ParsePublishedDate(latedate);
    if (dateResult != jfItemParseResultState::SUCCEESS) {
        return dateResult;
    }

    // word count
    if (!xparser.MovePast("<div class=\"word_count\">")) return SetError(perr + "Missing wordcount");
    if (!xparser.GetDelimitedULong("<b>","</b>",qval,oerr)) {
        return SetError(perr + "Could not parse wordcount! : " + oerr);
    }
    result->word_count = qval;

    // finishing touches
    return jfItemParseResultState::SUCCEESS;
}
// --------------------------------
jfItemParseResultState jfFIMFanficItemParser::ParseFIMTags() {
    const QString perr = "Error parsing FIM Fanfic: ";
    QString buffer, buffer2;

    QStringList characters;
    QString content_types, warnings,genres;
    // Tag checking loop
    while (xparser.MovePast("class=\"tag-")) {
        // getting the tag
        if (!xparser.GetMovePast("\"",buffer)) return SetError(perr + "Could not get tag type!");
        if (!xparser.GetDelimited(">","</a>",buffer2)) return SetError(perr + "missing genre link");
        // converting it to the internal format...
        if (buffer=="character") characters.append(buffer2.trimmed());
        else if (buffer=="content") {
            if (!content_types.isEmpty()) content_types += ", ";
            content_types += buffer2.trimmed();
        }
        else if (buffer=="warning") {
            if (!warnings.isEmpty()) warnings += ", ";
            warnings += buffer2.trimmed();
        }
        // series will be treated as a genre for now
        else {
            if (!genres.isEmpty()) genres += ", ";
            genres += buffer2.trimmed();
        }
    }
    result->characters = characters;
    result->content_types = content_types;
    result->warnings = warnings;
    result->genres = genres;

    return jfItemParseResultState::SUCCEESS;
}
// --------------------------
jfItemParseResultState jfFIMFanficItemParser::ParsePublishedDate(const QDate& latedate) {
    const QString perr = "Error parsing FIM Fanfic: ";
    bool haspubdate = true;
    ulong qval;

    // this is more complicated than it used to be
    if (!xparser.MovePast("<span class=\"desktop\"><b>Published:</b> </span><")) {
        /* since I've come cross a case of a story with no pubdate, I have to deal with
        it, instead of assuming a parsing error */
        haspubdate = false;
    }
    else {
        QString oerr;
        if (!xparser.GetDelimitedULong("data-time=\"","\"",qval,oerr)) {
            haspubdate = false;
        }
        else haspubdate = true;
    }
    if (haspubdate) {
        // the extracted date is in Unix TimeStamp format
        QDateTime temppubdate;
        temppubdate.setTime_t(qval);
        if (!temppubdate.isValid()) return SetError(perr + "Could not parse published date!");
        QDate pubdate = temppubdate.date();
        // annoyingly, with fimfiction 4 there is no modified/updated date anymore!
        // so I use a date built while scanning the list of chapters
        if (latedate > pubdate) result->updated_date = latedate;
        else result->updated_date = pubdate;
    }
    else result->updated_date = latedate;
    return jfItemParseResultState::SUCCEESS;
}
// ---------------------------------
bool jfFIMFanficItemParser::ParseFIMDate(QString insrc, QDate& out) const {
    const QString fname = "jfFIM_Fanfic::ParseFIMDate";
    // variables
    QString split1,split2;
    int spos;
    // splitting the string
    insrc = insrc.trimmed();
    spos = insrc.indexOf(' ');
    if (spos<0) return false;
    split1 = insrc.left(spos);
    split2 = insrc.mid(spos+1).trimmed();
    // checking and truncating the first part (to get rid of the 'st', 'nd', 'rd', 'th' )
    if (split1.length()<3) return false;
    split1.chop(2);
    // rebuilding, after which we can use QDate::ParseFormat
    insrc = split1 + " " + split2;
    out = english_locale.toDate(insrc,"d MMM yyyy");
    // out = QDate::fromString(insrc,"d MMM yyyy");
    // returning the success
    return (out.isValid());
}
// ---------------------------------
QString jfFIMFanficItemParser::CleanUpSummary(QString&& raw_description) const {
    jfStringParser* vparse;
    QString bufx, nrerep;
    bool imgrep, qres;
    // skipping the images
    imgrep = false;
    vparse = new jfStringParser(raw_description);
    while(vparse->GetMovePast("<img",bufx)) {
        imgrep = true;
        nrerep += bufx;
        qres = vparse->MovePast(">");
        if (!qres) break;
    }
    if (imgrep) {
        if (qres) {
            vparse->GetToEnd(bufx);
            nrerep += bufx;
        }
        delete vparse;
        raw_description = nrerep;
    }
    // post processing
    raw_description.replace("</p><br /><p><p class=\"double\"><hr>","<br><br>\n",Qt::CaseInsensitive);
    raw_description.replace("</p><p class=\"double\"><hr></p>","<br><br>\n",Qt::CaseInsensitive);
    raw_description.replace("</p><p><hr></p>","<br><br>\n",Qt::CaseInsensitive);
    raw_description.replace("<hr />","<br><br>\n",Qt::CaseInsensitive);
    raw_description.replace("</p><p class=\"double\">","<br><br>\n",Qt::CaseInsensitive);
    raw_description.replace("</p>","<br>\n",Qt::CaseInsensitive);
    raw_description.replace("<br><br>\n<br>\n","<br><br>\n",Qt::CaseInsensitive);
    raw_description.replace("<p>"," ",Qt::CaseInsensitive);
    raw_description = raw_description.simplified();
    raw_description += "</center>";
    return raw_description;
}

// =============================================================================
// =============================================================================

jfFIMExtractParser::jfFIMExtractParser():jfFicExtractParseBase(true) {
    fim_extract = NULL;
    english_locale = QLocale(QLocale::English, QLocale::UnitedStates);
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMExtractParser::Clear() {
    ClearCore(true);
    fim_extract = NULL;
}
// ----------------------------------
const jfFicExtract* jfFIMExtractParser::GetUntypedItem() {
    jfFicExtract* copy_item = untyped_item;
    untyped_item = NULL;
    fim_extract = NULL;
    has_result = false;
    return copy_item;
}
// ----------------------------------
const jfFicExtract_FIM* jfFIMExtractParser::GetFIMExtract() {
    jfFicExtract_FIM* copy_item = fim_extract;
    untyped_item = NULL;
    fim_extract = NULL;
    has_result = false;
    return copy_item;
}
// ----------------------------------
jfFIMExtractParser::~jfFIMExtractParser() {
    fim_extract = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMExtractParser::ClearItem() {
    if (fim_extract != NULL) delete fim_extract;
    fim_extract = NULL;
    untyped_item = NULL;
}
// ----------------------------------
jfItemParseResultState jfFIMExtractParser::InnerParse() {
    // constants
    const QString fname = "jfFIMExtractParser::InnerParse";
    const QString perr = "Error while parsing FIM Extract: ";
    const QString dstr1 = "<a href=\"/user/";
    const QString partstart = "<article class=\"story_container\"";
    const QString partend = "</article>";

    fim_extract = NULL;

    unsigned long qval;
    QString errval, buffer;

    // starting
    if (!xparser.MovePast("<div class=\"user-page-header\"")) {
        return SetError(perr + "Missing User Page Header");
    }
    // getting author id and name
    if (!xparser.GetDelimitedULong(dstr1,"/",qval,errval)) {
        return SetError(perr + errval);
    }
    size_t rec_authid = qval;

    if (!xparser.GetMovePast("\"",buffer)) {
        return SetError(perr + "Error while getting Author Link!");
    }
    QString rec_authname;
    if (!xparser.GetDelimited(">","</a>",rec_authname)) {
        return SetError(perr + "Error while getting Author Name!");
    }
    rec_authname = rec_authname.trimmed();

    // building the author link
    QString rec_authlink = "https://www.fimfiction.net/user/" + QString::number(rec_authid) + "/";
    rec_authlink += buffer.trimmed();


    // extracting the main contents
    if (!xparser.GetDelimited(partstart,partend,buffer)) {
        buffer = xparser.GetBlock(2000);
        return SetError(perr + "could not get main contents\n"+buffer);
    }
    // the following method parses the contents
    jfItemParseResultState coreResult = CoreParse(std::move(buffer));
    if (coreResult != jfItemParseResultState::SUCCEESS) return coreResult;

    // finishing
    fim_extract->auth_id = rec_authid;
    fim_extract->author_name = rec_authname;

    return jfItemParseResultState::SUCCEESS;

}

//------------------------------------------------------------
/* In the original implementation, I used the Fic Item parser, but since I
 * have separated the extract parser, I replace it with a custom parser here:
 *GOAL : title, numeric id, part count, word count, updated date, and complete state. */
jfItemParseResultState jfFIMExtractParser::CoreParse(QString&& rawcore) {
    const QString fname = "jfFIMExtractParser::CoreParse";
    const QString perr = "Error parsing FIM Fanfic for Extract: ";

    xparser.ChangeData(rawcore,true);

    // we go directly to the fic id
    if (!xparser.MovePast("<div class=\"story_content_box\"")) {
        return SetError(perr + "Cannot find content box");
    }
    QString oerr; ulong qval;
    // getting the fic id
    if (!xparser.GetDelimitedULong("id=\"story_","\"",qval,oerr)) {
        return SetError(perr + oerr + " While getting story id.");
    }
    size_t num_id = qval;

    // the title
    if (!xparser.MovePast("<a class=\"story_name\"")) {
        return SetError(perr + "Cannot find title start!");
    }
    QString buffer;
    // the title
    if (!xparser.GetDelimited(">","</a>",buffer)) {
        return SetError(perr + "Cannot find title!");
    }
    QString title = htmlparse::HTML2Text(buffer);

    // additional parsing to get part names and dates
    if (!xparser.MovePast("<!--Chapters-->")) {
        return SetError(perr + "Cannot find chapters!");
    }
    const QString datestart = "<span class=\"date\"><b>&#xb7;</b>";
    QStringList extracted_names;
    QString namebuf;
    int findwhich = 0;
    QDate latedate = QDate(1900,1,1);
    QDate currdate;

    while (xparser.MovePast("<li data-published class=\"")) {
        if (!xparser.MovePast("<a class=\"chapter-title\" href=\"/story/")) {
            return SetError(perr + "Cannot find chapter title!");
        }
        if (!xparser.GetDelimited("\">","</a>",namebuf)) {
            return SetError(perr + "Problems in extracting part names");
        }
        extracted_names.append(htmlparse::ConvertEntities(namebuf.trimmed(),false));
        if (!xparser.GetDelimitedEndPair(datestart,"<span ","</span>",buffer)) {
            return SetError(perr + "Unable to extract updated date for chapter!");
        }
        buffer = buffer.trimmed();
        if (!ParseFIMDate(buffer,currdate)) {
            return SetError(perr + "Unable to parse chapter date!");
        }
        if (currdate > latedate) latedate = currdate;
        findwhich++;
    }

    if (findwhich==0) {
        // this actually happens, so not really a parsing error
        ClearItem();
        return jfItemParseResultState::DEFECTIVE;
    }

    if (!xparser.MovePast("<div class=\"chapters-footer\">")) {
        return SetError(perr + "Cannot find story footer.");
    }

    // this seems like an okay spot to create the extract
    fim_extract = new jfFicExtract_FIM();
    untyped_item = fim_extract;
    fim_extract->fic_id = num_id;
    fim_extract->fic_title = title;
    fim_extract->pcount = findwhich;
    fim_extract->partnames = extracted_names;
    fim_extract->updated_date = latedate;

    // status
    if (!xparser.MovePast("<span class=\"completed-status")) {
        return SetError(perr + "No status span found!");
    }
    if (!xparser.GetDelimited(">","</span>",buffer)) {
        return SetError(perr + "Status not found!");
    }
    fim_extract->complete = (buffer.trimmed() == "Complete");

    // word count
    if (!xparser.MovePast("<div class=\"word_count\">")) return SetError(perr + "Missing wordcount");
    if (!xparser.GetDelimitedULong("<b>","</b>",qval,oerr)) {
        return SetError(perr + "Could not parse wordcount! : " + oerr);
    }
    fim_extract->wordcount = qval;

    // finishing touches
    return jfItemParseResultState::SUCCEESS;
}
// ---------------------------------
bool jfFIMExtractParser::ParseFIMDate(QString insrc, QDate& out) const {
    const QString fname = "jfFIM_Fanfic::ParseFIMDate";
    // variables
    QString split1,split2;
    int spos;
    // splitting the string
    insrc = insrc.trimmed();
    spos = insrc.indexOf(' ');
    if (spos<0) return false;
    split1 = insrc.left(spos);
    split2 = insrc.mid(spos+1).trimmed();
    // checking and truncating the first part (to get rid of the 'st', 'nd', 'rd', 'th' )
    if (split1.length()<3) return false;
    split1.chop(2);
    // rebuilding, after which we can use QDate::ParseFormat
    insrc = split1 + " " + split2;
    out = english_locale.toDate(insrc,"d MMM yyyy");
    // out = QDate::fromString(insrc,"d MMM yyyy");
    // returning the success
    return (out.isValid());
}

/*****************************************************************************/
