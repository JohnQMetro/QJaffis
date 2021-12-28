/////////////////////////////////////////////////////////////////////////////
// Name:        filter_groups.cpp
// Author :     John Q Metro
// Purpose :    Defines global filter map groupings and associated stuff
// Created:     01.08.06
// Started conversion to Qt June 23, 2013
// Updated:     December 28, 2021
/////////////////////////////////////////////////////////////////////////////
// headers
// ----------------------------------------------------------------------------
#ifndef JFILTERGROUPS
  #include "filter_groups.h"
#endif
#ifndef BASECOLLECT_H_INCLUDED
  #include "../structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED

#ifndef SEXP_FILTERS_H_INCLUDED
  #include "base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
#ifndef MOREFILTERS1_H_INCLUDED
  #include "extended/morefilters1.h"
#endif
#ifndef PAIRFILTER_H_INCLUDED
  #include "extended/pairfilter.h"
#endif // PAIRFILTER_H_INCLUDED
#ifndef DATERANGE_FILTER_H
  #include "extended/daterange.h"
#endif // DATERANGE_FILTER_H
#ifndef EXTENDED_SEXP_FILTER_H
  #include "extended/extended_sexp.h"
#endif // EXTENDED_SEXP_FILTER_H

#ifndef FIMFILTERS1_H_INCLUDED
  #include "fim/fimfilters1.h"
#endif // FIMFILTERS1_H_INCLUDED
#ifndef FIMFILTERS2_H_INCLUDED
  #include "fim/fimfilters2.h"
#endif // FIMFILTERS2_H_INCLUDED
#ifndef FIMFILTERS3_H
    #include "fim/fimfilters3.h"
#endif // FIMFILTERS3_H
#ifndef AO3_STRINGFILTERS_H_INCLUDED
  #include "ao3/ao3_stringfilters.h"
#endif // AO3_STRINGFILTERS_H_INCLUDED
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "ao3/ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
#ifndef AO3_SPECIALS2_H_INCLUDED
  #include "ao3/ao3_specials2.h"
#endif // AO3_SPECIALS2_H_INCLUDED
#ifndef AO3_SPECIALS3_H
  #include "ao3/ao3_specials3.h"
#endif // AO3_SPECIALS3_H

#ifndef FFN_SPECIAL1_H
  #include "ffn/ffn_special1.h"
#endif // FFN_SPECIAL1_H


//-----------------------------------------


#include <assert.h>
#include <QMessageBox>

