/*****************************************************************************
Name    : baseitem.h
Basic   : Base class for items that can be searched, like Fanfic Info Items
Author  : John Q Metro
Started : March 7, 2023
Updated : August 25, , 2023
Notes   : Replaces the earlier jfBasePD
******************************************************************************/
#ifndef BASEITEM_H
    #define BASEITEM_H
#endif // BASEITEM_H

#pragma once
//-------------------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef HTMLSKELTON_H_INCLUDED
  #include "../utils/htmlskelton.h"
#endif // HTMLSKELTON_H_INCLUDED

#include <QString>
#include <bitset>
#include <QColor>
/*****************************************************************************/
/* This to allow the jfSearchResultItem class to have data
 * set without using set methods or complex constructors. */
struct jfSearchResultItemData {
    size_t numeric_id;
    QString title;
    QString summary;
    QString url;
};
//=============================================
const QString& IdForBaseSearch();


/* The base class for searched items. */
class jfSearchResultItem {
  public:
    jfSearchResultItem();
    jfSearchResultItem(const jfSearchResultItemData& init_data);
    jfSearchResultItem(const jfSearchResultItem& source);
    // data get and set
    size_t GetId() const;
    const QString& GetTitle() const;
    const QString& GetSummary() const;
    const QString& GetURL() const;
    const QStringList& GetTypeLabels() const;
    const QString& GetTypeLabel() const;
    // tokenized parts
    const QStringList* GetTokTitle() const;
    const QStringList* GetTokSummary() const;
    // file i/o
    bool AddToFile(QTextStream* outfile) const;
    bool GetFromFile(jfFileReader* infile);
    // other
    bool LoadValues(jfSkeletonParser* inparser) const;
    // destructor
    virtual ~jfSearchResultItem();
  protected:
    void StoreToCopyBase(jfSearchResultItem* destination) const;
    virtual bool LoadMoreValues(jfSkeletonParser* inparser) const =0;
    virtual bool AddRestToFile(QTextStream* outfile) const =0;
    virtual bool ReadRestFromFile(jfFileReader* infile) =0;

    size_t numeric_id;
    QString title;
    QString summary;
    QString url;

    QStringList type_labels;
};
// ===========================================
// update status enum
enum class jfUpdateStatus {
    NONE, UPDATED, MISSING, NEW, AUTHORNC
};

// string
QString UpdateStatusToString(const jfUpdateStatus& inval);
jfUpdateStatus StringToUpdateStatus(const QString& inval);

// ===========================================
/* 'Meta' info for an item like search, categorization, update, etc. */
class jfItemMetaFlags {
  public:
    jfItemMetaFlags();
    jfItemMetaFlags(const jfItemMetaFlags& other);

    std::bitset<32> category;
    jfUpdateStatus ustatus;
    bool included;
    bool ignore;
    QColor background_color;
};
// ============================================
struct jfItemFlagGroup {
    const jfSearchResultItem* item;           // the search result item
    jfItemMetaFlags* flags;  // associated flags and stuff

    void Dispose();
    bool isNull() const;
};
//=============================================
/* A front end to jfSearchResultItem used in displaying Results. */
class jfResultUnit {
  public:
    jfResultUnit(const jfSearchResultItem* source, jfItemMetaFlags* in_flags, QString&& in_html);
    jfResultUnit(const QString in_html, const jfSearchResultItem* source, jfItemMetaFlags* in_flags);

    // getting the string data
    const QString& GetUrl() const;
    const QString& GetHTML() const;
    const QString& GetTitle() const;
    size_t GetId() const;
    // marking data
    bool IsIncluded() const;
    bool SetIncluded(bool is_included);
    bool HasCategory() const;
    bool InCategory(const size_t& catindex) const;
    // specials
    bool SameID(const size_t& cval) const;
    const jfSearchResultItem* SourceLink() const;
    bool HasFlags() const;
    jfItemMetaFlags* Flags() const;
    // getting rid of stuff
    void Dispose();
    virtual ~jfResultUnit();
  protected:
    // internal data
    QString htmldata;
    const jfSearchResultItem* slink;
    jfItemMetaFlags* flags;
};

Q_DECLARE_METATYPE(jfResultUnit*);

/*****************************************************************************/
