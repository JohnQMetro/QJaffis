/******************************************************************************
Name    :   upd_utils.h
Author  :   John Q Metro
Purpose :   Parsers for Fanfiction.Net fic items for Updating.
Created :   March 26, 2023
Updated :   March 26, 2023
******************************************************************************/
#pragma once

#ifndef UPD_UTILS_H
    #define UPD_UTILS_H
#endif // UPD_UTILS_H
//------------------------------------------------
#ifndef UPD_TYPES_H_INCLUDED
  #include "upd_types.h"
#endif // UPD_TYPES_H_INCLUDED

#include "../../fanficnet/data/ffn_ficutils.h"

/*****************************************************************************/
/* Parser for jfFFNItemAuthor. */
class jfFFNAuthorFanficParser : public jfFFNFanficParseBase {
  public:
    jfFFNAuthorFanficParser();
    // custom methods
    // void SetCategory(const jfFFN_CategoryCore* in_cat_link);
    // void ClearCategory();
    virtual void Clear() override;
    virtual const jfSearchResultItem* GetUntypedItem() override;
    jfFFNItemAuthor* GetFanfic();

    virtual ~jfFFNAuthorFanficParser();
  protected:
    virtual jfFFNItemCore* fanfic() const override;
    virtual void ClearItem() override;
    virtual jfItemParseResultState GetLinkTitle() override;
    virtual jfItemParseResultState InnerParse() override;

    jfFFNItemAuthor* ffn_fic;
    QRegularExpression storyid_start;
    QRegularExpression title_img;
    QRegularExpression catname_start;
    // const jfFFN_CategoryCore* cat_link;
};
/*****************************************************************************/
