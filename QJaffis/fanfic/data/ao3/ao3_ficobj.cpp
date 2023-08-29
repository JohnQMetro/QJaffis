/******************************************************************************
Name    :   ao3_ficobj.cpp
Author  :   John Q Metro
Purpose :   Defines fanfic object data of archoveofourown.org
Created :   August 26, 2012
Conversion to Qt Started September 28, 2013
Updated :   March 18, 2023
******************************************************************************/
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED
//---------------------------------------
#ifndef FICBASE_H
  #include "../ficbase.h"
#endif // FICBASE_H
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef AO3_CONSTS_H_INCLUDED
  #include "ao3_consts.h"
#endif // AO3_CONSTS_H_INCLUDED


#ifndef DISPLAYHTMLSPEC_H
    #include "../displayhtmlspec.h"
#endif // DISPLAYHTMLSPEC_
//-------------------------------------
#include <assert.h>
#include <QRegExp>
#include <QRegularExpression>
//**************************************************************************
const QString& IdForAO3Fanfic() {
    static QString AO3_FANFIC_TYPE_ID = QString("AO3Fanfic");
    return AO3_FANFIC_TYPE_ID;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// default constructors
//----------------------------
jfAO3Fanfic::jfAO3Fanfic():jfGenericFanfic2() {
    type_labels.append(IdForFanficPairs());
    type_labels.append(IdForAO3Fanfic());

    rating = warn = ao3con::unchar;
    eccount = -1;
    kudcount = 0;
    series_index = 0;
}
// --------------------------
jfAO3Fanfic::jfAO3Fanfic(const jfSearchResultItemData& init_data):jfGenericFanfic2(init_data) {
    type_labels.append(IdForFanficPairs());
    type_labels.append(IdForAO3Fanfic());

    rating = warn = ao3con::unchar;
    eccount = -1;
    kudcount = 0;
    series_index = 0;
}
//----------------------------
jfAO3Fanfic::jfAO3Fanfic(const jfAO3Fanfic& src):jfGenericFanfic2(src) {
    type_labels.append(IdForFanficPairs());
    type_labels.append(IdForAO3Fanfic());

    rating = src.rating;
    orientations = src.orientations;
    warn = src.warn;
    eccount = src.eccount;
    kudcount = src.kudcount;
    warntags = src.warntags;
    extratags = src.extratags;
    // categories and series
    series_index = src.series_index;
    series_name = src.series_name;
    series_url = src.series_url;
    cats.assign(src.cats.begin(),src.cats.end());
    RelationshipCopy(src);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting info
//----------------------------
QChar jfAO3Fanfic::GetRating() const { return rating; }
//----------------------------
bool jfAO3Fanfic::MultipleOrientations() const {
    return orientations.contains(",");
}
//----------------------------
const QString& jfAO3Fanfic::GetOrientations() const { return orientations; }
//----------------------------
QChar jfAO3Fanfic::GetWarnflag() const { return warn; }
//----------------------------
const QString& jfAO3Fanfic::GetWarntag() const { return warntags;}
//----------------------------
bool jfAO3Fanfic::TestWarntag(QChar inval) const {
  return warntags.contains(inval);
}
//----------------------------
int jfAO3Fanfic::GetEstPartCount() const { return eccount; }
//----------------------------
size_t jfAO3Fanfic::GetKudos() const { return kudcount; }
//----------------------------
const QStringList& jfAO3Fanfic::GetExtraTags() const { return extratags; }
//----------------------------
QString jfAO3Fanfic::GetJoinedExtraTags() const {
    return extratags.join(',');
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfAO3Fanfic::InSeries() const { return (series_index!=0); }
//----------------------------
size_t jfAO3Fanfic::SeriesIndex() const { return series_index; }
//----------------------------
QString jfAO3Fanfic::SeriesName() const { return series_name; }
//----------------------------
QString jfAO3Fanfic::SeriesUrl() const { return series_url; }
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfAO3Fanfic::GetFandoms() const {
  QString resval;
  for (size_t xl = 0;xl<cats.size();xl++) {
    if (!resval.isEmpty()) resval += ", ";
    resval += cats[xl]->GetName();
  }
  return resval;
}
//---------------------------------
bool jfAO3Fanfic::MultipleFandoms() const {
  return (cats.size()>1);
}
//---------------------------------
QString jfAO3Fanfic::GetFandom(size_t inindex) const {
  if (inindex>=cats.size()) return "";
  else return cats[inindex]->GetName();
}
//---------------------------------
size_t jfAO3Fanfic::FandomCount() const {
  return (cats.size());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special url construction
QString jfAO3Fanfic::MakeAuthorUrl() const { return  author_url; }
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfAO3Fanfic::LoadMoreValues2(jfSkeletonParser* inparser) const {
    // checking
    assert(inparser!=NULL);
    QString rtval;
    // starting
    LoadRelationships(inparser, false);
    // rating
    inparser->AddText("ITEMF_RATING",RatingToString());
    // orientation
    inparser->AddText("ITEMF_ORIENT",GetOrientations());
    // warnings
    inparser->AddText("ITEMF_WARNING",WarnToString());
    // kudos
    inparser->AddUInt("ITEMF_KUDOS",kudcount);
    // the strings
    inparser->AddText("ITEMF_EXTRATAGS",GetJoinedExtraTags());
    inparser->AddText("ITEMF_FANDOMS",GetFandoms());
    // series stuff
    inparser->AddBool("ITEMF_INSERIES",InSeries());
    inparser->AddUInt("ITEMF_SERIESINDEX",series_index);
    inparser->AddText("ITEMF_SERIESNAME",series_name);
    inparser->AddText("ITEMF_SERIESURL",series_url);
    // done
    return true;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal to string helper methods
//----------------------------
QString jfAO3Fanfic::RatingToString() const {
    if (rating=='G') return "General";
    else if (rating=='T') return "Teen";
    else if (rating=='M') return "Mature";
    else if (rating=='E') return "Explicit";
    else if (rating=='_') return "Unspecified";
    else assert(false);
    return "";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfAO3Fanfic::~jfAO3Fanfic() {
    ClearPairingVector(char_pairs);
    cats.clear();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QString jfAO3Fanfic::WarnToString() const {
    // variables
    QString resval;
    // testing
    if (warn=='N') return "Creator Chose Not To Use Archive Warnings";
    else if (warn=='_') return "No Archive Warnings Apply";
    else if (warn=='E') return "External Site";
    else if (warn=='W') {
        if (TestWarntag('V')) resval = "Violence";
        if (TestWarntag('D')) {
            if (!resval.isEmpty()) resval+= ", ";
            resval += "Character Death";
        }
        if (TestWarntag('R')) {
            if (!resval.isEmpty()) resval+= ", ";
            resval += "Rape";
        }
        if (TestWarntag('P')) {
            if (!resval.isEmpty()) resval+= ", ";
            resval += "Underage Sex";
        }
        // done
        return resval;
    }
    else assert(false);
    return "";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
//----------------------------
jfFicExtract* jfAO3Fanfic::MakeExtract() const {
  jfFicExtract_AO3* result;
  result = new jfFicExtract_AO3();
  LoadIntoExtract(result);
  return result;
}
//----------------------------
void jfAO3Fanfic::LoadIntoExtract(jfFicExtract* into) const {
  // loading basic stuff
  LoadIntoExtractCore(into);
  into->complete = completed;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//----------------------------
bool jfAO3Fanfic::AddExtraStuff(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  QStringList* cfinders;
  size_t chloop;
  // checking and special conditions
  if (outfile==NULL) return false;

  // long line 7
  xresult << series_index << series_name << series_url;
  (*outfile) << xresult << "\n";
  xresult.FullClear();

  // chapter finders (line 8)
  cfinders = new QStringList();
  for (chloop=0;chloop<cats.size();chloop++) {
    cfinders->append(cats[chloop]->GetCombo());
  }
  xresult << StrArrToList(cfinders,',');
  (*outfile) << xresult << "\n";

  // done
  return AddExtraStuff(outfile);
}
//----------------------------
bool jfAO3Fanfic::ReadExtraStuff(jfFileReader* infile) {
  // constants and variables
  const QString funcname = "jfAO3Fanfic::ReadRestFromFile";
  QString buffer;
  QStringList* clist;

  assert(infile!=NULL);

  // reading line 7
  if (!infile->ReadParseLine(3,funcname)) return false;
  if (!infile->lp.SIntVal(0,series_index)) return infile->BuildError("The series index is wrong!");
  series_name = infile->lp.UnEscStr(1);
  series_url = infile->lp.UnEscStr(2);

  // getting chapters (line 8)
  if (!infile->ReadUnEsc(buffer,funcname)) return infile->BuildError("Category list not found!");
  clist = ListToStrArr(buffer,',');
  // first error handling
  if (clist==NULL) return infile->BuildError("The categories are empty!");
  if (clist->isEmpty()) {
    delete clist;
    return infile->BuildError("The categories are empty!");
  }
  // chapter finders
  const jfGeneralCategory* temp = NULL;

  for (int cloop = 0; cloop < (clist->count()) ; cloop++) {
    temp = ao3_catdata::ao3_catmanager->GetData()->FindCatFromFinder(clist->at(cloop));
    if (temp==NULL) break;
    cats.push_back(dynamic_cast<const jfAO3_Category*>(temp));
  }
  // cheking the results
  delete clist;
  if (temp==NULL) return infile->BuildError("Unknown category!");

  // finally
  return ReadExtraStuff(infile);
}
//----------------------------
bool jfAO3Fanfic::AddMoreExtraStuff(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 9 will be relationships
  (*outfile) << RelationshipsAsStorageString() << "\n";
  // preparing line 10
  xresult << rating << orientations << warn << warntags << eccount << kudcount;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // doing line 11
  xresult << extratags;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // done
  return true;
}
//----------------------------
bool jfAO3Fanfic::ReadMoreExtraStuff(jfFileReader* infile) {
    // constants and variables
  const QString funcname = "jfAO3Fanfic::ReadExtraStuff";
  QString buffer;
  QRegExp qcheck = QRegExp("[^" + ao3con::warn2_ac + "]");
  // the line with characters
  assert(infile!=NULL);

  // line 9 is relationships
  if (!infile->ReadLine(buffer, funcname)) return false;
  if (!SetRelationshipsFromString(buffer, false)) return infile->BuildError("Could not parse relationships!");

  // reading line 10
  if (!infile->ReadParseLine(6,funcname)) return false;
  // testing special chars
  if (!infile->lp.CharVal(0,rating,ao3con::rating_ac)) return infile->BuildError("Rating is invalid!");
  orientations = infile->lp.UnEscStr(1);
  if (!infile->lp.CharVal(2,warn,ao3con::warn1_ac)) return infile->BuildError("Warn Flag is invalid!");
  buffer = infile->lp.UnEscStr(3);
  if (buffer.contains(qcheck)) {
    return infile->BuildError("Warning tags not correct!");
  }
  if (!infile->lp.IIntVal(4,eccount)) return infile->BuildError("Estimated chaper count is not a number!");
  if (!infile->lp.SIntVal(5,kudcount)) return infile->BuildError("The amount of kudos is invalid!");

  // line 11
  QString joined_extra_tags;
  if (!infile->ReadUnEsc(joined_extra_tags)) return infile->BuildError("Could not get tags!");
  extratags = joined_extra_tags.split(',');
  // finally
  return true;
}
//*******************************************************************************
