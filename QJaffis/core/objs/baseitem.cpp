/*****************************************************************************
Name    : baseitem.cpp
Basic   : Base class for items that can be searched, like Fanfic Info Items
Author  : John Q Metro
Started : March 7, 2023
Updated : March 26, 2023
Notes   : Replaces the earlier jfBasePD
******************************************************************************/
#include "baseitem.h"

#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
/*****************************************************************************/

const QString jfSearchResultItem::BASE_TYPE_ID = QString("Core");
// ++++++++++++++++++++++++++++++++++++++
// constructors
jfSearchResultItem::jfSearchResultItem() {
    type_labels.append(BASE_TYPE_ID);
    numeric_id = 0;
}
// --------------------------------------
jfSearchResultItem::jfSearchResultItem(const jfSearchResultItemData& init_data) {
    type_labels.append(BASE_TYPE_ID);
    numeric_id = init_data.numeric_id;
    title = init_data.title;
    summary = init_data.summary;
    url = init_data.url;
}
// --------------------------------------
jfSearchResultItem::jfSearchResultItem(const jfSearchResultItem& source) {
    type_labels.append(BASE_TYPE_ID);
    numeric_id = source.numeric_id;
    title = source.title;
    summary = source.summary;
    url = source.url;
}
//+++++++++++++++++++++++++++++++++++++++
// data get and set
size_t jfSearchResultItem::GetId() const {
    return numeric_id;
}
const QString& jfSearchResultItem::GetTitle() const {
    return title;
}
const QString& jfSearchResultItem::GetSummary() const {
    return summary;
}
const QString& jfSearchResultItem::GetURL() const {
    return url;
}
const QStringList& jfSearchResultItem::GetTypeLabels() const {
    return type_labels;
}
// --------------------------------
// the last of the type labels the the most specific
const QString& jfSearchResultItem::GetTypeLabel() const {
    int tlc = type_labels.size();
    return type_labels.at(tlc-1);
}
// +++++++++++++++++++++++++++++++++++++++
// tokenized parts (for tokenized expression filter matching)
// --------------------------------
const QStringList* jfSearchResultItem::GetTokTitle() const {
    return TokenizeString(title,true);
}
// --------------------------------
const QStringList* jfSearchResultItem::GetTokSummary() const {
    return TokenizeString(summary,true);
}
// +++++++++++++++++++++++++++++++++++++++++++++++
/* Outputting to the typical format used by this program, with backwards compatability. */
bool jfSearchResultItem::AddToFile(QTextStream* outfile) const {
    const QString fname = "jfSearchResultItem::AddToFile";
    if (outfile==NULL) return false;

    jfOutString s_out;
    // line 1
    s_out << GetTypeLabel() << numeric_id << true;
    (*outfile) << (QString("###") + s_out) << '\n';
    s_out.FullClear();
    // line 2
    s_out << title << summary;
    (*outfile) << s_out << '\n';
    s_out.FullClear();
    // line 3
    (*outfile) << url << "\n";

    bool rbuf = AddRestToFile(outfile);
    if (!rbuf) return false;
    // adding on the footer
    (*outfile) << "###\n";
    return true;
}
// --------------------------------------------
bool jfSearchResultItem::GetFromFile(jfFileReader* infile) {
    const QString funcname="jfSearchResultItem::GetFromFile";
    // input data
    QString buffer;

    // starting checks
    assert(infile!=NULL);
    // Reading and processing line 1
    if (!infile->ReadParseSLine(3,funcname)) return false;
    buffer = infile->lp.UnEscStr(0);
    if (buffer!=GetTypeLabel()) {
      return infile->BuildError("The type (" + buffer + ") does not match!");
    }
    // field two is a number id
    if (!infile->lp.SIntVal(1,numeric_id)) {
      return infile->BuildError("The id is not an integer!");
    }
    // field number 3 is ignored

    // we get the next line
    if (!infile->ReadParseLine(2,funcname)) return false;
    // the title
    title = infile->lp.UnEscStr(0);
    if (title.isEmpty()) infile->BuildError("The title is blank!");
    // summary
    summary = infile->lp.UnEscStr(1);

    // the next line is the url
    if (!infile->ReadLine(buffer, funcname)) return false;
    url = buffer;

    // we do the virtual methods
    if (!ReadRestFromFile(infile)) return false;
    // we should be left just before a final line...
    if (!infile->ReadLast(funcname)) return false;
    return true;
    // done
}
// +++++++++++++++++++++++++++++++++++++++++++
// other
bool jfSearchResultItem::LoadValues(jfSkeletonParser* inparser) const {
    // runs some checks first
    assert(inparser!=NULL);
    // adding the values
    inparser->AddText("ITEM_NAME",title);
    inparser->AddText("ITEM_DESCRIPTION",summary);
    inparser->AddText("ITEM_URL",url);
    inparser->AddUInt("ITEM_ID",numeric_id);

    return LoadMoreValues(inparser);
}
// +++++++++++++++++++++++++++++++++++++++++++
jfSearchResultItem::~jfSearchResultItem() {}
// +++++++++++++++++++++++++++++++++++++++++++
void jfSearchResultItem::StoreToCopyBase(jfSearchResultItem* destination) const {
    destination->numeric_id = numeric_id;
    destination->title = title;
    destination->summary = summary;
    destination->url = url;
}
// =====================================================================
QString UpdateStatusToString(const jfUpdateStatus& inval) {
    QString result;
    if (inval==jfUpdateStatus::NONE) return "";
    else if (inval==jfUpdateStatus::NEW) return "new";
    else if (inval==jfUpdateStatus::UPDATED) result = "updated";
    else if (inval==jfUpdateStatus::MISSING) result = "missing";
    else if (inval==jfUpdateStatus::AUTHORNC) result = "authornc";
    else assert(false);
    // done
    return result;
}
// -----------------------------------------------
jfUpdateStatus StringToUpdateStatus(const QString& inval) {
    if (inval == "new") return jfUpdateStatus::NEW;
    else if (inval == "updated") return jfUpdateStatus::UPDATED;
    else if (inval == "missing") return jfUpdateStatus::MISSING;
    else if (inval == "authornc") return jfUpdateStatus::AUTHORNC;
    else return jfUpdateStatus::NONE;
}
// ======================================================================
// constructor for the meta flags object
jfItemMetaFlags::jfItemMetaFlags() {
    included = true;
    ignore = false;
    ustatus = jfUpdateStatus::NONE;
};
// --------------------------------------
jfItemMetaFlags::jfItemMetaFlags(const jfItemMetaFlags& other) {
    category = other.category;
    ustatus = other.ustatus;
    included = other.included;
    ignore = other.ignore;
    background_color = other.background_color;
}
// ==================================================================
void jfItemFlagGroup::Dispose() {
    if (item != NULL) {
        delete item;
        item = NULL;
    }
    if (flags != NULL) {
        delete flags;
        flags = NULL;
    }
}
// --------------------------------------
bool jfItemFlagGroup::isNull() const {
    return (item == NULL) && (flags == NULL);
}
// ======================================================================
/* A front end to jfSearchResultItem used in displaying Results. */
//+++++++++++++++++++++++++++++++++++++++++++
// constructors
// ---------------------
jfResultUnit::jfResultUnit(const jfSearchResultItem* source, jfItemMetaFlags* in_flags,QString&& in_html) {
    assert(source != NULL);
    htmldata = in_html;
    slink = source;
    flags = in_flags;
}
// ---------------------
jfResultUnit::jfResultUnit(const QString in_html, const jfSearchResultItem* source, jfItemMetaFlags* in_flags) {
    assert(source != NULL);
    htmldata = in_html;
    slink = source;
    flags = in_flags;
}
//+++++++++++++++++++++++++++++++++++++++++++
// getting the string data
// ---------------------
const QString& jfResultUnit::GetUrl() const {
    return slink->GetURL();
}
// ---------------------
const QString& jfResultUnit::GetHTML() const {
    return htmldata;
}
// ---------------------
const QString& jfResultUnit::GetTitle() const {
    return slink->GetTitle();
}
// ---------------------
size_t jfResultUnit::GetId() const {
    return slink->GetId();
}
// ++++++++++++++++++++++++++++++++++++
// marking data
// ----------------------
bool jfResultUnit::IsIncluded() const {
    if (flags == NULL) return false;
    else return flags->included;
}
// ----------------------
bool jfResultUnit::SetIncluded(bool is_included) {
    if (flags == NULL) return false;
    else {
        flags->included = is_included;
        return true;
    }
}
// ---------------------
bool jfResultUnit::HasCategory() const {
    if (flags == NULL) return false;
    else return (flags->category).any();
}
// ---------------------
bool jfResultUnit::InCategory(const size_t& catindex) const {
    assert(catindex<32);
    if (flags == NULL) return false;
    else return (flags->category)[catindex];
}
// ++++++++++++++++++++++++++++++++++++
// specials
// ---------------------
bool jfResultUnit::SameID(const size_t& cval) const {
  return (cval==(slink->GetId()));
}
// ---------------------
const jfSearchResultItem* jfResultUnit::SourceLink() const {
    return slink;
}
// ---------------------
bool jfResultUnit::HasFlags() const {
    return (flags != NULL);
}
// ---------------------------
jfItemMetaFlags* jfResultUnit::Flags() const {
    return flags;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfResultUnit::Dispose() {
    if (slink != NULL) {
        delete slink;
        slink = NULL;
    }
    if (flags != NULL) {
        delete flags;
        flags = NULL;
    }
}
// ---------------------------------
jfResultUnit::~jfResultUnit() {
    slink = NULL;
    flags = NULL;
}


/*****************************************************************************/
