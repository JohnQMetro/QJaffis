/******************************************************************************
Name    :   fim_ficutils.h
Author  :   John Q Metro
Purpose :   Parsers and Displayers for the Fanfic object for fimfiction.net
Created :   March 13, 2023
Updated :   March 20, 2023
******************************************************************************/
#pragma once

#ifndef FIM_FICUTILS_H
    #define FIM_FICUTILS_H
#endif // FIM_FICUTILS_H
// ------------------------------------
#include "../../../core/objs/itemutils.h"
#include "../ficitemutils.h"

#ifndef FIM_FICOBJ_H_INCLUDED
  #include "fim_ficobj.h"
#endif // FIM_FICOBJ_H_INCLUDED

#ifndef FICEXTRACT_H_INCLUDED
  #include "../../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
/*****************************************************************************/
class jfFIMFanficOutputter: public jfFanficItemOutputterBase {
  public:
    jfFIMFanficOutputter();
    jfFIMFanficOutputter(const jfDisplayHTMLHelper* in_html_helper);
    void SetFanfic(const jfFIM_Fanfic* in_fim_fanfic);
    virtual void SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) override;
    // to string functions
    virtual QString ToText() const override;
    virtual QString ToDisplayHTML() const override;

    virtual ~jfFIMFanficOutputter();
  protected:
    const jfFIM_Fanfic* fim_fanfic;

};
// ===================================================================
class jfFIMFanficItemParser : public jfItemParseBase {
  public:
    // constructor
    jfFIMFanficItemParser();
    // clearing or getting results
    virtual void Clear() override;
    virtual const jfSearchResultItem* GetUntypedItem() override;
    jfFIM_Fanfic* GetFanfic();
    // destructor
    virtual ~jfFIMFanficItemParser();
  protected:
    virtual void ClearItem() override;
    // parsing methods
    virtual jfItemParseResultState InnerParse() override;
    jfItemParseResultState ParseFIMTags();
    jfItemParseResultState ParsePublishedDate(const QDate& latedate);
    bool ParseFIMDate(QString insrc, QDate& out) const;
    QString CleanUpSummary(QString&& raw_description) const;

    jfFIM_Fanfic* result;
    QLocale english_locale;
};
// ============================================================================
/* This is for parsing to Fim Fic Extracts */
class jfFIMExtractParser : public jfFicExtractParseBase  {
  public:
    jfFIMExtractParser();
    virtual void Clear() override;
    virtual const jfFicExtract* GetUntypedItem() override;
    const jfFicExtract_FIM* GetFIMExtract();
    virtual ~jfFIMExtractParser();
  protected:
    virtual void ClearItem() override;
    virtual jfItemParseResultState InnerParse() override;
    jfItemParseResultState CoreParse(QString&& rawcore);
    bool ParseFIMDate(QString insrc, QDate& out) const;

    jfFicExtract_FIM* fim_extract;
    QLocale english_locale;
};

/*****************************************************************************/
