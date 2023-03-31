/******************************************************************************
Name    :   ffn_ficutils.cpp
Author  :   John Q Metro
Purpose :   Outputters and Parsers for Fanfiction.Net fic items.
Created :   March 21, 2023
Updated :   March 22, 2023
******************************************************************************/

#include "ffn_ficutils.h"

#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED

/*****************************************************************************/
jfFFNItemOutputter::jfFFNItemOutputter():jfFanficItemOutputterBase(HTMLHelpers::ffn_item_helper) {
    ffn_fanfic = NULL;
}
// -------------------------------------------
jfFFNItemOutputter::jfFFNItemOutputter(const jfDisplayHTMLHelper* in_html_helper):jfFanficItemOutputterBase(in_html_helper) {
    ffn_fanfic = NULL;
}
// +++++++++++++++++++++++++++++++++++++++
void jfFFNItemOutputter::SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) {
    assert(in_source != NULL);
    assert(in_source->GetTypeLabel() == jfFFNItemCore::FFN_CORE_TYPE_ID);
    ffn_fanfic = dynamic_cast<const jfFFNItemCore*>(in_source);
    fanfic_source = dynamic_cast<const jfGenericFanfic*>(in_source);
    source = in_source;
    flags = in_flags;
}
// --------------------------------------------
void jfFFNItemOutputter::SetFFNItemSource(const jfFFNItemCore* in_source, const jfItemMetaFlags* in_flags) {
    assert(in_source != NULL);
    ffn_fanfic = in_source;
    fanfic_source = in_source;
    source = in_source;
    flags = in_flags;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// to string functions
QString jfFFNItemOutputter::ToText() const {
    QString result,buffer;
    // building the title and author line
    result = ffn_fanfic->GetTitle() + " by " + ffn_fanfic->GetAuthor() + "\n";
    // next line: link display
    result += ffn_fanfic->GetURL() + "\n";
    // adding the main description
    result += ffn_fanfic->GetSummary() + "\n";
    // next up.. two extra lines
    result += "Published: ";
    result += (ffn_fanfic->GetPublished()).toString("MM-dd-yyyy");
    if (ffn_fanfic->IsUpdated()) {
      result += " - Updated: " + (ffn_fanfic->GetUpdates()).toString("MM-dd-yyyy");
    }
    // the rating...
    if (ffn_fanfic->GetRating() =='+') buffer = "K+";
    else buffer = ffn_fanfic->GetRating();
    result += " - Rating: " + buffer;
    // part count and word count
    result += " - Parts: " + QString::number(ffn_fanfic->GetChapterCount());
    result += " - Words: " + QString::number(ffn_fanfic->GetWordcount());
    result += " - Favs: " + QString::number(ffn_fanfic->GetFavs()) + "\n";
    // final info,
    result += ffn_fanfic->GetLanguage();
    if (!(ffn_fanfic->GetGenres()).isEmpty()) {
      result += " - Genres: " + ffn_fanfic->GetGenres();
    }
    // character data
    if (!(ffn_fanfic->GetCharacterList()).isEmpty()) {
      result += " - Characters Specified: " + (ffn_fanfic->GetCharacterList()).join(", ");
    }
    // pairings
    if (ffn_fanfic->RelationshipCount() > 0) {
        result += " - Pairings: " + ffn_fanfic->RelationshipsAsDisplayString(true);
    }

    if (ffn_fanfic->IsComplete()) {
      result += " - Complete";
    }
    // finishing off
    result += "\n";
    // done
    return result;
}
// ---------------------------------------------
QString jfFFNItemOutputter::ToDisplayHTML() const {
    const QString fname = ("jfFFNItem::ToDisplayHTML");
    QString result,buffer;
    // we start with the table
    result = DisplayHTMLHeader(html_helper);
    result += ("</td></tr>\n");
    // next line: link display
    result += ("<tr><td>");
    result += html_helper->WrapText("ficlink", ffn_fanfic->GetURL(), true);

    // adding the main description
    result += "<div style=\"margin-left:10px;\">";
    QString description = ffn_fanfic->GetSummary();
    QString rdesc = (description.length() > 1000) ? (description.left(1000) + "...") : description;
    result += html_helper->WrapTextNoEsc("description",rdesc,false);
    result += "</div>";

    // next up.. two extra lines
    QString mdata = "Published: " + (ffn_fanfic->GetPublished()).toString("MM-dd-yyyy");
    if (ffn_fanfic->IsUpdated()) {
      mdata += " - Updated: " + (ffn_fanfic->GetUpdates()).toString("MM-dd-yyyy");
    }
    // the rating...
    if ((ffn_fanfic->GetRating())==('+')) buffer = ("K+");
    else buffer = ffn_fanfic->GetRating();
    mdata += (" - Rating: ") + buffer;
    // part count and word count
    mdata += " - Parts: " + QString::number(ffn_fanfic->GetChapterCount());
    mdata += " - Words: " + QString::number(ffn_fanfic->GetWordcount());
    mdata += " - Favs: " + QString::number(ffn_fanfic->GetFavs());
    if (ffn_fanfic->IsComplete()) {
      mdata += " - Complete";
    }
    result += html_helper->WrapText("basicinfo", mdata, true);

    // characters
    result += html_helper->ConditionalWrapText("characters", false,"Characters: ", true, (ffn_fanfic->GetCharacterList()).join(","), false);
    // relationships
    result += html_helper->ConditionalWrapText("pairings", true,"Pairings: ", true, ffn_fanfic->RelationshipsAsDisplayString(true), false);
    // genres
    result += html_helper->ConditionalWrapText("genres", true, "Genres: ", true, ffn_fanfic->GetGenres(), true);

    // finishing off
    result += ("</td></tr>\n</table>");
    // done
    return result;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNItemOutputter::~jfFFNItemOutputter() {
    ffn_fanfic = NULL;
}

//=========================================================================
/* A base class for item parsing */
jfFFNFanficParseBase::jfFFNFanficParseBase():jfItemParseBase(false) {
    summary_start = QRegularExpression(
                "<div\\s+class=['\"]?z-indent\\s+z-padtop['\"]?>",
                QRegularExpression::CaseInsensitiveOption);
    date_start = QRegularExpression("<span\\s+data-xutime=['\"]",QRegularExpression::CaseInsensitiveOption);

}
// ------------------------------------------
jfFFNFanficParseBase::~jfFFNFanficParseBase() {

}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFFNFanficParseBase::ExtractDescription(QString &parserr, QString& out_description) {
    // going ahed
    if (!xparser.MovePast(summary_start)) {
        return failMsg(parserr,"Could not find start of description!");
    }
    if (!xparser.GetMovePast("<div ", out_description)) {
        return failMsg(parserr,"Could not find end of description!");
    }
    return true;
}
//-------------------------------------------------
bool jfFFNFanficParseBase::Rating2Words(QString &parserr) {
    // varibales
    QString buffer,buffer2;
    QString xerr;
    unsigned long qval;
    // starting
    // extrating the rating
    if (!(xparser.GetDelimited("Rated:","-",buffer))) return failMsg(parserr,"Rating not found!");
    buffer = buffer.trimmed();
    if (buffer=="K+") fanfic()->rating = '+';
    else {
        if (buffer.length()!=1) return failMsg(parserr,"Rating is malformed!");
        fanfic()->rating = buffer[0];
    }
    // next is the language
    if (!xparser.GetMovePast("- ",buffer)) return failMsg(parserr,"Language section not found!");
    fanfic()->language = buffer.trimmed();
    // next is the genres, or possibly the chapters
    if (!xparser.StartsWithAtIndex("Chapters:")) {
        if (xparser.GetMovePast("Sci-",buffer2)) buffer2+="Sci-";
        else buffer2 = "";
        if (!xparser.GetMovePast("-",buffer)) return failMsg(parserr,"Genres section not found!");
        fanfic()->genres = buffer2 + buffer;
    }
    // Chapter count
    if (!xparser.GetDelimitedULong("Chapters:","-", qval,xerr)) {
        return failMsg(parserr,xerr + " When getting chapter count.");
    }
    fanfic()->part_count = qval;
    // word count
    if (!xparser.GetDelimitedULong("Words:","-", qval,xerr)) {
        return failMsg(parserr,xerr + " When getting word count.");
    }
    fanfic()->word_count = qval;
    // number of favs
    if (!xparser.GetDelimitedULong("Favs:","-", qval,xerr)) {
        fanfic()->favs = 0;
    }
    else fanfic()->favs = qval;
    return true;
}
// -------------------------------------------------------------------------
// I find that to properly handle that character list, I need to split and keep the delimiters
QStringList jfFFNFanficParseBase::SplitCharacters(const QString& source) const {
    const QChar comm = QChar(',');
    const QChar opbr = QChar('[');
    const QChar clbr = QChar(']');
    QStringList result;
    result.reserve(10);
    QString charbuf;
    charbuf.reserve(25);

    // we now go over the string character by character
    for (int ind = 0; ind < source.size(); ind++) {
        QChar curr = source.at(ind);
        if ((curr == comm) || (curr == opbr) || (curr == clbr)) {
            charbuf = charbuf.trimmed();
            if (charbuf.size() > 0) result.append(charbuf);
            charbuf = "";
            result.append(QString(curr));
        }
        else charbuf += curr;
    }
    if (charbuf.size() > 0) {
        charbuf = charbuf.trimmed();
        if (charbuf.size() > 0) result.append(charbuf);
    }
    return result;
}
// --------------------------------------------------------------------------
bool jfFFNFanficParseBase::Characters_Pairs_and_Completion(QString &parserr) {
    QString buffer, buffer2;
    QString raw_characters;

    fanfic()->completed = false;

    // If we do have fields, the split is always at the last dash, using a custom method...
    if (xparser.GetAfterLast("-",buffer)) {
        // buffer 2 is character data
        buffer2 = xparser.GetSkipped();
        buffer2 = buffer2.trimmed();
        // buffer is probably completion
        buffer = buffer.trimmed();
        if (buffer=="Complete") fanfic()->completed = true;
        // but maybe not, if the name has a dash...
        else buffer2 = buffer2 + "-" + buffer;
        raw_characters = buffer2;
    }
    // below, there must be only 1 field
    else {
        bool moref = xparser.GetToEnd(buffer);
        if (! moref) {
            return failMsg(parserr,"Strange error after dates!");
        }
        buffer = buffer.trimmed();
        // What is the field? I will assume there is no character known as 'Complete'
        if (buffer=="Complete") fanfic()->completed = true;
        else raw_characters = buffer;
    }

    // processing characters
    if (! raw_characters.isEmpty()) {
        QStringList char_parts = SplitCharacters(raw_characters);
        bool inpair = false;
        QStringList pair_members;

        // the list contains both characters and separators
        for (QString char_part : char_parts) {
            if (char_part == "[") {
                // pairing start
                inpair = true;
            }
            else if (char_part == ",") {
                // merely a divider
            }
            else if (char_part == "]") {
                if (inpair) {
                    // the pairing has come to an end, save the data
                    inpair = false;
                    if (pair_members.size() > 0) {
                        fanfic()->char_pairs.append(new jfPairingStruct(pair_members, false));
                        pair_members.clear();
                    }
                }
            }
            else {
                // character processing
                QString parsed_character = htmlparse::ConvertEntities(char_part,false);
                fanfic()->characters.append(parsed_character);
                if (inpair) pair_members.append(parsed_character);

            }
        }
        // to avoid discarding data if we end somehow still inside a pair...
        if (inpair) {
            if (pair_members.size() > 0) {
                fanfic()->char_pairs.append(new jfPairingStruct(pair_members, false));
                pair_members.clear();
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool jfFFNFanficParseBase::Dates_and_Completion(QString &parserr) {
    // varibales
    QString buffer,xoerr,buffer2;
    bool uset;
    unsigned long xoutval;
    QDateTime xdatetime;
    // starting
    uset = false;
    // next is the optional Updated Section
    if (xparser.MovePast("Updated:")) {
        if (!xparser.MovePast(date_start)) {
            return failMsg(parserr,"Cannot find start of Updated Date");
        }
        if (!xparser.GetMovePastAltULong("\">","'>",xoutval,xoerr)) {
            return failMsg(parserr,"Problem with Updated Date :" + xoerr);
        }

        xdatetime.setTime_t((uint)(xoutval));
        fanfic()->updated_date = xdatetime.date();
        uset = true;
    }
    // the published section is next
    if (!xparser.MovePast("Published:")) {
        return failMsg(parserr,"Published section is not found!");
    }
    if (!xparser.MovePast(date_start)) {
        return failMsg(parserr,"Cannot find start of Published Date");
    }
    if (!xparser.GetMovePastAltULong("\">","'>",xoutval,xoerr)) {
        return failMsg(parserr,"Problem with Published Date :" + xoerr);
    }

    xdatetime.setTime_t((uint)(xoutval));
    fanfic()->published = xdatetime.date();
    if (!uset) fanfic()->updated_date = fanfic()->published;
    fanfic()->isupdated = uset;
    // first, there is always a dash if at least 1 field is there
    if (xparser.MovePast("-")) {
        return Characters_Pairs_and_Completion(parserr);
    }
    else return true;
}
//-----------------------------
bool jfFFNFanficParseBase::failMsg(QString &pto, const QString message) const {
  pto = "PARSE ERROR: " + message;
  return false;
}

// ======================================================================
// ======================================================================

jfFFNFanficParse::jfFFNFanficParse():jfFFNFanficParseBase() {
    ffn_fic = NULL;
    storyid_start = QRegularExpression(
                "<a\\s+class=['\"]?stitle['\"]?\\s+href=['\"]?/s/",
                QRegularExpression::CaseInsensitiveOption);

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom methods
// -----------------------------
void jfFFNFanficParse::SetCategory(const jfFFN_CategoryCore* in_cat_link) {
    cat_link = in_cat_link;
}
// -----------------------------
void jfFFNFanficParse::ClearCategory() {
    cat_link = NULL;
}
// -----------------------------
void jfFFNFanficParse::Clear() {
    ClearCore(true);
    ffn_fic = NULL;
}
// -----------------------------
const jfSearchResultItem* jfFFNFanficParse::GetUntypedItem() {
    ffn_fic = NULL;
    return jfItemParseBase::GetUntypedItem();
}
// -----------------------------
const jfFFNItem* jfFFNFanficParse::getFanfic() {
   jfFFNItem* ffn_bak = ffn_fic;
   ffn_fic = NULL;
   untyped_item = NULL;
   return ffn_bak;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNFanficParse::~jfFFNFanficParse() {
    ffn_fic = NULL;
    cat_link = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFFNItemCore* jfFFNFanficParse::fanfic() const {
    return ffn_fic;
}
// -----------------------------------------------
void jfFFNFanficParse::ClearItem() {
    if (ffn_fic != NULL) {
        delete ffn_fic;
        ffn_fic = NULL;
    }
    untyped_item = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfItemParseResultState jfFFNFanficParse::GetLinkTitle() {
    const QString fname = "jfFFNFanficParse::GetLinkTitle";
    const QString perr = "Error parsing Fanfiction.net Fic: ";

    // next up is the story id
    if (!xparser.MovePast(storyid_start)) {
        return SetError(perr + "Start of Fic ID not found!");
    }
    ulong qval; QString oerr;
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

    // moving past the image
    if (!xparser.MovePastTwo("<img ",">")) {
        return SetError(perr + "Fic image not found!");
    }

    // next up is the title
    if (!xparser.GetMovePast("</a>",buffer)) {
        // some sort of parsing bug for a particular sega story, I cannot seem to fix, so workaround
        if ((cat_link->GetName()).contains("SEGA")) return jfItemParseResultState::DEFECTIVE;
        else return SetError(perr + "End of title field not found!");
    }
    ffn_fic->title = buffer;
    return jfItemParseResultState::SUCCEESS;
}
// -----------------------------
jfItemParseResultState jfFFNFanficParse::InnerParse() {
    const QString perr = "Error parsing Fanfiction.net Fic: ";

    assert(cat_link != NULL);
    ffn_fic = new jfFFNItem();
    untyped_item = ffn_fic;

    jfItemParseResultState link_title = GetLinkTitle();
    if (link_title != jfItemParseResultState::SUCCEESS) return link_title;

    // the author id
    ulong qval;
    QString oerr;
    if (!xparser.GetDelimitedULong("<a href=\"/u/","/",qval,oerr)) {
        /* in fanfiction.net, you can sometimes comes across unreachable stories with
        no author (because the actual author id has been deleted somehow). these should
        be discarded, they are not a sign that parsing is wrong */
        return jfItemParseResultState::DEFECTIVE;
    }
    ffn_fic->author_id = qval;

    // the author
    QString buffer;
    if (!(xparser.GetDelimited("\">","</a>", buffer))){
        return SetError(perr + "Author not found!");
    }
    ffn_fic->author_name = buffer;
    // next, the description!
    if (!ExtractDescription(oerr, ffn_fic->summary)) {
        return SetError(perr + oerr);
    }
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
    ffn_fic->author_url = ffn_fic->MakeAuthorUrl();

    return jfItemParseResultState::SUCCEESS;
}

// jfFFNItem* ffn_fic;



/*****************************************************************************/
