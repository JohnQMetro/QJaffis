/////////////////////////////////////////////////////////////////////////////
// Name:        filter_globals.h
// Author :     John Q Metro
// Purpose :    Declares global filter constants & stuff
// Created:     30.07.06
// Conversion to QT Started April 9, 2013
// Updated:     September 3, 2012
/////////////////////////////////////////////////////////////////////////////
// headers
#define JFILTERGLOBALS

#ifndef JFILTERGROUPS
  #include "filter_groups.h"
#endif
//=============================================================================
//===========================================================================
namespace filterglobals {
  const QString fnamex = "config/gfilt.dat";
  const size_t ITEM_NLIST = 9;
  extern QStringList* fclist[];
  extern jfMapOfFiltermaps* global_filters;
  bool LoadStrings();
  void Shutdown();
}
/////////////////////////////////////////////////////////////////////////////