//*****************************************************************************
jfBaseFilter* MakeFilter(const QString& typestring, const QString& newname) {
  // local variables
  jfBaseFilter* result;
  jfFFNGenresFilter* itag_result;
  jfFIMGenreFilter* gtag_result;
  jfFIMCharacterFilter* ctag_result;
  jfFIMContentTypeFilter* xtag_result;
  jfFIMWarningsFilter* wtag_result;

  jfAO3OrientationFilter* otag_result;

  // the multiple types
  if (typestring=="UrlFilter") result = new jfUrlFilter();
  else if (typestring=="ExprFilter") result = new jfExpressionFilter();

  else if (typestring=="WordCountFilter") result = new jfWordCountFilter();
  else if (typestring=="FFNTagFilter") {
    itag_result = new jfFFNGenresFilter();
    itag_result->SetToEmpty();
    result = itag_result;
  }
  else if (typestring=="AuthorFilter") result = new jfAuthExprFilter();
  else if (typestring=="CharacterFilter") result = new jfCharExprFilter();
  else if (typestring=="CategoryFilter") result = new jfFFN_CategoryExprFilter();
  else if (typestring=="PairFilterSingle") result = new jfPairFilterSingle();
  else if (typestring=="PairFilterList") result = new jfPairFilterList();
  else if (typestring=="PairFilterMultiple") result = new jfPairFilterMultiple();
  else if (typestring=="CompletedFilter") result = new jfCompletedFilter();
  else if (typestring=="LanguageFilter") result = new jfLanguageExprFilter();
  else if (typestring=="FFNRatingFilter") result = new jfFFNRatingFilter();
  else if (typestring=="UpdatedDateFilter") result = new jfUpdatedDateFilter();
  else if (typestring=="PublishedDateFilter") result = new jfPublishedDateFilter();
  else if (typestring=="FimThumbsFilter") result = new jfFimThumbsFilter();
  else if (typestring=="FIMGenreFilter") {
    gtag_result = new jfFIMGenreFilter();
    gtag_result->SetToEmpty();
    result = gtag_result;
  }
  else if (typestring=="FIMCharacterFilter") {
    ctag_result = new jfFIMCharacterFilter();
    ctag_result->SetToEmpty();
    result = ctag_result;
  }
  else if (typestring=="FIMContentTypeFilte") {
    xtag_result = new jfFIMContentTypeFilter();
    xtag_result->SetToEmpty();
    result = xtag_result;
  }
  else if (typestring=="FIMWarningFilter") {
    wtag_result = new jfFIMWarningsFilter();
    wtag_result->SetToEmpty();
    result = wtag_result;
  }
  else if (typestring=="FimRatingFilter") result = new jfFimRatingFilter();
  else if (typestring=="ExtraTagFilter") result = new jfExtraTagFilter();
  else if (typestring=="AO3FandomFilter") result = new jfAO3FandomFilter();
  else if (typestring=="AO3RatingFilter") result = new jfAO3RatingFilter();
  else if (typestring=="AO3OrientationFilter") {
      otag_result = new jfAO3OrientationFilter();
      otag_result->SetToEmpty();
      result = otag_result;
  }
  else if (typestring=="AO3PairFilter") result = new jfAO3PairFilter();
  else if (typestring=="AO3WarnFilter") result = new jfAO3WarnFilter();
  else if (typestring=="AO3KudoCountFilter") result = new jfAO3KudoFilter();
  else if (typestring=="FFNFavsFilter") result = new jfFFNFavsFilter();
  else if (typestring=="FimThumbPercentFilter") result = new jfFimThumbPercentFilter();
  else if (typestring=="FIMGroupSCFilter") result = new jfFIMGroupSCFilter();
  else if (typestring=="FIMGroupMCFilter") result = new jfFIMGroupMCFilter();
  else if (typestring=="FIMShortDescFilter") result = new jfFIM_ShortDesc_ExprFilter();
  else if (typestring=="FIMDualDescFilter") result = new jfFIM_DualDesc_ExprFilter();
  else result = NULL;
  // setting the name
  /**/JDEBUGLOGS("MakeFilter",1,newname);
  if (result!=NULL) result->SetName(newname);
  // done
  return result;
}
//=============================================================================
// returns the name of the map
QString jfFilterPointer::GetMapName() const {
  return filtermap->GetName();
}
//----------------------------------------------------------------------------
// checks for filter names
bool jfFilterPointer::CheckForName(const QString& fname) const {
  return filtermap->NameExists(fname);
}
//--------------------------------------------------------------------------
// renames an element
bool jfFilterPointer::RenameElem(const QString& newname) {
  QString oldname;
  bool xtemp;
  oldname = filter->GetName();
  // first, we run the 'do nothing' check
  if (newname == oldname) return false;
  // next, we run the invalud checks
  if (newname.isEmpty()) return false;
  if (CheckForName(newname)) return false;
  // we go ahead with the renameing
  filtermap->RenameElem(xtemp,oldname,newname);
  assert(xtemp);
  return true;
}
//============================================================================
bool jfMapOfFiltermaps::NameExists(const QString& mapname) const {
  // local variables
  jfMapOfFiltermaps::const_iterator xindex;
  // starting
  if (!empty()) {
    xindex = find(mapname);
    if (xindex != end()) return true;
  }
  return false;
}
//----------------------------------------------------------------------------
// produces a pointer to a specific filter
jfFilterPointer* jfMapOfFiltermaps::GetPointer(const QString& mapname, const QString& filtername) {
  // local variables
  jfFilterPointer* result;
  jfMapOfFiltermaps::const_iterator xindex;
  jfFilterMap* map_ptr;
  // testing for not found conditions, and doing intermediate steps
  if (empty()) return NULL;
  xindex = find(mapname);
  if (xindex == end()) return NULL;
  map_ptr = ((*xindex).second);
  if (!(map_ptr->NameExists(filtername))) return NULL;
  // building the result
  result = new jfFilterPointer();
  result->filter = map_ptr->GetItem(filtername);
  result->filtermap = map_ptr;
  result->mapomaps = this;
  // done
  return result;
}
//-----------------------------------------------------------------------------
QStringList* jfMapOfFiltermaps::GetMapNames() const {
  // local varibales
  QStringList* res;
  jfMapOfFiltermaps::const_iterator xindex;
  // processing
  res = NULL;
  if (!empty()) {
    res = new QStringList();
    xindex = begin();
    // the name extraction loop
    while (xindex!=end()) {
      res->append((*xindex).first);
      xindex++;
    }
  }
  return res;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfMapOfFiltermaps::FindMapIndex(const QString& mapname, size_t& newindex) const {
  jfMapOfFiltermaps::const_iterator positem;
  // we start...
  positem = find(mapname);
  if (positem==end()) return false;
  // looping to find the index
  positem = begin();
  newindex = 0;
  while((positem->first)!=mapname) {
    positem++;
    newindex++;
  }
  return true;
}
//----------------------------------------------------------------------------
// some other special methods
bool jfMapOfFiltermaps::RenameMap(const QString& oldname, const QString& newname, size_t& newindex) {
  jfMapOfFiltermaps::iterator positem;
  jfFilterMap* themap;
  // a check of the newname
  if (newname.isEmpty()) return false;
  // first, we have to find the map with oldname
  positem = find(oldname);
  if (positem==end()) return false;
  assert((positem->second)!=NULL);
  // if found, we save it
  themap = positem->second;
  // we only need to do the change if the names are unequal
  if (oldname!=newname) {
    // we check to see if newname already exists
    if (NameExists(newname)) return false;
    // otherwise, we go ahead...
    erase(oldname);           // erasing the old
    themap->SetName(newname); // changing the name
    (*this)[newname] = themap;       // inserting the new
  }
  // done
  return FindMapIndex(newname,newindex);
}
//=============================================================================

void jfMapOfFiltermaps::WriteToFile(const QString& filename) const {
  // local variables
  QTextStream* outfile;
  jfMapOfFiltermaps::const_iterator xindex;
  jfFilterMap *cmap;
  // we start
  outfile = GetOpenFile(filename,true);
  /* we now iterate over all of the maps, calling the file i/o function
  for each map */
  xindex = begin();
  while (xindex!=end()) {
    cmap = (*xindex).second;
    cmap->AddToFile(outfile);
    xindex++;
  }
  // now that we are done, we finish writing
  CloseFileStream(outfile,true);
  // clearing up
  delete outfile;
}
//-------------------------------------------------------------------------
bool jfMapOfFiltermaps::ReadFromFile(const QString& filename) {
  // constants
  const QString fname = "jfMapOfFiltermaps::ReadFromFile";
  // local variables
  jfFileReader* infile;
  jfFilterMap *cmap;
  bool iores;
  size_t mcount;
  QMessageBox msgBox;
  // we prepare for loading...
  /**/JDEBUGLOGS(fname,1,filename)
  infile = new jfFileReader(filename);
  // if the file does not exist, we do nothing
  if (!infile->OpenFile()) {
    delete infile;
    /**/JDEBUGLOG(fname,2)
    return false;
  }
  // the infile does exist
  else {
    if (!empty()) EmptyMaps();
    mcount = 0;
    /**/JDEBUGLOG(fname,3)
    while (!infile->AtEnd()) {
      // trying the read the map
      cmap = new jfFilterMap();
      iores = cmap->GetFromFile(infile);
      mcount++;
      /**/JDEBUGLOGST(fname,4,mcount)
      if (!iores) {
        /**/JDEBUGLOGS(fname,5,infile->err)
        msgBox.setText(infile->err);
        msgBox.exec();
        delete cmap;
        infile->CloseFile();
        delete infile;
        return false;
      }
      // the read worked!
      /**/JDEBUGLOGST(fname,6,mcount)
      (*this)[cmap->GetName()] = cmap;
    }
    infile->CloseFile();
    delete infile;
    /**/JDEBUGLOG(fname,7)
    return true;
  }
}

//==========================================================================
int jfMapOfFiltermaps::EmptyMaps() {
  // local variables
  int count = 0;
  jfMapOfFiltermaps::iterator xindex;
  // starting to check
  if (!empty()) {
    // we prepare for iteration
    xindex = begin();
    // we delete each map
    while (xindex!=end()) {
      delete (*xindex).second;
      (*xindex).second = NULL;
      count++;
      xindex++;
    }
    clear();
  }
  return count;
}
/////////////////////////////////////////////////////////////////////////////
