/******************************************************************************
// Name:        htmlskelton.cpp
// Author :     John Q Metro
// Purpose :    For filling in html i/o skeltons with output info
// Created:     November 15, 2009
// Conversion to Qt started March 10, 2013
// Updated:     November 15, 2009
******************************************************************************/
#ifndef HTMLSKELTON_H_INCLUDED
  #include "htmlskelton.h"
#endif // HTMLSKELTON_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef STRINGPARSER_H_INCLUDED
  #include "stringparser.h"
#endif // STRINGPARSER_H_INCLUDED

#include <assert.h>
//****************************************************************************/
// the constructor
jfSkeletonParser::jfSkeletonParser() {
  nametags_text = NULL;
  values_text = NULL;
  text_count = 0;
  // boolean items
  nametags_bool = NULL;
  values_bool = NULL;
  bool_count = 0;
  // integer items
  nametags_uint = NULL;
  values_uint = NULL;
  uint_count = 0;;
  // parsing stuff
  done = false;
  total_count = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data and parsing
//---------------------------------------------------------------------------
bool jfSkeletonParser::SetSkeleton(QString inskel) {
  src_skelton = inskel;
  done = false;
  return !inskel.isEmpty();
}
//---------------------------------------------------------------------------
bool jfSkeletonParser::ProduceIntermediate() {
  // consts
  const QString csn_st = "<<<&";
  const QString csn_ed = "&>>>";
  const QString cs_end = "<<&>>";
  // local variables
  jfStringParser* qparser;
  QString buffer,namebuffer;
  jfsp_DATATYPE ftype;
  size_t fpos;
  // starting
  qparser = new jfStringParser(src_skelton, true);
  int_result.clear();
  // the main loop
  while (qparser->GetMovePast(csn_st,buffer)) {
    int_result += buffer;
    // we find the nametag
    if (!qparser->GetMovePast(csn_ed,namebuffer)) {
      delete qparser;
      return false;
    }
    // we get the stuff within the conditional
    if (!qparser->GetMovePast(cs_end,buffer)) {
      delete qparser;
      return false;
    }
    // we, we check if we do add the conditional stuff
    if (!FindTag(namebuffer,ftype,fpos)) {
      delete qparser;
      return false;
    }
    // we've got the conditional, do we include?
    if (NotEmpty(ftype,fpos)) {
      int_result += buffer;
    }
  }
  // at the end of the loop, we add the non-conditional stuff at the end
  qparser->GetToEnd(buffer);
  int_result += buffer;
  // finishing off
  delete qparser;
  return true;
}
//---------------------------------------------------------------------------
/* we replace things with their values. Since logically, most tags will be
used, we use search and replace */
bool jfSkeletonParser::MakeResult() {
  // constants
  QString nstart = "<<#";
  QString ntend = "#>>";
  // local varibales
  size_t loopc;
  QString nametag,replacement;
  // starting
  dne_result = int_result;
  // the text loop
  for (loopc=0;loopc<text_count;loopc++) {
    // getting the tag and its replacemnt
    nametag = nstart + (nametags_text->at(loopc) + ntend);
    replacement = values_text->at(loopc);
    // doing search and replace
    dne_result.replace(nametag,replacement);
  }
  // the boolen loop
  for (loopc=0;loopc<bool_count;loopc++) {
    // getting the tag and its replacemnt
    nametag = nstart + (nametags_bool->at(loopc) + ntend);
    replacement = Bool2Str(values_bool->at(loopc));
    // doing search and replace
    dne_result.replace(nametag,replacement);
  }
  // the unsigned integer loop
  for (loopc=0;loopc<uint_count;loopc++) {
    // getting the tag and its replacemnt
    nametag = nstart + (nametags_uint->at(loopc) + ntend);
    replacement = QString::number(values_uint->at(loopc));
    // doing search and replace
    dne_result.replace(nametag,replacement);
  }
  // done
  return true;
}
//---------------------------------------------------------------------------
QString jfSkeletonParser::GetResult() const {
  return dne_result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// adding items
//---------------------------------------------------------------------------
bool jfSkeletonParser::AddText(QString nametag, const QString& value) {
  // local variables
  jfsp_DATATYPE ores;
  size_t oloc;
  // we create the arrays if we previously had none...
  if (text_count==0) {
    nametags_text = new QStringList();
    values_text = new QStringList();
  }
  // if we do, we have some checking to do.
  else {
    // we check if the name exists...
    if (FindTag(nametag,ores,oloc)) {
      // if it dowes, but it is not a text tag, fail!
      if (ores!=jfsk_TEXT) return false;
      // if it does, and is text, we change the value
      else {
        (*values_text)[oloc] = value;
        return true;
      }
    }
    // finding the tag failed, so we add... after the next closing brace
  }
  // adding
  nametags_text->append(nametag);
  values_text->append(value);
  text_count++;
  total_count++;
  return true;
}
//---------------------------------------------------------------------------
bool jfSkeletonParser::AddBool(QString nametag, const bool& value) {
  // local variables
  jfsp_DATATYPE ores;
  size_t oloc;
  // we create the arrays if we previously had none...
  if (bool_count==0) {
    nametags_bool = new QStringList();
    values_bool = new QVector<bool>;
  }
  // if we do, we have some checking to do.
  else {
    // we check if the name exists...
    if (FindTag(nametag,ores,oloc)) {
      // if it dowes, but it is not a text tag, fail!
      if (ores!=jfsk_BOOL) return false;
      // if it does, and is text, we change the value
      else {
        (*values_bool)[oloc] = value;
        return true;
      }
    }
    // finding the tag failed, so we add... after the next closing brace
  }
  // adding
  nametags_bool->append(nametag);
  values_bool->push_back(value);
  bool_count++;
  total_count++;
  return true;
}
//---------------------------------------------------------------------------
bool jfSkeletonParser::AddUInt(QString nametag, const size_t& value) {
  // local variables
  jfsp_DATATYPE ores;
  size_t oloc;
  // we create the arrays if we previously had none...
  if (uint_count==0) {
    nametags_uint = new QStringList();
    values_uint = new QVector<uint>;
  }
  // if we do, we have some checking to do.
  else {
    // we check if the name exists...
    if (FindTag(nametag,ores,oloc)) {
      // if it dowes, but it is not a text tag, fail!
      if (ores!=jfsk_UINT) return false;
      // if it does, and is text, we change the value
      else {
        (*values_uint)[oloc] = value;
        return true;
      }
    }
    // finding the tag failed, so we add... after the next closing brace
  }
  // adding
  nametags_uint->append(nametag);
  values_uint->push_back(value);
  uint_count++;
  total_count++;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfSkeletonParser::~jfSkeletonParser() {
  if (text_count!=0) {
    delete  nametags_text;
    delete values_text;
  }
  if (bool_count!=0) {
    delete nametags_bool;
    delete values_bool;
  }
  if (uint_count!=0) {
    delete nametags_uint;
    delete values_uint;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSkeletonParser::FindTag(QString nametag, jfsp_DATATYPE& ftype, size_t& fpos) const {
  // local variables
  size_t loopc;
  // searching text
  for (loopc=0;loopc<text_count;loopc++) {
    if ((nametags_text->at(loopc))==nametag) {
      ftype = jfsk_TEXT;
      fpos = loopc;
      return true;
    }
  }
  // searching bool
  for (loopc=0;loopc<bool_count;loopc++) {
    if ((nametags_bool->at(loopc))==nametag) {
      ftype = jfsk_BOOL;
      fpos = loopc;
      return true;
    }
  }
  // searching uint
  for (loopc=0;loopc<uint_count;loopc++) {
    if ((nametags_uint->at(loopc))==nametag) {
      ftype = jfsk_UINT;
      fpos = loopc;
      return true;
    }
  }
  // all failed
  return false;
}
//---------------------------------------------------------------------------
bool jfSkeletonParser::NotEmpty(jfsp_DATATYPE ftype, size_t fpos) const {
  if (ftype==jfsk_TEXT) {
    assert(fpos<text_count);
    QString examine = values_text->at(fpos);
    return !(examine.isEmpty());
  }
  else if (ftype==jfsk_BOOL) {
    assert(fpos<bool_count);
    return (*values_bool)[fpos];
  }
  else if (ftype==jfsk_UINT) {
    assert(fpos<uint_count);
    return (0!=((*values_uint)[fpos]));
  }
  else assert(false);
  return false;
}
//****************************************************************************/
