/******************************************************************************
Name    :   ffn_ficutils.h
Author  :   John Q Metro
Purpose :   Outputters and Parsers for Fanfiction.Net fic items.
Created :   March 21, 2023
Updated :   March 21, 2023
******************************************************************************/
#pragma once

#ifndef FFN_FICUTILS_H
    #define FFN_FICUTILS_H
#endif // FFN_FICUTILS_H
// -----------------------------------------
#include "../../core/objs/baseitem.h"
#include "../../core/objs/itemutils.h"
#include "../../fanfic/data/ficitemutils.h"

#ifndef DISPLAYHTMLSPEC_H
  #include "../../fanfic/data/displayhtmlspec.h"
#endif // DISPLAYHTMLSPEC_


#ifndef JFFNFICOBJ
  #include "ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED
/*****************************************************************************/
class jfFFNItemOutputter : public jfFanficItemOutputterBase {
  public:
    jfFFNItemOutputter();
    jfFFNItemOutputter(const jfDisplayHTMLHelper* in_html_helper);
    virtual void SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) override;
    void SetFFNItemSource(const jfFFNItemCore* in_source, const jfItemMetaFlags* in_flags);
    // to string functions
    virtual QString ToText() const override;
    virtual QString ToDisplayHTML() const override;

    virtual ~jfFFNItemOutputter();
  protected:

    // const jfSearchResultItem* source;
    const jfFFNItemCore* ffn_fanfic;
    // const jfItemMetaFlags* flags;
};
//=========================================================================

/** Parser class that is a base and holds various parser utilities. */
class jfFFNFanficParseBase : public jfItemParseBase {
  public:
    jfFFNFanficParseBase();
    virtual ~jfFFNFanficParseBase();
  protected:
    virtual jfFFNItemCore* fanfic() const = 0;
    // parsing helper methods
    virtual jfItemParseResultState GetLinkTitle() = 0;
    bool ExtractDescription(QString &parserr, QString& out_description);
    bool Rating2Words(QString &parserr);
    QStringList SplitCharacters(const QString& source) const;
    bool Characters_Pairs_and_Completion(QString &parserr);
    bool Dates_and_Completion(QString &parserr);
    bool failMsg(QString &pto, const QString message)const;

    QRegularExpression summary_start;
    QRegularExpression date_start;
};

// ==================================================================

/* Parser for jfFFNItem. */
class jfFFNFanficParse : public jfFFNFanficParseBase {
  public:
    jfFFNFanficParse();
    // custom methods
    void SetCategory(const jfFFN_CategoryCore* in_cat_link);
    void ClearCategory();
    virtual void Clear() override;
    virtual const jfSearchResultItem* GetUntypedItem() override;
    const jfFFNItem* getFanfic();

    ~jfFFNFanficParse();
  protected:
    virtual jfFFNItemCore* fanfic() const override;
    virtual void ClearItem() override;
    virtual jfItemParseResultState GetLinkTitle() override;
    virtual jfItemParseResultState InnerParse() override;

    jfFFNItem* ffn_fic;
    QRegularExpression storyid_start;
    const jfFFN_CategoryCore* cat_link;
};
/*****************************************************************************/
