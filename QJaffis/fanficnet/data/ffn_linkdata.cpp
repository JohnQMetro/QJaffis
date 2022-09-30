/******************************************************************************
Name    :   ffn_linkdata.cpp
Author  :   John Q Metro
Purpose :   Constants used to help build a Fanfiction.Net url
Created :   October 26, 2012
// Conversion to Qt Started July 10, 2014
Updated :   February 8, 2018
******************************************************************************/
#ifndef FFN_LINKDATA_H_INCLUDED
  #include "ffn_linkdata.h"
#endif // FFN_LINKDATA_H_INCLUDED
//-------------------------------
#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
//*******************************************************************
// fiction rating
size_t ffn_consts::fr_idlist[frcount] = {10,3,2,1,12,13,14};
const QString ffn_consts::fr_names[frcount] = {"Any","K \u2192 T","K and K+","K","K+","T","M"};
const QStringList* ffn_consts::fr_list;
// genre
size_t ffn_consts::gen_idlist[gencount] = {0,6,10,18,4,19,14,21,1,8,3,20,7,9,5,2,13,15,11,12,16,17};
const QString ffn_consts::gen_names[gencount] = {"All", "Adventure","Angst", "Crime", "Drama", "Family",
                        "Fantasy", "Friendship", "General", "Horror", "Humor", "Hurt/Comfort", "Mystery",
                        "Parody", "Poetry", "Romance", "Sci-Fi", "Spiritual", "Supernatural", "Suspense",
                        "Tragedy", "Western"};
const QStringList* ffn_consts::gen_list;
const QStringList* ffn_consts::taggen_list;
// wordcount
size_t ffn_consts::wc_idlist[wccount] = {0,11,51,1,5,10,20,40,60,100};
const QString ffn_consts::wc_names[wccount] = {"Any","less than 1000","less than 5000",
                "more than 1000","more than 5000","more than 10,000","more than 20,000",
                "more than 40,000","more than 60,000","more than 100,000"};
const QStringList* ffn_consts::wc_list;
// wordcount
size_t ffn_consts::lan_idlist[lancount] = {0,1,2,3,4,10,8,32,13};
const QString ffn_consts::lan_names[lancount] = {"Any","English","Spanish","French",
              "German","Russian","Portuguese","Indonesian","Polish"};
const QStringList* ffn_consts::lan_list;
//==============================================================================
// methods
QString ffn_consts::MakeURLPart(const size_t invalues[]) {
  QString result;
  result  = "&r="   + QString::number(invalues[0]);
  result += "&g1="  + QString::number(invalues[1]);
  result += "&len=" + QString::number(invalues[2]);
  result += "&lan=" + QString::number(invalues[3]);
  result += "&s="   + QString::number(invalues[4]);
  if (invalues[5] != 0) {
      result += "&_g1=" + QString::number(invalues[5]);
  }
  return result;
}
//---------------------------------------------------------------------------
bool ffn_consts::ReverseLookup(size_t ipart, size_t inid, size_t& outidx) {
  size_t qval;
  if (ipart == 5) ipart = 1;
  if (ipart>=5) return false;
  if (ipart==0) {
    for (qval=0;qval<frcount;qval++) {
      if (fr_idlist[qval]==inid) {
        outidx = qval;
        return true;
      }
    }
  }
  else if (ipart==1) {
    for (qval=0;qval<gencount;qval++) {
      if (gen_idlist[qval]==inid) {
        outidx = qval;
        return true;
      }
    }
  }
  else if (ipart==2) {
    for (qval=0;qval<wccount;qval++) {
      if (wc_idlist[qval]==inid) {
        outidx = qval;
        return true;
      }
    }
  }
  else if (ipart==3) {
    for (qval=0;qval<lancount;qval++) {
      if (lan_idlist[qval]==inid) {
        outidx = qval;
        return true;
      }
    }
  }
  else if (ipart==4) {
    if (inid>=3) return false;
    outidx = inid;
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
void ffn_consts::SetupLists() {
  size_t idx;
  fr_list = new QStringList();
  for (idx=0;idx<frcount;idx++) (const_cast<QStringList*>(fr_list))->append(fr_names[idx]);
  gen_list = new QStringList();
  taggen_list = new QStringList();
  for (idx=0;idx<gencount;idx++) {
    (const_cast<QStringList*>(gen_list))->append(gen_names[idx]);
    if (idx!=0) (const_cast<QStringList*>(taggen_list))->append(gen_names[idx]);
  }
  wc_list = new QStringList();
  for (idx=0;idx<wccount;idx++) (const_cast<QStringList*>(wc_list))->append(wc_names[idx]);
  lan_list = new QStringList();
  for (idx=0;idx<lancount;idx++) (const_cast<QStringList*>(lan_list))->append(lan_names[idx]);
}
//*****************************************************************************
