/******************************************************************************
Name    :   defcolors.cpp
Author  :   John Q Metro
Purpose :   Colour constants
Created :   July 13, 2017
Updated :
******************************************************************************/
#ifndef DEFCOLORS_H
    #include "defcolors.h"
#endif // DEFCOLORS_H
//-----------------------------------------
# include <QColor>
/*****************************************************************************/
QColor defcolour::base;
QColor defcolour::highlighted;
QColor defcolour::ignored;
QColor defcolour::highl_ignore;

void  defcolour::SetupColours() {
    base = QColor::fromRgb(255,255,255);
    highlighted = QColor::fromRgb(245,245,255);
    ignored = QColor::fromRgb(255,235,235);
    highl_ignore = QColor::fromRgb(245,225,235);
}

/*****************************************************************************/

