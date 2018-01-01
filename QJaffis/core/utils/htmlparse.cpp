/*****************************************************************************
Name    : htmlparse.cpp
Basic   : Declares some utility functions and things used in manipulating html
Author  : John Q Metro
Started : September 13, 2009
Conversion to QT started : March 9, 2013
Updated : August 6, 2015

******************************************************************************/
#ifndef HTMLPARSE_H_INCLUDED
  #include "htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
#ifndef STRINGPARSER_H_INCLUDED
  #include "stringparser.h"
#endif // STRINGPARSER_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "utils3.h"
#endif // UTILS3_H_INCLUDED

#include <QTextStream>
#include <assert.h>

/*****************************************************************************/
// the namespace variables
QStringList* htmlparse::entity_names = NULL;
QString      htmlparse::entity_chars;
bool         htmlparse::loaded       = false;
//---------------------------------------------------------------------------
/* the purpose of this function is to load data from files about HTML entities */
bool htmlparse::LoadEntities() {
  // variables
  QString cline,buffer, oerr;
  jfStringParser* qparser;
  QTextStream* infile;
  ulong tval;
  bool hpres;
  // we start
  infile = GetOpenFile(entity_fname,false,"ISO 8859-1");
  if (infile==NULL) return false;
  // preparing for the loop
  if (htmlparse::loaded) {
    assert(htmlparse::entity_names!=NULL);
    delete htmlparse::entity_names;
    htmlparse::loaded = false;
    htmlparse::entity_chars.clear();
  }
  htmlparse::entity_names = new QStringList();
  qparser = new jfStringParser();
  // looping thru the data file, line by line
  while (!infile->atEnd()) {
    // getting the line and checking it
    cline = infile->readLine();
    qparser->ChangeData(cline);
    if (!(qparser->StartsWith("<!ENTITY"))) continue;
    // we have a line starting with entity! extract the name!
    hpres = qparser->GetDelimited("<!ENTITY","CDATA",buffer);
    assert(hpres);
    buffer = buffer.trimmed();
    htmlparse::entity_names->append(buffer);
    // next up is the numeric value
    hpres = qparser->GetDelimitedULong("\"&#",";\"",tval,oerr);
    assert(hpres);
    htmlparse::entity_chars+= QChar((uint)tval);
  }
  // clearing up some stuff
  delete qparser;
  CloseFileStream(infile,false);
  delete infile;
  // now that we are done, we check the results
  if (htmlparse::entity_names->count()==0) {
    delete htmlparse::entity_names;
    htmlparse::entity_names = NULL;
		return false;
  }
  else htmlparse::loaded = true;
	return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool htmlparse::EntityToChar(QString inval, QChar& outval) {
  int xfound;
  unsigned long tval;
  // a special case
  if (!htmlparse::loaded) return false;
  // we try to look up inval
  xfound = htmlparse::entity_names->indexOf(inval);
  // if found, we lookup the character
  if (xfound!=-1) {
    outval = htmlparse::entity_chars[xfound];
    return true;
  }
  // otherwise, we try to convert as a number
  else {
    // the character is supposed to start with #, but we'll ignore violations
    if (inval[0]=='#') inval = inval.mid(1);
    // hex or decimal?
    if (inval[0]=='x') {
      inval = inval.mid(1);
      if (!Str2ULong(inval,tval,true)) return false;
    }
    else if (!Str2ULong(inval,tval,false)) return false;
    outval = QChar((uint)tval);
  }
  return true;
}
//-----------------------------------------------------------------------------
// replaces certain tags with newlines
QString htmlparse::ConvertNewlines(const QString& inval) {
  QString result = inval;
  result.replace("<br>","\n");
  result.replace("<br />","\n");
  result.replace("<br/>","\n");
  result.replace("</p>","\n");
  return result;
}

//-----------------------------------------------------------------------------
QString htmlparse::StripTags(const QString& inval) {
  // variables
  jfStringParser* qparser;
  QString result,buf;
  size_t qindex;
  size_t which;
  bool sbfound = true;
  // starting
  qparser = new jfStringParser(inval);
  // there is a posibility we start in a tag
  if (!qparser->Find1stPair("<",">",which,qindex)) {
    delete qparser;
    return inval;
  }
  // if which = 2, we start in a tag
  if (which==2) qparser->MovePast(">");
  // now, the loop
  while (qparser->GetMovePast("<",buf)) {
    result += buf;
    sbfound = qparser->MovePast(">");
  }
  // now, we might have to tack on the end...
  if (sbfound) {
    if (qparser->GetToEnd(buf)) result +=buf;
  }
  delete qparser;
  return result;
}
//------------------------------------------------------
QString htmlparse::ConvertEntities(const QString& inval, bool keephtml) {
  // variables
  jfStringParser* qparser;
  QString result,buf,ebuf;
  QChar tent;
  // starting
  qparser = new jfStringParser(inval);
  /* unlike tags, ends of entities are no escaped, so we
  ignore the possibility of starting and ending in an entity */
  while (qparser->GetMovePast("&",buf)) {
    result += buf;
    if (!qparser->GetMovePast(";",ebuf)) {
      // here, we just add the rest
      result += "&";
      break;
    }
    if (keephtml && TestEntity(ebuf)) {
      result += "&" + ebuf + ";";
    }
    else {
      // if entity extraction works, we convert
      if (EntityToChar(ebuf,tent)) result += tent;
      else result += "?";
    }
  }
  // done
  if (qparser->GetToEnd(buf)) result += buf;
  delete qparser;
  return result;
}
//------------------------------------------------------------------
QString htmlparse::HTML2Text(const QString& inval) {
  QString resval;
  resval = ConvertNewlines(inval);
  resval = StripTags(resval);
  resval = ConvertEntities(resval,false);
  return resval.trimmed();
}
//------------------------------------------------------------------
/* manipulates html input by converting all entities except the minimum
 to characters, basically to UTF-8 */
QString htmlparse::ToUTFHtml(const QString& inval) {
  // variables
  QString result;
  result = StripTags(inval);
  result = ConvertEntities(result,true);
	// finishing...
  return result;
}
//-----------------------------------------------------------------
/* in converting to utf8 html, most entities can be converted to characters,
 but some never should be. This function test if the inval is the latter */
bool htmlparse::TestEntity(const QString& inval) {
  if ((inval=="lt") || (inval=="lt") || (inval=="amp")) return true;
  if ((inval=="38") || (inval=="60") || (inval=="62")) return true;
	else return false;
}
//----------------------------------------------------------------------
QString htmlparse::Text2BasicHTML(const QString& inval) {
  QString result = inval;
  result.replace("&","&amp;");
  result.replace("<","&lt;");
  result.replace(">","&gt;");
  result.replace("\n","<br>\n");
  return result;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString htmlparse::GoogleEscape(const QString& instring) {
  QString Result;
  Result = instring;
  Result.replace(" ","+");
  Result = QueryEncode(Result);
  return Result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* it seems that some websites mix utf-8 encoding with windows-1252 characters,
and Qt Does not convert them automatically) */
void htmlparse::Win1252_ToUnicode(QString& result) {
  result.replace(QChar(0x92),QChar(0x2019));
  result.replace(QChar((uint)0x93),QChar((uint)0x201C));
  result.replace(QChar(0x94),QChar(0x201D));
}

/*****************************************************************************/
