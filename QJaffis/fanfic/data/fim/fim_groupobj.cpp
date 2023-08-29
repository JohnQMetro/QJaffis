/******************************************************************************
Name    :   fim_groupobj.cpp
Author  :   John Q Metro
Purpose :   Group object for fimfiction.net
Created :   July 31, 2013
Started conversion to Qt August 3, 2015
Updated :   March 21, 2023
******************************************************************************/
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
//----------------------------------------------------------------------

//******************************************************************************
const QString& IdForFIMGroups() {
    static QString FIMGROUP_TYPE_ID = QString("FIMGroup");
    return FIMGROUP_TYPE_ID;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfFIMGroup::jfFIMGroup():jfSearchResultItem() {
    type_labels.append(IdForFIMGroups());
    ficcount = memcount = 0;
}
//---------------------------------
jfFIMGroup::jfFIMGroup(const jfFIMGroup& init_data):jfSearchResultItem(init_data) {
    type_labels.append(IdForFIMGroups());
    ficcount = init_data.ficcount;
    memcount = init_data.memcount;
    forumlink = init_data.forumlink;
    founder = init_data.founder;
    last_comment = init_data.last_comment;
}
// -------------------------------------
jfFIMGroup::jfFIMGroup(const jfSearchResultItem& source):jfSearchResultItem(source) {
    type_labels.append(IdForFIMGroups());
    ficcount = memcount = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data getting
//------------------------------
size_t jfFIMGroup::GetFicCount() const {
  return ficcount;
}
//------------------------------
size_t jfFIMGroup::GetMemCount() const {
  return memcount;
}
//------------------------------
QString jfFIMGroup::GetForumlink() const {
  return forumlink;
}
//------------------------------
QDate jfFIMGroup::GetDate() const {
  return last_comment;
}
//------------------------------
QString jfFIMGroup::GetFounder() const {
  return founder;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFIMGroup::LoadMoreValues(jfSkeletonParser* inparser) const {

    inparser->AddUInt("ITEMG_FICCOUNT",ficcount);
    inparser->AddUInt("ITEMG_MEMCOUNT",memcount);
    inparser->AddText("ITEMG_FORUMURL",forumlink);
    inparser->AddText("ITEMG_FOUNDER",founder);

    if (last_comment.isValid()) {
        inparser->AddText("ITEMG_PUBDATE",last_comment.toString("d MMMM yyyy"));
    }
    else {
        inparser->AddText("ITEMG_PUBDATE","?");
    }
    QString extract = htmlparse::MakeShorterText(summary, 4, 300);
    inparser->AddText("ITEMG_DESCEXT", extract);
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//------------------------------
bool jfFIMGroup::AddRestToFile(QTextStream* outfile) const {
    if (outfile == NULL) return false;
    jfOutString xresult;
    xresult << ficcount << memcount << founder;
    (*outfile) << xresult << '\n';
    xresult.FullClear();
    xresult << last_comment << forumlink;
    (*outfile) << xresult << '\n';
    // done
    return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o input

bool jfFIMGroup::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFIMGroup::ReadRestFromFile";
  assert(infile!=NULL);

  if (!infile->ReadParseLine(3,funcname)) return false;
  // getting the values
  if (!infile->lp.SIntVal(0,ficcount)) return infile->BuildError("The fic count is not valid!");
  if (!infile->lp.SIntVal(1,memcount)) return infile->BuildError("The member count is not valid!");
  founder = infile->lp.UnEscStr(2);

  if (!infile->ReadParseLine(2,funcname)) return false;
  if (!infile->lp.DateVal(0,last_comment)) {
    return infile->BuildError("Unable to convert the last comment date field properly");
  }
  forumlink = infile->lp.UnEscStr(1);
  return true;
}

// ============================================================================
// ============================================================================

jfFIMGroupOutputter::jfFIMGroupOutputter():jfResultItemOutputter() {
    fim_group_source = NULL;
}
// -----------------------------
void jfFIMGroupOutputter::SetSource(const jfFIMGroup* in_source, const jfItemMetaFlags* in_flags) {
    fim_group_source = in_source;
    source = in_source;
    flags = in_flags;
}
// ------------------------------
void jfFIMGroupOutputter::SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) {
    assert(in_source != NULL);
    assert(in_source->GetTypeLabel() == IdForFIMGroups());
    fim_group_source = dynamic_cast<const jfFIMGroup*>(in_source);
    source = in_source;
    flags = in_flags;
}
// ++++++++++++++++++++++++++++++++
QString jfFIMGroupOutputter::ToText() const {
    assert(fim_group_source != NULL);
    QString result,buffer;
    // building the title and founder line
    result += fim_group_source->GetTitle() + " founded by " + fim_group_source->GetFounder() + "\n";
    // next line: link display
    result += fim_group_source->GetURL() + "\n";
    // adding the main description
    result += fim_group_source->GetSummary() + "\n";
    // next up.. critical information
    result += "Stories : " + QString::number(fim_group_source->GetFicCount());
    result += " - Members : " + QString::number(fim_group_source->GetMemCount());
    result += " - Last Comment : " + FormatLastDate();
    result += "\n";
    // finally, the forum link
    result += fim_group_source->GetForumlink() + "\n";
    // done
    return result;
}
// -----------------------------
QString jfFIMGroupOutputter::ToDisplayHTML() const {
    const QString fname = "jfFIMGroupOutputter::ToDisplayHTML";
    assert(fim_group_source != NULL);

    QString result;
    // we start with the table
    result = "<table width=99%><tr><td>";
    // building the title line
    result += "<font size=+3 color=#010101><a href=\"";
    result += fim_group_source->GetURL() + "\">";
    result += fim_group_source->GetTitle() + "</a>";
    // adding the author stuff
    result += " founded by " + fim_group_source->GetFounder();
    result += "</font></td></tr>\n";
    // next line: link display
    result += "<tr><td>";
    result += "<font color=\"green\" size=+1>";
    result += fim_group_source->GetURL() + "</font><br>\n";
    // adding the main description
    result += "<font color=#010101 size=+2>" + GetSummaryExtract(5,500) + "<br>\n";
    // next up.. critical information
    result += "<font color=gray size=+1>Published: ";
    result += "Stories : " + QString::number(fim_group_source->GetFicCount());
    result += " - Members : " + QString::number(fim_group_source->GetMemCount());
    result += " - Last Comment : " + FormatLastDate();
    // else result += last_comment.Format(wxT("%m-%d-%Y"));
    result += "</font><br>\n";
    // the forum link
    result += "<font color=green size=+1>";
    result += fim_group_source->GetForumlink() + "</font></td></tr>\n</table>";
    // done
    return result;
}
// ++++++++++++++++++++++++++++++++
jfFIMGroupOutputter::~jfFIMGroupOutputter() {
    fim_group_source = NULL;
}
// ++++++++++++++++++++++++++++++++
QString jfFIMGroupOutputter::FormatLastDate() const {
    if (fim_group_source == NULL) return "";
    else if ((fim_group_source->GetDate()).isValid()) {
        return (fim_group_source->GetDate()).toString("d MMMM yyyy");
    }
    else return "?";
}

// ============================================================================
// ===========================================================================

/** Generating a jfFIMGroup object from raw html source. */
//++++++++++++++++++++++++++++++++++
jfFIMGroupItemParser::jfFIMGroupItemParser():jfItemParseBase(false) {
    item_result = NULL;
    num_id = 0;
}
//++++++++++++++++++++++++++++++++++
jfItemParseResultState jfFIMGroupItemParser::ParseFromCardSource(const QString& source) {
    ClearCore(false);
    item_result = NULL;
    xparser.ChangeData(source, case_sensetive);
    result_state = InnerCardParse();
    has_result = true;
    return result_state;
}
// -----------------------------------------------
void jfFIMGroupItemParser::Clear() {
    ClearCore(true);
    item_result = NULL;
    num_id = 0;
    url.clear();
    forumurl.clear();
}
// ----------------------------------
const jfSearchResultItem* jfFIMGroupItemParser::GetUntypedItem() {
    item_result = NULL;
    return jfItemParseBase::GetUntypedItem();
}
// ----------------------------------
jfFIMGroup* jfFIMGroupItemParser::GetGroupItem() {
    jfFIMGroup* item_data = item_result;
    item_result = NULL;
    untyped_item = NULL;
    return item_data;
}
//++++++++++++++++++++++++++++++++++
jfFIMGroupItemParser::~jfFIMGroupItemParser() {
    item_result = NULL;
}
//++++++++++++++++++++++++++++++++++
void jfFIMGroupItemParser::ClearItem() {
    if (untyped_item != NULL) {
        delete untyped_item;
        untyped_item = NULL;
    }
    item_result = NULL;
}
// ----------------------------------
jfItemParseResultState jfFIMGroupItemParser::InnerParse() {
    const QString fname = "jfFIMGroupItemParser::InnerParse";
    item_result = NULL;
    QString parse_err = "FIM Group parsing error ";

    /**/JDEBUGLOG(fname,1)
    // here. we assume we have skipped past the <!--Content--> tag
    if (!ParseLinkExtract(parse_err)) {
        return SetError(parse_err);
    }
    // looking for the name
    if (!xparser.MovePast("<a class=\"group_name\"")) {
        return SetError(parse_err + "Cannot find group name");
    }
    QString buffer;
    if (!xparser.GetDelimited("style=\"\">","</a>",buffer)) {
        return SetError(parse_err + "Cannot extract group name");
    }
    QString group_name = buffer.trimmed();
    if (group_name.isEmpty()) group_name = "{EMPTY NAME}";

    // getting the founder
    if (!xparser.MovePast("Founded by")) {
        return SetError(parse_err + "Cannot find founder name");
    }
    QString founder;
    if (!xparser.GetDelimited("\"><b>","</b></a>",founder)) {
        return SetError(parse_err + "Cannot extract founder name");
    }
    /**/JDEBUGLOGS(fname,2,founder)

    // getting the story count
    if (!xparser.MovePast("<ul class=\"buttons\">")) {
        return SetError(parse_err + "Cannot find start of special buttons!");
    }
    /**/JDEBUGLOG(fname,3)
    ulong tval;
    if (!xparser.GetDelimitedULong(" class=\"button\">","<br />",tval,parse_err)) {
        return SetError(parse_err + " While getting the fic count.");
    }
    size_t ficcount = tval;
    /**/JDEBUGLOGU(fname,4,tval)

    // getting the number of members
    if (!xparser.GetDelimitedULong(" class=\"button\">","<br />",tval,parse_err)) {
        return SetError(parse_err + " While getting the member count.");
    }
    size_t memcount = tval;
    /**/JDEBUGLOGST(fname,3,memcount)

    // the description
    // the description now comes in multiple blocks...
    QString rawhtml_desc = "";
    while (xparser.MovePast("<article class=\"article group_description bbcode\">")) {
        if (!xparser.GetMovePast("</article>",buffer)) {
            return SetError(parse_err + "Malformed group description block!");
        }
        rawhtml_desc += buffer.trimmed() + "\n";
    }
    // handling description
    QString description = htmlparse::Text2BasicHTML(htmlparse::HTML2Text(rawhtml_desc));
    description.replace("!","! ");
    description.replace("?","? ");

    // getting the date of the last comment
    QDate last_comment;
    if (xparser.MovePast("<div class=\"comment_list\"")) {
        if (xparser.GetDelimited("data-time=\"","\"",buffer)) {
            if (!ParseDateSet(buffer, last_comment)) {
                return SetError(parse_err + "Could not parse last comment date!");
            }
        }
        else last_comment = QDate();
    }
    else last_comment = QDate();
    /**/JDEBUGLOG(fname,5)

    // we create the final result
    item_result = new jfFIMGroup();
    item_result->numeric_id = num_id;
    item_result->title = group_name;
    item_result->summary = description;
    item_result->url = url;

    item_result->ficcount = ficcount;
    item_result->memcount = memcount;
    item_result->forumlink = forumurl;
    item_result->founder = founder;
    item_result->last_comment = last_comment;
    untyped_item = item_result;

    // done
    return jfItemParseResultState::SUCCEESS;
}
// ----------------------------
jfItemParseResultState jfFIMGroupItemParser::InnerCardParse() {
    const QString fname = "jfFIMGroupItemParser::InnerCardParse";
    QString parse_err = "FIM Card Group parsing error: ";

    // link data
    if (!ParseLinkExtract(parse_err)) return SetError(parse_err);

    // group name
    QString group_name;
    if (!xparser.GetDelimited("<div class=\"group-name\">","</div>",group_name)) {
        return SetError(parse_err + "Unable to extract group name!");
    }
    group_name = group_name.trimmed();
    if (group_name.isEmpty()) group_name = "{EMPTY NAME}";

    // name of the founder
    QString founder;
    if (!xparser.GetDelimited("<div class=\"group-info\">","<b>",founder)) {
        return SetError(parse_err + "Could not extract founder name!");
    }
    founder = founder.trimmed();

    // creation date...
    QString raw_creation_date;
    if (!xparser.GetDelimited("data-time=\"","\"",raw_creation_date)) {
        return SetError(parse_err + "Could not extract creation date!");
    }
    // member count
    ulong tval;
    if (!xparser.GetDelimitedULong("<b>","</b> members",tval,parse_err)) {
        return SetError(parse_err + "Could not extract member count!");
    }
    size_t memcount = tval;

    // moving
    if (!xparser.MovePast("<div class=\"bottom-links\"")) {
        return SetError(parse_err + "Could not find count divider!");
    }

    // story count
    if (!xparser.GetDelimitedULong("<span class=\"number\">","</span>",tval,parse_err)) {
        return SetError(parse_err + " : Could not extract story count!");
    }
    size_t ficcount = tval;

    // the cards do not includes description or date of last comment, so this is it...
    // we create the final result
    item_result = new jfFIMGroup();
    item_result->numeric_id = num_id;
    item_result->title = group_name;
    item_result->url = url;

    item_result->ficcount = ficcount;
    item_result->memcount = memcount;
    item_result->forumlink = forumurl;
    item_result->founder = founder;
    item_result->last_comment = QDate();
    untyped_item = item_result;

    // done
    return jfItemParseResultState::SUCCEESS;
}
// ----------------------------------------------
// parsing the custom date time
bool jfFIMGroupItemParser::ParseDateSet(const QString& invalue, QDate& result) const {
    // the input should be a string UNIX timestamp
    ulong tval;
    result = QDate(); // invalid dummy result
    if (!Str2ULong(invalue,tval)) return false;
    QDateTime temp_datetime;
    temp_datetime.setTime_t((uint)tval);
    if (!temp_datetime.isValid()) return false;
    // here, the QDateTime is valid...
    result = temp_datetime.date();
    return true;
}
//---------------------------------------------------------
bool jfFIMGroupItemParser::ParseLinkExtract(QString& perr_out) {
    // getting link data
    ulong tval;
    if (!xparser.GetDelimitedULong("<a href=\"/group/","/",tval, perr_out)) {
      perr_out += " While getting the group link";
      return false;
    }
    num_id = tval;
    QString buffer;
    if (!xparser.GetMovePast("\"",buffer)) {
      perr_out = "Cannot find end of group link";
      return false;
    }
    // creating the link
    url = "https://www.fimfiction.net/group/" + QString::number(num_id);
    url += "/" + buffer;
    forumurl = url + "/forum";
    // done
    return true;
}

//*****************************************************************************

// alternate constructor for use when loading data from file
jfFIMGroupCollection::jfFIMGroupCollection(QString&& in_name, size_t in_num_id):jfSearchResultsCollection<jfFIMGroup>(in_name, in_num_id) {

}
// ----------------------------
jfFIMGroupCollection::jfFIMGroupCollection(const QString& in_name, size_t in_num_id):jfSearchResultsCollection<jfFIMGroup>(in_name, in_num_id) {

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented page methods
//-----------------------------------------------------
QString jfFIMGroupCollection::TypeId() const {
  return "FIMGroupCollection";
}
//-----------------------------------------------------
bool jfFIMGroupCollection::LoadValues(jfSkeletonParser* inparser,size_t which) const {
  LoadCoreValues(inparser,which);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more virtual i/o methods
//-----------------------------------------------------
bool jfFIMGroupCollection::ReadItemFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const {
    jfFIMGroup* result = new jfFIMGroup();
    if (result->GetFromFile(infile)) {
        target_group.item = result;
        return true;
    }
    else {
        delete result;
        return false;
    }
}
// ---------------------------------------------------
bool jfFIMGroupCollection::AddDelta(QTextStream* outfile) const {
  return true;
}
//-----------------------------------------------------
bool jfFIMGroupCollection::ReadDelta(jfFileReader* infile) {
  return true;
}
//*****************************************************************************
