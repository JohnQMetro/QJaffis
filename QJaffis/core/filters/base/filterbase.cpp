/////////////////////////////////////////////////////////////////////////////
// Name:        filterbase.cpp
// Author :     John Q Metro
// Purpose :    Defines some filtering classes
// Created:     08.02.06
// Conversion to QT Started April 8, 2013
// Updated:     August 22, 2012
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifndef JFBASEFILTER
  #include "filterbase.h"
#endif
#ifndef UTILS1_H_INCLUDED
  #include "../../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../utils/logging.h"
#endif // LOGGING_H_INCLUDED
//-------------------------
#include <assert.h>
//*****************************************************************************
//=============================================================================
jfBaseFilter::jfBaseFilter():jfBaseObj() {
  name = "FilterBase Default Name";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfBaseFilter::MatchTypeid(const QString& tomatch) const {
  return (tomatch==GetTypeID());
}
//-----------------------------------------------------------------------------
// returns a general filter type. The default is 0, unless overriden
size_t jfBaseFilter::GetFilType() const { return 0;}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfBaseFilter::Match(const jfBasePD* testelem) const {
  assert(testelem!=NULL);
  if (isEmpty()) {
		return true;
	}
  return CoreMatch(testelem);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// subitem copying method, so we do not have to replicate for each child class
void jfBaseFilter::CopyOver(jfBaseFilter* core) const {
  assert(core!=NULL);
  // copying over what infomration we have defined
  core->name = name + ", copy";
  core->description = description;
  core->num_id = num_id;
  core->validdata = validdata;
}
//---------------------------------------------------------------------------
// the only thing in line 3 is the ExtraLines info
bool jfBaseFilter::AddMiddleToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // adding
  buffer = QString::number(ExtraLines());
  (*outfile) << buffer << '\n';
  return true;
}
//---------------------------------------------------------------------------
/* we don't actually do anything here other than skip a line... the number of
 lines is determined by looking at type info */
bool jfBaseFilter::ReadMiddleFromFile(jfFileReader* infile) {
  const QString fname = "jfBaseFilter::ReadMiddleFromFile";
  // checks
  assert(infile!=NULL);
  return infile->SkipLine(fname);
}
//=============================================================================
// the constructor
jfUrlFilter::jfUrlFilter():jfBaseFilter() {
  excludes = NULL;
  cimatch = false;
  excludeonly = true;
  alternates = NULL;
  https_incl = true;
}
//----------------------------------------------------------------------------
// a copy constructor
jfUrlFilter::jfUrlFilter(const jfUrlFilter& source):jfBaseFilter() {
  // copying the most basic items
  name = source.name + ", copy";
  description = source.description;
  validdata = source.validdata;
  // copying alternates
  if (source.alternates!=NULL) alternates = new QStringList(*(source.alternates));
  // copying some other stuff
  excludeonly = source.excludeonly;
  cimatch = source.cimatch;
  https_incl = source.https_incl;
  // copying the excludes
  if (source.excludes!=NULL) excludes = new QStringList(*(source.excludes));
  else excludes = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info methods
bool jfUrlFilter::isEmpty() const {
  bool atest,etest;
  etest = (excludes==NULL) || (excludes->isEmpty());
  atest = (alternates==NULL) || (alternates->isEmpty());
  return etest && atest;
}
//-----------------------------------------------------------------------------
/* converts a string version of the contents into the contents
The fields are:
1: case sensentivity
2: http include
3: number of alternates
<alternates>
<excludes>

*/
bool jfUrlFilter::FromString(const QString& sourcedata)  {
  // local variables
  QString ldata,buffer;
  jfLineParse* data;
  size_t altlen, tnum, aindex;
  // parses the source string
  // preliminary steps
  data = new jfLineParse(sourcedata);
  if (data==NULL) return false;
  tnum = data->Num();
  if (tnum<3) {
    delete data;
    return false;
  }
  // we can now proceed with trying to start the conversion
  if (!(data->BoolVal(0,cimatch))) {
    delete data;
    return false;
  }
  // the https value
  if (!(data->BoolVal(1,https_incl))) {
    delete data;
    return false;
  }
  // next up is the number of items in the alternate
  if (!(data->SIntVal(2,altlen))) {
    delete data;
    return false;
  }
  if (tnum<3+altlen) {
    delete data;
    return false;
  }
  if (altlen==0) {
    excludeonly = true;
    alternates = NULL;
  }
  else {
    excludeonly = false;
    // we extract the alternate items one by one
    alternates = data->GetMultiple(3,altlen);
  }
  // once done here, we get the exclude values
  // index calculation and special cases
  aindex = 3 + altlen;
  altlen = tnum - aindex;
  if (altlen==0) excludes = NULL;
  else excludes = data->GetRest(aindex);
  // done
  delete data;
  SetValid();
  return validdata;
}
//-----------------------------------------------------------------------------
// converts the contents into a semicolon-delimted string list
QString jfUrlFilter::ToString() const{
  QStringList *result;
  QString alist;
  // setting up the result and adding the first 3 values
  result = new QStringList();
  result->append(cimatch?("true"):("false"));
  result->append(https_incl?("true"):("false"));
  if (alternates==NULL) result->append("0");
  else result->append(QString::number(alternates->size()));
  // we now add the alternates
  if (alternates!=NULL) result->append(*alternates);
  // next up is the excludes
  if (excludes!=NULL) result->append(*excludes);
  // converts the array of strings into a string-list
  alist = StrArrToList(result,';');
  delete result;
  return alist;
}
//==============================================================================
size_t jfUrlFilter::SetAlternates(QStringList* mmatch, bool replace) {
  if (replace) {
    if (alternates!=NULL) delete alternates;
  }
  alternates = mmatch;
  SetValid();
  if (mmatch==NULL) {
    excludeonly = true;
    return 0;
  }
  else {
    excludeonly = false;
    return (mmatch->size());
  }
}
//-----------------------------------------------------------------------------
size_t jfUrlFilter::AppendToAlternates(QString url2add) {
  if (alternates==NULL) {
    if (url2add.isEmpty()) return 0;
    alternates = new QStringList();
    alternates->append(url2add);
    excludeonly = false;
  }
  else if (!url2add.isEmpty()) {
    alternates->append(url2add);
    excludeonly = false;
  }
  return alternates->size();
}
//-----------------------------------------------------------------------------
QStringList* jfUrlFilter::GetAlternates() const {
  if (alternates!=NULL) return new QStringList(*alternates);
  else return NULL;
}
// ----------------------------------------------------------------------------
// sets the exclude list
size_t jfUrlFilter::SetExcludes(QStringList* excl, bool replace) {
    const QString fname = "jfUrlFilter::SetExcludes";
  if (replace) {
    if (excludes!=NULL) delete excludes;
  }
  if (excl == NULL) {
     /**/JDEBUGLOGS(fname,1,"The excludes input was NULL.")
  }

  excludes = excl;
  SetValid();
  if (excl==NULL) return 0;
  else return (excl->size());
}
//-----------------------------------------------------------------------------
size_t jfUrlFilter::AppendToExcludes(QString url2add) {
  if (excludes==NULL) {
    if (url2add.isEmpty()) return 0;
    excludes = new QStringList();
    excludes->append(url2add);
  }
  else if (!url2add.isEmpty()) excludes->append(url2add);
  return excludes->size();
}
// ----------------------------------------------------------------------------
// returns the exclude list
QStringList* jfUrlFilter::GetExcludes() const{
  if (excludes!=NULL) return new QStringList(*excludes);
  else return NULL;
}
// ----------------------------------------------------------------------------
// returns the exclude list, even if it is NULL (creates an empty list then)
QStringList* jfUrlFilter::GetExcludes2() {
  if (excludes==NULL) excludes = new QStringList();
  return excludes;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic constants
// ----------------------------------------------------------------------------
QString jfUrlFilter::GetTypeID() const {
  return "UrlFilter";
}
// ----------------------------------------------------------------------------
QString jfUrlFilter::GetTypeDescription() const {
  return "The Url Filter matches strings using simple wilcards against \
the URL associated with the item. The match occurs if the main string matches\
, and none of the exclude strings match.";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting a copy of the filter
jfUrlFilter* jfUrlFilter::Copy() const {
  // local variables
  jfUrlFilter* res;
  // creating the result
  res = new jfUrlFilter(*this);
  return res;
}
//-----------------------------------------------------------------------------
jfBaseFilter* jfUrlFilter::GenCopy() const {
  return Copy();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// private methods
//-----------------------------------------------------------------------------
bool jfUrlFilter::CoreMatch(const jfBasePD* testelem) const {
  // checking
  assert(validdata);
  QRegExp matchbuf;
  // local varibales
  QString urlbuffer, mbuffer, buffer;
  bool ismatch = true;
  bool found = false;
  size_t x;
  Qt::CaseSensitivity qtcmatch;
  // starting...
  urlbuffer = testelem->GetUrl();
  qtcmatch = (cimatch)?(Qt::CaseInsensitive):(Qt::CaseSensitive);
  // alternates matching
  if (!excludeonly) {
    for (x=0;x<(alternates->size());x++) {
      // getting the item to match against
      mbuffer = alternates->at(x);
      buffer = "http://" + mbuffer;
      matchbuf = QRegExp(buffer,qtcmatch,QRegExp::WildcardUnix);
      // we start with the match
      if (matchbuf.exactMatch(urlbuffer)) {
        found = true;
        break;
      }
      if (https_incl) {
        buffer = "https://" + mbuffer;
        matchbuf = QRegExp(buffer,qtcmatch,QRegExp::WildcardUnix);
        if (matchbuf.exactMatch(urlbuffer)) {
          found = true;
          break;
        }
      }
      // next loop
    }
    // at the end of the loop, if found is false, the result is false
    if (!found) return false;
    // a short circut
    if (found && (excludes==NULL)) return true;
  }
  // testing the excludes
  for (x=0;x<(excludes->size());x++) {
    // getting the current exclude
    mbuffer = excludes->at(x);
    // testing
      buffer = "http://" + mbuffer;
      matchbuf = QRegExp(buffer,qtcmatch,QRegExp::WildcardUnix);
      if (matchbuf.exactMatch(urlbuffer)) {
        ismatch = false;
        break;
      }
      if (https_incl) {
        buffer = "https://" + mbuffer;
        matchbuf = QRegExp(buffer,qtcmatch,QRegExp::WildcardUnix);
        if (matchbuf.exactMatch(urlbuffer)) {
          ismatch = false;
          break;
        }
      }
  }
  // done!
  return ismatch;
}
// ----------------------------------------------------------------------------
// calculates if the data is valid
void jfUrlFilter::SetValid() {
  validdata = ((alternates!=NULL) || (excludes!=NULL));
}
// ----------------------------------------------------------------------------
size_t jfUrlFilter::ExtraLines() const {
  return 3;
}
// ----------------------------------------------------------------------------
bool jfUrlFilter::AddRestToFile(QTextStream* outfile) const {
  // variables
  jfOutString r_out;
  QString buffer;
  // checks
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 4
  r_out << excludeonly << cimatch << https_incl;
  (*outfile) << r_out << '\n';
  // line 5 is for the alternates
  if (alternates!=NULL) buffer+= StrArrToEscList(alternates, IO_DELIMS);
  (*outfile) << buffer << '\n';
  // line 6 is composed of all the excludes
  buffer.clear();
  if (excludes!=NULL) buffer+= StrArrToEscList(excludes, IO_DELIMS);
  (*outfile) << buffer << '\n';
  // done
  return true;
}
//---------------------------------------------------------------------------
/* reads in the 4th, 5th, and 6th lines */
bool jfUrlFilter::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfUrlFilter::ReadRestFromFile";
  const QString isnatv = " field is not a truth value!";
  // input data
  QString cline;
  QStringList* qres;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  // parsing the line
  if (!infile->ReadParseLine(3,funcname)) return false;
  // the first field must be a truth field
  if (!infile->lp.BoolVal(0,excludeonly)) return infile->BuildError("The exclude only"+isnatv);
  // the second field must also be a truth field
  if (!infile->lp.BoolVal(1,cimatch)) return infile->BuildError("The case sensetivity"+isnatv);
  // the third field must also be a truth field
  if (!infile->lp.BoolVal(2,https_incl)) return infile->BuildError("The https include"+isnatv);
  // the next line (line 5)
  if (!infile->ReadLine(cline)) return false;
  qres = EscListToStrArr(cline,IO_DELIMS);
  if ((qres==NULL) || (qres->isEmpty())) {
    excludeonly = true;
    alternates = NULL;
    if (qres!=NULL) delete qres;
  }
  else alternates = qres;
  // the next line (line 6)
  if (!infile->ReadLine(cline)) return false;
  qres = EscListToStrArr(cline,IO_DELIMS);
  if ((qres==NULL) || (qres->isEmpty())) {
    excludes = NULL;
    if (qres!=NULL) delete qres;
  }
  else excludes = qres;
  return true;
}
// ----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
