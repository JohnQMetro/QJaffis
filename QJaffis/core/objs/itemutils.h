/*****************************************************************************
Name    : itemutils.h
Basic   : Classes and stuff that help with Search Result Item manipulation
Author  : John Q Metro
Started : March 10, 2023
Updated : March 12, 2023
Notes   : Replaces stuff that was previous entirely in jfBasePD items.
******************************************************************************/
#ifndef ITEMUTILS_H
    #define ITEMUTILS_H
#endif // ITEMUTILS_H
// -----------------------------
#pragma once

#ifndef STRINGPARSER_H_INCLUDED
  #include "../utils/stringparser.h"
#endif // STRINGPARSER_H_INCLUDED

#include <bitset>
#include "baseitem.h"
#include <QString>
/*****************************************************************************/

/* This is for converting jfSearchResultItem items to Text or HTML */
class jfResultItemOutputter {
  public:
    jfResultItemOutputter();
    virtual void SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) = 0;
    // for getting a truncated summary
    QString GetSummaryExtract(size_t lines, size_t maxlen) const;
    // to string functions
    virtual QString ToText() const =0;
    virtual QString ToDisplayHTML() const=0;

    virtual ~jfResultItemOutputter();
  protected:
    QString UpdateStatus2String(const jfUpdateStatus& inval) const;
    QString MakeUpdateStatusSpan(const jfUpdateStatus& ustatus) const;
    QString MakeUpdateStatusBlock(const jfUpdateStatus& ustatus) const;

    const jfSearchResultItem* source;
    const jfItemMetaFlags* flags;
};
//======================================================================
// the success state of an item parse
enum class jfItemParseResultState {
    SUCCEESS,   // all is well
    DEFECTIVE,  // parsing went fine, execpt the item is bad
    FAILURE     // parsing failed, likely due to a format change. Alter the parsing code!
};
//+++++++++++++++++++++++++++++++++++++++++++
/* A base class for item parsing */
class jfItemParseBase {
  public:
    jfItemParseBase(bool in_case_sensetive);
    jfItemParseResultState ParseFromSource(const QString& source);
    virtual void Clear() = 0;

    // result getting
    bool HasResult() const;
    jfItemParseResultState LastResultState() const;
    virtual const jfSearchResultItem* GetUntypedItem();
    const QString& LastError() const;

    virtual ~jfItemParseBase();
  protected:
    virtual void ClearItem() = 0;
    jfItemParseResultState SetError(const QString err_msg);

    void ClearCore(bool clear_source);
    virtual jfItemParseResultState InnerParse() = 0;

    bool case_sensetive;
    jfStringParser xparser;

    bool has_result;
    jfItemParseResultState result_state;
    jfSearchResultItem* untyped_item;
    QString error_message;
};

/*****************************************************************************/
