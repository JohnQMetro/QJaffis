/////////////////////////////////////////////////////////////////////////////
// Name:        filter_groups.h
// Author :     John Q Metro
// Purpose :    Declares global filter map groupings and associated stuff
// Created:     01.08.06
// Conversion to QT Started April 9, 2013
// Updated:     April 5, 2023 (re-basing)
/////////////////////////////////////////////////////////////////////////////
// headers
// ----------------------------------------------------------------------------
#define JFILTERGROUPS

#ifndef BASECOLLECT_H_INCLUDED
  #include "../structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED

/*
#ifndef JFBASEFILTER
  #include "filterbase.h"
#endif
*/

#ifndef EXPRESSION_H
  #include "base/expression.h"
#endif // EXPRESSION_H

#include "base/basefilter.h"
#include "filtermap.h"

// ----------------------------------------------------------------------------
#include <map>
//*****************************************************************************
// forward declarations
class jfMapOfFiltermaps;
//============================================================================
// jfBaseFilter* MakeFilter(const QString& typestring, const QString& newname);
//===========================================================================
// the following class serves as a sort of pointer for a specific filter,
// as well as it's context
class jfFilterPointer{
  friend class jfMapOfFiltermaps;
  public:
    jfFilterBase* filter;
    // public methods
    QString GetMapName() const;
    bool CheckForName(const QString& fname) const;
    bool RenameElem(const QString& newname);
  protected:
    jfFilterMap* filtermap;
    jfMapOfFiltermaps* mapomaps;
};
//============================================================================
// the array of filter types
class jfMapOfFiltermaps : public std::map<QString,jfFilterMap*,ltstr> {
  public:
    // info methods
    bool NameExists(const QString& mapname) const;
    jfFilterPointer* GetPointer(const QString& mapname, const QString& filtername);
    QStringList* GetMapNames() const;
    // some other special methods
    bool FindMapIndex(const QString& mapname, size_t& newindex) const;
    bool RenameMap(const QString& oldname, const QString& newname, size_t& newindex);
    //file i/o
    void WriteToFile(const QString& filename) const;
    bool ReadFromFile(const QString& filename);
    // finishing
    int EmptyMaps();
  protected:
};



/////////////////////////////////////////////////////////////////////////////
