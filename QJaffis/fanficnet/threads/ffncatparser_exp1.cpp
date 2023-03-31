/*****************************************************************************
Name    : ffncatparser_exp1.cpp
Basic   : Regexes for Cargory parsers
Author  : John Q Metro
Started : September 5, 2022
Updated : December 18, 2022
******************************************************************************/
#include "ffncatparser_exp1.h"
/*****************************************************************************/
jfFFNCategory_ParserExpressions::jfFFNCategory_ParserExpressions() {

    section_headerA = QRegularExpression("\\s+class=['\"]?xcontrast_outer['\"]?\\s+id=['\"]?content_parent['\"]>", QRegularExpression::CaseInsensitiveOption);

    section_name = QRegularExpression("align=['\"]?absmiddle['\"]?>([^<]*)<", QRegularExpression::CaseInsensitiveOption);

    section_column_headerA = QRegularExpression("<td\\s+style=['\"]?line-height:150%;?['\"]?\\s+valign=['\"]?TOP['\"]?>", QRegularExpression::CaseInsensitiveOption);
    section_column_headerB = QRegularExpression("<td\\s+valign=['\"]?TOP['\"]?\\s+style=['\"]?line-height:150%;?['\"]?>",QRegularExpression::CaseInsensitiveOption);

    arrow_tagA = QRegularExpression("<img\\s+src=['\"]?/(?:/ff7\\d.b-cdn.net/)?static/fcons/arrow-switch.png['\"]?", QRegularExpression::CaseInsensitiveOption);
    list_output = QRegularExpression("<div\\s+id=['\"]?list_output['\"]?>", QRegularExpression::CaseInsensitiveOption);
    btn_href = QRegularExpression("<a\\s+class=['\"]?btn['\"]?\\s+href=['\"]?/", QRegularExpression::CaseInsensitiveOption);

}


/*****************************************************************************/
