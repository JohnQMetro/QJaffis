/******************************************************************************
Name    :   basefilter.cpp
Author  :   John Q Metro
Purpose :   A Revised set of base classes for filters
Created :   April 1, 2023
Updated :   April 1, 2023
******************************************************************************/
#include "basefilter.h"

#include <assert.h>

/*****************************************************************************/
// base constructors and destructors
//--------------------------
jfFilterBase::jfFilterBase(const QString& filter_name) {
    name = filter_name;
    old_unescape = false;
}
//--------------------------
jfFilterBase::jfFilterBase(QString&& filter_name) {
    name = filter_name;
    old_unescape = false;
}
//--------------------------
jfFilterBase::~jfFilterBase() {}
// ++++++++++++++++++++++++++++++++++++++++
// core match methods
bool jfFilterBase::Match(const jfSearchResultItem* testelem) const {
    assert(GetTypeMetaInfo().CanFilter(testelem));
    if (IsEmpty()) return true;
    else return CoreMatch(testelem);
}
// ++++++++++++++++++++++++++++++++++++++++
// accessing the internals
//--------------------------
const QString& jfFilterBase::GetName() const {
    return name;
}
//--------------------------
bool jfFilterBase::SetName(const QString& new_name) {
    if (new_name.isEmpty()) return false;
    else {
        name = new_name;
        return true;
    }
}
//--------------------------
const QString& jfFilterBase::GetDescription() const {
    return description;
}
//--------------------------
void jfFilterBase::SetDescription(const QString& newdesc) {
    description = newdesc;
}
// ++++++++++++++++++++++++++++++++++++++++
// specials
const QString& jfFilterBase::GetTypeIdentifier() const {
    return GetTypeMetaInfo().Identifier();
}
// ------------------------------------------
bool jfFilterBase::MatchTypeIdentifier(const QString to_match) const {
    return (GetTypeMetaInfo().Identifier() == to_match);
}
// ++++++++++++++++++++++++++++++++++++++++
// i/o
bool jfFilterBase::FromString(const QString& sourcedata) {
    QString ignored_error;
    return FromStringInner(sourcedata, ignored_error);
}
// ----------------------------------------
bool jfFilterBase::AddToFile(QTextStream* outfile) const {
    if (outfile == NULL) return false;
    jfOutString s_out;
    // type, name, and description
    s_out << QString("Fil") << GetTypeIdentifier() << name << description;
    QString qbuf = "###" + QString(s_out);
    (*outfile) << qbuf << '\n';
    s_out.FullClear();

    // for the next line, we will just use ToString
    (*outfile) << EscapeString(ToString(), IO_DELIMS) << '\n';

    // adding on the footer
    (*outfile) << "###\n";
    return true;
}
// -----------------------------
bool jfFilterBase::GetFromFile(jfFileReader* infile) {
    const QString funcname="jfFilterBase::GetFromFile";

    assert(infile != NULL);
    bool use_old = false;
    // I will attempt here to also read the old format!
    if (!infile->ReadParseSLine(4, funcname)) {
        if (!infile->ParseOldSLine(3, funcname)) return false;
        else use_old = true;
    }
    QString in_type;

    // the new format comes first
    if (!use_old) {
        // 'Fil' Check
        if ((infile->lp.Get(0)) != "Fil") {
            return infile->BuildError("The 4 fiels do not start with 'Fil', bad format!");
        }
        // type check
        in_type = infile->lp.UnEscStr(1);
        if (!MatchTypeIdentifier(in_type)) {
            return infile->BuildError("The type (" + in_type + ") does not match!");
        }
        // name and description
        QString in_name = infile->lp.UnEscStr(2);
        QString in_description = infile->lp.UnEscStr(3);

        // the following line is the filter content, we unescape
        QString raw_filter_string;
        if (!infile->ReadUnEsc(raw_filter_string,funcname)) return false;
        QString parse_error;
        if (!FromStringInner(raw_filter_string, parse_error)) {
            return infile->BuildError("The filter data could not be parsed: " + parse_error);
        }
        // we should be left just before a final line...
        if (!infile->ReadLast(funcname)) return false;
        // finishing
        name = in_name;
        description = in_description;
        return true;
    }

    // old format
    else {
        // type check
        in_type = infile->lp.UnEscStr(0);
        if (!MatchTypeIdentifier(in_type)) {
            return infile->BuildError("The type (" + in_type + ") does not match!");
        }

        // field two is a number id
        size_t num_id; // we no longer use this
        if (!infile->lp.SIntVal(1,num_id)) {
          return infile->BuildError("The id is not an integer!");
        }
        // field number 3 is the validdata attribute
        bool validdata;
        if (!infile->lp.BoolVal(2,validdata)) {
          return infile->BuildError("Validdata is not a boolean!");
        }

        // we get the next line
        if (!infile->ReadParseLine(2,funcname)) return false;
        // the name
        QString in_name = infile->lp.UnEscStr(0);
        if (name.isEmpty()) infile->BuildError("The name is blank!");
        // description
        QString in_description = infile->lp.UnEscStr(1);

        // the old filter format had a 'number of lines' field that we ended up not using
        infile->SkipLine(funcname);

        // a special method to handle the rest...
        if (!CompatReadRestFromFile(infile)) return false;

        // we should be left just before a final line...
        if (!infile->ReadLast(funcname)) return false;
        // finishing
        name = in_name;
        description = in_description;
        return true;
    }
}
// ------------------------------------------------------
/* This method replaces ReadRestFromFile in reading filters created before the
 * current (April 2023) revision. This default will work for many types, but
 * can be overriden. */
