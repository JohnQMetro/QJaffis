/////////////////////////////////////////////////////////////////////////////
// Name:        filter_globals.cpp
// Author :     John Q Metro
// Purpose :    Defines global filter constants & stuff
// Created:     30.07.06
// Conversion to QT Started April 11, 2013
// Updated:     January 21, 2018
/////////////////////////////////////////////////////////////////////////////
// headers
// ----------------------------------------------------------------------------
#ifndef JFILTERGLOBALS
  #include "filter_globals.h"
#endif
#ifndef BASECOLLECT_H_INCLUDED
  #include "../structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
//***************************************************************************
namespace filterstrd {
  const QString newfimfils = QString("FimThumbsFilter,FIMGenreFilter,FIMCharacterFilter,") +
        QString("FimRatingFilter,FimThumbPercentFilter,FIMShortDescFilter,") +
          "FIMDualDescFilter,FIMContentTypeFilter,FIMWarningFilter";
  const QString newao3fils = QString("ExtraTagFilter,AO3FandomFilter,AO3RatingFilter,") +
        "AO3OrientationFilter,AO3PairFilter,AO3WarnFilter,AO3KudoCountFilter";
  const QString fimgfils = "FIMGroupSCFilter,FIMGroupMCFilter";
  // filters compatible with jfBasePD
  const QString basepd_fc = "ExprFilter";
  // filters compatible with jfGoogleItem
  const QString google_fc = basepd_fc + "," + "UrlFilter";
  // filters compatible with jfGenericFanfic
  const QString genfic_fc = basepd_fc + "," + "AuthorFilter,UpdatedDateFilter" +
      ",PairFilterSingle,PairFilterList,PairFilterMultiple";
  // filters compatible with jfGenericFanfic2
  const QString genfic2_fc = genfic_fc + "," + "WordCountFilter,CompletedFilter";
  // filters compatible with jfFFNItem
  const QString ffnfic_fc = genfic2_fc + "," + "CharacterFilter,CategoryFilter," +
    "FFNTagFilter,LanguageFilter,FFNRatingFilter,PublishedDateFilter,FFNFavsFilter";
  // filters compatible with jfFIM_Fanfic
  const QString fimfic_fc = genfic2_fc + "," + newfimfils;
  // filters compatible with jfAO3_Fanfic
  const QString ao3fic_fc = genfic2_fc + "," + "CharacterFilter" + ","  + newao3fils;
  // filters compatible with jfFIMGroup
  const QString fimgroup_fc = basepd_fc + "," + fimgfils;
  // all filters
  const QString all_filters = ffnfic_fc + "," + "UrlFilter," + "," + newfimfils; // + fimgfils
}
//===========================================================================

jfMapOfFiltermaps* filterglobals::global_filters = NULL;
QStringList* filterglobals::fclist[filterglobals::ITEM_NLIST] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
//----------------------------------------------------------------------------
// initializing globals
bool filterglobals::LoadStrings() {
  // constants
  const QString fname = "filterglobals::LoadStrings";
  // initial cleaning
  size_t loopc;
  for (loopc=0;loopc<filterglobals::ITEM_NLIST;loopc++) {
    if (fclist[loopc]!=NULL) delete fclist[loopc];
  }
  // creating the list of list of filters
  fclist[0] = ListToStrArr(filterstrd::all_filters,',');
  fclist[1] = ListToStrArr(filterstrd::basepd_fc,',');
  fclist[2] = ListToStrArr(filterstrd::google_fc,',');
  fclist[3] = ListToStrArr(filterstrd::genfic_fc,',');
  fclist[4] = ListToStrArr(filterstrd::genfic2_fc,',');
  fclist[5] = ListToStrArr(filterstrd::ffnfic_fc,',');
  fclist[6] = ListToStrArr(filterstrd::fimfic_fc,',');
  fclist[7] = ListToStrArr(filterstrd::ao3fic_fc,',');
  fclist[8] = ListToStrArr(filterstrd::fimgroup_fc,',');
  // emptying the global filters if need be
  if (global_filters!=NULL) {
    global_filters->EmptyMaps();
    delete global_filters;
  }
  // loading up the globals
  global_filters = new jfMapOfFiltermaps();
  return global_filters->ReadFromFile(fnamex);
}
//--------------------------------------------------------------------------
void filterglobals::Shutdown() {
  // local variables
  size_t loopc;
  // removing the list of filterable object type names
  for (loopc=0;loopc<filterglobals::ITEM_NLIST;loopc++) {
    if (fclist[loopc]!=NULL) {
      delete fclist[loopc];
      fclist[loopc] = NULL;
    }
  }
  // writing to the outfile
  global_filters->WriteToFile(fnamex);
  global_filters->EmptyMaps();
  delete global_filters;
  global_filters = NULL;
}

/////////////////////////////////////////////////////////////////////////////
