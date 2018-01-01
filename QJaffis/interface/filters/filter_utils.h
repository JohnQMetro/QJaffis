/******************************************************************************
Name    :   filter_utils.h
Author  :   John Q Metro
Purpose :   A filter editor creator, and anything else that comes to mind
Created :   December 27, 2010
Conversion to Qt Started Oct 24, 2013
Updated :   Febuary 19, 2012
******************************************************************************/
#ifndef FILTER_UTILS_H_INCLUDED
#define FILTER_UTILS_H_INCLUDED
#endif // FILTER_UTILS_H_INCLUDED
//-----------------------------------------------------------
#ifndef JFBASEFILTER
  #include "../../core/filters/base/filterbase.h"
#endif
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../core/structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
//------------------------------------------------
#include <QWidget>
//***************************************************************************
// creates an appropriate editor using the provided links
jfBaseFilterEditor* MakeFilterEditor(QString in_typeid, QWidget* parent,
                      jfFilterMap* containing_map, jfBaseFilter* input_filter, bool isglobal);
//***************************************************************************
