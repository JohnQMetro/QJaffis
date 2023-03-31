/*****************************************************************************
Name    : ficitemutils.h
Basic   : Classes and stuff that help with Fanfic Item manipulation
Author  : John Q Metro
Started : March 11, 2023
Updated : March 16, 2023
Notes   : Replaces some parsing and output stuff
******************************************************************************/
#ifndef FICITEMUTILS_H
    #define FICITEMUTILS_H
#endif // FICITEMUTILS_H

#pragma once
// ---------------------------------------------------
#include "../../core/objs/baseitem.h"
#include "../../core/objs/itemutils.h"

#ifndef FICBASE_H
  #include "ficbase.h"
#endif // FICBASE_H

#ifndef DISPLAYHTMLSPEC_H
    #include "displayhtmlspec.h"
#endif // DISPLAYHTMLSPEC_

#ifndef FICEXTRACT_H_INCLUDED
  #include "../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
/*****************************************************************************/
/* This is for converting jfSearchResultItem items to Text or HTML */
class jfFanficItemOutputterBase : public jfResultItemOutputter {
  public:
    jfFanficItemOutputterBase(const jfDisplayHTMLHelper* in_html_helper);
    // to string functions
    virtual QString ToText() const =0;
    virtual QString ToDisplayHTML() const=0;

    virtual ~jfFanficItemOutputterBase();
  protected:
    QString DisplayHTMLHeader() const;
    QString DisplayHTMLHeader(const jfDisplayHTMLHelper* helper ) const;

    const jfGenericFanfic* fanfic_source;
    const jfDisplayHTMLHelper* html_helper;
};
// ====================================================================
/* This is for parsing to Fic Extracts */
class jfFicExtractParseBase {
  public:
    jfFicExtractParseBase(bool in_case_sensetive);
    jfItemParseResultState ParseFromSource(const QString& source);
    virtual void Clear() = 0;

    // result getting
    bool HasResult() const;
    jfItemParseResultState LastResultState() const;
    virtual const jfFicExtract* GetUntypedItem() = 0;
    const QString& LastError() const;

    virtual ~jfFicExtractParseBase();
  protected:
    virtual void ClearItem() = 0;
    jfItemParseResultState SetError(const QString err_msg);

    void ClearCore(bool clear_source);
    virtual jfItemParseResultState InnerParse() = 0;

    bool case_sensetive;
    jfStringParser xparser;

    bool has_result;
    jfItemParseResultState result_state;
    jfFicExtract* untyped_item;
    QString error_message;
};

/*****************************************************************************/
