/******************************************************************************
Name    :   filter_utils.h
Author  :   John Q Metro
Purpose :   A filter editor creator, and anything else that comes to mind
Created :   December 27, 2010
Conversion to Qt Started Oct 24, 2013
Updated :   August 24, 2023
******************************************************************************/
#ifndef FILTER_UTILS_H_INCLUDED
#define FILTER_UTILS_H_INCLUDED
#endif // FILTER_UTILS_H_INCLUDED
//-----------------------------------------------------------
#ifndef JFBASEFILTER
  #include "../../core/filters/base/basefilter.h"
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
jfBaseFilterEditor* MakeFilterEditor(QWidget* parent,
                                     jfFilterMap* containing_map, bool isglobal,
                                     jfFilterBase* input_filter );
//***************************************************************************