bool jfFilterBase::CompatReadRestFromFile(jfFileReader* infile) {
    const QString fname = "jfFilterBase::CompatReadRestFromFile";
    QString raw_filter_string;

    if (old_unescape) {
        if (!infile->ReadUnEsc(raw_filter_string, fname)) return false;
    }
    else {
        if (!infile->ReadLine(raw_filter_string,fname)) return false;
    }
    QString parse_error;
    if (!FromStringInner(raw_filter_string, parse_error)) {
        return infile->BuildError("The filter data could not be parsed: " + parse_error);
    }
    return true;
}

// ====================================================
// ====================================================

/* Meta information for a filter type, meant to have one instance per type
 * of filter. */
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterTypeMeta::jfFilterTypeMeta(jfFilterTypeGroup group,
                                   const QString& identifier, const QString name,
                                   const QString& desc,
                                   std::vector<QString> type_identifiers,
                                   jfFilterBase* (*in_factory)(const QString&)) {
    assert(in_factory != NULL);
    assert(type_identifiers.size() > 0);

    typeGroup = group;
    typeIdentifier = identifier;
    typeName = name;
    typeDescription = desc;
    compatable_identifiers = type_identifiers;
    factory = in_factory;

    // we auto-add this to the global map
    allfilters::getFilterTypes()[typeIdentifier] = this;
}
// ------------------------------------------------------------
jfFilterTypeMeta::jfFilterTypeMeta(jfFilterTypeGroup group,
                                   const QString& identifier, const QString name,
                                   const QString& desc,
                                   const QString& type_identifier,
                                   jfFilterBase* (*in_factory)(const QString&)) {
    assert(in_factory != NULL);
    assert(!type_identifier.isEmpty());

    typeGroup = group;
    typeIdentifier = identifier;
    typeName = name;
    typeDescription = desc;
    compatable_identifiers.push_back(type_identifier);
    factory = in_factory;

    // we auto-add this to the global map
    allfilters::getFilterTypes()[typeIdentifier] = this;
}
// ------------------------------------
const jfFilterTypeGroup& jfFilterTypeMeta::Group() const {
    return typeGroup;
}
// ------------------------------------
const QString& jfFilterTypeMeta::Identifier() const {
    return typeIdentifier;
}
// ------------------------------------
const QString& jfFilterTypeMeta::Name() const {
    return typeName;
}
// ------------------------------------
const QString& jfFilterTypeMeta::Description() const {
    return typeDescription;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterTypeMeta::CanFilter(const jfSearchResultItem* testelem) const {
    if (testelem == NULL) return false;
    for (QString ident : compatable_identifiers) {
        if ((testelem->GetTypeLabels()).contains(ident)) return true;
    }
    return false;
}
// ------------------------------------
jfFilterBase* jfFilterTypeMeta::MakeWithName(const QString filterName) const {
    return factory(filterName);
}
// +++++++++++++++++++++++++++++++++++++++++++++
jfFilterTypeMeta::~jfFilterTypeMeta() {
    factory = NULL;
}


// ===================================================================
std::map<QString,const jfFilterTypeMeta*>& allfilters::getFilterTypes() {
    static std::map<QString,const jfFilterTypeMeta*> filter_types = std::map<QString,const jfFilterTypeMeta*>();
    return filter_types;
}

/* This method is for creating an empty filter with a name using the
 * filter type identifier, and the name. */
jfFilterBase* allfilters::MakeFilter(const QString& typeIdentifier, const QString& filterName) {
    auto search = getFilterTypes().find(typeIdentifier);
    if (search != getFilterTypes().end()) {
        return (search->second)->MakeWithName(filterName);
    }
    // special compatability case
    else if ((typeIdentifier == "AO3KudoCountFilter") || (typeIdentifier == "FFNFavsFilter")) {
        QString compatType = "KudoCountFilter";
        auto search = getFilterTypes().find(compatType);
        if (search != getFilterTypes().end()) {
            return (search->second)->MakeWithName(filterName);
        }

    }
    return NULL;
}



/*****************************************************************************/
