/******************************************************************************
Name    :   specialfuncs.h
Author  :   John Q Metro
Purpose :   Declares some special functions
Created :   january 1, 2010
Conversion to Qt Started February 18, 2014
Updated :   January 1, 2010
******************************************************************************/
#ifndef SPECIALFUNCS_H_INCLUDED
#define SPECIALFUNCS_H_INCLUDED
#endif // SPECIALFUNCS_H_INCLUDED
//------------------------------------
#ifndef SEARCHCORE_H_INCLUDED
  #include "../objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
/*****************************************************************************/
// a function that takes a file and creates a Search Object from it
jfSearchCore* MakeSearchFromFile(QString infpath, QString& outerr);
/*****************************************************************************/
