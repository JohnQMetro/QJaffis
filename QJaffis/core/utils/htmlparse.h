/*****************************************************************************
Name    : htmlparse.h
Basic   : Defines some utility functions and things used in manipulating html
Author  : John Q Metro
Started : September 13, 2009
Conversion to QT started : March 9, 2013
Updated : August 6, 2015 (added ConvertNewlines)

******************************************************************************/
#ifndef HTMLPARSE_H_INCLUDED
#define HTMLPARSE_H_INCLUDED
#endif // HTMLPARSE_H_INCLUDED
//------------------
#ifndef UTILS2_H_INCLUDED
  #include "utils2.h"
#endif // UTILS2_H_INCLUDED
//----------------
#include <QString>
#include <QStringList>
/*****************************************************************************/
namespace htmlparse {
  const QString entity_fname = "config/html_entities.dat";
  extern QStringList* entity_names;
  extern QString entity_chars;
  extern bool loaded;
  // data loading
  bool LoadEntities();
  // parsing
  bool EntityToChar(QString inval, QChar& outval);
  QString ConvertNewlines(const QString& inval);
  QString StripTags(const QString& inval);
  QString ConvertEntities(const QString& inval, bool keephtml);
  QString HTML2Text(const QString& inval);
  QString ToUTFHtml(const QString& inval);
  bool TestEntity(const QString& inval);
  QString Text2BasicHTML(const QString& inval);
  // escaping
  QString GoogleEscape(const QString& instring);
  // some character conversion stuff
  /* it seems that some websites mix utf-8 encoding with windows-1252 characters,
  and Qt Does not convert them automatically) */
  void Win1252_ToUnicode(QString& result);

}
/*****************************************************************************/
