/******************************************************************************
Name    :   basefilter.h
Author  :   John Q Metro
Purpose :   A Revised set of base classes for filters
Created :   April 1, 2023
Updated :   April 15, 2023
******************************************************************************/
#pragma once

#ifndef BASEFILTER_H
    #define BASEFILTER_H
#endif // BASEFILTER_H
//------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "../../utils/utils2.h"
#endif // UTILS2_H_INCLUDED

#include "../../objs/baseitem.h"

#include <QString>
#include <QTextStream>
/*****************************************************************************/
class jfFilterTypeMeta; // a forward declaration
// ------------------------------------------------
class jfFilterBase {
  public:
    // base constructors and destructors
    jfFilterBase(const QString& filter_name);
    jfFilterBase(QString&& filter_name);
    virtual ~jfFilterBase();
    virtual jfFilterBase* GenCopy() const = 0;

    // core match methods
    virtual bool IsEmpty() const = 0; // empty filters always match
    bool Match(const jfSearchResultItem* testelem) const;

    // accessing the internals
    const QString& GetName() const;
    bool SetName(const QString& new_name);
    const QString& GetDescription() const;
    void SetDescription(const QString& newdesc);
    // specials
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const = 0;
    virtual const QString& GetTypeIdentifier() const;
    virtual bool MatchTypeIdentifier(const QString to_match) const;

    // i/o
    bool FromString(const QString& sourcedata);
    virtual QString ToString() const = 0;
    virtual bool AddToFile(QTextStream* outfile) const;
    bool GetFromFile(jfFileReader* infile);
  protected:
    virtual bool CompatReadRestFromFile(jfFileReader* infile);
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) = 0;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const = 0;

    QString name,description;
    bool old_unescape;
    /* Set to true if we unescape the FromString source while reading the old file format. */
};
// ====================================================
// generic create method for making filters
template<class T_filter>
jfFilterBase* createFilter(const QString& filterName) {
    return new T_filter(filterName);
}
// +++++++++++++++++++++++++++++++++++++++++++++++
// an enum that is a 'generic' type group for filters
enum class jfFilterTypeGroup {
    EXPRESSION, // the general expression type
    PAIR,       // relationship pair filters
    DATE,       // published or updated date
    CATEGORY,   // rating or sexual orientation
    GENRE,      // genre, including the extra tags percent filter
    COUNTS,     // word and part counts
    CHARACTERS, // characters
    FLAGS,
    FANDOM,
    FAVS,
    MISC
};

// +++++++++++++++++++++++++++++++++++++++++++++++
/* Meta information for a filter type, meant to have one instance per type
 * of filter. */
class jfFilterTypeMeta {
  public:
    // constructors
    jfFilterTypeMeta(jfFilterTypeGroup typeGroup,
                     const QString& identifier, const QString name,
                     const QString& desc, std::vector<QString> type_identifiers,
                     jfFilterBase* (*in_factory)(const QString&));
    jfFilterTypeMeta(jfFilterTypeGroup typeGroup,
                     const QString& identifier, const QString name,
                     const QString& desc, const QString& type_identifier,
                     jfFilterBase* (*in_factory)(const QString&));

    // type information
    const jfFilterTypeGroup& Group() const;
    const QString& Identifier() const;
    const QString& Name() const;
    const QString& Description() const;
    bool CanFilter(const jfSearchResultItem* testelem) const;
    // making
    jfFilterBase* MakeWithName(const QString filterName) const;
    // the destructor
    virtual ~jfFilterTypeMeta();
  protected:
    jfFilterTypeGroup typeGroup;
    QString typeIdentifier;
    QString typeName;
    QString typeDescription;
    std::vector<QString> compatable_identifiers;
    jfFilterBase* (*factory)(const QString&);
};
// ===================================================================

namespace allfilters {
    std::map<QString,const jfFilterTypeMeta*>& getFilterTypes();
    jfFilterBase* MakeFilter(const QString& typeIdentifier, const QString& filterName);
}

/*****************************************************************************/
