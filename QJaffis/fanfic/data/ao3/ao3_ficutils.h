/******************************************************************************
Name    :   ao3_ficutils.h
Author  :   John Q Metro
Purpose :   Parsers and Outputters for jfAO3Fanfic
Created :   March 18, 2023
Updated :   March 30, 2023
******************************************************************************/
#ifndef AO3_FICUTILS_H
    #define AO3_FICUTILS_H
#endif // AO3_FICUTILS_H

#pragma once
//--------------------------------
#include "../../../core/objs/itemutils.h"
#include "../ficitemutils.h"

#ifndef AO3_FICOBJ_H_INCLUDED
  #include "ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
/*****************************************************************************/
class jfAO3FanficOutputter: public jfFanficItemOutputterBase {
  public:
    jfAO3FanficOutputter();
    jfAO3FanficOutputter(const jfDisplayHTMLHelper* in_html_helper);
    void SetFanfic(const jfAO3Fanfic* in_fim_fanfic);
    virtual void SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) override;
    // to string functions
    virtual QString ToText() const override;
    virtual QString ToDisplayHTML() const override;

    virtual ~jfAO3FanficOutputter();
  protected:
    const jfAO3Fanfic* ao3_fanfic;

};
// ===================================================================
class jfAO3FanficItemParser : public jfItemParseBase {
  public:
    // constructor
    jfAO3FanficItemParser();
    // clearing or getting results
    virtual void Clear() override;
    virtual const jfSearchResultItem* GetUntypedItem() override;
    const jfAO3Fanfic* GetFanfic();
    // other
    void SetCategory(const jfAO3_Category* this_category);
    void ClearCategory();
    // destructor
    ~jfAO3FanficItemParser();
  protected:
    virtual void ClearItem() override;
    // parsing methods
    virtual jfItemParseResultState InnerParse() override;
    // internal parsing methods
    jfItemParseResultState ParseStart();
    bool ParseFandoms(QString&& fandom_source, std::vector<const jfAO3_Category*>& target) const;
    jfItemParseResultState ParseMiddle();
    jfItemParseResultState ParseTags();
    jfItemParseResultState ParseEnd();
    QString ProcessSummary(QString&& description) const;

    jfAO3Fanfic* result;
    QLocale english_locale;
    QRegularExpression breaks;
    const jfAO3_Category* current_category;
};
/*****************************************************************************/
