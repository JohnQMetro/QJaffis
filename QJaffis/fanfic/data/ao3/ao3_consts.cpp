/******************************************************************************
Name    :   ao3_consts.cpp
Author  :   John Q Metro
Purpose :   Defines some constants and utility functions for AO3
Created :   September 8, 2014
Conversion to Qt started October 15, 2014
Updated :   June 20, 2017 (added english-only option)
******************************************************************************/
#ifndef AO3_CONSTS_H_INCLUDED
  #include "ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED
//-----------------------------------
#include <QByteArray>


//*****************************************************************************
const QString ao3con::rating_ac = "GTME_";
const QString ao3con::orient_ac = "FGHMOS_";
const QString ao3con::warn1_ac = "W_NE";
const QString ao3con::warn2_ac = "VDRP";
const QChar ao3con::unchar = '_';
//-------------------------------------------
const size_t ao3con::rat_count = 6;
const QString ao3con::rat_labels[ao3con::rat_count] = {"Any", "General Audiences",
        "Teen And Up","Mature","Explicit","Not Rated"};
const QString ao3con::rat_ids[ao3con::rat_count-1] = {"10","11","12","13","9"};
 QStringList* ao3con::rat_list = NULL;
//-------------------------------------------
const size_t ao3con::soc_count = 7;
const QString ao3con::soc_labels[ao3con::soc_count]  = {"Any","Gen","F/M", "M/M",
                                                        "F/F","Multi","Other"};
const QString ao3con::soc_ids[ao3con::soc_count-1]  = {"21","22","23","116","2246","24"};
 QStringList* ao3con::soc_list = NULL;

//-------------------------------------------
const size_t ao3con::warn_count = 7;
const QString ao3con::warn_labels[ao3con::warn_count]  = {"Any","No Warnings","Violence",
                       "Character Death","Chose No Warnings","Rape","Underage"};
const QString ao3con::warn_ids[ao3con::warn_count-1]  = {"16","17","18","14","19","20"};
 QStringList* ao3con::warn_list = NULL;
//-------------------------------------------
const size_t ao3con::order_count = 9;
const QString ao3con::order_labels[ao3con::order_count]  = {"Date Posted","Date Updated"
               ,"Author","Title","Word Count","Hits","Kudos","Comments","Bookmarks"};
const QString ao3con::order_ids[ao3con::order_count]  = {"created_at","revised_at",
        "authors_to_sort_on","title_to_sort_on","word_count","hits","kudos_count",
        "comments_count","bookmarks_count"};
 QStringList* ao3con::order_list = NULL;
//-------------------------------------------
const QString ao3con::url_start = "https://archiveofourown.org/works?utf8=%E2%9C%93&commit=Sort+and+Filter";
//-------------------------------------------
QString ao3con::MakeUrlPart(size_t rindex, size_t sindex, size_t windex, QString intag, bool conly, bool ieng_only) {
  if (rindex>rat_count) return "";
  if (sindex>soc_count) return "";
  if (windex>warn_count) return "";
  // starting
  QString resval;
  // building
  if (rindex!=0) resval = "&work_search[rating_ids][]=" + rat_ids[rindex-1];
  if (sindex!=0) resval += "&work_search[category_ids][]=" + soc_ids[sindex-1];
  if (windex!=0) resval += "&work_search[warning_ids][]=" + warn_ids[windex-1];
  if (!intag.isEmpty()) {
    resval += "&work_search[other_tag_names]=" + AO3Encode(intag);
  }
  if (ieng_only) resval += "&work_search[language_id]=1";
  if (conly) resval += "&work_search[complete]=1";
  return resval;
}
//-----------------------------------------------------------------------
/* the a03 process for &tag_id= seems to be :
-Take the name, replace & with *a*, . with *d*, ? with *q*, and / with *s* .
-Convert to to utf-8 and then treat each byte separatly.
-Characters of 128 or more, as well as selected characters less than than, are %
encoded. Selected characters are %<>#{}\^`';/:@=$+()"
-Spaces are replaced with +
*/
QString ao3con::AO3Encode(const QString inval) {
  // constants
  const QByteArray dont_encode = QByteArray("!&*,/?[]| ");
  const QString pesc_chars = "%<>#{}\\^`';/:@=$+()\"";
  // special case
  if (inval.isEmpty()) return inval;
  // variables
  QString resval;
  QByteArray temres;
  QByteArray mbcopy;
  // conversion
  mbcopy = inval.toUtf8();
  temres = mbcopy.toPercentEncoding(dont_encode);
  // special replacements
  resval = QString(temres);
  resval.replace('&',"*a*");
  resval.replace('.',"*d*");
  resval.replace('?',"*q*");
  resval.replace('/',"*s*");
  resval.replace(' ',"+");
  // done with the loop
  return resval;
}
//----------------------------------------------------------------------
void ao3con::SetupLists() {
  size_t idx_l;
  rat_list = new QStringList();
  for (idx_l = 0; idx_l<rat_count;idx_l++) rat_list->append(rat_labels[idx_l]);
  soc_list = new QStringList();
  for (idx_l = 0; idx_l<soc_count;idx_l++) soc_list->append(soc_labels[idx_l]);
  warn_list = new QStringList();
  for (idx_l = 0; idx_l<warn_count;idx_l++) warn_list->append(warn_labels[idx_l]);
  order_list = new QStringList();
  for (idx_l = 0; idx_l<order_count;idx_l++) order_list->append(order_labels[idx_l]);
}

//*****************************************************************************

