/*****************************************************************************
Name    : ffncatparser_exp1.h
Basic   : Regexes for Cargory parsers
Author  : John Q Metro
Started : September 5, 2022
Updated : September 5, 2022

******************************************************************************/
#pragma once

#include <QRegularExpression>
/*****************************************************************************/
class jfFFNCategory_ParserExpressions {
  public:
    QRegularExpression section_headerA;
    QRegularExpression section_name;
    QRegularExpression section_column_headerA;
    QRegularExpression section_column_headerB;

    QRegularExpression arrow_tagA;
    QRegularExpression list_output;
    QRegularExpression btn_href;

    jfFFNCategory_ParserExpressions();
  protected:
};


/*****************************************************************************/
