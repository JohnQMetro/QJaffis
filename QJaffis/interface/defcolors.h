/******************************************************************************
Name    :   defcolors.h
Author  :   John Q Metro
Purpose :   Colour constants
Created :   July 13, 2017
Updated :
******************************************************************************/
#ifndef DEFCOLORS_H
    #define DEFCOLORS_H
#endif // DEFCOLORS_H
//-----------------------------------------
# include <QColor>
/*****************************************************************************/
namespace defcolour {
    extern QColor base;
    extern QColor highlighted;
    extern QColor ignored;
    extern QColor highl_ignore;


    void SetupColours();
}
/*****************************************************************************/
