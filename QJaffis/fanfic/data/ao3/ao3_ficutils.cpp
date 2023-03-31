/******************************************************************************
Name    :   ao3_ficutils.cpp
Author  :   John Q Metro
Purpose :   Parsers and Outputters for jfAO3Fanfic
Created :   March 18, 2023
Updated :   March 30, 2023
******************************************************************************/
#include "ao3_ficutils.h"

#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED

/*****************************************************************************/
jfAO3FanficOutputter::jfAO3FanficOutputter():jfFanficItemOutputterBase(HTMLHelpers::ao3_item_helper) {
    ao3_fanfic = NULL;
}
//-----------------------------------------------------
jfAO3FanficOutputter::jfAO3FanficOutputter(const jfDisplayHTMLHelper* in_html_helper):jfFanficItemOutputterBase(in_html_helper) {
    ao3_fanfic = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
void jfAO3FanficOutputter::SetFanfic(const jfAO3Fanfic* in_fim_fanfic) {
    ao3_fanfic = in_fim_fanfic;
    fanfic_source = in_fim_fanfic;
    source = in_fim_fanfic;
}
//-------------------------------------
void jfAO3FanficOutputter::SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) {
    assert(in_source != NULL);
    assert(in_source->GetTypeLabel() == jfAO3Fanfic::AO3_FANFIC_TYPE_ID);
    ao3_fanfic = dynamic_cast<const jfAO3Fanfic*>(in_source);
    fanfic_source = ao3_fanfic;
    source = in_source;
    flags = in_flags;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
QString jfAO3FanficOutputter::ToText() const {
    QString result,buffer;
    // building the title and author line
    result += ao3_fanfic->GetTitle() + " by " + ao3_fanfic->GetAuthor() + "\n";
    // next line: link display
    result += ao3_fanfic->GetURL() + "\n";
    // adding the main description
    result += ao3_fanfic->GetSummary() + "\n";
    if (ao3_fanfic->InSeries()) {
      result += "Part " + QString::number(ao3_fanfic->SeriesIndex()) + " of the " + ao3_fanfic->SeriesName();
      result += " Series.\n";
    }
    // next up.. two extra lines
    result += "Updated: " + (ao3_fanfic->GetUpdates()).toString("MM-dd-yyyy");

    // the rating... and orientation
    result += " - Rating: " + ao3_fanfic->RatingToString();
    result += " - Orientation";
    result += ((ao3_fanfic->MultipleOrientations()))?"s: ":": ";
    result += ao3_fanfic->GetOrientations();

    // part count and word count
    result += " - Parts: " + QString::number(ao3_fanfic->GetChapterCount());
    result += " - Words: " + QString::number(ao3_fanfic->GetWordcount());
    size_t kudcount = ao3_fanfic->GetKudos();
    if (kudcount!=0) result += " - Kudos: " + QString::number(kudcount);
    result+= "\n";
    // final info,
    result += "Warnings: " + ao3_fanfic->WarnToString();
    if (!(ao3_fanfic->GetCharacterList()).isEmpty()) {
      result += " - Characters: " + (ao3_fanfic->GetCharacterList()).join(", ") + "\n";
    }
    // character data
    if (ao3_fanfic->RelationshipCount() > 0) {
      result += "Pairings: " + ao3_fanfic->RelationshipsAsDisplayString(false);
    }
    // tags
    if (!(ao3_fanfic->GetExtraTags()).isEmpty()) {
      result += " - Tags: " + ao3_fanfic->GetJoinedExtraTags();
    }
    if (ao3_fanfic->IsComplete()) {
      result += " - Complete";
    }
    // finishing off
    result += "\n";
    // done
    return result;
}
//-----------------------------------------------------
QString jfAO3FanficOutputter::ToDisplayHTML() const {
    const QString fname = "jfAO3FanficOutputter::ToDisplayHTML";
    QString result,buffer,optline;

    // we start with the table
    result = DisplayHTMLHeader(html_helper);
    // fandom display
    result += "<br>\n" + html_helper->WrapText("fandoms", ao3_fanfic->GetFandoms());
    result += "</td></tr>\n";
    // next line: link display
    result += "<tr><td>";
    result += html_helper->WrapText("ficlink", ao3_fanfic->GetURL(), false);

    // adding the main description
    result += "<div style=\"margin-left:10px;\">";
    QString rdesc = GetSummaryExtract(6, 1000);
    result += html_helper->WrapTextNoEsc("description",rdesc,false);
    result += "</div>";

    // series
    if (ao3_fanfic->InSeries()) {
        QString series = "Part " + QString::number(ao3_fanfic->SeriesIndex()) + " of the " + ao3_fanfic->SeriesName() + " Series";
        result += html_helper->WrapText("series", series, true);
    }
    // standard metadata
    QString mdata = "Updated: " + (ao3_fanfic->GetUpdates()).toString("MM-dd-yyyy");
    mdata += " - Rating: " + ao3_fanfic->RatingToString();
    mdata += " - Orientation";
    mdata += ((ao3_fanfic->MultipleOrientations())?"s: ":": ") + ao3_fanfic->GetOrientations();
    // part count and word count
    mdata += " - Parts: " + QString::number(ao3_fanfic->GetChapterCount());
    mdata += " - Words: " + QString::number(ao3_fanfic->GetWordcount());
    // kudos
    mdata += " - Kudos: " + QString::number(ao3_fanfic->GetKudos());
    if (ao3_fanfic->IsComplete()) {
      mdata += " - Complete";
    }
    result += html_helper->WrapText("basicinfo", mdata, true);

    // final info
    optline = html_helper->ConditionalWrapText("warnings",false,"Warnings: ", true, ao3_fanfic->WarnToString());
    optline += html_helper->ConditionalWrapText("characters", !optline.isEmpty(), "Characters: ", true, (ao3_fanfic->GetCharacterList()).join(", "));
    optline += html_helper->ConditionalWrapText("pairings", !optline.isEmpty(), "Pairings: ", true, ao3_fanfic->RelationshipsAsDisplayString(false));
    if (!optline.isEmpty()) result += optline + "<br>\n";

    // extra tags
    result += html_helper->ConditionalWrapText("tags", false, "", false, ao3_fanfic->GetJoinedExtraTags());

    // finishing off
    result += "</td></tr>\n</table>";
    // done
    return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
jfAO3FanficOutputter::~jfAO3FanficOutputter() {
    ao3_fanfic = NULL;
}

// ===========================================================================================
// ===========================================================================================
jfAO3FanficItemParser::jfAO3FanficItemParser():jfItemParseBase(true) {
    result = NULL;
    current_category = NULL;
    english_locale = QLocale(QLocale::English, QLocale::UnitedStates);
    QRegularExpression breaks = QRegularExpression(
                "<br/?>[\\s\\-*]*<br/?>([\\s\\-*]*<br/?>)*",
                QRegularExpression::CaseInsensitiveOption);

}
// ++++++++++++++++++++++++++++++++++++++++++
// clearing or getting results
void jfAO3FanficItemParser::Clear() {
    ClearCore(true);
    result = NULL;
}
// --------------------------------
const jfSearchResultItem* jfAO3FanficItemParser::GetUntypedItem() {
    result = NULL;
    return jfItemParseBase::GetUntypedItem();
}
// --------------------------------
const jfAO3Fanfic* jfAO3FanficItemParser::GetFanfic() {
    jfAO3Fanfic* result_bak = result;
    result = NULL;
    untyped_item = NULL;
    return result_bak;
}
// ++++++++++++++++++++++++++++++++++++++++++
// other
//----------------------------------
void jfAO3FanficItemParser::SetCategory(const jfAO3_Category* this_category) {
    current_category = this_category;
}
//----------------------------------
void jfAO3FanficItemParser::ClearCategory() {
    current_category = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++++
// destructor
jfAO3FanficItemParser::~jfAO3FanficItemParser() {
    result = NULL;
    current_category = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++++
void jfAO3FanficItemParser::ClearItem() {
    if (result != NULL) {
        delete result;
        result = NULL;
    }
    untyped_item = NULL;
}
// ++++++++++++++++++++++++++++++++++++++++++
// parsing methods
// ----------------------------------
jfItemParseResultState jfAO3FanficItemParser::InnerParse() {
    const QString fxname = "jfAO3FanficItemParser::InnerParse";
    result = NULL;
    // calling
    jfItemParseResultState inner_result = ParseStart();
    if (inner_result != jfItemParseResultState::SUCCEESS) return inner_result;

    inner_result = ParseMiddle();
    if (inner_result != jfItemParseResultState::SUCCEESS) return inner_result;
    inner_result = ParseTags();
    if (inner_result != jfItemParseResultState::SUCCEESS) return inner_result;
    inner_result = ParseEnd();
    if (inner_result != jfItemParseResultState::SUCCEESS) return inner_result;
    else return jfItemParseResultState::SUCCEESS;
}

// internal parsing methods
//-------------------------------------------
// handles fic name, id, author name and id, and category info
jfItemParseResultState jfAO3FanficItemParser::ParseStart() {
  // constants
  const QString perr = "Error in parsing AO3 Start : ";

  QString errpass, buffer;
  unsigned long tval;

  // the fic id first
  if (!xparser.GetDelimitedULong("<a href=\"/works/","\">",tval,errpass)) {
    return SetError(perr + "Error in getting fic id : " + errpass);
  }
  size_t num_id = tval;
  QString primarylink = "https://archiveofourown.org/works/" + QString::number(num_id);
  primarylink += "?view_adult=true";

  // title
  if (!xparser.GetMovePast("</a>", buffer)) return SetError(perr + "Could not extract title!");
  QString title = htmlparse::ConvertEntities(buffer.trimmed(), false);

  // author
  size_t aloc, bloc;
  // can be rather complicated because we can have anonymous stories with no link
  if (!xparser.Find1stPair("<a rel=\"author\"","Anonymous",aloc,bloc)) {
    return SetError(perr + "Could not get author!");
  }
  QString author_url, author_name;
  if (aloc==1) {
    // author link
    if (!xparser.MovePast("<a rel=\"author\"")) {
      return SetError(perr + "Error in getting start of author link!");
    }
    if (!xparser.GetDelimited("href=\"","\"",buffer)) {
      return SetError(perr + "Error in getting author link!");
    }
    author_url = "https://archiveofourown.org" + buffer;
    // author name
    if (!xparser.GetDelimited(">","</a>",author_name)) {
      return SetError(perr + "Error in getting author name!");
    }
    author_name = htmlparse::ConvertEntities(author_name, false);
  }
  else {
    author_name = "Anonymous";
  }

  // starting the result
  result = new jfAO3Fanfic();
  untyped_item = result;
  result->numeric_id = num_id;
  result->url = primarylink;
  result->title = title;
  result->author_name = author_name;
  result->author_url = author_url;

  // fandoms
  if (!xparser.GetDelimited("<h5 class=\"fandoms","</h5>",buffer)) {
    return SetError(perr + "Cannot find fandoms!");
  }
  bool cats_okay = ParseFandoms(std::move(buffer), result->cats);
  // things are fine
  if (cats_okay) {
      return jfItemParseResultState::SUCCEESS;
  }
  // no categories found. If we have no backup, that is not a parsing error
  else if (current_category == NULL) {
    return jfItemParseResultState::DEFECTIVE;
  }
  // but if there is backup, thing is an error
  else {
      return SetError(perr + "No fandoms!");
  }

}
// ------------------------------------------
bool jfAO3FanficItemParser::ParseFandoms(QString&& fandom_source, std::vector<const jfAO3_Category*>& target) const {
    jfStringParser* tparser = new jfStringParser(fandom_source);
    QString curl;
    const jfGeneralCategory* temp = NULL;

    bool has_current = (current_category != NULL);
    if (has_current) target.push_back(current_category);

    while (tparser->MovePast("<a class=\"tag\"")) {
        if (!tparser->GetDelimited("href=\"","\"",curl)) break;
        curl = "https://archiveofourown.org" + curl;
        temp = ao3_catdata::ao3_catmanager->GetData()->FindByUrl(curl);
        /* note that I've decided to ignore unfound fandoms after my first test
         * revealed multiple names/urls for the same fandom. Too difficult to track
         * all of them. */
        if (temp != NULL) {
            // we do not add the current category twice
            if (has_current) {
                if (curl == (current_category->GetUrl())) continue;
            }
            target.push_back(dynamic_cast<const jfAO3_Category*>(temp));
        }
    }
    delete tparser;
    return (target.size() > 0);
}
//-------------------------------------------
// special flags and date
jfItemParseResultState jfAO3FanficItemParser::ParseMiddle() {
    // constants
    const QString perr = "Error in parsing AO3 Middle : ";

    // variables
    QString buffer;
    QString whatwarn = "";
    // starting...
    // rating
    if (!xparser.GetDelimited("<span class=\"rating-","\"",buffer)) {
        return SetError(perr + "Cannot find rating!");
    }
    QChar rating;
    if (buffer=="teen rating") rating = 'T';
    else if (buffer=="explicit rating") rating = 'E';
    else if (buffer=="general-audience rating") rating = 'G';
    else if (buffer=="mature rating") rating = 'M';
    else if (buffer=="notrated rating") rating = '_';
    else return SetError(perr + "Rating unrecognized!");
    result->rating = rating;

    // warning flag
    if (!xparser.GetDelimited("<span class=\"warning-","\"",buffer)) {
        return SetError(perr + "Cannot find warning flag!");
    }
    QChar warn;
    if (buffer=="yes warnings") warn = 'W';
    else if (buffer=="no warnings") warn = '_';
    else if (buffer=="choosenotto warnings") warn = 'N';
    else if (buffer=="mature rating") warn = 'E';
    else return SetError(perr + "Warning Flags unrecognized!");
    // we try to get the actual warning
    if (warn == 'W') {
        if (xparser.GetDelimited("title=\"","\">",buffer)) {
            whatwarn = buffer.trimmed().replace("\\'","'");;
        }
    }

    // orientation
    if (!xparser.MovePastTwo("<span class=\"category-","\"")) {
        return SetError(perr + "Cannot start of find orientation!");
    }
    if (!xparser.GetDelimited("title=\"","\"",buffer)) {
        return SetError(perr + "Cannot find orientation!");
    }
    if (buffer == "No category") result->orientations = "";
    else {
        result->orientations = buffer.trimmed();
    }

    // completed flag
    if (!xparser.GetDelimited("<span class=\"complete-","\"",buffer)) {
        return SetError(perr + "Cannot find completion status!");
    }
    result->completed = (buffer=="yes iswip");
    // getting the date
    if (!xparser.GetDelimited("<p class=\"datetime\">","</p>",buffer)) {
        return SetError(perr + "Cannot find date!");
    }
    // converting the date
    buffer = buffer.trimmed();
    QDate updated_date = english_locale.toDate(buffer,"d MMM yyyy");
    // updated_date = QDate::fromString(buffer,"d MMM yyyy");
    if (!updated_date.isValid()) {
        return SetError(perr + "Date Conversion failed! " + buffer);
    }
    result->updated_date = updated_date;
    return jfItemParseResultState::SUCCEESS;
}
//-------------------------------------------
jfItemParseResultState jfAO3FanficItemParser::ParseTags() {
    // constants
    const QString perr = "Error in parsing AO3 Tags : ";
    // variables
    QString tag_class,tag_content;
    QStringList extrawarn;
    QStringList characters;
    QStringList extratags;
    QString warntags;
    // the tag loop
    while(xparser.GetDelimited("<li class='","'>",tag_class)) {
        if (!xparser.MovePast("<a class=\"tag\"")) {
            return SetError(perr + "Cannot find middle part of tag!");
        }
        if (!xparser.GetDelimited("\">","</a>",tag_content)) {
            return SetError(perr + "Cannot find final part of tag!");
        }
        tag_content = htmlparse::ConvertEntities(tag_content, false);
        // here, we've got the tag strings, just sort them!
        if (tag_class=="warnings") {
            if (tag_content=="Underage") warntags += 'P';
            else if (tag_content=="Graphic Depictions Of Violence") warntags += 'V';
            else if (tag_content=="Major Character Death") warntags += 'D';
            else if (tag_content=="Rape/Non-Con") warntags += 'R';
            else if (tag_content=="Creator Chose Not To Use Archive Warnings") ;
            else if (tag_content=="No Archive Warnings Apply") ;
            /* annoyingly, AO3 has just come up with a bunch of additional warnings which
             * seem to be not standardized or filterable. so, I'll stick them in extratags */
            else extrawarn.append(tag_content.trimmed());
        }
        else if (tag_class=="characters") {
            characters.append(tag_content);
        }
        else if (tag_class=="relationships") {
            if (! result->ParseAndAddPair(tag_content, true)) {
                // TODO: log
            }
        }
        else if (tag_class=="freeforms") {
            extratags.append(tag_content.trimmed());
        }
        else return SetError(perr + "Unrecognized Tag Type! (" + tag_class + ")");
    }
    if (!extrawarn.isEmpty()) {
        extratags.append(extrawarn);
    }
    // adding
    result->warntags = warntags;
    result->characters = characters;
    result->extratags = extratags;
    // done
    return jfItemParseResultState::SUCCEESS;
}
//-------------------------------------------
jfItemParseResultState jfAO3FanficItemParser::ParseEnd() {
    // constants
    const QString perr = "Error in parsing AO3 End : ";
    const QString sum_string = "<blockquote class=\"userstuff summary\">";

    // getting the summary
    QString summary;
    if (!xparser.GetDelimited(sum_string,"</blockquote>",summary)) {
        // sometimes there is no summary
        summary = "";
    }
    result->summary = ProcessSummary(std::move(summary));


    unsigned long tval;
    QString errout, buffer;

    // series (optional)
    if (xparser.MovePast("<h6 class=\"landmark heading\">Series</h6>")) {
        if (!xparser.GetDelimitedULong("Part <strong>","</strong>",tval,errout)) {
            return SetError(perr + "Series index is wrong! " + errout);
        }
        result->series_index = tval;
        if (!xparser.GetDelimited("<a href=\"","\">",buffer)) {
            return SetError(perr + "Problems getting series url!");
        }
        result->series_url = "https://archiveofourown.org" + buffer;
        if (!xparser.GetMovePast("</a>",buffer)) {
            return SetError(perr + "Problems getting series name!");
        }
        result->series_name = htmlparse::ConvertEntities(buffer, false);
    }
    else result->series_index = 0;

    // words
    if (!xparser.MovePast("<dt class=\"words\">Words:</dt>")) {
        return SetError(perr + "Cannot find wordcount!");
    }
    if (!xparser.GetDelimitedULong("<dd class=\"words\">","</dd>",tval,errout)) {
        // following the old code, this apparently happens, so it is not always a parsing error
        return jfItemParseResultState::DEFECTIVE;
    }
    result->word_count = tval;

    // part count
    if (!xparser.MovePast("<dt class=\"chapters\">Chapters:</dt>")) {
        return SetError(perr + "Cannot find part count!");
    }
    if (!xparser.MovePast("<dd class=\"chapters\">")) {
        return SetError(perr + "Cannot find start of chapter counts!");
    }
    // try the 'not embedded in a link' first
    if (!xparser.GetMovePastULong("/",tval,errout)) {
        // if that fails, assume we are embedded in a link
        if (!xparser.GetDelimitedULong("\">","</a>",tval,errout)) {
            return SetError(perr + "Cannot extract part count! " + errout);
        }
        xparser.MovePast("/");
    }
    result->part_count = tval;

    // estimated part count
    int eccount;
    if (!xparser.GetMovePast("</dd>",buffer)) {
        return SetError(perr + "Cannot find estimated part count!");
    }
    if (buffer=="?") eccount = -1;
    else if (Str2ULong(buffer,tval,false)) eccount = tval;
    else return SetError(perr + "Estimated part count is not a number!");
    result->eccount = eccount;

    // kudos
    if (xparser.MovePast("<dt class=\"kudos\">Kudos:</dt>")) {
        if (!xparser.MovePast("<dd class=\"kudos\">")) return SetError(perr + "Missing Kudos!");
        if (!xparser.GetDelimitedULong("\">","</a>",tval,errout)) {
            return SetError(perr + "Kudos is not a number!");
        }
        result->kudcount = tval;
    }
    else result->kudcount = 0;
    // done
    return jfItemParseResultState::SUCCEESS;
}
// ---------------------------------------------------------------------
QString jfAO3FanficItemParser::ProcessSummary(QString&& description) const {
  if (!description.isEmpty()) {
    description = description.trimmed();
    description = description.mid(3,description.length()-7);
    // post processing
    description.replace("</p><p><hr></p>","<br>\n",Qt::CaseInsensitive);
    description.replace("<hr />","<br>\n",Qt::CaseInsensitive);
    description.replace("</p><p>","<br>\n",Qt::CaseInsensitive);
    description.replace("</p>","<br>\n",Qt::CaseInsensitive);
    description.replace("<p>"," ",Qt::CaseInsensitive);
    // description.replace(breaks,"<br>\n");

    description = description.simplified();
  }
  return description;
}
/*****************************************************************************/
