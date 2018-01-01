/******************************************************************************
Name    :   ffn_linkdata.h
Author  :   John Q Metro
Purpose :   Constants used to help build a Fanfiction.Net url
Created :   October 26, 2012
// Conversion to Qt Started July 10, 2014
Updated :   July 29, 2016
******************************************************************************/
#ifndef FFN_LINKDATA_H_INCLUDED
  #define FFN_LINKDATA_H_INCLUDED
#endif // FFN_LINKDATA_H_INCLUDED

#include <QString>
#include <QStringList>

//*******************************************************************
namespace ffn_consts {
  // fiction rating
  const size_t frcount = 7;
  extern size_t fr_idlist[];
  extern const QString fr_names[];
  extern const QStringList* fr_list;
  // genre
  const size_t gencount = 22;
  extern size_t gen_idlist[];
  extern const QString gen_names[];
  extern const QStringList* gen_list;
  extern const QStringList* taggen_list;
  // wordcount
  const size_t wccount = 10;
  extern size_t wc_idlist[];
  extern const QString wc_names[];
  extern const QStringList* wc_list;
  // language
  const size_t lancount = 9;
  extern size_t lan_idlist[];
  extern const QString lan_names[];
  extern const QStringList* lan_list;
  // methods
  QString MakeURLPart(const size_t invalues[]);
  bool ReverseLookup(size_t ipart, size_t inid, size_t& outidx);
  void SetupLists();
}
//*****************************************************************************
